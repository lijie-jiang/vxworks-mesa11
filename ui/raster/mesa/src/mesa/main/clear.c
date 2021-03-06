/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 1999-2007  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */


/**
 * \file clear.c
 * glClearColor, glClearIndex, glClear() functions.
 */



#include "glheader.h"
#include "clear.h"
#include "context.h"
#include "enums.h"
#include "fbobject.h"
#include "get.h"
#include "macros.h"
#include "mtypes.h"
#include "state.h"



void GLAPIENTRY
_mesa_ClearIndex( GLfloat c )
{
   GET_CURRENT_CONTEXT(ctx);

   ctx->Color.ClearIndex = (GLuint) c;
}


/**
 * Specify the clear values for the color buffers.
 *
 * \param red red color component.
 * \param green green color component.
 * \param blue blue color component.
 * \param alpha alpha component.
 *
 * \sa glClearColor().
 */
void GLAPIENTRY
_mesa_ClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
   GET_CURRENT_CONTEXT(ctx);

   ctx->Color.ClearColor.f[0] = red;
   ctx->Color.ClearColor.f[1] = green;
   ctx->Color.ClearColor.f[2] = blue;
   ctx->Color.ClearColor.f[3] = alpha;
}


/**
 * GL_EXT_texture_integer
 */
void GLAPIENTRY
_mesa_ClearColorIiEXT(GLint r, GLint g, GLint b, GLint a)
{
   GET_CURRENT_CONTEXT(ctx);

   ctx->Color.ClearColor.i[0] = r;
   ctx->Color.ClearColor.i[1] = g;
   ctx->Color.ClearColor.i[2] = b;
   ctx->Color.ClearColor.i[3] = a;
}


/**
 * GL_EXT_texture_integer
 */
void GLAPIENTRY
_mesa_ClearColorIuiEXT(GLuint r, GLuint g, GLuint b, GLuint a)
{
   GET_CURRENT_CONTEXT(ctx);

   ctx->Color.ClearColor.ui[0] = r;
   ctx->Color.ClearColor.ui[1] = g;
   ctx->Color.ClearColor.ui[2] = b;
   ctx->Color.ClearColor.ui[3] = a;
}


/**
 * Returns true if color writes are enabled for the given color attachment.
 *
 * Beyond checking ColorMask, this uses _mesa_format_has_color_component to
 * ignore components that don't actually exist in the format (such as X in
 * XRGB).
 */
static bool
color_buffer_writes_enabled(const struct gl_context *ctx, unsigned idx)
{
   struct gl_renderbuffer *rb = ctx->DrawBuffer->_ColorDrawBuffers[idx];
   GLuint c;
   GLubyte colorMask = 0;

   if (rb) {
      for (c = 0; c < 4; c++) {
         if (_mesa_format_has_color_component(rb->Format, c))
            colorMask |= ctx->Color.ColorMask[idx][c];
      }
   }

   return colorMask != 0;
}


/**
 * Clear buffers.
 *
 * \param mask bit-mask indicating the buffers to be cleared.
 *
 * Flushes the vertices and verifies the parameter.
 * If __struct gl_contextRec::NewState is set then calls _mesa_update_state()
 * to update gl_frame_buffer::_Xmin, etc.  If the rasterization mode is set to
 * GL_RENDER then requests the driver to clear the buffers, via the
 * dd_function_table::Clear callback.
 */
