/* Copyright (C) 2014 Connor Abbott
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Connor Abbott (cwabbott0@gmail.com)
 */

#ifndef _NIR_OPCODES_
#define _NIR_OPCODES_



typedef enum {
   nir_op_b2f,
   nir_op_b2i,
   nir_op_ball2,
   nir_op_ball3,
   nir_op_ball4,
   nir_op_ball_fequal2,
   nir_op_ball_fequal3,
   nir_op_ball_fequal4,
   nir_op_ball_iequal2,
   nir_op_ball_iequal3,
   nir_op_ball_iequal4,
   nir_op_bany2,
   nir_op_bany3,
   nir_op_bany4,
   nir_op_bany_fnequal2,
   nir_op_bany_fnequal3,
   nir_op_bany_fnequal4,
   nir_op_bany_inequal2,
   nir_op_bany_inequal3,
   nir_op_bany_inequal4,
   nir_op_bcsel,
   nir_op_bfi,
   nir_op_bfm,
   nir_op_bit_count,
   nir_op_bitfield_insert,
   nir_op_bitfield_reverse,
   nir_op_f2b,
   nir_op_f2i,
   nir_op_f2u,
   nir_op_fabs,
   nir_op_fadd,
   nir_op_fall2,
   nir_op_fall3,
   nir_op_fall4,
   nir_op_fall_equal2,
   nir_op_fall_equal3,
   nir_op_fall_equal4,
   nir_op_fand,
   nir_op_fany2,
   nir_op_fany3,
   nir_op_fany4,
   nir_op_fany_nequal2,
   nir_op_fany_nequal3,
   nir_op_fany_nequal4,
   nir_op_fceil,
   nir_op_fcos,
   nir_op_fcsel,
   nir_op_fddx,
   nir_op_fddx_coarse,
   nir_op_fddx_fine,
   nir_op_fddy,
   nir_op_fddy_coarse,
   nir_op_fddy_fine,
   nir_op_fdiv,
   nir_op_fdot2,
   nir_op_fdot3,
   nir_op_fdot4,
   nir_op_feq,
   nir_op_fexp2,
   nir_op_ffloor,
   nir_op_ffma,
   nir_op_ffract,
   nir_op_fge,
   nir_op_find_lsb,
   nir_op_flog2,
   nir_op_flrp,
   nir_op_flt,
   nir_op_fmax,
   nir_op_fmin,
   nir_op_fmod,
   nir_op_fmov,
   nir_op_fmul,
   nir_op_fne,
   nir_op_fneg,
   nir_op_fnoise1_1,
   nir_op_fnoise1_2,
   nir_op_fnoise1_3,
   nir_op_fnoise1_4,
   nir_op_fnoise2_1,
   nir_op_fnoise2_2,
   nir_op_fnoise2_3,
   nir_op_fnoise2_4,
   nir_op_fnoise3_1,
   nir_op_fnoise3_2,
   nir_op_fnoise3_3,
   nir_op_fnoise3_4,
   nir_op_fnoise4_1,
   nir_op_fnoise4_2,
   nir_op_fnoise4_3,
   nir_op_fnoise4_4,
   nir_op_fnot,
   nir_op_for,
   nir_op_fpow,
   nir_op_frcp,
   nir_op_fround_even,
   nir_op_frsq,
   nir_op_fsat,
   nir_op_fsign,
   nir_op_fsin,
   nir_op_fsqrt,
   nir_op_fsub,
   nir_op_ftrunc,
   nir_op_fxor,
   nir_op_i2b,
   nir_op_i2f,
   nir_op_iabs,
   nir_op_iadd,
   nir_op_iand,
   nir_op_ibitfield_extract,
   nir_op_idiv,
   nir_op_ieq,
   nir_op_ifind_msb,
   nir_op_ige,
   nir_op_ilt,
   nir_op_imax,
   nir_op_imin,
   nir_op_imov,
   nir_op_imul,
   nir_op_imul_high,
   nir_op_ine,
   nir_op_ineg,
   nir_op_inot,
   nir_op_ior,
   nir_op_ishl,
   nir_op_ishr,
   nir_op_isign,
   nir_op_isub,
   nir_op_ixor,
   nir_op_ldexp,
   nir_op_pack_half_2x16,
   nir_op_pack_half_2x16_split,
   nir_op_pack_snorm_2x16,
   nir_op_pack_snorm_4x8,
   nir_op_pack_unorm_2x16,
   nir_op_pack_unorm_4x8,
   nir_op_seq,
   nir_op_sge,
   nir_op_slt,
   nir_op_sne,
   nir_op_u2f,
   nir_op_uadd_carry,
   nir_op_ubitfield_extract,
   nir_op_udiv,
   nir_op_ufind_msb,
   nir_op_uge,
   nir_op_ult,
   nir_op_umax,
   nir_op_umin,
   nir_op_umod,
   nir_op_umul_high,
   nir_op_unpack_half_2x16,
   nir_op_unpack_half_2x16_split_x,
   nir_op_unpack_half_2x16_split_y,
   nir_op_unpack_snorm_2x16,
   nir_op_unpack_snorm_4x8,
   nir_op_unpack_unorm_2x16,
   nir_op_unpack_unorm_4x8,
   nir_op_ushr,
   nir_op_usub_borrow,
   nir_op_vec2,
   nir_op_vec3,
   nir_op_vec4,
   nir_last_opcode = nir_op_vec4,
   nir_num_opcodes = nir_last_opcode + 1
} nir_op;

#endif /* _NIR_OPCODES_ */
