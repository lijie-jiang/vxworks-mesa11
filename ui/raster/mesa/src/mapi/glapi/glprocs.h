/* DO NOT EDIT - This file generated automatically by gl_procs.py (from Mesa) script */

/*
 * Copyright (C) 1999-2001  Brian Paul   All Rights Reserved.
 * (C) Copyright IBM Corporation 2004, 2006
 * All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL, IBM,
 * AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


/* This file is only included by glapi.c and is used for
 * the GetProcAddress() function
 */

typedef struct {
    GLint Name_offset;
#if defined(NEED_FUNCTION_POINTER) || defined(GLX_INDIRECT_RENDERING)
    _glapi_proc Address;
#endif
    GLuint Offset;
} glprocs_table_t;

#if   !defined(NEED_FUNCTION_POINTER) && !defined(GLX_INDIRECT_RENDERING)
#  define NAME_FUNC_OFFSET(n,f1,f2,f3,o) { n , o }
#elif  defined(NEED_FUNCTION_POINTER) && !defined(GLX_INDIRECT_RENDERING)
#  define NAME_FUNC_OFFSET(n,f1,f2,f3,o) { n , (_glapi_proc) f1 , o }
#elif  defined(NEED_FUNCTION_POINTER) &&  defined(GLX_INDIRECT_RENDERING)
#  define NAME_FUNC_OFFSET(n,f1,f2,f3,o) { n , (_glapi_proc) f2 , o }
#elif !defined(NEED_FUNCTION_POINTER) &&  defined(GLX_INDIRECT_RENDERING)
#  define NAME_FUNC_OFFSET(n,f1,f2,f3,o) { n , (_glapi_proc) f3 , o }
#endif



static const char gl_string_table[] =
    "glNewList\0"
    "glEndList\0"
    "glCallList\0"
    "glCallLists\0"
    "glDeleteLists\0"
    "glGenLists\0"
    "glListBase\0"
    "glBegin\0"
    "glBitmap\0"
    "glColor3b\0"
    "glColor3bv\0"
    "glColor3d\0"
    "glColor3dv\0"
    "glColor3f\0"
    "glColor3fv\0"
    "glColor3i\0"
    "glColor3iv\0"
    "glColor3s\0"
    "glColor3sv\0"
    "glColor3ub\0"
    "glColor3ubv\0"
    "glColor3ui\0"
    "glColor3uiv\0"
    "glColor3us\0"
    "glColor3usv\0"
    "glColor4b\0"
    "glColor4bv\0"
    "glColor4d\0"
    "glColor4dv\0"
    "glColor4f\0"
    "glColor4fv\0"
    "glColor4i\0"
    "glColor4iv\0"
    "glColor4s\0"
    "glColor4sv\0"
    "glColor4ub\0"
    "glColor4ubv\0"
    "glColor4ui\0"
    "glColor4uiv\0"
    "glColor4us\0"
    "glColor4usv\0"
    "glEdgeFlag\0"
    "glEdgeFlagv\0"
    "glEnd\0"
    "glIndexd\0"
    "glIndexdv\0"
    "glIndexf\0"
    "glIndexfv\0"
    "glIndexi\0"
    "glIndexiv\0"
    "glIndexs\0"
    "glIndexsv\0"
    "glNormal3b\0"
    "glNormal3bv\0"
    "glNormal3d\0"
    "glNormal3dv\0"
    "glNormal3f\0"
    "glNormal3fv\0"
    "glNormal3i\0"
    "glNormal3iv\0"
    "glNormal3s\0"
    "glNormal3sv\0"
    "glRasterPos2d\0"
    "glRasterPos2dv\0"
    "glRasterPos2f\0"
    "glRasterPos2fv\0"
    "glRasterPos2i\0"
    "glRasterPos2iv\0"
    "glRasterPos2s\0"
    "glRasterPos2sv\0"
    "glRasterPos3d\0"
    "glRasterPos3dv\0"
    "glRasterPos3f\0"
    "glRasterPos3fv\0"
    "glRasterPos3i\0"
    "glRasterPos3iv\0"
    "glRasterPos3s\0"
    "glRasterPos3sv\0"
    "glRasterPos4d\0"
    "glRasterPos4dv\0"
    "glRasterPos4f\0"
    "glRasterPos4fv\0"
    "glRasterPos4i\0"
    "glRasterPos4iv\0"
    "glRasterPos4s\0"
    "glRasterPos4sv\0"
    "glRectd\0"
    "glRectdv\0"
    "glRectf\0"
    "glRectfv\0"
    "glRecti\0"
    "glRectiv\0"
    "glRects\0"
    "glRectsv\0"
    "glTexCoord1d\0"
    "glTexCoord1dv\0"
    "glTexCoord1f\0"
    "glTexCoord1fv\0"
    "glTexCoord1i\0"
    "glTexCoord1iv\0"
    "glTexCoord1s\0"
    "glTexCoord1sv\0"
    "glTexCoord2d\0"
    "glTexCoord2dv\0"
    "glTexCoord2f\0"
    "glTexCoord2fv\0"
    "glTexCoord2i\0"
    "glTexCoord2iv\0"
    "glTexCoord2s\0"
    "glTexCoord2sv\0"
    "glTexCoord3d\0"
    "glTexCoord3dv\0"
    "glTexCoord3f\0"
    "glTexCoord3fv\0"
    "glTexCoord3i\0"
    "glTexCoord3iv\0"
    "glTexCoord3s\0"
    "glTexCoord3sv\0"
    "glTexCoord4d\0"
    "glTexCoord4dv\0"
    "glTexCoord4f\0"
    "glTexCoord4fv\0"
    "glTexCoord4i\0"
    "glTexCoord4iv\0"
    "glTexCoord4s\0"
    "glTexCoord4sv\0"
    "glVertex2d\0"
    "glVertex2dv\0"
    "glVertex2f\0"
    "glVertex2fv\0"
    "glVertex2i\0"
    "glVertex2iv\0"
    "glVertex2s\0"
    "glVertex2sv\0"
    "glVertex3d\0"
    "glVertex3dv\0"
    "glVertex3f\0"
    "glVertex3fv\0"
    "glVertex3i\0"
    "glVertex3iv\0"
    "glVertex3s\0"
    "glVertex3sv\0"
    "glVertex4d\0"
    "glVertex4dv\0"
    "glVertex4f\0"
    "glVertex4fv\0"
    "glVertex4i\0"
    "glVertex4iv\0"
    "glVertex4s\0"
    "glVertex4sv\0"
    "glClipPlane\0"
    "glColorMaterial\0"
    "glCullFace\0"
    "glFogf\0"
    "glFogfv\0"
    "glFogi\0"
    "glFogiv\0"
    "glFrontFace\0"
    "glHint\0"
    "glLightf\0"
    "glLightfv\0"
    "glLighti\0"
    "glLightiv\0"
    "glLightModelf\0"
    "glLightModelfv\0"
    "glLightModeli\0"
    "glLightModeliv\0"
    "glLineStipple\0"
    "glLineWidth\0"
    "glMaterialf\0"
    "glMaterialfv\0"
    "glMateriali\0"
    "glMaterialiv\0"
    "glPointSize\0"
    "glPolygonMode\0"
    "glPolygonStipple\0"
    "glScissor\0"
    "glShadeModel\0"
    "glTexParameterf\0"
    "glTexParameterfv\0"
    "glTexParameteri\0"
    "glTexParameteriv\0"
    "glTexImage1D\0"
    "glTexImage2D\0"
    "glTexEnvf\0"
    "glTexEnvfv\0"
    "glTexEnvi\0"
    "glTexEnviv\0"
    "glTexGend\0"
    "glTexGendv\0"
    "glTexGenf\0"
    "glTexGenfv\0"
    "glTexGeni\0"
    "glTexGeniv\0"
    "glFeedbackBuffer\0"
    "glSelectBuffer\0"
    "glRenderMode\0"
    "glInitNames\0"
    "glLoadName\0"
    "glPassThrough\0"
    "glPopName\0"
    "glPushName\0"
    "glDrawBuffer\0"
    "glClear\0"
    "glClearAccum\0"
    "glClearIndex\0"
    "glClearColor\0"
    "glClearStencil\0"
    "glClearDepth\0"
    "glStencilMask\0"
    "glColorMask\0"
    "glDepthMask\0"
    "glIndexMask\0"
    "glAccum\0"
    "glDisable\0"
    "glEnable\0"
    "glFinish\0"
    "glFlush\0"
    "glPopAttrib\0"
    "glPushAttrib\0"
    "glMap1d\0"
    "glMap1f\0"
    "glMap2d\0"
    "glMap2f\0"
    "glMapGrid1d\0"
    "glMapGrid1f\0"
    "glMapGrid2d\0"
    "glMapGrid2f\0"
    "glEvalCoord1d\0"
    "glEvalCoord1dv\0"
    "glEvalCoord1f\0"
    "glEvalCoord1fv\0"
    "glEvalCoord2d\0"
    "glEvalCoord2dv\0"
    "glEvalCoord2f\0"
    "glEvalCoord2fv\0"
    "glEvalMesh1\0"
    "glEvalPoint1\0"
    "glEvalMesh2\0"
    "glEvalPoint2\0"
    "glAlphaFunc\0"
    "glBlendFunc\0"
    "glLogicOp\0"
    "glStencilFunc\0"
    "glStencilOp\0"
    "glDepthFunc\0"
    "glPixelZoom\0"
    "glPixelTransferf\0"
    "glPixelTransferi\0"
    "glPixelStoref\0"
    "glPixelStorei\0"
    "glPixelMapfv\0"
    "glPixelMapuiv\0"
    "glPixelMapusv\0"
    "glReadBuffer\0"
    "glCopyPixels\0"
    "glReadPixels\0"
    "glDrawPixels\0"
    "glGetBooleanv\0"
    "glGetClipPlane\0"
    "glGetDoublev\0"
    "glGetError\0"
    "glGetFloatv\0"
    "glGetIntegerv\0"
    "glGetLightfv\0"
    "glGetLightiv\0"
    "glGetMapdv\0"
    "glGetMapfv\0"
    "glGetMapiv\0"
    "glGetMaterialfv\0"
    "glGetMaterialiv\0"
    "glGetPixelMapfv\0"
    "glGetPixelMapuiv\0"
    "glGetPixelMapusv\0"
    "glGetPolygonStipple\0"
    "glGetString\0"
    "glGetTexEnvfv\0"
    "glGetTexEnviv\0"
    "glGetTexGendv\0"
    "glGetTexGenfv\0"
    "glGetTexGeniv\0"
    "glGetTexImage\0"
    "glGetTexParameterfv\0"
    "glGetTexParameteriv\0"
    "glGetTexLevelParameterfv\0"
    "glGetTexLevelParameteriv\0"
    "glIsEnabled\0"
    "glIsList\0"
    "glDepthRange\0"
    "glFrustum\0"
    "glLoadIdentity\0"
    "glLoadMatrixf\0"
    "glLoadMatrixd\0"
    "glMatrixMode\0"
    "glMultMatrixf\0"
    "glMultMatrixd\0"
    "glOrtho\0"
    "glPopMatrix\0"
    "glPushMatrix\0"
    "glRotated\0"
    "glRotatef\0"
    "glScaled\0"
    "glScalef\0"
    "glTranslated\0"
    "glTranslatef\0"
    "glViewport\0"
    "glArrayElement\0"
    "glBindTexture\0"
    "glColorPointer\0"
    "glDisableClientState\0"
    "glDrawArrays\0"
    "glDrawElements\0"
    "glEdgeFlagPointer\0"
    "glEnableClientState\0"
    "glIndexPointer\0"
    "glIndexub\0"
    "glIndexubv\0"
    "glInterleavedArrays\0"
    "glNormalPointer\0"
    "glPolygonOffset\0"
    "glTexCoordPointer\0"
    "glVertexPointer\0"
    "glAreTexturesResident\0"
    "glCopyTexImage1D\0"
    "glCopyTexImage2D\0"
    "glCopyTexSubImage1D\0"
    "glCopyTexSubImage2D\0"
    "glDeleteTextures\0"
    "glGenTextures\0"
    "glGetPointerv\0"
    "glIsTexture\0"
    "glPrioritizeTextures\0"
    "glTexSubImage1D\0"
    "glTexSubImage2D\0"
    "glPopClientAttrib\0"
    "glPushClientAttrib\0"
    "glBlendColor\0"
    "glBlendEquation\0"
    "glDrawRangeElements\0"
    "glColorTable\0"
    "glColorTableParameterfv\0"
    "glColorTableParameteriv\0"
    "glCopyColorTable\0"
    "glGetColorTable\0"
    "glGetColorTableParameterfv\0"
    "glGetColorTableParameteriv\0"
    "glColorSubTable\0"
    "glCopyColorSubTable\0"
    "glConvolutionFilter1D\0"
    "glConvolutionFilter2D\0"
    "glConvolutionParameterf\0"
    "glConvolutionParameterfv\0"
    "glConvolutionParameteri\0"
    "glConvolutionParameteriv\0"
    "glCopyConvolutionFilter1D\0"
    "glCopyConvolutionFilter2D\0"
    "glGetConvolutionFilter\0"
    "glGetConvolutionParameterfv\0"
    "glGetConvolutionParameteriv\0"
    "glGetSeparableFilter\0"
    "glSeparableFilter2D\0"
    "glGetHistogram\0"
    "glGetHistogramParameterfv\0"
    "glGetHistogramParameteriv\0"
    "glGetMinmax\0"
    "glGetMinmaxParameterfv\0"
    "glGetMinmaxParameteriv\0"
    "glHistogram\0"
    "glMinmax\0"
    "glResetHistogram\0"
    "glResetMinmax\0"
    "glTexImage3D\0"
    "glTexSubImage3D\0"
    "glCopyTexSubImage3D\0"
    "glActiveTexture\0"
    "glClientActiveTexture\0"
    "glMultiTexCoord1d\0"
    "glMultiTexCoord1dv\0"
    "glMultiTexCoord1fARB\0"
    "glMultiTexCoord1fvARB\0"
    "glMultiTexCoord1i\0"
    "glMultiTexCoord1iv\0"
    "glMultiTexCoord1s\0"
    "glMultiTexCoord1sv\0"
    "glMultiTexCoord2d\0"
    "glMultiTexCoord2dv\0"
    "glMultiTexCoord2fARB\0"
    "glMultiTexCoord2fvARB\0"
    "glMultiTexCoord2i\0"
    "glMultiTexCoord2iv\0"
    "glMultiTexCoord2s\0"
    "glMultiTexCoord2sv\0"
    "glMultiTexCoord3d\0"
    "glMultiTexCoord3dv\0"
    "glMultiTexCoord3fARB\0"
    "glMultiTexCoord3fvARB\0"
    "glMultiTexCoord3i\0"
    "glMultiTexCoord3iv\0"
    "glMultiTexCoord3s\0"
    "glMultiTexCoord3sv\0"
    "glMultiTexCoord4d\0"
    "glMultiTexCoord4dv\0"
    "glMultiTexCoord4fARB\0"
    "glMultiTexCoord4fvARB\0"
    "glMultiTexCoord4i\0"
    "glMultiTexCoord4iv\0"
    "glMultiTexCoord4s\0"
    "glMultiTexCoord4sv\0"
    "glCompressedTexImage1D\0"
    "glCompressedTexImage2D\0"
    "glCompressedTexImage3D\0"
    "glCompressedTexSubImage1D\0"
    "glCompressedTexSubImage2D\0"
    "glCompressedTexSubImage3D\0"
    "glGetCompressedTexImage\0"
    "glLoadTransposeMatrixd\0"
    "glLoadTransposeMatrixf\0"
    "glMultTransposeMatrixd\0"
    "glMultTransposeMatrixf\0"
    "glSampleCoverage\0"
    "glBlendFuncSeparate\0"
    "glFogCoordPointer\0"
    "glFogCoordd\0"
    "glFogCoorddv\0"
    "glMultiDrawArrays\0"
    "glPointParameterf\0"
    "glPointParameterfv\0"
    "glPointParameteri\0"
    "glPointParameteriv\0"
    "glSecondaryColor3b\0"
    "glSecondaryColor3bv\0"
    "glSecondaryColor3d\0"
    "glSecondaryColor3dv\0"
    "glSecondaryColor3i\0"
    "glSecondaryColor3iv\0"
    "glSecondaryColor3s\0"
    "glSecondaryColor3sv\0"
    "glSecondaryColor3ub\0"
    "glSecondaryColor3ubv\0"
    "glSecondaryColor3ui\0"
    "glSecondaryColor3uiv\0"
    "glSecondaryColor3us\0"
    "glSecondaryColor3usv\0"
    "glSecondaryColorPointer\0"
    "glWindowPos2d\0"
    "glWindowPos2dv\0"
    "glWindowPos2f\0"
    "glWindowPos2fv\0"
    "glWindowPos2i\0"
    "glWindowPos2iv\0"
    "glWindowPos2s\0"
    "glWindowPos2sv\0"
    "glWindowPos3d\0"
    "glWindowPos3dv\0"
    "glWindowPos3f\0"
    "glWindowPos3fv\0"
    "glWindowPos3i\0"
    "glWindowPos3iv\0"
    "glWindowPos3s\0"
    "glWindowPos3sv\0"
    "glBeginQuery\0"
    "glBindBuffer\0"
    "glBufferData\0"
    "glBufferSubData\0"
    "glDeleteBuffers\0"
    "glDeleteQueries\0"
    "glEndQuery\0"
    "glGenBuffers\0"
    "glGenQueries\0"
    "glGetBufferParameteriv\0"
    "glGetBufferPointerv\0"
    "glGetBufferSubData\0"
    "glGetQueryObjectiv\0"
    "glGetQueryObjectuiv\0"
    "glGetQueryiv\0"
    "glIsBuffer\0"
    "glIsQuery\0"
    "glMapBuffer\0"
    "glUnmapBuffer\0"
    "glAttachShader\0"
    "glBindAttribLocation\0"
    "glBlendEquationSeparate\0"
    "glCompileShader\0"
    "glCreateProgram\0"
    "glCreateShader\0"
    "glDeleteProgram\0"
    "glDeleteShader\0"
    "glDetachShader\0"
    "glDisableVertexAttribArray\0"
    "glDrawBuffers\0"
    "glEnableVertexAttribArray\0"
    "glGetActiveAttrib\0"
    "glGetActiveUniform\0"
    "glGetAttachedShaders\0"
    "glGetAttribLocation\0"
    "glGetProgramInfoLog\0"
    "glGetProgramiv\0"
    "glGetShaderInfoLog\0"
    "glGetShaderSource\0"
    "glGetShaderiv\0"
    "glGetUniformLocation\0"
    "glGetUniformfv\0"
    "glGetUniformiv\0"
    "glGetVertexAttribPointerv\0"
    "glGetVertexAttribdv\0"
    "glGetVertexAttribfv\0"
    "glGetVertexAttribiv\0"
    "glIsProgram\0"
    "glIsShader\0"
    "glLinkProgram\0"
    "glShaderSource\0"
    "glStencilFuncSeparate\0"
    "glStencilMaskSeparate\0"
    "glStencilOpSeparate\0"
    "glUniform1f\0"
    "glUniform1fv\0"
    "glUniform1i\0"
    "glUniform1iv\0"
    "glUniform2f\0"
    "glUniform2fv\0"
    "glUniform2i\0"
    "glUniform2iv\0"
    "glUniform3f\0"
    "glUniform3fv\0"
    "glUniform3i\0"
    "glUniform3iv\0"
    "glUniform4f\0"
    "glUniform4fv\0"
    "glUniform4i\0"
    "glUniform4iv\0"
    "glUniformMatrix2fv\0"
    "glUniformMatrix3fv\0"
    "glUniformMatrix4fv\0"
    "glUseProgram\0"
    "glValidateProgram\0"
    "glVertexAttrib1d\0"
    "glVertexAttrib1dv\0"
    "glVertexAttrib1s\0"
    "glVertexAttrib1sv\0"
    "glVertexAttrib2d\0"
    "glVertexAttrib2dv\0"
    "glVertexAttrib2s\0"
    "glVertexAttrib2sv\0"
    "glVertexAttrib3d\0"
    "glVertexAttrib3dv\0"
    "glVertexAttrib3s\0"
    "glVertexAttrib3sv\0"
    "glVertexAttrib4Nbv\0"
    "glVertexAttrib4Niv\0"
    "glVertexAttrib4Nsv\0"
    "glVertexAttrib4Nub\0"
    "glVertexAttrib4Nubv\0"
    "glVertexAttrib4Nuiv\0"
    "glVertexAttrib4Nusv\0"
    "glVertexAttrib4bv\0"
    "glVertexAttrib4d\0"
    "glVertexAttrib4dv\0"
    "glVertexAttrib4iv\0"
    "glVertexAttrib4s\0"
    "glVertexAttrib4sv\0"
    "glVertexAttrib4ubv\0"
    "glVertexAttrib4uiv\0"
    "glVertexAttrib4usv\0"
    "glVertexAttribPointer\0"
    "glUniformMatrix2x3fv\0"
    "glUniformMatrix2x4fv\0"
    "glUniformMatrix3x2fv\0"
    "glUniformMatrix3x4fv\0"
    "glUniformMatrix4x2fv\0"
    "glUniformMatrix4x3fv\0"
    "glBeginConditionalRender\0"
    "glBeginTransformFeedback\0"
    "glBindBufferBase\0"
    "glBindBufferRange\0"
    "glBindFragDataLocation\0"
    "glClampColor\0"
    "glClearBufferfi\0"
    "glClearBufferfv\0"
    "glClearBufferiv\0"
    "glClearBufferuiv\0"
    "glColorMaski\0"
    "glDisablei\0"
    "glEnablei\0"
    "glEndConditionalRender\0"
    "glEndTransformFeedback\0"
    "glGetBooleani_v\0"
    "glGetFragDataLocation\0"
    "glGetIntegeri_v\0"
    "glGetStringi\0"
    "glGetTexParameterIiv\0"
    "glGetTexParameterIuiv\0"
    "glGetTransformFeedbackVarying\0"
    "glGetUniformuiv\0"
    "glGetVertexAttribIiv\0"
    "glGetVertexAttribIuiv\0"
    "glIsEnabledi\0"
    "glTexParameterIiv\0"
    "glTexParameterIuiv\0"
    "glTransformFeedbackVaryings\0"
    "glUniform1ui\0"
    "glUniform1uiv\0"
    "glUniform2ui\0"
    "glUniform2uiv\0"
    "glUniform3ui\0"
    "glUniform3uiv\0"
    "glUniform4ui\0"
    "glUniform4uiv\0"
    "glVertexAttribI1iv\0"
    "glVertexAttribI1uiv\0"
    "glVertexAttribI4bv\0"
    "glVertexAttribI4sv\0"
    "glVertexAttribI4ubv\0"
    "glVertexAttribI4usv\0"
    "glVertexAttribIPointer\0"
    "glPrimitiveRestartIndex\0"
    "glTexBuffer\0"
    "glFramebufferTexture\0"
    "glGetBufferParameteri64v\0"
    "glGetInteger64i_v\0"
    "glVertexAttribDivisor\0"
    "glMinSampleShading\0"
    "glMemoryBarrierByRegion\0"
    "glBindProgramARB\0"
    "glDeleteProgramsARB\0"
    "glGenProgramsARB\0"
    "glGetProgramEnvParameterdvARB\0"
    "glGetProgramEnvParameterfvARB\0"
    "glGetProgramLocalParameterdvARB\0"
    "glGetProgramLocalParameterfvARB\0"
    "glGetProgramStringARB\0"
    "glGetProgramivARB\0"
    "glIsProgramARB\0"
    "glProgramEnvParameter4dARB\0"
    "glProgramEnvParameter4dvARB\0"
    "glProgramEnvParameter4fARB\0"
    "glProgramEnvParameter4fvARB\0"
    "glProgramLocalParameter4dARB\0"
    "glProgramLocalParameter4dvARB\0"
    "glProgramLocalParameter4fARB\0"
    "glProgramLocalParameter4fvARB\0"
    "glProgramStringARB\0"
    "glVertexAttrib1fARB\0"
    "glVertexAttrib1fvARB\0"
    "glVertexAttrib2fARB\0"
    "glVertexAttrib2fvARB\0"
    "glVertexAttrib3fARB\0"
    "glVertexAttrib3fvARB\0"
    "glVertexAttrib4fARB\0"
    "glVertexAttrib4fvARB\0"
    "glAttachObjectARB\0"
    "glCreateProgramObjectARB\0"
    "glCreateShaderObjectARB\0"
    "glDeleteObjectARB\0"
    "glDetachObjectARB\0"
    "glGetAttachedObjectsARB\0"
    "glGetHandleARB\0"
    "glGetInfoLogARB\0"
    "glGetObjectParameterfvARB\0"
    "glGetObjectParameterivARB\0"
    "glDrawArraysInstancedARB\0"
    "glDrawElementsInstancedARB\0"
    "glBindFramebuffer\0"
    "glBindRenderbuffer\0"
    "glBlitFramebuffer\0"
    "glCheckFramebufferStatus\0"
    "glDeleteFramebuffers\0"
    "glDeleteRenderbuffers\0"
    "glFramebufferRenderbuffer\0"
    "glFramebufferTexture1D\0"
    "glFramebufferTexture2D\0"
    "glFramebufferTexture3D\0"
    "glFramebufferTextureLayer\0"
    "glGenFramebuffers\0"
    "glGenRenderbuffers\0"
    "glGenerateMipmap\0"
    "glGetFramebufferAttachmentParameteriv\0"
    "glGetRenderbufferParameteriv\0"
    "glIsFramebuffer\0"
    "glIsRenderbuffer\0"
    "glRenderbufferStorage\0"
    "glRenderbufferStorageMultisample\0"
    "glFlushMappedBufferRange\0"
    "glMapBufferRange\0"
    "glBindVertexArray\0"
    "glDeleteVertexArrays\0"
    "glGenVertexArrays\0"
    "glIsVertexArray\0"
    "glGetActiveUniformBlockName\0"
    "glGetActiveUniformBlockiv\0"
    "glGetActiveUniformName\0"
    "glGetActiveUniformsiv\0"
    "glGetUniformBlockIndex\0"
    "glGetUniformIndices\0"
    "glUniformBlockBinding\0"
    "glCopyBufferSubData\0"
    "glClientWaitSync\0"
    "glDeleteSync\0"
    "glFenceSync\0"
    "glGetInteger64v\0"
    "glGetSynciv\0"
    "glIsSync\0"
    "glWaitSync\0"
    "glDrawElementsBaseVertex\0"
    "glDrawElementsInstancedBaseVertex\0"
    "glDrawRangeElementsBaseVertex\0"
    "glMultiDrawElementsBaseVertex\0"
    "glProvokingVertex\0"
    "glGetMultisamplefv\0"
    "glSampleMaski\0"
    "glTexImage2DMultisample\0"
    "glTexImage3DMultisample\0"
    "glBlendEquationSeparateiARB\0"
    "glBlendEquationiARB\0"
    "glBlendFuncSeparateiARB\0"
    "glBlendFunciARB\0"
    "glBindFragDataLocationIndexed\0"
    "glGetFragDataIndex\0"
    "glBindSampler\0"
    "glDeleteSamplers\0"
    "glGenSamplers\0"
    "glGetSamplerParameterIiv\0"
    "glGetSamplerParameterIuiv\0"
    "glGetSamplerParameterfv\0"
    "glGetSamplerParameteriv\0"
    "glIsSampler\0"
    "glSamplerParameterIiv\0"
    "glSamplerParameterIuiv\0"
    "glSamplerParameterf\0"
    "glSamplerParameterfv\0"
    "glSamplerParameteri\0"
    "glSamplerParameteriv\0"
    "glGetQueryObjecti64v\0"
    "glGetQueryObjectui64v\0"
    "glQueryCounter\0"
    "glColorP3ui\0"
    "glColorP3uiv\0"
    "glColorP4ui\0"
    "glColorP4uiv\0"
    "glMultiTexCoordP1ui\0"
    "glMultiTexCoordP1uiv\0"
    "glMultiTexCoordP2ui\0"
    "glMultiTexCoordP2uiv\0"
    "glMultiTexCoordP3ui\0"
    "glMultiTexCoordP3uiv\0"
    "glMultiTexCoordP4ui\0"
    "glMultiTexCoordP4uiv\0"
    "glNormalP3ui\0"
    "glNormalP3uiv\0"
    "glSecondaryColorP3ui\0"
    "glSecondaryColorP3uiv\0"
    "glTexCoordP1ui\0"
    "glTexCoordP1uiv\0"
    "glTexCoordP2ui\0"
    "glTexCoordP2uiv\0"
    "glTexCoordP3ui\0"
    "glTexCoordP3uiv\0"
    "glTexCoordP4ui\0"
    "glTexCoordP4uiv\0"
    "glVertexAttribP1ui\0"
    "glVertexAttribP1uiv\0"
    "glVertexAttribP2ui\0"
    "glVertexAttribP2uiv\0"
    "glVertexAttribP3ui\0"
    "glVertexAttribP3uiv\0"
    "glVertexAttribP4ui\0"
    "glVertexAttribP4uiv\0"
    "glVertexP2ui\0"
    "glVertexP2uiv\0"
    "glVertexP3ui\0"
    "glVertexP3uiv\0"
    "glVertexP4ui\0"
    "glVertexP4uiv\0"
    "glDrawArraysIndirect\0"
    "glDrawElementsIndirect\0"
    "glGetUniformdv\0"
    "glUniform1d\0"
    "glUniform1dv\0"
    "glUniform2d\0"
    "glUniform2dv\0"
    "glUniform3d\0"
    "glUniform3dv\0"
    "glUniform4d\0"
    "glUniform4dv\0"
    "glUniformMatrix2dv\0"
    "glUniformMatrix2x3dv\0"
    "glUniformMatrix2x4dv\0"
    "glUniformMatrix3dv\0"
    "glUniformMatrix3x2dv\0"
    "glUniformMatrix3x4dv\0"
    "glUniformMatrix4dv\0"
    "glUniformMatrix4x2dv\0"
    "glUniformMatrix4x3dv\0"
    "glGetActiveSubroutineName\0"
    "glGetActiveSubroutineUniformName\0"
    "glGetActiveSubroutineUniformiv\0"
    "glGetProgramStageiv\0"
    "glGetSubroutineIndex\0"
    "glGetSubroutineUniformLocation\0"
    "glGetUniformSubroutineuiv\0"
    "glUniformSubroutinesuiv\0"
    "glPatchParameterfv\0"
    "glPatchParameteri\0"
    "glBindTransformFeedback\0"
    "glDeleteTransformFeedbacks\0"
    "glDrawTransformFeedback\0"
    "glGenTransformFeedbacks\0"
    "glIsTransformFeedback\0"
    "glPauseTransformFeedback\0"
    "glResumeTransformFeedback\0"
    "glBeginQueryIndexed\0"
    "glDrawTransformFeedbackStream\0"
    "glEndQueryIndexed\0"
    "glGetQueryIndexediv\0"
    "glClearDepthf\0"
    "glDepthRangef\0"
    "glGetShaderPrecisionFormat\0"
    "glReleaseShaderCompiler\0"
    "glShaderBinary\0"
    "glGetProgramBinary\0"
    "glProgramBinary\0"
    "glProgramParameteri\0"
    "glGetVertexAttribLdv\0"
    "glVertexAttribL1d\0"
    "glVertexAttribL1dv\0"
    "glVertexAttribL2d\0"
    "glVertexAttribL2dv\0"
    "glVertexAttribL3d\0"
    "glVertexAttribL3dv\0"
    "glVertexAttribL4d\0"
    "glVertexAttribL4dv\0"
    "glVertexAttribLPointer\0"
    "glDepthRangeArrayv\0"
    "glDepthRangeIndexed\0"
    "glGetDoublei_v\0"
    "glGetFloati_v\0"
    "glScissorArrayv\0"
    "glScissorIndexed\0"
    "glScissorIndexedv\0"
    "glViewportArrayv\0"
    "glViewportIndexedf\0"
    "glViewportIndexedfv\0"
    "glGetGraphicsResetStatusARB\0"
    "glGetnColorTableARB\0"
    "glGetnCompressedTexImageARB\0"
    "glGetnConvolutionFilterARB\0"
    "glGetnHistogramARB\0"
    "glGetnMapdvARB\0"
    "glGetnMapfvARB\0"
    "glGetnMapivARB\0"
    "glGetnMinmaxARB\0"
    "glGetnPixelMapfvARB\0"
    "glGetnPixelMapuivARB\0"
    "glGetnPixelMapusvARB\0"
    "glGetnPolygonStippleARB\0"
    "glGetnSeparableFilterARB\0"
    "glGetnTexImageARB\0"
    "glGetnUniformdvARB\0"
    "glGetnUniformfvARB\0"
    "glGetnUniformivARB\0"
    "glGetnUniformuivARB\0"
    "glReadnPixelsARB\0"
    "glDrawArraysInstancedBaseInstance\0"
    "glDrawElementsInstancedBaseInstance\0"
    "glDrawElementsInstancedBaseVertexBaseInstance\0"
    "glDrawTransformFeedbackInstanced\0"
    "glDrawTransformFeedbackStreamInstanced\0"
    "glGetInternalformativ\0"
    "glGetActiveAtomicCounterBufferiv\0"
    "glBindImageTexture\0"
    "glMemoryBarrier\0"
    "glTexStorage1D\0"
    "glTexStorage2D\0"
    "glTexStorage3D\0"
    "glTextureStorage1DEXT\0"
    "glTextureStorage2DEXT\0"
    "glTextureStorage3DEXT\0"
    "glClearBufferData\0"
    "glClearBufferSubData\0"
    "glDispatchCompute\0"
    "glDispatchComputeIndirect\0"
    "glCopyImageSubData\0"
    "glTextureView\0"
    "glBindVertexBuffer\0"
    "glVertexAttribBinding\0"
    "glVertexAttribFormat\0"
    "glVertexAttribIFormat\0"
    "glVertexAttribLFormat\0"
    "glVertexBindingDivisor\0"
    "glFramebufferParameteri\0"
    "glGetFramebufferParameteriv\0"
    "glMultiDrawArraysIndirect\0"
    "glMultiDrawElementsIndirect\0"
    "glGetProgramInterfaceiv\0"
    "glGetProgramResourceIndex\0"
    "glGetProgramResourceLocation\0"
    "glGetProgramResourceLocationIndex\0"
    "glGetProgramResourceName\0"
    "glGetProgramResourceiv\0"
    "glTexBufferRange\0"
    "glTexStorage2DMultisample\0"
    "glTexStorage3DMultisample\0"
    "glBufferStorage\0"
    "glClearTexImage\0"
    "glClearTexSubImage\0"
    "glBindBuffersBase\0"
    "glBindBuffersRange\0"
    "glBindImageTextures\0"
    "glBindSamplers\0"
    "glBindTextures\0"
    "glBindVertexBuffers\0"
    "glClipControl\0"
    "glBindTextureUnit\0"
    "glBlitNamedFramebuffer\0"
    "glCheckNamedFramebufferStatus\0"
    "glClearNamedBufferData\0"
    "glClearNamedBufferSubData\0"
    "glClearNamedFramebufferfi\0"
    "glClearNamedFramebufferfv\0"
    "glClearNamedFramebufferiv\0"
    "glClearNamedFramebufferuiv\0"
    "glCompressedTextureSubImage1D\0"
    "glCompressedTextureSubImage2D\0"
    "glCompressedTextureSubImage3D\0"
    "glCopyNamedBufferSubData\0"
    "glCopyTextureSubImage1D\0"
    "glCopyTextureSubImage2D\0"
    "glCopyTextureSubImage3D\0"
    "glCreateBuffers\0"
    "glCreateFramebuffers\0"
    "glCreateProgramPipelines\0"
    "glCreateQueries\0"
    "glCreateRenderbuffers\0"
    "glCreateSamplers\0"
    "glCreateTextures\0"
    "glCreateTransformFeedbacks\0"
    "glCreateVertexArrays\0"
    "glDisableVertexArrayAttrib\0"
    "glEnableVertexArrayAttrib\0"
    "glFlushMappedNamedBufferRange\0"
    "glGenerateTextureMipmap\0"
    "glGetCompressedTextureImage\0"
    "glGetNamedBufferParameteri64v\0"
    "glGetNamedBufferParameteriv\0"
    "glGetNamedBufferPointerv\0"
    "glGetNamedBufferSubData\0"
    "glGetNamedFramebufferAttachmentParameteriv\0"
    "glGetNamedFramebufferParameteriv\0"
    "glGetNamedRenderbufferParameteriv\0"
    "glGetQueryBufferObjecti64v\0"
    "glGetQueryBufferObjectiv\0"
    "glGetQueryBufferObjectui64v\0"
    "glGetQueryBufferObjectuiv\0"
    "glGetTextureImage\0"
    "glGetTextureLevelParameterfv\0"
    "glGetTextureLevelParameteriv\0"
    "glGetTextureParameterIiv\0"
    "glGetTextureParameterIuiv\0"
    "glGetTextureParameterfv\0"
    "glGetTextureParameteriv\0"
    "glGetTransformFeedbacki64_v\0"
    "glGetTransformFeedbacki_v\0"
    "glGetTransformFeedbackiv\0"
    "glGetVertexArrayIndexed64iv\0"
    "glGetVertexArrayIndexediv\0"
    "glGetVertexArrayiv\0"
    "glInvalidateNamedFramebufferData\0"
    "glInvalidateNamedFramebufferSubData\0"
    "glMapNamedBuffer\0"
    "glMapNamedBufferRange\0"
    "glNamedBufferData\0"
    "glNamedBufferStorage\0"
    "glNamedBufferSubData\0"
    "glNamedFramebufferDrawBuffer\0"
    "glNamedFramebufferDrawBuffers\0"
    "glNamedFramebufferParameteri\0"
    "glNamedFramebufferReadBuffer\0"
    "glNamedFramebufferRenderbuffer\0"
    "glNamedFramebufferTexture\0"
    "glNamedFramebufferTextureLayer\0"
    "glNamedRenderbufferStorage\0"
    "glNamedRenderbufferStorageMultisample\0"
    "glTextureBuffer\0"
    "glTextureBufferRange\0"
    "glTextureParameterIiv\0"
    "glTextureParameterIuiv\0"
    "glTextureParameterf\0"
    "glTextureParameterfv\0"
    "glTextureParameteri\0"
    "glTextureParameteriv\0"
    "glTextureStorage1D\0"
    "glTextureStorage2D\0"
    "glTextureStorage2DMultisample\0"
    "glTextureStorage3D\0"
    "glTextureStorage3DMultisample\0"
    "glTextureSubImage1D\0"
    "glTextureSubImage2D\0"
    "glTextureSubImage3D\0"
    "glTransformFeedbackBufferBase\0"
    "glTransformFeedbackBufferRange\0"
    "glUnmapNamedBuffer\0"
    "glVertexArrayAttribBinding\0"
    "glVertexArrayAttribFormat\0"
    "glVertexArrayAttribIFormat\0"
    "glVertexArrayAttribLFormat\0"
    "glVertexArrayBindingDivisor\0"
    "glVertexArrayElementBuffer\0"
    "glVertexArrayVertexBuffer\0"
    "glVertexArrayVertexBuffers\0"
    "glGetCompressedTextureSubImage\0"
    "glGetTextureSubImage\0"
    "glInvalidateBufferData\0"
    "glInvalidateBufferSubData\0"
    "glInvalidateFramebuffer\0"
    "glInvalidateSubFramebuffer\0"
    "glInvalidateTexImage\0"
    "glInvalidateTexSubImage\0"
    "glPolygonOffsetEXT\0"
    "glDrawTexfOES\0"
    "glDrawTexfvOES\0"
    "glDrawTexiOES\0"
    "glDrawTexivOES\0"
    "glDrawTexsOES\0"
    "glDrawTexsvOES\0"
    "glDrawTexxOES\0"
    "glDrawTexxvOES\0"
    "glPointSizePointerOES\0"
    "glQueryMatrixxOES\0"
    "glSampleMaskSGIS\0"
    "glSamplePatternSGIS\0"
    "glColorPointerEXT\0"
    "glEdgeFlagPointerEXT\0"
    "glIndexPointerEXT\0"
    "glNormalPointerEXT\0"
    "glTexCoordPointerEXT\0"
    "glVertexPointerEXT\0"
    "glDiscardFramebufferEXT\0"
    "glActiveShaderProgram\0"
    "glBindProgramPipeline\0"
    "glCreateShaderProgramv\0"
    "glDeleteProgramPipelines\0"
    "glGenProgramPipelines\0"
    "glGetProgramPipelineInfoLog\0"
    "glGetProgramPipelineiv\0"
    "glIsProgramPipeline\0"
    "glLockArraysEXT\0"
    "glProgramUniform1d\0"
    "glProgramUniform1dv\0"
    "glProgramUniform1f\0"
    "glProgramUniform1fv\0"
    "glProgramUniform1i\0"
    "glProgramUniform1iv\0"
    "glProgramUniform1ui\0"
    "glProgramUniform1uiv\0"
    "glProgramUniform2d\0"
    "glProgramUniform2dv\0"
    "glProgramUniform2f\0"
    "glProgramUniform2fv\0"
    "glProgramUniform2i\0"
    "glProgramUniform2iv\0"
    "glProgramUniform2ui\0"
    "glProgramUniform2uiv\0"
    "glProgramUniform3d\0"
    "glProgramUniform3dv\0"
    "glProgramUniform3f\0"
    "glProgramUniform3fv\0"
    "glProgramUniform3i\0"
    "glProgramUniform3iv\0"
    "glProgramUniform3ui\0"
    "glProgramUniform3uiv\0"
    "glProgramUniform4d\0"
    "glProgramUniform4dv\0"
    "glProgramUniform4f\0"
    "glProgramUniform4fv\0"
    "glProgramUniform4i\0"
    "glProgramUniform4iv\0"
    "glProgramUniform4ui\0"
    "glProgramUniform4uiv\0"
    "glProgramUniformMatrix2dv\0"
    "glProgramUniformMatrix2fv\0"
    "glProgramUniformMatrix2x3dv\0"
    "glProgramUniformMatrix2x3fv\0"
    "glProgramUniformMatrix2x4dv\0"
    "glProgramUniformMatrix2x4fv\0"
    "glProgramUniformMatrix3dv\0"
    "glProgramUniformMatrix3fv\0"
    "glProgramUniformMatrix3x2dv\0"
    "glProgramUniformMatrix3x2fv\0"
    "glProgramUniformMatrix3x4dv\0"
    "glProgramUniformMatrix3x4fv\0"
    "glProgramUniformMatrix4dv\0"
    "glProgramUniformMatrix4fv\0"
    "glProgramUniformMatrix4x2dv\0"
    "glProgramUniformMatrix4x2fv\0"
    "glProgramUniformMatrix4x3dv\0"
    "glProgramUniformMatrix4x3fv\0"
    "glUnlockArraysEXT\0"
    "glUseProgramStages\0"
    "glValidateProgramPipeline\0"
    "glDebugMessageCallback\0"
    "glDebugMessageControl\0"
    "glDebugMessageInsert\0"
    "glGetDebugMessageLog\0"
    "glGetObjectLabel\0"
    "glGetObjectPtrLabel\0"
    "glObjectLabel\0"
    "glObjectPtrLabel\0"
    "glPopDebugGroup\0"
    "glPushDebugGroup\0"
    "glSecondaryColor3fEXT\0"
    "glSecondaryColor3fvEXT\0"
    "glMultiDrawElementsEXT\0"
    "glFogCoordfEXT\0"
    "glFogCoordfvEXT\0"
    "glResizeBuffersMESA\0"
    "glWindowPos4dMESA\0"
    "glWindowPos4dvMESA\0"
    "glWindowPos4fMESA\0"
    "glWindowPos4fvMESA\0"
    "glWindowPos4iMESA\0"
    "glWindowPos4ivMESA\0"
    "glWindowPos4sMESA\0"
    "glWindowPos4svMESA\0"
    "glMultiModeDrawArraysIBM\0"
    "glMultiModeDrawElementsIBM\0"
    "glAreProgramsResidentNV\0"
    "glExecuteProgramNV\0"
    "glGetProgramParameterdvNV\0"
    "glGetProgramParameterfvNV\0"
    "glGetProgramStringNV\0"
    "glGetProgramivNV\0"
    "glGetTrackMatrixivNV\0"
    "glGetVertexAttribdvNV\0"
    "glGetVertexAttribfvNV\0"
    "glGetVertexAttribivNV\0"
    "glLoadProgramNV\0"
    "glProgramParameters4dvNV\0"
    "glProgramParameters4fvNV\0"
    "glRequestResidentProgramsNV\0"
    "glTrackMatrixNV\0"
    "glVertexAttrib1dNV\0"
    "glVertexAttrib1dvNV\0"
    "glVertexAttrib1fNV\0"
    "glVertexAttrib1fvNV\0"
    "glVertexAttrib1sNV\0"
    "glVertexAttrib1svNV\0"
    "glVertexAttrib2dNV\0"
    "glVertexAttrib2dvNV\0"
    "glVertexAttrib2fNV\0"
    "glVertexAttrib2fvNV\0"
    "glVertexAttrib2sNV\0"
    "glVertexAttrib2svNV\0"
    "glVertexAttrib3dNV\0"
    "glVertexAttrib3dvNV\0"
    "glVertexAttrib3fNV\0"
    "glVertexAttrib3fvNV\0"
    "glVertexAttrib3sNV\0"
    "glVertexAttrib3svNV\0"
    "glVertexAttrib4dNV\0"
    "glVertexAttrib4dvNV\0"
    "glVertexAttrib4fNV\0"
    "glVertexAttrib4fvNV\0"
    "glVertexAttrib4sNV\0"
    "glVertexAttrib4svNV\0"
    "glVertexAttrib4ubNV\0"
    "glVertexAttrib4ubvNV\0"
    "glVertexAttribPointerNV\0"
    "glVertexAttribs1dvNV\0"
    "glVertexAttribs1fvNV\0"
    "glVertexAttribs1svNV\0"
    "glVertexAttribs2dvNV\0"
    "glVertexAttribs2fvNV\0"
    "glVertexAttribs2svNV\0"
    "glVertexAttribs3dvNV\0"
    "glVertexAttribs3fvNV\0"
    "glVertexAttribs3svNV\0"
    "glVertexAttribs4dvNV\0"
    "glVertexAttribs4fvNV\0"
    "glVertexAttribs4svNV\0"
    "glVertexAttribs4ubvNV\0"
    "glGetTexBumpParameterfvATI\0"
    "glGetTexBumpParameterivATI\0"
    "glTexBumpParameterfvATI\0"
    "glTexBumpParameterivATI\0"
    "glAlphaFragmentOp1ATI\0"
    "glAlphaFragmentOp2ATI\0"
    "glAlphaFragmentOp3ATI\0"
    "glBeginFragmentShaderATI\0"
    "glBindFragmentShaderATI\0"
    "glColorFragmentOp1ATI\0"
    "glColorFragmentOp2ATI\0"
    "glColorFragmentOp3ATI\0"
    "glDeleteFragmentShaderATI\0"
    "glEndFragmentShaderATI\0"
    "glGenFragmentShadersATI\0"
    "glPassTexCoordATI\0"
    "glSampleMapATI\0"
    "glSetFragmentShaderConstantATI\0"
    "glActiveStencilFaceEXT\0"
    "glBindVertexArrayAPPLE\0"
    "glGenVertexArraysAPPLE\0"
    "glGetProgramNamedParameterdvNV\0"
    "glGetProgramNamedParameterfvNV\0"
    "glProgramNamedParameter4dNV\0"
    "glProgramNamedParameter4dvNV\0"
    "glProgramNamedParameter4fNV\0"
    "glProgramNamedParameter4fvNV\0"
    "glPrimitiveRestartNV\0"
    "glGetTexGenxvOES\0"
    "glTexGenxOES\0"
    "glTexGenxvOES\0"
    "glDepthBoundsEXT\0"
    "glBindFramebufferEXT\0"
    "glBindRenderbufferEXT\0"
    "glBufferParameteriAPPLE\0"
    "glFlushMappedBufferRangeAPPLE\0"
    "glVertexAttribI1iEXT\0"
    "glVertexAttribI1uiEXT\0"
    "glVertexAttribI2iEXT\0"
    "glVertexAttribI2ivEXT\0"
    "glVertexAttribI2uiEXT\0"
    "glVertexAttribI2uivEXT\0"
    "glVertexAttribI3iEXT\0"
    "glVertexAttribI3ivEXT\0"
    "glVertexAttribI3uiEXT\0"
    "glVertexAttribI3uivEXT\0"
    "glVertexAttribI4iEXT\0"
    "glVertexAttribI4ivEXT\0"
    "glVertexAttribI4uiEXT\0"
    "glVertexAttribI4uivEXT\0"
    "glClearColorIiEXT\0"
    "glClearColorIuiEXT\0"
    "glBindBufferOffsetEXT\0"
    "glBeginPerfMonitorAMD\0"
    "glDeletePerfMonitorsAMD\0"
    "glEndPerfMonitorAMD\0"
    "glGenPerfMonitorsAMD\0"
    "glGetPerfMonitorCounterDataAMD\0"
    "glGetPerfMonitorCounterInfoAMD\0"
    "glGetPerfMonitorCounterStringAMD\0"
    "glGetPerfMonitorCountersAMD\0"
    "glGetPerfMonitorGroupStringAMD\0"
    "glGetPerfMonitorGroupsAMD\0"
    "glSelectPerfMonitorCountersAMD\0"
    "glGetObjectParameterivAPPLE\0"
    "glObjectPurgeableAPPLE\0"
    "glObjectUnpurgeableAPPLE\0"
    "glActiveProgramEXT\0"
    "glCreateShaderProgramEXT\0"
    "glUseShaderProgramEXT\0"
    "glTextureBarrierNV\0"
    "glVDPAUFiniNV\0"
    "glVDPAUGetSurfaceivNV\0"
    "glVDPAUInitNV\0"
    "glVDPAUIsSurfaceNV\0"
    "glVDPAUMapSurfacesNV\0"
    "glVDPAURegisterOutputSurfaceNV\0"
    "glVDPAURegisterVideoSurfaceNV\0"
    "glVDPAUSurfaceAccessNV\0"
    "glVDPAUUnmapSurfacesNV\0"
    "glVDPAUUnregisterSurfaceNV\0"
    "glBeginPerfQueryINTEL\0"
    "glCreatePerfQueryINTEL\0"
    "glDeletePerfQueryINTEL\0"
    "glEndPerfQueryINTEL\0"
    "glGetFirstPerfQueryIdINTEL\0"
    "glGetNextPerfQueryIdINTEL\0"
    "glGetPerfCounterInfoINTEL\0"
    "glGetPerfQueryDataINTEL\0"
    "glGetPerfQueryIdByNameINTEL\0"
    "glGetPerfQueryInfoINTEL\0"
    "glPolygonOffsetClampEXT\0"
    "glStencilFuncSeparateATI\0"
    "glProgramEnvParameters4fvEXT\0"
    "glProgramLocalParameters4fvEXT\0"
    "glEGLImageTargetRenderbufferStorageOES\0"
    "glEGLImageTargetTexture2DOES\0"
    "glAlphaFuncx\0"
    "glClearColorx\0"
    "glClearDepthx\0"
    "glColor4x\0"
    "glDepthRangex\0"
    "glFogx\0"
    "glFogxv\0"
    "glFrustumf\0"
    "glFrustumx\0"
    "glLightModelx\0"
    "glLightModelxv\0"
    "glLightx\0"
    "glLightxv\0"
    "glLineWidthx\0"
    "glLoadMatrixx\0"
    "glMaterialx\0"
    "glMaterialxv\0"
    "glMultMatrixx\0"
    "glMultiTexCoord4x\0"
    "glNormal3x\0"
    "glOrthof\0"
    "glOrthox\0"
    "glPointSizex\0"
    "glPolygonOffsetx\0"
    "glRotatex\0"
    "glSampleCoveragex\0"
    "glScalex\0"
    "glTexEnvx\0"
    "glTexEnvxv\0"
    "glTexParameterx\0"
    "glTranslatex\0"
    "glClipPlanef\0"
    "glClipPlanex\0"
    "glGetClipPlanef\0"
    "glGetClipPlanex\0"
    "glGetFixedv\0"
    "glGetLightxv\0"
    "glGetMaterialxv\0"
    "glGetTexEnvxv\0"
    "glGetTexParameterxv\0"
    "glPointParameterx\0"
    "glPointParameterxv\0"
    "glTexParameterxv\0"
    "glTexGenfOES\0"
    "glTexGenfvOES\0"
    "glTexGeniOES\0"
    "glTexGenivOES\0"
    "glReadBufferNV\0"
    "glGetTexGenfvOES\0"
    "glGetTexGenivOES\0"
    "glArrayElementEXT\0"
    "glBindTextureEXT\0"
    "glDrawArraysEXT\0"
    "glAreTexturesResidentEXT\0"
    "glCopyTexImage1DEXT\0"
    "glCopyTexImage2DEXT\0"
    "glCopyTexSubImage1DEXT\0"
    "glCopyTexSubImage2DEXT\0"
    "glDeleteTexturesEXT\0"
    "glGenTexturesEXT\0"
    "glGetPointervEXT\0"
    "glIsTextureEXT\0"
    "glPrioritizeTexturesEXT\0"
    "glTexSubImage1DEXT\0"
    "glTexSubImage2DEXT\0"
    "glBlendColorEXT\0"
    "glBlendEquationEXT\0"
    "glBlendEquationOES\0"
    "glDrawRangeElementsEXT\0"
    "glColorTableSGI\0"
    "glColorTableEXT\0"
    "glColorTableParameterfvSGI\0"
    "glColorTableParameterivSGI\0"
    "glCopyColorTableSGI\0"
    "glGetColorTableSGI\0"
    "glGetColorTableEXT\0"
    "glGetColorTableParameterfvSGI\0"
    "glGetColorTableParameterfvEXT\0"
    "glGetColorTableParameterivSGI\0"
    "glGetColorTableParameterivEXT\0"
    "glColorSubTableEXT\0"
    "glCopyColorSubTableEXT\0"
    "glConvolutionFilter1DEXT\0"
    "glConvolutionFilter2DEXT\0"
    "glConvolutionParameterfEXT\0"
    "glConvolutionParameterfvEXT\0"
    "glConvolutionParameteriEXT\0"
    "glConvolutionParameterivEXT\0"
    "glCopyConvolutionFilter1DEXT\0"
    "glCopyConvolutionFilter2DEXT\0"
    "glGetConvolutionFilterEXT\0"
    "glGetConvolutionParameterfvEXT\0"
    "glGetConvolutionParameterivEXT\0"
    "glGetSeparableFilterEXT\0"
    "glSeparableFilter2DEXT\0"
    "glGetHistogramEXT\0"
    "glGetHistogramParameterfvEXT\0"
    "glGetHistogramParameterivEXT\0"
    "glGetMinmaxEXT\0"
    "glGetMinmaxParameterfvEXT\0"
    "glGetMinmaxParameterivEXT\0"
    "glHistogramEXT\0"
    "glMinmaxEXT\0"
    "glResetHistogramEXT\0"
    "glResetMinmaxEXT\0"
    "glTexImage3DEXT\0"
    "glTexImage3DOES\0"
    "glTexSubImage3DEXT\0"
    "glTexSubImage3DOES\0"
    "glCopyTexSubImage3DEXT\0"
    "glCopyTexSubImage3DOES\0"
    "glActiveTextureARB\0"
    "glClientActiveTextureARB\0"
    "glMultiTexCoord1dARB\0"
    "glMultiTexCoord1dvARB\0"
    "glMultiTexCoord1f\0"
    "glMultiTexCoord1fv\0"
    "glMultiTexCoord1iARB\0"
    "glMultiTexCoord1ivARB\0"
    "glMultiTexCoord1sARB\0"
    "glMultiTexCoord1svARB\0"
    "glMultiTexCoord2dARB\0"
    "glMultiTexCoord2dvARB\0"
    "glMultiTexCoord2f\0"
    "glMultiTexCoord2fv\0"
    "glMultiTexCoord2iARB\0"
    "glMultiTexCoord2ivARB\0"
    "glMultiTexCoord2sARB\0"
    "glMultiTexCoord2svARB\0"
    "glMultiTexCoord3dARB\0"
    "glMultiTexCoord3dvARB\0"
    "glMultiTexCoord3f\0"
    "glMultiTexCoord3fv\0"
    "glMultiTexCoord3iARB\0"
    "glMultiTexCoord3ivARB\0"
    "glMultiTexCoord3sARB\0"
    "glMultiTexCoord3svARB\0"
    "glMultiTexCoord4dARB\0"
    "glMultiTexCoord4dvARB\0"
    "glMultiTexCoord4f\0"
    "glMultiTexCoord4fv\0"
    "glMultiTexCoord4iARB\0"
    "glMultiTexCoord4ivARB\0"
    "glMultiTexCoord4sARB\0"
    "glMultiTexCoord4svARB\0"
    "glCompressedTexImage1DARB\0"
    "glCompressedTexImage2DARB\0"
    "glCompressedTexImage3DARB\0"
    "glCompressedTexImage3DOES\0"
    "glCompressedTexSubImage1DARB\0"
    "glCompressedTexSubImage2DARB\0"
    "glCompressedTexSubImage3DARB\0"
    "glCompressedTexSubImage3DOES\0"
    "glGetCompressedTexImageARB\0"
    "glLoadTransposeMatrixdARB\0"
    "glLoadTransposeMatrixfARB\0"
    "glMultTransposeMatrixdARB\0"
    "glMultTransposeMatrixfARB\0"
    "glSampleCoverageARB\0"
    "glBlendFuncSeparateEXT\0"
    "glBlendFuncSeparateINGR\0"
    "glBlendFuncSeparateOES\0"
    "glFogCoordPointerEXT\0"
    "glFogCoorddEXT\0"
    "glFogCoorddvEXT\0"
    "glMultiDrawArraysEXT\0"
    "glPointParameterfARB\0"
    "glPointParameterfEXT\0"
    "glPointParameterfSGIS\0"
    "glPointParameterfvARB\0"
    "glPointParameterfvEXT\0"
    "glPointParameterfvSGIS\0"
    "glPointParameteriNV\0"
    "glPointParameterivNV\0"
    "glSecondaryColor3bEXT\0"
    "glSecondaryColor3bvEXT\0"
    "glSecondaryColor3dEXT\0"
    "glSecondaryColor3dvEXT\0"
    "glSecondaryColor3iEXT\0"
    "glSecondaryColor3ivEXT\0"
    "glSecondaryColor3sEXT\0"
    "glSecondaryColor3svEXT\0"
    "glSecondaryColor3ubEXT\0"
    "glSecondaryColor3ubvEXT\0"
    "glSecondaryColor3uiEXT\0"
    "glSecondaryColor3uivEXT\0"
    "glSecondaryColor3usEXT\0"
    "glSecondaryColor3usvEXT\0"
    "glSecondaryColorPointerEXT\0"
    "glWindowPos2dARB\0"
    "glWindowPos2dMESA\0"
    "glWindowPos2dvARB\0"
    "glWindowPos2dvMESA\0"
    "glWindowPos2fARB\0"
    "glWindowPos2fMESA\0"
    "glWindowPos2fvARB\0"
    "glWindowPos2fvMESA\0"
    "glWindowPos2iARB\0"
    "glWindowPos2iMESA\0"
    "glWindowPos2ivARB\0"
    "glWindowPos2ivMESA\0"
    "glWindowPos2sARB\0"
    "glWindowPos2sMESA\0"
    "glWindowPos2svARB\0"
    "glWindowPos2svMESA\0"
    "glWindowPos3dARB\0"
    "glWindowPos3dMESA\0"
    "glWindowPos3dvARB\0"
    "glWindowPos3dvMESA\0"
    "glWindowPos3fARB\0"
    "glWindowPos3fMESA\0"
    "glWindowPos3fvARB\0"
    "glWindowPos3fvMESA\0"
    "glWindowPos3iARB\0"
    "glWindowPos3iMESA\0"
    "glWindowPos3ivARB\0"
    "glWindowPos3ivMESA\0"
    "glWindowPos3sARB\0"
    "glWindowPos3sMESA\0"
    "glWindowPos3svARB\0"
    "glWindowPos3svMESA\0"
    "glBeginQueryARB\0"
    "glBindBufferARB\0"
    "glBufferDataARB\0"
    "glBufferSubDataARB\0"
    "glDeleteBuffersARB\0"
    "glDeleteQueriesARB\0"
    "glEndQueryARB\0"
    "glGenBuffersARB\0"
    "glGenQueriesARB\0"
    "glGetBufferParameterivARB\0"
    "glGetBufferPointervARB\0"
    "glGetBufferPointervOES\0"
    "glGetBufferSubDataARB\0"
    "glGetQueryObjectivARB\0"
    "glGetQueryObjectuivARB\0"
    "glGetQueryivARB\0"
    "glIsBufferARB\0"
    "glIsQueryARB\0"
    "glMapBufferARB\0"
    "glMapBufferOES\0"
    "glUnmapBufferARB\0"
    "glUnmapBufferOES\0"
    "glBindAttribLocationARB\0"
    "glBlendEquationSeparateEXT\0"
    "glBlendEquationSeparateATI\0"
    "glBlendEquationSeparateOES\0"
    "glCompileShaderARB\0"
    "glDisableVertexAttribArrayARB\0"
    "glDrawBuffersARB\0"
    "glDrawBuffersATI\0"
    "glDrawBuffersNV\0"
    "glDrawBuffersEXT\0"
    "glEnableVertexAttribArrayARB\0"
    "glGetActiveAttribARB\0"
    "glGetActiveUniformARB\0"
    "glGetAttribLocationARB\0"
    "glGetShaderSourceARB\0"
    "glGetUniformLocationARB\0"
    "glGetUniformfvARB\0"
    "glGetUniformivARB\0"
    "glGetVertexAttribPointervARB\0"
    "glGetVertexAttribPointervNV\0"
    "glGetVertexAttribdvARB\0"
    "glGetVertexAttribfvARB\0"
    "glGetVertexAttribivARB\0"
    "glLinkProgramARB\0"
    "glShaderSourceARB\0"
    "glStencilOpSeparateATI\0"
    "glUniform1fARB\0"
    "glUniform1fvARB\0"
    "glUniform1iARB\0"
    "glUniform1ivARB\0"
    "glUniform2fARB\0"
    "glUniform2fvARB\0"
    "glUniform2iARB\0"
    "glUniform2ivARB\0"
    "glUniform3fARB\0"
    "glUniform3fvARB\0"
    "glUniform3iARB\0"
    "glUniform3ivARB\0"
    "glUniform4fARB\0"
    "glUniform4fvARB\0"
    "glUniform4iARB\0"
    "glUniform4ivARB\0"
    "glUniformMatrix2fvARB\0"
    "glUniformMatrix3fvARB\0"
    "glUniformMatrix4fvARB\0"
    "glUseProgramObjectARB\0"
    "glValidateProgramARB\0"
    "glVertexAttrib1dARB\0"
    "glVertexAttrib1dvARB\0"
    "glVertexAttrib1sARB\0"
    "glVertexAttrib1svARB\0"
    "glVertexAttrib2dARB\0"
    "glVertexAttrib2dvARB\0"
    "glVertexAttrib2sARB\0"
    "glVertexAttrib2svARB\0"
    "glVertexAttrib3dARB\0"
    "glVertexAttrib3dvARB\0"
    "glVertexAttrib3sARB\0"
    "glVertexAttrib3svARB\0"
    "glVertexAttrib4NbvARB\0"
    "glVertexAttrib4NivARB\0"
    "glVertexAttrib4NsvARB\0"
    "glVertexAttrib4NubARB\0"
    "glVertexAttrib4NubvARB\0"
    "glVertexAttrib4NuivARB\0"
    "glVertexAttrib4NusvARB\0"
    "glVertexAttrib4bvARB\0"
    "glVertexAttrib4dARB\0"
    "glVertexAttrib4dvARB\0"
    "glVertexAttrib4ivARB\0"
    "glVertexAttrib4sARB\0"
    "glVertexAttrib4svARB\0"
    "glVertexAttrib4ubvARB\0"
    "glVertexAttrib4uivARB\0"
    "glVertexAttrib4usvARB\0"
    "glVertexAttribPointerARB\0"
    "glBeginConditionalRenderNV\0"
    "glBeginTransformFeedbackEXT\0"
    "glBindBufferBaseEXT\0"
    "glBindBufferRangeEXT\0"
    "glBindFragDataLocationEXT\0"
    "glClampColorARB\0"
    "glColorMaskIndexedEXT\0"
    "glDisableIndexedEXT\0"
    "glEnableIndexedEXT\0"
    "glEndConditionalRenderNV\0"
    "glEndTransformFeedbackEXT\0"
    "glGetBooleanIndexedvEXT\0"
    "glGetFragDataLocationEXT\0"
    "glGetIntegerIndexedvEXT\0"
    "glGetTexParameterIivEXT\0"
    "glGetTexParameterIuivEXT\0"
    "glGetTransformFeedbackVaryingEXT\0"
    "glGetUniformuivEXT\0"
    "glGetVertexAttribIivEXT\0"
    "glGetVertexAttribIuivEXT\0"
    "glIsEnabledIndexedEXT\0"
    "glTexParameterIivEXT\0"
    "glTexParameterIuivEXT\0"
    "glTransformFeedbackVaryingsEXT\0"
    "glUniform1uiEXT\0"
    "glUniform1uivEXT\0"
    "glUniform2uiEXT\0"
    "glUniform2uivEXT\0"
    "glUniform3uiEXT\0"
    "glUniform3uivEXT\0"
    "glUniform4uiEXT\0"
    "glUniform4uivEXT\0"
    "glVertexAttribI1ivEXT\0"
    "glVertexAttribI1uivEXT\0"
    "glVertexAttribI4bvEXT\0"
    "glVertexAttribI4svEXT\0"
    "glVertexAttribI4ubvEXT\0"
    "glVertexAttribI4usvEXT\0"
    "glVertexAttribIPointerEXT\0"
    "glPrimitiveRestartIndexNV\0"
    "glTexBufferARB\0"
    "glFramebufferTextureARB\0"
    "glVertexAttribDivisorARB\0"
    "glMinSampleShadingARB\0"
    "glBindProgramNV\0"
    "glDeleteProgramsNV\0"
    "glGenProgramsNV\0"
    "glIsProgramNV\0"
    "glProgramParameter4dNV\0"
    "glProgramParameter4dvNV\0"
    "glProgramParameter4fNV\0"
    "glProgramParameter4fvNV\0"
    "glVertexAttrib1f\0"
    "glVertexAttrib1fv\0"
    "glVertexAttrib2f\0"
    "glVertexAttrib2fv\0"
    "glVertexAttrib3f\0"
    "glVertexAttrib3fv\0"
    "glVertexAttrib4f\0"
    "glVertexAttrib4fv\0"
    "glDrawArraysInstancedEXT\0"
    "glDrawArraysInstanced\0"
    "glDrawElementsInstancedEXT\0"
    "glDrawElementsInstanced\0"
    "glBindFramebufferOES\0"
    "glBindRenderbufferOES\0"
    "glBlitFramebufferEXT\0"
    "glCheckFramebufferStatusEXT\0"
    "glCheckFramebufferStatusOES\0"
    "glDeleteFramebuffersEXT\0"
    "glDeleteFramebuffersOES\0"
    "glDeleteRenderbuffersEXT\0"
    "glDeleteRenderbuffersOES\0"
    "glFramebufferRenderbufferEXT\0"
    "glFramebufferRenderbufferOES\0"
    "glFramebufferTexture1DEXT\0"
    "glFramebufferTexture2DEXT\0"
    "glFramebufferTexture2DOES\0"
    "glFramebufferTexture3DEXT\0"
    "glFramebufferTexture3DOES\0"
    "glFramebufferTextureLayerARB\0"
    "glFramebufferTextureLayerEXT\0"
    "glGenFramebuffersEXT\0"
    "glGenFramebuffersOES\0"
    "glGenRenderbuffersEXT\0"
    "glGenRenderbuffersOES\0"
    "glGenerateMipmapEXT\0"
    "glGenerateMipmapOES\0"
    "glGetFramebufferAttachmentParameterivEXT\0"
    "glGetFramebufferAttachmentParameterivOES\0"
    "glGetRenderbufferParameterivEXT\0"
    "glGetRenderbufferParameterivOES\0"
    "glIsFramebufferEXT\0"
    "glIsFramebufferOES\0"
    "glIsRenderbufferEXT\0"
    "glIsRenderbufferOES\0"
    "glRenderbufferStorageEXT\0"
    "glRenderbufferStorageOES\0"
    "glRenderbufferStorageMultisampleEXT\0"
    "glFlushMappedBufferRangeEXT\0"
    "glMapBufferRangeEXT\0"
    "glBindVertexArrayOES\0"
    "glDeleteVertexArraysAPPLE\0"
    "glDeleteVertexArraysOES\0"
    "glGenVertexArraysOES\0"
    "glIsVertexArrayAPPLE\0"
    "glIsVertexArrayOES\0"
    "glProvokingVertexEXT\0"
    "glBlendEquationSeparateIndexedAMD\0"
    "glBlendEquationSeparatei\0"
    "glBlendEquationIndexedAMD\0"
    "glBlendEquationi\0"
    "glBlendFuncSeparateIndexedAMD\0"
    "glBlendFuncSeparatei\0"
    "glBlendFuncIndexedAMD\0"
    "glBlendFunci\0"
    "glGetQueryObjecti64vEXT\0"
    "glGetQueryObjectui64vEXT\0"
    "glClearDepthfOES\0"
    "glDepthRangefOES\0"
    "glGetProgramBinaryOES\0"
    "glProgramBinaryOES\0"
    "glProgramParameteriARB\0"
    "glProgramParameteriEXT\0"
    "glSampleMaskEXT\0"
    "glSamplePatternEXT\0"
    "glActiveShaderProgramEXT\0"
    "glBindProgramPipelineEXT\0"
    "glCreateShaderProgramvEXT\0"
    "glDeleteProgramPipelinesEXT\0"
    "glGenProgramPipelinesEXT\0"
    "glGetProgramPipelineInfoLogEXT\0"
    "glGetProgramPipelineivEXT\0"
    "glIsProgramPipelineEXT\0"
    "glProgramUniform1fEXT\0"
    "glProgramUniform1fvEXT\0"
    "glProgramUniform1iEXT\0"
    "glProgramUniform1ivEXT\0"
    "glProgramUniform1uiEXT\0"
    "glProgramUniform1uivEXT\0"
    "glProgramUniform2fEXT\0"
    "glProgramUniform2fvEXT\0"
    "glProgramUniform2iEXT\0"
    "glProgramUniform2ivEXT\0"
    "glProgramUniform2uiEXT\0"
    "glProgramUniform2uivEXT\0"
    "glProgramUniform3fEXT\0"
    "glProgramUniform3fvEXT\0"
    "glProgramUniform3iEXT\0"
    "glProgramUniform3ivEXT\0"
    "glProgramUniform3uiEXT\0"
    "glProgramUniform3uivEXT\0"
    "glProgramUniform4fEXT\0"
    "glProgramUniform4fvEXT\0"
    "glProgramUniform4iEXT\0"
    "glProgramUniform4ivEXT\0"
    "glProgramUniform4uiEXT\0"
    "glProgramUniform4uivEXT\0"
    "glProgramUniformMatrix2fvEXT\0"
    "glProgramUniformMatrix2x3fvEXT\0"
    "glProgramUniformMatrix2x4fvEXT\0"
    "glProgramUniformMatrix3fvEXT\0"
    "glProgramUniformMatrix3x2fvEXT\0"
    "glProgramUniformMatrix3x4fvEXT\0"
    "glProgramUniformMatrix4fvEXT\0"
    "glProgramUniformMatrix4x2fvEXT\0"
    "glProgramUniformMatrix4x3fvEXT\0"
    "glUseProgramStagesEXT\0"
    "glValidateProgramPipelineEXT\0"
    "glDebugMessageCallbackARB\0"
    "glDebugMessageControlARB\0"
    "glDebugMessageInsertARB\0"
    "glGetDebugMessageLogARB\0"
    "glSecondaryColor3f\0"
    "glSecondaryColor3fv\0"
    "glMultiDrawElements\0"
    "glFogCoordf\0"
    "glFogCoordfv\0"
    "glVertexAttribI1i\0"
    "glVertexAttribI1ui\0"
    "glVertexAttribI2i\0"
    "glVertexAttribI2iv\0"
    "glVertexAttribI2ui\0"
    "glVertexAttribI2uiv\0"
    "glVertexAttribI3i\0"
    "glVertexAttribI3iv\0"
    "glVertexAttribI3ui\0"
    "glVertexAttribI3uiv\0"
    "glVertexAttribI4i\0"
    "glVertexAttribI4iv\0"
    "glVertexAttribI4ui\0"
    "glVertexAttribI4uiv\0"
    "glTextureBarrier\0"
    "glAlphaFuncxOES\0"
    "glClearColorxOES\0"
    "glClearDepthxOES\0"
    "glColor4xOES\0"
    "glDepthRangexOES\0"
    "glFogxOES\0"
    "glFogxvOES\0"
    "glFrustumfOES\0"
    "glFrustumxOES\0"
    "glLightModelxOES\0"
    "glLightModelxvOES\0"
    "glLightxOES\0"
    "glLightxvOES\0"
    "glLineWidthxOES\0"
    "glLoadMatrixxOES\0"
    "glMaterialxOES\0"
    "glMaterialxvOES\0"
    "glMultMatrixxOES\0"
    "glMultiTexCoord4xOES\0"
    "glNormal3xOES\0"
    "glOrthofOES\0"
    "glOrthoxOES\0"
    "glPointSizexOES\0"
    "glPolygonOffsetxOES\0"
    "glRotatexOES\0"
    "glSampleCoveragexOES\0"
    "glScalexOES\0"
    "glTexEnvxOES\0"
    "glTexEnvxvOES\0"
    "glTexParameterxOES\0"
    "glTranslatexOES\0"
    "glClipPlanefOES\0"
    "glClipPlanexOES\0"
    "glGetClipPlanefOES\0"
    "glGetClipPlanexOES\0"
    "glGetFixedvOES\0"
    "glGetLightxvOES\0"
    "glGetMaterialxvOES\0"
    "glGetTexEnvxvOES\0"
    "glGetTexParameterxvOES\0"
    "glPointParameterxOES\0"
    "glPointParameterxvOES\0"
    "glTexParameterxvOES\0"
    ;


#ifdef USE_MGL_NAMESPACE
#define gl_dispatch_stub_343 mgl_dispatch_stub_343
#define gl_dispatch_stub_344 mgl_dispatch_stub_344
#define gl_dispatch_stub_345 mgl_dispatch_stub_345
#define gl_dispatch_stub_356 mgl_dispatch_stub_356
#define gl_dispatch_stub_357 mgl_dispatch_stub_357
#define gl_dispatch_stub_358 mgl_dispatch_stub_358
#define gl_dispatch_stub_359 mgl_dispatch_stub_359
#define gl_dispatch_stub_361 mgl_dispatch_stub_361
#define gl_dispatch_stub_362 mgl_dispatch_stub_362
#define gl_dispatch_stub_363 mgl_dispatch_stub_363
#define gl_dispatch_stub_364 mgl_dispatch_stub_364
#define gl_dispatch_stub_365 mgl_dispatch_stub_365
#define gl_dispatch_stub_366 mgl_dispatch_stub_366
#define gl_dispatch_stub_621 mgl_dispatch_stub_621
#define gl_dispatch_stub_731 mgl_dispatch_stub_731
#define gl_dispatch_stub_732 mgl_dispatch_stub_732
#define gl_dispatch_stub_733 mgl_dispatch_stub_733
#define gl_dispatch_stub_774 mgl_dispatch_stub_774
#define gl_dispatch_stub_775 mgl_dispatch_stub_775
#define gl_dispatch_stub_776 mgl_dispatch_stub_776
#define gl_dispatch_stub_777 mgl_dispatch_stub_777
#define gl_dispatch_stub_778 mgl_dispatch_stub_778
#define gl_dispatch_stub_779 mgl_dispatch_stub_779
#define gl_dispatch_stub_780 mgl_dispatch_stub_780
#define gl_dispatch_stub_781 mgl_dispatch_stub_781
#define gl_dispatch_stub_782 mgl_dispatch_stub_782
#define gl_dispatch_stub_783 mgl_dispatch_stub_783
#define gl_dispatch_stub_784 mgl_dispatch_stub_784
#define gl_dispatch_stub_785 mgl_dispatch_stub_785
#define gl_dispatch_stub_786 mgl_dispatch_stub_786
#define gl_dispatch_stub_787 mgl_dispatch_stub_787
#define gl_dispatch_stub_788 mgl_dispatch_stub_788
#define gl_dispatch_stub_789 mgl_dispatch_stub_789
#define gl_dispatch_stub_790 mgl_dispatch_stub_790
#define gl_dispatch_stub_791 mgl_dispatch_stub_791
#define gl_dispatch_stub_792 mgl_dispatch_stub_792
#define gl_dispatch_stub_793 mgl_dispatch_stub_793
#define gl_dispatch_stub_794 mgl_dispatch_stub_794
#define gl_dispatch_stub_795 mgl_dispatch_stub_795
#define gl_dispatch_stub_796 mgl_dispatch_stub_796
#define gl_dispatch_stub_797 mgl_dispatch_stub_797
#define gl_dispatch_stub_798 mgl_dispatch_stub_798
#define gl_dispatch_stub_799 mgl_dispatch_stub_799
#define gl_dispatch_stub_800 mgl_dispatch_stub_800
#define gl_dispatch_stub_801 mgl_dispatch_stub_801
#define gl_dispatch_stub_821 mgl_dispatch_stub_821
#define gl_dispatch_stub_822 mgl_dispatch_stub_822
#define gl_dispatch_stub_823 mgl_dispatch_stub_823
#define gl_dispatch_stub_824 mgl_dispatch_stub_824
#define gl_dispatch_stub_825 mgl_dispatch_stub_825
#define gl_dispatch_stub_826 mgl_dispatch_stub_826
#define gl_dispatch_stub_827 mgl_dispatch_stub_827
#define gl_dispatch_stub_828 mgl_dispatch_stub_828
#define gl_dispatch_stub_829 mgl_dispatch_stub_829
#define gl_dispatch_stub_830 mgl_dispatch_stub_830
#define gl_dispatch_stub_866 mgl_dispatch_stub_866
#define gl_dispatch_stub_888 mgl_dispatch_stub_888
#define gl_dispatch_stub_889 mgl_dispatch_stub_889
#define gl_dispatch_stub_892 mgl_dispatch_stub_892
#define gl_dispatch_stub_893 mgl_dispatch_stub_893
#define gl_dispatch_stub_894 mgl_dispatch_stub_894
#define gl_dispatch_stub_895 mgl_dispatch_stub_895
#define gl_dispatch_stub_896 mgl_dispatch_stub_896
#define gl_dispatch_stub_897 mgl_dispatch_stub_897
#define gl_dispatch_stub_910 mgl_dispatch_stub_910
#define gl_dispatch_stub_911 mgl_dispatch_stub_911
#define gl_dispatch_stub_912 mgl_dispatch_stub_912
#define gl_dispatch_stub_913 mgl_dispatch_stub_913
#define gl_dispatch_stub_914 mgl_dispatch_stub_914
#define gl_dispatch_stub_915 mgl_dispatch_stub_915
#define gl_dispatch_stub_916 mgl_dispatch_stub_916
#define gl_dispatch_stub_917 mgl_dispatch_stub_917
#define gl_dispatch_stub_918 mgl_dispatch_stub_918
#define gl_dispatch_stub_919 mgl_dispatch_stub_919
#define gl_dispatch_stub_920 mgl_dispatch_stub_920
#define gl_dispatch_stub_921 mgl_dispatch_stub_921
#define gl_dispatch_stub_922 mgl_dispatch_stub_922
#define gl_dispatch_stub_923 mgl_dispatch_stub_923
#define gl_dispatch_stub_924 mgl_dispatch_stub_924
#define gl_dispatch_stub_925 mgl_dispatch_stub_925
#define gl_dispatch_stub_926 mgl_dispatch_stub_926
#define gl_dispatch_stub_927 mgl_dispatch_stub_927
#define gl_dispatch_stub_928 mgl_dispatch_stub_928
#define gl_dispatch_stub_929 mgl_dispatch_stub_929
#define gl_dispatch_stub_930 mgl_dispatch_stub_930
#define gl_dispatch_stub_931 mgl_dispatch_stub_931
#define gl_dispatch_stub_932 mgl_dispatch_stub_932
#define gl_dispatch_stub_933 mgl_dispatch_stub_933
#define gl_dispatch_stub_934 mgl_dispatch_stub_934
#define gl_dispatch_stub_935 mgl_dispatch_stub_935
#define gl_dispatch_stub_936 mgl_dispatch_stub_936
#define gl_dispatch_stub_937 mgl_dispatch_stub_937
#define gl_dispatch_stub_938 mgl_dispatch_stub_938
#define gl_dispatch_stub_939 mgl_dispatch_stub_939
#define gl_dispatch_stub_940 mgl_dispatch_stub_940
#define gl_dispatch_stub_941 mgl_dispatch_stub_941
#define gl_dispatch_stub_942 mgl_dispatch_stub_942
#define gl_dispatch_stub_943 mgl_dispatch_stub_943
#define gl_dispatch_stub_944 mgl_dispatch_stub_944
#define gl_dispatch_stub_945 mgl_dispatch_stub_945
#define gl_dispatch_stub_946 mgl_dispatch_stub_946
#define gl_dispatch_stub_947 mgl_dispatch_stub_947
#define gl_dispatch_stub_948 mgl_dispatch_stub_948
#define gl_dispatch_stub_949 mgl_dispatch_stub_949
#define gl_dispatch_stub_950 mgl_dispatch_stub_950
#define gl_dispatch_stub_951 mgl_dispatch_stub_951
#define gl_dispatch_stub_952 mgl_dispatch_stub_952
#define gl_dispatch_stub_953 mgl_dispatch_stub_953
#define gl_dispatch_stub_954 mgl_dispatch_stub_954
#define gl_dispatch_stub_955 mgl_dispatch_stub_955
#define gl_dispatch_stub_956 mgl_dispatch_stub_956
#define gl_dispatch_stub_957 mgl_dispatch_stub_957
#define gl_dispatch_stub_958 mgl_dispatch_stub_958
#define gl_dispatch_stub_959 mgl_dispatch_stub_959
#define gl_dispatch_stub_960 mgl_dispatch_stub_960
#define gl_dispatch_stub_961 mgl_dispatch_stub_961
#define gl_dispatch_stub_962 mgl_dispatch_stub_962
#define gl_dispatch_stub_963 mgl_dispatch_stub_963
#define gl_dispatch_stub_964 mgl_dispatch_stub_964
#define gl_dispatch_stub_965 mgl_dispatch_stub_965
#define gl_dispatch_stub_966 mgl_dispatch_stub_966
#define gl_dispatch_stub_967 mgl_dispatch_stub_967
#define gl_dispatch_stub_968 mgl_dispatch_stub_968
#define gl_dispatch_stub_969 mgl_dispatch_stub_969
#define gl_dispatch_stub_970 mgl_dispatch_stub_970
#define gl_dispatch_stub_971 mgl_dispatch_stub_971
#define gl_dispatch_stub_972 mgl_dispatch_stub_972
#define gl_dispatch_stub_973 mgl_dispatch_stub_973
#define gl_dispatch_stub_974 mgl_dispatch_stub_974
#define gl_dispatch_stub_975 mgl_dispatch_stub_975
#define gl_dispatch_stub_976 mgl_dispatch_stub_976
#define gl_dispatch_stub_977 mgl_dispatch_stub_977
#define gl_dispatch_stub_978 mgl_dispatch_stub_978
#define gl_dispatch_stub_979 mgl_dispatch_stub_979
#define gl_dispatch_stub_980 mgl_dispatch_stub_980
#define gl_dispatch_stub_981 mgl_dispatch_stub_981
#define gl_dispatch_stub_982 mgl_dispatch_stub_982
#define gl_dispatch_stub_983 mgl_dispatch_stub_983
#define gl_dispatch_stub_984 mgl_dispatch_stub_984
#define gl_dispatch_stub_985 mgl_dispatch_stub_985
#define gl_dispatch_stub_986 mgl_dispatch_stub_986
#define gl_dispatch_stub_987 mgl_dispatch_stub_987
#define gl_dispatch_stub_988 mgl_dispatch_stub_988
#define gl_dispatch_stub_989 mgl_dispatch_stub_989
#define gl_dispatch_stub_990 mgl_dispatch_stub_990
#define gl_dispatch_stub_991 mgl_dispatch_stub_991
#define gl_dispatch_stub_992 mgl_dispatch_stub_992
#define gl_dispatch_stub_993 mgl_dispatch_stub_993
#define gl_dispatch_stub_994 mgl_dispatch_stub_994
#define gl_dispatch_stub_995 mgl_dispatch_stub_995
#define gl_dispatch_stub_996 mgl_dispatch_stub_996
#define gl_dispatch_stub_997 mgl_dispatch_stub_997
#define gl_dispatch_stub_998 mgl_dispatch_stub_998
#define gl_dispatch_stub_999 mgl_dispatch_stub_999
#define gl_dispatch_stub_1000 mgl_dispatch_stub_1000
#define gl_dispatch_stub_1001 mgl_dispatch_stub_1001
#define gl_dispatch_stub_1002 mgl_dispatch_stub_1002
#define gl_dispatch_stub_1003 mgl_dispatch_stub_1003
#define gl_dispatch_stub_1004 mgl_dispatch_stub_1004
#define gl_dispatch_stub_1005 mgl_dispatch_stub_1005
#define gl_dispatch_stub_1006 mgl_dispatch_stub_1006
#define gl_dispatch_stub_1007 mgl_dispatch_stub_1007
#define gl_dispatch_stub_1008 mgl_dispatch_stub_1008
#define gl_dispatch_stub_1009 mgl_dispatch_stub_1009
#define gl_dispatch_stub_1016 mgl_dispatch_stub_1016
#define gl_dispatch_stub_1017 mgl_dispatch_stub_1017
#define gl_dispatch_stub_1018 mgl_dispatch_stub_1018
#define gl_dispatch_stub_1019 mgl_dispatch_stub_1019
#define gl_dispatch_stub_1020 mgl_dispatch_stub_1020
#define gl_dispatch_stub_1021 mgl_dispatch_stub_1021
#define gl_dispatch_stub_1022 mgl_dispatch_stub_1022
#define gl_dispatch_stub_1023 mgl_dispatch_stub_1023
#define gl_dispatch_stub_1024 mgl_dispatch_stub_1024
#define gl_dispatch_stub_1026 mgl_dispatch_stub_1026
#define gl_dispatch_stub_1027 mgl_dispatch_stub_1027
#define gl_dispatch_stub_1028 mgl_dispatch_stub_1028
#define gl_dispatch_stub_1035 mgl_dispatch_stub_1035
#define gl_dispatch_stub_1036 mgl_dispatch_stub_1036
#define gl_dispatch_stub_1037 mgl_dispatch_stub_1037
#define gl_dispatch_stub_1038 mgl_dispatch_stub_1038
#define gl_dispatch_stub_1039 mgl_dispatch_stub_1039
#define gl_dispatch_stub_1040 mgl_dispatch_stub_1040
#define gl_dispatch_stub_1041 mgl_dispatch_stub_1041
#define gl_dispatch_stub_1042 mgl_dispatch_stub_1042
#define gl_dispatch_stub_1043 mgl_dispatch_stub_1043
#define gl_dispatch_stub_1045 mgl_dispatch_stub_1045
#define gl_dispatch_stub_1046 mgl_dispatch_stub_1046
#define gl_dispatch_stub_1047 mgl_dispatch_stub_1047
#define gl_dispatch_stub_1048 mgl_dispatch_stub_1048
#define gl_dispatch_stub_1049 mgl_dispatch_stub_1049
#define gl_dispatch_stub_1050 mgl_dispatch_stub_1050
#define gl_dispatch_stub_1051 mgl_dispatch_stub_1051
#define gl_dispatch_stub_1052 mgl_dispatch_stub_1052
#define gl_dispatch_stub_1053 mgl_dispatch_stub_1053
#define gl_dispatch_stub_1054 mgl_dispatch_stub_1054
#define gl_dispatch_stub_1055 mgl_dispatch_stub_1055
#define gl_dispatch_stub_1056 mgl_dispatch_stub_1056
#define gl_dispatch_stub_1057 mgl_dispatch_stub_1057
#define gl_dispatch_stub_1058 mgl_dispatch_stub_1058
#define gl_dispatch_stub_1059 mgl_dispatch_stub_1059
#define gl_dispatch_stub_1060 mgl_dispatch_stub_1060
#define gl_dispatch_stub_1061 mgl_dispatch_stub_1061
#define gl_dispatch_stub_1062 mgl_dispatch_stub_1062
#define gl_dispatch_stub_1063 mgl_dispatch_stub_1063
#define gl_dispatch_stub_1064 mgl_dispatch_stub_1064
#define gl_dispatch_stub_1065 mgl_dispatch_stub_1065
#define gl_dispatch_stub_1066 mgl_dispatch_stub_1066
#define gl_dispatch_stub_1067 mgl_dispatch_stub_1067
#define gl_dispatch_stub_1068 mgl_dispatch_stub_1068
#define gl_dispatch_stub_1069 mgl_dispatch_stub_1069
#define gl_dispatch_stub_1070 mgl_dispatch_stub_1070
#define gl_dispatch_stub_1071 mgl_dispatch_stub_1071
#define gl_dispatch_stub_1072 mgl_dispatch_stub_1072
#define gl_dispatch_stub_1073 mgl_dispatch_stub_1073
#define gl_dispatch_stub_1074 mgl_dispatch_stub_1074
#define gl_dispatch_stub_1075 mgl_dispatch_stub_1075
#define gl_dispatch_stub_1076 mgl_dispatch_stub_1076
#define gl_dispatch_stub_1077 mgl_dispatch_stub_1077
#define gl_dispatch_stub_1078 mgl_dispatch_stub_1078
#define gl_dispatch_stub_1079 mgl_dispatch_stub_1079
#define gl_dispatch_stub_1080 mgl_dispatch_stub_1080
#define gl_dispatch_stub_1081 mgl_dispatch_stub_1081
#define gl_dispatch_stub_1082 mgl_dispatch_stub_1082
#define gl_dispatch_stub_1083 mgl_dispatch_stub_1083
#define gl_dispatch_stub_1084 mgl_dispatch_stub_1084
#define gl_dispatch_stub_1085 mgl_dispatch_stub_1085
#define gl_dispatch_stub_1086 mgl_dispatch_stub_1086
#define gl_dispatch_stub_1087 mgl_dispatch_stub_1087
#define gl_dispatch_stub_1088 mgl_dispatch_stub_1088
#define gl_dispatch_stub_1089 mgl_dispatch_stub_1089
#define gl_dispatch_stub_1090 mgl_dispatch_stub_1090
#define gl_dispatch_stub_1091 mgl_dispatch_stub_1091
#define gl_dispatch_stub_1092 mgl_dispatch_stub_1092
#define gl_dispatch_stub_1093 mgl_dispatch_stub_1093
#define gl_dispatch_stub_1094 mgl_dispatch_stub_1094
#define gl_dispatch_stub_1096 mgl_dispatch_stub_1096
#define gl_dispatch_stub_1097 mgl_dispatch_stub_1097
#define gl_dispatch_stub_1113 mgl_dispatch_stub_1113
#define gl_dispatch_stub_1114 mgl_dispatch_stub_1114
#define gl_dispatch_stub_1115 mgl_dispatch_stub_1115
#define gl_dispatch_stub_1116 mgl_dispatch_stub_1116
#define gl_dispatch_stub_1117 mgl_dispatch_stub_1117
#define gl_dispatch_stub_1118 mgl_dispatch_stub_1118
#define gl_dispatch_stub_1119 mgl_dispatch_stub_1119
#define gl_dispatch_stub_1120 mgl_dispatch_stub_1120
#define gl_dispatch_stub_1121 mgl_dispatch_stub_1121
#define gl_dispatch_stub_1122 mgl_dispatch_stub_1122
#define gl_dispatch_stub_1123 mgl_dispatch_stub_1123
#define gl_dispatch_stub_1124 mgl_dispatch_stub_1124
#define gl_dispatch_stub_1125 mgl_dispatch_stub_1125
#define gl_dispatch_stub_1126 mgl_dispatch_stub_1126
#define gl_dispatch_stub_1127 mgl_dispatch_stub_1127
#define gl_dispatch_stub_1128 mgl_dispatch_stub_1128
#define gl_dispatch_stub_1129 mgl_dispatch_stub_1129
#define gl_dispatch_stub_1130 mgl_dispatch_stub_1130
#define gl_dispatch_stub_1131 mgl_dispatch_stub_1131
#define gl_dispatch_stub_1132 mgl_dispatch_stub_1132
#define gl_dispatch_stub_1133 mgl_dispatch_stub_1133
#define gl_dispatch_stub_1134 mgl_dispatch_stub_1134
#define gl_dispatch_stub_1135 mgl_dispatch_stub_1135
#define gl_dispatch_stub_1136 mgl_dispatch_stub_1136
#define gl_dispatch_stub_1137 mgl_dispatch_stub_1137
#define gl_dispatch_stub_1138 mgl_dispatch_stub_1138
#define gl_dispatch_stub_1139 mgl_dispatch_stub_1139
#define gl_dispatch_stub_1140 mgl_dispatch_stub_1140
#define gl_dispatch_stub_1141 mgl_dispatch_stub_1141
#define gl_dispatch_stub_1142 mgl_dispatch_stub_1142
#define gl_dispatch_stub_1143 mgl_dispatch_stub_1143
#define gl_dispatch_stub_1144 mgl_dispatch_stub_1144
#define gl_dispatch_stub_1145 mgl_dispatch_stub_1145
#define gl_dispatch_stub_1146 mgl_dispatch_stub_1146
#define gl_dispatch_stub_1147 mgl_dispatch_stub_1147
#define gl_dispatch_stub_1148 mgl_dispatch_stub_1148
#define gl_dispatch_stub_1149 mgl_dispatch_stub_1149
#define gl_dispatch_stub_1150 mgl_dispatch_stub_1150
#define gl_dispatch_stub_1151 mgl_dispatch_stub_1151
#define gl_dispatch_stub_1152 mgl_dispatch_stub_1152
#define gl_dispatch_stub_1153 mgl_dispatch_stub_1153
#define gl_dispatch_stub_1154 mgl_dispatch_stub_1154
#define gl_dispatch_stub_1155 mgl_dispatch_stub_1155
#define gl_dispatch_stub_1156 mgl_dispatch_stub_1156
#define gl_dispatch_stub_1157 mgl_dispatch_stub_1157
#define gl_dispatch_stub_1158 mgl_dispatch_stub_1158
#define gl_dispatch_stub_1159 mgl_dispatch_stub_1159
#define gl_dispatch_stub_1160 mgl_dispatch_stub_1160
#define gl_dispatch_stub_1161 mgl_dispatch_stub_1161
#define gl_dispatch_stub_1162 mgl_dispatch_stub_1162
#define gl_dispatch_stub_1163 mgl_dispatch_stub_1163
#define gl_dispatch_stub_1164 mgl_dispatch_stub_1164
#define gl_dispatch_stub_1165 mgl_dispatch_stub_1165
#define gl_dispatch_stub_1166 mgl_dispatch_stub_1166
#define gl_dispatch_stub_1167 mgl_dispatch_stub_1167
#define gl_dispatch_stub_1168 mgl_dispatch_stub_1168
#define gl_dispatch_stub_1169 mgl_dispatch_stub_1169
#define gl_dispatch_stub_1170 mgl_dispatch_stub_1170
#define gl_dispatch_stub_1171 mgl_dispatch_stub_1171
#define gl_dispatch_stub_1172 mgl_dispatch_stub_1172
#define gl_dispatch_stub_1173 mgl_dispatch_stub_1173
#define gl_dispatch_stub_1174 mgl_dispatch_stub_1174
#define gl_dispatch_stub_1175 mgl_dispatch_stub_1175
#define gl_dispatch_stub_1176 mgl_dispatch_stub_1176
#define gl_dispatch_stub_1177 mgl_dispatch_stub_1177
#define gl_dispatch_stub_1178 mgl_dispatch_stub_1178
#define gl_dispatch_stub_1179 mgl_dispatch_stub_1179
#define gl_dispatch_stub_1180 mgl_dispatch_stub_1180
#define gl_dispatch_stub_1181 mgl_dispatch_stub_1181
#define gl_dispatch_stub_1182 mgl_dispatch_stub_1182
#define gl_dispatch_stub_1183 mgl_dispatch_stub_1183
#define gl_dispatch_stub_1184 mgl_dispatch_stub_1184
#define gl_dispatch_stub_1185 mgl_dispatch_stub_1185
#define gl_dispatch_stub_1186 mgl_dispatch_stub_1186
#define gl_dispatch_stub_1187 mgl_dispatch_stub_1187
#define gl_dispatch_stub_1188 mgl_dispatch_stub_1188
#define gl_dispatch_stub_1189 mgl_dispatch_stub_1189
#define gl_dispatch_stub_1190 mgl_dispatch_stub_1190
#define gl_dispatch_stub_1191 mgl_dispatch_stub_1191
#define gl_dispatch_stub_1192 mgl_dispatch_stub_1192
#define gl_dispatch_stub_1193 mgl_dispatch_stub_1193
#define gl_dispatch_stub_1194 mgl_dispatch_stub_1194
#define gl_dispatch_stub_1195 mgl_dispatch_stub_1195
#define gl_dispatch_stub_1196 mgl_dispatch_stub_1196
#define gl_dispatch_stub_1197 mgl_dispatch_stub_1197
#define gl_dispatch_stub_1198 mgl_dispatch_stub_1198
#define gl_dispatch_stub_1199 mgl_dispatch_stub_1199
#define gl_dispatch_stub_1200 mgl_dispatch_stub_1200
#define gl_dispatch_stub_1201 mgl_dispatch_stub_1201
#define gl_dispatch_stub_1202 mgl_dispatch_stub_1202
#define gl_dispatch_stub_1203 mgl_dispatch_stub_1203
#define gl_dispatch_stub_1204 mgl_dispatch_stub_1204
#define gl_dispatch_stub_1205 mgl_dispatch_stub_1205
#define gl_dispatch_stub_1207 mgl_dispatch_stub_1207
#define gl_dispatch_stub_1208 mgl_dispatch_stub_1208
#define gl_dispatch_stub_1209 mgl_dispatch_stub_1209
#define gl_dispatch_stub_1210 mgl_dispatch_stub_1210
#define gl_dispatch_stub_1213 mgl_dispatch_stub_1213
#define gl_dispatch_stub_1214 mgl_dispatch_stub_1214
#define gl_dispatch_stub_1231 mgl_dispatch_stub_1231
#define gl_dispatch_stub_1232 mgl_dispatch_stub_1232
#define gl_dispatch_stub_1233 mgl_dispatch_stub_1233
#define gl_dispatch_stub_1234 mgl_dispatch_stub_1234
#define gl_dispatch_stub_1235 mgl_dispatch_stub_1235
#define gl_dispatch_stub_1236 mgl_dispatch_stub_1236
#define gl_dispatch_stub_1237 mgl_dispatch_stub_1237
#define gl_dispatch_stub_1238 mgl_dispatch_stub_1238
#define gl_dispatch_stub_1239 mgl_dispatch_stub_1239
#define gl_dispatch_stub_1240 mgl_dispatch_stub_1240
#define gl_dispatch_stub_1241 mgl_dispatch_stub_1241
#define gl_dispatch_stub_1242 mgl_dispatch_stub_1242
#define gl_dispatch_stub_1243 mgl_dispatch_stub_1243
#define gl_dispatch_stub_1244 mgl_dispatch_stub_1244
#define gl_dispatch_stub_1245 mgl_dispatch_stub_1245
#define gl_dispatch_stub_1246 mgl_dispatch_stub_1246
#define gl_dispatch_stub_1247 mgl_dispatch_stub_1247
#define gl_dispatch_stub_1248 mgl_dispatch_stub_1248
#define gl_dispatch_stub_1250 mgl_dispatch_stub_1250
#define gl_dispatch_stub_1251 mgl_dispatch_stub_1251
#define gl_dispatch_stub_1252 mgl_dispatch_stub_1252
#define gl_dispatch_stub_1253 mgl_dispatch_stub_1253
#define gl_dispatch_stub_1254 mgl_dispatch_stub_1254
#define gl_dispatch_stub_1255 mgl_dispatch_stub_1255
#define gl_dispatch_stub_1256 mgl_dispatch_stub_1256
#define gl_dispatch_stub_1257 mgl_dispatch_stub_1257
#define gl_dispatch_stub_1258 mgl_dispatch_stub_1258
#define gl_dispatch_stub_1259 mgl_dispatch_stub_1259
#define gl_dispatch_stub_1260 mgl_dispatch_stub_1260
#define gl_dispatch_stub_1261 mgl_dispatch_stub_1261
#define gl_dispatch_stub_1262 mgl_dispatch_stub_1262
#define gl_dispatch_stub_1263 mgl_dispatch_stub_1263
#define gl_dispatch_stub_1264 mgl_dispatch_stub_1264
#define gl_dispatch_stub_1265 mgl_dispatch_stub_1265
#define gl_dispatch_stub_1266 mgl_dispatch_stub_1266
#define gl_dispatch_stub_1267 mgl_dispatch_stub_1267
#define gl_dispatch_stub_1268 mgl_dispatch_stub_1268
#define gl_dispatch_stub_1269 mgl_dispatch_stub_1269
#define gl_dispatch_stub_1270 mgl_dispatch_stub_1270
#define gl_dispatch_stub_1271 mgl_dispatch_stub_1271
#define gl_dispatch_stub_1272 mgl_dispatch_stub_1272
#define gl_dispatch_stub_1273 mgl_dispatch_stub_1273
#define gl_dispatch_stub_1274 mgl_dispatch_stub_1274
#define gl_dispatch_stub_1275 mgl_dispatch_stub_1275
#endif /* USE_MGL_NAMESPACE */


#if defined(NEED_FUNCTION_POINTER) || defined(GLX_INDIRECT_RENDERING)
void GLAPIENTRY gl_dispatch_stub_343(GLenum target, GLenum format, GLenum type, GLvoid * table);
void GLAPIENTRY gl_dispatch_stub_344(GLenum target, GLenum pname, GLfloat * params);
void GLAPIENTRY gl_dispatch_stub_345(GLenum target, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_356(GLenum target, GLenum format, GLenum type, GLvoid * image);
void GLAPIENTRY gl_dispatch_stub_357(GLenum target, GLenum pname, GLfloat * params);
void GLAPIENTRY gl_dispatch_stub_358(GLenum target, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_359(GLenum target, GLenum format, GLenum type, GLvoid * row, GLvoid * column, GLvoid * span);
void GLAPIENTRY gl_dispatch_stub_361(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values);
void GLAPIENTRY gl_dispatch_stub_362(GLenum target, GLenum pname, GLfloat * params);
void GLAPIENTRY gl_dispatch_stub_363(GLenum target, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_364(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values);
void GLAPIENTRY gl_dispatch_stub_365(GLenum target, GLenum pname, GLfloat * params);
void GLAPIENTRY gl_dispatch_stub_366(GLenum target, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_621(GLbitfield barriers);
void GLAPIENTRY gl_dispatch_stub_731(GLuint id, GLenum pname, GLint64 * params);
void GLAPIENTRY gl_dispatch_stub_732(GLuint id, GLenum pname, GLuint64 * params);
void GLAPIENTRY gl_dispatch_stub_733(GLuint id, GLenum target);
void GLAPIENTRY gl_dispatch_stub_774(GLuint program, GLint location, GLdouble * params);
void GLAPIENTRY gl_dispatch_stub_775(GLint location, GLdouble x);
void GLAPIENTRY gl_dispatch_stub_776(GLint location, GLsizei count, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_777(GLint location, GLdouble x, GLdouble y);
void GLAPIENTRY gl_dispatch_stub_778(GLint location, GLsizei count, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_779(GLint location, GLdouble x, GLdouble y, GLdouble z);
void GLAPIENTRY gl_dispatch_stub_780(GLint location, GLsizei count, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_781(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void GLAPIENTRY gl_dispatch_stub_782(GLint location, GLsizei count, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_783(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_784(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_785(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_786(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_787(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_788(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_789(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_790(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_791(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_792(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei * length, GLchar * name);
void GLAPIENTRY gl_dispatch_stub_793(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei * length, GLchar * name);
void GLAPIENTRY gl_dispatch_stub_794(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint * values);
void GLAPIENTRY gl_dispatch_stub_795(GLuint program, GLenum shadertype, GLenum pname, GLint * values);
GLuint GLAPIENTRY gl_dispatch_stub_796(GLuint program, GLenum shadertype, const GLchar * name);
GLint GLAPIENTRY gl_dispatch_stub_797(GLuint program, GLenum shadertype, const GLchar * name);
void GLAPIENTRY gl_dispatch_stub_798(GLenum shadertype, GLint location, GLuint * params);
void GLAPIENTRY gl_dispatch_stub_799(GLenum shadertype, GLsizei count, const GLuint * indices);
void GLAPIENTRY gl_dispatch_stub_800(GLenum pname, const GLfloat * values);
void GLAPIENTRY gl_dispatch_stub_801(GLenum pname, GLint value);
void GLAPIENTRY gl_dispatch_stub_821(GLuint index, GLenum pname, GLdouble * params);
void GLAPIENTRY gl_dispatch_stub_822(GLuint index, GLdouble x);
void GLAPIENTRY gl_dispatch_stub_823(GLuint index, const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_824(GLuint index, GLdouble x, GLdouble y);
void GLAPIENTRY gl_dispatch_stub_825(GLuint index, const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_826(GLuint index, GLdouble x, GLdouble y, GLdouble z);
void GLAPIENTRY gl_dispatch_stub_827(GLuint index, const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_828(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void GLAPIENTRY gl_dispatch_stub_829(GLuint index, const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_830(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
void GLAPIENTRY gl_dispatch_stub_866(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint * params);
void GLAPIENTRY gl_dispatch_stub_888(GLenum target, GLenum pname, GLint param);
void GLAPIENTRY gl_dispatch_stub_889(GLenum target, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_892(GLuint program, GLenum programInterface, GLenum pname, GLint * params);
GLuint GLAPIENTRY gl_dispatch_stub_893(GLuint program, GLenum programInterface, const GLchar * name);
GLint GLAPIENTRY gl_dispatch_stub_894(GLuint program, GLenum programInterface, const GLchar * name);
GLint GLAPIENTRY gl_dispatch_stub_895(GLuint program, GLenum programInterface, const GLchar * name);
void GLAPIENTRY gl_dispatch_stub_896(GLuint program, GLenum programInterface, GLuint index, GLsizei  bufSize, GLsizei * length, GLchar * name);
void GLAPIENTRY gl_dispatch_stub_897(GLuint program, GLenum programInterface, GLuint index, GLsizei  propCount, const GLenum * props, GLsizei  bufSize, GLsizei * length, GLint * params);
void GLAPIENTRY gl_dispatch_stub_910(GLenum origin, GLenum depth);
void GLAPIENTRY gl_dispatch_stub_911(GLuint unit, GLuint texture);
void GLAPIENTRY gl_dispatch_stub_912(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
GLenum GLAPIENTRY gl_dispatch_stub_913(GLuint framebuffer, GLenum target);
void GLAPIENTRY gl_dispatch_stub_914(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const GLvoid * data);
void GLAPIENTRY gl_dispatch_stub_915(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const GLvoid * data);
void GLAPIENTRY gl_dispatch_stub_916(GLuint framebuffer, GLenum buffer, GLfloat depth, GLint stencil);
void GLAPIENTRY gl_dispatch_stub_917(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_918(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint * value);
void GLAPIENTRY gl_dispatch_stub_919(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint * value);
void GLAPIENTRY gl_dispatch_stub_920(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid * data);
void GLAPIENTRY gl_dispatch_stub_921(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data);
void GLAPIENTRY gl_dispatch_stub_922(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data);
void GLAPIENTRY gl_dispatch_stub_923(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
void GLAPIENTRY gl_dispatch_stub_924(GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
void GLAPIENTRY gl_dispatch_stub_925(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void GLAPIENTRY gl_dispatch_stub_926(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void GLAPIENTRY gl_dispatch_stub_927(GLsizei n, GLuint * buffers);
void GLAPIENTRY gl_dispatch_stub_928(GLsizei n, GLuint * framebuffers);
void GLAPIENTRY gl_dispatch_stub_929(GLsizei n, GLuint * pipelines);
void GLAPIENTRY gl_dispatch_stub_930(GLenum target, GLsizei n, GLuint * ids);
void GLAPIENTRY gl_dispatch_stub_931(GLsizei n, GLuint * renderbuffers);
void GLAPIENTRY gl_dispatch_stub_932(GLsizei n, GLuint * samplers);
void GLAPIENTRY gl_dispatch_stub_933(GLenum target, GLsizei n, GLuint * textures);
void GLAPIENTRY gl_dispatch_stub_934(GLsizei n, GLuint * ids);
void GLAPIENTRY gl_dispatch_stub_935(GLsizei n, GLuint * arrays);
void GLAPIENTRY gl_dispatch_stub_936(GLuint vaobj, GLuint index);
void GLAPIENTRY gl_dispatch_stub_937(GLuint vaobj, GLuint index);
void GLAPIENTRY gl_dispatch_stub_938(GLuint buffer, GLintptr offset, GLsizeiptr length);
void GLAPIENTRY gl_dispatch_stub_939(GLuint texture);
void GLAPIENTRY gl_dispatch_stub_940(GLuint texture, GLint level, GLsizei bufSize, GLvoid * pixels);
void GLAPIENTRY gl_dispatch_stub_941(GLuint buffer, GLenum pname, GLint64 * params);
void GLAPIENTRY gl_dispatch_stub_942(GLuint buffer, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_943(GLuint buffer, GLenum pname, GLvoid ** params);
void GLAPIENTRY gl_dispatch_stub_944(GLuint buffer, GLintptr offset, GLsizeiptr size, GLvoid * data);
void GLAPIENTRY gl_dispatch_stub_945(GLuint framebuffer, GLenum attachment, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_946(GLuint framebuffer, GLenum pname, GLint * param);
void GLAPIENTRY gl_dispatch_stub_947(GLuint renderbuffer, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_948(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
void GLAPIENTRY gl_dispatch_stub_949(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
void GLAPIENTRY gl_dispatch_stub_950(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
void GLAPIENTRY gl_dispatch_stub_951(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
void GLAPIENTRY gl_dispatch_stub_952(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, GLvoid * pixels);
void GLAPIENTRY gl_dispatch_stub_953(GLuint texture, GLint level, GLenum pname, GLfloat * params);
void GLAPIENTRY gl_dispatch_stub_954(GLuint texture, GLint level, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_955(GLuint texture, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_956(GLuint texture, GLenum pname, GLuint * params);
void GLAPIENTRY gl_dispatch_stub_957(GLuint texture, GLenum pname, GLfloat * params);
void GLAPIENTRY gl_dispatch_stub_958(GLuint texture, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_959(GLuint xfb, GLenum pname, GLuint index, GLint64 * param);
void GLAPIENTRY gl_dispatch_stub_960(GLuint xfb, GLenum pname, GLuint index, GLint * param);
void GLAPIENTRY gl_dispatch_stub_961(GLuint xfb, GLenum pname, GLint * param);
void GLAPIENTRY gl_dispatch_stub_962(GLuint vaobj, GLuint index, GLenum pname, GLint64 * param);
void GLAPIENTRY gl_dispatch_stub_963(GLuint vaobj, GLuint index, GLenum pname, GLint * param);
void GLAPIENTRY gl_dispatch_stub_964(GLuint vaobj, GLenum pname, GLint * param);
void GLAPIENTRY gl_dispatch_stub_965(GLuint framebuffer, GLsizei numAttachments, const GLenum * attachments);
void GLAPIENTRY gl_dispatch_stub_966(GLuint framebuffer, GLsizei numAttachments, const GLenum * attachments, GLint x, GLint y, GLsizei width, GLsizei height);
GLvoid * GLAPIENTRY gl_dispatch_stub_967(GLuint buffer, GLenum access);
GLvoid * GLAPIENTRY gl_dispatch_stub_968(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
void GLAPIENTRY gl_dispatch_stub_969(GLuint buffer, GLsizeiptr size, const GLvoid * data, GLenum usage);
void GLAPIENTRY gl_dispatch_stub_970(GLuint buffer, GLsizeiptr size, const GLvoid * data, GLbitfield flags);
void GLAPIENTRY gl_dispatch_stub_971(GLuint buffer, GLintptr offset, GLsizeiptr size, const GLvoid * data);
void GLAPIENTRY gl_dispatch_stub_972(GLuint framebuffer, GLenum buf);
void GLAPIENTRY gl_dispatch_stub_973(GLuint framebuffer, GLsizei n, const GLenum * bufs);
void GLAPIENTRY gl_dispatch_stub_974(GLuint framebuffer, GLenum pname, GLint param);
void GLAPIENTRY gl_dispatch_stub_975(GLuint framebuffer, GLenum buf);
void GLAPIENTRY gl_dispatch_stub_976(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
void GLAPIENTRY gl_dispatch_stub_977(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
void GLAPIENTRY gl_dispatch_stub_978(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
void GLAPIENTRY gl_dispatch_stub_979(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
void GLAPIENTRY gl_dispatch_stub_980(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
void GLAPIENTRY gl_dispatch_stub_981(GLuint texture, GLenum internalformat, GLuint buffer);
void GLAPIENTRY gl_dispatch_stub_982(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
void GLAPIENTRY gl_dispatch_stub_983(GLuint texture, GLenum pname, const GLint * params);
void GLAPIENTRY gl_dispatch_stub_984(GLuint texture, GLenum pname, const GLuint * params);
void GLAPIENTRY gl_dispatch_stub_985(GLuint texture, GLenum pname, GLfloat param);
void GLAPIENTRY gl_dispatch_stub_986(GLuint texture, GLenum pname, const GLfloat * param);
void GLAPIENTRY gl_dispatch_stub_987(GLuint texture, GLenum pname, GLint param);
void GLAPIENTRY gl_dispatch_stub_988(GLuint texture, GLenum pname, const GLint * param);
void GLAPIENTRY gl_dispatch_stub_989(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);
void GLAPIENTRY gl_dispatch_stub_990(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
void GLAPIENTRY gl_dispatch_stub_991(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
void GLAPIENTRY gl_dispatch_stub_992(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
void GLAPIENTRY gl_dispatch_stub_993(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
void GLAPIENTRY gl_dispatch_stub_994(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid * pixels);
void GLAPIENTRY gl_dispatch_stub_995(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels);
void GLAPIENTRY gl_dispatch_stub_996(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * pixels);
void GLAPIENTRY gl_dispatch_stub_997(GLuint xfb, GLuint index, GLuint buffer);
void GLAPIENTRY gl_dispatch_stub_998(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
GLboolean GLAPIENTRY gl_dispatch_stub_999(GLuint buffer);
void GLAPIENTRY gl_dispatch_stub_1000(GLuint vaobj, GLuint attribindex, GLuint bindingindex);
void GLAPIENTRY gl_dispatch_stub_1001(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
void GLAPIENTRY gl_dispatch_stub_1002(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
void GLAPIENTRY gl_dispatch_stub_1003(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
void GLAPIENTRY gl_dispatch_stub_1004(GLuint vaobj, GLuint bindingindex, GLuint divisor);
void GLAPIENTRY gl_dispatch_stub_1005(GLuint vaobj, GLuint buffer);
void GLAPIENTRY gl_dispatch_stub_1006(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
void GLAPIENTRY gl_dispatch_stub_1007(GLuint vaobj, GLuint first, GLsizei count, const GLuint * buffers, const GLintptr * offsets, const GLsizei * strides);
void GLAPIENTRY gl_dispatch_stub_1008(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei bufSize, GLvoid * pixels);
void GLAPIENTRY gl_dispatch_stub_1009(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, GLvoid * pixels);
void GLAPIENTRY gl_dispatch_stub_1016(GLfloat factor, GLfloat bias);
void GLAPIENTRY gl_dispatch_stub_1017(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height);
void GLAPIENTRY gl_dispatch_stub_1018(const GLfloat * coords);
void GLAPIENTRY gl_dispatch_stub_1019(GLint x, GLint y, GLint z, GLint width, GLint height);
void GLAPIENTRY gl_dispatch_stub_1020(const GLint * coords);
void GLAPIENTRY gl_dispatch_stub_1021(GLshort x, GLshort y, GLshort z, GLshort width, GLshort height);
void GLAPIENTRY gl_dispatch_stub_1022(const GLshort * coords);
void GLAPIENTRY gl_dispatch_stub_1023(GLfixed x, GLfixed y, GLfixed z, GLfixed width, GLfixed height);
void GLAPIENTRY gl_dispatch_stub_1024(const GLfixed * coords);
GLbitfield GLAPIENTRY gl_dispatch_stub_1026(GLfixed * mantissa, GLint * exponent);
void GLAPIENTRY gl_dispatch_stub_1027(GLclampf value, GLboolean invert);
void GLAPIENTRY gl_dispatch_stub_1028(GLenum pattern);
void GLAPIENTRY gl_dispatch_stub_1035(GLenum target, GLsizei numAttachments, const GLenum * attachments);
void GLAPIENTRY gl_dispatch_stub_1036(GLuint pipeline, GLuint program);
void GLAPIENTRY gl_dispatch_stub_1037(GLuint pipeline);
GLuint GLAPIENTRY gl_dispatch_stub_1038(GLenum type, GLsizei count, const GLchar * const * strings);
void GLAPIENTRY gl_dispatch_stub_1039(GLsizei n, const GLuint * pipelines);
void GLAPIENTRY gl_dispatch_stub_1040(GLsizei n, GLuint * pipelines);
void GLAPIENTRY gl_dispatch_stub_1041(GLuint pipeline, GLsizei bufSize, GLsizei * length, GLchar * infoLog);
void GLAPIENTRY gl_dispatch_stub_1042(GLuint pipeline, GLenum pname, GLint * params);
GLboolean GLAPIENTRY gl_dispatch_stub_1043(GLuint pipeline);
void GLAPIENTRY gl_dispatch_stub_1045(GLuint program, GLint location, GLdouble x);
void GLAPIENTRY gl_dispatch_stub_1046(GLuint program, GLint location, GLsizei count, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_1047(GLuint program, GLint location, GLfloat x);
void GLAPIENTRY gl_dispatch_stub_1048(GLuint program, GLint location, GLsizei count, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1049(GLuint program, GLint location, GLint x);
void GLAPIENTRY gl_dispatch_stub_1050(GLuint program, GLint location, GLsizei count, const GLint * value);
void GLAPIENTRY gl_dispatch_stub_1051(GLuint program, GLint location, GLuint x);
void GLAPIENTRY gl_dispatch_stub_1052(GLuint program, GLint location, GLsizei count, const GLuint * value);
void GLAPIENTRY gl_dispatch_stub_1053(GLuint program, GLint location, GLdouble x, GLdouble y);
void GLAPIENTRY gl_dispatch_stub_1054(GLuint program, GLint location, GLsizei count, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_1055(GLuint program, GLint location, GLfloat x, GLfloat y);
void GLAPIENTRY gl_dispatch_stub_1056(GLuint program, GLint location, GLsizei count, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1057(GLuint program, GLint location, GLint x, GLint y);
void GLAPIENTRY gl_dispatch_stub_1058(GLuint program, GLint location, GLsizei count, const GLint * value);
void GLAPIENTRY gl_dispatch_stub_1059(GLuint program, GLint location, GLuint x, GLuint y);
void GLAPIENTRY gl_dispatch_stub_1060(GLuint program, GLint location, GLsizei count, const GLuint * value);
void GLAPIENTRY gl_dispatch_stub_1061(GLuint program, GLint location, GLdouble x, GLdouble y, GLdouble z);
void GLAPIENTRY gl_dispatch_stub_1062(GLuint program, GLint location, GLsizei count, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_1063(GLuint program, GLint location, GLfloat x, GLfloat y, GLfloat z);
void GLAPIENTRY gl_dispatch_stub_1064(GLuint program, GLint location, GLsizei count, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1065(GLuint program, GLint location, GLint x, GLint y, GLint z);
void GLAPIENTRY gl_dispatch_stub_1066(GLuint program, GLint location, GLsizei count, const GLint * value);
void GLAPIENTRY gl_dispatch_stub_1067(GLuint program, GLint location, GLuint x, GLuint y, GLuint z);
void GLAPIENTRY gl_dispatch_stub_1068(GLuint program, GLint location, GLsizei count, const GLuint * value);
void GLAPIENTRY gl_dispatch_stub_1069(GLuint program, GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void GLAPIENTRY gl_dispatch_stub_1070(GLuint program, GLint location, GLsizei count, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_1071(GLuint program, GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void GLAPIENTRY gl_dispatch_stub_1072(GLuint program, GLint location, GLsizei count, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1073(GLuint program, GLint location, GLint x, GLint y, GLint z, GLint w);
void GLAPIENTRY gl_dispatch_stub_1074(GLuint program, GLint location, GLsizei count, const GLint * value);
void GLAPIENTRY gl_dispatch_stub_1075(GLuint program, GLint location, GLuint x, GLuint y, GLuint z, GLuint w);
void GLAPIENTRY gl_dispatch_stub_1076(GLuint program, GLint location, GLsizei count, const GLuint * value);
void GLAPIENTRY gl_dispatch_stub_1077(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_1078(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1079(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_1080(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1081(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_1082(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1083(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_1084(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1085(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_1086(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1087(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_1088(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1089(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_1090(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1091(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_1092(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1093(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
void GLAPIENTRY gl_dispatch_stub_1094(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1096(GLuint pipeline, GLbitfield stages, GLuint program);
void GLAPIENTRY gl_dispatch_stub_1097(GLuint pipeline);
void GLAPIENTRY gl_dispatch_stub_1113(void);
void GLAPIENTRY gl_dispatch_stub_1114(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void GLAPIENTRY gl_dispatch_stub_1115(const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_1116(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void GLAPIENTRY gl_dispatch_stub_1117(const GLfloat * v);
void GLAPIENTRY gl_dispatch_stub_1118(GLint x, GLint y, GLint z, GLint w);
void GLAPIENTRY gl_dispatch_stub_1119(const GLint * v);
void GLAPIENTRY gl_dispatch_stub_1120(GLshort x, GLshort y, GLshort z, GLshort w);
void GLAPIENTRY gl_dispatch_stub_1121(const GLshort * v);
void GLAPIENTRY gl_dispatch_stub_1122(const GLenum * mode, const GLint * first, const GLsizei * count, GLsizei primcount, GLint modestride);
void GLAPIENTRY gl_dispatch_stub_1123(const GLenum * mode, const GLsizei * count, GLenum type, const GLvoid * const * indices, GLsizei primcount, GLint modestride);
GLboolean GLAPIENTRY gl_dispatch_stub_1124(GLsizei n, const GLuint * ids, GLboolean * residences);
void GLAPIENTRY gl_dispatch_stub_1125(GLenum target, GLuint id, const GLfloat * params);
void GLAPIENTRY gl_dispatch_stub_1126(GLenum target, GLuint index, GLenum pname, GLdouble * params);
void GLAPIENTRY gl_dispatch_stub_1127(GLenum target, GLuint index, GLenum pname, GLfloat * params);
void GLAPIENTRY gl_dispatch_stub_1128(GLuint id, GLenum pname, GLubyte * program);
void GLAPIENTRY gl_dispatch_stub_1129(GLuint id, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_1130(GLenum target, GLuint address, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_1131(GLuint index, GLenum pname, GLdouble * params);
void GLAPIENTRY gl_dispatch_stub_1132(GLuint index, GLenum pname, GLfloat * params);
void GLAPIENTRY gl_dispatch_stub_1133(GLuint index, GLenum pname, GLint * params);
void GLAPIENTRY gl_dispatch_stub_1134(GLenum target, GLuint id, GLsizei len, const GLubyte * program);
void GLAPIENTRY gl_dispatch_stub_1135(GLenum target, GLuint index, GLsizei num, const GLdouble * params);
void GLAPIENTRY gl_dispatch_stub_1136(GLenum target, GLuint index, GLsizei num, const GLfloat * params);
void GLAPIENTRY gl_dispatch_stub_1137(GLsizei n, const GLuint * ids);
void GLAPIENTRY gl_dispatch_stub_1138(GLenum target, GLuint address, GLenum matrix, GLenum transform);
void GLAPIENTRY gl_dispatch_stub_1139(GLuint index, GLdouble x);
void GLAPIENTRY gl_dispatch_stub_1140(GLuint index, const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_1141(GLuint index, GLfloat x);
void GLAPIENTRY gl_dispatch_stub_1142(GLuint index, const GLfloat * v);
void GLAPIENTRY gl_dispatch_stub_1143(GLuint index, GLshort x);
void GLAPIENTRY gl_dispatch_stub_1144(GLuint index, const GLshort * v);
void GLAPIENTRY gl_dispatch_stub_1145(GLuint index, GLdouble x, GLdouble y);
void GLAPIENTRY gl_dispatch_stub_1146(GLuint index, const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_1147(GLuint index, GLfloat x, GLfloat y);
void GLAPIENTRY gl_dispatch_stub_1148(GLuint index, const GLfloat * v);
void GLAPIENTRY gl_dispatch_stub_1149(GLuint index, GLshort x, GLshort y);
void GLAPIENTRY gl_dispatch_stub_1150(GLuint index, const GLshort * v);
void GLAPIENTRY gl_dispatch_stub_1151(GLuint index, GLdouble x, GLdouble y, GLdouble z);
void GLAPIENTRY gl_dispatch_stub_1152(GLuint index, const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_1153(GLuint index, GLfloat x, GLfloat y, GLfloat z);
void GLAPIENTRY gl_dispatch_stub_1154(GLuint index, const GLfloat * v);
void GLAPIENTRY gl_dispatch_stub_1155(GLuint index, GLshort x, GLshort y, GLshort z);
void GLAPIENTRY gl_dispatch_stub_1156(GLuint index, const GLshort * v);
void GLAPIENTRY gl_dispatch_stub_1157(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void GLAPIENTRY gl_dispatch_stub_1158(GLuint index, const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_1159(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void GLAPIENTRY gl_dispatch_stub_1160(GLuint index, const GLfloat * v);
void GLAPIENTRY gl_dispatch_stub_1161(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
void GLAPIENTRY gl_dispatch_stub_1162(GLuint index, const GLshort * v);
void GLAPIENTRY gl_dispatch_stub_1163(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
void GLAPIENTRY gl_dispatch_stub_1164(GLuint index, const GLubyte * v);
void GLAPIENTRY gl_dispatch_stub_1165(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
void GLAPIENTRY gl_dispatch_stub_1166(GLuint index, GLsizei n, const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_1167(GLuint index, GLsizei n, const GLfloat * v);
void GLAPIENTRY gl_dispatch_stub_1168(GLuint index, GLsizei n, const GLshort * v);
void GLAPIENTRY gl_dispatch_stub_1169(GLuint index, GLsizei n, const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_1170(GLuint index, GLsizei n, const GLfloat * v);
void GLAPIENTRY gl_dispatch_stub_1171(GLuint index, GLsizei n, const GLshort * v);
void GLAPIENTRY gl_dispatch_stub_1172(GLuint index, GLsizei n, const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_1173(GLuint index, GLsizei n, const GLfloat * v);
void GLAPIENTRY gl_dispatch_stub_1174(GLuint index, GLsizei n, const GLshort * v);
void GLAPIENTRY gl_dispatch_stub_1175(GLuint index, GLsizei n, const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_1176(GLuint index, GLsizei n, const GLfloat * v);
void GLAPIENTRY gl_dispatch_stub_1177(GLuint index, GLsizei n, const GLshort * v);
void GLAPIENTRY gl_dispatch_stub_1178(GLuint index, GLsizei n, const GLubyte * v);
void GLAPIENTRY gl_dispatch_stub_1179(GLenum pname, GLfloat * param);
void GLAPIENTRY gl_dispatch_stub_1180(GLenum pname, GLint * param);
void GLAPIENTRY gl_dispatch_stub_1181(GLenum pname, const GLfloat * param);
void GLAPIENTRY gl_dispatch_stub_1182(GLenum pname, const GLint * param);
void GLAPIENTRY gl_dispatch_stub_1183(GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod);
void GLAPIENTRY gl_dispatch_stub_1184(GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod);
void GLAPIENTRY gl_dispatch_stub_1185(GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod);
void GLAPIENTRY gl_dispatch_stub_1186(void);
void GLAPIENTRY gl_dispatch_stub_1187(GLuint id);
void GLAPIENTRY gl_dispatch_stub_1188(GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod);
void GLAPIENTRY gl_dispatch_stub_1189(GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod);
void GLAPIENTRY gl_dispatch_stub_1190(GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod);
void GLAPIENTRY gl_dispatch_stub_1191(GLuint id);
void GLAPIENTRY gl_dispatch_stub_1192(void);
GLuint GLAPIENTRY gl_dispatch_stub_1193(GLuint range);
void GLAPIENTRY gl_dispatch_stub_1194(GLuint dst, GLuint coord, GLenum swizzle);
void GLAPIENTRY gl_dispatch_stub_1195(GLuint dst, GLuint interp, GLenum swizzle);
void GLAPIENTRY gl_dispatch_stub_1196(GLuint dst, const GLfloat * value);
void GLAPIENTRY gl_dispatch_stub_1197(GLenum face);
void GLAPIENTRY gl_dispatch_stub_1198(GLuint array);
void GLAPIENTRY gl_dispatch_stub_1199(GLsizei n, GLuint * arrays);
void GLAPIENTRY gl_dispatch_stub_1200(GLuint id, GLsizei len, const GLubyte * name, GLdouble * params);
void GLAPIENTRY gl_dispatch_stub_1201(GLuint id, GLsizei len, const GLubyte * name, GLfloat * params);
void GLAPIENTRY gl_dispatch_stub_1202(GLuint id, GLsizei len, const GLubyte * name, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void GLAPIENTRY gl_dispatch_stub_1203(GLuint id, GLsizei len, const GLubyte * name, const GLdouble * v);
void GLAPIENTRY gl_dispatch_stub_1204(GLuint id, GLsizei len, const GLubyte * name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void GLAPIENTRY gl_dispatch_stub_1205(GLuint id, GLsizei len, const GLubyte * name, const GLfloat * v);
void GLAPIENTRY gl_dispatch_stub_1207(GLenum coord, GLenum pname, GLfixed * params);
void GLAPIENTRY gl_dispatch_stub_1208(GLenum coord, GLenum pname, GLint param);
void GLAPIENTRY gl_dispatch_stub_1209(GLenum coord, GLenum pname, const GLfixed * params);
void GLAPIENTRY gl_dispatch_stub_1210(GLclampd zmin, GLclampd zmax);
void GLAPIENTRY gl_dispatch_stub_1213(GLenum target, GLenum pname, GLint param);
void GLAPIENTRY gl_dispatch_stub_1214(GLenum target, GLintptr offset, GLsizeiptr size);
void GLAPIENTRY gl_dispatch_stub_1231(GLenum target, GLuint index, GLuint buffer, GLintptr offset);
void GLAPIENTRY gl_dispatch_stub_1232(GLuint monitor);
void GLAPIENTRY gl_dispatch_stub_1233(GLsizei n, GLuint * monitors);
void GLAPIENTRY gl_dispatch_stub_1234(GLuint monitor);
void GLAPIENTRY gl_dispatch_stub_1235(GLsizei n, GLuint * monitors);
void GLAPIENTRY gl_dispatch_stub_1236(GLuint monitor, GLenum pname, GLsizei dataSize, GLuint * data, GLint * bytesWritten);
void GLAPIENTRY gl_dispatch_stub_1237(GLuint group, GLuint counter, GLenum pname, GLvoid * data);
void GLAPIENTRY gl_dispatch_stub_1238(GLuint group, GLuint counter, GLsizei bufSize, GLsizei * length, GLchar * counterString);
void GLAPIENTRY gl_dispatch_stub_1239(GLuint group, GLint * numCounters, GLint * maxActiveCounters, GLsizei countersSize, GLuint * counters);
void GLAPIENTRY gl_dispatch_stub_1240(GLuint group, GLsizei bufSize, GLsizei * length, GLchar * groupString);
void GLAPIENTRY gl_dispatch_stub_1241(GLint * numGroups, GLsizei groupsSize, GLuint * groups);
void GLAPIENTRY gl_dispatch_stub_1242(GLuint monitor, GLboolean enable, GLuint group, GLint numCounters, GLuint * counterList);
void GLAPIENTRY gl_dispatch_stub_1243(GLenum objectType, GLuint name, GLenum pname, GLint * value);
GLenum GLAPIENTRY gl_dispatch_stub_1244(GLenum objectType, GLuint name, GLenum option);
GLenum GLAPIENTRY gl_dispatch_stub_1245(GLenum objectType, GLuint name, GLenum option);
void GLAPIENTRY gl_dispatch_stub_1246(GLuint program);
GLuint GLAPIENTRY gl_dispatch_stub_1247(GLenum type, const GLchar * string);
void GLAPIENTRY gl_dispatch_stub_1248(GLenum type, GLuint program);
void GLAPIENTRY gl_dispatch_stub_1250(void);
void GLAPIENTRY gl_dispatch_stub_1251(GLintptr surface, GLenum pname, GLsizei bufSize, GLsizei * length, GLint * values);
void GLAPIENTRY gl_dispatch_stub_1252(const GLvoid * vdpDevice, const GLvoid * getProcAddress);
GLboolean GLAPIENTRY gl_dispatch_stub_1253(GLintptr surface);
void GLAPIENTRY gl_dispatch_stub_1254(GLsizei numSurfaces, const GLintptr * surfaces);
GLintptr GLAPIENTRY gl_dispatch_stub_1255(const GLvoid * vdpSurface, GLenum target, GLsizei numTextureNames, const GLuint * textureNames);
GLintptr GLAPIENTRY gl_dispatch_stub_1256(const GLvoid * vdpSurface, GLenum target, GLsizei numTextureNames, const GLuint * textureNames);
void GLAPIENTRY gl_dispatch_stub_1257(GLintptr surface, GLenum access);
void GLAPIENTRY gl_dispatch_stub_1258(GLsizei numSurfaces, const GLintptr * surfaces);
void GLAPIENTRY gl_dispatch_stub_1259(GLintptr surface);
void GLAPIENTRY gl_dispatch_stub_1260(GLuint queryHandle);
void GLAPIENTRY gl_dispatch_stub_1261(GLuint queryId, GLuint * queryHandle);
void GLAPIENTRY gl_dispatch_stub_1262(GLuint queryHandle);
void GLAPIENTRY gl_dispatch_stub_1263(GLuint queryHandle);
void GLAPIENTRY gl_dispatch_stub_1264(GLuint * queryId);
void GLAPIENTRY gl_dispatch_stub_1265(GLuint queryId, GLuint * nextQueryId);
void GLAPIENTRY gl_dispatch_stub_1266(GLuint queryId, GLuint counterId, GLuint counterNameLength, GLchar * counterName, GLuint counterDescLength, GLchar * counterDesc, GLuint * counterOffset, GLuint * counterDataSize, GLuint * counterTypeEnum, GLuint * counterDataTypeEnum, GLuint64 * rawCounterMaxValue);
void GLAPIENTRY gl_dispatch_stub_1267(GLuint queryHandle, GLuint flags, GLsizei dataSize, GLvoid * data, GLuint * bytesWritten);
void GLAPIENTRY gl_dispatch_stub_1268(GLchar * queryName, GLuint * queryId);
void GLAPIENTRY gl_dispatch_stub_1269(GLuint queryId, GLuint queryNameLength, GLchar * queryName, GLuint * dataSize, GLuint * noCounters, GLuint * noInstances, GLuint * capsMask);
void GLAPIENTRY gl_dispatch_stub_1270(GLfloat factor, GLfloat units, GLfloat clamp);
void GLAPIENTRY gl_dispatch_stub_1271(GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);
void GLAPIENTRY gl_dispatch_stub_1272(GLenum target, GLuint index, GLsizei count, const GLfloat * params);
void GLAPIENTRY gl_dispatch_stub_1273(GLenum target, GLuint index, GLsizei count, const GLfloat * params);
void GLAPIENTRY gl_dispatch_stub_1274(GLenum target, GLvoid * writeOffset);
void GLAPIENTRY gl_dispatch_stub_1275(GLenum target, GLvoid * writeOffset);

/* OpenGL ES specific prototypes */

/* category GL_OES_EGL_image */
GLAPI void GLAPIENTRY glEGLImageTargetRenderbufferStorageOES(GLenum target, GLvoid * writeOffset);
GLAPI void GLAPIENTRY glEGLImageTargetTexture2DOES(GLenum target, GLvoid * writeOffset);
/* category GL_OES_blend_equation_separate */
GLAPI void GLAPIENTRY glBlendEquationSeparateOES(GLenum modeRGB, GLenum modeA);
/* category GL_OES_blend_func_separate */
GLAPI void GLAPIENTRY glBlendFuncSeparateOES(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
/* category GL_OES_blend_subtract */
GLAPI void GLAPIENTRY glBlendEquationOES(GLenum mode);
/* category GL_OES_draw_texture */
GLAPI void GLAPIENTRY glDrawTexfOES(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height);
GLAPI void GLAPIENTRY glDrawTexfvOES(const GLfloat * coords);
GLAPI void GLAPIENTRY glDrawTexiOES(GLint x, GLint y, GLint z, GLint width, GLint height);
GLAPI void GLAPIENTRY glDrawTexivOES(const GLint * coords);
GLAPI void GLAPIENTRY glDrawTexsOES(GLshort x, GLshort y, GLshort z, GLshort width, GLshort height);
GLAPI void GLAPIENTRY glDrawTexsvOES(const GLshort * coords);
GLAPI void GLAPIENTRY glDrawTexxOES(GLfixed x, GLfixed y, GLfixed z, GLfixed width, GLfixed height);
GLAPI void GLAPIENTRY glDrawTexxvOES(const GLfixed * coords);
/* category GL_OES_fixed_point */
GLAPI void GLAPIENTRY glGetTexGenxvOES(GLenum coord, GLenum pname, GLfixed * params);
GLAPI void GLAPIENTRY glTexGenxOES(GLenum coord, GLenum pname, GLint param);
GLAPI void GLAPIENTRY glTexGenxvOES(GLenum coord, GLenum pname, const GLfixed * params);
GLAPI void GLAPIENTRY glAlphaFuncxOES(GLenum func, GLclampx ref);
GLAPI void GLAPIENTRY glClearColorxOES(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
GLAPI void GLAPIENTRY glClearDepthxOES(GLclampx depth);
GLAPI void GLAPIENTRY glColor4xOES(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
GLAPI void GLAPIENTRY glDepthRangexOES(GLclampx zNear, GLclampx zFar);
GLAPI void GLAPIENTRY glFogxOES(GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glFogxvOES(GLenum pname, const GLfixed * params);
GLAPI void GLAPIENTRY glFrustumxOES(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
GLAPI void GLAPIENTRY glLightModelxOES(GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glLightModelxvOES(GLenum pname, const GLfixed * params);
GLAPI void GLAPIENTRY glLightxOES(GLenum light, GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glLightxvOES(GLenum light, GLenum pname, const GLfixed * params);
GLAPI void GLAPIENTRY glLineWidthxOES(GLfixed width);
GLAPI void GLAPIENTRY glLoadMatrixxOES(const GLfixed * m);
GLAPI void GLAPIENTRY glMaterialxOES(GLenum face, GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glMaterialxvOES(GLenum face, GLenum pname, const GLfixed * params);
GLAPI void GLAPIENTRY glMultMatrixxOES(const GLfixed * m);
GLAPI void GLAPIENTRY glMultiTexCoord4xOES(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
GLAPI void GLAPIENTRY glNormal3xOES(GLfixed nx, GLfixed ny, GLfixed nz);
GLAPI void GLAPIENTRY glOrthoxOES(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
GLAPI void GLAPIENTRY glPointSizexOES(GLfixed size);
GLAPI void GLAPIENTRY glPolygonOffsetxOES(GLfixed factor, GLfixed units);
GLAPI void GLAPIENTRY glRotatexOES(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
GLAPI void GLAPIENTRY glSampleCoveragexOES(GLclampx value, GLboolean invert);
GLAPI void GLAPIENTRY glScalexOES(GLfixed x, GLfixed y, GLfixed z);
GLAPI void GLAPIENTRY glTexEnvxOES(GLenum target, GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glTexEnvxvOES(GLenum target, GLenum pname, const GLfixed * params);
GLAPI void GLAPIENTRY glTexParameterxOES(GLenum target, GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glTranslatexOES(GLfixed x, GLfixed y, GLfixed z);
GLAPI void GLAPIENTRY glClipPlanexOES(GLenum plane, const GLfixed * equation);
GLAPI void GLAPIENTRY glGetClipPlanexOES(GLenum plane, GLfixed * equation);
GLAPI void GLAPIENTRY glGetFixedvOES(GLenum pname, GLfixed * params);
GLAPI void GLAPIENTRY glGetLightxvOES(GLenum light, GLenum pname, GLfixed * params);
GLAPI void GLAPIENTRY glGetMaterialxvOES(GLenum face, GLenum pname, GLfixed * params);
GLAPI void GLAPIENTRY glGetTexEnvxvOES(GLenum target, GLenum pname, GLfixed * params);
GLAPI void GLAPIENTRY glGetTexParameterxvOES(GLenum target, GLenum pname, GLfixed * params);
GLAPI void GLAPIENTRY glPointParameterxOES(GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glPointParameterxvOES(GLenum pname, const GLfixed * params);
GLAPI void GLAPIENTRY glTexParameterxvOES(GLenum target, GLenum pname, const GLfixed * params);
/* category GL_OES_framebuffer_object */
GLAPI void GLAPIENTRY glBindFramebufferOES(GLenum target, GLuint framebuffer);
GLAPI void GLAPIENTRY glBindRenderbufferOES(GLenum target, GLuint renderbuffer);
GLAPI GLenum GLAPIENTRY glCheckFramebufferStatusOES(GLenum target);
GLAPI void GLAPIENTRY glDeleteFramebuffersOES(GLsizei n, const GLuint * framebuffers);
GLAPI void GLAPIENTRY glDeleteRenderbuffersOES(GLsizei n, const GLuint * renderbuffers);
GLAPI void GLAPIENTRY glFramebufferRenderbufferOES(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
GLAPI void GLAPIENTRY glFramebufferTexture2DOES(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
GLAPI void GLAPIENTRY glGenFramebuffersOES(GLsizei n, GLuint * framebuffers);
GLAPI void GLAPIENTRY glGenRenderbuffersOES(GLsizei n, GLuint * renderbuffers);
GLAPI void GLAPIENTRY glGenerateMipmapOES(GLenum target);
GLAPI void GLAPIENTRY glGetFramebufferAttachmentParameterivOES(GLenum target, GLenum attachment, GLenum pname, GLint * params);
GLAPI void GLAPIENTRY glGetRenderbufferParameterivOES(GLenum target, GLenum pname, GLint * params);
GLAPI GLboolean GLAPIENTRY glIsFramebufferOES(GLuint framebuffer);
GLAPI GLboolean GLAPIENTRY glIsRenderbufferOES(GLuint renderbuffer);
GLAPI void GLAPIENTRY glRenderbufferStorageOES(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
/* category GL_OES_get_program_binary */
GLAPI void GLAPIENTRY glGetProgramBinaryOES(GLuint program, GLsizei bufSize, GLsizei * length, GLenum * binaryFormat, GLvoid * binary);
GLAPI void GLAPIENTRY glProgramBinaryOES(GLuint program, GLenum binaryFormat, const GLvoid * binary, GLint length);
/* category GL_OES_mapbuffer */
GLAPI void GLAPIENTRY glGetBufferPointervOES(GLenum target, GLenum pname, GLvoid ** params);
GLAPI GLvoid * GLAPIENTRY glMapBufferOES(GLenum target, GLenum access);
GLAPI GLboolean GLAPIENTRY glUnmapBufferOES(GLenum target);
/* category GL_OES_point_size_array */
GLAPI void GLAPIENTRY glPointSizePointerOES(GLenum type, GLsizei stride, const GLvoid * pointer);
/* category GL_OES_query_matrix */
GLAPI GLbitfield GLAPIENTRY glQueryMatrixxOES(GLfixed * mantissa, GLint * exponent);
/* category GL_OES_single_precision */
GLAPI void GLAPIENTRY glClearDepthfOES(GLclampf depth);
GLAPI void GLAPIENTRY glDepthRangefOES(GLclampf zNear, GLclampf zFar);
GLAPI void GLAPIENTRY glFrustumfOES(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
GLAPI void GLAPIENTRY glOrthofOES(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
GLAPI void GLAPIENTRY glClipPlanefOES(GLenum plane, const GLfloat * equation);
GLAPI void GLAPIENTRY glGetClipPlanefOES(GLenum plane, GLfloat * equation);
/* category GL_OES_texture_3D */
GLAPI void GLAPIENTRY glTexImage3DOES(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * pixels);
GLAPI void GLAPIENTRY glTexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * pixels);
GLAPI void GLAPIENTRY glCopyTexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI void GLAPIENTRY glCompressedTexImage3DOES(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid * data);
GLAPI void GLAPIENTRY glCompressedTexSubImage3DOES(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data);
GLAPI void GLAPIENTRY glFramebufferTexture3DOES(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
/* category GL_OES_texture_cube_map */
GLAPI void GLAPIENTRY glTexGenfOES(GLenum coord, GLenum pname, GLfloat param);
GLAPI void GLAPIENTRY glTexGenfvOES(GLenum coord, GLenum pname, const GLfloat * params);
GLAPI void GLAPIENTRY glTexGeniOES(GLenum coord, GLenum pname, GLint param);
GLAPI void GLAPIENTRY glTexGenivOES(GLenum coord, GLenum pname, const GLint * params);
GLAPI void GLAPIENTRY glGetTexGenfvOES(GLenum coord, GLenum pname, GLfloat * params);
GLAPI void GLAPIENTRY glGetTexGenivOES(GLenum coord, GLenum pname, GLint * params);
/* category GL_OES_vertex_array_object */
GLAPI void GLAPIENTRY glBindVertexArrayOES(GLuint array);
GLAPI void GLAPIENTRY glDeleteVertexArraysOES(GLsizei n, const GLuint * arrays);
GLAPI void GLAPIENTRY glGenVertexArraysOES(GLsizei n, GLuint * arrays);
GLAPI GLboolean GLAPIENTRY glIsVertexArrayOES(GLuint array);
/* category es1.0 */
GLAPI void GLAPIENTRY glAlphaFuncx(GLenum func, GLclampx ref);
GLAPI void GLAPIENTRY glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
GLAPI void GLAPIENTRY glClearDepthx(GLclampx depth);
GLAPI void GLAPIENTRY glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
GLAPI void GLAPIENTRY glDepthRangex(GLclampx zNear, GLclampx zFar);
GLAPI void GLAPIENTRY glFogx(GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glFogxv(GLenum pname, const GLfixed * params);
GLAPI void GLAPIENTRY glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
GLAPI void GLAPIENTRY glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
GLAPI void GLAPIENTRY glLightModelx(GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glLightModelxv(GLenum pname, const GLfixed * params);
GLAPI void GLAPIENTRY glLightx(GLenum light, GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glLightxv(GLenum light, GLenum pname, const GLfixed * params);
GLAPI void GLAPIENTRY glLineWidthx(GLfixed width);
GLAPI void GLAPIENTRY glLoadMatrixx(const GLfixed * m);
GLAPI void GLAPIENTRY glMaterialx(GLenum face, GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glMaterialxv(GLenum face, GLenum pname, const GLfixed * params);
GLAPI void GLAPIENTRY glMultMatrixx(const GLfixed * m);
GLAPI void GLAPIENTRY glMultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
GLAPI void GLAPIENTRY glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz);
GLAPI void GLAPIENTRY glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
GLAPI void GLAPIENTRY glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
GLAPI void GLAPIENTRY glPointSizex(GLfixed size);
GLAPI void GLAPIENTRY glPolygonOffsetx(GLfixed factor, GLfixed units);
GLAPI void GLAPIENTRY glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
GLAPI void GLAPIENTRY glSampleCoveragex(GLclampx value, GLboolean invert);
GLAPI void GLAPIENTRY glScalex(GLfixed x, GLfixed y, GLfixed z);
GLAPI void GLAPIENTRY glTexEnvx(GLenum target, GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glTexEnvxv(GLenum target, GLenum pname, const GLfixed * params);
GLAPI void GLAPIENTRY glTexParameterx(GLenum target, GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glTranslatex(GLfixed x, GLfixed y, GLfixed z);
/* category es1.1 */
GLAPI void GLAPIENTRY glClipPlanef(GLenum plane, const GLfloat * equation);
GLAPI void GLAPIENTRY glClipPlanex(GLenum plane, const GLfixed * equation);
GLAPI void GLAPIENTRY glGetClipPlanef(GLenum plane, GLfloat * equation);
GLAPI void GLAPIENTRY glGetClipPlanex(GLenum plane, GLfixed * equation);
GLAPI void GLAPIENTRY glGetFixedv(GLenum pname, GLfixed * params);
GLAPI void GLAPIENTRY glGetLightxv(GLenum light, GLenum pname, GLfixed * params);
GLAPI void GLAPIENTRY glGetMaterialxv(GLenum face, GLenum pname, GLfixed * params);
GLAPI void GLAPIENTRY glGetTexEnvxv(GLenum target, GLenum pname, GLfixed * params);
GLAPI void GLAPIENTRY glGetTexParameterxv(GLenum target, GLenum pname, GLfixed * params);
GLAPI void GLAPIENTRY glPointParameterx(GLenum pname, GLfixed param);
GLAPI void GLAPIENTRY glPointParameterxv(GLenum pname, const GLfixed * params);
GLAPI void GLAPIENTRY glTexParameterxv(GLenum target, GLenum pname, const GLfixed * params);

#endif /* defined(NEED_FUNCTION_POINTER) || defined(GLX_INDIRECT_RENDERING) */

static const glprocs_table_t static_functions[] = {
    NAME_FUNC_OFFSET(    0, glNewList, glNewList, NULL, 0),
    NAME_FUNC_OFFSET(   10, glEndList, glEndList, NULL, 1),
    NAME_FUNC_OFFSET(   20, glCallList, glCallList, NULL, 2),
    NAME_FUNC_OFFSET(   31, glCallLists, glCallLists, NULL, 3),
    NAME_FUNC_OFFSET(   43, glDeleteLists, glDeleteLists, NULL, 4),
    NAME_FUNC_OFFSET(   57, glGenLists, glGenLists, NULL, 5),
    NAME_FUNC_OFFSET(   68, glListBase, glListBase, NULL, 6),
    NAME_FUNC_OFFSET(   79, glBegin, glBegin, NULL, 7),
    NAME_FUNC_OFFSET(   87, glBitmap, glBitmap, NULL, 8),
    NAME_FUNC_OFFSET(   96, glColor3b, glColor3b, NULL, 9),
    NAME_FUNC_OFFSET(  106, glColor3bv, glColor3bv, NULL, 10),
    NAME_FUNC_OFFSET(  117, glColor3d, glColor3d, NULL, 11),
    NAME_FUNC_OFFSET(  127, glColor3dv, glColor3dv, NULL, 12),
    NAME_FUNC_OFFSET(  138, glColor3f, glColor3f, NULL, 13),
    NAME_FUNC_OFFSET(  148, glColor3fv, glColor3fv, NULL, 14),
    NAME_FUNC_OFFSET(  159, glColor3i, glColor3i, NULL, 15),
    NAME_FUNC_OFFSET(  169, glColor3iv, glColor3iv, NULL, 16),
    NAME_FUNC_OFFSET(  180, glColor3s, glColor3s, NULL, 17),
    NAME_FUNC_OFFSET(  190, glColor3sv, glColor3sv, NULL, 18),
    NAME_FUNC_OFFSET(  201, glColor3ub, glColor3ub, NULL, 19),
    NAME_FUNC_OFFSET(  212, glColor3ubv, glColor3ubv, NULL, 20),
    NAME_FUNC_OFFSET(  224, glColor3ui, glColor3ui, NULL, 21),
    NAME_FUNC_OFFSET(  235, glColor3uiv, glColor3uiv, NULL, 22),
    NAME_FUNC_OFFSET(  247, glColor3us, glColor3us, NULL, 23),
    NAME_FUNC_OFFSET(  258, glColor3usv, glColor3usv, NULL, 24),
    NAME_FUNC_OFFSET(  270, glColor4b, glColor4b, NULL, 25),
    NAME_FUNC_OFFSET(  280, glColor4bv, glColor4bv, NULL, 26),
    NAME_FUNC_OFFSET(  291, glColor4d, glColor4d, NULL, 27),
    NAME_FUNC_OFFSET(  301, glColor4dv, glColor4dv, NULL, 28),
    NAME_FUNC_OFFSET(  312, glColor4f, glColor4f, NULL, 29),
    NAME_FUNC_OFFSET(  322, glColor4fv, glColor4fv, NULL, 30),
    NAME_FUNC_OFFSET(  333, glColor4i, glColor4i, NULL, 31),
    NAME_FUNC_OFFSET(  343, glColor4iv, glColor4iv, NULL, 32),
    NAME_FUNC_OFFSET(  354, glColor4s, glColor4s, NULL, 33),
    NAME_FUNC_OFFSET(  364, glColor4sv, glColor4sv, NULL, 34),
    NAME_FUNC_OFFSET(  375, glColor4ub, glColor4ub, NULL, 35),
    NAME_FUNC_OFFSET(  386, glColor4ubv, glColor4ubv, NULL, 36),
    NAME_FUNC_OFFSET(  398, glColor4ui, glColor4ui, NULL, 37),
    NAME_FUNC_OFFSET(  409, glColor4uiv, glColor4uiv, NULL, 38),
    NAME_FUNC_OFFSET(  421, glColor4us, glColor4us, NULL, 39),
    NAME_FUNC_OFFSET(  432, glColor4usv, glColor4usv, NULL, 40),
    NAME_FUNC_OFFSET(  444, glEdgeFlag, glEdgeFlag, NULL, 41),
    NAME_FUNC_OFFSET(  455, glEdgeFlagv, glEdgeFlagv, NULL, 42),
    NAME_FUNC_OFFSET(  467, glEnd, glEnd, NULL, 43),
    NAME_FUNC_OFFSET(  473, glIndexd, glIndexd, NULL, 44),
    NAME_FUNC_OFFSET(  482, glIndexdv, glIndexdv, NULL, 45),
    NAME_FUNC_OFFSET(  492, glIndexf, glIndexf, NULL, 46),
    NAME_FUNC_OFFSET(  501, glIndexfv, glIndexfv, NULL, 47),
    NAME_FUNC_OFFSET(  511, glIndexi, glIndexi, NULL, 48),
    NAME_FUNC_OFFSET(  520, glIndexiv, glIndexiv, NULL, 49),
    NAME_FUNC_OFFSET(  530, glIndexs, glIndexs, NULL, 50),
    NAME_FUNC_OFFSET(  539, glIndexsv, glIndexsv, NULL, 51),
    NAME_FUNC_OFFSET(  549, glNormal3b, glNormal3b, NULL, 52),
    NAME_FUNC_OFFSET(  560, glNormal3bv, glNormal3bv, NULL, 53),
    NAME_FUNC_OFFSET(  572, glNormal3d, glNormal3d, NULL, 54),
    NAME_FUNC_OFFSET(  583, glNormal3dv, glNormal3dv, NULL, 55),
    NAME_FUNC_OFFSET(  595, glNormal3f, glNormal3f, NULL, 56),
    NAME_FUNC_OFFSET(  606, glNormal3fv, glNormal3fv, NULL, 57),
    NAME_FUNC_OFFSET(  618, glNormal3i, glNormal3i, NULL, 58),
    NAME_FUNC_OFFSET(  629, glNormal3iv, glNormal3iv, NULL, 59),
    NAME_FUNC_OFFSET(  641, glNormal3s, glNormal3s, NULL, 60),
    NAME_FUNC_OFFSET(  652, glNormal3sv, glNormal3sv, NULL, 61),
    NAME_FUNC_OFFSET(  664, glRasterPos2d, glRasterPos2d, NULL, 62),
    NAME_FUNC_OFFSET(  678, glRasterPos2dv, glRasterPos2dv, NULL, 63),
    NAME_FUNC_OFFSET(  693, glRasterPos2f, glRasterPos2f, NULL, 64),
    NAME_FUNC_OFFSET(  707, glRasterPos2fv, glRasterPos2fv, NULL, 65),
    NAME_FUNC_OFFSET(  722, glRasterPos2i, glRasterPos2i, NULL, 66),
    NAME_FUNC_OFFSET(  736, glRasterPos2iv, glRasterPos2iv, NULL, 67),
    NAME_FUNC_OFFSET(  751, glRasterPos2s, glRasterPos2s, NULL, 68),
    NAME_FUNC_OFFSET(  765, glRasterPos2sv, glRasterPos2sv, NULL, 69),
    NAME_FUNC_OFFSET(  780, glRasterPos3d, glRasterPos3d, NULL, 70),
    NAME_FUNC_OFFSET(  794, glRasterPos3dv, glRasterPos3dv, NULL, 71),
    NAME_FUNC_OFFSET(  809, glRasterPos3f, glRasterPos3f, NULL, 72),
    NAME_FUNC_OFFSET(  823, glRasterPos3fv, glRasterPos3fv, NULL, 73),
    NAME_FUNC_OFFSET(  838, glRasterPos3i, glRasterPos3i, NULL, 74),
    NAME_FUNC_OFFSET(  852, glRasterPos3iv, glRasterPos3iv, NULL, 75),
    NAME_FUNC_OFFSET(  867, glRasterPos3s, glRasterPos3s, NULL, 76),
    NAME_FUNC_OFFSET(  881, glRasterPos3sv, glRasterPos3sv, NULL, 77),
    NAME_FUNC_OFFSET(  896, glRasterPos4d, glRasterPos4d, NULL, 78),
    NAME_FUNC_OFFSET(  910, glRasterPos4dv, glRasterPos4dv, NULL, 79),
    NAME_FUNC_OFFSET(  925, glRasterPos4f, glRasterPos4f, NULL, 80),
    NAME_FUNC_OFFSET(  939, glRasterPos4fv, glRasterPos4fv, NULL, 81),
    NAME_FUNC_OFFSET(  954, glRasterPos4i, glRasterPos4i, NULL, 82),
    NAME_FUNC_OFFSET(  968, glRasterPos4iv, glRasterPos4iv, NULL, 83),
    NAME_FUNC_OFFSET(  983, glRasterPos4s, glRasterPos4s, NULL, 84),
    NAME_FUNC_OFFSET(  997, glRasterPos4sv, glRasterPos4sv, NULL, 85),
    NAME_FUNC_OFFSET( 1012, glRectd, glRectd, NULL, 86),
    NAME_FUNC_OFFSET( 1020, glRectdv, glRectdv, NULL, 87),
    NAME_FUNC_OFFSET( 1029, glRectf, glRectf, NULL, 88),
    NAME_FUNC_OFFSET( 1037, glRectfv, glRectfv, NULL, 89),
    NAME_FUNC_OFFSET( 1046, glRecti, glRecti, NULL, 90),
    NAME_FUNC_OFFSET( 1054, glRectiv, glRectiv, NULL, 91),
    NAME_FUNC_OFFSET( 1063, glRects, glRects, NULL, 92),
    NAME_FUNC_OFFSET( 1071, glRectsv, glRectsv, NULL, 93),
    NAME_FUNC_OFFSET( 1080, glTexCoord1d, glTexCoord1d, NULL, 94),
    NAME_FUNC_OFFSET( 1093, glTexCoord1dv, glTexCoord1dv, NULL, 95),
    NAME_FUNC_OFFSET( 1107, glTexCoord1f, glTexCoord1f, NULL, 96),
    NAME_FUNC_OFFSET( 1120, glTexCoord1fv, glTexCoord1fv, NULL, 97),
    NAME_FUNC_OFFSET( 1134, glTexCoord1i, glTexCoord1i, NULL, 98),
    NAME_FUNC_OFFSET( 1147, glTexCoord1iv, glTexCoord1iv, NULL, 99),
    NAME_FUNC_OFFSET( 1161, glTexCoord1s, glTexCoord1s, NULL, 100),
    NAME_FUNC_OFFSET( 1174, glTexCoord1sv, glTexCoord1sv, NULL, 101),
    NAME_FUNC_OFFSET( 1188, glTexCoord2d, glTexCoord2d, NULL, 102),
    NAME_FUNC_OFFSET( 1201, glTexCoord2dv, glTexCoord2dv, NULL, 103),
    NAME_FUNC_OFFSET( 1215, glTexCoord2f, glTexCoord2f, NULL, 104),
    NAME_FUNC_OFFSET( 1228, glTexCoord2fv, glTexCoord2fv, NULL, 105),
    NAME_FUNC_OFFSET( 1242, glTexCoord2i, glTexCoord2i, NULL, 106),
    NAME_FUNC_OFFSET( 1255, glTexCoord2iv, glTexCoord2iv, NULL, 107),
    NAME_FUNC_OFFSET( 1269, glTexCoord2s, glTexCoord2s, NULL, 108),
    NAME_FUNC_OFFSET( 1282, glTexCoord2sv, glTexCoord2sv, NULL, 109),
    NAME_FUNC_OFFSET( 1296, glTexCoord3d, glTexCoord3d, NULL, 110),
    NAME_FUNC_OFFSET( 1309, glTexCoord3dv, glTexCoord3dv, NULL, 111),
    NAME_FUNC_OFFSET( 1323, glTexCoord3f, glTexCoord3f, NULL, 112),
    NAME_FUNC_OFFSET( 1336, glTexCoord3fv, glTexCoord3fv, NULL, 113),
    NAME_FUNC_OFFSET( 1350, glTexCoord3i, glTexCoord3i, NULL, 114),
    NAME_FUNC_OFFSET( 1363, glTexCoord3iv, glTexCoord3iv, NULL, 115),
    NAME_FUNC_OFFSET( 1377, glTexCoord3s, glTexCoord3s, NULL, 116),
    NAME_FUNC_OFFSET( 1390, glTexCoord3sv, glTexCoord3sv, NULL, 117),
    NAME_FUNC_OFFSET( 1404, glTexCoord4d, glTexCoord4d, NULL, 118),
    NAME_FUNC_OFFSET( 1417, glTexCoord4dv, glTexCoord4dv, NULL, 119),
    NAME_FUNC_OFFSET( 1431, glTexCoord4f, glTexCoord4f, NULL, 120),
    NAME_FUNC_OFFSET( 1444, glTexCoord4fv, glTexCoord4fv, NULL, 121),
    NAME_FUNC_OFFSET( 1458, glTexCoord4i, glTexCoord4i, NULL, 122),
    NAME_FUNC_OFFSET( 1471, glTexCoord4iv, glTexCoord4iv, NULL, 123),
    NAME_FUNC_OFFSET( 1485, glTexCoord4s, glTexCoord4s, NULL, 124),
    NAME_FUNC_OFFSET( 1498, glTexCoord4sv, glTexCoord4sv, NULL, 125),
    NAME_FUNC_OFFSET( 1512, glVertex2d, glVertex2d, NULL, 126),
    NAME_FUNC_OFFSET( 1523, glVertex2dv, glVertex2dv, NULL, 127),
    NAME_FUNC_OFFSET( 1535, glVertex2f, glVertex2f, NULL, 128),
    NAME_FUNC_OFFSET( 1546, glVertex2fv, glVertex2fv, NULL, 129),
    NAME_FUNC_OFFSET( 1558, glVertex2i, glVertex2i, NULL, 130),
    NAME_FUNC_OFFSET( 1569, glVertex2iv, glVertex2iv, NULL, 131),
    NAME_FUNC_OFFSET( 1581, glVertex2s, glVertex2s, NULL, 132),
    NAME_FUNC_OFFSET( 1592, glVertex2sv, glVertex2sv, NULL, 133),
    NAME_FUNC_OFFSET( 1604, glVertex3d, glVertex3d, NULL, 134),
    NAME_FUNC_OFFSET( 1615, glVertex3dv, glVertex3dv, NULL, 135),
    NAME_FUNC_OFFSET( 1627, glVertex3f, glVertex3f, NULL, 136),
    NAME_FUNC_OFFSET( 1638, glVertex3fv, glVertex3fv, NULL, 137),
    NAME_FUNC_OFFSET( 1650, glVertex3i, glVertex3i, NULL, 138),
    NAME_FUNC_OFFSET( 1661, glVertex3iv, glVertex3iv, NULL, 139),
    NAME_FUNC_OFFSET( 1673, glVertex3s, glVertex3s, NULL, 140),
    NAME_FUNC_OFFSET( 1684, glVertex3sv, glVertex3sv, NULL, 141),
    NAME_FUNC_OFFSET( 1696, glVertex4d, glVertex4d, NULL, 142),
    NAME_FUNC_OFFSET( 1707, glVertex4dv, glVertex4dv, NULL, 143),
    NAME_FUNC_OFFSET( 1719, glVertex4f, glVertex4f, NULL, 144),
    NAME_FUNC_OFFSET( 1730, glVertex4fv, glVertex4fv, NULL, 145),
    NAME_FUNC_OFFSET( 1742, glVertex4i, glVertex4i, NULL, 146),
    NAME_FUNC_OFFSET( 1753, glVertex4iv, glVertex4iv, NULL, 147),
    NAME_FUNC_OFFSET( 1765, glVertex4s, glVertex4s, NULL, 148),
    NAME_FUNC_OFFSET( 1776, glVertex4sv, glVertex4sv, NULL, 149),
    NAME_FUNC_OFFSET( 1788, glClipPlane, glClipPlane, NULL, 150),
    NAME_FUNC_OFFSET( 1800, glColorMaterial, glColorMaterial, NULL, 151),
    NAME_FUNC_OFFSET( 1816, glCullFace, glCullFace, NULL, 152),
    NAME_FUNC_OFFSET( 1827, glFogf, glFogf, NULL, 153),
    NAME_FUNC_OFFSET( 1834, glFogfv, glFogfv, NULL, 154),
    NAME_FUNC_OFFSET( 1842, glFogi, glFogi, NULL, 155),
    NAME_FUNC_OFFSET( 1849, glFogiv, glFogiv, NULL, 156),
    NAME_FUNC_OFFSET( 1857, glFrontFace, glFrontFace, NULL, 157),
    NAME_FUNC_OFFSET( 1869, glHint, glHint, NULL, 158),
    NAME_FUNC_OFFSET( 1876, glLightf, glLightf, NULL, 159),
    NAME_FUNC_OFFSET( 1885, glLightfv, glLightfv, NULL, 160),
    NAME_FUNC_OFFSET( 1895, glLighti, glLighti, NULL, 161),
    NAME_FUNC_OFFSET( 1904, glLightiv, glLightiv, NULL, 162),
    NAME_FUNC_OFFSET( 1914, glLightModelf, glLightModelf, NULL, 163),
    NAME_FUNC_OFFSET( 1928, glLightModelfv, glLightModelfv, NULL, 164),
    NAME_FUNC_OFFSET( 1943, glLightModeli, glLightModeli, NULL, 165),
    NAME_FUNC_OFFSET( 1957, glLightModeliv, glLightModeliv, NULL, 166),
    NAME_FUNC_OFFSET( 1972, glLineStipple, glLineStipple, NULL, 167),
    NAME_FUNC_OFFSET( 1986, glLineWidth, glLineWidth, NULL, 168),
    NAME_FUNC_OFFSET( 1998, glMaterialf, glMaterialf, NULL, 169),
    NAME_FUNC_OFFSET( 2010, glMaterialfv, glMaterialfv, NULL, 170),
    NAME_FUNC_OFFSET( 2023, glMateriali, glMateriali, NULL, 171),
    NAME_FUNC_OFFSET( 2035, glMaterialiv, glMaterialiv, NULL, 172),
    NAME_FUNC_OFFSET( 2048, glPointSize, glPointSize, NULL, 173),
    NAME_FUNC_OFFSET( 2060, glPolygonMode, glPolygonMode, NULL, 174),
    NAME_FUNC_OFFSET( 2074, glPolygonStipple, glPolygonStipple, NULL, 175),
    NAME_FUNC_OFFSET( 2091, glScissor, glScissor, NULL, 176),
    NAME_FUNC_OFFSET( 2101, glShadeModel, glShadeModel, NULL, 177),
    NAME_FUNC_OFFSET( 2114, glTexParameterf, glTexParameterf, NULL, 178),
    NAME_FUNC_OFFSET( 2130, glTexParameterfv, glTexParameterfv, NULL, 179),
    NAME_FUNC_OFFSET( 2147, glTexParameteri, glTexParameteri, NULL, 180),
    NAME_FUNC_OFFSET( 2163, glTexParameteriv, glTexParameteriv, NULL, 181),
    NAME_FUNC_OFFSET( 2180, glTexImage1D, glTexImage1D, NULL, 182),
    NAME_FUNC_OFFSET( 2193, glTexImage2D, glTexImage2D, NULL, 183),
    NAME_FUNC_OFFSET( 2206, glTexEnvf, glTexEnvf, NULL, 184),
    NAME_FUNC_OFFSET( 2216, glTexEnvfv, glTexEnvfv, NULL, 185),
    NAME_FUNC_OFFSET( 2227, glTexEnvi, glTexEnvi, NULL, 186),
    NAME_FUNC_OFFSET( 2237, glTexEnviv, glTexEnviv, NULL, 187),
    NAME_FUNC_OFFSET( 2248, glTexGend, glTexGend, NULL, 188),
    NAME_FUNC_OFFSET( 2258, glTexGendv, glTexGendv, NULL, 189),
    NAME_FUNC_OFFSET( 2269, glTexGenf, glTexGenf, NULL, 190),
    NAME_FUNC_OFFSET( 2279, glTexGenfv, glTexGenfv, NULL, 191),
    NAME_FUNC_OFFSET( 2290, glTexGeni, glTexGeni, NULL, 192),
    NAME_FUNC_OFFSET( 2300, glTexGeniv, glTexGeniv, NULL, 193),
    NAME_FUNC_OFFSET( 2311, glFeedbackBuffer, glFeedbackBuffer, NULL, 194),
    NAME_FUNC_OFFSET( 2328, glSelectBuffer, glSelectBuffer, NULL, 195),
    NAME_FUNC_OFFSET( 2343, glRenderMode, glRenderMode, NULL, 196),
    NAME_FUNC_OFFSET( 2356, glInitNames, glInitNames, NULL, 197),
    NAME_FUNC_OFFSET( 2368, glLoadName, glLoadName, NULL, 198),
    NAME_FUNC_OFFSET( 2379, glPassThrough, glPassThrough, NULL, 199),
    NAME_FUNC_OFFSET( 2393, glPopName, glPopName, NULL, 200),
    NAME_FUNC_OFFSET( 2403, glPushName, glPushName, NULL, 201),
    NAME_FUNC_OFFSET( 2414, glDrawBuffer, glDrawBuffer, NULL, 202),
    NAME_FUNC_OFFSET( 2427, glClear, glClear, NULL, 203),
    NAME_FUNC_OFFSET( 2435, glClearAccum, glClearAccum, NULL, 204),
    NAME_FUNC_OFFSET( 2448, glClearIndex, glClearIndex, NULL, 205),
    NAME_FUNC_OFFSET( 2461, glClearColor, glClearColor, NULL, 206),
    NAME_FUNC_OFFSET( 2474, glClearStencil, glClearStencil, NULL, 207),
    NAME_FUNC_OFFSET( 2489, glClearDepth, glClearDepth, NULL, 208),
    NAME_FUNC_OFFSET( 2502, glStencilMask, glStencilMask, NULL, 209),
    NAME_FUNC_OFFSET( 2516, glColorMask, glColorMask, NULL, 210),
    NAME_FUNC_OFFSET( 2528, glDepthMask, glDepthMask, NULL, 211),
    NAME_FUNC_OFFSET( 2540, glIndexMask, glIndexMask, NULL, 212),
    NAME_FUNC_OFFSET( 2552, glAccum, glAccum, NULL, 213),
    NAME_FUNC_OFFSET( 2560, glDisable, glDisable, NULL, 214),
    NAME_FUNC_OFFSET( 2570, glEnable, glEnable, NULL, 215),
    NAME_FUNC_OFFSET( 2579, glFinish, glFinish, NULL, 216),
    NAME_FUNC_OFFSET( 2588, glFlush, glFlush, NULL, 217),
    NAME_FUNC_OFFSET( 2596, glPopAttrib, glPopAttrib, NULL, 218),
    NAME_FUNC_OFFSET( 2608, glPushAttrib, glPushAttrib, NULL, 219),
    NAME_FUNC_OFFSET( 2621, glMap1d, glMap1d, NULL, 220),
    NAME_FUNC_OFFSET( 2629, glMap1f, glMap1f, NULL, 221),
    NAME_FUNC_OFFSET( 2637, glMap2d, glMap2d, NULL, 222),
    NAME_FUNC_OFFSET( 2645, glMap2f, glMap2f, NULL, 223),
    NAME_FUNC_OFFSET( 2653, glMapGrid1d, glMapGrid1d, NULL, 224),
    NAME_FUNC_OFFSET( 2665, glMapGrid1f, glMapGrid1f, NULL, 225),
    NAME_FUNC_OFFSET( 2677, glMapGrid2d, glMapGrid2d, NULL, 226),
    NAME_FUNC_OFFSET( 2689, glMapGrid2f, glMapGrid2f, NULL, 227),
    NAME_FUNC_OFFSET( 2701, glEvalCoord1d, glEvalCoord1d, NULL, 228),
    NAME_FUNC_OFFSET( 2715, glEvalCoord1dv, glEvalCoord1dv, NULL, 229),
    NAME_FUNC_OFFSET( 2730, glEvalCoord1f, glEvalCoord1f, NULL, 230),
    NAME_FUNC_OFFSET( 2744, glEvalCoord1fv, glEvalCoord1fv, NULL, 231),
    NAME_FUNC_OFFSET( 2759, glEvalCoord2d, glEvalCoord2d, NULL, 232),
    NAME_FUNC_OFFSET( 2773, glEvalCoord2dv, glEvalCoord2dv, NULL, 233),
    NAME_FUNC_OFFSET( 2788, glEvalCoord2f, glEvalCoord2f, NULL, 234),
    NAME_FUNC_OFFSET( 2802, glEvalCoord2fv, glEvalCoord2fv, NULL, 235),
    NAME_FUNC_OFFSET( 2817, glEvalMesh1, glEvalMesh1, NULL, 236),
    NAME_FUNC_OFFSET( 2829, glEvalPoint1, glEvalPoint1, NULL, 237),
    NAME_FUNC_OFFSET( 2842, glEvalMesh2, glEvalMesh2, NULL, 238),
    NAME_FUNC_OFFSET( 2854, glEvalPoint2, glEvalPoint2, NULL, 239),
    NAME_FUNC_OFFSET( 2867, glAlphaFunc, glAlphaFunc, NULL, 240),
    NAME_FUNC_OFFSET( 2879, glBlendFunc, glBlendFunc, NULL, 241),
    NAME_FUNC_OFFSET( 2891, glLogicOp, glLogicOp, NULL, 242),
    NAME_FUNC_OFFSET( 2901, glStencilFunc, glStencilFunc, NULL, 243),
    NAME_FUNC_OFFSET( 2915, glStencilOp, glStencilOp, NULL, 244),
    NAME_FUNC_OFFSET( 2927, glDepthFunc, glDepthFunc, NULL, 245),
    NAME_FUNC_OFFSET( 2939, glPixelZoom, glPixelZoom, NULL, 246),
    NAME_FUNC_OFFSET( 2951, glPixelTransferf, glPixelTransferf, NULL, 247),
    NAME_FUNC_OFFSET( 2968, glPixelTransferi, glPixelTransferi, NULL, 248),
    NAME_FUNC_OFFSET( 2985, glPixelStoref, glPixelStoref, NULL, 249),
    NAME_FUNC_OFFSET( 2999, glPixelStorei, glPixelStorei, NULL, 250),
    NAME_FUNC_OFFSET( 3013, glPixelMapfv, glPixelMapfv, NULL, 251),
    NAME_FUNC_OFFSET( 3026, glPixelMapuiv, glPixelMapuiv, NULL, 252),
    NAME_FUNC_OFFSET( 3040, glPixelMapusv, glPixelMapusv, NULL, 253),
    NAME_FUNC_OFFSET( 3054, glReadBuffer, glReadBuffer, NULL, 254),
    NAME_FUNC_OFFSET( 3067, glCopyPixels, glCopyPixels, NULL, 255),
    NAME_FUNC_OFFSET( 3080, glReadPixels, glReadPixels, NULL, 256),
    NAME_FUNC_OFFSET( 3093, glDrawPixels, glDrawPixels, NULL, 257),
    NAME_FUNC_OFFSET( 3106, glGetBooleanv, glGetBooleanv, NULL, 258),
    NAME_FUNC_OFFSET( 3120, glGetClipPlane, glGetClipPlane, NULL, 259),
    NAME_FUNC_OFFSET( 3135, glGetDoublev, glGetDoublev, NULL, 260),
    NAME_FUNC_OFFSET( 3148, glGetError, glGetError, NULL, 261),
    NAME_FUNC_OFFSET( 3159, glGetFloatv, glGetFloatv, NULL, 262),
    NAME_FUNC_OFFSET( 3171, glGetIntegerv, glGetIntegerv, NULL, 263),
    NAME_FUNC_OFFSET( 3185, glGetLightfv, glGetLightfv, NULL, 264),
    NAME_FUNC_OFFSET( 3198, glGetLightiv, glGetLightiv, NULL, 265),
    NAME_FUNC_OFFSET( 3211, glGetMapdv, glGetMapdv, NULL, 266),
    NAME_FUNC_OFFSET( 3222, glGetMapfv, glGetMapfv, NULL, 267),
    NAME_FUNC_OFFSET( 3233, glGetMapiv, glGetMapiv, NULL, 268),
    NAME_FUNC_OFFSET( 3244, glGetMaterialfv, glGetMaterialfv, NULL, 269),
    NAME_FUNC_OFFSET( 3260, glGetMaterialiv, glGetMaterialiv, NULL, 270),
    NAME_FUNC_OFFSET( 3276, glGetPixelMapfv, glGetPixelMapfv, NULL, 271),
    NAME_FUNC_OFFSET( 3292, glGetPixelMapuiv, glGetPixelMapuiv, NULL, 272),
    NAME_FUNC_OFFSET( 3309, glGetPixelMapusv, glGetPixelMapusv, NULL, 273),
    NAME_FUNC_OFFSET( 3326, glGetPolygonStipple, glGetPolygonStipple, NULL, 274),
    NAME_FUNC_OFFSET( 3346, glGetString, glGetString, NULL, 275),
    NAME_FUNC_OFFSET( 3358, glGetTexEnvfv, glGetTexEnvfv, NULL, 276),
    NAME_FUNC_OFFSET( 3372, glGetTexEnviv, glGetTexEnviv, NULL, 277),
    NAME_FUNC_OFFSET( 3386, glGetTexGendv, glGetTexGendv, NULL, 278),
    NAME_FUNC_OFFSET( 3400, glGetTexGenfv, glGetTexGenfv, NULL, 279),
    NAME_FUNC_OFFSET( 3414, glGetTexGeniv, glGetTexGeniv, NULL, 280),
    NAME_FUNC_OFFSET( 3428, glGetTexImage, glGetTexImage, NULL, 281),
    NAME_FUNC_OFFSET( 3442, glGetTexParameterfv, glGetTexParameterfv, NULL, 282),
    NAME_FUNC_OFFSET( 3462, glGetTexParameteriv, glGetTexParameteriv, NULL, 283),
    NAME_FUNC_OFFSET( 3482, glGetTexLevelParameterfv, glGetTexLevelParameterfv, NULL, 284),
    NAME_FUNC_OFFSET( 3507, glGetTexLevelParameteriv, glGetTexLevelParameteriv, NULL, 285),
    NAME_FUNC_OFFSET( 3532, glIsEnabled, glIsEnabled, NULL, 286),
    NAME_FUNC_OFFSET( 3544, glIsList, glIsList, NULL, 287),
    NAME_FUNC_OFFSET( 3553, glDepthRange, glDepthRange, NULL, 288),
    NAME_FUNC_OFFSET( 3566, glFrustum, glFrustum, NULL, 289),
    NAME_FUNC_OFFSET( 3576, glLoadIdentity, glLoadIdentity, NULL, 290),
    NAME_FUNC_OFFSET( 3591, glLoadMatrixf, glLoadMatrixf, NULL, 291),
    NAME_FUNC_OFFSET( 3605, glLoadMatrixd, glLoadMatrixd, NULL, 292),
    NAME_FUNC_OFFSET( 3619, glMatrixMode, glMatrixMode, NULL, 293),
    NAME_FUNC_OFFSET( 3632, glMultMatrixf, glMultMatrixf, NULL, 294),
    NAME_FUNC_OFFSET( 3646, glMultMatrixd, glMultMatrixd, NULL, 295),
    NAME_FUNC_OFFSET( 3660, glOrtho, glOrtho, NULL, 296),
    NAME_FUNC_OFFSET( 3668, glPopMatrix, glPopMatrix, NULL, 297),
    NAME_FUNC_OFFSET( 3680, glPushMatrix, glPushMatrix, NULL, 298),
    NAME_FUNC_OFFSET( 3693, glRotated, glRotated, NULL, 299),
    NAME_FUNC_OFFSET( 3703, glRotatef, glRotatef, NULL, 300),
    NAME_FUNC_OFFSET( 3713, glScaled, glScaled, NULL, 301),
    NAME_FUNC_OFFSET( 3722, glScalef, glScalef, NULL, 302),
    NAME_FUNC_OFFSET( 3731, glTranslated, glTranslated, NULL, 303),
    NAME_FUNC_OFFSET( 3744, glTranslatef, glTranslatef, NULL, 304),
    NAME_FUNC_OFFSET( 3757, glViewport, glViewport, NULL, 305),
    NAME_FUNC_OFFSET( 3768, glArrayElement, glArrayElement, NULL, 306),
    NAME_FUNC_OFFSET( 3783, glBindTexture, glBindTexture, NULL, 307),
    NAME_FUNC_OFFSET( 3797, glColorPointer, glColorPointer, NULL, 308),
    NAME_FUNC_OFFSET( 3812, glDisableClientState, glDisableClientState, NULL, 309),
    NAME_FUNC_OFFSET( 3833, glDrawArrays, glDrawArrays, NULL, 310),
    NAME_FUNC_OFFSET( 3846, glDrawElements, glDrawElements, NULL, 311),
    NAME_FUNC_OFFSET( 3861, glEdgeFlagPointer, glEdgeFlagPointer, NULL, 312),
    NAME_FUNC_OFFSET( 3879, glEnableClientState, glEnableClientState, NULL, 313),
    NAME_FUNC_OFFSET( 3899, glIndexPointer, glIndexPointer, NULL, 314),
    NAME_FUNC_OFFSET( 3914, glIndexub, glIndexub, NULL, 315),
    NAME_FUNC_OFFSET( 3924, glIndexubv, glIndexubv, NULL, 316),
    NAME_FUNC_OFFSET( 3935, glInterleavedArrays, glInterleavedArrays, NULL, 317),
    NAME_FUNC_OFFSET( 3955, glNormalPointer, glNormalPointer, NULL, 318),
    NAME_FUNC_OFFSET( 3971, glPolygonOffset, glPolygonOffset, NULL, 319),
    NAME_FUNC_OFFSET( 3987, glTexCoordPointer, glTexCoordPointer, NULL, 320),
    NAME_FUNC_OFFSET( 4005, glVertexPointer, glVertexPointer, NULL, 321),
    NAME_FUNC_OFFSET( 4021, glAreTexturesResident, glAreTexturesResident, NULL, 322),
    NAME_FUNC_OFFSET( 4043, glCopyTexImage1D, glCopyTexImage1D, NULL, 323),
    NAME_FUNC_OFFSET( 4060, glCopyTexImage2D, glCopyTexImage2D, NULL, 324),
    NAME_FUNC_OFFSET( 4077, glCopyTexSubImage1D, glCopyTexSubImage1D, NULL, 325),
    NAME_FUNC_OFFSET( 4097, glCopyTexSubImage2D, glCopyTexSubImage2D, NULL, 326),
    NAME_FUNC_OFFSET( 4117, glDeleteTextures, glDeleteTextures, NULL, 327),
    NAME_FUNC_OFFSET( 4134, glGenTextures, glGenTextures, NULL, 328),
    NAME_FUNC_OFFSET( 4148, glGetPointerv, glGetPointerv, NULL, 329),
    NAME_FUNC_OFFSET( 4162, glIsTexture, glIsTexture, NULL, 330),
    NAME_FUNC_OFFSET( 4174, glPrioritizeTextures, glPrioritizeTextures, NULL, 331),
    NAME_FUNC_OFFSET( 4195, glTexSubImage1D, glTexSubImage1D, NULL, 332),
    NAME_FUNC_OFFSET( 4211, glTexSubImage2D, glTexSubImage2D, NULL, 333),
    NAME_FUNC_OFFSET( 4227, glPopClientAttrib, glPopClientAttrib, NULL, 334),
    NAME_FUNC_OFFSET( 4245, glPushClientAttrib, glPushClientAttrib, NULL, 335),
    NAME_FUNC_OFFSET( 4264, glBlendColor, glBlendColor, NULL, 336),
    NAME_FUNC_OFFSET( 4277, glBlendEquation, glBlendEquation, NULL, 337),
    NAME_FUNC_OFFSET( 4293, glDrawRangeElements, glDrawRangeElements, NULL, 338),
    NAME_FUNC_OFFSET( 4313, glColorTable, glColorTable, NULL, 339),
    NAME_FUNC_OFFSET( 4326, glColorTableParameterfv, glColorTableParameterfv, NULL, 340),
    NAME_FUNC_OFFSET( 4350, glColorTableParameteriv, glColorTableParameteriv, NULL, 341),
    NAME_FUNC_OFFSET( 4374, glCopyColorTable, glCopyColorTable, NULL, 342),
    NAME_FUNC_OFFSET( 4391, glGetColorTable, glGetColorTable, NULL, 343),
    NAME_FUNC_OFFSET( 4407, glGetColorTableParameterfv, glGetColorTableParameterfv, NULL, 344),
    NAME_FUNC_OFFSET( 4434, glGetColorTableParameteriv, glGetColorTableParameteriv, NULL, 345),
    NAME_FUNC_OFFSET( 4461, glColorSubTable, glColorSubTable, NULL, 346),
    NAME_FUNC_OFFSET( 4477, glCopyColorSubTable, glCopyColorSubTable, NULL, 347),
    NAME_FUNC_OFFSET( 4497, glConvolutionFilter1D, glConvolutionFilter1D, NULL, 348),
    NAME_FUNC_OFFSET( 4519, glConvolutionFilter2D, glConvolutionFilter2D, NULL, 349),
    NAME_FUNC_OFFSET( 4541, glConvolutionParameterf, glConvolutionParameterf, NULL, 350),
    NAME_FUNC_OFFSET( 4565, glConvolutionParameterfv, glConvolutionParameterfv, NULL, 351),
    NAME_FUNC_OFFSET( 4590, glConvolutionParameteri, glConvolutionParameteri, NULL, 352),
    NAME_FUNC_OFFSET( 4614, glConvolutionParameteriv, glConvolutionParameteriv, NULL, 353),
    NAME_FUNC_OFFSET( 4639, glCopyConvolutionFilter1D, glCopyConvolutionFilter1D, NULL, 354),
    NAME_FUNC_OFFSET( 4665, glCopyConvolutionFilter2D, glCopyConvolutionFilter2D, NULL, 355),
    NAME_FUNC_OFFSET( 4691, glGetConvolutionFilter, glGetConvolutionFilter, NULL, 356),
    NAME_FUNC_OFFSET( 4714, glGetConvolutionParameterfv, glGetConvolutionParameterfv, NULL, 357),
    NAME_FUNC_OFFSET( 4742, glGetConvolutionParameteriv, glGetConvolutionParameteriv, NULL, 358),
    NAME_FUNC_OFFSET( 4770, glGetSeparableFilter, glGetSeparableFilter, NULL, 359),
    NAME_FUNC_OFFSET( 4791, glSeparableFilter2D, glSeparableFilter2D, NULL, 360),
    NAME_FUNC_OFFSET( 4811, glGetHistogram, glGetHistogram, NULL, 361),
    NAME_FUNC_OFFSET( 4826, glGetHistogramParameterfv, glGetHistogramParameterfv, NULL, 362),
    NAME_FUNC_OFFSET( 4852, glGetHistogramParameteriv, glGetHistogramParameteriv, NULL, 363),
    NAME_FUNC_OFFSET( 4878, glGetMinmax, glGetMinmax, NULL, 364),
    NAME_FUNC_OFFSET( 4890, glGetMinmaxParameterfv, glGetMinmaxParameterfv, NULL, 365),
    NAME_FUNC_OFFSET( 4913, glGetMinmaxParameteriv, glGetMinmaxParameteriv, NULL, 366),
    NAME_FUNC_OFFSET( 4936, glHistogram, glHistogram, NULL, 367),
    NAME_FUNC_OFFSET( 4948, glMinmax, glMinmax, NULL, 368),
    NAME_FUNC_OFFSET( 4957, glResetHistogram, glResetHistogram, NULL, 369),
    NAME_FUNC_OFFSET( 4974, glResetMinmax, glResetMinmax, NULL, 370),
    NAME_FUNC_OFFSET( 4988, glTexImage3D, glTexImage3D, NULL, 371),
    NAME_FUNC_OFFSET( 5001, glTexSubImage3D, glTexSubImage3D, NULL, 372),
    NAME_FUNC_OFFSET( 5017, glCopyTexSubImage3D, glCopyTexSubImage3D, NULL, 373),
    NAME_FUNC_OFFSET( 5037, glActiveTexture, glActiveTexture, NULL, 374),
    NAME_FUNC_OFFSET( 5053, glClientActiveTexture, glClientActiveTexture, NULL, 375),
    NAME_FUNC_OFFSET( 5075, glMultiTexCoord1d, glMultiTexCoord1d, NULL, 376),
    NAME_FUNC_OFFSET( 5093, glMultiTexCoord1dv, glMultiTexCoord1dv, NULL, 377),
    NAME_FUNC_OFFSET( 5112, glMultiTexCoord1fARB, glMultiTexCoord1fARB, NULL, 378),
    NAME_FUNC_OFFSET( 5133, glMultiTexCoord1fvARB, glMultiTexCoord1fvARB, NULL, 379),
    NAME_FUNC_OFFSET( 5155, glMultiTexCoord1i, glMultiTexCoord1i, NULL, 380),
    NAME_FUNC_OFFSET( 5173, glMultiTexCoord1iv, glMultiTexCoord1iv, NULL, 381),
    NAME_FUNC_OFFSET( 5192, glMultiTexCoord1s, glMultiTexCoord1s, NULL, 382),
    NAME_FUNC_OFFSET( 5210, glMultiTexCoord1sv, glMultiTexCoord1sv, NULL, 383),
    NAME_FUNC_OFFSET( 5229, glMultiTexCoord2d, glMultiTexCoord2d, NULL, 384),
    NAME_FUNC_OFFSET( 5247, glMultiTexCoord2dv, glMultiTexCoord2dv, NULL, 385),
    NAME_FUNC_OFFSET( 5266, glMultiTexCoord2fARB, glMultiTexCoord2fARB, NULL, 386),
    NAME_FUNC_OFFSET( 5287, glMultiTexCoord2fvARB, glMultiTexCoord2fvARB, NULL, 387),
    NAME_FUNC_OFFSET( 5309, glMultiTexCoord2i, glMultiTexCoord2i, NULL, 388),
    NAME_FUNC_OFFSET( 5327, glMultiTexCoord2iv, glMultiTexCoord2iv, NULL, 389),
    NAME_FUNC_OFFSET( 5346, glMultiTexCoord2s, glMultiTexCoord2s, NULL, 390),
    NAME_FUNC_OFFSET( 5364, glMultiTexCoord2sv, glMultiTexCoord2sv, NULL, 391),
    NAME_FUNC_OFFSET( 5383, glMultiTexCoord3d, glMultiTexCoord3d, NULL, 392),
    NAME_FUNC_OFFSET( 5401, glMultiTexCoord3dv, glMultiTexCoord3dv, NULL, 393),
    NAME_FUNC_OFFSET( 5420, glMultiTexCoord3fARB, glMultiTexCoord3fARB, NULL, 394),
    NAME_FUNC_OFFSET( 5441, glMultiTexCoord3fvARB, glMultiTexCoord3fvARB, NULL, 395),
    NAME_FUNC_OFFSET( 5463, glMultiTexCoord3i, glMultiTexCoord3i, NULL, 396),
    NAME_FUNC_OFFSET( 5481, glMultiTexCoord3iv, glMultiTexCoord3iv, NULL, 397),
    NAME_FUNC_OFFSET( 5500, glMultiTexCoord3s, glMultiTexCoord3s, NULL, 398),
    NAME_FUNC_OFFSET( 5518, glMultiTexCoord3sv, glMultiTexCoord3sv, NULL, 399),
    NAME_FUNC_OFFSET( 5537, glMultiTexCoord4d, glMultiTexCoord4d, NULL, 400),
    NAME_FUNC_OFFSET( 5555, glMultiTexCoord4dv, glMultiTexCoord4dv, NULL, 401),
    NAME_FUNC_OFFSET( 5574, glMultiTexCoord4fARB, glMultiTexCoord4fARB, NULL, 402),
    NAME_FUNC_OFFSET( 5595, glMultiTexCoord4fvARB, glMultiTexCoord4fvARB, NULL, 403),
    NAME_FUNC_OFFSET( 5617, glMultiTexCoord4i, glMultiTexCoord4i, NULL, 404),
    NAME_FUNC_OFFSET( 5635, glMultiTexCoord4iv, glMultiTexCoord4iv, NULL, 405),
    NAME_FUNC_OFFSET( 5654, glMultiTexCoord4s, glMultiTexCoord4s, NULL, 406),
    NAME_FUNC_OFFSET( 5672, glMultiTexCoord4sv, glMultiTexCoord4sv, NULL, 407),
    NAME_FUNC_OFFSET( 5691, glCompressedTexImage1D, glCompressedTexImage1D, NULL, 408),
    NAME_FUNC_OFFSET( 5714, glCompressedTexImage2D, glCompressedTexImage2D, NULL, 409),
    NAME_FUNC_OFFSET( 5737, glCompressedTexImage3D, glCompressedTexImage3D, NULL, 410),
    NAME_FUNC_OFFSET( 5760, glCompressedTexSubImage1D, glCompressedTexSubImage1D, NULL, 411),
    NAME_FUNC_OFFSET( 5786, glCompressedTexSubImage2D, glCompressedTexSubImage2D, NULL, 412),
    NAME_FUNC_OFFSET( 5812, glCompressedTexSubImage3D, glCompressedTexSubImage3D, NULL, 413),
    NAME_FUNC_OFFSET( 5838, glGetCompressedTexImage, glGetCompressedTexImage, NULL, 414),
    NAME_FUNC_OFFSET( 5862, glLoadTransposeMatrixd, glLoadTransposeMatrixd, NULL, 415),
    NAME_FUNC_OFFSET( 5885, glLoadTransposeMatrixf, glLoadTransposeMatrixf, NULL, 416),
    NAME_FUNC_OFFSET( 5908, glMultTransposeMatrixd, glMultTransposeMatrixd, NULL, 417),
    NAME_FUNC_OFFSET( 5931, glMultTransposeMatrixf, glMultTransposeMatrixf, NULL, 418),
    NAME_FUNC_OFFSET( 5954, glSampleCoverage, glSampleCoverage, NULL, 419),
    NAME_FUNC_OFFSET( 5971, glBlendFuncSeparate, glBlendFuncSeparate, NULL, 420),
    NAME_FUNC_OFFSET( 5991, glFogCoordPointer, glFogCoordPointer, NULL, 421),
    NAME_FUNC_OFFSET( 6009, glFogCoordd, glFogCoordd, NULL, 422),
    NAME_FUNC_OFFSET( 6021, glFogCoorddv, glFogCoorddv, NULL, 423),
    NAME_FUNC_OFFSET( 6034, glMultiDrawArrays, glMultiDrawArrays, NULL, 424),
    NAME_FUNC_OFFSET( 6052, glPointParameterf, glPointParameterf, NULL, 425),
    NAME_FUNC_OFFSET( 6070, glPointParameterfv, glPointParameterfv, NULL, 426),
    NAME_FUNC_OFFSET( 6089, glPointParameteri, glPointParameteri, NULL, 427),
    NAME_FUNC_OFFSET( 6107, glPointParameteriv, glPointParameteriv, NULL, 428),
    NAME_FUNC_OFFSET( 6126, glSecondaryColor3b, glSecondaryColor3b, NULL, 429),
    NAME_FUNC_OFFSET( 6145, glSecondaryColor3bv, glSecondaryColor3bv, NULL, 430),
    NAME_FUNC_OFFSET( 6165, glSecondaryColor3d, glSecondaryColor3d, NULL, 431),
    NAME_FUNC_OFFSET( 6184, glSecondaryColor3dv, glSecondaryColor3dv, NULL, 432),
    NAME_FUNC_OFFSET( 6204, glSecondaryColor3i, glSecondaryColor3i, NULL, 433),
    NAME_FUNC_OFFSET( 6223, glSecondaryColor3iv, glSecondaryColor3iv, NULL, 434),
    NAME_FUNC_OFFSET( 6243, glSecondaryColor3s, glSecondaryColor3s, NULL, 435),
    NAME_FUNC_OFFSET( 6262, glSecondaryColor3sv, glSecondaryColor3sv, NULL, 436),
    NAME_FUNC_OFFSET( 6282, glSecondaryColor3ub, glSecondaryColor3ub, NULL, 437),
    NAME_FUNC_OFFSET( 6302, glSecondaryColor3ubv, glSecondaryColor3ubv, NULL, 438),
    NAME_FUNC_OFFSET( 6323, glSecondaryColor3ui, glSecondaryColor3ui, NULL, 439),
    NAME_FUNC_OFFSET( 6343, glSecondaryColor3uiv, glSecondaryColor3uiv, NULL, 440),
    NAME_FUNC_OFFSET( 6364, glSecondaryColor3us, glSecondaryColor3us, NULL, 441),
    NAME_FUNC_OFFSET( 6384, glSecondaryColor3usv, glSecondaryColor3usv, NULL, 442),
    NAME_FUNC_OFFSET( 6405, glSecondaryColorPointer, glSecondaryColorPointer, NULL, 443),
    NAME_FUNC_OFFSET( 6429, glWindowPos2d, glWindowPos2d, NULL, 444),
    NAME_FUNC_OFFSET( 6443, glWindowPos2dv, glWindowPos2dv, NULL, 445),
    NAME_FUNC_OFFSET( 6458, glWindowPos2f, glWindowPos2f, NULL, 446),
    NAME_FUNC_OFFSET( 6472, glWindowPos2fv, glWindowPos2fv, NULL, 447),
    NAME_FUNC_OFFSET( 6487, glWindowPos2i, glWindowPos2i, NULL, 448),
    NAME_FUNC_OFFSET( 6501, glWindowPos2iv, glWindowPos2iv, NULL, 449),
    NAME_FUNC_OFFSET( 6516, glWindowPos2s, glWindowPos2s, NULL, 450),
    NAME_FUNC_OFFSET( 6530, glWindowPos2sv, glWindowPos2sv, NULL, 451),
    NAME_FUNC_OFFSET( 6545, glWindowPos3d, glWindowPos3d, NULL, 452),
    NAME_FUNC_OFFSET( 6559, glWindowPos3dv, glWindowPos3dv, NULL, 453),
    NAME_FUNC_OFFSET( 6574, glWindowPos3f, glWindowPos3f, NULL, 454),
    NAME_FUNC_OFFSET( 6588, glWindowPos3fv, glWindowPos3fv, NULL, 455),
    NAME_FUNC_OFFSET( 6603, glWindowPos3i, glWindowPos3i, NULL, 456),
    NAME_FUNC_OFFSET( 6617, glWindowPos3iv, glWindowPos3iv, NULL, 457),
    NAME_FUNC_OFFSET( 6632, glWindowPos3s, glWindowPos3s, NULL, 458),
    NAME_FUNC_OFFSET( 6646, glWindowPos3sv, glWindowPos3sv, NULL, 459),
    NAME_FUNC_OFFSET( 6661, glBeginQuery, glBeginQuery, NULL, 460),
    NAME_FUNC_OFFSET( 6674, glBindBuffer, glBindBuffer, NULL, 461),
    NAME_FUNC_OFFSET( 6687, glBufferData, glBufferData, NULL, 462),
    NAME_FUNC_OFFSET( 6700, glBufferSubData, glBufferSubData, NULL, 463),
    NAME_FUNC_OFFSET( 6716, glDeleteBuffers, glDeleteBuffers, NULL, 464),
    NAME_FUNC_OFFSET( 6732, glDeleteQueries, glDeleteQueries, NULL, 465),
    NAME_FUNC_OFFSET( 6748, glEndQuery, glEndQuery, NULL, 466),
    NAME_FUNC_OFFSET( 6759, glGenBuffers, glGenBuffers, NULL, 467),
    NAME_FUNC_OFFSET( 6772, glGenQueries, glGenQueries, NULL, 468),
    NAME_FUNC_OFFSET( 6785, glGetBufferParameteriv, glGetBufferParameteriv, NULL, 469),
    NAME_FUNC_OFFSET( 6808, glGetBufferPointerv, glGetBufferPointerv, NULL, 470),
    NAME_FUNC_OFFSET( 6828, glGetBufferSubData, glGetBufferSubData, NULL, 471),
    NAME_FUNC_OFFSET( 6847, glGetQueryObjectiv, glGetQueryObjectiv, NULL, 472),
    NAME_FUNC_OFFSET( 6866, glGetQueryObjectuiv, glGetQueryObjectuiv, NULL, 473),
    NAME_FUNC_OFFSET( 6886, glGetQueryiv, glGetQueryiv, NULL, 474),
    NAME_FUNC_OFFSET( 6899, glIsBuffer, glIsBuffer, NULL, 475),
    NAME_FUNC_OFFSET( 6910, glIsQuery, glIsQuery, NULL, 476),
    NAME_FUNC_OFFSET( 6920, glMapBuffer, glMapBuffer, NULL, 477),
    NAME_FUNC_OFFSET( 6932, glUnmapBuffer, glUnmapBuffer, NULL, 478),
    NAME_FUNC_OFFSET( 6946, glAttachShader, glAttachShader, NULL, 479),
    NAME_FUNC_OFFSET( 6961, glBindAttribLocation, glBindAttribLocation, NULL, 480),
    NAME_FUNC_OFFSET( 6982, glBlendEquationSeparate, glBlendEquationSeparate, NULL, 481),
    NAME_FUNC_OFFSET( 7006, glCompileShader, glCompileShader, NULL, 482),
    NAME_FUNC_OFFSET( 7022, glCreateProgram, glCreateProgram, NULL, 483),
    NAME_FUNC_OFFSET( 7038, glCreateShader, glCreateShader, NULL, 484),
    NAME_FUNC_OFFSET( 7053, glDeleteProgram, glDeleteProgram, NULL, 485),
    NAME_FUNC_OFFSET( 7069, glDeleteShader, glDeleteShader, NULL, 486),
    NAME_FUNC_OFFSET( 7084, glDetachShader, glDetachShader, NULL, 487),
    NAME_FUNC_OFFSET( 7099, glDisableVertexAttribArray, glDisableVertexAttribArray, NULL, 488),
    NAME_FUNC_OFFSET( 7126, glDrawBuffers, glDrawBuffers, NULL, 489),
    NAME_FUNC_OFFSET( 7140, glEnableVertexAttribArray, glEnableVertexAttribArray, NULL, 490),
    NAME_FUNC_OFFSET( 7166, glGetActiveAttrib, glGetActiveAttrib, NULL, 491),
    NAME_FUNC_OFFSET( 7184, glGetActiveUniform, glGetActiveUniform, NULL, 492),
    NAME_FUNC_OFFSET( 7203, glGetAttachedShaders, glGetAttachedShaders, NULL, 493),
    NAME_FUNC_OFFSET( 7224, glGetAttribLocation, glGetAttribLocation, NULL, 494),
    NAME_FUNC_OFFSET( 7244, glGetProgramInfoLog, glGetProgramInfoLog, NULL, 495),
    NAME_FUNC_OFFSET( 7264, glGetProgramiv, glGetProgramiv, NULL, 496),
    NAME_FUNC_OFFSET( 7279, glGetShaderInfoLog, glGetShaderInfoLog, NULL, 497),
    NAME_FUNC_OFFSET( 7298, glGetShaderSource, glGetShaderSource, NULL, 498),
    NAME_FUNC_OFFSET( 7316, glGetShaderiv, glGetShaderiv, NULL, 499),
    NAME_FUNC_OFFSET( 7330, glGetUniformLocation, glGetUniformLocation, NULL, 500),
    NAME_FUNC_OFFSET( 7351, glGetUniformfv, glGetUniformfv, NULL, 501),
    NAME_FUNC_OFFSET( 7366, glGetUniformiv, glGetUniformiv, NULL, 502),
    NAME_FUNC_OFFSET( 7381, glGetVertexAttribPointerv, glGetVertexAttribPointerv, NULL, 503),
    NAME_FUNC_OFFSET( 7407, glGetVertexAttribdv, glGetVertexAttribdv, NULL, 504),
    NAME_FUNC_OFFSET( 7427, glGetVertexAttribfv, glGetVertexAttribfv, NULL, 505),
    NAME_FUNC_OFFSET( 7447, glGetVertexAttribiv, glGetVertexAttribiv, NULL, 506),
    NAME_FUNC_OFFSET( 7467, glIsProgram, glIsProgram, NULL, 507),
    NAME_FUNC_OFFSET( 7479, glIsShader, glIsShader, NULL, 508),
    NAME_FUNC_OFFSET( 7490, glLinkProgram, glLinkProgram, NULL, 509),
    NAME_FUNC_OFFSET( 7504, glShaderSource, glShaderSource, NULL, 510),
    NAME_FUNC_OFFSET( 7519, glStencilFuncSeparate, glStencilFuncSeparate, NULL, 511),
    NAME_FUNC_OFFSET( 7541, glStencilMaskSeparate, glStencilMaskSeparate, NULL, 512),
    NAME_FUNC_OFFSET( 7563, glStencilOpSeparate, glStencilOpSeparate, NULL, 513),
    NAME_FUNC_OFFSET( 7583, glUniform1f, glUniform1f, NULL, 514),
    NAME_FUNC_OFFSET( 7595, glUniform1fv, glUniform1fv, NULL, 515),
    NAME_FUNC_OFFSET( 7608, glUniform1i, glUniform1i, NULL, 516),
    NAME_FUNC_OFFSET( 7620, glUniform1iv, glUniform1iv, NULL, 517),
    NAME_FUNC_OFFSET( 7633, glUniform2f, glUniform2f, NULL, 518),
    NAME_FUNC_OFFSET( 7645, glUniform2fv, glUniform2fv, NULL, 519),
    NAME_FUNC_OFFSET( 7658, glUniform2i, glUniform2i, NULL, 520),
    NAME_FUNC_OFFSET( 7670, glUniform2iv, glUniform2iv, NULL, 521),
    NAME_FUNC_OFFSET( 7683, glUniform3f, glUniform3f, NULL, 522),
    NAME_FUNC_OFFSET( 7695, glUniform3fv, glUniform3fv, NULL, 523),
    NAME_FUNC_OFFSET( 7708, glUniform3i, glUniform3i, NULL, 524),
    NAME_FUNC_OFFSET( 7720, glUniform3iv, glUniform3iv, NULL, 525),
    NAME_FUNC_OFFSET( 7733, glUniform4f, glUniform4f, NULL, 526),
    NAME_FUNC_OFFSET( 7745, glUniform4fv, glUniform4fv, NULL, 527),
    NAME_FUNC_OFFSET( 7758, glUniform4i, glUniform4i, NULL, 528),
    NAME_FUNC_OFFSET( 7770, glUniform4iv, glUniform4iv, NULL, 529),
    NAME_FUNC_OFFSET( 7783, glUniformMatrix2fv, glUniformMatrix2fv, NULL, 530),
    NAME_FUNC_OFFSET( 7802, glUniformMatrix3fv, glUniformMatrix3fv, NULL, 531),
    NAME_FUNC_OFFSET( 7821, glUniformMatrix4fv, glUniformMatrix4fv, NULL, 532),
    NAME_FUNC_OFFSET( 7840, glUseProgram, glUseProgram, NULL, 533),
    NAME_FUNC_OFFSET( 7853, glValidateProgram, glValidateProgram, NULL, 534),
    NAME_FUNC_OFFSET( 7871, glVertexAttrib1d, glVertexAttrib1d, NULL, 535),
    NAME_FUNC_OFFSET( 7888, glVertexAttrib1dv, glVertexAttrib1dv, NULL, 536),
    NAME_FUNC_OFFSET( 7906, glVertexAttrib1s, glVertexAttrib1s, NULL, 537),
    NAME_FUNC_OFFSET( 7923, glVertexAttrib1sv, glVertexAttrib1sv, NULL, 538),
    NAME_FUNC_OFFSET( 7941, glVertexAttrib2d, glVertexAttrib2d, NULL, 539),
    NAME_FUNC_OFFSET( 7958, glVertexAttrib2dv, glVertexAttrib2dv, NULL, 540),
    NAME_FUNC_OFFSET( 7976, glVertexAttrib2s, glVertexAttrib2s, NULL, 541),
    NAME_FUNC_OFFSET( 7993, glVertexAttrib2sv, glVertexAttrib2sv, NULL, 542),
    NAME_FUNC_OFFSET( 8011, glVertexAttrib3d, glVertexAttrib3d, NULL, 543),
    NAME_FUNC_OFFSET( 8028, glVertexAttrib3dv, glVertexAttrib3dv, NULL, 544),
    NAME_FUNC_OFFSET( 8046, glVertexAttrib3s, glVertexAttrib3s, NULL, 545),
    NAME_FUNC_OFFSET( 8063, glVertexAttrib3sv, glVertexAttrib3sv, NULL, 546),
    NAME_FUNC_OFFSET( 8081, glVertexAttrib4Nbv, glVertexAttrib4Nbv, NULL, 547),
    NAME_FUNC_OFFSET( 8100, glVertexAttrib4Niv, glVertexAttrib4Niv, NULL, 548),
    NAME_FUNC_OFFSET( 8119, glVertexAttrib4Nsv, glVertexAttrib4Nsv, NULL, 549),
    NAME_FUNC_OFFSET( 8138, glVertexAttrib4Nub, glVertexAttrib4Nub, NULL, 550),
    NAME_FUNC_OFFSET( 8157, glVertexAttrib4Nubv, glVertexAttrib4Nubv, NULL, 551),
    NAME_FUNC_OFFSET( 8177, glVertexAttrib4Nuiv, glVertexAttrib4Nuiv, NULL, 552),
    NAME_FUNC_OFFSET( 8197, glVertexAttrib4Nusv, glVertexAttrib4Nusv, NULL, 553),
    NAME_FUNC_OFFSET( 8217, glVertexAttrib4bv, glVertexAttrib4bv, NULL, 554),
    NAME_FUNC_OFFSET( 8235, glVertexAttrib4d, glVertexAttrib4d, NULL, 555),
    NAME_FUNC_OFFSET( 8252, glVertexAttrib4dv, glVertexAttrib4dv, NULL, 556),
    NAME_FUNC_OFFSET( 8270, glVertexAttrib4iv, glVertexAttrib4iv, NULL, 557),
    NAME_FUNC_OFFSET( 8288, glVertexAttrib4s, glVertexAttrib4s, NULL, 558),
    NAME_FUNC_OFFSET( 8305, glVertexAttrib4sv, glVertexAttrib4sv, NULL, 559),
    NAME_FUNC_OFFSET( 8323, glVertexAttrib4ubv, glVertexAttrib4ubv, NULL, 560),
    NAME_FUNC_OFFSET( 8342, glVertexAttrib4uiv, glVertexAttrib4uiv, NULL, 561),
    NAME_FUNC_OFFSET( 8361, glVertexAttrib4usv, glVertexAttrib4usv, NULL, 562),
    NAME_FUNC_OFFSET( 8380, glVertexAttribPointer, glVertexAttribPointer, NULL, 563),
    NAME_FUNC_OFFSET( 8402, glUniformMatrix2x3fv, glUniformMatrix2x3fv, NULL, 564),
    NAME_FUNC_OFFSET( 8423, glUniformMatrix2x4fv, glUniformMatrix2x4fv, NULL, 565),
    NAME_FUNC_OFFSET( 8444, glUniformMatrix3x2fv, glUniformMatrix3x2fv, NULL, 566),
    NAME_FUNC_OFFSET( 8465, glUniformMatrix3x4fv, glUniformMatrix3x4fv, NULL, 567),
    NAME_FUNC_OFFSET( 8486, glUniformMatrix4x2fv, glUniformMatrix4x2fv, NULL, 568),
    NAME_FUNC_OFFSET( 8507, glUniformMatrix4x3fv, glUniformMatrix4x3fv, NULL, 569),
    NAME_FUNC_OFFSET( 8528, glBeginConditionalRender, glBeginConditionalRender, NULL, 570),
    NAME_FUNC_OFFSET( 8553, glBeginTransformFeedback, glBeginTransformFeedback, NULL, 571),
    NAME_FUNC_OFFSET( 8578, glBindBufferBase, glBindBufferBase, NULL, 572),
    NAME_FUNC_OFFSET( 8595, glBindBufferRange, glBindBufferRange, NULL, 573),
    NAME_FUNC_OFFSET( 8613, glBindFragDataLocation, glBindFragDataLocation, NULL, 574),
    NAME_FUNC_OFFSET( 8636, glClampColor, glClampColor, NULL, 575),
    NAME_FUNC_OFFSET( 8649, glClearBufferfi, glClearBufferfi, NULL, 576),
    NAME_FUNC_OFFSET( 8665, glClearBufferfv, glClearBufferfv, NULL, 577),
    NAME_FUNC_OFFSET( 8681, glClearBufferiv, glClearBufferiv, NULL, 578),
    NAME_FUNC_OFFSET( 8697, glClearBufferuiv, glClearBufferuiv, NULL, 579),
    NAME_FUNC_OFFSET( 8714, glColorMaski, glColorMaski, NULL, 580),
    NAME_FUNC_OFFSET( 8727, glDisablei, glDisablei, NULL, 581),
    NAME_FUNC_OFFSET( 8738, glEnablei, glEnablei, NULL, 582),
    NAME_FUNC_OFFSET( 8748, glEndConditionalRender, glEndConditionalRender, NULL, 583),
    NAME_FUNC_OFFSET( 8771, glEndTransformFeedback, glEndTransformFeedback, NULL, 584),
    NAME_FUNC_OFFSET( 8794, glGetBooleani_v, glGetBooleani_v, NULL, 585),
    NAME_FUNC_OFFSET( 8810, glGetFragDataLocation, glGetFragDataLocation, NULL, 586),
    NAME_FUNC_OFFSET( 8832, glGetIntegeri_v, glGetIntegeri_v, NULL, 587),
    NAME_FUNC_OFFSET( 8848, glGetStringi, glGetStringi, NULL, 588),
    NAME_FUNC_OFFSET( 8861, glGetTexParameterIiv, glGetTexParameterIiv, NULL, 589),
    NAME_FUNC_OFFSET( 8882, glGetTexParameterIuiv, glGetTexParameterIuiv, NULL, 590),
    NAME_FUNC_OFFSET( 8904, glGetTransformFeedbackVarying, glGetTransformFeedbackVarying, NULL, 591),
    NAME_FUNC_OFFSET( 8934, glGetUniformuiv, glGetUniformuiv, NULL, 592),
    NAME_FUNC_OFFSET( 8950, glGetVertexAttribIiv, glGetVertexAttribIiv, NULL, 593),
    NAME_FUNC_OFFSET( 8971, glGetVertexAttribIuiv, glGetVertexAttribIuiv, NULL, 594),
    NAME_FUNC_OFFSET( 8993, glIsEnabledi, glIsEnabledi, NULL, 595),
    NAME_FUNC_OFFSET( 9006, glTexParameterIiv, glTexParameterIiv, NULL, 596),
    NAME_FUNC_OFFSET( 9024, glTexParameterIuiv, glTexParameterIuiv, NULL, 597),
    NAME_FUNC_OFFSET( 9043, glTransformFeedbackVaryings, glTransformFeedbackVaryings, NULL, 598),
    NAME_FUNC_OFFSET( 9071, glUniform1ui, glUniform1ui, NULL, 599),
    NAME_FUNC_OFFSET( 9084, glUniform1uiv, glUniform1uiv, NULL, 600),
    NAME_FUNC_OFFSET( 9098, glUniform2ui, glUniform2ui, NULL, 601),
    NAME_FUNC_OFFSET( 9111, glUniform2uiv, glUniform2uiv, NULL, 602),
    NAME_FUNC_OFFSET( 9125, glUniform3ui, glUniform3ui, NULL, 603),
    NAME_FUNC_OFFSET( 9138, glUniform3uiv, glUniform3uiv, NULL, 604),
    NAME_FUNC_OFFSET( 9152, glUniform4ui, glUniform4ui, NULL, 605),
    NAME_FUNC_OFFSET( 9165, glUniform4uiv, glUniform4uiv, NULL, 606),
    NAME_FUNC_OFFSET( 9179, glVertexAttribI1iv, glVertexAttribI1iv, NULL, 607),
    NAME_FUNC_OFFSET( 9198, glVertexAttribI1uiv, glVertexAttribI1uiv, NULL, 608),
    NAME_FUNC_OFFSET( 9218, glVertexAttribI4bv, glVertexAttribI4bv, NULL, 609),
    NAME_FUNC_OFFSET( 9237, glVertexAttribI4sv, glVertexAttribI4sv, NULL, 610),
    NAME_FUNC_OFFSET( 9256, glVertexAttribI4ubv, glVertexAttribI4ubv, NULL, 611),
    NAME_FUNC_OFFSET( 9276, glVertexAttribI4usv, glVertexAttribI4usv, NULL, 612),
    NAME_FUNC_OFFSET( 9296, glVertexAttribIPointer, glVertexAttribIPointer, NULL, 613),
    NAME_FUNC_OFFSET( 9319, glPrimitiveRestartIndex, glPrimitiveRestartIndex, NULL, 614),
    NAME_FUNC_OFFSET( 9343, glTexBuffer, glTexBuffer, NULL, 615),
    NAME_FUNC_OFFSET( 9355, glFramebufferTexture, glFramebufferTexture, NULL, 616),
    NAME_FUNC_OFFSET( 9376, glGetBufferParameteri64v, glGetBufferParameteri64v, NULL, 617),
    NAME_FUNC_OFFSET( 9401, glGetInteger64i_v, glGetInteger64i_v, NULL, 618),
    NAME_FUNC_OFFSET( 9419, glVertexAttribDivisor, glVertexAttribDivisor, NULL, 619),
    NAME_FUNC_OFFSET( 9441, glMinSampleShading, glMinSampleShading, NULL, 620),
    NAME_FUNC_OFFSET( 9460, gl_dispatch_stub_621, gl_dispatch_stub_621, NULL, 621),
    NAME_FUNC_OFFSET( 9484, glBindProgramARB, glBindProgramARB, NULL, 622),
    NAME_FUNC_OFFSET( 9501, glDeleteProgramsARB, glDeleteProgramsARB, NULL, 623),
    NAME_FUNC_OFFSET( 9521, glGenProgramsARB, glGenProgramsARB, NULL, 624),
    NAME_FUNC_OFFSET( 9538, glGetProgramEnvParameterdvARB, glGetProgramEnvParameterdvARB, NULL, 625),
    NAME_FUNC_OFFSET( 9568, glGetProgramEnvParameterfvARB, glGetProgramEnvParameterfvARB, NULL, 626),
    NAME_FUNC_OFFSET( 9598, glGetProgramLocalParameterdvARB, glGetProgramLocalParameterdvARB, NULL, 627),
    NAME_FUNC_OFFSET( 9630, glGetProgramLocalParameterfvARB, glGetProgramLocalParameterfvARB, NULL, 628),
    NAME_FUNC_OFFSET( 9662, glGetProgramStringARB, glGetProgramStringARB, NULL, 629),
    NAME_FUNC_OFFSET( 9684, glGetProgramivARB, glGetProgramivARB, NULL, 630),
    NAME_FUNC_OFFSET( 9702, glIsProgramARB, glIsProgramARB, NULL, 631),
    NAME_FUNC_OFFSET( 9717, glProgramEnvParameter4dARB, glProgramEnvParameter4dARB, NULL, 632),
    NAME_FUNC_OFFSET( 9744, glProgramEnvParameter4dvARB, glProgramEnvParameter4dvARB, NULL, 633),
    NAME_FUNC_OFFSET( 9772, glProgramEnvParameter4fARB, glProgramEnvParameter4fARB, NULL, 634),
    NAME_FUNC_OFFSET( 9799, glProgramEnvParameter4fvARB, glProgramEnvParameter4fvARB, NULL, 635),
    NAME_FUNC_OFFSET( 9827, glProgramLocalParameter4dARB, glProgramLocalParameter4dARB, NULL, 636),
    NAME_FUNC_OFFSET( 9856, glProgramLocalParameter4dvARB, glProgramLocalParameter4dvARB, NULL, 637),
    NAME_FUNC_OFFSET( 9886, glProgramLocalParameter4fARB, glProgramLocalParameter4fARB, NULL, 638),
    NAME_FUNC_OFFSET( 9915, glProgramLocalParameter4fvARB, glProgramLocalParameter4fvARB, NULL, 639),
    NAME_FUNC_OFFSET( 9945, glProgramStringARB, glProgramStringARB, NULL, 640),
    NAME_FUNC_OFFSET( 9964, glVertexAttrib1fARB, glVertexAttrib1fARB, NULL, 641),
    NAME_FUNC_OFFSET( 9984, glVertexAttrib1fvARB, glVertexAttrib1fvARB, NULL, 642),
    NAME_FUNC_OFFSET(10005, glVertexAttrib2fARB, glVertexAttrib2fARB, NULL, 643),
    NAME_FUNC_OFFSET(10025, glVertexAttrib2fvARB, glVertexAttrib2fvARB, NULL, 644),
    NAME_FUNC_OFFSET(10046, glVertexAttrib3fARB, glVertexAttrib3fARB, NULL, 645),
    NAME_FUNC_OFFSET(10066, glVertexAttrib3fvARB, glVertexAttrib3fvARB, NULL, 646),
    NAME_FUNC_OFFSET(10087, glVertexAttrib4fARB, glVertexAttrib4fARB, NULL, 647),
    NAME_FUNC_OFFSET(10107, glVertexAttrib4fvARB, glVertexAttrib4fvARB, NULL, 648),
    NAME_FUNC_OFFSET(10128, glAttachObjectARB, glAttachObjectARB, NULL, 649),
    NAME_FUNC_OFFSET(10146, glCreateProgramObjectARB, glCreateProgramObjectARB, NULL, 650),
    NAME_FUNC_OFFSET(10171, glCreateShaderObjectARB, glCreateShaderObjectARB, NULL, 651),
    NAME_FUNC_OFFSET(10195, glDeleteObjectARB, glDeleteObjectARB, NULL, 652),
    NAME_FUNC_OFFSET(10213, glDetachObjectARB, glDetachObjectARB, NULL, 653),
    NAME_FUNC_OFFSET(10231, glGetAttachedObjectsARB, glGetAttachedObjectsARB, NULL, 654),
    NAME_FUNC_OFFSET(10255, glGetHandleARB, glGetHandleARB, NULL, 655),
    NAME_FUNC_OFFSET(10270, glGetInfoLogARB, glGetInfoLogARB, NULL, 656),
    NAME_FUNC_OFFSET(10286, glGetObjectParameterfvARB, glGetObjectParameterfvARB, NULL, 657),
    NAME_FUNC_OFFSET(10312, glGetObjectParameterivARB, glGetObjectParameterivARB, NULL, 658),
    NAME_FUNC_OFFSET(10338, glDrawArraysInstancedARB, glDrawArraysInstancedARB, NULL, 659),
    NAME_FUNC_OFFSET(10363, glDrawElementsInstancedARB, glDrawElementsInstancedARB, NULL, 660),
    NAME_FUNC_OFFSET(10390, glBindFramebuffer, glBindFramebuffer, NULL, 661),
    NAME_FUNC_OFFSET(10408, glBindRenderbuffer, glBindRenderbuffer, NULL, 662),
    NAME_FUNC_OFFSET(10427, glBlitFramebuffer, glBlitFramebuffer, NULL, 663),
    NAME_FUNC_OFFSET(10445, glCheckFramebufferStatus, glCheckFramebufferStatus, NULL, 664),
    NAME_FUNC_OFFSET(10470, glDeleteFramebuffers, glDeleteFramebuffers, NULL, 665),
    NAME_FUNC_OFFSET(10491, glDeleteRenderbuffers, glDeleteRenderbuffers, NULL, 666),
    NAME_FUNC_OFFSET(10513, glFramebufferRenderbuffer, glFramebufferRenderbuffer, NULL, 667),
    NAME_FUNC_OFFSET(10539, glFramebufferTexture1D, glFramebufferTexture1D, NULL, 668),
    NAME_FUNC_OFFSET(10562, glFramebufferTexture2D, glFramebufferTexture2D, NULL, 669),
    NAME_FUNC_OFFSET(10585, glFramebufferTexture3D, glFramebufferTexture3D, NULL, 670),
    NAME_FUNC_OFFSET(10608, glFramebufferTextureLayer, glFramebufferTextureLayer, NULL, 671),
    NAME_FUNC_OFFSET(10634, glGenFramebuffers, glGenFramebuffers, NULL, 672),
    NAME_FUNC_OFFSET(10652, glGenRenderbuffers, glGenRenderbuffers, NULL, 673),
    NAME_FUNC_OFFSET(10671, glGenerateMipmap, glGenerateMipmap, NULL, 674),
    NAME_FUNC_OFFSET(10688, glGetFramebufferAttachmentParameteriv, glGetFramebufferAttachmentParameteriv, NULL, 675),
    NAME_FUNC_OFFSET(10726, glGetRenderbufferParameteriv, glGetRenderbufferParameteriv, NULL, 676),
    NAME_FUNC_OFFSET(10755, glIsFramebuffer, glIsFramebuffer, NULL, 677),
    NAME_FUNC_OFFSET(10771, glIsRenderbuffer, glIsRenderbuffer, NULL, 678),
    NAME_FUNC_OFFSET(10788, glRenderbufferStorage, glRenderbufferStorage, NULL, 679),
    NAME_FUNC_OFFSET(10810, glRenderbufferStorageMultisample, glRenderbufferStorageMultisample, NULL, 680),
    NAME_FUNC_OFFSET(10843, glFlushMappedBufferRange, glFlushMappedBufferRange, NULL, 681),
    NAME_FUNC_OFFSET(10868, glMapBufferRange, glMapBufferRange, NULL, 682),
    NAME_FUNC_OFFSET(10885, glBindVertexArray, glBindVertexArray, NULL, 683),
    NAME_FUNC_OFFSET(10903, glDeleteVertexArrays, glDeleteVertexArrays, NULL, 684),
    NAME_FUNC_OFFSET(10924, glGenVertexArrays, glGenVertexArrays, NULL, 685),
    NAME_FUNC_OFFSET(10942, glIsVertexArray, glIsVertexArray, NULL, 686),
    NAME_FUNC_OFFSET(10958, glGetActiveUniformBlockName, glGetActiveUniformBlockName, NULL, 687),
    NAME_FUNC_OFFSET(10986, glGetActiveUniformBlockiv, glGetActiveUniformBlockiv, NULL, 688),
    NAME_FUNC_OFFSET(11012, glGetActiveUniformName, glGetActiveUniformName, NULL, 689),
    NAME_FUNC_OFFSET(11035, glGetActiveUniformsiv, glGetActiveUniformsiv, NULL, 690),
    NAME_FUNC_OFFSET(11057, glGetUniformBlockIndex, glGetUniformBlockIndex, NULL, 691),
    NAME_FUNC_OFFSET(11080, glGetUniformIndices, glGetUniformIndices, NULL, 692),
    NAME_FUNC_OFFSET(11100, glUniformBlockBinding, glUniformBlockBinding, NULL, 693),
    NAME_FUNC_OFFSET(11122, glCopyBufferSubData, glCopyBufferSubData, NULL, 694),
    NAME_FUNC_OFFSET(11142, glClientWaitSync, glClientWaitSync, NULL, 695),
    NAME_FUNC_OFFSET(11159, glDeleteSync, glDeleteSync, NULL, 696),
    NAME_FUNC_OFFSET(11172, glFenceSync, glFenceSync, NULL, 697),
    NAME_FUNC_OFFSET(11184, glGetInteger64v, glGetInteger64v, NULL, 698),
    NAME_FUNC_OFFSET(11200, glGetSynciv, glGetSynciv, NULL, 699),
    NAME_FUNC_OFFSET(11212, glIsSync, glIsSync, NULL, 700),
    NAME_FUNC_OFFSET(11221, glWaitSync, glWaitSync, NULL, 701),
    NAME_FUNC_OFFSET(11232, glDrawElementsBaseVertex, glDrawElementsBaseVertex, NULL, 702),
    NAME_FUNC_OFFSET(11257, glDrawElementsInstancedBaseVertex, glDrawElementsInstancedBaseVertex, NULL, 703),
    NAME_FUNC_OFFSET(11291, glDrawRangeElementsBaseVertex, glDrawRangeElementsBaseVertex, NULL, 704),
    NAME_FUNC_OFFSET(11321, glMultiDrawElementsBaseVertex, glMultiDrawElementsBaseVertex, NULL, 705),
    NAME_FUNC_OFFSET(11351, glProvokingVertex, glProvokingVertex, NULL, 706),
    NAME_FUNC_OFFSET(11369, glGetMultisamplefv, glGetMultisamplefv, NULL, 707),
    NAME_FUNC_OFFSET(11388, glSampleMaski, glSampleMaski, NULL, 708),
    NAME_FUNC_OFFSET(11402, glTexImage2DMultisample, glTexImage2DMultisample, NULL, 709),
    NAME_FUNC_OFFSET(11426, glTexImage3DMultisample, glTexImage3DMultisample, NULL, 710),
    NAME_FUNC_OFFSET(11450, glBlendEquationSeparateiARB, glBlendEquationSeparateiARB, NULL, 711),
    NAME_FUNC_OFFSET(11478, glBlendEquationiARB, glBlendEquationiARB, NULL, 712),
    NAME_FUNC_OFFSET(11498, glBlendFuncSeparateiARB, glBlendFuncSeparateiARB, NULL, 713),
    NAME_FUNC_OFFSET(11522, glBlendFunciARB, glBlendFunciARB, NULL, 714),
    NAME_FUNC_OFFSET(11538, glBindFragDataLocationIndexed, glBindFragDataLocationIndexed, NULL, 715),
    NAME_FUNC_OFFSET(11568, glGetFragDataIndex, glGetFragDataIndex, NULL, 716),
    NAME_FUNC_OFFSET(11587, glBindSampler, glBindSampler, NULL, 717),
    NAME_FUNC_OFFSET(11601, glDeleteSamplers, glDeleteSamplers, NULL, 718),
    NAME_FUNC_OFFSET(11618, glGenSamplers, glGenSamplers, NULL, 719),
    NAME_FUNC_OFFSET(11632, glGetSamplerParameterIiv, glGetSamplerParameterIiv, NULL, 720),
    NAME_FUNC_OFFSET(11657, glGetSamplerParameterIuiv, glGetSamplerParameterIuiv, NULL, 721),
    NAME_FUNC_OFFSET(11683, glGetSamplerParameterfv, glGetSamplerParameterfv, NULL, 722),
    NAME_FUNC_OFFSET(11707, glGetSamplerParameteriv, glGetSamplerParameteriv, NULL, 723),
    NAME_FUNC_OFFSET(11731, glIsSampler, glIsSampler, NULL, 724),
    NAME_FUNC_OFFSET(11743, glSamplerParameterIiv, glSamplerParameterIiv, NULL, 725),
    NAME_FUNC_OFFSET(11765, glSamplerParameterIuiv, glSamplerParameterIuiv, NULL, 726),
    NAME_FUNC_OFFSET(11788, glSamplerParameterf, glSamplerParameterf, NULL, 727),
    NAME_FUNC_OFFSET(11808, glSamplerParameterfv, glSamplerParameterfv, NULL, 728),
    NAME_FUNC_OFFSET(11829, glSamplerParameteri, glSamplerParameteri, NULL, 729),
    NAME_FUNC_OFFSET(11849, glSamplerParameteriv, glSamplerParameteriv, NULL, 730),
    NAME_FUNC_OFFSET(11870, gl_dispatch_stub_731, gl_dispatch_stub_731, NULL, 731),
    NAME_FUNC_OFFSET(11891, gl_dispatch_stub_732, gl_dispatch_stub_732, NULL, 732),
    NAME_FUNC_OFFSET(11913, gl_dispatch_stub_733, gl_dispatch_stub_733, NULL, 733),
    NAME_FUNC_OFFSET(11928, glColorP3ui, glColorP3ui, NULL, 734),
    NAME_FUNC_OFFSET(11940, glColorP3uiv, glColorP3uiv, NULL, 735),
    NAME_FUNC_OFFSET(11953, glColorP4ui, glColorP4ui, NULL, 736),
    NAME_FUNC_OFFSET(11965, glColorP4uiv, glColorP4uiv, NULL, 737),
    NAME_FUNC_OFFSET(11978, glMultiTexCoordP1ui, glMultiTexCoordP1ui, NULL, 738),
    NAME_FUNC_OFFSET(11998, glMultiTexCoordP1uiv, glMultiTexCoordP1uiv, NULL, 739),
    NAME_FUNC_OFFSET(12019, glMultiTexCoordP2ui, glMultiTexCoordP2ui, NULL, 740),
    NAME_FUNC_OFFSET(12039, glMultiTexCoordP2uiv, glMultiTexCoordP2uiv, NULL, 741),
    NAME_FUNC_OFFSET(12060, glMultiTexCoordP3ui, glMultiTexCoordP3ui, NULL, 742),
    NAME_FUNC_OFFSET(12080, glMultiTexCoordP3uiv, glMultiTexCoordP3uiv, NULL, 743),
    NAME_FUNC_OFFSET(12101, glMultiTexCoordP4ui, glMultiTexCoordP4ui, NULL, 744),
    NAME_FUNC_OFFSET(12121, glMultiTexCoordP4uiv, glMultiTexCoordP4uiv, NULL, 745),
    NAME_FUNC_OFFSET(12142, glNormalP3ui, glNormalP3ui, NULL, 746),
    NAME_FUNC_OFFSET(12155, glNormalP3uiv, glNormalP3uiv, NULL, 747),
    NAME_FUNC_OFFSET(12169, glSecondaryColorP3ui, glSecondaryColorP3ui, NULL, 748),
    NAME_FUNC_OFFSET(12190, glSecondaryColorP3uiv, glSecondaryColorP3uiv, NULL, 749),
    NAME_FUNC_OFFSET(12212, glTexCoordP1ui, glTexCoordP1ui, NULL, 750),
    NAME_FUNC_OFFSET(12227, glTexCoordP1uiv, glTexCoordP1uiv, NULL, 751),
    NAME_FUNC_OFFSET(12243, glTexCoordP2ui, glTexCoordP2ui, NULL, 752),
    NAME_FUNC_OFFSET(12258, glTexCoordP2uiv, glTexCoordP2uiv, NULL, 753),
    NAME_FUNC_OFFSET(12274, glTexCoordP3ui, glTexCoordP3ui, NULL, 754),
    NAME_FUNC_OFFSET(12289, glTexCoordP3uiv, glTexCoordP3uiv, NULL, 755),
    NAME_FUNC_OFFSET(12305, glTexCoordP4ui, glTexCoordP4ui, NULL, 756),
    NAME_FUNC_OFFSET(12320, glTexCoordP4uiv, glTexCoordP4uiv, NULL, 757),
    NAME_FUNC_OFFSET(12336, glVertexAttribP1ui, glVertexAttribP1ui, NULL, 758),
    NAME_FUNC_OFFSET(12355, glVertexAttribP1uiv, glVertexAttribP1uiv, NULL, 759),
    NAME_FUNC_OFFSET(12375, glVertexAttribP2ui, glVertexAttribP2ui, NULL, 760),
    NAME_FUNC_OFFSET(12394, glVertexAttribP2uiv, glVertexAttribP2uiv, NULL, 761),
    NAME_FUNC_OFFSET(12414, glVertexAttribP3ui, glVertexAttribP3ui, NULL, 762),
    NAME_FUNC_OFFSET(12433, glVertexAttribP3uiv, glVertexAttribP3uiv, NULL, 763),
    NAME_FUNC_OFFSET(12453, glVertexAttribP4ui, glVertexAttribP4ui, NULL, 764),
    NAME_FUNC_OFFSET(12472, glVertexAttribP4uiv, glVertexAttribP4uiv, NULL, 765),
    NAME_FUNC_OFFSET(12492, glVertexP2ui, glVertexP2ui, NULL, 766),
    NAME_FUNC_OFFSET(12505, glVertexP2uiv, glVertexP2uiv, NULL, 767),
    NAME_FUNC_OFFSET(12519, glVertexP3ui, glVertexP3ui, NULL, 768),
    NAME_FUNC_OFFSET(12532, glVertexP3uiv, glVertexP3uiv, NULL, 769),
    NAME_FUNC_OFFSET(12546, glVertexP4ui, glVertexP4ui, NULL, 770),
    NAME_FUNC_OFFSET(12559, glVertexP4uiv, glVertexP4uiv, NULL, 771),
    NAME_FUNC_OFFSET(12573, glDrawArraysIndirect, glDrawArraysIndirect, NULL, 772),
    NAME_FUNC_OFFSET(12594, glDrawElementsIndirect, glDrawElementsIndirect, NULL, 773),
    NAME_FUNC_OFFSET(12617, gl_dispatch_stub_774, gl_dispatch_stub_774, NULL, 774),
    NAME_FUNC_OFFSET(12632, gl_dispatch_stub_775, gl_dispatch_stub_775, NULL, 775),
    NAME_FUNC_OFFSET(12644, gl_dispatch_stub_776, gl_dispatch_stub_776, NULL, 776),
    NAME_FUNC_OFFSET(12657, gl_dispatch_stub_777, gl_dispatch_stub_777, NULL, 777),
    NAME_FUNC_OFFSET(12669, gl_dispatch_stub_778, gl_dispatch_stub_778, NULL, 778),
    NAME_FUNC_OFFSET(12682, gl_dispatch_stub_779, gl_dispatch_stub_779, NULL, 779),
    NAME_FUNC_OFFSET(12694, gl_dispatch_stub_780, gl_dispatch_stub_780, NULL, 780),
    NAME_FUNC_OFFSET(12707, gl_dispatch_stub_781, gl_dispatch_stub_781, NULL, 781),
    NAME_FUNC_OFFSET(12719, gl_dispatch_stub_782, gl_dispatch_stub_782, NULL, 782),
    NAME_FUNC_OFFSET(12732, gl_dispatch_stub_783, gl_dispatch_stub_783, NULL, 783),
    NAME_FUNC_OFFSET(12751, gl_dispatch_stub_784, gl_dispatch_stub_784, NULL, 784),
    NAME_FUNC_OFFSET(12772, gl_dispatch_stub_785, gl_dispatch_stub_785, NULL, 785),
    NAME_FUNC_OFFSET(12793, gl_dispatch_stub_786, gl_dispatch_stub_786, NULL, 786),
    NAME_FUNC_OFFSET(12812, gl_dispatch_stub_787, gl_dispatch_stub_787, NULL, 787),
    NAME_FUNC_OFFSET(12833, gl_dispatch_stub_788, gl_dispatch_stub_788, NULL, 788),
    NAME_FUNC_OFFSET(12854, gl_dispatch_stub_789, gl_dispatch_stub_789, NULL, 789),
    NAME_FUNC_OFFSET(12873, gl_dispatch_stub_790, gl_dispatch_stub_790, NULL, 790),
    NAME_FUNC_OFFSET(12894, gl_dispatch_stub_791, gl_dispatch_stub_791, NULL, 791),
    NAME_FUNC_OFFSET(12915, gl_dispatch_stub_792, gl_dispatch_stub_792, NULL, 792),
    NAME_FUNC_OFFSET(12941, gl_dispatch_stub_793, gl_dispatch_stub_793, NULL, 793),
    NAME_FUNC_OFFSET(12974, gl_dispatch_stub_794, gl_dispatch_stub_794, NULL, 794),
    NAME_FUNC_OFFSET(13005, gl_dispatch_stub_795, gl_dispatch_stub_795, NULL, 795),
    NAME_FUNC_OFFSET(13025, gl_dispatch_stub_796, gl_dispatch_stub_796, NULL, 796),
    NAME_FUNC_OFFSET(13046, gl_dispatch_stub_797, gl_dispatch_stub_797, NULL, 797),
    NAME_FUNC_OFFSET(13077, gl_dispatch_stub_798, gl_dispatch_stub_798, NULL, 798),
    NAME_FUNC_OFFSET(13103, gl_dispatch_stub_799, gl_dispatch_stub_799, NULL, 799),
    NAME_FUNC_OFFSET(13127, gl_dispatch_stub_800, gl_dispatch_stub_800, NULL, 800),
    NAME_FUNC_OFFSET(13146, gl_dispatch_stub_801, gl_dispatch_stub_801, NULL, 801),
    NAME_FUNC_OFFSET(13164, glBindTransformFeedback, glBindTransformFeedback, NULL, 802),
    NAME_FUNC_OFFSET(13188, glDeleteTransformFeedbacks, glDeleteTransformFeedbacks, NULL, 803),
    NAME_FUNC_OFFSET(13215, glDrawTransformFeedback, glDrawTransformFeedback, NULL, 804),
    NAME_FUNC_OFFSET(13239, glGenTransformFeedbacks, glGenTransformFeedbacks, NULL, 805),
    NAME_FUNC_OFFSET(13263, glIsTransformFeedback, glIsTransformFeedback, NULL, 806),
    NAME_FUNC_OFFSET(13285, glPauseTransformFeedback, glPauseTransformFeedback, NULL, 807),
    NAME_FUNC_OFFSET(13310, glResumeTransformFeedback, glResumeTransformFeedback, NULL, 808),
    NAME_FUNC_OFFSET(13336, glBeginQueryIndexed, glBeginQueryIndexed, NULL, 809),
    NAME_FUNC_OFFSET(13356, glDrawTransformFeedbackStream, glDrawTransformFeedbackStream, NULL, 810),
    NAME_FUNC_OFFSET(13386, glEndQueryIndexed, glEndQueryIndexed, NULL, 811),
    NAME_FUNC_OFFSET(13404, glGetQueryIndexediv, glGetQueryIndexediv, NULL, 812),
    NAME_FUNC_OFFSET(13424, glClearDepthf, glClearDepthf, NULL, 813),
    NAME_FUNC_OFFSET(13438, glDepthRangef, glDepthRangef, NULL, 814),
    NAME_FUNC_OFFSET(13452, glGetShaderPrecisionFormat, glGetShaderPrecisionFormat, NULL, 815),
    NAME_FUNC_OFFSET(13479, glReleaseShaderCompiler, glReleaseShaderCompiler, NULL, 816),
    NAME_FUNC_OFFSET(13503, glShaderBinary, glShaderBinary, NULL, 817),
    NAME_FUNC_OFFSET(13518, glGetProgramBinary, glGetProgramBinary, NULL, 818),
    NAME_FUNC_OFFSET(13537, glProgramBinary, glProgramBinary, NULL, 819),
    NAME_FUNC_OFFSET(13553, glProgramParameteri, glProgramParameteri, NULL, 820),
    NAME_FUNC_OFFSET(13573, gl_dispatch_stub_821, gl_dispatch_stub_821, NULL, 821),
    NAME_FUNC_OFFSET(13594, gl_dispatch_stub_822, gl_dispatch_stub_822, NULL, 822),
    NAME_FUNC_OFFSET(13612, gl_dispatch_stub_823, gl_dispatch_stub_823, NULL, 823),
    NAME_FUNC_OFFSET(13631, gl_dispatch_stub_824, gl_dispatch_stub_824, NULL, 824),
    NAME_FUNC_OFFSET(13649, gl_dispatch_stub_825, gl_dispatch_stub_825, NULL, 825),
    NAME_FUNC_OFFSET(13668, gl_dispatch_stub_826, gl_dispatch_stub_826, NULL, 826),
    NAME_FUNC_OFFSET(13686, gl_dispatch_stub_827, gl_dispatch_stub_827, NULL, 827),
    NAME_FUNC_OFFSET(13705, gl_dispatch_stub_828, gl_dispatch_stub_828, NULL, 828),
    NAME_FUNC_OFFSET(13723, gl_dispatch_stub_829, gl_dispatch_stub_829, NULL, 829),
    NAME_FUNC_OFFSET(13742, gl_dispatch_stub_830, gl_dispatch_stub_830, NULL, 830),
    NAME_FUNC_OFFSET(13765, glDepthRangeArrayv, glDepthRangeArrayv, NULL, 831),
    NAME_FUNC_OFFSET(13784, glDepthRangeIndexed, glDepthRangeIndexed, NULL, 832),
    NAME_FUNC_OFFSET(13804, glGetDoublei_v, glGetDoublei_v, NULL, 833),
    NAME_FUNC_OFFSET(13819, glGetFloati_v, glGetFloati_v, NULL, 834),
    NAME_FUNC_OFFSET(13833, glScissorArrayv, glScissorArrayv, NULL, 835),
    NAME_FUNC_OFFSET(13849, glScissorIndexed, glScissorIndexed, NULL, 836),
    NAME_FUNC_OFFSET(13866, glScissorIndexedv, glScissorIndexedv, NULL, 837),
    NAME_FUNC_OFFSET(13884, glViewportArrayv, glViewportArrayv, NULL, 838),
    NAME_FUNC_OFFSET(13901, glViewportIndexedf, glViewportIndexedf, NULL, 839),
    NAME_FUNC_OFFSET(13920, glViewportIndexedfv, glViewportIndexedfv, NULL, 840),
    NAME_FUNC_OFFSET(13940, glGetGraphicsResetStatusARB, glGetGraphicsResetStatusARB, NULL, 841),
    NAME_FUNC_OFFSET(13968, glGetnColorTableARB, glGetnColorTableARB, NULL, 842),
    NAME_FUNC_OFFSET(13988, glGetnCompressedTexImageARB, glGetnCompressedTexImageARB, NULL, 843),
    NAME_FUNC_OFFSET(14016, glGetnConvolutionFilterARB, glGetnConvolutionFilterARB, NULL, 844),
    NAME_FUNC_OFFSET(14043, glGetnHistogramARB, glGetnHistogramARB, NULL, 845),
    NAME_FUNC_OFFSET(14062, glGetnMapdvARB, glGetnMapdvARB, NULL, 846),
    NAME_FUNC_OFFSET(14077, glGetnMapfvARB, glGetnMapfvARB, NULL, 847),
    NAME_FUNC_OFFSET(14092, glGetnMapivARB, glGetnMapivARB, NULL, 848),
    NAME_FUNC_OFFSET(14107, glGetnMinmaxARB, glGetnMinmaxARB, NULL, 849),
    NAME_FUNC_OFFSET(14123, glGetnPixelMapfvARB, glGetnPixelMapfvARB, NULL, 850),
    NAME_FUNC_OFFSET(14143, glGetnPixelMapuivARB, glGetnPixelMapuivARB, NULL, 851),
    NAME_FUNC_OFFSET(14164, glGetnPixelMapusvARB, glGetnPixelMapusvARB, NULL, 852),
    NAME_FUNC_OFFSET(14185, glGetnPolygonStippleARB, glGetnPolygonStippleARB, NULL, 853),
    NAME_FUNC_OFFSET(14209, glGetnSeparableFilterARB, glGetnSeparableFilterARB, NULL, 854),
    NAME_FUNC_OFFSET(14234, glGetnTexImageARB, glGetnTexImageARB, NULL, 855),
    NAME_FUNC_OFFSET(14252, glGetnUniformdvARB, glGetnUniformdvARB, NULL, 856),
    NAME_FUNC_OFFSET(14271, glGetnUniformfvARB, glGetnUniformfvARB, NULL, 857),
    NAME_FUNC_OFFSET(14290, glGetnUniformivARB, glGetnUniformivARB, NULL, 858),
    NAME_FUNC_OFFSET(14309, glGetnUniformuivARB, glGetnUniformuivARB, NULL, 859),
    NAME_FUNC_OFFSET(14329, glReadnPixelsARB, glReadnPixelsARB, NULL, 860),
    NAME_FUNC_OFFSET(14346, glDrawArraysInstancedBaseInstance, glDrawArraysInstancedBaseInstance, NULL, 861),
    NAME_FUNC_OFFSET(14380, glDrawElementsInstancedBaseInstance, glDrawElementsInstancedBaseInstance, NULL, 862),
    NAME_FUNC_OFFSET(14416, glDrawElementsInstancedBaseVertexBaseInstance, glDrawElementsInstancedBaseVertexBaseInstance, NULL, 863),
    NAME_FUNC_OFFSET(14462, glDrawTransformFeedbackInstanced, glDrawTransformFeedbackInstanced, NULL, 864),
    NAME_FUNC_OFFSET(14495, glDrawTransformFeedbackStreamInstanced, glDrawTransformFeedbackStreamInstanced, NULL, 865),
    NAME_FUNC_OFFSET(14534, gl_dispatch_stub_866, gl_dispatch_stub_866, NULL, 866),
    NAME_FUNC_OFFSET(14556, glGetActiveAtomicCounterBufferiv, glGetActiveAtomicCounterBufferiv, NULL, 867),
    NAME_FUNC_OFFSET(14589, glBindImageTexture, glBindImageTexture, NULL, 868),
    NAME_FUNC_OFFSET(14608, glMemoryBarrier, glMemoryBarrier, NULL, 869),
    NAME_FUNC_OFFSET(14624, glTexStorage1D, glTexStorage1D, NULL, 870),
    NAME_FUNC_OFFSET(14639, glTexStorage2D, glTexStorage2D, NULL, 871),
    NAME_FUNC_OFFSET(14654, glTexStorage3D, glTexStorage3D, NULL, 872),
    NAME_FUNC_OFFSET(14669, glTextureStorage1DEXT, glTextureStorage1DEXT, NULL, 873),
    NAME_FUNC_OFFSET(14691, glTextureStorage2DEXT, glTextureStorage2DEXT, NULL, 874),
    NAME_FUNC_OFFSET(14713, glTextureStorage3DEXT, glTextureStorage3DEXT, NULL, 875),
    NAME_FUNC_OFFSET(14735, glClearBufferData, glClearBufferData, NULL, 876),
    NAME_FUNC_OFFSET(14753, glClearBufferSubData, glClearBufferSubData, NULL, 877),
    NAME_FUNC_OFFSET(14774, glDispatchCompute, glDispatchCompute, NULL, 878),
    NAME_FUNC_OFFSET(14792, glDispatchComputeIndirect, glDispatchComputeIndirect, NULL, 879),
    NAME_FUNC_OFFSET(14818, glCopyImageSubData, glCopyImageSubData, NULL, 880),
    NAME_FUNC_OFFSET(14837, glTextureView, glTextureView, NULL, 881),
    NAME_FUNC_OFFSET(14851, glBindVertexBuffer, glBindVertexBuffer, NULL, 882),
    NAME_FUNC_OFFSET(14870, glVertexAttribBinding, glVertexAttribBinding, NULL, 883),
    NAME_FUNC_OFFSET(14892, glVertexAttribFormat, glVertexAttribFormat, NULL, 884),
    NAME_FUNC_OFFSET(14913, glVertexAttribIFormat, glVertexAttribIFormat, NULL, 885),
    NAME_FUNC_OFFSET(14935, glVertexAttribLFormat, glVertexAttribLFormat, NULL, 886),
    NAME_FUNC_OFFSET(14957, glVertexBindingDivisor, glVertexBindingDivisor, NULL, 887),
    NAME_FUNC_OFFSET(14980, gl_dispatch_stub_888, gl_dispatch_stub_888, NULL, 888),
    NAME_FUNC_OFFSET(15004, gl_dispatch_stub_889, gl_dispatch_stub_889, NULL, 889),
    NAME_FUNC_OFFSET(15032, glMultiDrawArraysIndirect, glMultiDrawArraysIndirect, NULL, 890),
    NAME_FUNC_OFFSET(15058, glMultiDrawElementsIndirect, glMultiDrawElementsIndirect, NULL, 891),
    NAME_FUNC_OFFSET(15086, gl_dispatch_stub_892, gl_dispatch_stub_892, NULL, 892),
    NAME_FUNC_OFFSET(15110, gl_dispatch_stub_893, gl_dispatch_stub_893, NULL, 893),
    NAME_FUNC_OFFSET(15136, gl_dispatch_stub_894, gl_dispatch_stub_894, NULL, 894),
    NAME_FUNC_OFFSET(15165, gl_dispatch_stub_895, gl_dispatch_stub_895, NULL, 895),
    NAME_FUNC_OFFSET(15199, gl_dispatch_stub_896, gl_dispatch_stub_896, NULL, 896),
    NAME_FUNC_OFFSET(15224, gl_dispatch_stub_897, gl_dispatch_stub_897, NULL, 897),
    NAME_FUNC_OFFSET(15247, glTexBufferRange, glTexBufferRange, NULL, 898),
    NAME_FUNC_OFFSET(15264, glTexStorage2DMultisample, glTexStorage2DMultisample, NULL, 899),
    NAME_FUNC_OFFSET(15290, glTexStorage3DMultisample, glTexStorage3DMultisample, NULL, 900),
    NAME_FUNC_OFFSET(15316, glBufferStorage, glBufferStorage, NULL, 901),
    NAME_FUNC_OFFSET(15332, glClearTexImage, glClearTexImage, NULL, 902),
    NAME_FUNC_OFFSET(15348, glClearTexSubImage, glClearTexSubImage, NULL, 903),
    NAME_FUNC_OFFSET(15367, glBindBuffersBase, glBindBuffersBase, NULL, 904),
    NAME_FUNC_OFFSET(15385, glBindBuffersRange, glBindBuffersRange, NULL, 905),
    NAME_FUNC_OFFSET(15404, glBindImageTextures, glBindImageTextures, NULL, 906),
    NAME_FUNC_OFFSET(15424, glBindSamplers, glBindSamplers, NULL, 907),
    NAME_FUNC_OFFSET(15439, glBindTextures, glBindTextures, NULL, 908),
    NAME_FUNC_OFFSET(15454, glBindVertexBuffers, glBindVertexBuffers, NULL, 909),
    NAME_FUNC_OFFSET(15474, gl_dispatch_stub_910, gl_dispatch_stub_910, NULL, 910),
    NAME_FUNC_OFFSET(15488, gl_dispatch_stub_911, gl_dispatch_stub_911, NULL, 911),
    NAME_FUNC_OFFSET(15506, gl_dispatch_stub_912, gl_dispatch_stub_912, NULL, 912),
    NAME_FUNC_OFFSET(15529, gl_dispatch_stub_913, gl_dispatch_stub_913, NULL, 913),
    NAME_FUNC_OFFSET(15559, gl_dispatch_stub_914, gl_dispatch_stub_914, NULL, 914),
    NAME_FUNC_OFFSET(15582, gl_dispatch_stub_915, gl_dispatch_stub_915, NULL, 915),
    NAME_FUNC_OFFSET(15608, gl_dispatch_stub_916, gl_dispatch_stub_916, NULL, 916),
    NAME_FUNC_OFFSET(15634, gl_dispatch_stub_917, gl_dispatch_stub_917, NULL, 917),
    NAME_FUNC_OFFSET(15660, gl_dispatch_stub_918, gl_dispatch_stub_918, NULL, 918),
    NAME_FUNC_OFFSET(15686, gl_dispatch_stub_919, gl_dispatch_stub_919, NULL, 919),
    NAME_FUNC_OFFSET(15713, gl_dispatch_stub_920, gl_dispatch_stub_920, NULL, 920),
    NAME_FUNC_OFFSET(15743, gl_dispatch_stub_921, gl_dispatch_stub_921, NULL, 921),
    NAME_FUNC_OFFSET(15773, gl_dispatch_stub_922, gl_dispatch_stub_922, NULL, 922),
    NAME_FUNC_OFFSET(15803, gl_dispatch_stub_923, gl_dispatch_stub_923, NULL, 923),
    NAME_FUNC_OFFSET(15828, gl_dispatch_stub_924, gl_dispatch_stub_924, NULL, 924),
    NAME_FUNC_OFFSET(15852, gl_dispatch_stub_925, gl_dispatch_stub_925, NULL, 925),
    NAME_FUNC_OFFSET(15876, gl_dispatch_stub_926, gl_dispatch_stub_926, NULL, 926),
    NAME_FUNC_OFFSET(15900, gl_dispatch_stub_927, gl_dispatch_stub_927, NULL, 927),
    NAME_FUNC_OFFSET(15916, gl_dispatch_stub_928, gl_dispatch_stub_928, NULL, 928),
    NAME_FUNC_OFFSET(15937, gl_dispatch_stub_929, gl_dispatch_stub_929, NULL, 929),
    NAME_FUNC_OFFSET(15962, gl_dispatch_stub_930, gl_dispatch_stub_930, NULL, 930),
    NAME_FUNC_OFFSET(15978, gl_dispatch_stub_931, gl_dispatch_stub_931, NULL, 931),
    NAME_FUNC_OFFSET(16000, gl_dispatch_stub_932, gl_dispatch_stub_932, NULL, 932),
    NAME_FUNC_OFFSET(16017, gl_dispatch_stub_933, gl_dispatch_stub_933, NULL, 933),
    NAME_FUNC_OFFSET(16034, gl_dispatch_stub_934, gl_dispatch_stub_934, NULL, 934),
    NAME_FUNC_OFFSET(16061, gl_dispatch_stub_935, gl_dispatch_stub_935, NULL, 935),
    NAME_FUNC_OFFSET(16082, gl_dispatch_stub_936, gl_dispatch_stub_936, NULL, 936),
    NAME_FUNC_OFFSET(16109, gl_dispatch_stub_937, gl_dispatch_stub_937, NULL, 937),
    NAME_FUNC_OFFSET(16135, gl_dispatch_stub_938, gl_dispatch_stub_938, NULL, 938),
    NAME_FUNC_OFFSET(16165, gl_dispatch_stub_939, gl_dispatch_stub_939, NULL, 939),
    NAME_FUNC_OFFSET(16189, gl_dispatch_stub_940, gl_dispatch_stub_940, NULL, 940),
    NAME_FUNC_OFFSET(16217, gl_dispatch_stub_941, gl_dispatch_stub_941, NULL, 941),
    NAME_FUNC_OFFSET(16247, gl_dispatch_stub_942, gl_dispatch_stub_942, NULL, 942),
    NAME_FUNC_OFFSET(16275, gl_dispatch_stub_943, gl_dispatch_stub_943, NULL, 943),
    NAME_FUNC_OFFSET(16300, gl_dispatch_stub_944, gl_dispatch_stub_944, NULL, 944),
    NAME_FUNC_OFFSET(16324, gl_dispatch_stub_945, gl_dispatch_stub_945, NULL, 945),
    NAME_FUNC_OFFSET(16367, gl_dispatch_stub_946, gl_dispatch_stub_946, NULL, 946),
    NAME_FUNC_OFFSET(16400, gl_dispatch_stub_947, gl_dispatch_stub_947, NULL, 947),
    NAME_FUNC_OFFSET(16434, gl_dispatch_stub_948, gl_dispatch_stub_948, NULL, 948),
    NAME_FUNC_OFFSET(16461, gl_dispatch_stub_949, gl_dispatch_stub_949, NULL, 949),
    NAME_FUNC_OFFSET(16486, gl_dispatch_stub_950, gl_dispatch_stub_950, NULL, 950),
    NAME_FUNC_OFFSET(16514, gl_dispatch_stub_951, gl_dispatch_stub_951, NULL, 951),
    NAME_FUNC_OFFSET(16540, gl_dispatch_stub_952, gl_dispatch_stub_952, NULL, 952),
    NAME_FUNC_OFFSET(16558, gl_dispatch_stub_953, gl_dispatch_stub_953, NULL, 953),
    NAME_FUNC_OFFSET(16587, gl_dispatch_stub_954, gl_dispatch_stub_954, NULL, 954),
    NAME_FUNC_OFFSET(16616, gl_dispatch_stub_955, gl_dispatch_stub_955, NULL, 955),
    NAME_FUNC_OFFSET(16641, gl_dispatch_stub_956, gl_dispatch_stub_956, NULL, 956),
    NAME_FUNC_OFFSET(16667, gl_dispatch_stub_957, gl_dispatch_stub_957, NULL, 957),
    NAME_FUNC_OFFSET(16691, gl_dispatch_stub_958, gl_dispatch_stub_958, NULL, 958),
    NAME_FUNC_OFFSET(16715, gl_dispatch_stub_959, gl_dispatch_stub_959, NULL, 959),
    NAME_FUNC_OFFSET(16743, gl_dispatch_stub_960, gl_dispatch_stub_960, NULL, 960),
    NAME_FUNC_OFFSET(16769, gl_dispatch_stub_961, gl_dispatch_stub_961, NULL, 961),
    NAME_FUNC_OFFSET(16794, gl_dispatch_stub_962, gl_dispatch_stub_962, NULL, 962),
    NAME_FUNC_OFFSET(16822, gl_dispatch_stub_963, gl_dispatch_stub_963, NULL, 963),
    NAME_FUNC_OFFSET(16848, gl_dispatch_stub_964, gl_dispatch_stub_964, NULL, 964),
    NAME_FUNC_OFFSET(16867, gl_dispatch_stub_965, gl_dispatch_stub_965, NULL, 965),
    NAME_FUNC_OFFSET(16900, gl_dispatch_stub_966, gl_dispatch_stub_966, NULL, 966),
    NAME_FUNC_OFFSET(16936, gl_dispatch_stub_967, gl_dispatch_stub_967, NULL, 967),
    NAME_FUNC_OFFSET(16953, gl_dispatch_stub_968, gl_dispatch_stub_968, NULL, 968),
    NAME_FUNC_OFFSET(16975, gl_dispatch_stub_969, gl_dispatch_stub_969, NULL, 969),
    NAME_FUNC_OFFSET(16993, gl_dispatch_stub_970, gl_dispatch_stub_970, NULL, 970),
    NAME_FUNC_OFFSET(17014, gl_dispatch_stub_971, gl_dispatch_stub_971, NULL, 971),
    NAME_FUNC_OFFSET(17035, gl_dispatch_stub_972, gl_dispatch_stub_972, NULL, 972),
    NAME_FUNC_OFFSET(17064, gl_dispatch_stub_973, gl_dispatch_stub_973, NULL, 973),
    NAME_FUNC_OFFSET(17094, gl_dispatch_stub_974, gl_dispatch_stub_974, NULL, 974),
    NAME_FUNC_OFFSET(17123, gl_dispatch_stub_975, gl_dispatch_stub_975, NULL, 975),
    NAME_FUNC_OFFSET(17152, gl_dispatch_stub_976, gl_dispatch_stub_976, NULL, 976),
    NAME_FUNC_OFFSET(17183, gl_dispatch_stub_977, gl_dispatch_stub_977, NULL, 977),
    NAME_FUNC_OFFSET(17209, gl_dispatch_stub_978, gl_dispatch_stub_978, NULL, 978),
    NAME_FUNC_OFFSET(17240, gl_dispatch_stub_979, gl_dispatch_stub_979, NULL, 979),
    NAME_FUNC_OFFSET(17267, gl_dispatch_stub_980, gl_dispatch_stub_980, NULL, 980),
    NAME_FUNC_OFFSET(17305, gl_dispatch_stub_981, gl_dispatch_stub_981, NULL, 981),
    NAME_FUNC_OFFSET(17321, gl_dispatch_stub_982, gl_dispatch_stub_982, NULL, 982),
    NAME_FUNC_OFFSET(17342, gl_dispatch_stub_983, gl_dispatch_stub_983, NULL, 983),
    NAME_FUNC_OFFSET(17364, gl_dispatch_stub_984, gl_dispatch_stub_984, NULL, 984),
    NAME_FUNC_OFFSET(17387, gl_dispatch_stub_985, gl_dispatch_stub_985, NULL, 985),
    NAME_FUNC_OFFSET(17407, gl_dispatch_stub_986, gl_dispatch_stub_986, NULL, 986),
    NAME_FUNC_OFFSET(17428, gl_dispatch_stub_987, gl_dispatch_stub_987, NULL, 987),
    NAME_FUNC_OFFSET(17448, gl_dispatch_stub_988, gl_dispatch_stub_988, NULL, 988),
    NAME_FUNC_OFFSET(17469, gl_dispatch_stub_989, gl_dispatch_stub_989, NULL, 989),
    NAME_FUNC_OFFSET(17488, gl_dispatch_stub_990, gl_dispatch_stub_990, NULL, 990),
    NAME_FUNC_OFFSET(17507, gl_dispatch_stub_991, gl_dispatch_stub_991, NULL, 991),
    NAME_FUNC_OFFSET(17537, gl_dispatch_stub_992, gl_dispatch_stub_992, NULL, 992),
    NAME_FUNC_OFFSET(17556, gl_dispatch_stub_993, gl_dispatch_stub_993, NULL, 993),
    NAME_FUNC_OFFSET(17586, gl_dispatch_stub_994, gl_dispatch_stub_994, NULL, 994),
    NAME_FUNC_OFFSET(17606, gl_dispatch_stub_995, gl_dispatch_stub_995, NULL, 995),
    NAME_FUNC_OFFSET(17626, gl_dispatch_stub_996, gl_dispatch_stub_996, NULL, 996),
    NAME_FUNC_OFFSET(17646, gl_dispatch_stub_997, gl_dispatch_stub_997, NULL, 997),
    NAME_FUNC_OFFSET(17676, gl_dispatch_stub_998, gl_dispatch_stub_998, NULL, 998),
    NAME_FUNC_OFFSET(17707, gl_dispatch_stub_999, gl_dispatch_stub_999, NULL, 999),
    NAME_FUNC_OFFSET(17726, gl_dispatch_stub_1000, gl_dispatch_stub_1000, NULL, 1000),
    NAME_FUNC_OFFSET(17753, gl_dispatch_stub_1001, gl_dispatch_stub_1001, NULL, 1001),
    NAME_FUNC_OFFSET(17779, gl_dispatch_stub_1002, gl_dispatch_stub_1002, NULL, 1002),
    NAME_FUNC_OFFSET(17806, gl_dispatch_stub_1003, gl_dispatch_stub_1003, NULL, 1003),
    NAME_FUNC_OFFSET(17833, gl_dispatch_stub_1004, gl_dispatch_stub_1004, NULL, 1004),
    NAME_FUNC_OFFSET(17861, gl_dispatch_stub_1005, gl_dispatch_stub_1005, NULL, 1005),
    NAME_FUNC_OFFSET(17888, gl_dispatch_stub_1006, gl_dispatch_stub_1006, NULL, 1006),
    NAME_FUNC_OFFSET(17914, gl_dispatch_stub_1007, gl_dispatch_stub_1007, NULL, 1007),
    NAME_FUNC_OFFSET(17941, gl_dispatch_stub_1008, gl_dispatch_stub_1008, NULL, 1008),
    NAME_FUNC_OFFSET(17972, gl_dispatch_stub_1009, gl_dispatch_stub_1009, NULL, 1009),
    NAME_FUNC_OFFSET(17993, glInvalidateBufferData, glInvalidateBufferData, NULL, 1010),
    NAME_FUNC_OFFSET(18016, glInvalidateBufferSubData, glInvalidateBufferSubData, NULL, 1011),
    NAME_FUNC_OFFSET(18042, glInvalidateFramebuffer, glInvalidateFramebuffer, NULL, 1012),
    NAME_FUNC_OFFSET(18066, glInvalidateSubFramebuffer, glInvalidateSubFramebuffer, NULL, 1013),
    NAME_FUNC_OFFSET(18093, glInvalidateTexImage, glInvalidateTexImage, NULL, 1014),
    NAME_FUNC_OFFSET(18114, glInvalidateTexSubImage, glInvalidateTexSubImage, NULL, 1015),
    NAME_FUNC_OFFSET(18138, gl_dispatch_stub_1016, gl_dispatch_stub_1016, NULL, 1016),
    NAME_FUNC_OFFSET(18157, gl_dispatch_stub_1017, gl_dispatch_stub_1017, NULL, 1017),
    NAME_FUNC_OFFSET(18171, gl_dispatch_stub_1018, gl_dispatch_stub_1018, NULL, 1018),
    NAME_FUNC_OFFSET(18186, gl_dispatch_stub_1019, gl_dispatch_stub_1019, NULL, 1019),
    NAME_FUNC_OFFSET(18200, gl_dispatch_stub_1020, gl_dispatch_stub_1020, NULL, 1020),
    NAME_FUNC_OFFSET(18215, gl_dispatch_stub_1021, gl_dispatch_stub_1021, NULL, 1021),
    NAME_FUNC_OFFSET(18229, gl_dispatch_stub_1022, gl_dispatch_stub_1022, NULL, 1022),
    NAME_FUNC_OFFSET(18244, gl_dispatch_stub_1023, gl_dispatch_stub_1023, NULL, 1023),
    NAME_FUNC_OFFSET(18258, gl_dispatch_stub_1024, gl_dispatch_stub_1024, NULL, 1024),
    NAME_FUNC_OFFSET(18273, glPointSizePointerOES, glPointSizePointerOES, NULL, 1025),
    NAME_FUNC_OFFSET(18295, gl_dispatch_stub_1026, gl_dispatch_stub_1026, NULL, 1026),
    NAME_FUNC_OFFSET(18313, gl_dispatch_stub_1027, gl_dispatch_stub_1027, NULL, 1027),
    NAME_FUNC_OFFSET(18330, gl_dispatch_stub_1028, gl_dispatch_stub_1028, NULL, 1028),
    NAME_FUNC_OFFSET(18350, glColorPointerEXT, glColorPointerEXT, NULL, 1029),
    NAME_FUNC_OFFSET(18368, glEdgeFlagPointerEXT, glEdgeFlagPointerEXT, NULL, 1030),
    NAME_FUNC_OFFSET(18389, glIndexPointerEXT, glIndexPointerEXT, NULL, 1031),
    NAME_FUNC_OFFSET(18407, glNormalPointerEXT, glNormalPointerEXT, NULL, 1032),
    NAME_FUNC_OFFSET(18426, glTexCoordPointerEXT, glTexCoordPointerEXT, NULL, 1033),
    NAME_FUNC_OFFSET(18447, glVertexPointerEXT, glVertexPointerEXT, NULL, 1034),
    NAME_FUNC_OFFSET(18466, gl_dispatch_stub_1035, gl_dispatch_stub_1035, NULL, 1035),
    NAME_FUNC_OFFSET(18490, gl_dispatch_stub_1036, gl_dispatch_stub_1036, NULL, 1036),
    NAME_FUNC_OFFSET(18512, gl_dispatch_stub_1037, gl_dispatch_stub_1037, NULL, 1037),
    NAME_FUNC_OFFSET(18534, gl_dispatch_stub_1038, gl_dispatch_stub_1038, NULL, 1038),
    NAME_FUNC_OFFSET(18557, gl_dispatch_stub_1039, gl_dispatch_stub_1039, NULL, 1039),
    NAME_FUNC_OFFSET(18582, gl_dispatch_stub_1040, gl_dispatch_stub_1040, NULL, 1040),
    NAME_FUNC_OFFSET(18604, gl_dispatch_stub_1041, gl_dispatch_stub_1041, NULL, 1041),
    NAME_FUNC_OFFSET(18632, gl_dispatch_stub_1042, gl_dispatch_stub_1042, NULL, 1042),
    NAME_FUNC_OFFSET(18655, gl_dispatch_stub_1043, gl_dispatch_stub_1043, NULL, 1043),
    NAME_FUNC_OFFSET(18675, glLockArraysEXT, glLockArraysEXT, NULL, 1044),
    NAME_FUNC_OFFSET(18691, gl_dispatch_stub_1045, gl_dispatch_stub_1045, NULL, 1045),
    NAME_FUNC_OFFSET(18710, gl_dispatch_stub_1046, gl_dispatch_stub_1046, NULL, 1046),
    NAME_FUNC_OFFSET(18730, gl_dispatch_stub_1047, gl_dispatch_stub_1047, NULL, 1047),
    NAME_FUNC_OFFSET(18749, gl_dispatch_stub_1048, gl_dispatch_stub_1048, NULL, 1048),
    NAME_FUNC_OFFSET(18769, gl_dispatch_stub_1049, gl_dispatch_stub_1049, NULL, 1049),
    NAME_FUNC_OFFSET(18788, gl_dispatch_stub_1050, gl_dispatch_stub_1050, NULL, 1050),
    NAME_FUNC_OFFSET(18808, gl_dispatch_stub_1051, gl_dispatch_stub_1051, NULL, 1051),
    NAME_FUNC_OFFSET(18828, gl_dispatch_stub_1052, gl_dispatch_stub_1052, NULL, 1052),
    NAME_FUNC_OFFSET(18849, gl_dispatch_stub_1053, gl_dispatch_stub_1053, NULL, 1053),
    NAME_FUNC_OFFSET(18868, gl_dispatch_stub_1054, gl_dispatch_stub_1054, NULL, 1054),
    NAME_FUNC_OFFSET(18888, gl_dispatch_stub_1055, gl_dispatch_stub_1055, NULL, 1055),
    NAME_FUNC_OFFSET(18907, gl_dispatch_stub_1056, gl_dispatch_stub_1056, NULL, 1056),
    NAME_FUNC_OFFSET(18927, gl_dispatch_stub_1057, gl_dispatch_stub_1057, NULL, 1057),
    NAME_FUNC_OFFSET(18946, gl_dispatch_stub_1058, gl_dispatch_stub_1058, NULL, 1058),
    NAME_FUNC_OFFSET(18966, gl_dispatch_stub_1059, gl_dispatch_stub_1059, NULL, 1059),
    NAME_FUNC_OFFSET(18986, gl_dispatch_stub_1060, gl_dispatch_stub_1060, NULL, 1060),
    NAME_FUNC_OFFSET(19007, gl_dispatch_stub_1061, gl_dispatch_stub_1061, NULL, 1061),
    NAME_FUNC_OFFSET(19026, gl_dispatch_stub_1062, gl_dispatch_stub_1062, NULL, 1062),
    NAME_FUNC_OFFSET(19046, gl_dispatch_stub_1063, gl_dispatch_stub_1063, NULL, 1063),
    NAME_FUNC_OFFSET(19065, gl_dispatch_stub_1064, gl_dispatch_stub_1064, NULL, 1064),
    NAME_FUNC_OFFSET(19085, gl_dispatch_stub_1065, gl_dispatch_stub_1065, NULL, 1065),
    NAME_FUNC_OFFSET(19104, gl_dispatch_stub_1066, gl_dispatch_stub_1066, NULL, 1066),
    NAME_FUNC_OFFSET(19124, gl_dispatch_stub_1067, gl_dispatch_stub_1067, NULL, 1067),
    NAME_FUNC_OFFSET(19144, gl_dispatch_stub_1068, gl_dispatch_stub_1068, NULL, 1068),
    NAME_FUNC_OFFSET(19165, gl_dispatch_stub_1069, gl_dispatch_stub_1069, NULL, 1069),
    NAME_FUNC_OFFSET(19184, gl_dispatch_stub_1070, gl_dispatch_stub_1070, NULL, 1070),
    NAME_FUNC_OFFSET(19204, gl_dispatch_stub_1071, gl_dispatch_stub_1071, NULL, 1071),
    NAME_FUNC_OFFSET(19223, gl_dispatch_stub_1072, gl_dispatch_stub_1072, NULL, 1072),
    NAME_FUNC_OFFSET(19243, gl_dispatch_stub_1073, gl_dispatch_stub_1073, NULL, 1073),
    NAME_FUNC_OFFSET(19262, gl_dispatch_stub_1074, gl_dispatch_stub_1074, NULL, 1074),
    NAME_FUNC_OFFSET(19282, gl_dispatch_stub_1075, gl_dispatch_stub_1075, NULL, 1075),
    NAME_FUNC_OFFSET(19302, gl_dispatch_stub_1076, gl_dispatch_stub_1076, NULL, 1076),
    NAME_FUNC_OFFSET(19323, gl_dispatch_stub_1077, gl_dispatch_stub_1077, NULL, 1077),
    NAME_FUNC_OFFSET(19349, gl_dispatch_stub_1078, gl_dispatch_stub_1078, NULL, 1078),
    NAME_FUNC_OFFSET(19375, gl_dispatch_stub_1079, gl_dispatch_stub_1079, NULL, 1079),
    NAME_FUNC_OFFSET(19403, gl_dispatch_stub_1080, gl_dispatch_stub_1080, NULL, 1080),
    NAME_FUNC_OFFSET(19431, gl_dispatch_stub_1081, gl_dispatch_stub_1081, NULL, 1081),
    NAME_FUNC_OFFSET(19459, gl_dispatch_stub_1082, gl_dispatch_stub_1082, NULL, 1082),
    NAME_FUNC_OFFSET(19487, gl_dispatch_stub_1083, gl_dispatch_stub_1083, NULL, 1083),
    NAME_FUNC_OFFSET(19513, gl_dispatch_stub_1084, gl_dispatch_stub_1084, NULL, 1084),
    NAME_FUNC_OFFSET(19539, gl_dispatch_stub_1085, gl_dispatch_stub_1085, NULL, 1085),
    NAME_FUNC_OFFSET(19567, gl_dispatch_stub_1086, gl_dispatch_stub_1086, NULL, 1086),
    NAME_FUNC_OFFSET(19595, gl_dispatch_stub_1087, gl_dispatch_stub_1087, NULL, 1087),
    NAME_FUNC_OFFSET(19623, gl_dispatch_stub_1088, gl_dispatch_stub_1088, NULL, 1088),
    NAME_FUNC_OFFSET(19651, gl_dispatch_stub_1089, gl_dispatch_stub_1089, NULL, 1089),
    NAME_FUNC_OFFSET(19677, gl_dispatch_stub_1090, gl_dispatch_stub_1090, NULL, 1090),
    NAME_FUNC_OFFSET(19703, gl_dispatch_stub_1091, gl_dispatch_stub_1091, NULL, 1091),
    NAME_FUNC_OFFSET(19731, gl_dispatch_stub_1092, gl_dispatch_stub_1092, NULL, 1092),
    NAME_FUNC_OFFSET(19759, gl_dispatch_stub_1093, gl_dispatch_stub_1093, NULL, 1093),
    NAME_FUNC_OFFSET(19787, gl_dispatch_stub_1094, gl_dispatch_stub_1094, NULL, 1094),
    NAME_FUNC_OFFSET(19815, glUnlockArraysEXT, glUnlockArraysEXT, NULL, 1095),
    NAME_FUNC_OFFSET(19833, gl_dispatch_stub_1096, gl_dispatch_stub_1096, NULL, 1096),
    NAME_FUNC_OFFSET(19852, gl_dispatch_stub_1097, gl_dispatch_stub_1097, NULL, 1097),
    NAME_FUNC_OFFSET(19878, glDebugMessageCallback, glDebugMessageCallback, NULL, 1098),
    NAME_FUNC_OFFSET(19901, glDebugMessageControl, glDebugMessageControl, NULL, 1099),
    NAME_FUNC_OFFSET(19923, glDebugMessageInsert, glDebugMessageInsert, NULL, 1100),
    NAME_FUNC_OFFSET(19944, glGetDebugMessageLog, glGetDebugMessageLog, NULL, 1101),
    NAME_FUNC_OFFSET(19965, glGetObjectLabel, glGetObjectLabel, NULL, 1102),
    NAME_FUNC_OFFSET(19982, glGetObjectPtrLabel, glGetObjectPtrLabel, NULL, 1103),
    NAME_FUNC_OFFSET(20002, glObjectLabel, glObjectLabel, NULL, 1104),
    NAME_FUNC_OFFSET(20016, glObjectPtrLabel, glObjectPtrLabel, NULL, 1105),
    NAME_FUNC_OFFSET(20033, glPopDebugGroup, glPopDebugGroup, NULL, 1106),
    NAME_FUNC_OFFSET(20049, glPushDebugGroup, glPushDebugGroup, NULL, 1107),
    NAME_FUNC_OFFSET(20066, glSecondaryColor3fEXT, glSecondaryColor3fEXT, NULL, 1108),
    NAME_FUNC_OFFSET(20088, glSecondaryColor3fvEXT, glSecondaryColor3fvEXT, NULL, 1109),
    NAME_FUNC_OFFSET(20111, glMultiDrawElementsEXT, glMultiDrawElementsEXT, NULL, 1110),
    NAME_FUNC_OFFSET(20134, glFogCoordfEXT, glFogCoordfEXT, NULL, 1111),
    NAME_FUNC_OFFSET(20149, glFogCoordfvEXT, glFogCoordfvEXT, NULL, 1112),
    NAME_FUNC_OFFSET(20165, gl_dispatch_stub_1113, gl_dispatch_stub_1113, NULL, 1113),
    NAME_FUNC_OFFSET(20185, gl_dispatch_stub_1114, gl_dispatch_stub_1114, NULL, 1114),
    NAME_FUNC_OFFSET(20203, gl_dispatch_stub_1115, gl_dispatch_stub_1115, NULL, 1115),
    NAME_FUNC_OFFSET(20222, gl_dispatch_stub_1116, gl_dispatch_stub_1116, NULL, 1116),
    NAME_FUNC_OFFSET(20240, gl_dispatch_stub_1117, gl_dispatch_stub_1117, NULL, 1117),
    NAME_FUNC_OFFSET(20259, gl_dispatch_stub_1118, gl_dispatch_stub_1118, NULL, 1118),
    NAME_FUNC_OFFSET(20277, gl_dispatch_stub_1119, gl_dispatch_stub_1119, NULL, 1119),
    NAME_FUNC_OFFSET(20296, gl_dispatch_stub_1120, gl_dispatch_stub_1120, NULL, 1120),
    NAME_FUNC_OFFSET(20314, gl_dispatch_stub_1121, gl_dispatch_stub_1121, NULL, 1121),
    NAME_FUNC_OFFSET(20333, gl_dispatch_stub_1122, gl_dispatch_stub_1122, NULL, 1122),
    NAME_FUNC_OFFSET(20358, gl_dispatch_stub_1123, gl_dispatch_stub_1123, NULL, 1123),
    NAME_FUNC_OFFSET(20385, gl_dispatch_stub_1124, gl_dispatch_stub_1124, NULL, 1124),
    NAME_FUNC_OFFSET(20409, gl_dispatch_stub_1125, gl_dispatch_stub_1125, NULL, 1125),
    NAME_FUNC_OFFSET(20428, gl_dispatch_stub_1126, gl_dispatch_stub_1126, NULL, 1126),
    NAME_FUNC_OFFSET(20454, gl_dispatch_stub_1127, gl_dispatch_stub_1127, NULL, 1127),
    NAME_FUNC_OFFSET(20480, gl_dispatch_stub_1128, gl_dispatch_stub_1128, NULL, 1128),
    NAME_FUNC_OFFSET(20501, gl_dispatch_stub_1129, gl_dispatch_stub_1129, NULL, 1129),
    NAME_FUNC_OFFSET(20518, gl_dispatch_stub_1130, gl_dispatch_stub_1130, NULL, 1130),
    NAME_FUNC_OFFSET(20539, gl_dispatch_stub_1131, gl_dispatch_stub_1131, NULL, 1131),
    NAME_FUNC_OFFSET(20561, gl_dispatch_stub_1132, gl_dispatch_stub_1132, NULL, 1132),
    NAME_FUNC_OFFSET(20583, gl_dispatch_stub_1133, gl_dispatch_stub_1133, NULL, 1133),
    NAME_FUNC_OFFSET(20605, gl_dispatch_stub_1134, gl_dispatch_stub_1134, NULL, 1134),
    NAME_FUNC_OFFSET(20621, gl_dispatch_stub_1135, gl_dispatch_stub_1135, NULL, 1135),
    NAME_FUNC_OFFSET(20646, gl_dispatch_stub_1136, gl_dispatch_stub_1136, NULL, 1136),
    NAME_FUNC_OFFSET(20671, gl_dispatch_stub_1137, gl_dispatch_stub_1137, NULL, 1137),
    NAME_FUNC_OFFSET(20699, gl_dispatch_stub_1138, gl_dispatch_stub_1138, NULL, 1138),
    NAME_FUNC_OFFSET(20715, gl_dispatch_stub_1139, gl_dispatch_stub_1139, NULL, 1139),
    NAME_FUNC_OFFSET(20734, gl_dispatch_stub_1140, gl_dispatch_stub_1140, NULL, 1140),
    NAME_FUNC_OFFSET(20754, gl_dispatch_stub_1141, gl_dispatch_stub_1141, NULL, 1141),
    NAME_FUNC_OFFSET(20773, gl_dispatch_stub_1142, gl_dispatch_stub_1142, NULL, 1142),
    NAME_FUNC_OFFSET(20793, gl_dispatch_stub_1143, gl_dispatch_stub_1143, NULL, 1143),
    NAME_FUNC_OFFSET(20812, gl_dispatch_stub_1144, gl_dispatch_stub_1144, NULL, 1144),
    NAME_FUNC_OFFSET(20832, gl_dispatch_stub_1145, gl_dispatch_stub_1145, NULL, 1145),
    NAME_FUNC_OFFSET(20851, gl_dispatch_stub_1146, gl_dispatch_stub_1146, NULL, 1146),
    NAME_FUNC_OFFSET(20871, gl_dispatch_stub_1147, gl_dispatch_stub_1147, NULL, 1147),
    NAME_FUNC_OFFSET(20890, gl_dispatch_stub_1148, gl_dispatch_stub_1148, NULL, 1148),
    NAME_FUNC_OFFSET(20910, gl_dispatch_stub_1149, gl_dispatch_stub_1149, NULL, 1149),
    NAME_FUNC_OFFSET(20929, gl_dispatch_stub_1150, gl_dispatch_stub_1150, NULL, 1150),
    NAME_FUNC_OFFSET(20949, gl_dispatch_stub_1151, gl_dispatch_stub_1151, NULL, 1151),
    NAME_FUNC_OFFSET(20968, gl_dispatch_stub_1152, gl_dispatch_stub_1152, NULL, 1152),
    NAME_FUNC_OFFSET(20988, gl_dispatch_stub_1153, gl_dispatch_stub_1153, NULL, 1153),
    NAME_FUNC_OFFSET(21007, gl_dispatch_stub_1154, gl_dispatch_stub_1154, NULL, 1154),
    NAME_FUNC_OFFSET(21027, gl_dispatch_stub_1155, gl_dispatch_stub_1155, NULL, 1155),
    NAME_FUNC_OFFSET(21046, gl_dispatch_stub_1156, gl_dispatch_stub_1156, NULL, 1156),
    NAME_FUNC_OFFSET(21066, gl_dispatch_stub_1157, gl_dispatch_stub_1157, NULL, 1157),
    NAME_FUNC_OFFSET(21085, gl_dispatch_stub_1158, gl_dispatch_stub_1158, NULL, 1158),
    NAME_FUNC_OFFSET(21105, gl_dispatch_stub_1159, gl_dispatch_stub_1159, NULL, 1159),
    NAME_FUNC_OFFSET(21124, gl_dispatch_stub_1160, gl_dispatch_stub_1160, NULL, 1160),
    NAME_FUNC_OFFSET(21144, gl_dispatch_stub_1161, gl_dispatch_stub_1161, NULL, 1161),
    NAME_FUNC_OFFSET(21163, gl_dispatch_stub_1162, gl_dispatch_stub_1162, NULL, 1162),
    NAME_FUNC_OFFSET(21183, gl_dispatch_stub_1163, gl_dispatch_stub_1163, NULL, 1163),
    NAME_FUNC_OFFSET(21203, gl_dispatch_stub_1164, gl_dispatch_stub_1164, NULL, 1164),
    NAME_FUNC_OFFSET(21224, gl_dispatch_stub_1165, gl_dispatch_stub_1165, NULL, 1165),
    NAME_FUNC_OFFSET(21248, gl_dispatch_stub_1166, gl_dispatch_stub_1166, NULL, 1166),
    NAME_FUNC_OFFSET(21269, gl_dispatch_stub_1167, gl_dispatch_stub_1167, NULL, 1167),
    NAME_FUNC_OFFSET(21290, gl_dispatch_stub_1168, gl_dispatch_stub_1168, NULL, 1168),
    NAME_FUNC_OFFSET(21311, gl_dispatch_stub_1169, gl_dispatch_stub_1169, NULL, 1169),
    NAME_FUNC_OFFSET(21332, gl_dispatch_stub_1170, gl_dispatch_stub_1170, NULL, 1170),
    NAME_FUNC_OFFSET(21353, gl_dispatch_stub_1171, gl_dispatch_stub_1171, NULL, 1171),
    NAME_FUNC_OFFSET(21374, gl_dispatch_stub_1172, gl_dispatch_stub_1172, NULL, 1172),
    NAME_FUNC_OFFSET(21395, gl_dispatch_stub_1173, gl_dispatch_stub_1173, NULL, 1173),
    NAME_FUNC_OFFSET(21416, gl_dispatch_stub_1174, gl_dispatch_stub_1174, NULL, 1174),
    NAME_FUNC_OFFSET(21437, gl_dispatch_stub_1175, gl_dispatch_stub_1175, NULL, 1175),
    NAME_FUNC_OFFSET(21458, gl_dispatch_stub_1176, gl_dispatch_stub_1176, NULL, 1176),
    NAME_FUNC_OFFSET(21479, gl_dispatch_stub_1177, gl_dispatch_stub_1177, NULL, 1177),
    NAME_FUNC_OFFSET(21500, gl_dispatch_stub_1178, gl_dispatch_stub_1178, NULL, 1178),
    NAME_FUNC_OFFSET(21522, gl_dispatch_stub_1179, gl_dispatch_stub_1179, NULL, 1179),
    NAME_FUNC_OFFSET(21549, gl_dispatch_stub_1180, gl_dispatch_stub_1180, NULL, 1180),
    NAME_FUNC_OFFSET(21576, gl_dispatch_stub_1181, gl_dispatch_stub_1181, NULL, 1181),
    NAME_FUNC_OFFSET(21600, gl_dispatch_stub_1182, gl_dispatch_stub_1182, NULL, 1182),
    NAME_FUNC_OFFSET(21624, gl_dispatch_stub_1183, gl_dispatch_stub_1183, NULL, 1183),
    NAME_FUNC_OFFSET(21646, gl_dispatch_stub_1184, gl_dispatch_stub_1184, NULL, 1184),
    NAME_FUNC_OFFSET(21668, gl_dispatch_stub_1185, gl_dispatch_stub_1185, NULL, 1185),
    NAME_FUNC_OFFSET(21690, gl_dispatch_stub_1186, gl_dispatch_stub_1186, NULL, 1186),
    NAME_FUNC_OFFSET(21715, gl_dispatch_stub_1187, gl_dispatch_stub_1187, NULL, 1187),
    NAME_FUNC_OFFSET(21739, gl_dispatch_stub_1188, gl_dispatch_stub_1188, NULL, 1188),
    NAME_FUNC_OFFSET(21761, gl_dispatch_stub_1189, gl_dispatch_stub_1189, NULL, 1189),
    NAME_FUNC_OFFSET(21783, gl_dispatch_stub_1190, gl_dispatch_stub_1190, NULL, 1190),
    NAME_FUNC_OFFSET(21805, gl_dispatch_stub_1191, gl_dispatch_stub_1191, NULL, 1191),
    NAME_FUNC_OFFSET(21831, gl_dispatch_stub_1192, gl_dispatch_stub_1192, NULL, 1192),
    NAME_FUNC_OFFSET(21854, gl_dispatch_stub_1193, gl_dispatch_stub_1193, NULL, 1193),
    NAME_FUNC_OFFSET(21878, gl_dispatch_stub_1194, gl_dispatch_stub_1194, NULL, 1194),
    NAME_FUNC_OFFSET(21896, gl_dispatch_stub_1195, gl_dispatch_stub_1195, NULL, 1195),
    NAME_FUNC_OFFSET(21911, gl_dispatch_stub_1196, gl_dispatch_stub_1196, NULL, 1196),
    NAME_FUNC_OFFSET(21942, gl_dispatch_stub_1197, gl_dispatch_stub_1197, NULL, 1197),
    NAME_FUNC_OFFSET(21965, gl_dispatch_stub_1198, gl_dispatch_stub_1198, NULL, 1198),
    NAME_FUNC_OFFSET(21988, gl_dispatch_stub_1199, gl_dispatch_stub_1199, NULL, 1199),
    NAME_FUNC_OFFSET(22011, gl_dispatch_stub_1200, gl_dispatch_stub_1200, NULL, 1200),
    NAME_FUNC_OFFSET(22042, gl_dispatch_stub_1201, gl_dispatch_stub_1201, NULL, 1201),
    NAME_FUNC_OFFSET(22073, gl_dispatch_stub_1202, gl_dispatch_stub_1202, NULL, 1202),
    NAME_FUNC_OFFSET(22101, gl_dispatch_stub_1203, gl_dispatch_stub_1203, NULL, 1203),
    NAME_FUNC_OFFSET(22130, gl_dispatch_stub_1204, gl_dispatch_stub_1204, NULL, 1204),
    NAME_FUNC_OFFSET(22158, gl_dispatch_stub_1205, gl_dispatch_stub_1205, NULL, 1205),
    NAME_FUNC_OFFSET(22187, glPrimitiveRestartNV, glPrimitiveRestartNV, NULL, 1206),
    NAME_FUNC_OFFSET(22208, gl_dispatch_stub_1207, gl_dispatch_stub_1207, NULL, 1207),
    NAME_FUNC_OFFSET(22225, gl_dispatch_stub_1208, gl_dispatch_stub_1208, NULL, 1208),
    NAME_FUNC_OFFSET(22238, gl_dispatch_stub_1209, gl_dispatch_stub_1209, NULL, 1209),
    NAME_FUNC_OFFSET(22252, gl_dispatch_stub_1210, gl_dispatch_stub_1210, NULL, 1210),
    NAME_FUNC_OFFSET(22269, glBindFramebufferEXT, glBindFramebufferEXT, NULL, 1211),
    NAME_FUNC_OFFSET(22290, glBindRenderbufferEXT, glBindRenderbufferEXT, NULL, 1212),
    NAME_FUNC_OFFSET(22312, gl_dispatch_stub_1213, gl_dispatch_stub_1213, NULL, 1213),
    NAME_FUNC_OFFSET(22336, gl_dispatch_stub_1214, gl_dispatch_stub_1214, NULL, 1214),
    NAME_FUNC_OFFSET(22366, glVertexAttribI1iEXT, glVertexAttribI1iEXT, NULL, 1215),
    NAME_FUNC_OFFSET(22387, glVertexAttribI1uiEXT, glVertexAttribI1uiEXT, NULL, 1216),
    NAME_FUNC_OFFSET(22409, glVertexAttribI2iEXT, glVertexAttribI2iEXT, NULL, 1217),
    NAME_FUNC_OFFSET(22430, glVertexAttribI2ivEXT, glVertexAttribI2ivEXT, NULL, 1218),
    NAME_FUNC_OFFSET(22452, glVertexAttribI2uiEXT, glVertexAttribI2uiEXT, NULL, 1219),
    NAME_FUNC_OFFSET(22474, glVertexAttribI2uivEXT, glVertexAttribI2uivEXT, NULL, 1220),
    NAME_FUNC_OFFSET(22497, glVertexAttribI3iEXT, glVertexAttribI3iEXT, NULL, 1221),
    NAME_FUNC_OFFSET(22518, glVertexAttribI3ivEXT, glVertexAttribI3ivEXT, NULL, 1222),
    NAME_FUNC_OFFSET(22540, glVertexAttribI3uiEXT, glVertexAttribI3uiEXT, NULL, 1223),
    NAME_FUNC_OFFSET(22562, glVertexAttribI3uivEXT, glVertexAttribI3uivEXT, NULL, 1224),
    NAME_FUNC_OFFSET(22585, glVertexAttribI4iEXT, glVertexAttribI4iEXT, NULL, 1225),
    NAME_FUNC_OFFSET(22606, glVertexAttribI4ivEXT, glVertexAttribI4ivEXT, NULL, 1226),
    NAME_FUNC_OFFSET(22628, glVertexAttribI4uiEXT, glVertexAttribI4uiEXT, NULL, 1227),
    NAME_FUNC_OFFSET(22650, glVertexAttribI4uivEXT, glVertexAttribI4uivEXT, NULL, 1228),
    NAME_FUNC_OFFSET(22673, glClearColorIiEXT, glClearColorIiEXT, NULL, 1229),
    NAME_FUNC_OFFSET(22691, glClearColorIuiEXT, glClearColorIuiEXT, NULL, 1230),
    NAME_FUNC_OFFSET(22710, gl_dispatch_stub_1231, gl_dispatch_stub_1231, NULL, 1231),
    NAME_FUNC_OFFSET(22732, gl_dispatch_stub_1232, gl_dispatch_stub_1232, NULL, 1232),
    NAME_FUNC_OFFSET(22754, gl_dispatch_stub_1233, gl_dispatch_stub_1233, NULL, 1233),
    NAME_FUNC_OFFSET(22778, gl_dispatch_stub_1234, gl_dispatch_stub_1234, NULL, 1234),
    NAME_FUNC_OFFSET(22798, gl_dispatch_stub_1235, gl_dispatch_stub_1235, NULL, 1235),
    NAME_FUNC_OFFSET(22819, gl_dispatch_stub_1236, gl_dispatch_stub_1236, NULL, 1236),
    NAME_FUNC_OFFSET(22850, gl_dispatch_stub_1237, gl_dispatch_stub_1237, NULL, 1237),
    NAME_FUNC_OFFSET(22881, gl_dispatch_stub_1238, gl_dispatch_stub_1238, NULL, 1238),
    NAME_FUNC_OFFSET(22914, gl_dispatch_stub_1239, gl_dispatch_stub_1239, NULL, 1239),
    NAME_FUNC_OFFSET(22942, gl_dispatch_stub_1240, gl_dispatch_stub_1240, NULL, 1240),
    NAME_FUNC_OFFSET(22973, gl_dispatch_stub_1241, gl_dispatch_stub_1241, NULL, 1241),
    NAME_FUNC_OFFSET(22999, gl_dispatch_stub_1242, gl_dispatch_stub_1242, NULL, 1242),
    NAME_FUNC_OFFSET(23030, gl_dispatch_stub_1243, gl_dispatch_stub_1243, NULL, 1243),
    NAME_FUNC_OFFSET(23058, gl_dispatch_stub_1244, gl_dispatch_stub_1244, NULL, 1244),
    NAME_FUNC_OFFSET(23081, gl_dispatch_stub_1245, gl_dispatch_stub_1245, NULL, 1245),
    NAME_FUNC_OFFSET(23106, gl_dispatch_stub_1246, gl_dispatch_stub_1246, NULL, 1246),
    NAME_FUNC_OFFSET(23125, gl_dispatch_stub_1247, gl_dispatch_stub_1247, NULL, 1247),
    NAME_FUNC_OFFSET(23150, gl_dispatch_stub_1248, gl_dispatch_stub_1248, NULL, 1248),
    NAME_FUNC_OFFSET(23172, glTextureBarrierNV, glTextureBarrierNV, NULL, 1249),
    NAME_FUNC_OFFSET(23191, gl_dispatch_stub_1250, gl_dispatch_stub_1250, NULL, 1250),
    NAME_FUNC_OFFSET(23205, gl_dispatch_stub_1251, gl_dispatch_stub_1251, NULL, 1251),
    NAME_FUNC_OFFSET(23227, gl_dispatch_stub_1252, gl_dispatch_stub_1252, NULL, 1252),
    NAME_FUNC_OFFSET(23241, gl_dispatch_stub_1253, gl_dispatch_stub_1253, NULL, 1253),
    NAME_FUNC_OFFSET(23260, gl_dispatch_stub_1254, gl_dispatch_stub_1254, NULL, 1254),
    NAME_FUNC_OFFSET(23281, gl_dispatch_stub_1255, gl_dispatch_stub_1255, NULL, 1255),
    NAME_FUNC_OFFSET(23312, gl_dispatch_stub_1256, gl_dispatch_stub_1256, NULL, 1256),
    NAME_FUNC_OFFSET(23342, gl_dispatch_stub_1257, gl_dispatch_stub_1257, NULL, 1257),
    NAME_FUNC_OFFSET(23365, gl_dispatch_stub_1258, gl_dispatch_stub_1258, NULL, 1258),
    NAME_FUNC_OFFSET(23388, gl_dispatch_stub_1259, gl_dispatch_stub_1259, NULL, 1259),
    NAME_FUNC_OFFSET(23415, gl_dispatch_stub_1260, gl_dispatch_stub_1260, NULL, 1260),
    NAME_FUNC_OFFSET(23437, gl_dispatch_stub_1261, gl_dispatch_stub_1261, NULL, 1261),
    NAME_FUNC_OFFSET(23460, gl_dispatch_stub_1262, gl_dispatch_stub_1262, NULL, 1262),
    NAME_FUNC_OFFSET(23483, gl_dispatch_stub_1263, gl_dispatch_stub_1263, NULL, 1263),
    NAME_FUNC_OFFSET(23503, gl_dispatch_stub_1264, gl_dispatch_stub_1264, NULL, 1264),
    NAME_FUNC_OFFSET(23530, gl_dispatch_stub_1265, gl_dispatch_stub_1265, NULL, 1265),
    NAME_FUNC_OFFSET(23556, gl_dispatch_stub_1266, gl_dispatch_stub_1266, NULL, 1266),
    NAME_FUNC_OFFSET(23582, gl_dispatch_stub_1267, gl_dispatch_stub_1267, NULL, 1267),
    NAME_FUNC_OFFSET(23606, gl_dispatch_stub_1268, gl_dispatch_stub_1268, NULL, 1268),
    NAME_FUNC_OFFSET(23634, gl_dispatch_stub_1269, gl_dispatch_stub_1269, NULL, 1269),
    NAME_FUNC_OFFSET(23658, gl_dispatch_stub_1270, gl_dispatch_stub_1270, NULL, 1270),
    NAME_FUNC_OFFSET(23682, gl_dispatch_stub_1271, gl_dispatch_stub_1271, NULL, 1271),
    NAME_FUNC_OFFSET(23707, gl_dispatch_stub_1272, gl_dispatch_stub_1272, NULL, 1272),
    NAME_FUNC_OFFSET(23736, gl_dispatch_stub_1273, gl_dispatch_stub_1273, NULL, 1273),
    NAME_FUNC_OFFSET(23767, gl_dispatch_stub_1274, gl_dispatch_stub_1274, NULL, 1274),
    NAME_FUNC_OFFSET(23806, gl_dispatch_stub_1275, gl_dispatch_stub_1275, NULL, 1275),
    NAME_FUNC_OFFSET(23835, glAlphaFuncx, glAlphaFuncx, NULL, 1276),
    NAME_FUNC_OFFSET(23848, glClearColorx, glClearColorx, NULL, 1277),
    NAME_FUNC_OFFSET(23862, glClearDepthx, glClearDepthx, NULL, 1278),
    NAME_FUNC_OFFSET(23876, glColor4x, glColor4x, NULL, 1279),
    NAME_FUNC_OFFSET(23886, glDepthRangex, glDepthRangex, NULL, 1280),
    NAME_FUNC_OFFSET(23900, glFogx, glFogx, NULL, 1281),
    NAME_FUNC_OFFSET(23907, glFogxv, glFogxv, NULL, 1282),
    NAME_FUNC_OFFSET(23915, glFrustumf, glFrustumf, NULL, 1283),
    NAME_FUNC_OFFSET(23926, glFrustumx, glFrustumx, NULL, 1284),
    NAME_FUNC_OFFSET(23937, glLightModelx, glLightModelx, NULL, 1285),
    NAME_FUNC_OFFSET(23951, glLightModelxv, glLightModelxv, NULL, 1286),
    NAME_FUNC_OFFSET(23966, glLightx, glLightx, NULL, 1287),
    NAME_FUNC_OFFSET(23975, glLightxv, glLightxv, NULL, 1288),
    NAME_FUNC_OFFSET(23985, glLineWidthx, glLineWidthx, NULL, 1289),
    NAME_FUNC_OFFSET(23998, glLoadMatrixx, glLoadMatrixx, NULL, 1290),
    NAME_FUNC_OFFSET(24012, glMaterialx, glMaterialx, NULL, 1291),
    NAME_FUNC_OFFSET(24024, glMaterialxv, glMaterialxv, NULL, 1292),
    NAME_FUNC_OFFSET(24037, glMultMatrixx, glMultMatrixx, NULL, 1293),
    NAME_FUNC_OFFSET(24051, glMultiTexCoord4x, glMultiTexCoord4x, NULL, 1294),
    NAME_FUNC_OFFSET(24069, glNormal3x, glNormal3x, NULL, 1295),
    NAME_FUNC_OFFSET(24080, glOrthof, glOrthof, NULL, 1296),
    NAME_FUNC_OFFSET(24089, glOrthox, glOrthox, NULL, 1297),
    NAME_FUNC_OFFSET(24098, glPointSizex, glPointSizex, NULL, 1298),
    NAME_FUNC_OFFSET(24111, glPolygonOffsetx, glPolygonOffsetx, NULL, 1299),
    NAME_FUNC_OFFSET(24128, glRotatex, glRotatex, NULL, 1300),
    NAME_FUNC_OFFSET(24138, glSampleCoveragex, glSampleCoveragex, NULL, 1301),
    NAME_FUNC_OFFSET(24156, glScalex, glScalex, NULL, 1302),
    NAME_FUNC_OFFSET(24165, glTexEnvx, glTexEnvx, NULL, 1303),
    NAME_FUNC_OFFSET(24175, glTexEnvxv, glTexEnvxv, NULL, 1304),
    NAME_FUNC_OFFSET(24186, glTexParameterx, glTexParameterx, NULL, 1305),
    NAME_FUNC_OFFSET(24202, glTranslatex, glTranslatex, NULL, 1306),
    NAME_FUNC_OFFSET(24215, glClipPlanef, glClipPlanef, NULL, 1307),
    NAME_FUNC_OFFSET(24228, glClipPlanex, glClipPlanex, NULL, 1308),
    NAME_FUNC_OFFSET(24241, glGetClipPlanef, glGetClipPlanef, NULL, 1309),
    NAME_FUNC_OFFSET(24257, glGetClipPlanex, glGetClipPlanex, NULL, 1310),
    NAME_FUNC_OFFSET(24273, glGetFixedv, glGetFixedv, NULL, 1311),
    NAME_FUNC_OFFSET(24285, glGetLightxv, glGetLightxv, NULL, 1312),
    NAME_FUNC_OFFSET(24298, glGetMaterialxv, glGetMaterialxv, NULL, 1313),
    NAME_FUNC_OFFSET(24314, glGetTexEnvxv, glGetTexEnvxv, NULL, 1314),
    NAME_FUNC_OFFSET(24328, glGetTexParameterxv, glGetTexParameterxv, NULL, 1315),
    NAME_FUNC_OFFSET(24348, glPointParameterx, glPointParameterx, NULL, 1316),
    NAME_FUNC_OFFSET(24366, glPointParameterxv, glPointParameterxv, NULL, 1317),
    NAME_FUNC_OFFSET(24385, glTexParameterxv, glTexParameterxv, NULL, 1318),
    NAME_FUNC_OFFSET(24402, glTexGenf, glTexGenf, NULL, 190),
    NAME_FUNC_OFFSET(24415, glTexGenfv, glTexGenfv, NULL, 191),
    NAME_FUNC_OFFSET(24429, glTexGeni, glTexGeni, NULL, 192),
    NAME_FUNC_OFFSET(24442, glTexGeniv, glTexGeniv, NULL, 193),
    NAME_FUNC_OFFSET(24456, glReadBuffer, glReadBuffer, NULL, 254),
    NAME_FUNC_OFFSET(24471, glGetTexGenfv, glGetTexGenfv, NULL, 279),
    NAME_FUNC_OFFSET(24488, glGetTexGeniv, glGetTexGeniv, NULL, 280),
    NAME_FUNC_OFFSET(24505, glArrayElement, glArrayElement, NULL, 306),
    NAME_FUNC_OFFSET(24523, glBindTexture, glBindTexture, NULL, 307),
    NAME_FUNC_OFFSET(24540, glDrawArrays, glDrawArrays, NULL, 310),
    NAME_FUNC_OFFSET(24556, glAreTexturesResident, glAreTexturesResidentEXT, glAreTexturesResidentEXT, 322),
    NAME_FUNC_OFFSET(24581, glCopyTexImage1D, glCopyTexImage1D, NULL, 323),
    NAME_FUNC_OFFSET(24601, glCopyTexImage2D, glCopyTexImage2D, NULL, 324),
    NAME_FUNC_OFFSET(24621, glCopyTexSubImage1D, glCopyTexSubImage1D, NULL, 325),
    NAME_FUNC_OFFSET(24644, glCopyTexSubImage2D, glCopyTexSubImage2D, NULL, 326),
    NAME_FUNC_OFFSET(24667, glDeleteTextures, glDeleteTexturesEXT, glDeleteTexturesEXT, 327),
    NAME_FUNC_OFFSET(24687, glGenTextures, glGenTexturesEXT, glGenTexturesEXT, 328),
    NAME_FUNC_OFFSET(24704, glGetPointerv, glGetPointerv, NULL, 329),
    NAME_FUNC_OFFSET(24721, glIsTexture, glIsTextureEXT, glIsTextureEXT, 330),
    NAME_FUNC_OFFSET(24736, glPrioritizeTextures, glPrioritizeTextures, NULL, 331),
    NAME_FUNC_OFFSET(24760, glTexSubImage1D, glTexSubImage1D, NULL, 332),
    NAME_FUNC_OFFSET(24779, glTexSubImage2D, glTexSubImage2D, NULL, 333),
    NAME_FUNC_OFFSET(24798, glBlendColor, glBlendColor, NULL, 336),
    NAME_FUNC_OFFSET(24814, glBlendEquation, glBlendEquation, NULL, 337),
    NAME_FUNC_OFFSET(24833, glBlendEquation, glBlendEquation, NULL, 337),
    NAME_FUNC_OFFSET(24852, glDrawRangeElements, glDrawRangeElements, NULL, 338),
    NAME_FUNC_OFFSET(24875, glColorTable, glColorTable, NULL, 339),
    NAME_FUNC_OFFSET(24891, glColorTable, glColorTable, NULL, 339),
    NAME_FUNC_OFFSET(24907, glColorTableParameterfv, glColorTableParameterfv, NULL, 340),
    NAME_FUNC_OFFSET(24934, glColorTableParameteriv, glColorTableParameteriv, NULL, 341),
    NAME_FUNC_OFFSET(24961, glCopyColorTable, glCopyColorTable, NULL, 342),
    NAME_FUNC_OFFSET(24981, glGetColorTable, gl_dispatch_stub_343, gl_dispatch_stub_343, 343),
    NAME_FUNC_OFFSET(25000, glGetColorTable, gl_dispatch_stub_343, gl_dispatch_stub_343, 343),
    NAME_FUNC_OFFSET(25019, glGetColorTableParameterfv, gl_dispatch_stub_344, gl_dispatch_stub_344, 344),
    NAME_FUNC_OFFSET(25049, glGetColorTableParameterfv, gl_dispatch_stub_344, gl_dispatch_stub_344, 344),
    NAME_FUNC_OFFSET(25079, glGetColorTableParameteriv, gl_dispatch_stub_345, gl_dispatch_stub_345, 345),
    NAME_FUNC_OFFSET(25109, glGetColorTableParameteriv, gl_dispatch_stub_345, gl_dispatch_stub_345, 345),
    NAME_FUNC_OFFSET(25139, glColorSubTable, glColorSubTable, NULL, 346),
    NAME_FUNC_OFFSET(25158, glCopyColorSubTable, glCopyColorSubTable, NULL, 347),
    NAME_FUNC_OFFSET(25181, glConvolutionFilter1D, glConvolutionFilter1D, NULL, 348),
    NAME_FUNC_OFFSET(25206, glConvolutionFilter2D, glConvolutionFilter2D, NULL, 349),
    NAME_FUNC_OFFSET(25231, glConvolutionParameterf, glConvolutionParameterf, NULL, 350),
    NAME_FUNC_OFFSET(25258, glConvolutionParameterfv, glConvolutionParameterfv, NULL, 351),
    NAME_FUNC_OFFSET(25286, glConvolutionParameteri, glConvolutionParameteri, NULL, 352),
    NAME_FUNC_OFFSET(25313, glConvolutionParameteriv, glConvolutionParameteriv, NULL, 353),
    NAME_FUNC_OFFSET(25341, glCopyConvolutionFilter1D, glCopyConvolutionFilter1D, NULL, 354),
    NAME_FUNC_OFFSET(25370, glCopyConvolutionFilter2D, glCopyConvolutionFilter2D, NULL, 355),
    NAME_FUNC_OFFSET(25399, glGetConvolutionFilter, gl_dispatch_stub_356, gl_dispatch_stub_356, 356),
    NAME_FUNC_OFFSET(25425, glGetConvolutionParameterfv, gl_dispatch_stub_357, gl_dispatch_stub_357, 357),
    NAME_FUNC_OFFSET(25456, glGetConvolutionParameteriv, gl_dispatch_stub_358, gl_dispatch_stub_358, 358),
    NAME_FUNC_OFFSET(25487, glGetSeparableFilter, gl_dispatch_stub_359, gl_dispatch_stub_359, 359),
    NAME_FUNC_OFFSET(25511, glSeparableFilter2D, glSeparableFilter2D, NULL, 360),
    NAME_FUNC_OFFSET(25534, glGetHistogram, gl_dispatch_stub_361, gl_dispatch_stub_361, 361),
    NAME_FUNC_OFFSET(25552, glGetHistogramParameterfv, gl_dispatch_stub_362, gl_dispatch_stub_362, 362),
    NAME_FUNC_OFFSET(25581, glGetHistogramParameteriv, gl_dispatch_stub_363, gl_dispatch_stub_363, 363),
    NAME_FUNC_OFFSET(25610, glGetMinmax, gl_dispatch_stub_364, gl_dispatch_stub_364, 364),
    NAME_FUNC_OFFSET(25625, glGetMinmaxParameterfv, gl_dispatch_stub_365, gl_dispatch_stub_365, 365),
    NAME_FUNC_OFFSET(25651, glGetMinmaxParameteriv, gl_dispatch_stub_366, gl_dispatch_stub_366, 366),
    NAME_FUNC_OFFSET(25677, glHistogram, glHistogram, NULL, 367),
    NAME_FUNC_OFFSET(25692, glMinmax, glMinmax, NULL, 368),
    NAME_FUNC_OFFSET(25704, glResetHistogram, glResetHistogram, NULL, 369),
    NAME_FUNC_OFFSET(25724, glResetMinmax, glResetMinmax, NULL, 370),
    NAME_FUNC_OFFSET(25741, glTexImage3D, glTexImage3D, NULL, 371),
    NAME_FUNC_OFFSET(25757, glTexImage3D, glTexImage3D, NULL, 371),
    NAME_FUNC_OFFSET(25773, glTexSubImage3D, glTexSubImage3D, NULL, 372),
    NAME_FUNC_OFFSET(25792, glTexSubImage3D, glTexSubImage3D, NULL, 372),
    NAME_FUNC_OFFSET(25811, glCopyTexSubImage3D, glCopyTexSubImage3D, NULL, 373),
    NAME_FUNC_OFFSET(25834, glCopyTexSubImage3D, glCopyTexSubImage3D, NULL, 373),
    NAME_FUNC_OFFSET(25857, glActiveTexture, glActiveTexture, NULL, 374),
    NAME_FUNC_OFFSET(25876, glClientActiveTexture, glClientActiveTexture, NULL, 375),
    NAME_FUNC_OFFSET(25901, glMultiTexCoord1d, glMultiTexCoord1d, NULL, 376),
    NAME_FUNC_OFFSET(25922, glMultiTexCoord1dv, glMultiTexCoord1dv, NULL, 377),
    NAME_FUNC_OFFSET(25944, glMultiTexCoord1fARB, glMultiTexCoord1fARB, NULL, 378),
    NAME_FUNC_OFFSET(25962, glMultiTexCoord1fvARB, glMultiTexCoord1fvARB, NULL, 379),
    NAME_FUNC_OFFSET(25981, glMultiTexCoord1i, glMultiTexCoord1i, NULL, 380),
    NAME_FUNC_OFFSET(26002, glMultiTexCoord1iv, glMultiTexCoord1iv, NULL, 381),
    NAME_FUNC_OFFSET(26024, glMultiTexCoord1s, glMultiTexCoord1s, NULL, 382),
    NAME_FUNC_OFFSET(26045, glMultiTexCoord1sv, glMultiTexCoord1sv, NULL, 383),
    NAME_FUNC_OFFSET(26067, glMultiTexCoord2d, glMultiTexCoord2d, NULL, 384),
    NAME_FUNC_OFFSET(26088, glMultiTexCoord2dv, glMultiTexCoord2dv, NULL, 385),
    NAME_FUNC_OFFSET(26110, glMultiTexCoord2fARB, glMultiTexCoord2fARB, NULL, 386),
    NAME_FUNC_OFFSET(26128, glMultiTexCoord2fvARB, glMultiTexCoord2fvARB, NULL, 387),
    NAME_FUNC_OFFSET(26147, glMultiTexCoord2i, glMultiTexCoord2i, NULL, 388),
    NAME_FUNC_OFFSET(26168, glMultiTexCoord2iv, glMultiTexCoord2iv, NULL, 389),
    NAME_FUNC_OFFSET(26190, glMultiTexCoord2s, glMultiTexCoord2s, NULL, 390),
    NAME_FUNC_OFFSET(26211, glMultiTexCoord2sv, glMultiTexCoord2sv, NULL, 391),
    NAME_FUNC_OFFSET(26233, glMultiTexCoord3d, glMultiTexCoord3d, NULL, 392),
    NAME_FUNC_OFFSET(26254, glMultiTexCoord3dv, glMultiTexCoord3dv, NULL, 393),
    NAME_FUNC_OFFSET(26276, glMultiTexCoord3fARB, glMultiTexCoord3fARB, NULL, 394),
    NAME_FUNC_OFFSET(26294, glMultiTexCoord3fvARB, glMultiTexCoord3fvARB, NULL, 395),
    NAME_FUNC_OFFSET(26313, glMultiTexCoord3i, glMultiTexCoord3i, NULL, 396),
    NAME_FUNC_OFFSET(26334, glMultiTexCoord3iv, glMultiTexCoord3iv, NULL, 397),
    NAME_FUNC_OFFSET(26356, glMultiTexCoord3s, glMultiTexCoord3s, NULL, 398),
    NAME_FUNC_OFFSET(26377, glMultiTexCoord3sv, glMultiTexCoord3sv, NULL, 399),
    NAME_FUNC_OFFSET(26399, glMultiTexCoord4d, glMultiTexCoord4d, NULL, 400),
    NAME_FUNC_OFFSET(26420, glMultiTexCoord4dv, glMultiTexCoord4dv, NULL, 401),
    NAME_FUNC_OFFSET(26442, glMultiTexCoord4fARB, glMultiTexCoord4fARB, NULL, 402),
    NAME_FUNC_OFFSET(26460, glMultiTexCoord4fvARB, glMultiTexCoord4fvARB, NULL, 403),
    NAME_FUNC_OFFSET(26479, glMultiTexCoord4i, glMultiTexCoord4i, NULL, 404),
    NAME_FUNC_OFFSET(26500, glMultiTexCoord4iv, glMultiTexCoord4iv, NULL, 405),
    NAME_FUNC_OFFSET(26522, glMultiTexCoord4s, glMultiTexCoord4s, NULL, 406),
    NAME_FUNC_OFFSET(26543, glMultiTexCoord4sv, glMultiTexCoord4sv, NULL, 407),
    NAME_FUNC_OFFSET(26565, glCompressedTexImage1D, glCompressedTexImage1D, NULL, 408),
    NAME_FUNC_OFFSET(26591, glCompressedTexImage2D, glCompressedTexImage2D, NULL, 409),
    NAME_FUNC_OFFSET(26617, glCompressedTexImage3D, glCompressedTexImage3D, NULL, 410),
    NAME_FUNC_OFFSET(26643, glCompressedTexImage3D, glCompressedTexImage3D, NULL, 410),
    NAME_FUNC_OFFSET(26669, glCompressedTexSubImage1D, glCompressedTexSubImage1D, NULL, 411),
    NAME_FUNC_OFFSET(26698, glCompressedTexSubImage2D, glCompressedTexSubImage2D, NULL, 412),
    NAME_FUNC_OFFSET(26727, glCompressedTexSubImage3D, glCompressedTexSubImage3D, NULL, 413),
    NAME_FUNC_OFFSET(26756, glCompressedTexSubImage3D, glCompressedTexSubImage3D, NULL, 413),
    NAME_FUNC_OFFSET(26785, glGetCompressedTexImage, glGetCompressedTexImage, NULL, 414),
    NAME_FUNC_OFFSET(26812, glLoadTransposeMatrixd, glLoadTransposeMatrixd, NULL, 415),
    NAME_FUNC_OFFSET(26838, glLoadTransposeMatrixf, glLoadTransposeMatrixf, NULL, 416),
    NAME_FUNC_OFFSET(26864, glMultTransposeMatrixd, glMultTransposeMatrixd, NULL, 417),
    NAME_FUNC_OFFSET(26890, glMultTransposeMatrixf, glMultTransposeMatrixf, NULL, 418),
    NAME_FUNC_OFFSET(26916, glSampleCoverage, glSampleCoverage, NULL, 419),
    NAME_FUNC_OFFSET(26936, glBlendFuncSeparate, glBlendFuncSeparate, NULL, 420),
    NAME_FUNC_OFFSET(26959, glBlendFuncSeparate, glBlendFuncSeparate, NULL, 420),
    NAME_FUNC_OFFSET(26983, glBlendFuncSeparate, glBlendFuncSeparate, NULL, 420),
    NAME_FUNC_OFFSET(27006, glFogCoordPointer, glFogCoordPointer, NULL, 421),
    NAME_FUNC_OFFSET(27027, glFogCoordd, glFogCoordd, NULL, 422),
    NAME_FUNC_OFFSET(27042, glFogCoorddv, glFogCoorddv, NULL, 423),
    NAME_FUNC_OFFSET(27058, glMultiDrawArrays, glMultiDrawArrays, NULL, 424),
    NAME_FUNC_OFFSET(27079, glPointParameterf, glPointParameterf, NULL, 425),
    NAME_FUNC_OFFSET(27100, glPointParameterf, glPointParameterf, NULL, 425),
    NAME_FUNC_OFFSET(27121, glPointParameterf, glPointParameterf, NULL, 425),
    NAME_FUNC_OFFSET(27143, glPointParameterfv, glPointParameterfv, NULL, 426),
    NAME_FUNC_OFFSET(27165, glPointParameterfv, glPointParameterfv, NULL, 426),
    NAME_FUNC_OFFSET(27187, glPointParameterfv, glPointParameterfv, NULL, 426),
    NAME_FUNC_OFFSET(27210, glPointParameteri, glPointParameteri, NULL, 427),
    NAME_FUNC_OFFSET(27230, glPointParameteriv, glPointParameteriv, NULL, 428),
    NAME_FUNC_OFFSET(27251, glSecondaryColor3b, glSecondaryColor3b, NULL, 429),
    NAME_FUNC_OFFSET(27273, glSecondaryColor3bv, glSecondaryColor3bv, NULL, 430),
    NAME_FUNC_OFFSET(27296, glSecondaryColor3d, glSecondaryColor3d, NULL, 431),
    NAME_FUNC_OFFSET(27318, glSecondaryColor3dv, glSecondaryColor3dv, NULL, 432),
    NAME_FUNC_OFFSET(27341, glSecondaryColor3i, glSecondaryColor3i, NULL, 433),
    NAME_FUNC_OFFSET(27363, glSecondaryColor3iv, glSecondaryColor3iv, NULL, 434),
    NAME_FUNC_OFFSET(27386, glSecondaryColor3s, glSecondaryColor3s, NULL, 435),
    NAME_FUNC_OFFSET(27408, glSecondaryColor3sv, glSecondaryColor3sv, NULL, 436),
    NAME_FUNC_OFFSET(27431, glSecondaryColor3ub, glSecondaryColor3ub, NULL, 437),
    NAME_FUNC_OFFSET(27454, glSecondaryColor3ubv, glSecondaryColor3ubv, NULL, 438),
    NAME_FUNC_OFFSET(27478, glSecondaryColor3ui, glSecondaryColor3ui, NULL, 439),
    NAME_FUNC_OFFSET(27501, glSecondaryColor3uiv, glSecondaryColor3uiv, NULL, 440),
    NAME_FUNC_OFFSET(27525, glSecondaryColor3us, glSecondaryColor3us, NULL, 441),
    NAME_FUNC_OFFSET(27548, glSecondaryColor3usv, glSecondaryColor3usv, NULL, 442),
    NAME_FUNC_OFFSET(27572, glSecondaryColorPointer, glSecondaryColorPointer, NULL, 443),
    NAME_FUNC_OFFSET(27599, glWindowPos2d, glWindowPos2d, NULL, 444),
    NAME_FUNC_OFFSET(27616, glWindowPos2d, glWindowPos2d, NULL, 444),
    NAME_FUNC_OFFSET(27634, glWindowPos2dv, glWindowPos2dv, NULL, 445),
    NAME_FUNC_OFFSET(27652, glWindowPos2dv, glWindowPos2dv, NULL, 445),
    NAME_FUNC_OFFSET(27671, glWindowPos2f, glWindowPos2f, NULL, 446),
    NAME_FUNC_OFFSET(27688, glWindowPos2f, glWindowPos2f, NULL, 446),
    NAME_FUNC_OFFSET(27706, glWindowPos2fv, glWindowPos2fv, NULL, 447),
    NAME_FUNC_OFFSET(27724, glWindowPos2fv, glWindowPos2fv, NULL, 447),
    NAME_FUNC_OFFSET(27743, glWindowPos2i, glWindowPos2i, NULL, 448),
    NAME_FUNC_OFFSET(27760, glWindowPos2i, glWindowPos2i, NULL, 448),
    NAME_FUNC_OFFSET(27778, glWindowPos2iv, glWindowPos2iv, NULL, 449),
    NAME_FUNC_OFFSET(27796, glWindowPos2iv, glWindowPos2iv, NULL, 449),
    NAME_FUNC_OFFSET(27815, glWindowPos2s, glWindowPos2s, NULL, 450),
    NAME_FUNC_OFFSET(27832, glWindowPos2s, glWindowPos2s, NULL, 450),
    NAME_FUNC_OFFSET(27850, glWindowPos2sv, glWindowPos2sv, NULL, 451),
    NAME_FUNC_OFFSET(27868, glWindowPos2sv, glWindowPos2sv, NULL, 451),
    NAME_FUNC_OFFSET(27887, glWindowPos3d, glWindowPos3d, NULL, 452),
    NAME_FUNC_OFFSET(27904, glWindowPos3d, glWindowPos3d, NULL, 452),
    NAME_FUNC_OFFSET(27922, glWindowPos3dv, glWindowPos3dv, NULL, 453),
    NAME_FUNC_OFFSET(27940, glWindowPos3dv, glWindowPos3dv, NULL, 453),
    NAME_FUNC_OFFSET(27959, glWindowPos3f, glWindowPos3f, NULL, 454),
    NAME_FUNC_OFFSET(27976, glWindowPos3f, glWindowPos3f, NULL, 454),
    NAME_FUNC_OFFSET(27994, glWindowPos3fv, glWindowPos3fv, NULL, 455),
    NAME_FUNC_OFFSET(28012, glWindowPos3fv, glWindowPos3fv, NULL, 455),
    NAME_FUNC_OFFSET(28031, glWindowPos3i, glWindowPos3i, NULL, 456),
    NAME_FUNC_OFFSET(28048, glWindowPos3i, glWindowPos3i, NULL, 456),
    NAME_FUNC_OFFSET(28066, glWindowPos3iv, glWindowPos3iv, NULL, 457),
    NAME_FUNC_OFFSET(28084, glWindowPos3iv, glWindowPos3iv, NULL, 457),
    NAME_FUNC_OFFSET(28103, glWindowPos3s, glWindowPos3s, NULL, 458),
    NAME_FUNC_OFFSET(28120, glWindowPos3s, glWindowPos3s, NULL, 458),
    NAME_FUNC_OFFSET(28138, glWindowPos3sv, glWindowPos3sv, NULL, 459),
    NAME_FUNC_OFFSET(28156, glWindowPos3sv, glWindowPos3sv, NULL, 459),
    NAME_FUNC_OFFSET(28175, glBeginQuery, glBeginQuery, NULL, 460),
    NAME_FUNC_OFFSET(28191, glBindBuffer, glBindBuffer, NULL, 461),
    NAME_FUNC_OFFSET(28207, glBufferData, glBufferData, NULL, 462),
    NAME_FUNC_OFFSET(28223, glBufferSubData, glBufferSubData, NULL, 463),
    NAME_FUNC_OFFSET(28242, glDeleteBuffers, glDeleteBuffers, NULL, 464),
    NAME_FUNC_OFFSET(28261, glDeleteQueries, glDeleteQueries, NULL, 465),
    NAME_FUNC_OFFSET(28280, glEndQuery, glEndQuery, NULL, 466),
    NAME_FUNC_OFFSET(28294, glGenBuffers, glGenBuffers, NULL, 467),
    NAME_FUNC_OFFSET(28310, glGenQueries, glGenQueries, NULL, 468),
    NAME_FUNC_OFFSET(28326, glGetBufferParameteriv, glGetBufferParameteriv, NULL, 469),
    NAME_FUNC_OFFSET(28352, glGetBufferPointerv, glGetBufferPointerv, NULL, 470),
    NAME_FUNC_OFFSET(28375, glGetBufferPointerv, glGetBufferPointerv, NULL, 470),
    NAME_FUNC_OFFSET(28398, glGetBufferSubData, glGetBufferSubData, NULL, 471),
    NAME_FUNC_OFFSET(28420, glGetQueryObjectiv, glGetQueryObjectiv, NULL, 472),
    NAME_FUNC_OFFSET(28442, glGetQueryObjectuiv, glGetQueryObjectuiv, NULL, 473),
    NAME_FUNC_OFFSET(28465, glGetQueryiv, glGetQueryiv, NULL, 474),
    NAME_FUNC_OFFSET(28481, glIsBuffer, glIsBuffer, NULL, 475),
    NAME_FUNC_OFFSET(28495, glIsQuery, glIsQuery, NULL, 476),
    NAME_FUNC_OFFSET(28508, glMapBuffer, glMapBuffer, NULL, 477),
    NAME_FUNC_OFFSET(28523, glMapBuffer, glMapBuffer, NULL, 477),
    NAME_FUNC_OFFSET(28538, glUnmapBuffer, glUnmapBuffer, NULL, 478),
    NAME_FUNC_OFFSET(28555, glUnmapBuffer, glUnmapBuffer, NULL, 478),
    NAME_FUNC_OFFSET(28572, glBindAttribLocation, glBindAttribLocation, NULL, 480),
    NAME_FUNC_OFFSET(28596, glBlendEquationSeparate, glBlendEquationSeparate, NULL, 481),
    NAME_FUNC_OFFSET(28623, glBlendEquationSeparate, glBlendEquationSeparate, NULL, 481),
    NAME_FUNC_OFFSET(28650, glBlendEquationSeparate, glBlendEquationSeparate, NULL, 481),
    NAME_FUNC_OFFSET(28677, glCompileShader, glCompileShader, NULL, 482),
    NAME_FUNC_OFFSET(28696, glDisableVertexAttribArray, glDisableVertexAttribArray, NULL, 488),
    NAME_FUNC_OFFSET(28726, glDrawBuffers, glDrawBuffers, NULL, 489),
    NAME_FUNC_OFFSET(28743, glDrawBuffers, glDrawBuffers, NULL, 489),
    NAME_FUNC_OFFSET(28760, glDrawBuffers, glDrawBuffers, NULL, 489),
    NAME_FUNC_OFFSET(28776, glDrawBuffers, glDrawBuffers, NULL, 489),
    NAME_FUNC_OFFSET(28793, glEnableVertexAttribArray, glEnableVertexAttribArray, NULL, 490),
    NAME_FUNC_OFFSET(28822, glGetActiveAttrib, glGetActiveAttrib, NULL, 491),
    NAME_FUNC_OFFSET(28843, glGetActiveUniform, glGetActiveUniform, NULL, 492),
    NAME_FUNC_OFFSET(28865, glGetAttribLocation, glGetAttribLocation, NULL, 494),
    NAME_FUNC_OFFSET(28888, glGetShaderSource, glGetShaderSource, NULL, 498),
    NAME_FUNC_OFFSET(28909, glGetUniformLocation, glGetUniformLocation, NULL, 500),
    NAME_FUNC_OFFSET(28933, glGetUniformfv, glGetUniformfv, NULL, 501),
    NAME_FUNC_OFFSET(28951, glGetUniformiv, glGetUniformiv, NULL, 502),
    NAME_FUNC_OFFSET(28969, glGetVertexAttribPointerv, glGetVertexAttribPointerv, NULL, 503),
    NAME_FUNC_OFFSET(28998, glGetVertexAttribPointerv, glGetVertexAttribPointerv, NULL, 503),
    NAME_FUNC_OFFSET(29026, glGetVertexAttribdv, glGetVertexAttribdv, NULL, 504),
    NAME_FUNC_OFFSET(29049, glGetVertexAttribfv, glGetVertexAttribfv, NULL, 505),
    NAME_FUNC_OFFSET(29072, glGetVertexAttribiv, glGetVertexAttribiv, NULL, 506),
    NAME_FUNC_OFFSET(29095, glLinkProgram, glLinkProgram, NULL, 509),
    NAME_FUNC_OFFSET(29112, glShaderSource, glShaderSource, NULL, 510),
    NAME_FUNC_OFFSET(29130, glStencilOpSeparate, glStencilOpSeparate, NULL, 513),
    NAME_FUNC_OFFSET(29153, glUniform1f, glUniform1f, NULL, 514),
    NAME_FUNC_OFFSET(29168, glUniform1fv, glUniform1fv, NULL, 515),
    NAME_FUNC_OFFSET(29184, glUniform1i, glUniform1i, NULL, 516),
    NAME_FUNC_OFFSET(29199, glUniform1iv, glUniform1iv, NULL, 517),
    NAME_FUNC_OFFSET(29215, glUniform2f, glUniform2f, NULL, 518),
    NAME_FUNC_OFFSET(29230, glUniform2fv, glUniform2fv, NULL, 519),
    NAME_FUNC_OFFSET(29246, glUniform2i, glUniform2i, NULL, 520),
    NAME_FUNC_OFFSET(29261, glUniform2iv, glUniform2iv, NULL, 521),
    NAME_FUNC_OFFSET(29277, glUniform3f, glUniform3f, NULL, 522),
    NAME_FUNC_OFFSET(29292, glUniform3fv, glUniform3fv, NULL, 523),
    NAME_FUNC_OFFSET(29308, glUniform3i, glUniform3i, NULL, 524),
    NAME_FUNC_OFFSET(29323, glUniform3iv, glUniform3iv, NULL, 525),
    NAME_FUNC_OFFSET(29339, glUniform4f, glUniform4f, NULL, 526),
    NAME_FUNC_OFFSET(29354, glUniform4fv, glUniform4fv, NULL, 527),
    NAME_FUNC_OFFSET(29370, glUniform4i, glUniform4i, NULL, 528),
    NAME_FUNC_OFFSET(29385, glUniform4iv, glUniform4iv, NULL, 529),
    NAME_FUNC_OFFSET(29401, glUniformMatrix2fv, glUniformMatrix2fv, NULL, 530),
    NAME_FUNC_OFFSET(29423, glUniformMatrix3fv, glUniformMatrix3fv, NULL, 531),
    NAME_FUNC_OFFSET(29445, glUniformMatrix4fv, glUniformMatrix4fv, NULL, 532),
    NAME_FUNC_OFFSET(29467, glUseProgram, glUseProgram, NULL, 533),
    NAME_FUNC_OFFSET(29489, glValidateProgram, glValidateProgram, NULL, 534),
    NAME_FUNC_OFFSET(29510, glVertexAttrib1d, glVertexAttrib1d, NULL, 535),
    NAME_FUNC_OFFSET(29530, glVertexAttrib1dv, glVertexAttrib1dv, NULL, 536),
    NAME_FUNC_OFFSET(29551, glVertexAttrib1s, glVertexAttrib1s, NULL, 537),
    NAME_FUNC_OFFSET(29571, glVertexAttrib1sv, glVertexAttrib1sv, NULL, 538),
    NAME_FUNC_OFFSET(29592, glVertexAttrib2d, glVertexAttrib2d, NULL, 539),
    NAME_FUNC_OFFSET(29612, glVertexAttrib2dv, glVertexAttrib2dv, NULL, 540),
    NAME_FUNC_OFFSET(29633, glVertexAttrib2s, glVertexAttrib2s, NULL, 541),
    NAME_FUNC_OFFSET(29653, glVertexAttrib2sv, glVertexAttrib2sv, NULL, 542),
    NAME_FUNC_OFFSET(29674, glVertexAttrib3d, glVertexAttrib3d, NULL, 543),
    NAME_FUNC_OFFSET(29694, glVertexAttrib3dv, glVertexAttrib3dv, NULL, 544),
    NAME_FUNC_OFFSET(29715, glVertexAttrib3s, glVertexAttrib3s, NULL, 545),
    NAME_FUNC_OFFSET(29735, glVertexAttrib3sv, glVertexAttrib3sv, NULL, 546),
    NAME_FUNC_OFFSET(29756, glVertexAttrib4Nbv, glVertexAttrib4Nbv, NULL, 547),
    NAME_FUNC_OFFSET(29778, glVertexAttrib4Niv, glVertexAttrib4Niv, NULL, 548),
    NAME_FUNC_OFFSET(29800, glVertexAttrib4Nsv, glVertexAttrib4Nsv, NULL, 549),
    NAME_FUNC_OFFSET(29822, glVertexAttrib4Nub, glVertexAttrib4Nub, NULL, 550),
    NAME_FUNC_OFFSET(29844, glVertexAttrib4Nubv, glVertexAttrib4Nubv, NULL, 551),
    NAME_FUNC_OFFSET(29867, glVertexAttrib4Nuiv, glVertexAttrib4Nuiv, NULL, 552),
    NAME_FUNC_OFFSET(29890, glVertexAttrib4Nusv, glVertexAttrib4Nusv, NULL, 553),
    NAME_FUNC_OFFSET(29913, glVertexAttrib4bv, glVertexAttrib4bv, NULL, 554),
    NAME_FUNC_OFFSET(29934, glVertexAttrib4d, glVertexAttrib4d, NULL, 555),
    NAME_FUNC_OFFSET(29954, glVertexAttrib4dv, glVertexAttrib4dv, NULL, 556),
    NAME_FUNC_OFFSET(29975, glVertexAttrib4iv, glVertexAttrib4iv, NULL, 557),
    NAME_FUNC_OFFSET(29996, glVertexAttrib4s, glVertexAttrib4s, NULL, 558),
    NAME_FUNC_OFFSET(30016, glVertexAttrib4sv, glVertexAttrib4sv, NULL, 559),
    NAME_FUNC_OFFSET(30037, glVertexAttrib4ubv, glVertexAttrib4ubv, NULL, 560),
    NAME_FUNC_OFFSET(30059, glVertexAttrib4uiv, glVertexAttrib4uiv, NULL, 561),
    NAME_FUNC_OFFSET(30081, glVertexAttrib4usv, glVertexAttrib4usv, NULL, 562),
    NAME_FUNC_OFFSET(30103, glVertexAttribPointer, glVertexAttribPointer, NULL, 563),
    NAME_FUNC_OFFSET(30128, glBeginConditionalRender, glBeginConditionalRender, NULL, 570),
    NAME_FUNC_OFFSET(30155, glBeginTransformFeedback, glBeginTransformFeedback, NULL, 571),
    NAME_FUNC_OFFSET(30183, glBindBufferBase, glBindBufferBase, NULL, 572),
    NAME_FUNC_OFFSET(30203, glBindBufferRange, glBindBufferRange, NULL, 573),
    NAME_FUNC_OFFSET(30224, glBindFragDataLocation, glBindFragDataLocation, NULL, 574),
    NAME_FUNC_OFFSET(30250, glClampColor, glClampColor, NULL, 575),
    NAME_FUNC_OFFSET(30266, glColorMaski, glColorMaski, NULL, 580),
    NAME_FUNC_OFFSET(30288, glDisablei, glDisablei, NULL, 581),
    NAME_FUNC_OFFSET(30308, glEnablei, glEnablei, NULL, 582),
    NAME_FUNC_OFFSET(30327, glEndConditionalRender, glEndConditionalRender, NULL, 583),
    NAME_FUNC_OFFSET(30352, glEndTransformFeedback, glEndTransformFeedback, NULL, 584),
    NAME_FUNC_OFFSET(30378, glGetBooleani_v, glGetBooleani_v, NULL, 585),
    NAME_FUNC_OFFSET(30402, glGetFragDataLocation, glGetFragDataLocation, NULL, 586),
    NAME_FUNC_OFFSET(30427, glGetIntegeri_v, glGetIntegeri_v, NULL, 587),
    NAME_FUNC_OFFSET(30451, glGetTexParameterIiv, glGetTexParameterIiv, NULL, 589),
    NAME_FUNC_OFFSET(30475, glGetTexParameterIuiv, glGetTexParameterIuiv, NULL, 590),
    NAME_FUNC_OFFSET(30500, glGetTransformFeedbackVarying, glGetTransformFeedbackVarying, NULL, 591),
    NAME_FUNC_OFFSET(30533, glGetUniformuiv, glGetUniformuiv, NULL, 592),
    NAME_FUNC_OFFSET(30552, glGetVertexAttribIiv, glGetVertexAttribIiv, NULL, 593),
    NAME_FUNC_OFFSET(30576, glGetVertexAttribIuiv, glGetVertexAttribIuiv, NULL, 594),
    NAME_FUNC_OFFSET(30601, glIsEnabledi, glIsEnabledi, NULL, 595),
    NAME_FUNC_OFFSET(30623, glTexParameterIiv, glTexParameterIiv, NULL, 596),
    NAME_FUNC_OFFSET(30644, glTexParameterIuiv, glTexParameterIuiv, NULL, 597),
    NAME_FUNC_OFFSET(30666, glTransformFeedbackVaryings, glTransformFeedbackVaryings, NULL, 598),
    NAME_FUNC_OFFSET(30697, glUniform1ui, glUniform1ui, NULL, 599),
    NAME_FUNC_OFFSET(30713, glUniform1uiv, glUniform1uiv, NULL, 600),
    NAME_FUNC_OFFSET(30730, glUniform2ui, glUniform2ui, NULL, 601),
    NAME_FUNC_OFFSET(30746, glUniform2uiv, glUniform2uiv, NULL, 602),
    NAME_FUNC_OFFSET(30763, glUniform3ui, glUniform3ui, NULL, 603),
    NAME_FUNC_OFFSET(30779, glUniform3uiv, glUniform3uiv, NULL, 604),
    NAME_FUNC_OFFSET(30796, glUniform4ui, glUniform4ui, NULL, 605),
    NAME_FUNC_OFFSET(30812, glUniform4uiv, glUniform4uiv, NULL, 606),
    NAME_FUNC_OFFSET(30829, glVertexAttribI1iv, glVertexAttribI1iv, NULL, 607),
    NAME_FUNC_OFFSET(30851, glVertexAttribI1uiv, glVertexAttribI1uiv, NULL, 608),
    NAME_FUNC_OFFSET(30874, glVertexAttribI4bv, glVertexAttribI4bv, NULL, 609),
    NAME_FUNC_OFFSET(30896, glVertexAttribI4sv, glVertexAttribI4sv, NULL, 610),
    NAME_FUNC_OFFSET(30918, glVertexAttribI4ubv, glVertexAttribI4ubv, NULL, 611),
    NAME_FUNC_OFFSET(30941, glVertexAttribI4usv, glVertexAttribI4usv, NULL, 612),
    NAME_FUNC_OFFSET(30964, glVertexAttribIPointer, glVertexAttribIPointer, NULL, 613),
    NAME_FUNC_OFFSET(30990, glPrimitiveRestartIndex, glPrimitiveRestartIndex, NULL, 614),
    NAME_FUNC_OFFSET(31016, glTexBuffer, glTexBuffer, NULL, 615),
    NAME_FUNC_OFFSET(31031, glFramebufferTexture, glFramebufferTexture, NULL, 616),
    NAME_FUNC_OFFSET(31055, glVertexAttribDivisor, glVertexAttribDivisor, NULL, 619),
    NAME_FUNC_OFFSET(31080, glMinSampleShading, glMinSampleShading, NULL, 620),
    NAME_FUNC_OFFSET(31102, glBindProgramARB, glBindProgramARB, NULL, 622),
    NAME_FUNC_OFFSET(31118, glDeleteProgramsARB, glDeleteProgramsARB, NULL, 623),
    NAME_FUNC_OFFSET(31137, glGenProgramsARB, glGenProgramsARB, NULL, 624),
    NAME_FUNC_OFFSET(31153, glIsProgramARB, glIsProgramARB, NULL, 631),
    NAME_FUNC_OFFSET(31167, glProgramEnvParameter4dARB, glProgramEnvParameter4dARB, NULL, 632),
    NAME_FUNC_OFFSET(31190, glProgramEnvParameter4dvARB, glProgramEnvParameter4dvARB, NULL, 633),
    NAME_FUNC_OFFSET(31214, glProgramEnvParameter4fARB, glProgramEnvParameter4fARB, NULL, 634),
    NAME_FUNC_OFFSET(31237, glProgramEnvParameter4fvARB, glProgramEnvParameter4fvARB, NULL, 635),
    NAME_FUNC_OFFSET(31261, glVertexAttrib1fARB, glVertexAttrib1fARB, NULL, 641),
    NAME_FUNC_OFFSET(31278, glVertexAttrib1fvARB, glVertexAttrib1fvARB, NULL, 642),
    NAME_FUNC_OFFSET(31296, glVertexAttrib2fARB, glVertexAttrib2fARB, NULL, 643),
    NAME_FUNC_OFFSET(31313, glVertexAttrib2fvARB, glVertexAttrib2fvARB, NULL, 644),
    NAME_FUNC_OFFSET(31331, glVertexAttrib3fARB, glVertexAttrib3fARB, NULL, 645),
    NAME_FUNC_OFFSET(31348, glVertexAttrib3fvARB, glVertexAttrib3fvARB, NULL, 646),
    NAME_FUNC_OFFSET(31366, glVertexAttrib4fARB, glVertexAttrib4fARB, NULL, 647),
    NAME_FUNC_OFFSET(31383, glVertexAttrib4fvARB, glVertexAttrib4fvARB, NULL, 648),
    NAME_FUNC_OFFSET(31401, glDrawArraysInstancedARB, glDrawArraysInstancedARB, NULL, 659),
    NAME_FUNC_OFFSET(31426, glDrawArraysInstancedARB, glDrawArraysInstancedARB, NULL, 659),
    NAME_FUNC_OFFSET(31448, glDrawElementsInstancedARB, glDrawElementsInstancedARB, NULL, 660),
    NAME_FUNC_OFFSET(31475, glDrawElementsInstancedARB, glDrawElementsInstancedARB, NULL, 660),
    NAME_FUNC_OFFSET(31499, glBindFramebuffer, glBindFramebuffer, NULL, 661),
    NAME_FUNC_OFFSET(31520, glBindRenderbuffer, glBindRenderbuffer, NULL, 662),
    NAME_FUNC_OFFSET(31542, glBlitFramebuffer, glBlitFramebuffer, NULL, 663),
    NAME_FUNC_OFFSET(31563, glCheckFramebufferStatus, glCheckFramebufferStatus, NULL, 664),
    NAME_FUNC_OFFSET(31591, glCheckFramebufferStatus, glCheckFramebufferStatus, NULL, 664),
    NAME_FUNC_OFFSET(31619, glDeleteFramebuffers, glDeleteFramebuffers, NULL, 665),
    NAME_FUNC_OFFSET(31643, glDeleteFramebuffers, glDeleteFramebuffers, NULL, 665),
    NAME_FUNC_OFFSET(31667, glDeleteRenderbuffers, glDeleteRenderbuffers, NULL, 666),
    NAME_FUNC_OFFSET(31692, glDeleteRenderbuffers, glDeleteRenderbuffers, NULL, 666),
    NAME_FUNC_OFFSET(31717, glFramebufferRenderbuffer, glFramebufferRenderbuffer, NULL, 667),
    NAME_FUNC_OFFSET(31746, glFramebufferRenderbuffer, glFramebufferRenderbuffer, NULL, 667),
    NAME_FUNC_OFFSET(31775, glFramebufferTexture1D, glFramebufferTexture1D, NULL, 668),
    NAME_FUNC_OFFSET(31801, glFramebufferTexture2D, glFramebufferTexture2D, NULL, 669),
    NAME_FUNC_OFFSET(31827, glFramebufferTexture2D, glFramebufferTexture2D, NULL, 669),
    NAME_FUNC_OFFSET(31853, glFramebufferTexture3D, glFramebufferTexture3D, NULL, 670),
    NAME_FUNC_OFFSET(31879, glFramebufferTexture3D, glFramebufferTexture3D, NULL, 670),
    NAME_FUNC_OFFSET(31905, glFramebufferTextureLayer, glFramebufferTextureLayer, NULL, 671),
    NAME_FUNC_OFFSET(31934, glFramebufferTextureLayer, glFramebufferTextureLayer, NULL, 671),
    NAME_FUNC_OFFSET(31963, glGenFramebuffers, glGenFramebuffers, NULL, 672),
    NAME_FUNC_OFFSET(31984, glGenFramebuffers, glGenFramebuffers, NULL, 672),
    NAME_FUNC_OFFSET(32005, glGenRenderbuffers, glGenRenderbuffers, NULL, 673),
    NAME_FUNC_OFFSET(32027, glGenRenderbuffers, glGenRenderbuffers, NULL, 673),
    NAME_FUNC_OFFSET(32049, glGenerateMipmap, glGenerateMipmap, NULL, 674),
    NAME_FUNC_OFFSET(32069, glGenerateMipmap, glGenerateMipmap, NULL, 674),
    NAME_FUNC_OFFSET(32089, glGetFramebufferAttachmentParameteriv, glGetFramebufferAttachmentParameteriv, NULL, 675),
    NAME_FUNC_OFFSET(32130, glGetFramebufferAttachmentParameteriv, glGetFramebufferAttachmentParameteriv, NULL, 675),
    NAME_FUNC_OFFSET(32171, glGetRenderbufferParameteriv, glGetRenderbufferParameteriv, NULL, 676),
    NAME_FUNC_OFFSET(32203, glGetRenderbufferParameteriv, glGetRenderbufferParameteriv, NULL, 676),
    NAME_FUNC_OFFSET(32235, glIsFramebuffer, glIsFramebuffer, NULL, 677),
    NAME_FUNC_OFFSET(32254, glIsFramebuffer, glIsFramebuffer, NULL, 677),
    NAME_FUNC_OFFSET(32273, glIsRenderbuffer, glIsRenderbuffer, NULL, 678),
    NAME_FUNC_OFFSET(32293, glIsRenderbuffer, glIsRenderbuffer, NULL, 678),
    NAME_FUNC_OFFSET(32313, glRenderbufferStorage, glRenderbufferStorage, NULL, 679),
    NAME_FUNC_OFFSET(32338, glRenderbufferStorage, glRenderbufferStorage, NULL, 679),
    NAME_FUNC_OFFSET(32363, glRenderbufferStorageMultisample, glRenderbufferStorageMultisample, NULL, 680),
    NAME_FUNC_OFFSET(32399, glFlushMappedBufferRange, glFlushMappedBufferRange, NULL, 681),
    NAME_FUNC_OFFSET(32427, glMapBufferRange, glMapBufferRange, NULL, 682),
    NAME_FUNC_OFFSET(32447, glBindVertexArray, glBindVertexArray, NULL, 683),
    NAME_FUNC_OFFSET(32468, glDeleteVertexArrays, glDeleteVertexArrays, NULL, 684),
    NAME_FUNC_OFFSET(32494, glDeleteVertexArrays, glDeleteVertexArrays, NULL, 684),
    NAME_FUNC_OFFSET(32518, glGenVertexArrays, glGenVertexArrays, NULL, 685),
    NAME_FUNC_OFFSET(32539, glIsVertexArray, glIsVertexArray, NULL, 686),
    NAME_FUNC_OFFSET(32560, glIsVertexArray, glIsVertexArray, NULL, 686),
    NAME_FUNC_OFFSET(32579, glProvokingVertex, glProvokingVertex, NULL, 706),
    NAME_FUNC_OFFSET(32600, glBlendEquationSeparateiARB, glBlendEquationSeparateiARB, NULL, 711),
    NAME_FUNC_OFFSET(32634, glBlendEquationSeparateiARB, glBlendEquationSeparateiARB, NULL, 711),
    NAME_FUNC_OFFSET(32659, glBlendEquationiARB, glBlendEquationiARB, NULL, 712),
    NAME_FUNC_OFFSET(32685, glBlendEquationiARB, glBlendEquationiARB, NULL, 712),
    NAME_FUNC_OFFSET(32702, glBlendFuncSeparateiARB, glBlendFuncSeparateiARB, NULL, 713),
    NAME_FUNC_OFFSET(32732, glBlendFuncSeparateiARB, glBlendFuncSeparateiARB, NULL, 713),
    NAME_FUNC_OFFSET(32753, glBlendFunciARB, glBlendFunciARB, NULL, 714),
    NAME_FUNC_OFFSET(32775, glBlendFunciARB, glBlendFunciARB, NULL, 714),
    NAME_FUNC_OFFSET(32788, gl_dispatch_stub_731, gl_dispatch_stub_731, NULL, 731),
    NAME_FUNC_OFFSET(32812, gl_dispatch_stub_732, gl_dispatch_stub_732, NULL, 732),
    NAME_FUNC_OFFSET(32837, glClearDepthf, glClearDepthf, NULL, 813),
    NAME_FUNC_OFFSET(32854, glDepthRangef, glDepthRangef, NULL, 814),
    NAME_FUNC_OFFSET(32871, glGetProgramBinary, glGetProgramBinary, NULL, 818),
    NAME_FUNC_OFFSET(32893, glProgramBinary, glProgramBinary, NULL, 819),
    NAME_FUNC_OFFSET(32912, glProgramParameteri, glProgramParameteri, NULL, 820),
    NAME_FUNC_OFFSET(32935, glProgramParameteri, glProgramParameteri, NULL, 820),
    NAME_FUNC_OFFSET(32958, gl_dispatch_stub_1027, gl_dispatch_stub_1027, NULL, 1027),
    NAME_FUNC_OFFSET(32974, gl_dispatch_stub_1028, gl_dispatch_stub_1028, NULL, 1028),
    NAME_FUNC_OFFSET(32993, gl_dispatch_stub_1036, gl_dispatch_stub_1036, NULL, 1036),
    NAME_FUNC_OFFSET(33018, gl_dispatch_stub_1037, gl_dispatch_stub_1037, NULL, 1037),
    NAME_FUNC_OFFSET(33043, gl_dispatch_stub_1038, gl_dispatch_stub_1038, NULL, 1038),
    NAME_FUNC_OFFSET(33069, gl_dispatch_stub_1039, gl_dispatch_stub_1039, NULL, 1039),
    NAME_FUNC_OFFSET(33097, gl_dispatch_stub_1040, gl_dispatch_stub_1040, NULL, 1040),
    NAME_FUNC_OFFSET(33122, gl_dispatch_stub_1041, gl_dispatch_stub_1041, NULL, 1041),
    NAME_FUNC_OFFSET(33153, gl_dispatch_stub_1042, gl_dispatch_stub_1042, NULL, 1042),
    NAME_FUNC_OFFSET(33179, gl_dispatch_stub_1043, gl_dispatch_stub_1043, NULL, 1043),
    NAME_FUNC_OFFSET(33202, gl_dispatch_stub_1047, gl_dispatch_stub_1047, NULL, 1047),
    NAME_FUNC_OFFSET(33224, gl_dispatch_stub_1048, gl_dispatch_stub_1048, NULL, 1048),
    NAME_FUNC_OFFSET(33247, gl_dispatch_stub_1049, gl_dispatch_stub_1049, NULL, 1049),
    NAME_FUNC_OFFSET(33269, gl_dispatch_stub_1050, gl_dispatch_stub_1050, NULL, 1050),
    NAME_FUNC_OFFSET(33292, gl_dispatch_stub_1051, gl_dispatch_stub_1051, NULL, 1051),
    NAME_FUNC_OFFSET(33315, gl_dispatch_stub_1052, gl_dispatch_stub_1052, NULL, 1052),
    NAME_FUNC_OFFSET(33339, gl_dispatch_stub_1055, gl_dispatch_stub_1055, NULL, 1055),
    NAME_FUNC_OFFSET(33361, gl_dispatch_stub_1056, gl_dispatch_stub_1056, NULL, 1056),
    NAME_FUNC_OFFSET(33384, gl_dispatch_stub_1057, gl_dispatch_stub_1057, NULL, 1057),
    NAME_FUNC_OFFSET(33406, gl_dispatch_stub_1058, gl_dispatch_stub_1058, NULL, 1058),
    NAME_FUNC_OFFSET(33429, gl_dispatch_stub_1059, gl_dispatch_stub_1059, NULL, 1059),
    NAME_FUNC_OFFSET(33452, gl_dispatch_stub_1060, gl_dispatch_stub_1060, NULL, 1060),
    NAME_FUNC_OFFSET(33476, gl_dispatch_stub_1063, gl_dispatch_stub_1063, NULL, 1063),
    NAME_FUNC_OFFSET(33498, gl_dispatch_stub_1064, gl_dispatch_stub_1064, NULL, 1064),
    NAME_FUNC_OFFSET(33521, gl_dispatch_stub_1065, gl_dispatch_stub_1065, NULL, 1065),
    NAME_FUNC_OFFSET(33543, gl_dispatch_stub_1066, gl_dispatch_stub_1066, NULL, 1066),
    NAME_FUNC_OFFSET(33566, gl_dispatch_stub_1067, gl_dispatch_stub_1067, NULL, 1067),
    NAME_FUNC_OFFSET(33589, gl_dispatch_stub_1068, gl_dispatch_stub_1068, NULL, 1068),
    NAME_FUNC_OFFSET(33613, gl_dispatch_stub_1071, gl_dispatch_stub_1071, NULL, 1071),
    NAME_FUNC_OFFSET(33635, gl_dispatch_stub_1072, gl_dispatch_stub_1072, NULL, 1072),
    NAME_FUNC_OFFSET(33658, gl_dispatch_stub_1073, gl_dispatch_stub_1073, NULL, 1073),
    NAME_FUNC_OFFSET(33680, gl_dispatch_stub_1074, gl_dispatch_stub_1074, NULL, 1074),
    NAME_FUNC_OFFSET(33703, gl_dispatch_stub_1075, gl_dispatch_stub_1075, NULL, 1075),
    NAME_FUNC_OFFSET(33726, gl_dispatch_stub_1076, gl_dispatch_stub_1076, NULL, 1076),
    NAME_FUNC_OFFSET(33750, gl_dispatch_stub_1078, gl_dispatch_stub_1078, NULL, 1078),
    NAME_FUNC_OFFSET(33779, gl_dispatch_stub_1080, gl_dispatch_stub_1080, NULL, 1080),
    NAME_FUNC_OFFSET(33810, gl_dispatch_stub_1082, gl_dispatch_stub_1082, NULL, 1082),
    NAME_FUNC_OFFSET(33841, gl_dispatch_stub_1084, gl_dispatch_stub_1084, NULL, 1084),
    NAME_FUNC_OFFSET(33870, gl_dispatch_stub_1086, gl_dispatch_stub_1086, NULL, 1086),
    NAME_FUNC_OFFSET(33901, gl_dispatch_stub_1088, gl_dispatch_stub_1088, NULL, 1088),
    NAME_FUNC_OFFSET(33932, gl_dispatch_stub_1090, gl_dispatch_stub_1090, NULL, 1090),
    NAME_FUNC_OFFSET(33961, gl_dispatch_stub_1092, gl_dispatch_stub_1092, NULL, 1092),
    NAME_FUNC_OFFSET(33992, gl_dispatch_stub_1094, gl_dispatch_stub_1094, NULL, 1094),
    NAME_FUNC_OFFSET(34023, gl_dispatch_stub_1096, gl_dispatch_stub_1096, NULL, 1096),
    NAME_FUNC_OFFSET(34045, gl_dispatch_stub_1097, gl_dispatch_stub_1097, NULL, 1097),
    NAME_FUNC_OFFSET(34074, glDebugMessageCallback, glDebugMessageCallback, NULL, 1098),
    NAME_FUNC_OFFSET(34100, glDebugMessageControl, glDebugMessageControl, NULL, 1099),
    NAME_FUNC_OFFSET(34125, glDebugMessageInsert, glDebugMessageInsert, NULL, 1100),
    NAME_FUNC_OFFSET(34149, glGetDebugMessageLog, glGetDebugMessageLog, NULL, 1101),
    NAME_FUNC_OFFSET(34173, glSecondaryColor3fEXT, glSecondaryColor3fEXT, NULL, 1108),
    NAME_FUNC_OFFSET(34192, glSecondaryColor3fvEXT, glSecondaryColor3fvEXT, NULL, 1109),
    NAME_FUNC_OFFSET(34212, glMultiDrawElementsEXT, glMultiDrawElementsEXT, NULL, 1110),
    NAME_FUNC_OFFSET(34232, glFogCoordfEXT, glFogCoordfEXT, NULL, 1111),
    NAME_FUNC_OFFSET(34244, glFogCoordfvEXT, glFogCoordfvEXT, NULL, 1112),
    NAME_FUNC_OFFSET(34257, glVertexAttribI1iEXT, glVertexAttribI1iEXT, NULL, 1215),
    NAME_FUNC_OFFSET(34275, glVertexAttribI1uiEXT, glVertexAttribI1uiEXT, NULL, 1216),
    NAME_FUNC_OFFSET(34294, glVertexAttribI2iEXT, glVertexAttribI2iEXT, NULL, 1217),
    NAME_FUNC_OFFSET(34312, glVertexAttribI2ivEXT, glVertexAttribI2ivEXT, NULL, 1218),
    NAME_FUNC_OFFSET(34331, glVertexAttribI2uiEXT, glVertexAttribI2uiEXT, NULL, 1219),
    NAME_FUNC_OFFSET(34350, glVertexAttribI2uivEXT, glVertexAttribI2uivEXT, NULL, 1220),
    NAME_FUNC_OFFSET(34370, glVertexAttribI3iEXT, glVertexAttribI3iEXT, NULL, 1221),
    NAME_FUNC_OFFSET(34388, glVertexAttribI3ivEXT, glVertexAttribI3ivEXT, NULL, 1222),
    NAME_FUNC_OFFSET(34407, glVertexAttribI3uiEXT, glVertexAttribI3uiEXT, NULL, 1223),
    NAME_FUNC_OFFSET(34426, glVertexAttribI3uivEXT, glVertexAttribI3uivEXT, NULL, 1224),
    NAME_FUNC_OFFSET(34446, glVertexAttribI4iEXT, glVertexAttribI4iEXT, NULL, 1225),
    NAME_FUNC_OFFSET(34464, glVertexAttribI4ivEXT, glVertexAttribI4ivEXT, NULL, 1226),
    NAME_FUNC_OFFSET(34483, glVertexAttribI4uiEXT, glVertexAttribI4uiEXT, NULL, 1227),
    NAME_FUNC_OFFSET(34502, glVertexAttribI4uivEXT, glVertexAttribI4uivEXT, NULL, 1228),
    NAME_FUNC_OFFSET(34522, glTextureBarrierNV, glTextureBarrierNV, NULL, 1249),
    NAME_FUNC_OFFSET(34539, glAlphaFuncx, glAlphaFuncx, NULL, 1276),
    NAME_FUNC_OFFSET(34555, glClearColorx, glClearColorx, NULL, 1277),
    NAME_FUNC_OFFSET(34572, glClearDepthx, glClearDepthx, NULL, 1278),
    NAME_FUNC_OFFSET(34589, glColor4x, glColor4x, NULL, 1279),
    NAME_FUNC_OFFSET(34602, glDepthRangex, glDepthRangex, NULL, 1280),
    NAME_FUNC_OFFSET(34619, glFogx, glFogx, NULL, 1281),
    NAME_FUNC_OFFSET(34629, glFogxv, glFogxv, NULL, 1282),
    NAME_FUNC_OFFSET(34640, glFrustumf, glFrustumf, NULL, 1283),
    NAME_FUNC_OFFSET(34654, glFrustumx, glFrustumx, NULL, 1284),
    NAME_FUNC_OFFSET(34668, glLightModelx, glLightModelx, NULL, 1285),
    NAME_FUNC_OFFSET(34685, glLightModelxv, glLightModelxv, NULL, 1286),
    NAME_FUNC_OFFSET(34703, glLightx, glLightx, NULL, 1287),
    NAME_FUNC_OFFSET(34715, glLightxv, glLightxv, NULL, 1288),
    NAME_FUNC_OFFSET(34728, glLineWidthx, glLineWidthx, NULL, 1289),
    NAME_FUNC_OFFSET(34744, glLoadMatrixx, glLoadMatrixx, NULL, 1290),
    NAME_FUNC_OFFSET(34761, glMaterialx, glMaterialx, NULL, 1291),
    NAME_FUNC_OFFSET(34776, glMaterialxv, glMaterialxv, NULL, 1292),
    NAME_FUNC_OFFSET(34792, glMultMatrixx, glMultMatrixx, NULL, 1293),
    NAME_FUNC_OFFSET(34809, glMultiTexCoord4x, glMultiTexCoord4x, NULL, 1294),
    NAME_FUNC_OFFSET(34830, glNormal3x, glNormal3x, NULL, 1295),
    NAME_FUNC_OFFSET(34844, glOrthof, glOrthof, NULL, 1296),
    NAME_FUNC_OFFSET(34856, glOrthox, glOrthox, NULL, 1297),
    NAME_FUNC_OFFSET(34868, glPointSizex, glPointSizex, NULL, 1298),
    NAME_FUNC_OFFSET(34884, glPolygonOffsetx, glPolygonOffsetx, NULL, 1299),
    NAME_FUNC_OFFSET(34904, glRotatex, glRotatex, NULL, 1300),
    NAME_FUNC_OFFSET(34917, glSampleCoveragex, glSampleCoveragex, NULL, 1301),
    NAME_FUNC_OFFSET(34938, glScalex, glScalex, NULL, 1302),
    NAME_FUNC_OFFSET(34950, glTexEnvx, glTexEnvx, NULL, 1303),
    NAME_FUNC_OFFSET(34963, glTexEnvxv, glTexEnvxv, NULL, 1304),
    NAME_FUNC_OFFSET(34977, glTexParameterx, glTexParameterx, NULL, 1305),
    NAME_FUNC_OFFSET(34996, glTranslatex, glTranslatex, NULL, 1306),
    NAME_FUNC_OFFSET(35012, glClipPlanef, glClipPlanef, NULL, 1307),
    NAME_FUNC_OFFSET(35028, glClipPlanex, glClipPlanex, NULL, 1308),
    NAME_FUNC_OFFSET(35044, glGetClipPlanef, glGetClipPlanef, NULL, 1309),
    NAME_FUNC_OFFSET(35063, glGetClipPlanex, glGetClipPlanex, NULL, 1310),
    NAME_FUNC_OFFSET(35082, glGetFixedv, glGetFixedv, NULL, 1311),
    NAME_FUNC_OFFSET(35097, glGetLightxv, glGetLightxv, NULL, 1312),
    NAME_FUNC_OFFSET(35113, glGetMaterialxv, glGetMaterialxv, NULL, 1313),
    NAME_FUNC_OFFSET(35132, glGetTexEnvxv, glGetTexEnvxv, NULL, 1314),
    NAME_FUNC_OFFSET(35149, glGetTexParameterxv, glGetTexParameterxv, NULL, 1315),
    NAME_FUNC_OFFSET(35172, glPointParameterx, glPointParameterx, NULL, 1316),
    NAME_FUNC_OFFSET(35193, glPointParameterxv, glPointParameterxv, NULL, 1317),
    NAME_FUNC_OFFSET(35215, glTexParameterxv, glTexParameterxv, NULL, 1318),
    NAME_FUNC_OFFSET(-1, NULL, NULL, NULL, 0)
};

#undef NAME_FUNC_OFFSET