void GLAPIENTRY
_mesa_Clear( GLbitfield mask )
{
   GET_CURRENT_CONTEXT(ctx);
   FLUSH_VERTICES(ctx, 0);

   FLUSH_CURRENT(ctx, 0);

   if (MESA_VERBOSE & VERBOSE_API)
      _mesa_debug(ctx, "glClear 0x%x\n", mask);

   if (mask & ~(GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT |
                GL_ACCUM_BUFFER_BIT)) {
      /* invalid bit set */
      _mesa_error( ctx, GL_INVALID_VALUE, "glClear(0x%x)", mask);
      return;
   }

   /* Accumulation buffers were removed in core contexts, and they never
    * existed in OpenGL ES.
    */
   if ((mask & GL_ACCUM_BUFFER_BIT) != 0
       && (ctx->API == API_OPENGL_CORE || _mesa_is_gles(ctx))) {
      _mesa_error( ctx, GL_INVALID_VALUE, "glClear(GL_ACCUM_BUFFER_BIT)");
      return;
   }

   if (ctx->NewState) {
      _mesa_update_state( ctx );	/* update _Xmin, etc */
   }

   if (ctx->DrawBuffer->_Status != GL_FRAMEBUFFER_COMPLETE_EXT) {
      _mesa_error(ctx, GL_INVALID_FRAMEBUFFER_OPERATION_EXT,
                  "glClear(incomplete framebuffer)");
      return;
   }

   if (ctx->RasterDiscard)
      return;

   if (ctx->RenderMode == GL_RENDER) {
      GLbitfield bufferMask;

      /* don't clear depth buffer if depth writing disabled */
      if (!ctx->Depth.Mask)
         mask &= ~GL_DEPTH_BUFFER_BIT;

      /* Build the bitmask to send to device driver's Clear function.
       * Note that the GL_COLOR_BUFFER_BIT flag will expand to 0, 1, 2 or 4
       * of the BUFFER_BIT_FRONT/BACK_LEFT/RIGHT flags, or one of the
       * BUFFER_BIT_COLORn flags.
       */
      bufferMask = 0;
      if (mask & GL_COLOR_BUFFER_BIT) {
         GLuint i;
         for (i = 0; i < ctx->DrawBuffer->_NumColorDrawBuffers; i++) {
            GLint buf = ctx->DrawBuffer->_ColorDrawBufferIndexes[i];

            if (buf >= 0 && color_buffer_writes_enabled(ctx, i)) {
               bufferMask |= 1 << buf;
            }
         }
      }

      if ((mask & GL_DEPTH_BUFFER_BIT)
          && ctx->DrawBuffer->Visual.haveDepthBuffer) {
         bufferMask |= BUFFER_BIT_DEPTH;
      }

      if ((mask & GL_STENCIL_BUFFER_BIT)
          && ctx->DrawBuffer->Visual.haveStencilBuffer) {
         bufferMask |= BUFFER_BIT_STENCIL;
      }

      if ((mask & GL_ACCUM_BUFFER_BIT)
          && ctx->DrawBuffer->Visual.haveAccumBuffer) {
         bufferMask |= BUFFER_BIT_ACCUM;
      }

      assert(ctx->Driver.Clear);
      ctx->Driver.Clear(ctx, bufferMask);
   }
}


/** Returned by make_color_buffer_mask() for errors */
#define INVALID_MASK ~0x0U


/**
 * Convert the glClearBuffer 'drawbuffer' parameter into a bitmask of
 * BUFFER_BIT_x values.
 * Return INVALID_MASK if the drawbuffer value is invalid.
 */
static GLbitfield
make_color_buffer_mask(struct gl_context *ctx, GLint drawbuffer)
{
   const struct gl_renderbuffer_attachment *att = ctx->DrawBuffer->Attachment;
   GLbitfield mask = 0x0;

   /* From the GL 4.0 specification:
    *	If buffer is COLOR, a particular draw buffer DRAW_BUFFERi is
    *	specified by passing i as the parameter drawbuffer, and value
    *	points to a four-element vector specifying the R, G, B, and A
    *	color to clear that draw buffer to. If the draw buffer is one
    *	of FRONT, BACK, LEFT, RIGHT, or FRONT_AND_BACK, identifying
    *	multiple buffers, each selected buffer is cleared to the same
    *	value.
    *
    * Note that "drawbuffer" and "draw buffer" have different meaning.
    * "drawbuffer" specifies DRAW_BUFFERi, while "draw buffer" is what's
    * assigned to DRAW_BUFFERi. It could be COLOR_ATTACHMENT0, FRONT, BACK,
    * etc.
    */
   if (drawbuffer < 0 || drawbuffer >= (GLint)ctx->Const.MaxDrawBuffers) {
      return INVALID_MASK;
   }

   switch (ctx->DrawBuffer->ColorDrawBuffer[drawbuffer]) {
   case GL_FRONT:
      if (att[BUFFER_FRONT_LEFT].Renderbuffer)
         mask |= BUFFER_BIT_FRONT_LEFT;
      if (att[BUFFER_FRONT_RIGHT].Renderbuffer)
         mask |= BUFFER_BIT_FRONT_RIGHT;
      break;
   case GL_BACK:
      if (att[BUFFER_BACK_LEFT].Renderbuffer)
         mask |= BUFFER_BIT_BACK_LEFT;
      if (att[BUFFER_BACK_RIGHT].Renderbuffer)
         mask |= BUFFER_BIT_BACK_RIGHT;
      break;
   case GL_LEFT:
      if (att[BUFFER_FRONT_LEFT].Renderbuffer)
         mask |= BUFFER_BIT_FRONT_LEFT;
      if (att[BUFFER_BACK_LEFT].Renderbuffer)
         mask |= BUFFER_BIT_BACK_LEFT;
      break;
   case GL_RIGHT:
      if (att[BUFFER_FRONT_RIGHT].Renderbuffer)
         mask |= BUFFER_BIT_FRONT_RIGHT;
      if (att[BUFFER_BACK_RIGHT].Renderbuffer)
         mask |= BUFFER_BIT_BACK_RIGHT;
      break;
   case GL_FRONT_AND_BACK:
      if (att[BUFFER_FRONT_LEFT].Renderbuffer)
         mask |= BUFFER_BIT_FRONT_LEFT;
      if (att[BUFFER_BACK_LEFT].Renderbuffer)
         mask |= BUFFER_BIT_BACK_LEFT;
      if (att[BUFFER_FRONT_RIGHT].Renderbuffer)
         mask |= BUFFER_BIT_FRONT_RIGHT;
      if (att[BUFFER_BACK_RIGHT].Renderbuffer)
         mask |= BUFFER_BIT_BACK_RIGHT;
      break;
   default:
      {
         GLint buf = ctx->DrawBuffer->_ColorDrawBufferIndexes[drawbuffer];

         if (buf >= 0 && att[buf].Renderbuffer) {
            mask |= 1 << buf;
         }
      }
   }

   return mask;
}



/**
 * New in GL 3.0
 * Clear signed integer color buffer or stencil buffer (not depth).
 */
void GLAPIENTRY
_mesa_ClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint *value)
{
   GET_CURRENT_CONTEXT(ctx);
   FLUSH_VERTICES(ctx, 0);

   FLUSH_CURRENT(ctx, 0);

   if (ctx->NewState) {
      _mesa_update_state( ctx );
   }

   /* Page 498 of the PDF, section '17.4.3.1 Clearing Individual Buffers'
    * of the OpenGL 4.5 spec states:
    *
    *    "An INVALID_ENUM error is generated by ClearBufferiv and
    *     ClearNamedFramebufferiv if buffer is not COLOR or STENCIL."
    */
   if (buffer == GL_DEPTH || buffer == GL_DEPTH_STENCIL) {
      _mesa_error(ctx, GL_INVALID_ENUM,
                  "glClearBufferiv(buffer=GL_DEPTH || GL_DEPTH_STENCIL)");
      return;
   }

   switch (buffer) {
   case GL_STENCIL:
      /* Page 264 (page 280 of the PDF) of the OpenGL 3.0 spec says:
       *
       *     "ClearBuffer generates an INVALID VALUE error if buffer is
       *     COLOR and drawbuffer is less than zero, or greater than the
       *     value of MAX DRAW BUFFERS minus one; or if buffer is DEPTH,
       *     STENCIL, or DEPTH STENCIL and drawbuffer is not zero."
       */
      if (drawbuffer != 0) {
         _mesa_error(ctx, GL_INVALID_VALUE, "glClearBufferiv(drawbuffer=%d)",
                     drawbuffer);
         return;
      }
      else if (ctx->DrawBuffer->Attachment[BUFFER_STENCIL].Renderbuffer
               && !ctx->RasterDiscard) {
         /* Save current stencil clear value, set to 'value', do the
          * stencil clear and restore the clear value.
          * XXX in the future we may have a new ctx->Driver.ClearBuffer()
          * hook instead.
          */
         const GLuint clearSave = ctx->Stencil.Clear;
         ctx->Stencil.Clear = *value;
         ctx->Driver.Clear(ctx, BUFFER_BIT_STENCIL);
         ctx->Stencil.Clear = clearSave;
      }
      break;
   case GL_COLOR:
      {
         const GLbitfield mask = make_color_buffer_mask(ctx, drawbuffer);
         if (mask == INVALID_MASK) {
            _mesa_error(ctx, GL_INVALID_VALUE, "glClearBufferiv(drawbuffer=%d)",
                        drawbuffer);
            return;
         }
         else if (mask && !ctx->RasterDiscard) {
            union gl_color_union clearSave;

            /* save color */
            clearSave = ctx->Color.ClearColor;
            /* set color */
            COPY_4V(ctx->Color.ClearColor.i, value);
            /* clear buffer(s) */
            ctx->Driver.Clear(ctx, mask);
            /* restore color */
            ctx->Color.ClearColor = clearSave;
         }
      }
      break;
   case GL_DEPTH:
      /* Page 264 (page 280 of the PDF) of the OpenGL 3.0 spec says:
       *
       *     "The result of ClearBuffer is undefined if no conversion between
       *     the type of the specified value and the type of the buffer being
       *     cleared is defined (for example, if ClearBufferiv is called for a
       *     fixed- or floating-point buffer, or if ClearBufferfv is called
       *     for a signed or unsigned integer buffer). This is not an error."
       *
       * In this case we take "undefined" and "not an error" to mean "ignore."
       * Note that we still need to generate an error for the invalid
       * drawbuffer case (see the GL_STENCIL case above).
       */
      if (drawbuffer != 0) {
         _mesa_error(ctx, GL_INVALID_VALUE, "glClearBufferiv(drawbuffer=%d)",
                     drawbuffer);
         return;
      }
      return;
   default:
      _mesa_error(ctx, GL_INVALID_ENUM, "glClearBufferiv(buffer=%s)",
                  _mesa_enum_to_string(buffer));
      return;
   }
}


/**
 * The ClearBuffer framework is so complicated and so riddled with the
 * assumption that the framebuffer is bound that, for now, we will just fake
 * direct state access clearing for the user.
 */
void GLAPIENTRY
_mesa_ClearNamedFramebufferiv(GLuint framebuffer, GLenum buffer,
                              GLint drawbuffer, const GLint *value)
{
   GLint oldfb;

   _mesa_GetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldfb);
   _mesa_BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
   _mesa_ClearBufferiv(buffer, drawbuffer, value);
   _mesa_BindFramebuffer(GL_DRAW_FRAMEBUFFER, (GLuint) oldfb);
}


/**
 * New in GL 3.0
 * Clear unsigned integer color buffer (not depth, not stencil).
 */
void GLAPIENTRY
_mesa_ClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint *value)
{
   GET_CURRENT_CONTEXT(ctx);

   FLUSH_VERTICES(ctx, 0);
   FLUSH_CURRENT(ctx, 0);

   if (ctx->NewState) {
      _mesa_update_state( ctx );
   }

   switch (buffer) {
   case GL_COLOR:
      {
         const GLbitfield mask = make_color_buffer_mask(ctx, drawbuffer);
         if (mask == INVALID_MASK) {
            _mesa_error(ctx, GL_INVALID_VALUE, "glClearBufferuiv(drawbuffer=%d)",
                        drawbuffer);
            return;
         }
         else if (mask && !ctx->RasterDiscard) {
            union gl_color_union clearSave;

            /* save color */
            clearSave = ctx->Color.ClearColor;
            /* set color */
            COPY_4V(ctx->Color.ClearColor.ui, value);
            /* clear buffer(s) */
            ctx->Driver.Clear(ctx, mask);
            /* restore color */
            ctx->Color.ClearColor = clearSave;
         }
      }
      break;
   case GL_DEPTH:
   case GL_STENCIL:
      /* Page 264 (page 280 of the PDF) of the OpenGL 3.0 spec says:
       *
       *     "The result of ClearBuffer is undefined if no conversion between
       *     the type of the specified value and the type of the buffer being
       *     cleared is defined (for example, if ClearBufferiv is called for a
       *     fixed- or floating-point buffer, or if ClearBufferfv is called
       *     for a signed or unsigned integer buffer). This is not an error."
       *
       * In this case we take "undefined" and "not an error" to mean "ignore."
       * Even though we could do something sensible for GL_STENCIL, page 263
       * (page 279 of the PDF) says:
       *
       *     "Only ClearBufferiv should be used to clear stencil buffers."
       *
       * Note that we still need to generate an error for the invalid
       * drawbuffer case (see the GL_STENCIL case in _mesa_ClearBufferiv).
       */
      if (drawbuffer != 0) {
         _mesa_error(ctx, GL_INVALID_VALUE, "glClearBufferuiv(drawbuffer=%d)",
                     drawbuffer);
         return;
      }
      return;
   default:
      _mesa_error(ctx, GL_INVALID_ENUM, "glClearBufferuiv(buffer=%s)",
                  _mesa_enum_to_string(buffer));
      return;
   }
}


/**
 * The ClearBuffer framework is so complicated and so riddled with the
 * assumption that the framebuffer is bound that, for now, we will just fake
 * direct state access clearing for the user.
 */
void GLAPIENTRY
_mesa_ClearNamedFramebufferuiv(GLuint framebuffer, GLenum buffer,
                               GLint drawbuffer, const GLuint *value)
{
   GLint oldfb;

   _mesa_GetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldfb);
   _mesa_BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
   _mesa_ClearBufferuiv(buffer, drawbuffer, value);
   _mesa_BindFramebuffer(GL_DRAW_FRAMEBUFFER, (GLuint) oldfb);
}


/**
 * New in GL 3.0
 * Clear fixed-pt or float color buffer or depth buffer (not stencil).
 */
void GLAPIENTRY
_mesa_ClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value)
{
   GET_CURRENT_CONTEXT(ctx);

   FLUSH_VERTICES(ctx, 0);
   FLUSH_CURRENT(ctx, 0);

   if (ctx->NewState) {
      _mesa_update_state( ctx );
   }

   switch (buffer) {
   case GL_DEPTH:
      /* Page 264 (page 280 of the PDF) of the OpenGL 3.0 spec says:
       *
       *     "ClearBuffer generates an INVALID VALUE error if buffer is
       *     COLOR and drawbuffer is less than zero, or greater than the
       *     value of MAX DRAW BUFFERS minus one; or if buffer is DEPTH,
       *     STENCIL, or DEPTH STENCIL and drawbuffer is not zero."
       */
      if (drawbuffer != 0) {
         _mesa_error(ctx, GL_INVALID_VALUE, "glClearBufferfv(drawbuffer=%d)",
                     drawbuffer);
         return;
      }
      else if (ctx->DrawBuffer->Attachment[BUFFER_DEPTH].Renderbuffer
               && !ctx->RasterDiscard) {
         /* Save current depth clear value, set to 'value', do the
          * depth clear and restore the clear value.
          * XXX in the future we may have a new ctx->Driver.ClearBuffer()
          * hook instead.
          */
         const GLclampd clearSave = ctx->Depth.Clear;
         ctx->Depth.Clear = *value;
         ctx->Driver.Clear(ctx, BUFFER_BIT_DEPTH);
         ctx->Depth.Clear = clearSave;
      }
      /* clear depth buffer to value */
      break;
   case GL_COLOR:
      {
         const GLbitfield mask = make_color_buffer_mask(ctx, drawbuffer);
         if (mask == INVALID_MASK) {
            _mesa_error(ctx, GL_INVALID_VALUE, "glClearBufferfv(drawbuffer=%d)",
                        drawbuffer);
            return;
         }
         else if (mask && !ctx->RasterDiscard) {
            union gl_color_union clearSave;

            /* save color */
            clearSave = ctx->Color.ClearColor;
            /* set color */
            COPY_4V(ctx->Color.ClearColor.f, value);
            /* clear buffer(s) */
            ctx->Driver.Clear(ctx, mask);
            /* restore color */
            ctx->Color.ClearColor = clearSave;
         }
      }
      break;
   case GL_STENCIL:
      /* Page 264 (page 280 of the PDF) of the OpenGL 3.0 spec says:
       *
       *     "The result of ClearBuffer is undefined if no conversion between
       *     the type of the specified value and the type of the buffer being
       *     cleared is defined (for example, if ClearBufferiv is called for a
       *     fixed- or floating-point buffer, or if ClearBufferfv is called
       *     for a signed or unsigned integer buffer). This is not an error."
       *
       * In this case we take "undefined" and "not an error" to mean "ignore."
       * Note that we still need to generate an error for the invalid
       * drawbuffer case (see the GL_DEPTH case above).
       */
      if (drawbuffer != 0) {
         _mesa_error(ctx, GL_INVALID_VALUE, "glClearBufferfv(drawbuffer=%d)",
                     drawbuffer);
         return;
      }
      return;
   default:
      _mesa_error(ctx, GL_INVALID_ENUM, "glClearBufferfv(buffer=%s)",
                  _mesa_enum_to_string(buffer));
      return;
   }
}


/**
 * The ClearBuffer framework is so complicated and so riddled with the
 * assumption that the framebuffer is bound that, for now, we will just fake
 * direct state access clearing for the user.
 */
void GLAPIENTRY
_mesa_ClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer,
                              GLint drawbuffer, const GLfloat *value)
{
   GLint oldfb;

   _mesa_GetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldfb);
   _mesa_BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
   _mesa_ClearBufferfv(buffer, drawbuffer, value);
   _mesa_BindFramebuffer(GL_DRAW_FRAMEBUFFER, (GLuint) oldfb);
}


/**
 * New in GL 3.0
 * Clear depth/stencil buffer only.
 */
void GLAPIENTRY
_mesa_ClearBufferfi(GLenum buffer, GLint drawbuffer,
                    GLfloat depth, GLint stencil)
{
   GET_CURRENT_CONTEXT(ctx);
   GLbitfield mask = 0;

   FLUSH_VERTICES(ctx, 0);
   FLUSH_CURRENT(ctx, 0);

   if (buffer != GL_DEPTH_STENCIL) {
      _mesa_error(ctx, GL_INVALID_ENUM, "glClearBufferfi(buffer=%s)",
                  _mesa_enum_to_string(buffer));
      return;
   }

   /* Page 264 (page 280 of the PDF) of the OpenGL 3.0 spec says:
    *
    *     "ClearBuffer generates an INVALID VALUE error if buffer is
    *     COLOR and drawbuffer is less than zero, or greater than the
    *     value of MAX DRAW BUFFERS minus one; or if buffer is DEPTH,
    *     STENCIL, or DEPTH STENCIL and drawbuffer is not zero."
    */
   if (drawbuffer != 0) {
      _mesa_error(ctx, GL_INVALID_VALUE, "glClearBufferfi(drawbuffer=%d)",
                  drawbuffer);
      return;
   }

   if (ctx->RasterDiscard)
      return;

   if (ctx->NewState) {
      _mesa_update_state( ctx );
   }

   if (ctx->DrawBuffer->Attachment[BUFFER_DEPTH].Renderbuffer)
      mask |= BUFFER_BIT_DEPTH;
   if (ctx->DrawBuffer->Attachment[BUFFER_STENCIL].Renderbuffer)
      mask |= BUFFER_BIT_STENCIL;

   if (mask) {
      /* save current clear values */
      const GLclampd clearDepthSave = ctx->Depth.Clear;
      const GLuint clearStencilSave = ctx->Stencil.Clear;

      /* set new clear values */
      ctx->Depth.Clear = depth;
      ctx->Stencil.Clear = stencil;

      /* clear buffers */
      ctx->Driver.Clear(ctx, mask);

      /* restore */
      ctx->Depth.Clear = clearDepthSave;
      ctx->Stencil.Clear = clearStencilSave;
   }
}


/**
 * The ClearBuffer framework is so complicated and so riddled with the
 * assumption that the framebuffer is bound that, for now, we will just fake
 * direct state access clearing for the user.
 */
void GLAPIENTRY
_mesa_ClearNamedFramebufferfi(GLuint framebuffer, GLenum buffer,
                              GLfloat depth, GLint stencil)
{
   GLint oldfb;

   _mesa_GetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldfb);
   _mesa_BindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
   _mesa_ClearBufferfi(buffer, 0, depth, stencil);
   _mesa_BindFramebuffer(GL_DRAW_FRAMEBUFFER, (GLuint) oldfb);
}
