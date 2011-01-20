/* command line interpretation, image loading etc. for Gforth


  Copyright (C) 1995,1996,1997,1998,2000,2003,2004,2005,2006,2007,2008,2009 Free Software Foundation, Inc.

  This file is part of Gforth.

  Gforth is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation, either version 3
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see http://www.gnu.org/licenses/.
*/

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef PERF
#	include "perf.h"
#endif

#define ARRAY_LEN( ARR ) (sizeof( ARR )/sizeof( ARR[0] ))

#define USE_TOS
#define USE_FTOS
#define HAS_FILE
#define HAS_FLOATING
#define HAS_OS
#define HAS_DEBUG
#define HAS_GLOCALS
#define HAS_HASH
#define HAS_PEEPHOLE
#define HAS_DCOMPS
#define HAS_XCONDS
#define HAS_STANDARDTHREADING

typedef enum prim_num {
/* definitions of N_execute etc. */
N_paren_docol,
N_paren_docon,
N_paren_dovar,
N_paren_douser,
N_paren_dodefer,
N_paren_field,
N_paren_doval,
N_paren_dodoes,
N_paren_doabicode,
N_paren_do_semicolon_abi_code,
N_noop,
N_call,
N_execute,
N_perform,
N_semis,
N_unloop,
N_lit_perform,
N_does_exec,
#ifdef HAS_GLOCALS
N_branch_lp_plus_store_number,
#endif
N_branch,
N_question_branch,
#ifdef HAS_GLOCALS
N_question_branch_lp_plus_store_number,
#endif
#ifdef HAS_XCONDS
N_question_dupe_question_branch,
N_question_dupe_zero_equals_question_branch,
#endif
N_paren_next,
#ifdef HAS_GLOCALS
N_paren_next_lp_plus_store_number,
#endif
N_paren_loop,
#ifdef HAS_GLOCALS
N_paren_loop_lp_plus_store_number,
#endif
N_paren_plus_loop,
#ifdef HAS_GLOCALS
N_paren_plus_loop_lp_plus_store_number,
#endif
#ifdef HAS_XCONDS
N_paren_minus_loop,
#ifdef HAS_GLOCALS
N_paren_minus_loop_lp_plus_store_number,
#endif
N_paren_symmetric_plus_loop,
#ifdef HAS_GLOCALS
N_paren_symmetric_plus_loop_lp_plus_store_number,
#endif
#endif
N_paren_for,
N_paren_do,
N_paren_question_do,
#ifdef HAS_XCONDS
N_paren_plus_do,
N_paren_u_plus_do,
N_paren_minus_do,
N_paren_u_minus_do,
N_paren_try1,
N_paren_throw1,
#endif
N_i,
N_i_tick,
N_j,
N_k,
N_move,
N_c_move,
N_c_move_up,
N_fill,
N_compare,
N_toupper,
N_capscompare,
N_slash_string,
N_lit,
N_plus,
N_lit_plus,
N_under_plus,
N_minus,
N_negate,
N_one_plus,
N_one_minus,
N_max,
N_min,
N_abs,
N_star,
N_slash,
N_mod,
N_slash_mod,
N_star_slash_mod,
N_star_slash,
N_two_star,
N_two_slash,
N_f_m_slash_mod,
N_s_m_slash_rem,
N_m_star,
N_u_m_star,
N_u_m_slash_mod,
N_m_plus,
N_d_plus,
N_d_minus,
N_d_negate,
N_d_two_star,
N_d_two_slash,
N_and,
N_or,
N_x_or,
N_invert,
N_r_shift,
N_l_shift,
N_zero_equals,
N_zero_not_equals,
N_zero_less_than,
N_zero_greater_than,
N_zero_less_or_equal,
N_zero_greater_or_equal,
N_equals,
N_not_equals,
N_less_than,
N_greater_than,
N_less_or_equal,
N_greater_or_equal,
N_u_equals,
N_u_not_equals,
N_u_less_than,
N_u_greater_than,
N_u_less_or_equal,
N_u_greater_or_equal,
#ifdef HAS_DCOMPS
N_d_equals,
N_d_not_equals,
N_d_less_than,
N_d_greater_than,
N_d_less_or_equal,
N_d_greater_or_equal,
N_d_zero_equals,
N_d_zero_not_equals,
N_d_zero_less_than,
N_d_zero_greater_than,
N_d_zero_less_or_equal,
N_d_zero_greater_or_equal,
N_d_u_equals,
N_d_u_not_equals,
N_d_u_less_than,
N_d_u_greater_than,
N_d_u_less_or_equal,
N_d_u_greater_or_equal,
#endif
N_within,
N_useraddr,
N_up_store,
N_sp_fetch,
N_sp_store,
N_rp_fetch,
N_rp_store,
#ifdef HAS_FLOATING
N_fp_fetch,
N_fp_store,
#endif
N_to_r,
N_r_from,
N_rdrop,
N_two_to_r,
N_two_r_from,
N_two_r_fetch,
N_two_r_drop,
N_over,
N_drop,
N_swap,
N_dupe,
N_rote,
N_not_rote,
N_nip,
N_tuck,
N_question_dupe,
N_pick,
N_two_drop,
N_two_dupe,
N_two_over,
N_two_swap,
N_two_rote,
N_two_nip,
N_two_tuck,
N_fetch,
N_lit_fetch,
N_store,
N_plus_store,
N_c_fetch,
N_c_store,
N_two_store,
N_two_fetch,
N_cell_plus,
N_cells,
N_char_plus,
N_paren_chars,
N_count,
#ifdef HAS_F83HEADERSTRING
N_paren_f83find,
#else /* 174 */
N_paren_listlfind,
#ifdef HAS_HASH
N_paren_hashlfind,
N_paren_tablelfind,
N_paren_hashkey1,
#endif
#endif
N_paren_parse_white,
N_aligned,
N_f_aligned,
N_threading_method,
N_paren_key_file,
N_key_q_file,
N_stdin,
N_stdout,
N_stderr,
#ifdef HAS_OS
N_form,
N_wcwidth,
N_flush_icache,
N_paren_bye,
N_paren_system,
N_getenv,
N_open_pipe,
N_close_pipe,
N_time_and_date,
N_ms,
N_allocate,
N_free,
N_resize,
N_strerror,
N_strsignal,
N_call_c,
#endif
#ifdef HAS_FILE
N_close_file,
N_open_file,
N_create_file,
N_delete_file,
N_rename_file,
N_file_position,
N_reposition_file,
N_file_size,
N_resize_file,
N_read_file,
N_paren_read_line,
#endif
N_write_file,
N_emit_file,
#ifdef HAS_FILE
N_flush_file,
N_file_status,
N_file_eof_query,
N_open_dir,
N_read_dir,
N_close_dir,
N_match_file,
N_set_dir,
N_get_dir,
N_equals_mkdir,
#endif
N_newline,
#ifdef HAS_OS
N_utime,
N_cputime,
#endif
#ifdef HAS_FLOATING
N_f_equals,
N_f_not_equals,
N_f_less_than,
N_f_greater_than,
N_f_less_or_equal,
N_f_greater_or_equal,
N_f_zero_equals,
N_f_zero_not_equals,
N_f_zero_less_than,
N_f_zero_greater_than,
N_f_zero_less_or_equal,
N_f_zero_greater_or_equal,
N_s_to_f,
N_d_to_f,
N_f_to_d,
N_f_to_s,
N_f_store,
N_f_fetch,
N_d_f_fetch,
N_d_f_store,
N_s_f_fetch,
N_s_f_store,
N_f_plus,
N_f_minus,
N_f_star,
N_f_slash,
N_f_star_star,
N_fm_star,
N_fm_slash,
N_fm_star_slash,
N_fm_square,
N_f_negate,
N_f_drop,
N_f_dupe,
N_f_swap,
N_f_over,
N_f_rote,
N_f_nip,
N_f_tuck,
N_float_plus,
N_floats,
N_floor,
N_f_round,
N_f_max,
N_f_min,
N_represent,
N_to_float,
N_f_abs,
N_f_a_cos,
N_f_a_sine,
N_f_a_tan,
N_f_a_tan_two,
N_f_cos,
N_f_e_x_p,
N_f_e_x_p_m_one,
N_f_l_n,
N_f_l_n_p_one,
N_f_log,
N_f_a_log,
N_f_sine,
N_f_sine_cos,
N_f_square_root,
N_f_tan,
N_f_cinch,
N_f_cosh,
N_f_tan_h,
N_f_a_cinch,
N_f_a_cosh,
N_f_a_tan_h,
N_s_floats,
N_d_floats,
N_s_f_aligned,
N_d_f_aligned,
N_v_star,
N_faxpy,
#endif
#ifdef HAS_GLOCALS
N_fetch_local_number,
N_fetch_local_zero,
N_fetch_local_four,
N_fetch_local_eight,
N_fetch_local_twelve,
#ifdef HAS_FLOATING
N_f_fetch_local_number,
N_f_fetch_local_zero,
N_f_fetch_local_eight,
#endif
N_laddr_number,
N_lp_plus_store_number,
N_minus_four_lp_plus_store,
N_eight_lp_plus_store,
N_sixteen_lp_plus_store,
N_lp_store,
N_to_l,
#ifdef HAS_FLOATING
N_f_to_l,
N_fpick,
#endif
#endif
#ifdef HAS_OS
N_open_lib,
N_lib_sym,
N_wcall,
N_u_w_fetch,
N_s_w_fetch,
N_w_store,
N_u_l_fetch,
N_s_l_fetch,
N_l_store,
N_lib_error,
N_w_store_be,
N_l_store_be,
N_w_store_le,
N_l_store_le,
N_w_fetch_be,
N_l_fetch_be,
N_w_fetch_le,
N_l_fetch_le,
#ifdef HAS_64BIT
N_x_store,
N_u_x_fetch,
N_s_x_fetch,
N_b_e_x_store,
N_l_e_x_store,
N_b_e_u_x_fetch,
N_l_e_u_x_fetch,
#endif
#endif
#ifdef HAS_PEEPHOLE
N_compile_prim1,
N_finish_code,
N_forget_dyncode,
N_decompile_prim,
N_set_next_code,
N_call2,
N_tag_offsets,
#endif
N_abi_call,
N_semi_abi_code_exec,
N_lit_execute,
  N_START_SUPER
} PrimNum;

static int no_dynamic=0; /* if true, no code is generated
					     dynamically */
static int static_super_number = 10000; /* number of ss used if available */
#define MAX_STATE 9 /* maximum number of states */
static int maxstates = MAX_STATE; /* number of states for stack caching */

FILE *output;

#define MAX_IMMARGS 2

typedef void *Label;
typedef long Cell;
typedef unsigned long UCell;

typedef struct {
  Label start; /* NULL if not relocatable */
  Cell length; /* only includes the jump iff superend is true*/
  Cell restlength; /* length of the rest (i.e., the jump or (on superend) 0) */
  char superend; /* true if primitive ends superinstruction, i.e.,
                     unconditional branch, execute, etc. */
  Cell nimmargs;
  struct immarg {
    Cell offset; /* offset of immarg within prim */
    char rel;    /* true if immarg is relative */
  } immargs[MAX_IMMARGS];
} PrimInfo;

PrimInfo priminfos[]={
  {(Label)0x100047bc, 8, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100047d4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100047f4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004814, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004838, 8, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000484c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004868, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004888, 24, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100048b0, 60, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100048fc, 64, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000494c, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004960, 16, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004980, 16, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000499c, 12, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100049b4, 12, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100049d0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100049e8, 16, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004a04, 32, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004a34, 24, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004a5c, 8, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004a74, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004ab8, 64, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004b08, 40, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004b40, 36, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004b74, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004bbc, 68, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004c10, 60, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004c5c, 72, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004cb4, 88, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004d1c, 100, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004d90, 76, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004dec, 88, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004e54, 100, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004ec8, 116, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004f4c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004f74, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004f9c, 52, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10004fe0, 52, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005024, 52, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005068, 52, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100050ac, 52, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100050f0, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005138, 56, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000517c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000519c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100051bc, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100051dc, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100051fc, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005224, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000524c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005274, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000529c, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100052d0, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100052f8, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000532c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005354, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005378, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005394, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100053b0, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100053d8, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100053f4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000540c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005424, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000543c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005464, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000548c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100054ac, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100054c8, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005504, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005540, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005588, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100055bc, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100055f4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000560c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005624, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005650, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000567c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100056a4, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100056cc, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100056f8, 64, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005748, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005790, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100057d8, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005810, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000583c, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005878, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005894, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100058b0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100058cc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100058e4, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005900, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000591c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005938, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005958, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005970, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005990, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100059b0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100059cc, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100059f0, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005a18, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005a40, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005a68, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005a94, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005ac0, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005ae4, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005b0c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005b2c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005b4c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005b70, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005b94, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005bd8, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005c1c, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005c64, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005cac, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005cf4, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005d3c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005d64, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005d90, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005dac, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005de4, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005e1c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005e3c, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005e80, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005ec4, 60, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005f10, 60, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005f5c, 60, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005fa8, 60, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10005ff4, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006024, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006048, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006070, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006094, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100060b4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100060d4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100060f4, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000611c, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006148, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006164, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000618c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100061a4, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100061cc, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100061fc, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006228, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006240, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006264, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000627c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000629c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100062b8, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100062e0, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006308, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006320, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006344, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006378, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006398, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100063b0, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100063d4, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006400, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006430, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006470, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006490, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100064c8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100064e0, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006504, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006524, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000654c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006564, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006584, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100065ac, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100065d4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100065ec, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006604, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000661c, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006630, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006658, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006688, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100066b8, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100066e8, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006718, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000674c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006768, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006784, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100067a4, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100067d4, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000680c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006834, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000685c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006884, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100068c0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100068e0, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006904, 8, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000691c, 84, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006980, 60, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100069cc, 100, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006a40, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006a84, 84, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006ae8, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006b08, 64, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006b58, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006b78, 80, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006bd8, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006c08, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006c38, 148, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006cdc, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006d14, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006d50, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006d8c, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006dd0, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006e04, 72, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006e5c, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006ea4, 68, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006ef8, 64, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006f48, 100, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10006fbc, 60, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007008, 80, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007068, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100070b0, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100070e8, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000711c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000713c, 64, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000718c, 120, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007214, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000724c, 72, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100072a4, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100072e8, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000732c, 60, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007378, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100073a8, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100073e4, 68, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007438, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007474, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100074b4, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100074f0, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000752c, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000756c, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100075ac, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100075ec, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007630, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007670, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100076b0, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100076f4, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007738, 60, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007784, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100077bc, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100077f4, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007828, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007850, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007880, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100078b0, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100078d8, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007908, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007934, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007958, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000797c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100079a0, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100079c4, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100079f0, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007a2c, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007a68, 72, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007ac0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007ad8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007af0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007b10, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007b34, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007b58, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007b84, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007bb4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007bd4, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007c04, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007c1c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007c34, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007c54, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007c74, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007ca8, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007cd8, 256, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007de8, 64, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007e38, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007e50, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007e70, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007e90, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007eb0, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007edc, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007efc, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007f1c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007f3c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007f5c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007f7c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007f9c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007fbc, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10007fdc, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008020, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008040, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008060, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008080, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100080a0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100080c0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100080e0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008100, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008120, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008138, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008150, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000816c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008188, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100081cc, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000820c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008234, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008258, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000827c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100082a0, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100082c4, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100082f4, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008320, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000834c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008374, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008398, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100083b8, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100083d8, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100083f8, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008418, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000843c, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008468, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000849c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100084c0, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100084fc, 72, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008554, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000856c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008584, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100085a4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100085bc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100085d4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100085f4, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008634, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000865c, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008694, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100086bc, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100086f4, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008718, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008754, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008778, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100087b4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100087d4, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008804, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008824, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008844, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008858, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008888, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100088ac, 68, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008900, 72, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008958, 16, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008974, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008990, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100089b4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100089d4, 24, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100089fc, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008a18, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008a3c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008a58, 24, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008a80, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008aa0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008ac0, 20, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008ae4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008b04, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008b24, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008b40, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008b5c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008b78, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008b94, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008bb0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008bcc, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008be8, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008c04, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008c44, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008c5c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008c74, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008c8c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008ca4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008cbc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008cd4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008cec, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008d20, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008d60, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008da4, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008dec, 60, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008e38, 64, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008e88, 68, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008edc, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008ef8, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008f14, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008f30, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008f4c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008f68, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008f84, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008fa0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008fbc, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10008ffc, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000901c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000903c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000905c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000907c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000909c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100090bc, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100090dc, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009104, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000911c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000913c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000915c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009174, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000918c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100091a4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100091bc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100091d4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100091ec, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009204, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009224, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009244, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009264, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009284, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100092a4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100092c4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100092e4, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009300, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000931c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009338, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009354, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009370, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000938c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100093a8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100093c0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100093d8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100093f0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009408, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009420, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009438, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009450, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009488, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100094a0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100094b8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100094d0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100094e8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009500, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009518, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009530, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009548, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009560, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009578, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009590, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100095a8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100095c0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100095d8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100095f0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009608, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009644, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009664, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009684, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100096a4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100096c4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100096e4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009704, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009724, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000973c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009754, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000976c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009784, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000979c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100097b4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100097cc, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100097e8, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009804, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009820, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000983c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009858, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009874, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009890, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100098ac, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100098ec, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009904, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000991c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009934, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000994c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009964, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000997c, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100099bc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100099d4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x100099ec, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009a0c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009a30, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009a48, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009a60, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009a78, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009a90, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009aa8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009ac0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009ad8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009af0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009b08, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009b20, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009b38, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009b50, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009b68, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009b80, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009ba4, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009bc8, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009bec, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009c10, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009c34, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009c58, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009c78, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009c90, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009cc0, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009cf0, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009d14, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009d3c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009d54, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009d6c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009d84, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009d9c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009db4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009dcc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009de4, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009e00, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009e1c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009e38, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009e54, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009e70, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009eb4, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009eec, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009f14, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009f30, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009f4c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009f6c, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009f98, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009fc0, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x10009ff0, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a014, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a038, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a05c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a080, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a0a4, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a0c8, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a0ec, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a104, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a11c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a134, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a14c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a164, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a17c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a194, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a1b0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a1cc, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a1e8, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a204, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a220, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a23c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a258, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a26c, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a280, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a294, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a2a8, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a2bc, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a2d0, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a2e4, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a2f8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a310, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a328, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a340, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a358, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a370, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a388, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a3a0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a3b8, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a3f8, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a438, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a478, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a4b8, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a4f8, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a538, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a578, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a590, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a5a8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a5c0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a5d8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a5f0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a608, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a620, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a638, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a658, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a678, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a698, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a6b8, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a6d8, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a6f8, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a718, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a738, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a77c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a794, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a7ac, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a7c4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a7dc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a7f4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a80c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a824, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a848, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a86c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a890, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a8b4, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a8d8, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a8fc, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a920, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a944, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a95c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a974, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a98c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a9a4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a9bc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a9d4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000a9ec, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aa04, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aa1c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aa34, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aa4c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aa64, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aa7c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aa94, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aaac, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aacc, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aaec, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ab0c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ab2c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ab4c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ab6c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ab8c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000abac, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000abd4, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000abfc, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ac24, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ac4c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ac74, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ac9c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000acc4, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000acec, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ad14, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ad3c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ad64, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ad8c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000adb4, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ade8, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ae04, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ae24, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ae40, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ae5c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ae78, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ae94, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aeb0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aecc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aee4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000aefc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000af14, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000af2c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000af44, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000af5c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000af74, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000afa4, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000afdc, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b01c, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b060, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b0a8, 60, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b0f4, 64, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b144, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b160, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b17c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b198, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b1b4, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b1d0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b1ec, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b208, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b224, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b264, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b28c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b2b4, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b2dc, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b304, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b32c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b354, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b37c, 52, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b3c0, 44, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b3fc, 56, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b444, 60, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b490, 64, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b4e0, 68, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b534, 72, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b58c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b5ac, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b5cc, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b5ec, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b60c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b62c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b64c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b66c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b694, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b6bc, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b6e4, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b70c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b734, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b75c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b784, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b7cc, 60, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b818, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b83c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b860, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b884, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b8a8, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b8cc, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b8f0, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b914, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b958, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b9a0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b9c0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000b9e0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ba00, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ba20, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ba40, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ba60, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ba80, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000babc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bad4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000baec, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bb04, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bb1c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bb34, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bb4c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bb64, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bb84, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bb9c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bbc0, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bbe4, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bc08, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bc2c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bc50, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bc74, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bc8c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bca4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bcbc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bcd4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bcec, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bd04, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bd1c, 8, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bd30, 20, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bd50, 24, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bd74, 28, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bd9c, 32, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bdc8, 36, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bdf8, 40, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000be2c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000be4c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000be6c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000be8c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000beac, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000becc, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000beec, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bf0c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bf2c, 8, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bf40, 20, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bf60, 24, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bf84, 28, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bfac, 32, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000bfd8, 36, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c008, 40, 0, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c03c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c054, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c06c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c084, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c09c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c0b4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c0cc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c0e4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c104, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c124, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c144, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c164, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c184, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c1a4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c1c4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c1e4, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c228, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c244, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c260, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c27c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c298, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c2b4, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c2d0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c2ec, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c314, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c33c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c364, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c38c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c3b4, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c3dc, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c3f8, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c420, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c440, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c468, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c498, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c4ac, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c4c0, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c4d4, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c4e8, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c4fc, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c510, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c524, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c53c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c554, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c56c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c584, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c59c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c5b4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c5cc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c5e4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c5fc, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c618, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c634, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c650, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c66c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c688, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c6a4, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c6c0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c6dc, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c71c, 76, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c778, 88, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c7e0, 92, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c84c, 96, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c8bc, 100, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c930, 104, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000c9a8, 108, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ca24, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ca3c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ca54, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ca6c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ca84, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ca9c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cab4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cacc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cae4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cafc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cb14, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cb2c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cb44, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cb5c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cb74, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cb8c, 52, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cbd0, 44, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cc0c, 56, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cc54, 60, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cca0, 64, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ccf0, 68, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cd44, 72, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cd9c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cdc4, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cdec, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ce14, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ce3c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ce64, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ce8c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ceb4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cecc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cee4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cefc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cf14, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cf2c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cf44, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cf5c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cf78, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cf94, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cfb0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cfcc, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000cfe8, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d004, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d028, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d048, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d074, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d0a4, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d0d8, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d110, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d14c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d168, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d184, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d1a0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d1bc, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d1d8, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d1f4, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d210, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d230, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d250, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d270, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d290, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d2b0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d2d0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d2f0, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d32c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d344, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d35c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d374, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d38c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d3a4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d3bc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d3d4, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d3f8, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d41c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d440, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d464, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d488, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d4ac, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d4d0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d4ec, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d508, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d524, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d540, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d55c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d578, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d594, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d5b0, 48, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d5f0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d610, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d630, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d650, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d670, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d690, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d6b0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d6d0, 52, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d714, 44, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d750, 56, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d798, 60, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d7e4, 64, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d834, 68, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d888, 72, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d8e0, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d904, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d928, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d94c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d970, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d994, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d9b8, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000d9dc, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000da18, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000da54, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000da90, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dacc, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000db08, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000db20, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000db38, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000db50, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000db68, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000db80, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000db98, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dbb0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dbd0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dbf0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dc10, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dc30, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dc50, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dc70, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dc90, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dcac, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dcc8, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dce4, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dd00, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dd1c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dd38, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dd54, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dd74, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dd94, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ddb4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ddd4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ddf4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000de14, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000de34, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000de4c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000de64, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000de7c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000de94, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000deac, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dec4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dedc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000def4, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000df30, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000df54, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000df78, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000df9c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dfc0, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000dfe4, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e008, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e02c, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e058, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e084, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e0b0, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e0dc, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e108, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e144, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e178, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e1a0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e1c0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e1e0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e200, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e220, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e240, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e260, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e280, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e2a0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e2c0, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e304, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e32c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e354, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e37c, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e3a4, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e3cc, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e3f4, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e41c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e434, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e44c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e464, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e47c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e494, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e4ac, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e4c4, 52, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e508, 44, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e544, 56, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e58c, 60, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e5d8, 64, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e628, 68, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e67c, 72, 4, 1, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e6d4, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e6f0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e70c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e728, 52, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e76c, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e7b4, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e7d0, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e7ec, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e808, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e824, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e84c, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e878, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e8a8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e8c0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e8d8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e8f0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e908, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e920, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e938, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e96c, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e9a0, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000e9d4, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ea08, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ea3c, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ea70, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000eaa4, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000eac8, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000eaec, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000eb10, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000eb34, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000eb58, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000eb7c, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000eba0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ebc0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ebe0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ec00, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ec20, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ec40, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ec60, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ec80, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ec98, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ecb0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ecc8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ece0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ecf8, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ed10, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ed28, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ed40, 44, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ed7c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ed94, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000edac, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000edc4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000eddc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000edf4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ee0c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ee24, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ee3c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ee54, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ee6c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ee84, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ee9c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000eeb4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000eecc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000eee4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ef04, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ef24, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ef44, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ef64, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000ef84, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000efa4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000efc4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000efe4, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f01c, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f054, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f08c, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f0c4, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f0fc, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f134, 40, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f16c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f184, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f19c, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f1b4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f1cc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f1e4, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f1fc, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f214, 56, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f25c, 68, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f2b0, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f2c8, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f2e4, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f304, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f328, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f350, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f37c, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f3ac, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f3e0, 4, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f3f4, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f410, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f430, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f454, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f47c, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f4a8, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f4d8, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f50c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f528, 8, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f540, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f560, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f584, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f5ac, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f5d8, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f608, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f63c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f65c, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f67c, 12, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f698, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f6bc, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f6e4, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f710, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f740, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f774, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f798, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f7bc, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f7e0, 16, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f800, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f828, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f854, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f884, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f8b8, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f8e0, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f908, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f930, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f958, 20, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f97c, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f9a8, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000f9d8, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fa0c, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fa38, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fa64, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fa90, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fabc, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fae8, 24, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fb10, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fb40, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fb74, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fba4, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fbd4, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fc04, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fc34, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fc64, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fc94, 28, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fcc0, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fcf4, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fd28, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fd5c, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fd90, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fdc4, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fdf8, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fe2c, 36, 4, 0, 0, {{0, 0}, {0, 0}}},
  {(Label)0x1000fe60, 32, 4, 0, 0, {{0, 0}, {0, 0}}},
};

const char const* const prim_names[]={
"(docol)",
"(docon)",
"(dovar)",
"(douser)",
"(dodefer)",
"(dofield)",
"(dovalue)",
"(dodoes)",
"(doabicode)",
"(do;abicode)",
"noop",
"call",
"execute",
"perform",
";s",
"unloop",
"lit-perform",
"does-exec",
#ifdef HAS_GLOCALS
"branch-lp+!#",
#endif
"branch",
"?branch",
#ifdef HAS_GLOCALS
"?branch-lp+!#",
#endif
#ifdef HAS_XCONDS
"?dup-?branch",
"?dup-0=-?branch",
#endif
"(next)",
#ifdef HAS_GLOCALS
"(next)-lp+!#",
#endif
"(loop)",
#ifdef HAS_GLOCALS
"(loop)-lp+!#",
#endif
"(+loop)",
#ifdef HAS_GLOCALS
"(+loop)-lp+!#",
#endif
#ifdef HAS_XCONDS
"(-loop)",
#ifdef HAS_GLOCALS
"(-loop)-lp+!#",
#endif
"(s+loop)",
#ifdef HAS_GLOCALS
"(s+loop)-lp+!#",
#endif
#endif
"(for)",
"(do)",
"(?do)",
#ifdef HAS_XCONDS
"(+do)",
"(u+do)",
"(-do)",
"(u-do)",
"(try1)",
"(throw1)",
#endif
"i",
"i'",
"j",
"k",
"move",
"cmove",
"cmove>",
"fill",
"compare",
"toupper",
"capscompare",
"/string",
"lit",
"+",
"lit+",
"under+",
"-",
"negate",
"1+",
"1-",
"max",
"min",
"abs",
"*",
"/",
"mod",
"/mod",
"*/mod",
"*/",
"2*",
"2/",
"fm/mod",
"sm/rem",
"m*",
"um*",
"um/mod",
"m+",
"d+",
"d-",
"dnegate",
"d2*",
"d2/",
"and",
"or",
"xor",
"invert",
"rshift",
"lshift",
"0=",
"0<>",
"0<",
"0>",
"0<=",
"0>=",
"=",
"<>",
"<",
">",
"<=",
">=",
"u=",
"u<>",
"u<",
"u>",
"u<=",
"u>=",
#ifdef HAS_DCOMPS
"d=",
"d<>",
"d<",
"d>",
"d<=",
"d>=",
"d0=",
"d0<>",
"d0<",
"d0>",
"d0<=",
"d0>=",
"du=",
"du<>",
"du<",
"du>",
"du<=",
"du>=",
#endif
"within",
"useraddr",
"up!",
"sp@",
"sp!",
"rp@",
"rp!",
#ifdef HAS_FLOATING
"fp@",
"fp!",
#endif
">r",
"r>",
"rdrop",
"2>r",
"2r>",
"2r@",
"2rdrop",
"over",
"drop",
"swap",
"dup",
"rot",
"-rot",
"nip",
"tuck",
"?dup",
"pick",
"2drop",
"2dup",
"2over",
"2swap",
"2rot",
"2nip",
"2tuck",
"@",
"lit@",
"!",
"+!",
"c@",
"c!",
"2!",
"2@",
"cell+",
"cells",
"char+",
"(chars)",
"count",
#ifdef HAS_F83HEADERSTRING
"(f83find)",
#else /* 174 */
"(listlfind)",
#ifdef HAS_HASH
"(hashlfind)",
"(tablelfind)",
"(hashkey1)",
#endif
#endif
"(parse-white)",
"aligned",
"faligned",
"threading-method",
"key-file",
"key?-file",
"stdin",
"stdout",
"stderr",
#ifdef HAS_OS
"form",
"wcwidth",
"flush-icache",
"(bye)",
"(system)",
"getenv",
"open-pipe",
"close-pipe",
"time&date",
"ms",
"allocate",
"free",
"resize",
"strerror",
"strsignal",
"call-c",
#endif
#ifdef HAS_FILE
"close-file",
"open-file",
"create-file",
"delete-file",
"rename-file",
"file-position",
"reposition-file",
"file-size",
"resize-file",
"read-file",
"(read-line)",
#endif
"write-file",
"emit-file",
#ifdef HAS_FILE
"flush-file",
"file-status",
"file-eof?",
"open-dir",
"read-dir",
"close-dir",
"filename-match",
"set-dir",
"get-dir",
"=mkdir",
#endif
"newline",
#ifdef HAS_OS
"utime",
"cputime",
#endif
#ifdef HAS_FLOATING
"f=",
"f<>",
"f<",
"f>",
"f<=",
"f>=",
"f0=",
"f0<>",
"f0<",
"f0>",
"f0<=",
"f0>=",
"s>f",
"d>f",
"f>d",
"f>s",
"f!",
"f@",
"df@",
"df!",
"sf@",
"sf!",
"f+",
"f-",
"f*",
"f/",
"f**",
"fm*",
"fm/",
"fm*/",
"f**2",
"fnegate",
"fdrop",
"fdup",
"fswap",
"fover",
"frot",
"fnip",
"ftuck",
"float+",
"floats",
"floor",
"fround",
"fmax",
"fmin",
"represent",
">float",
"fabs",
"facos",
"fasin",
"fatan",
"fatan2",
"fcos",
"fexp",
"fexpm1",
"fln",
"flnp1",
"flog",
"falog",
"fsin",
"fsincos",
"fsqrt",
"ftan",
"fsinh",
"fcosh",
"ftanh",
"fasinh",
"facosh",
"fatanh",
"sfloats",
"dfloats",
"sfaligned",
"dfaligned",
"v*",
"faxpy",
#endif
#ifdef HAS_GLOCALS
"@local#",
"@local0",
"@local1",
"@local2",
"@local3",
#ifdef HAS_FLOATING
"f@local#",
"f@local0",
"f@local1",
#endif
"laddr#",
"lp+!#",
"lp-",
"lp+",
"lp+2",
"lp!",
">l",
#ifdef HAS_FLOATING
"f>l",
"fpick",
#endif
#endif
#ifdef HAS_OS
"open-lib",
"lib-sym",
"wcall",
"uw@",
"sw@",
"w!",
"ul@",
"sl@",
"l!",
"lib-error",
"be-w!",
"be-l!",
"le-w!",
"le-l!",
"be-uw@",
"be-ul@",
"le-uw@",
"le-ul@",
#ifdef HAS_64BIT
"x!",
"ux@",
"sx@",
"be-x!",
"le-x!",
"be-ux@",
"le-ux@",
#endif
#endif
#ifdef HAS_PEEPHOLE
"compile-prim1",
"finish-code",
"forget-dyncode",
"decompile-prim",
"set-next-code",
"call2",
"tag-offsets",
#endif
"abi-call",
";abi-code-exec",
"lit-execute",
};

static int is_relocatable(int p)
{
  return !no_dynamic && priminfos[p].start != NULL;
}

/* static superinstruction stuff */

struct cost { /* super_info might be a more accurate name */
  char loads;       /* number of stack loads */
  char stores;      /* number of stack stores */
  char updates;     /* number of stack pointer updates */
  char branch;	    /* is it a branch (SET_IP) */
  unsigned char state_in;    /* state on entry */
  unsigned char state_out;   /* state on exit */
  unsigned char imm_ops;     /* number of immediate operands */
  short offset;     /* offset into super2 table */
  unsigned char length;      /* number of components */
};

PrimNum super2[] = {
N_paren_docol, /* (docol) */
N_paren_docon, /* (docon) */
N_paren_dovar, /* (dovar) */
N_paren_douser, /* (douser) */
N_paren_dodefer, /* (dodefer) */
N_paren_field, /* (dofield) */
N_paren_doval, /* (dovalue) */
N_paren_dodoes, /* (dodoes) */
N_paren_doabicode, /* (doabicode) */
N_paren_do_semicolon_abi_code, /* (do;abicode) */
N_noop, /* noop */
N_call, /* call */
N_execute, /* execute */
N_perform, /* perform */
N_semis, /* ;s */
N_unloop, /* unloop */
N_lit_perform, /* lit-perform */
N_does_exec, /* does-exec */
#ifdef HAS_GLOCALS
N_branch_lp_plus_store_number, /* branch-lp+!# */
#endif
N_branch, /* branch */
N_question_branch, /* ?branch */
#ifdef HAS_GLOCALS
N_question_branch_lp_plus_store_number, /* ?branch-lp+!# */
#endif
#ifdef HAS_XCONDS
N_question_dupe_question_branch, /* ?dup-?branch */
N_question_dupe_zero_equals_question_branch, /* ?dup-0=-?branch */
#endif
N_paren_next, /* (next) */
#ifdef HAS_GLOCALS
N_paren_next_lp_plus_store_number, /* (next)-lp+!# */
#endif
N_paren_loop, /* (loop) */
#ifdef HAS_GLOCALS
N_paren_loop_lp_plus_store_number, /* (loop)-lp+!# */
#endif
N_paren_plus_loop, /* (+loop) */
#ifdef HAS_GLOCALS
N_paren_plus_loop_lp_plus_store_number, /* (+loop)-lp+!# */
#endif
#ifdef HAS_XCONDS
N_paren_minus_loop, /* (-loop) */
#ifdef HAS_GLOCALS
N_paren_minus_loop_lp_plus_store_number, /* (-loop)-lp+!# */
#endif
N_paren_symmetric_plus_loop, /* (s+loop) */
#ifdef HAS_GLOCALS
N_paren_symmetric_plus_loop_lp_plus_store_number, /* (s+loop)-lp+!# */
#endif
#endif
N_paren_for, /* (for) */
N_paren_do, /* (do) */
N_paren_question_do, /* (?do) */
#ifdef HAS_XCONDS
N_paren_plus_do, /* (+do) */
N_paren_u_plus_do, /* (u+do) */
N_paren_minus_do, /* (-do) */
N_paren_u_minus_do, /* (u-do) */
N_paren_try1, /* (try1) */
N_paren_throw1, /* (throw1) */
#endif
N_i, /* i */
N_i_tick, /* i' */
N_j, /* j */
N_k, /* k */
N_move, /* move */
N_c_move, /* cmove */
N_c_move_up, /* cmove> */
N_fill, /* fill */
N_compare, /* compare */
N_toupper, /* toupper */
N_capscompare, /* capscompare */
N_slash_string, /* /string */
N_lit, /* lit */
N_plus, /* + */
N_lit_plus, /* lit+ */
N_under_plus, /* under+ */
N_minus, /* - */
N_negate, /* negate */
N_one_plus, /* 1+ */
N_one_minus, /* 1- */
N_max, /* max */
N_min, /* min */
N_abs, /* abs */
N_star, /* * */
N_slash, /* / */
N_mod, /* mod */
N_slash_mod, /* /mod */
N_star_slash_mod, /* x/mod */
N_star_slash, /* x/ */
N_two_star, /* 2* */
N_two_slash, /* 2/ */
N_f_m_slash_mod, /* fm/mod */
N_s_m_slash_rem, /* sm/rem */
N_m_star, /* m* */
N_u_m_star, /* um* */
N_u_m_slash_mod, /* um/mod */
N_m_plus, /* m+ */
N_d_plus, /* d+ */
N_d_minus, /* d- */
N_d_negate, /* dnegate */
N_d_two_star, /* d2* */
N_d_two_slash, /* d2/ */
N_and, /* and */
N_or, /* or */
N_x_or, /* xor */
N_invert, /* invert */
N_r_shift, /* rshift */
N_l_shift, /* lshift */
N_zero_equals, /* 0= */
N_zero_not_equals, /* 0<> */
N_zero_less_than, /* 0< */
N_zero_greater_than, /* 0> */
N_zero_less_or_equal, /* 0<= */
N_zero_greater_or_equal, /* 0>= */
N_equals, /* = */
N_not_equals, /* <> */
N_less_than, /* < */
N_greater_than, /* > */
N_less_or_equal, /* <= */
N_greater_or_equal, /* >= */
N_u_equals, /* u= */
N_u_not_equals, /* u<> */
N_u_less_than, /* u< */
N_u_greater_than, /* u> */
N_u_less_or_equal, /* u<= */
N_u_greater_or_equal, /* u>= */
#ifdef HAS_DCOMPS
N_d_equals, /* d= */
N_d_not_equals, /* d<> */
N_d_less_than, /* d< */
N_d_greater_than, /* d> */
N_d_less_or_equal, /* d<= */
N_d_greater_or_equal, /* d>= */
N_d_zero_equals, /* d0= */
N_d_zero_not_equals, /* d0<> */
N_d_zero_less_than, /* d0< */
N_d_zero_greater_than, /* d0> */
N_d_zero_less_or_equal, /* d0<= */
N_d_zero_greater_or_equal, /* d0>= */
N_d_u_equals, /* du= */
N_d_u_not_equals, /* du<> */
N_d_u_less_than, /* du< */
N_d_u_greater_than, /* du> */
N_d_u_less_or_equal, /* du<= */
N_d_u_greater_or_equal, /* du>= */
#endif
N_within, /* within */
N_useraddr, /* useraddr */
N_up_store, /* up! */
N_sp_fetch, /* sp@ */
N_sp_store, /* sp! */
N_rp_fetch, /* rp@ */
N_rp_store, /* rp! */
#ifdef HAS_FLOATING
N_fp_fetch, /* fp@ */
N_fp_store, /* fp! */
#endif
N_to_r, /* >r */
N_r_from, /* r> */
N_rdrop, /* rdrop */
N_two_to_r, /* 2>r */
N_two_r_from, /* 2r> */
N_two_r_fetch, /* 2r@ */
N_two_r_drop, /* 2rdrop */
N_over, /* over */
N_drop, /* drop */
N_swap, /* swap */
N_dupe, /* dup */
N_rote, /* rot */
N_not_rote, /* -rot */
N_nip, /* nip */
N_tuck, /* tuck */
N_question_dupe, /* ?dup */
N_pick, /* pick */
N_two_drop, /* 2drop */
N_two_dupe, /* 2dup */
N_two_over, /* 2over */
N_two_swap, /* 2swap */
N_two_rote, /* 2rot */
N_two_nip, /* 2nip */
N_two_tuck, /* 2tuck */
N_fetch, /* @ */
N_lit_fetch, /* lit@ */
N_store, /* ! */
N_plus_store, /* +! */
N_c_fetch, /* c@ */
N_c_store, /* c! */
N_two_store, /* 2! */
N_two_fetch, /* 2@ */
N_cell_plus, /* cell+ */
N_cells, /* cells */
N_char_plus, /* char+ */
N_paren_chars, /* (chars) */
N_count, /* count */
#ifdef HAS_F83HEADERSTRING
N_paren_f83find, /* (f83find) */
#else /* 174 */
N_paren_listlfind, /* (listlfind) */
#ifdef HAS_HASH
N_paren_hashlfind, /* (hashlfind) */
N_paren_tablelfind, /* (tablelfind) */
N_paren_hashkey1, /* (hashkey1) */
#endif
#endif
N_paren_parse_white, /* (parse-white) */
N_aligned, /* aligned */
N_f_aligned, /* faligned */
N_threading_method, /* threading-method */
N_paren_key_file, /* key-file */
N_key_q_file, /* key?-file */
N_stdin, /* stdin */
N_stdout, /* stdout */
N_stderr, /* stderr */
#ifdef HAS_OS
N_form, /* form */
N_wcwidth, /* wcwidth */
N_flush_icache, /* flush-icache */
N_paren_bye, /* (bye) */
N_paren_system, /* (system) */
N_getenv, /* getenv */
N_open_pipe, /* open-pipe */
N_close_pipe, /* close-pipe */
N_time_and_date, /* time&date */
N_ms, /* ms */
N_allocate, /* allocate */
N_free, /* free */
N_resize, /* resize */
N_strerror, /* strerror */
N_strsignal, /* strsignal */
N_call_c, /* call-c */
#endif
#ifdef HAS_FILE
N_close_file, /* close-file */
N_open_file, /* open-file */
N_create_file, /* create-file */
N_delete_file, /* delete-file */
N_rename_file, /* rename-file */
N_file_position, /* file-position */
N_reposition_file, /* reposition-file */
N_file_size, /* file-size */
N_resize_file, /* resize-file */
N_read_file, /* read-file */
N_paren_read_line, /* (read-line) */
#endif
N_write_file, /* write-file */
N_emit_file, /* emit-file */
#ifdef HAS_FILE
N_flush_file, /* flush-file */
N_file_status, /* file-status */
N_file_eof_query, /* file-eof? */
N_open_dir, /* open-dir */
N_read_dir, /* read-dir */
N_close_dir, /* close-dir */
N_match_file, /* filename-match */
N_set_dir, /* set-dir */
N_get_dir, /* get-dir */
N_equals_mkdir, /* =mkdir */
#endif
N_newline, /* newline */
#ifdef HAS_OS
N_utime, /* utime */
N_cputime, /* cputime */
#endif
#ifdef HAS_FLOATING
N_f_equals, /* f= */
N_f_not_equals, /* f<> */
N_f_less_than, /* f< */
N_f_greater_than, /* f> */
N_f_less_or_equal, /* f<= */
N_f_greater_or_equal, /* f>= */
N_f_zero_equals, /* f0= */
N_f_zero_not_equals, /* f0<> */
N_f_zero_less_than, /* f0< */
N_f_zero_greater_than, /* f0> */
N_f_zero_less_or_equal, /* f0<= */
N_f_zero_greater_or_equal, /* f0>= */
N_s_to_f, /* s>f */
N_d_to_f, /* d>f */
N_f_to_d, /* f>d */
N_f_to_s, /* f>s */
N_f_store, /* f! */
N_f_fetch, /* f@ */
N_d_f_fetch, /* df@ */
N_d_f_store, /* df! */
N_s_f_fetch, /* sf@ */
N_s_f_store, /* sf! */
N_f_plus, /* f+ */
N_f_minus, /* f- */
N_f_star, /* f* */
N_f_slash, /* f/ */
N_f_star_star, /* f** */
N_fm_star, /* fm* */
N_fm_slash, /* fm/ */
N_fm_star_slash, /* fmx/ */
N_fm_square, /* f**2 */
N_f_negate, /* fnegate */
N_f_drop, /* fdrop */
N_f_dupe, /* fdup */
N_f_swap, /* fswap */
N_f_over, /* fover */
N_f_rote, /* frot */
N_f_nip, /* fnip */
N_f_tuck, /* ftuck */
N_float_plus, /* float+ */
N_floats, /* floats */
N_floor, /* floor */
N_f_round, /* fround */
N_f_max, /* fmax */
N_f_min, /* fmin */
N_represent, /* represent */
N_to_float, /* >float */
N_f_abs, /* fabs */
N_f_a_cos, /* facos */
N_f_a_sine, /* fasin */
N_f_a_tan, /* fatan */
N_f_a_tan_two, /* fatan2 */
N_f_cos, /* fcos */
N_f_e_x_p, /* fexp */
N_f_e_x_p_m_one, /* fexpm1 */
N_f_l_n, /* fln */
N_f_l_n_p_one, /* flnp1 */
N_f_log, /* flog */
N_f_a_log, /* falog */
N_f_sine, /* fsin */
N_f_sine_cos, /* fsincos */
N_f_square_root, /* fsqrt */
N_f_tan, /* ftan */
N_f_cinch, /* fsinh */
N_f_cosh, /* fcosh */
N_f_tan_h, /* ftanh */
N_f_a_cinch, /* fasinh */
N_f_a_cosh, /* facosh */
N_f_a_tan_h, /* fatanh */
N_s_floats, /* sfloats */
N_d_floats, /* dfloats */
N_s_f_aligned, /* sfaligned */
N_d_f_aligned, /* dfaligned */
N_v_star, /* v* */
N_faxpy, /* faxpy */
#endif
#ifdef HAS_GLOCALS
N_fetch_local_number, /* @local# */
N_fetch_local_zero, /* @local0 */
N_fetch_local_four, /* @local1 */
N_fetch_local_eight, /* @local2 */
N_fetch_local_twelve, /* @local3 */
#ifdef HAS_FLOATING
N_f_fetch_local_number, /* f@local# */
N_f_fetch_local_zero, /* f@local0 */
N_f_fetch_local_eight, /* f@local1 */
#endif
N_laddr_number, /* laddr# */
N_lp_plus_store_number, /* lp+!# */
N_minus_four_lp_plus_store, /* lp- */
N_eight_lp_plus_store, /* lp+ */
N_sixteen_lp_plus_store, /* lp+2 */
N_lp_store, /* lp! */
N_to_l, /* >l */
#ifdef HAS_FLOATING
N_f_to_l, /* f>l */
N_fpick, /* fpick */
#endif
#endif
#ifdef HAS_OS
N_open_lib, /* open-lib */
N_lib_sym, /* lib-sym */
N_wcall, /* wcall */
N_u_w_fetch, /* uw@ */
N_s_w_fetch, /* sw@ */
N_w_store, /* w! */
N_u_l_fetch, /* ul@ */
N_s_l_fetch, /* sl@ */
N_l_store, /* l! */
N_lib_error, /* lib-error */
N_w_store_be, /* be-w! */
N_l_store_be, /* be-l! */
N_w_store_le, /* le-w! */
N_l_store_le, /* le-l! */
N_w_fetch_be, /* be-uw@ */
N_l_fetch_be, /* be-ul@ */
N_w_fetch_le, /* le-uw@ */
N_l_fetch_le, /* le-ul@ */
#ifdef HAS_64BIT
N_x_store, /* x! */
N_u_x_fetch, /* ux@ */
N_s_x_fetch, /* sx@ */
N_b_e_x_store, /* be-x! */
N_l_e_x_store, /* le-x! */
N_b_e_u_x_fetch, /* be-ux@ */
N_l_e_u_x_fetch, /* le-ux@ */
#endif
#endif
#ifdef HAS_PEEPHOLE
N_compile_prim1, /* compile-prim1 */
N_finish_code, /* finish-code */
N_forget_dyncode, /* forget-dyncode */
N_decompile_prim, /* decompile-prim */
N_set_next_code, /* set-next-code */
N_call2, /* call2 */
N_tag_offsets, /* tag-offsets */
#endif
N_abi_call, /* abi-call */
N_semi_abi_code_exec, /* ;abi-code-exec */
N_lit_execute, /* lit-execute */
N_lit,N_plus, /* super1 */
N_lit,N_fetch, /* super3 */
N_lit,N_store, /* super6 */
N_store,N_semis, /* super11 */
N_lit,N_plus,N_fetch, /* super12 */
N_useraddr,N_fetch, /* super15 */
N_plus,N_fetch, /* super16 */
N_lit,N_store,N_semis, /* super18 */
N_lit,N_fetch,N_and, /* super19 */
N_useraddr,N_store, /* super23 */
N_fetch,N_semis, /* super25 */
N_lit,N_fetch,N_plus, /* super26 */
N_dupe,N_fetch, /* super27 */
};

struct cost super_costs[] = {
{ 0, 1, 1, 1 , 0, 0,0,N_paren_docol, 1}, /* (docol) */
{ 0, 1, 1, 0 , 0, 0,0,N_paren_docon, 1}, /* (docon) */
{ 0, 1, 1, 0 , 0, 0,0,N_paren_dovar, 1}, /* (dovar) */
{ 0, 1, 1, 0 , 0, 0,0,N_paren_douser, 1}, /* (douser) */
{ 0, 0, 0, 0 , 0, 0,0,N_paren_dodefer, 1}, /* (dodefer) */
{ 1, 1, 0, 0 , 0, 0,0,N_paren_field, 1}, /* (dofield) */
{ 0, 1, 1, 0 , 0, 0,0,N_paren_doval, 1}, /* (dovalue) */
{ 0, 2, 2, 1 , 0, 0,0,N_paren_dodoes, 1}, /* (dodoes) */
{ 0, 0, 0, 0 , 0, 0,0,N_paren_doabicode, 1}, /* (doabicode) */
{ 0, 0, 0, 0 , 0, 0,0,N_paren_do_semicolon_abi_code, 1}, /* (do;abicode) */
{ 0, 0, 0, 0 , 0, 0,0,N_noop, 1}, /* noop */
{ 1, 1, 1, 1 , 0, 0,1,N_call, 1}, /* call */
{ 1, 0, 1, 0 , 0, 0,0,N_execute, 1}, /* execute */
{ 1, 0, 1, 0 , 0, 0,0,N_perform, 1}, /* perform */
{ 1, 0, 1, 1 , 0, 0,0,N_semis, 1}, /* ;s */
{ 2, 0, 1, 0 , 0, 0,0,N_unloop, 1}, /* unloop */
{ 1, 0, 0, 0 , 0, 0,1,N_lit_perform, 1}, /* lit-perform */
{ 1, 2, 2, 1 , 0, 0,1,N_does_exec, 1}, /* does-exec */
#ifdef HAS_GLOCALS
{ 2, 0, 0, 1 , 0, 0,2,N_branch_lp_plus_store_number, 1}, /* branch-lp+!# */
#endif
{ 1, 0, 0, 1 , 0, 0,1,N_branch, 1}, /* branch */
{ 2, 0, 1, 1 , 0, 0,1,N_question_branch, 1}, /* ?branch */
#ifdef HAS_GLOCALS
{ 3, 0, 1, 1 , 0, 0,2,N_question_branch_lp_plus_store_number, 1}, /* ?branch-lp+!# */
#endif
#ifdef HAS_XCONDS
{ 2, 0, 1, 1 , 0, 0,1,N_question_dupe_question_branch, 1}, /* ?dup-?branch */
{ 2, 0, 1, 1 , 0, 0,1,N_question_dupe_zero_equals_question_branch, 1}, /* ?dup-0=-?branch */
#endif
{ 2, 1, 0, 1 , 0, 0,1,N_paren_next, 1}, /* (next) */
#ifdef HAS_GLOCALS
{ 3, 1, 0, 1 , 0, 0,2,N_paren_next_lp_plus_store_number, 1}, /* (next)-lp+!# */
#endif
{ 3, 2, 0, 1 , 0, 0,1,N_paren_loop, 1}, /* (loop) */
#ifdef HAS_GLOCALS
{ 4, 2, 0, 1 , 0, 0,2,N_paren_loop_lp_plus_store_number, 1}, /* (loop)-lp+!# */
#endif
{ 4, 2, 1, 1 , 0, 0,1,N_paren_plus_loop, 1}, /* (+loop) */
#ifdef HAS_GLOCALS
{ 5, 2, 1, 1 , 0, 0,2,N_paren_plus_loop_lp_plus_store_number, 1}, /* (+loop)-lp+!# */
#endif
#ifdef HAS_XCONDS
{ 4, 2, 1, 1 , 0, 0,1,N_paren_minus_loop, 1}, /* (-loop) */
#ifdef HAS_GLOCALS
{ 5, 2, 1, 1 , 0, 0,2,N_paren_minus_loop_lp_plus_store_number, 1}, /* (-loop)-lp+!# */
#endif
{ 4, 2, 1, 1 , 0, 0,1,N_paren_symmetric_plus_loop, 1}, /* (s+loop) */
#ifdef HAS_GLOCALS
{ 5, 2, 1, 1 , 0, 0,2,N_paren_symmetric_plus_loop_lp_plus_store_number, 1}, /* (s+loop)-lp+!# */
#endif
#endif
{ 1, 2, 2, 0 , 0, 0,0,N_paren_for, 1}, /* (for) */
{ 2, 2, 2, 0 , 0, 0,0,N_paren_do, 1}, /* (do) */
{ 3, 2, 2, 1 , 0, 0,1,N_paren_question_do, 1}, /* (?do) */
#ifdef HAS_XCONDS
{ 3, 2, 2, 1 , 0, 0,1,N_paren_plus_do, 1}, /* (+do) */
{ 3, 2, 2, 1 , 0, 0,1,N_paren_u_plus_do, 1}, /* (u+do) */
{ 3, 2, 2, 1 , 0, 0,1,N_paren_minus_do, 1}, /* (-do) */
{ 3, 2, 2, 1 , 0, 0,1,N_paren_u_minus_do, 1}, /* (u-do) */
{ 2, 6, 2, 0 , 0, 0,0,N_paren_try1, 1}, /* (try1) */
{ 2, 1, 1, 0 , 0, 0,0,N_paren_throw1, 1}, /* (throw1) */
#endif
{ 1, 2, 1, 0 , 0, 0,0,N_i, 1}, /* i */
{ 2, 3, 1, 0 , 0, 0,0,N_i_tick, 1}, /* i' */
{ 3, 4, 1, 0 , 0, 0,0,N_j, 1}, /* j */
{ 5, 6, 1, 0 , 0, 0,0,N_k, 1}, /* k */
{ 3, 0, 1, 0 , 0, 0,0,N_move, 1}, /* move */
{ 3, 0, 1, 0 , 0, 0,0,N_c_move, 1}, /* cmove */
{ 3, 0, 1, 0 , 0, 0,0,N_c_move_up, 1}, /* cmove> */
{ 3, 0, 1, 0 , 0, 0,0,N_fill, 1}, /* fill */
{ 4, 1, 1, 0 , 0, 0,0,N_compare, 1}, /* compare */
{ 1, 1, 0, 0 , 0, 0,0,N_toupper, 1}, /* toupper */
{ 4, 1, 1, 0 , 0, 0,0,N_capscompare, 1}, /* capscompare */
{ 3, 2, 1, 0 , 0, 0,0,N_slash_string, 1}, /* /string */
{ 1, 1, 1, 0 , 0, 0,1,N_lit, 1}, /* lit */
{ 2, 1, 1, 0 , 0, 0,0,N_plus, 1}, /* + */
{ 2, 1, 0, 0 , 0, 0,1,N_lit_plus, 1}, /* lit+ */
{ 3, 2, 1, 0 , 0, 0,0,N_under_plus, 1}, /* under+ */
{ 2, 1, 1, 0 , 0, 0,0,N_minus, 1}, /* - */
{ 1, 1, 0, 0 , 0, 0,0,N_negate, 1}, /* negate */
{ 1, 1, 0, 0 , 0, 0,0,N_one_plus, 1}, /* 1+ */
{ 1, 1, 0, 0 , 0, 0,0,N_one_minus, 1}, /* 1- */
{ 2, 1, 1, 0 , 0, 0,0,N_max, 1}, /* max */
{ 2, 1, 1, 0 , 0, 0,0,N_min, 1}, /* min */
{ 1, 1, 0, 0 , 0, 0,0,N_abs, 1}, /* abs */
{ 2, 1, 1, 0 , 0, 0,0,N_star, 1}, /* * */
{ 2, 1, 1, 0 , 0, 0,0,N_slash, 1}, /* / */
{ 2, 1, 1, 0 , 0, 0,0,N_mod, 1}, /* mod */
{ 2, 2, 0, 0 , 0, 0,0,N_slash_mod, 1}, /* /mod */
{ 3, 2, 1, 0 , 0, 0,0,N_star_slash_mod, 1}, /* x/mod */
{ 3, 1, 1, 0 , 0, 0,0,N_star_slash, 1}, /* x/ */
{ 1, 1, 0, 0 , 0, 0,0,N_two_star, 1}, /* 2* */
{ 1, 1, 0, 0 , 0, 0,0,N_two_slash, 1}, /* 2/ */
{ 3, 2, 1, 0 , 0, 0,0,N_f_m_slash_mod, 1}, /* fm/mod */
{ 3, 2, 1, 0 , 0, 0,0,N_s_m_slash_rem, 1}, /* sm/rem */
{ 2, 2, 0, 0 , 0, 0,0,N_m_star, 1}, /* m* */
{ 2, 2, 0, 0 , 0, 0,0,N_u_m_star, 1}, /* um* */
{ 3, 2, 1, 0 , 0, 0,0,N_u_m_slash_mod, 1}, /* um/mod */
{ 3, 2, 1, 0 , 0, 0,0,N_m_plus, 1}, /* m+ */
{ 4, 2, 1, 0 , 0, 0,0,N_d_plus, 1}, /* d+ */
{ 4, 2, 1, 0 , 0, 0,0,N_d_minus, 1}, /* d- */
{ 2, 2, 0, 0 , 0, 0,0,N_d_negate, 1}, /* dnegate */
{ 2, 2, 0, 0 , 0, 0,0,N_d_two_star, 1}, /* d2* */
{ 2, 2, 0, 0 , 0, 0,0,N_d_two_slash, 1}, /* d2/ */
{ 2, 1, 1, 0 , 0, 0,0,N_and, 1}, /* and */
{ 2, 1, 1, 0 , 0, 0,0,N_or, 1}, /* or */
{ 2, 1, 1, 0 , 0, 0,0,N_x_or, 1}, /* xor */
{ 1, 1, 0, 0 , 0, 0,0,N_invert, 1}, /* invert */
{ 2, 1, 1, 0 , 0, 0,0,N_r_shift, 1}, /* rshift */
{ 2, 1, 1, 0 , 0, 0,0,N_l_shift, 1}, /* lshift */
{ 1, 1, 0, 0 , 0, 0,0,N_zero_equals, 1}, /* 0= */
{ 1, 1, 0, 0 , 0, 0,0,N_zero_not_equals, 1}, /* 0<> */
{ 1, 1, 0, 0 , 0, 0,0,N_zero_less_than, 1}, /* 0< */
{ 1, 1, 0, 0 , 0, 0,0,N_zero_greater_than, 1}, /* 0> */
{ 1, 1, 0, 0 , 0, 0,0,N_zero_less_or_equal, 1}, /* 0<= */
{ 1, 1, 0, 0 , 0, 0,0,N_zero_greater_or_equal, 1}, /* 0>= */
{ 2, 1, 1, 0 , 0, 0,0,N_equals, 1}, /* = */
{ 2, 1, 1, 0 , 0, 0,0,N_not_equals, 1}, /* <> */
{ 2, 1, 1, 0 , 0, 0,0,N_less_than, 1}, /* < */
{ 2, 1, 1, 0 , 0, 0,0,N_greater_than, 1}, /* > */
{ 2, 1, 1, 0 , 0, 0,0,N_less_or_equal, 1}, /* <= */
{ 2, 1, 1, 0 , 0, 0,0,N_greater_or_equal, 1}, /* >= */
{ 2, 1, 1, 0 , 0, 0,0,N_u_equals, 1}, /* u= */
{ 2, 1, 1, 0 , 0, 0,0,N_u_not_equals, 1}, /* u<> */
{ 2, 1, 1, 0 , 0, 0,0,N_u_less_than, 1}, /* u< */
{ 2, 1, 1, 0 , 0, 0,0,N_u_greater_than, 1}, /* u> */
{ 2, 1, 1, 0 , 0, 0,0,N_u_less_or_equal, 1}, /* u<= */
{ 2, 1, 1, 0 , 0, 0,0,N_u_greater_or_equal, 1}, /* u>= */
#ifdef HAS_DCOMPS
{ 4, 1, 1, 0 , 0, 0,0,N_d_equals, 1}, /* d= */
{ 4, 1, 1, 0 , 0, 0,0,N_d_not_equals, 1}, /* d<> */
{ 4, 1, 1, 0 , 0, 0,0,N_d_less_than, 1}, /* d< */
{ 4, 1, 1, 0 , 0, 0,0,N_d_greater_than, 1}, /* d> */
{ 4, 1, 1, 0 , 0, 0,0,N_d_less_or_equal, 1}, /* d<= */
{ 4, 1, 1, 0 , 0, 0,0,N_d_greater_or_equal, 1}, /* d>= */
{ 2, 1, 1, 0 , 0, 0,0,N_d_zero_equals, 1}, /* d0= */
{ 2, 1, 1, 0 , 0, 0,0,N_d_zero_not_equals, 1}, /* d0<> */
{ 2, 1, 1, 0 , 0, 0,0,N_d_zero_less_than, 1}, /* d0< */
{ 2, 1, 1, 0 , 0, 0,0,N_d_zero_greater_than, 1}, /* d0> */
{ 2, 1, 1, 0 , 0, 0,0,N_d_zero_less_or_equal, 1}, /* d0<= */
{ 2, 1, 1, 0 , 0, 0,0,N_d_zero_greater_or_equal, 1}, /* d0>= */
{ 4, 1, 1, 0 , 0, 0,0,N_d_u_equals, 1}, /* du= */
{ 4, 1, 1, 0 , 0, 0,0,N_d_u_not_equals, 1}, /* du<> */
{ 4, 1, 1, 0 , 0, 0,0,N_d_u_less_than, 1}, /* du< */
{ 4, 1, 1, 0 , 0, 0,0,N_d_u_greater_than, 1}, /* du> */
{ 4, 1, 1, 0 , 0, 0,0,N_d_u_less_or_equal, 1}, /* du<= */
{ 4, 1, 1, 0 , 0, 0,0,N_d_u_greater_or_equal, 1}, /* du>= */
#endif
{ 3, 1, 1, 0 , 0, 0,0,N_within, 1}, /* within */
{ 1, 1, 1, 0 , 0, 0,1,N_useraddr, 1}, /* useraddr */
{ 1, 0, 1, 0 , 0, 0,0,N_up_store, 1}, /* up! */
{ 0, 1, 1, 0 , 0, 0,0,N_sp_fetch, 1}, /* sp@ */
{ 1, 0, 1, 0 , 0, 0,0,N_sp_store, 1}, /* sp! */
{ 0, 1, 1, 0 , 0, 0,0,N_rp_fetch, 1}, /* rp@ */
{ 1, 0, 1, 0 , 0, 0,0,N_rp_store, 1}, /* rp! */
#ifdef HAS_FLOATING
{ 0, 1, 1, 0 , 0, 0,0,N_fp_fetch, 1}, /* fp@ */
{ 1, 0, 1, 0 , 0, 0,0,N_fp_store, 1}, /* fp! */
#endif
{ 1, 1, 2, 0 , 0, 0,0,N_to_r, 1}, /* >r */
{ 1, 1, 2, 0 , 0, 0,0,N_r_from, 1}, /* r> */
{ 1, 0, 1, 0 , 0, 0,0,N_rdrop, 1}, /* rdrop */
{ 2, 2, 2, 0 , 0, 0,0,N_two_to_r, 1}, /* 2>r */
{ 2, 2, 2, 0 , 0, 0,0,N_two_r_from, 1}, /* 2r> */
{ 2, 4, 1, 0 , 0, 0,0,N_two_r_fetch, 1}, /* 2r@ */
{ 2, 0, 1, 0 , 0, 0,0,N_two_r_drop, 1}, /* 2rdrop */
{ 2, 3, 1, 0 , 0, 0,0,N_over, 1}, /* over */
{ 1, 0, 1, 0 , 0, 0,0,N_drop, 1}, /* drop */
{ 2, 2, 0, 0 , 0, 0,0,N_swap, 1}, /* swap */
{ 1, 2, 1, 0 , 0, 0,0,N_dupe, 1}, /* dup */
{ 3, 3, 0, 0 , 0, 0,0,N_rote, 1}, /* rot */
{ 3, 3, 0, 0 , 0, 0,0,N_not_rote, 1}, /* -rot */
{ 2, 1, 1, 0 , 0, 0,0,N_nip, 1}, /* nip */
{ 2, 3, 1, 0 , 0, 0,0,N_tuck, 1}, /* tuck */
{ 1, 1, 0, 0 , 0, 0,0,N_question_dupe, 1}, /* ?dup */
{ 1, 1, 0, 0 , 0, 0,0,N_pick, 1}, /* pick */
{ 2, 0, 1, 0 , 0, 0,0,N_two_drop, 1}, /* 2drop */
{ 2, 4, 1, 0 , 0, 0,0,N_two_dupe, 1}, /* 2dup */
{ 4, 6, 1, 0 , 0, 0,0,N_two_over, 1}, /* 2over */
{ 4, 4, 0, 0 , 0, 0,0,N_two_swap, 1}, /* 2swap */
{ 6, 6, 0, 0 , 0, 0,0,N_two_rote, 1}, /* 2rot */
{ 4, 2, 1, 0 , 0, 0,0,N_two_nip, 1}, /* 2nip */
{ 4, 6, 1, 0 , 0, 0,0,N_two_tuck, 1}, /* 2tuck */
{ 1, 1, 0, 0 , 0, 0,0,N_fetch, 1}, /* @ */
{ 1, 1, 1, 0 , 0, 0,1,N_lit_fetch, 1}, /* lit@ */
{ 2, 0, 1, 0 , 0, 0,0,N_store, 1}, /* ! */
{ 2, 0, 1, 0 , 0, 0,0,N_plus_store, 1}, /* +! */
{ 1, 1, 0, 0 , 0, 0,0,N_c_fetch, 1}, /* c@ */
{ 2, 0, 1, 0 , 0, 0,0,N_c_store, 1}, /* c! */
{ 3, 0, 1, 0 , 0, 0,0,N_two_store, 1}, /* 2! */
{ 1, 2, 1, 0 , 0, 0,0,N_two_fetch, 1}, /* 2@ */
{ 1, 1, 0, 0 , 0, 0,0,N_cell_plus, 1}, /* cell+ */
{ 1, 1, 0, 0 , 0, 0,0,N_cells, 1}, /* cells */
{ 1, 1, 0, 0 , 0, 0,0,N_char_plus, 1}, /* char+ */
{ 1, 1, 0, 0 , 0, 0,0,N_paren_chars, 1}, /* (chars) */
{ 1, 2, 1, 0 , 0, 0,0,N_count, 1}, /* count */
#ifdef HAS_F83HEADERSTRING
{ 3, 1, 1, 0 , 0, 0,0,N_paren_f83find, 1}, /* (f83find) */
#else /* 174 */
{ 3, 1, 1, 0 , 0, 0,0,N_paren_listlfind, 1}, /* (listlfind) */
#ifdef HAS_HASH
{ 3, 1, 1, 0 , 0, 0,0,N_paren_hashlfind, 1}, /* (hashlfind) */
{ 3, 1, 1, 0 , 0, 0,0,N_paren_tablelfind, 1}, /* (tablelfind) */
{ 3, 1, 1, 0 , 0, 0,0,N_paren_hashkey1, 1}, /* (hashkey1) */
#endif
#endif
{ 2, 2, 0, 0 , 0, 0,0,N_paren_parse_white, 1}, /* (parse-white) */
{ 1, 1, 0, 0 , 0, 0,0,N_aligned, 1}, /* aligned */
{ 1, 1, 0, 0 , 0, 0,0,N_f_aligned, 1}, /* faligned */
{ 0, 1, 1, 0 , 0, 0,0,N_threading_method, 1}, /* threading-method */
{ 1, 1, 0, 0 , 0, 0,0,N_paren_key_file, 1}, /* key-file */
{ 1, 1, 0, 0 , 0, 0,0,N_key_q_file, 1}, /* key?-file */
{ 0, 1, 1, 0 , 0, 0,0,N_stdin, 1}, /* stdin */
{ 0, 1, 1, 0 , 0, 0,0,N_stdout, 1}, /* stdout */
{ 0, 1, 1, 0 , 0, 0,0,N_stderr, 1}, /* stderr */
#ifdef HAS_OS
{ 0, 2, 1, 0 , 0, 0,0,N_form, 1}, /* form */
{ 1, 1, 0, 0 , 0, 0,0,N_wcwidth, 1}, /* wcwidth */
{ 2, 0, 1, 0 , 0, 0,0,N_flush_icache, 1}, /* flush-icache */
{ 1, 0, 1, 0 , 0, 0,0,N_paren_bye, 1}, /* (bye) */
{ 2, 2, 0, 0 , 0, 0,0,N_paren_system, 1}, /* (system) */
{ 2, 2, 0, 0 , 0, 0,0,N_getenv, 1}, /* getenv */
{ 3, 2, 1, 0 , 0, 0,0,N_open_pipe, 1}, /* open-pipe */
{ 1, 2, 1, 0 , 0, 0,0,N_close_pipe, 1}, /* close-pipe */
{ 0, 6, 1, 0 , 0, 0,0,N_time_and_date, 1}, /* time&date */
{ 1, 0, 1, 0 , 0, 0,0,N_ms, 1}, /* ms */
{ 1, 2, 1, 0 , 0, 0,0,N_allocate, 1}, /* allocate */
{ 1, 1, 0, 0 , 0, 0,0,N_free, 1}, /* free */
{ 2, 2, 0, 0 , 0, 0,0,N_resize, 1}, /* resize */
{ 1, 2, 1, 0 , 0, 0,0,N_strerror, 1}, /* strerror */
{ 1, 2, 1, 0 , 0, 0,0,N_strsignal, 1}, /* strsignal */
{ 1, 0, 1, 0 , 0, 0,0,N_call_c, 1}, /* call-c */
#endif
#ifdef HAS_FILE
{ 1, 1, 0, 0 , 0, 0,0,N_close_file, 1}, /* close-file */
{ 3, 2, 1, 0 , 0, 0,0,N_open_file, 1}, /* open-file */
{ 3, 2, 1, 0 , 0, 0,0,N_create_file, 1}, /* create-file */
{ 2, 1, 1, 0 , 0, 0,0,N_delete_file, 1}, /* delete-file */
{ 4, 1, 1, 0 , 0, 0,0,N_rename_file, 1}, /* rename-file */
{ 1, 3, 1, 0 , 0, 0,0,N_file_position, 1}, /* file-position */
{ 3, 1, 1, 0 , 0, 0,0,N_reposition_file, 1}, /* reposition-file */
{ 1, 3, 1, 0 , 0, 0,0,N_file_size, 1}, /* file-size */
{ 3, 1, 1, 0 , 0, 0,0,N_resize_file, 1}, /* resize-file */
{ 3, 2, 1, 0 , 0, 0,0,N_read_file, 1}, /* read-file */
{ 3, 4, 1, 0 , 0, 0,0,N_paren_read_line, 1}, /* (read-line) */
#endif
{ 3, 1, 1, 0 , 0, 0,0,N_write_file, 1}, /* write-file */
{ 2, 1, 1, 0 , 0, 0,0,N_emit_file, 1}, /* emit-file */
#ifdef HAS_FILE
{ 1, 1, 0, 0 , 0, 0,0,N_flush_file, 1}, /* flush-file */
{ 2, 2, 0, 0 , 0, 0,0,N_file_status, 1}, /* file-status */
{ 1, 1, 0, 0 , 0, 0,0,N_file_eof_query, 1}, /* file-eof? */
{ 2, 2, 0, 0 , 0, 0,0,N_open_dir, 1}, /* open-dir */
{ 3, 3, 0, 0 , 0, 0,0,N_read_dir, 1}, /* read-dir */
{ 1, 1, 0, 0 , 0, 0,0,N_close_dir, 1}, /* close-dir */
{ 4, 1, 1, 0 , 0, 0,0,N_match_file, 1}, /* filename-match */
{ 2, 1, 1, 0 , 0, 0,0,N_set_dir, 1}, /* set-dir */
{ 2, 2, 0, 0 , 0, 0,0,N_get_dir, 1}, /* get-dir */
{ 3, 1, 1, 0 , 0, 0,0,N_equals_mkdir, 1}, /* =mkdir */
#endif
{ 0, 2, 1, 0 , 0, 0,0,N_newline, 1}, /* newline */
#ifdef HAS_OS
{ 0, 2, 1, 0 , 0, 0,0,N_utime, 1}, /* utime */
{ 0, 4, 1, 0 , 0, 0,0,N_cputime, 1}, /* cputime */
#endif
#ifdef HAS_FLOATING
{ 2, 1, 2, 0 , 0, 0,0,N_f_equals, 1}, /* f= */
{ 2, 1, 2, 0 , 0, 0,0,N_f_not_equals, 1}, /* f<> */
{ 2, 1, 2, 0 , 0, 0,0,N_f_less_than, 1}, /* f< */
{ 2, 1, 2, 0 , 0, 0,0,N_f_greater_than, 1}, /* f> */
{ 2, 1, 2, 0 , 0, 0,0,N_f_less_or_equal, 1}, /* f<= */
{ 2, 1, 2, 0 , 0, 0,0,N_f_greater_or_equal, 1}, /* f>= */
{ 1, 1, 2, 0 , 0, 0,0,N_f_zero_equals, 1}, /* f0= */
{ 1, 1, 2, 0 , 0, 0,0,N_f_zero_not_equals, 1}, /* f0<> */
{ 1, 1, 2, 0 , 0, 0,0,N_f_zero_less_than, 1}, /* f0< */
{ 1, 1, 2, 0 , 0, 0,0,N_f_zero_greater_than, 1}, /* f0> */
{ 1, 1, 2, 0 , 0, 0,0,N_f_zero_less_or_equal, 1}, /* f0<= */
{ 1, 1, 2, 0 , 0, 0,0,N_f_zero_greater_or_equal, 1}, /* f0>= */
{ 1, 1, 2, 0 , 0, 0,0,N_s_to_f, 1}, /* s>f */
{ 2, 1, 2, 0 , 0, 0,0,N_d_to_f, 1}, /* d>f */
{ 1, 2, 2, 0 , 0, 0,0,N_f_to_d, 1}, /* f>d */
{ 1, 1, 2, 0 , 0, 0,0,N_f_to_s, 1}, /* f>s */
{ 2, 0, 2, 0 , 0, 0,0,N_f_store, 1}, /* f! */
{ 1, 1, 2, 0 , 0, 0,0,N_f_fetch, 1}, /* f@ */
{ 1, 1, 2, 0 , 0, 0,0,N_d_f_fetch, 1}, /* df@ */
{ 2, 0, 2, 0 , 0, 0,0,N_d_f_store, 1}, /* df! */
{ 1, 1, 2, 0 , 0, 0,0,N_s_f_fetch, 1}, /* sf@ */
{ 2, 0, 2, 0 , 0, 0,0,N_s_f_store, 1}, /* sf! */
{ 2, 1, 1, 0 , 0, 0,0,N_f_plus, 1}, /* f+ */
{ 2, 1, 1, 0 , 0, 0,0,N_f_minus, 1}, /* f- */
{ 2, 1, 1, 0 , 0, 0,0,N_f_star, 1}, /* f* */
{ 2, 1, 1, 0 , 0, 0,0,N_f_slash, 1}, /* f/ */
{ 2, 1, 1, 0 , 0, 0,0,N_f_star_star, 1}, /* f** */
{ 2, 1, 1, 0 , 0, 0,0,N_fm_star, 1}, /* fm* */
{ 2, 1, 1, 0 , 0, 0,0,N_fm_slash, 1}, /* fm/ */
{ 3, 1, 1, 0 , 0, 0,0,N_fm_star_slash, 1}, /* fmx/ */
{ 1, 1, 0, 0 , 0, 0,0,N_fm_square, 1}, /* f**2 */
{ 1, 1, 0, 0 , 0, 0,0,N_f_negate, 1}, /* fnegate */
{ 1, 0, 1, 0 , 0, 0,0,N_f_drop, 1}, /* fdrop */
{ 1, 2, 1, 0 , 0, 0,0,N_f_dupe, 1}, /* fdup */
{ 2, 2, 0, 0 , 0, 0,0,N_f_swap, 1}, /* fswap */
{ 2, 3, 1, 0 , 0, 0,0,N_f_over, 1}, /* fover */
{ 3, 3, 0, 0 , 0, 0,0,N_f_rote, 1}, /* frot */
{ 2, 1, 1, 0 , 0, 0,0,N_f_nip, 1}, /* fnip */
{ 2, 3, 1, 0 , 0, 0,0,N_f_tuck, 1}, /* ftuck */
{ 1, 1, 0, 0 , 0, 0,0,N_float_plus, 1}, /* float+ */
{ 1, 1, 0, 0 , 0, 0,0,N_floats, 1}, /* floats */
{ 1, 1, 0, 0 , 0, 0,0,N_floor, 1}, /* floor */
{ 1, 1, 0, 0 , 0, 0,0,N_f_round, 1}, /* fround */
{ 2, 1, 1, 0 , 0, 0,0,N_f_max, 1}, /* fmax */
{ 2, 1, 1, 0 , 0, 0,0,N_f_min, 1}, /* fmin */
{ 3, 3, 2, 0 , 0, 0,0,N_represent, 1}, /* represent */
{ 2, 1, 1, 0 , 0, 0,0,N_to_float, 1}, /* >float */
{ 1, 1, 0, 0 , 0, 0,0,N_f_abs, 1}, /* fabs */
{ 1, 1, 0, 0 , 0, 0,0,N_f_a_cos, 1}, /* facos */
{ 1, 1, 0, 0 , 0, 0,0,N_f_a_sine, 1}, /* fasin */
{ 1, 1, 0, 0 , 0, 0,0,N_f_a_tan, 1}, /* fatan */
{ 2, 1, 1, 0 , 0, 0,0,N_f_a_tan_two, 1}, /* fatan2 */
{ 1, 1, 0, 0 , 0, 0,0,N_f_cos, 1}, /* fcos */
{ 1, 1, 0, 0 , 0, 0,0,N_f_e_x_p, 1}, /* fexp */
{ 1, 1, 0, 0 , 0, 0,0,N_f_e_x_p_m_one, 1}, /* fexpm1 */
{ 1, 1, 0, 0 , 0, 0,0,N_f_l_n, 1}, /* fln */
{ 1, 1, 0, 0 , 0, 0,0,N_f_l_n_p_one, 1}, /* flnp1 */
{ 1, 1, 0, 0 , 0, 0,0,N_f_log, 1}, /* flog */
{ 1, 1, 0, 0 , 0, 0,0,N_f_a_log, 1}, /* falog */
{ 1, 1, 0, 0 , 0, 0,0,N_f_sine, 1}, /* fsin */
{ 1, 2, 1, 0 , 0, 0,0,N_f_sine_cos, 1}, /* fsincos */
{ 1, 1, 0, 0 , 0, 0,0,N_f_square_root, 1}, /* fsqrt */
{ 1, 1, 0, 0 , 0, 0,0,N_f_tan, 1}, /* ftan */
{ 1, 1, 0, 0 , 0, 0,0,N_f_cinch, 1}, /* fsinh */
{ 1, 1, 0, 0 , 0, 0,0,N_f_cosh, 1}, /* fcosh */
{ 1, 1, 0, 0 , 0, 0,0,N_f_tan_h, 1}, /* ftanh */
{ 1, 1, 0, 0 , 0, 0,0,N_f_a_cinch, 1}, /* fasinh */
{ 1, 1, 0, 0 , 0, 0,0,N_f_a_cosh, 1}, /* facosh */
{ 1, 1, 0, 0 , 0, 0,0,N_f_a_tan_h, 1}, /* fatanh */
{ 1, 1, 0, 0 , 0, 0,0,N_s_floats, 1}, /* sfloats */
{ 1, 1, 0, 0 , 0, 0,0,N_d_floats, 1}, /* dfloats */
{ 1, 1, 0, 0 , 0, 0,0,N_s_f_aligned, 1}, /* sfaligned */
{ 1, 1, 0, 0 , 0, 0,0,N_d_f_aligned, 1}, /* dfaligned */
{ 5, 1, 2, 0 , 0, 0,0,N_v_star, 1}, /* v* */
{ 6, 0, 2, 0 , 0, 0,0,N_faxpy, 1}, /* faxpy */
#endif
#ifdef HAS_GLOCALS
{ 1, 1, 1, 0 , 0, 0,1,N_fetch_local_number, 1}, /* @local# */
{ 0, 1, 1, 0 , 0, 0,0,N_fetch_local_zero, 1}, /* @local0 */
{ 0, 1, 1, 0 , 0, 0,0,N_fetch_local_four, 1}, /* @local1 */
{ 0, 1, 1, 0 , 0, 0,0,N_fetch_local_eight, 1}, /* @local2 */
{ 0, 1, 1, 0 , 0, 0,0,N_fetch_local_twelve, 1}, /* @local3 */
#ifdef HAS_FLOATING
{ 1, 1, 1, 0 , 0, 0,1,N_f_fetch_local_number, 1}, /* f@local# */
{ 0, 1, 1, 0 , 0, 0,0,N_f_fetch_local_zero, 1}, /* f@local0 */
{ 0, 1, 1, 0 , 0, 0,0,N_f_fetch_local_eight, 1}, /* f@local1 */
#endif
{ 1, 1, 1, 0 , 0, 0,1,N_laddr_number, 1}, /* laddr# */
{ 1, 0, 0, 0 , 0, 0,1,N_lp_plus_store_number, 1}, /* lp+!# */
{ 0, 0, 0, 0 , 0, 0,0,N_minus_four_lp_plus_store, 1}, /* lp- */
{ 0, 0, 0, 0 , 0, 0,0,N_eight_lp_plus_store, 1}, /* lp+ */
{ 0, 0, 0, 0 , 0, 0,0,N_sixteen_lp_plus_store, 1}, /* lp+2 */
{ 1, 0, 1, 0 , 0, 0,0,N_lp_store, 1}, /* lp! */
{ 1, 0, 1, 0 , 0, 0,0,N_to_l, 1}, /* >l */
#ifdef HAS_FLOATING
{ 1, 0, 1, 0 , 0, 0,0,N_f_to_l, 1}, /* f>l */
{ 1, 1, 2, 0 , 0, 0,0,N_fpick, 1}, /* fpick */
#endif
#endif
#ifdef HAS_OS
{ 2, 1, 1, 0 , 0, 0,0,N_open_lib, 1}, /* open-lib */
{ 3, 1, 1, 0 , 0, 0,0,N_lib_sym, 1}, /* lib-sym */
{ 1, 0, 1, 0 , 0, 0,0,N_wcall, 1}, /* wcall */
{ 1, 1, 0, 0 , 0, 0,0,N_u_w_fetch, 1}, /* uw@ */
{ 1, 1, 0, 0 , 0, 0,0,N_s_w_fetch, 1}, /* sw@ */
{ 2, 0, 1, 0 , 0, 0,0,N_w_store, 1}, /* w! */
{ 1, 1, 0, 0 , 0, 0,0,N_u_l_fetch, 1}, /* ul@ */
{ 1, 1, 0, 0 , 0, 0,0,N_s_l_fetch, 1}, /* sl@ */
{ 2, 0, 1, 0 , 0, 0,0,N_l_store, 1}, /* l! */
{ 0, 2, 1, 0 , 0, 0,0,N_lib_error, 1}, /* lib-error */
{ 2, 0, 1, 0 , 0, 0,0,N_w_store_be, 1}, /* be-w! */
{ 2, 0, 1, 0 , 0, 0,0,N_l_store_be, 1}, /* be-l! */
{ 2, 0, 1, 0 , 0, 0,0,N_w_store_le, 1}, /* le-w! */
{ 2, 0, 1, 0 , 0, 0,0,N_l_store_le, 1}, /* le-l! */
{ 1, 1, 0, 0 , 0, 0,0,N_w_fetch_be, 1}, /* be-uw@ */
{ 1, 1, 0, 0 , 0, 0,0,N_l_fetch_be, 1}, /* be-ul@ */
{ 1, 1, 0, 0 , 0, 0,0,N_w_fetch_le, 1}, /* le-uw@ */
{ 1, 1, 0, 0 , 0, 0,0,N_l_fetch_le, 1}, /* le-ul@ */
#ifdef HAS_64BIT
{ 2, 0, 1, 0 , 0, 0,0,N_x_store, 1}, /* x! */
{ 1, 1, 0, 0 , 0, 0,0,N_u_x_fetch, 1}, /* ux@ */
{ 1, 1, 0, 0 , 0, 0,0,N_s_x_fetch, 1}, /* sx@ */
{ 2, 0, 1, 0 , 0, 0,0,N_b_e_x_store, 1}, /* be-x! */
{ 2, 0, 1, 0 , 0, 0,0,N_l_e_x_store, 1}, /* le-x! */
{ 1, 1, 0, 0 , 0, 0,0,N_b_e_u_x_fetch, 1}, /* be-ux@ */
{ 1, 1, 0, 0 , 0, 0,0,N_l_e_u_x_fetch, 1}, /* le-ux@ */
#endif
#endif
#ifdef HAS_PEEPHOLE
{ 1, 0, 1, 0 , 0, 0,0,N_compile_prim1, 1}, /* compile-prim1 */
{ 0, 0, 0, 0 , 0, 0,0,N_finish_code, 1}, /* finish-code */
{ 1, 1, 0, 0 , 0, 0,0,N_forget_dyncode, 1}, /* forget-dyncode */
{ 1, 1, 0, 0 , 0, 0,0,N_decompile_prim, 1}, /* decompile-prim */
{ 1, 0, 0, 0 , 0, 0,1,N_set_next_code, 1}, /* set-next-code */
{ 2, 1, 1, 0 , 0, 0,2,N_call2, 1}, /* call2 */
{ 0, 1, 1, 0 , 0, 0,0,N_tag_offsets, 1}, /* tag-offsets */
#endif
{ 1, 0, 0, 0 , 0, 0,1,N_abi_call, 1}, /* abi-call */
{ 1, 0, 0, 0 , 0, 0,1,N_semi_abi_code_exec, 1}, /* ;abi-code-exec */
{ 1, 0, 0, 0 , 0, 0,1,N_lit_execute, 1}, /* lit-execute */
{ 2, 1, 0, 0 , 0, 0,2,N_START_SUPER+    0, 2}, /* super1 */
{ 1, 1, 1, 0 , 0, 0,2,N_START_SUPER+    2, 2}, /* super3 */
{ 2, 0, 1, 0 , 0, 0,2,N_START_SUPER+    4, 2}, /* super6 */
{ 3, 0, 2, 1 , 0, 0,1,N_START_SUPER+    6, 2}, /* super11 */
{ 2, 1, 0, 0 , 0, 0,3,N_START_SUPER+    8, 3}, /* super12 */
{ 1, 1, 1, 0 , 0, 0,2,N_START_SUPER+   11, 2}, /* super15 */
{ 2, 1, 1, 0 , 0, 0,1,N_START_SUPER+   13, 2}, /* super16 */
{ 3, 0, 2, 1 , 0, 0,3,N_START_SUPER+   15, 3}, /* super18 */
{ 2, 1, 0, 0 , 0, 0,3,N_START_SUPER+   18, 3}, /* super19 */
{ 2, 0, 1, 0 , 0, 0,2,N_START_SUPER+   21, 2}, /* super23 */
{ 2, 1, 1, 1 , 0, 0,1,N_START_SUPER+   23, 2}, /* super25 */
{ 2, 1, 0, 0 , 0, 0,3,N_START_SUPER+   25, 3}, /* super26 */
{ 1, 2, 1, 0 , 0, 0,1,N_START_SUPER+   28, 2}, /* super27 */
{ 0, 1, 1, 0 , 1, 0,1,N_lit, 1}, /* lit */
{ 0, 1, 1, 0 , 0, 2,1,N_lit, 1}, /* lit */
{ 0, 1, 1, 0 , 2, 3,1,N_lit, 1}, /* lit */
{ 0, 1, 1, 0 , 3, 4,1,N_lit, 1}, /* lit */
{ 0, 1, 1, 0 , 4, 5,1,N_lit, 1}, /* lit */
{ 0, 1, 1, 0 , 5, 6,1,N_lit, 1}, /* lit */
{ 0, 1, 1, 0 , 6, 7,1,N_lit, 1}, /* lit */
{ 0, 1, 1, 0 , 7, 8,1,N_lit, 1}, /* lit */
{ 0, 1, 1, 0 , 8, 8,1,N_lit, 1}, /* lit */
{ 0, 1, 0, 0 , 2, 2,0,N_fetch, 1}, /* @ */
{ 0, 1, 0, 0 , 3, 3,0,N_fetch, 1}, /* @ */
{ 0, 1, 0, 0 , 4, 4,0,N_fetch, 1}, /* @ */
{ 0, 1, 0, 0 , 5, 5,0,N_fetch, 1}, /* @ */
{ 0, 1, 0, 0 , 6, 6,0,N_fetch, 1}, /* @ */
{ 0, 1, 0, 0 , 7, 7,0,N_fetch, 1}, /* @ */
{ 0, 1, 0, 0 , 8, 8,0,N_fetch, 1}, /* @ */
{ 1, 0, 1, 1 , 2, 0,1,N_question_branch, 1}, /* ?branch */
{ 1, 0, 1, 1 , 3, 0,1,N_question_branch, 1}, /* ?branch */
{ 1, 0, 1, 1 , 4, 0,1,N_question_branch, 1}, /* ?branch */
{ 1, 0, 1, 1 , 5, 0,1,N_question_branch, 1}, /* ?branch */
{ 1, 0, 1, 1 , 6, 0,1,N_question_branch, 1}, /* ?branch */
{ 1, 0, 1, 1 , 7, 0,1,N_question_branch, 1}, /* ?branch */
{ 1, 0, 1, 1 , 8, 0,1,N_question_branch, 1}, /* ?branch */
{ 0, 1, 1, 0 , 1, 0,1,N_lit_fetch, 1}, /* lit@ */
{ 0, 1, 1, 0 , 0, 2,1,N_lit_fetch, 1}, /* lit@ */
{ 0, 1, 1, 0 , 2, 3,1,N_lit_fetch, 1}, /* lit@ */
{ 0, 1, 1, 0 , 3, 4,1,N_lit_fetch, 1}, /* lit@ */
{ 0, 1, 1, 0 , 4, 5,1,N_lit_fetch, 1}, /* lit@ */
{ 0, 1, 1, 0 , 5, 6,1,N_lit_fetch, 1}, /* lit@ */
{ 0, 1, 1, 0 , 6, 7,1,N_lit_fetch, 1}, /* lit@ */
{ 0, 1, 1, 0 , 7, 8,1,N_lit_fetch, 1}, /* lit@ */
{ 0, 1, 1, 0 , 8, 8,1,N_lit_fetch, 1}, /* lit@ */
{ 1, 2, 0, 0 , 2, 2,0,N_swap, 1}, /* swap */
{ 1, 2, 0, 0 , 3, 3,0,N_swap, 1}, /* swap */
{ 1, 2, 0, 0 , 4, 4,0,N_swap, 1}, /* swap */
{ 1, 2, 0, 0 , 5, 5,0,N_swap, 1}, /* swap */
{ 1, 2, 0, 0 , 6, 6,0,N_swap, 1}, /* swap */
{ 1, 2, 0, 0 , 7, 7,0,N_swap, 1}, /* swap */
{ 1, 2, 0, 0 , 8, 8,0,N_swap, 1}, /* swap */
{ 1, 2, 0, 0 , 1, 2,0,N_swap, 1}, /* swap */
{ 1, 2, 0, 0 , 0, 2,0,N_swap, 1}, /* swap */
{ 1, 2, 0, 0 , 2, 3,0,N_swap, 1}, /* swap */
{ 1, 2, 0, 0 , 3, 2,0,N_swap, 1}, /* swap */
{ 0, 1, 0, 0 , 2, 2,0,N_c_fetch, 1}, /* c@ */
{ 0, 1, 0, 0 , 3, 3,0,N_c_fetch, 1}, /* c@ */
{ 0, 1, 0, 0 , 4, 4,0,N_c_fetch, 1}, /* c@ */
{ 0, 1, 0, 0 , 5, 5,0,N_c_fetch, 1}, /* c@ */
{ 0, 1, 0, 0 , 6, 6,0,N_c_fetch, 1}, /* c@ */
{ 0, 1, 0, 0 , 7, 7,0,N_c_fetch, 1}, /* c@ */
{ 0, 1, 0, 0 , 8, 8,0,N_c_fetch, 1}, /* c@ */
{ 1, 1, 1, 0 , 2, 0,0,N_equals, 1}, /* = */
{ 1, 1, 1, 0 , 3, 2,0,N_equals, 1}, /* = */
{ 1, 1, 1, 0 , 4, 3,0,N_equals, 1}, /* = */
{ 1, 1, 1, 0 , 5, 4,0,N_equals, 1}, /* = */
{ 1, 1, 1, 0 , 6, 5,0,N_equals, 1}, /* = */
{ 1, 1, 1, 0 , 7, 6,0,N_equals, 1}, /* = */
{ 1, 1, 1, 0 , 8, 7,0,N_equals, 1}, /* = */
{ 0, 1, 0, 0 , 2, 2,0,N_zero_equals, 1}, /* 0= */
{ 0, 1, 0, 0 , 3, 3,0,N_zero_equals, 1}, /* 0= */
{ 0, 1, 0, 0 , 4, 4,0,N_zero_equals, 1}, /* 0= */
{ 0, 1, 0, 0 , 5, 5,0,N_zero_equals, 1}, /* 0= */
{ 0, 1, 0, 0 , 6, 6,0,N_zero_equals, 1}, /* 0= */
{ 0, 1, 0, 0 , 7, 7,0,N_zero_equals, 1}, /* 0= */
{ 0, 1, 0, 0 , 8, 8,0,N_zero_equals, 1}, /* 0= */
{ 0, 2, 1, 0 , 0, 2,0,N_dupe, 1}, /* dup */
{ 0, 2, 1, 0 , 2, 3,0,N_dupe, 1}, /* dup */
{ 0, 2, 1, 0 , 3, 4,0,N_dupe, 1}, /* dup */
{ 0, 2, 1, 0 , 4, 5,0,N_dupe, 1}, /* dup */
{ 0, 2, 1, 0 , 5, 6,0,N_dupe, 1}, /* dup */
{ 0, 2, 1, 0 , 6, 7,0,N_dupe, 1}, /* dup */
{ 0, 2, 1, 0 , 7, 8,0,N_dupe, 1}, /* dup */
{ 0, 2, 1, 0 , 8, 8,0,N_dupe, 1}, /* dup */
{ 0, 2, 1, 0 , 1, 0,0,N_dupe, 1}, /* dup */
{ 1, 1, 1, 0 , 2, 0,0,N_plus, 1}, /* + */
{ 1, 1, 1, 0 , 3, 2,0,N_plus, 1}, /* + */
{ 1, 1, 1, 0 , 4, 3,0,N_plus, 1}, /* + */
{ 1, 1, 1, 0 , 5, 4,0,N_plus, 1}, /* + */
{ 1, 1, 1, 0 , 6, 5,0,N_plus, 1}, /* + */
{ 1, 1, 1, 0 , 7, 6,0,N_plus, 1}, /* + */
{ 1, 1, 1, 0 , 8, 7,0,N_plus, 1}, /* + */
{ 0, 2, 1, 0 , 1, 0,0,N_i, 1}, /* i */
{ 0, 2, 1, 0 , 0, 2,0,N_i, 1}, /* i */
{ 0, 2, 1, 0 , 2, 3,0,N_i, 1}, /* i */
{ 0, 2, 1, 0 , 3, 4,0,N_i, 1}, /* i */
{ 0, 2, 1, 0 , 4, 5,0,N_i, 1}, /* i */
{ 0, 2, 1, 0 , 5, 6,0,N_i, 1}, /* i */
{ 0, 2, 1, 0 , 6, 7,0,N_i, 1}, /* i */
{ 0, 2, 1, 0 , 7, 8,0,N_i, 1}, /* i */
{ 0, 2, 1, 0 , 8, 8,0,N_i, 1}, /* i */
{ 1, 0, 1, 0 , 2, 1,0,N_plus_store, 1}, /* +! */
{ 1, 0, 1, 0 , 3, 0,0,N_plus_store, 1}, /* +! */
{ 1, 0, 1, 0 , 4, 2,0,N_plus_store, 1}, /* +! */
{ 1, 0, 1, 0 , 5, 3,0,N_plus_store, 1}, /* +! */
{ 1, 0, 1, 0 , 6, 4,0,N_plus_store, 1}, /* +! */
{ 1, 0, 1, 0 , 7, 5,0,N_plus_store, 1}, /* +! */
{ 1, 0, 1, 0 , 8, 6,0,N_plus_store, 1}, /* +! */
{ 1, 1, 1, 0 , 2, 0,0,N_and, 1}, /* and */
{ 1, 1, 1, 0 , 3, 2,0,N_and, 1}, /* and */
{ 1, 1, 1, 0 , 4, 3,0,N_and, 1}, /* and */
{ 1, 1, 1, 0 , 5, 4,0,N_and, 1}, /* and */
{ 1, 1, 1, 0 , 6, 5,0,N_and, 1}, /* and */
{ 1, 1, 1, 0 , 7, 6,0,N_and, 1}, /* and */
{ 1, 1, 1, 0 , 8, 7,0,N_and, 1}, /* and */
{ 0, 1, 1, 0 , 1, 0,1,N_useraddr, 1}, /* useraddr */
{ 0, 1, 1, 0 , 0, 2,1,N_useraddr, 1}, /* useraddr */
{ 0, 1, 1, 0 , 2, 3,1,N_useraddr, 1}, /* useraddr */
{ 0, 1, 1, 0 , 3, 4,1,N_useraddr, 1}, /* useraddr */
{ 0, 1, 1, 0 , 4, 5,1,N_useraddr, 1}, /* useraddr */
{ 0, 1, 1, 0 , 5, 6,1,N_useraddr, 1}, /* useraddr */
{ 0, 1, 1, 0 , 6, 7,1,N_useraddr, 1}, /* useraddr */
{ 0, 1, 1, 0 , 7, 8,1,N_useraddr, 1}, /* useraddr */
{ 0, 1, 1, 0 , 8, 8,1,N_useraddr, 1}, /* useraddr */
{ 1, 3, 1, 0 , 2, 3,0,N_over, 1}, /* over */
{ 1, 3, 1, 0 , 3, 4,0,N_over, 1}, /* over */
{ 1, 3, 1, 0 , 4, 5,0,N_over, 1}, /* over */
{ 1, 3, 1, 0 , 5, 6,0,N_over, 1}, /* over */
{ 1, 3, 1, 0 , 6, 7,0,N_over, 1}, /* over */
{ 1, 3, 1, 0 , 7, 8,0,N_over, 1}, /* over */
{ 1, 3, 1, 0 , 8, 8,0,N_over, 1}, /* over */
{ 1, 3, 1, 0 , 1, 0,0,N_over, 1}, /* over */
{ 1, 3, 1, 0 , 0, 2,0,N_over, 1}, /* over */
{ 1, 3, 1, 0 , 2, 0,0,N_over, 1}, /* over */
{ 1, 3, 1, 0 , 3, 2,0,N_over, 1}, /* over */
{ 0, 1, 0, 0 , 2, 2,0,N_negate, 1}, /* negate */
{ 0, 1, 0, 0 , 3, 3,0,N_negate, 1}, /* negate */
{ 0, 1, 0, 0 , 4, 4,0,N_negate, 1}, /* negate */
{ 0, 1, 0, 0 , 5, 5,0,N_negate, 1}, /* negate */
{ 0, 1, 0, 0 , 6, 6,0,N_negate, 1}, /* negate */
{ 0, 1, 0, 0 , 7, 7,0,N_negate, 1}, /* negate */
{ 0, 1, 0, 0 , 8, 8,0,N_negate, 1}, /* negate */
{ 0, 1, 0, 0 , 2, 2,0,N_cells, 1}, /* cells */
{ 0, 1, 0, 0 , 3, 3,0,N_cells, 1}, /* cells */
{ 0, 1, 0, 0 , 4, 4,0,N_cells, 1}, /* cells */
{ 0, 1, 0, 0 , 5, 5,0,N_cells, 1}, /* cells */
{ 0, 1, 0, 0 , 6, 6,0,N_cells, 1}, /* cells */
{ 0, 1, 0, 0 , 7, 7,0,N_cells, 1}, /* cells */
{ 0, 1, 0, 0 , 8, 8,0,N_cells, 1}, /* cells */
{ 2, 3, 0, 0 , 3, 3,0,N_rote, 1}, /* rot */
{ 2, 3, 0, 0 , 4, 4,0,N_rote, 1}, /* rot */
{ 2, 3, 0, 0 , 5, 5,0,N_rote, 1}, /* rot */
{ 2, 3, 0, 0 , 6, 6,0,N_rote, 1}, /* rot */
{ 2, 3, 0, 0 , 7, 7,0,N_rote, 1}, /* rot */
{ 2, 3, 0, 0 , 8, 8,0,N_rote, 1}, /* rot */
{ 2, 3, 0, 0 , 3, 0,0,N_rote, 1}, /* rot */
{ 2, 3, 0, 0 , 2, 3,0,N_rote, 1}, /* rot */
{ 2, 3, 0, 0 , 0, 3,0,N_rote, 1}, /* rot */
{ 2, 3, 0, 0 , 1, 3,0,N_rote, 1}, /* rot */
{ 2, 3, 0, 0 , 4, 2,0,N_rote, 1}, /* rot */
{ 2, 3, 0, 0 , 5, 3,0,N_rote, 1}, /* rot */
{ 1, 0, 1, 0 , 2, 1,0,N_store, 1}, /* ! */
{ 1, 0, 1, 0 , 3, 0,0,N_store, 1}, /* ! */
{ 1, 0, 1, 0 , 4, 2,0,N_store, 1}, /* ! */
{ 1, 0, 1, 0 , 5, 3,0,N_store, 1}, /* ! */
{ 1, 0, 1, 0 , 6, 4,0,N_store, 1}, /* ! */
{ 1, 0, 1, 0 , 7, 5,0,N_store, 1}, /* ! */
{ 1, 0, 1, 0 , 8, 6,0,N_store, 1}, /* ! */
{ 1, 4, 1, 0 , 2, 4,0,N_two_dupe, 1}, /* 2dup */
{ 1, 4, 1, 0 , 3, 5,0,N_two_dupe, 1}, /* 2dup */
{ 1, 4, 1, 0 , 4, 6,0,N_two_dupe, 1}, /* 2dup */
{ 1, 4, 1, 0 , 5, 7,0,N_two_dupe, 1}, /* 2dup */
{ 1, 4, 1, 0 , 6, 8,0,N_two_dupe, 1}, /* 2dup */
{ 1, 4, 1, 0 , 7, 8,0,N_two_dupe, 1}, /* 2dup */
{ 1, 4, 1, 0 , 8, 8,0,N_two_dupe, 1}, /* 2dup */
{ 1, 4, 1, 0 , 1, 0,0,N_two_dupe, 1}, /* 2dup */
{ 1, 4, 1, 0 , 1, 2,0,N_two_dupe, 1}, /* 2dup */
{ 1, 4, 1, 0 , 0, 3,0,N_two_dupe, 1}, /* 2dup */
{ 1, 4, 1, 0 , 2, 2,0,N_two_dupe, 1}, /* 2dup */
{ 1, 4, 1, 0 , 3, 0,0,N_two_dupe, 1}, /* 2dup */
{ 1, 4, 1, 0 , 4, 4,0,N_two_dupe, 1}, /* 2dup */
{ 1, 4, 1, 0 , 4, 2,0,N_two_dupe, 1}, /* 2dup */
{ 1, 1, 1, 0 , 2, 0,0,N_not_equals, 1}, /* <> */
{ 1, 1, 1, 0 , 3, 2,0,N_not_equals, 1}, /* <> */
{ 1, 1, 1, 0 , 4, 3,0,N_not_equals, 1}, /* <> */
{ 1, 1, 1, 0 , 5, 4,0,N_not_equals, 1}, /* <> */
{ 1, 1, 1, 0 , 6, 5,0,N_not_equals, 1}, /* <> */
{ 1, 1, 1, 0 , 7, 6,0,N_not_equals, 1}, /* <> */
{ 1, 1, 1, 0 , 8, 7,0,N_not_equals, 1}, /* <> */
{ 1, 0, 1, 0 , 2, 1,0,N_c_store, 1}, /* c! */
{ 1, 0, 1, 0 , 3, 0,0,N_c_store, 1}, /* c! */
{ 1, 0, 1, 0 , 4, 2,0,N_c_store, 1}, /* c! */
{ 1, 0, 1, 0 , 5, 3,0,N_c_store, 1}, /* c! */
{ 1, 0, 1, 0 , 6, 4,0,N_c_store, 1}, /* c! */
{ 1, 0, 1, 0 , 7, 5,0,N_c_store, 1}, /* c! */
{ 1, 0, 1, 0 , 8, 6,0,N_c_store, 1}, /* c! */
{ 1, 1, 0, 0 , 2, 2,1,N_lit_plus, 1}, /* lit+ */
{ 1, 1, 0, 0 , 3, 3,1,N_lit_plus, 1}, /* lit+ */
{ 1, 1, 0, 0 , 4, 4,1,N_lit_plus, 1}, /* lit+ */
{ 1, 1, 0, 0 , 5, 5,1,N_lit_plus, 1}, /* lit+ */
{ 1, 1, 0, 0 , 6, 6,1,N_lit_plus, 1}, /* lit+ */
{ 1, 1, 0, 0 , 7, 7,1,N_lit_plus, 1}, /* lit+ */
{ 1, 1, 0, 0 , 8, 8,1,N_lit_plus, 1}, /* lit+ */
{ 0, 0, 1, 0 , 0, 1,0,N_drop, 1}, /* drop */
{ 0, 0, 1, 0 , 2, 0,0,N_drop, 1}, /* drop */
{ 0, 0, 1, 0 , 3, 2,0,N_drop, 1}, /* drop */
{ 0, 0, 1, 0 , 4, 3,0,N_drop, 1}, /* drop */
{ 0, 0, 1, 0 , 5, 4,0,N_drop, 1}, /* drop */
{ 0, 0, 1, 0 , 6, 5,0,N_drop, 1}, /* drop */
{ 0, 0, 1, 0 , 7, 6,0,N_drop, 1}, /* drop */
{ 0, 0, 1, 0 , 8, 7,0,N_drop, 1}, /* drop */
{ 0, 0, 1, 0 , 1, 1,0,N_drop, 1}, /* drop */
{ 1, 1, 1, 0 , 2, 0,0,N_l_shift, 1}, /* lshift */
{ 1, 1, 1, 0 , 3, 2,0,N_l_shift, 1}, /* lshift */
{ 1, 1, 1, 0 , 4, 3,0,N_l_shift, 1}, /* lshift */
{ 1, 1, 1, 0 , 5, 4,0,N_l_shift, 1}, /* lshift */
{ 1, 1, 1, 0 , 6, 5,0,N_l_shift, 1}, /* lshift */
{ 1, 1, 1, 0 , 7, 6,0,N_l_shift, 1}, /* lshift */
{ 1, 1, 1, 0 , 8, 7,0,N_l_shift, 1}, /* lshift */
{ 1, 2, 0, 0 , 2, 2,0,N_slash_mod, 1}, /* /mod */
{ 1, 2, 0, 0 , 3, 3,0,N_slash_mod, 1}, /* /mod */
{ 1, 2, 0, 0 , 4, 4,0,N_slash_mod, 1}, /* /mod */
{ 1, 2, 0, 0 , 5, 5,0,N_slash_mod, 1}, /* /mod */
{ 1, 2, 0, 0 , 6, 6,0,N_slash_mod, 1}, /* /mod */
{ 1, 2, 0, 0 , 7, 7,0,N_slash_mod, 1}, /* /mod */
{ 1, 2, 0, 0 , 8, 8,0,N_slash_mod, 1}, /* /mod */
{ 0, 1, 2, 0 , 0, 1,0,N_to_r, 1}, /* >r */
{ 0, 1, 2, 0 , 2, 0,0,N_to_r, 1}, /* >r */
{ 0, 1, 2, 0 , 3, 2,0,N_to_r, 1}, /* >r */
{ 0, 1, 2, 0 , 4, 3,0,N_to_r, 1}, /* >r */
{ 0, 1, 2, 0 , 5, 4,0,N_to_r, 1}, /* >r */
{ 0, 1, 2, 0 , 6, 5,0,N_to_r, 1}, /* >r */
{ 0, 1, 2, 0 , 7, 6,0,N_to_r, 1}, /* >r */
{ 0, 1, 2, 0 , 8, 7,0,N_to_r, 1}, /* >r */
{ 0, 1, 2, 0 , 1, 0,0,N_r_from, 1}, /* r> */
{ 0, 1, 2, 0 , 0, 2,0,N_r_from, 1}, /* r> */
{ 0, 1, 2, 0 , 2, 3,0,N_r_from, 1}, /* r> */
{ 0, 1, 2, 0 , 3, 4,0,N_r_from, 1}, /* r> */
{ 0, 1, 2, 0 , 4, 5,0,N_r_from, 1}, /* r> */
{ 0, 1, 2, 0 , 5, 6,0,N_r_from, 1}, /* r> */
{ 0, 1, 2, 0 , 6, 7,0,N_r_from, 1}, /* r> */
{ 0, 1, 2, 0 , 7, 8,0,N_r_from, 1}, /* r> */
{ 0, 1, 2, 0 , 8, 8,0,N_r_from, 1}, /* r> */
{ 0, 1, 0, 0 , 2, 2,0,N_cell_plus, 1}, /* cell+ */
{ 0, 1, 0, 0 , 3, 3,0,N_cell_plus, 1}, /* cell+ */
{ 0, 1, 0, 0 , 4, 4,0,N_cell_plus, 1}, /* cell+ */
{ 0, 1, 0, 0 , 5, 5,0,N_cell_plus, 1}, /* cell+ */
{ 0, 1, 0, 0 , 6, 6,0,N_cell_plus, 1}, /* cell+ */
{ 0, 1, 0, 0 , 7, 7,0,N_cell_plus, 1}, /* cell+ */
{ 0, 1, 0, 0 , 8, 8,0,N_cell_plus, 1}, /* cell+ */
{ 0, 0, 0, 0 , 1, 1,1,N_lp_plus_store_number, 1}, /* lp+!# */
{ 0, 0, 0, 0 , 2, 2,1,N_lp_plus_store_number, 1}, /* lp+!# */
{ 0, 0, 0, 0 , 3, 3,1,N_lp_plus_store_number, 1}, /* lp+!# */
{ 0, 0, 0, 0 , 4, 4,1,N_lp_plus_store_number, 1}, /* lp+!# */
{ 0, 0, 0, 0 , 5, 5,1,N_lp_plus_store_number, 1}, /* lp+!# */
{ 0, 0, 0, 0 , 6, 6,1,N_lp_plus_store_number, 1}, /* lp+!# */
{ 0, 0, 0, 0 , 7, 7,1,N_lp_plus_store_number, 1}, /* lp+!# */
{ 0, 0, 0, 0 , 8, 8,1,N_lp_plus_store_number, 1}, /* lp+!# */
{ 1, 1, 1, 0 , 2, 0,0,N_minus, 1}, /* - */
{ 1, 1, 1, 0 , 3, 2,0,N_minus, 1}, /* - */
{ 1, 1, 1, 0 , 4, 3,0,N_minus, 1}, /* - */
{ 1, 1, 1, 0 , 5, 4,0,N_minus, 1}, /* - */
{ 1, 1, 1, 0 , 6, 5,0,N_minus, 1}, /* - */
{ 1, 1, 1, 0 , 7, 6,0,N_minus, 1}, /* - */
{ 1, 1, 1, 0 , 8, 7,0,N_minus, 1}, /* - */
{ 1, 0, 1, 0 , 1, 1,0,N_unloop, 1}, /* unloop */
{ 1, 0, 1, 0 , 2, 2,0,N_unloop, 1}, /* unloop */
{ 1, 0, 1, 0 , 3, 3,0,N_unloop, 1}, /* unloop */
{ 1, 0, 1, 0 , 4, 4,0,N_unloop, 1}, /* unloop */
{ 1, 0, 1, 0 , 5, 5,0,N_unloop, 1}, /* unloop */
{ 1, 0, 1, 0 , 6, 6,0,N_unloop, 1}, /* unloop */
{ 1, 0, 1, 0 , 7, 7,0,N_unloop, 1}, /* unloop */
{ 1, 0, 1, 0 , 8, 8,0,N_unloop, 1}, /* unloop */
{-1, 0, 0, 0 , 1, 1,0,N_eight_lp_plus_store, 1}, /* lp+ */
{-1, 0, 0, 0 , 2, 2,0,N_eight_lp_plus_store, 1}, /* lp+ */
{-1, 0, 0, 0 , 3, 3,0,N_eight_lp_plus_store, 1}, /* lp+ */
{-1, 0, 0, 0 , 4, 4,0,N_eight_lp_plus_store, 1}, /* lp+ */
{-1, 0, 0, 0 , 5, 5,0,N_eight_lp_plus_store, 1}, /* lp+ */
{-1, 0, 0, 0 , 6, 6,0,N_eight_lp_plus_store, 1}, /* lp+ */
{-1, 0, 0, 0 , 7, 7,0,N_eight_lp_plus_store, 1}, /* lp+ */
{-1, 0, 0, 0 , 8, 8,0,N_eight_lp_plus_store, 1}, /* lp+ */
{ 1, 1, 1, 0 , 2, 0,0,N_greater_or_equal, 1}, /* >= */
{ 1, 1, 1, 0 , 3, 2,0,N_greater_or_equal, 1}, /* >= */
{ 1, 1, 1, 0 , 4, 3,0,N_greater_or_equal, 1}, /* >= */
{ 1, 1, 1, 0 , 5, 4,0,N_greater_or_equal, 1}, /* >= */
{ 1, 1, 1, 0 , 6, 5,0,N_greater_or_equal, 1}, /* >= */
{ 1, 1, 1, 0 , 7, 6,0,N_greater_or_equal, 1}, /* >= */
{ 1, 1, 1, 0 , 8, 7,0,N_greater_or_equal, 1}, /* >= */
{ 1, 3, 1, 0 , 2, 3,0,N_tuck, 1}, /* tuck */
{ 1, 3, 1, 0 , 3, 4,0,N_tuck, 1}, /* tuck */
{ 1, 3, 1, 0 , 4, 5,0,N_tuck, 1}, /* tuck */
{ 1, 3, 1, 0 , 5, 6,0,N_tuck, 1}, /* tuck */
{ 1, 3, 1, 0 , 6, 7,0,N_tuck, 1}, /* tuck */
{ 1, 3, 1, 0 , 7, 8,0,N_tuck, 1}, /* tuck */
{ 1, 3, 1, 0 , 8, 8,0,N_tuck, 1}, /* tuck */
{ 1, 3, 1, 0 , 2, 2,0,N_tuck, 1}, /* tuck */
{ 1, 3, 1, 0 , 3, 3,0,N_tuck, 1}, /* tuck */
{ 2, 2, 1, 0 , 3, 2,0,N_slash_string, 1}, /* /string */
{ 2, 2, 1, 0 , 4, 3,0,N_slash_string, 1}, /* /string */
{ 2, 2, 1, 0 , 5, 4,0,N_slash_string, 1}, /* /string */
{ 2, 2, 1, 0 , 6, 5,0,N_slash_string, 1}, /* /string */
{ 2, 2, 1, 0 , 7, 6,0,N_slash_string, 1}, /* /string */
{ 2, 2, 1, 0 , 8, 7,0,N_slash_string, 1}, /* /string */
{ 0, 1, 0, 0 , 2, 2,0,N_char_plus, 1}, /* char+ */
{ 0, 1, 0, 0 , 3, 3,0,N_char_plus, 1}, /* char+ */
{ 0, 1, 0, 0 , 4, 4,0,N_char_plus, 1}, /* char+ */
{ 0, 1, 0, 0 , 5, 5,0,N_char_plus, 1}, /* char+ */
{ 0, 1, 0, 0 , 6, 6,0,N_char_plus, 1}, /* char+ */
{ 0, 1, 0, 0 , 7, 7,0,N_char_plus, 1}, /* char+ */
{ 0, 1, 0, 0 , 8, 8,0,N_char_plus, 1}, /* char+ */
{ 0, 1, 0, 0 , 2, 0,0,N_question_dupe, 1}, /* ?dup */
{ 0, 1, 0, 0 , 3, 0,0,N_question_dupe, 1}, /* ?dup */
{ 0, 1, 0, 0 , 4, 0,0,N_question_dupe, 1}, /* ?dup */
{ 0, 1, 0, 0 , 5, 0,0,N_question_dupe, 1}, /* ?dup */
{ 0, 1, 0, 0 , 6, 0,0,N_question_dupe, 1}, /* ?dup */
{ 0, 1, 0, 0 , 7, 0,0,N_question_dupe, 1}, /* ?dup */
{ 0, 1, 0, 0 , 8, 0,0,N_question_dupe, 1}, /* ?dup */
{-1, 1, 1, 0 , 1, 0,0,N_fetch_local_zero, 1}, /* @local0 */
{-1, 1, 1, 0 , 0, 2,0,N_fetch_local_zero, 1}, /* @local0 */
{-1, 1, 1, 0 , 2, 3,0,N_fetch_local_zero, 1}, /* @local0 */
{-1, 1, 1, 0 , 3, 4,0,N_fetch_local_zero, 1}, /* @local0 */
{-1, 1, 1, 0 , 4, 5,0,N_fetch_local_zero, 1}, /* @local0 */
{-1, 1, 1, 0 , 5, 6,0,N_fetch_local_zero, 1}, /* @local0 */
{-1, 1, 1, 0 , 6, 7,0,N_fetch_local_zero, 1}, /* @local0 */
{-1, 1, 1, 0 , 7, 8,0,N_fetch_local_zero, 1}, /* @local0 */
{-1, 1, 1, 0 , 8, 8,0,N_fetch_local_zero, 1}, /* @local0 */
{ 1, 1, 1, 0 , 2, 0,0,N_min, 1}, /* min */
{ 1, 1, 1, 0 , 3, 2,0,N_min, 1}, /* min */
{ 1, 1, 1, 0 , 4, 3,0,N_min, 1}, /* min */
{ 1, 1, 1, 0 , 5, 4,0,N_min, 1}, /* min */
{ 1, 1, 1, 0 , 6, 5,0,N_min, 1}, /* min */
{ 1, 1, 1, 0 , 7, 6,0,N_min, 1}, /* min */
{ 1, 1, 1, 0 , 8, 7,0,N_min, 1}, /* min */
{ 2, 2, 2, 1 , 2, 0,1,N_paren_u_plus_do, 1}, /* (u+do) */
{ 2, 2, 2, 1 , 3, 0,1,N_paren_u_plus_do, 1}, /* (u+do) */
{ 2, 2, 2, 1 , 4, 0,1,N_paren_u_plus_do, 1}, /* (u+do) */
{ 2, 2, 2, 1 , 5, 0,1,N_paren_u_plus_do, 1}, /* (u+do) */
{ 2, 2, 2, 1 , 6, 0,1,N_paren_u_plus_do, 1}, /* (u+do) */
{ 2, 2, 2, 1 , 7, 0,1,N_paren_u_plus_do, 1}, /* (u+do) */
{ 2, 2, 2, 1 , 8, 0,1,N_paren_u_plus_do, 1}, /* (u+do) */
{ 1, 2, 2, 0 , 2, 1,0,N_two_to_r, 1}, /* 2>r */
{ 1, 2, 2, 0 , 3, 0,0,N_two_to_r, 1}, /* 2>r */
{ 1, 2, 2, 0 , 4, 2,0,N_two_to_r, 1}, /* 2>r */
{ 1, 2, 2, 0 , 5, 3,0,N_two_to_r, 1}, /* 2>r */
{ 1, 2, 2, 0 , 6, 4,0,N_two_to_r, 1}, /* 2>r */
{ 1, 2, 2, 0 , 7, 5,0,N_two_to_r, 1}, /* 2>r */
{ 1, 2, 2, 0 , 8, 6,0,N_two_to_r, 1}, /* 2>r */
{ 1, 2, 2, 0 , 1, 2,0,N_two_r_from, 1}, /* 2r> */
{ 1, 2, 2, 0 , 0, 3,0,N_two_r_from, 1}, /* 2r> */
{ 1, 2, 2, 0 , 2, 4,0,N_two_r_from, 1}, /* 2r> */
{ 1, 2, 2, 0 , 3, 5,0,N_two_r_from, 1}, /* 2r> */
{ 1, 2, 2, 0 , 4, 6,0,N_two_r_from, 1}, /* 2r> */
{ 1, 2, 2, 0 , 5, 7,0,N_two_r_from, 1}, /* 2r> */
{ 1, 2, 2, 0 , 6, 8,0,N_two_r_from, 1}, /* 2r> */
{ 1, 2, 2, 0 , 7, 8,0,N_two_r_from, 1}, /* 2r> */
{ 1, 2, 2, 0 , 8, 8,0,N_two_r_from, 1}, /* 2r> */
{ 1, 4, 1, 0 , 1, 2,0,N_two_r_fetch, 1}, /* 2r@ */
{ 1, 4, 1, 0 , 0, 3,0,N_two_r_fetch, 1}, /* 2r@ */
{ 1, 4, 1, 0 , 2, 4,0,N_two_r_fetch, 1}, /* 2r@ */
{ 1, 4, 1, 0 , 3, 5,0,N_two_r_fetch, 1}, /* 2r@ */
{ 1, 4, 1, 0 , 4, 6,0,N_two_r_fetch, 1}, /* 2r@ */
{ 1, 4, 1, 0 , 5, 7,0,N_two_r_fetch, 1}, /* 2r@ */
{ 1, 4, 1, 0 , 6, 8,0,N_two_r_fetch, 1}, /* 2r@ */
{ 1, 4, 1, 0 , 7, 8,0,N_two_r_fetch, 1}, /* 2r@ */
{ 1, 4, 1, 0 , 8, 8,0,N_two_r_fetch, 1}, /* 2r@ */
{ 0, 2, 1, 0 , 0, 2,0,N_two_fetch, 1}, /* 2@ */
{ 0, 2, 1, 0 , 2, 3,0,N_two_fetch, 1}, /* 2@ */
{ 0, 2, 1, 0 , 3, 4,0,N_two_fetch, 1}, /* 2@ */
{ 0, 2, 1, 0 , 4, 5,0,N_two_fetch, 1}, /* 2@ */
{ 0, 2, 1, 0 , 5, 6,0,N_two_fetch, 1}, /* 2@ */
{ 0, 2, 1, 0 , 6, 7,0,N_two_fetch, 1}, /* 2@ */
{ 0, 2, 1, 0 , 7, 8,0,N_two_fetch, 1}, /* 2@ */
{ 0, 2, 1, 0 , 8, 8,0,N_two_fetch, 1}, /* 2@ */
{ 1, 1, 1, 0 , 2, 0,0,N_nip, 1}, /* nip */
{ 1, 1, 1, 0 , 3, 2,0,N_nip, 1}, /* nip */
{ 1, 1, 1, 0 , 4, 3,0,N_nip, 1}, /* nip */
{ 1, 1, 1, 0 , 5, 4,0,N_nip, 1}, /* nip */
{ 1, 1, 1, 0 , 6, 5,0,N_nip, 1}, /* nip */
{ 1, 1, 1, 0 , 7, 6,0,N_nip, 1}, /* nip */
{ 1, 1, 1, 0 , 8, 7,0,N_nip, 1}, /* nip */
{ 1, 1, 1, 0 , 1, 0,0,N_nip, 1}, /* nip */
{ 1, 1, 1, 0 , 2, 2,0,N_nip, 1}, /* nip */
{ 2, 1, 1, 0 , 3, 0,0,N_within, 1}, /* within */
{ 2, 1, 1, 0 , 4, 2,0,N_within, 1}, /* within */
{ 2, 1, 1, 0 , 5, 3,0,N_within, 1}, /* within */
{ 2, 1, 1, 0 , 6, 4,0,N_within, 1}, /* within */
{ 2, 1, 1, 0 , 7, 5,0,N_within, 1}, /* within */
{ 2, 1, 1, 0 , 8, 6,0,N_within, 1}, /* within */
{ 0, 1, 0, 0 , 2, 2,0,N_one_minus, 1}, /* 1- */
{ 0, 1, 0, 0 , 3, 3,0,N_one_minus, 1}, /* 1- */
{ 0, 1, 0, 0 , 4, 4,0,N_one_minus, 1}, /* 1- */
{ 0, 1, 0, 0 , 5, 5,0,N_one_minus, 1}, /* 1- */
{ 0, 1, 0, 0 , 6, 6,0,N_one_minus, 1}, /* 1- */
{ 0, 1, 0, 0 , 7, 7,0,N_one_minus, 1}, /* 1- */
{ 0, 1, 0, 0 , 8, 8,0,N_one_minus, 1}, /* 1- */
{ 0, 0, 1, 0 , 2, 0,0,N_execute, 1}, /* execute */
{ 0, 0, 1, 0 , 3, 0,0,N_execute, 1}, /* execute */
{ 0, 0, 1, 0 , 4, 0,0,N_execute, 1}, /* execute */
{ 0, 0, 1, 0 , 5, 0,0,N_execute, 1}, /* execute */
{ 0, 0, 1, 0 , 6, 0,0,N_execute, 1}, /* execute */
{ 0, 0, 1, 0 , 7, 0,0,N_execute, 1}, /* execute */
{ 0, 0, 1, 0 , 8, 0,0,N_execute, 1}, /* execute */
{-1, 0, 0, 0 , 1, 1,0,N_minus_four_lp_plus_store, 1}, /* lp- */
{-1, 0, 0, 0 , 2, 2,0,N_minus_four_lp_plus_store, 1}, /* lp- */
{-1, 0, 0, 0 , 3, 3,0,N_minus_four_lp_plus_store, 1}, /* lp- */
{-1, 0, 0, 0 , 4, 4,0,N_minus_four_lp_plus_store, 1}, /* lp- */
{-1, 0, 0, 0 , 5, 5,0,N_minus_four_lp_plus_store, 1}, /* lp- */
{-1, 0, 0, 0 , 6, 6,0,N_minus_four_lp_plus_store, 1}, /* lp- */
{-1, 0, 0, 0 , 7, 7,0,N_minus_four_lp_plus_store, 1}, /* lp- */
{-1, 0, 0, 0 , 8, 8,0,N_minus_four_lp_plus_store, 1}, /* lp- */
{ 0, 0, 1, 0 , 2, 0,0,N_perform, 1}, /* perform */
{ 0, 0, 1, 0 , 3, 0,0,N_perform, 1}, /* perform */
{ 0, 0, 1, 0 , 4, 0,0,N_perform, 1}, /* perform */
{ 0, 0, 1, 0 , 5, 0,0,N_perform, 1}, /* perform */
{ 0, 0, 1, 0 , 6, 0,0,N_perform, 1}, /* perform */
{ 0, 0, 1, 0 , 7, 0,0,N_perform, 1}, /* perform */
{ 0, 0, 1, 0 , 8, 0,0,N_perform, 1}, /* perform */
{ 1, 1, 1, 0 , 2, 0,0,N_x_or, 1}, /* xor */
{ 1, 1, 1, 0 , 3, 2,0,N_x_or, 1}, /* xor */
{ 1, 1, 1, 0 , 4, 3,0,N_x_or, 1}, /* xor */
{ 1, 1, 1, 0 , 5, 4,0,N_x_or, 1}, /* xor */
{ 1, 1, 1, 0 , 6, 5,0,N_x_or, 1}, /* xor */
{ 1, 1, 1, 0 , 7, 6,0,N_x_or, 1}, /* xor */
{ 1, 1, 1, 0 , 8, 7,0,N_x_or, 1}, /* xor */
{ 0, 1, 1, 0 , 1, 0,1,N_fetch_local_number, 1}, /* @local# */
{ 0, 1, 1, 0 , 0, 2,1,N_fetch_local_number, 1}, /* @local# */
{ 0, 1, 1, 0 , 2, 3,1,N_fetch_local_number, 1}, /* @local# */
{ 0, 1, 1, 0 , 3, 4,1,N_fetch_local_number, 1}, /* @local# */
{ 0, 1, 1, 0 , 4, 5,1,N_fetch_local_number, 1}, /* @local# */
{ 0, 1, 1, 0 , 5, 6,1,N_fetch_local_number, 1}, /* @local# */
{ 0, 1, 1, 0 , 6, 7,1,N_fetch_local_number, 1}, /* @local# */
{ 0, 1, 1, 0 , 7, 8,1,N_fetch_local_number, 1}, /* @local# */
{ 0, 1, 1, 0 , 8, 8,1,N_fetch_local_number, 1}, /* @local# */
{ 1, 1, 1, 0 , 2, 0,0,N_u_greater_than, 1}, /* u> */
{ 1, 1, 1, 0 , 3, 2,0,N_u_greater_than, 1}, /* u> */
{ 1, 1, 1, 0 , 4, 3,0,N_u_greater_than, 1}, /* u> */
{ 1, 1, 1, 0 , 5, 4,0,N_u_greater_than, 1}, /* u> */
{ 1, 1, 1, 0 , 6, 5,0,N_u_greater_than, 1}, /* u> */
{ 1, 1, 1, 0 , 7, 6,0,N_u_greater_than, 1}, /* u> */
{ 1, 1, 1, 0 , 8, 7,0,N_u_greater_than, 1}, /* u> */
{ 2, 3, 0, 0 , 3, 3,0,N_not_rote, 1}, /* -rot */
{ 2, 3, 0, 0 , 4, 4,0,N_not_rote, 1}, /* -rot */
{ 2, 3, 0, 0 , 5, 5,0,N_not_rote, 1}, /* -rot */
{ 2, 3, 0, 0 , 6, 6,0,N_not_rote, 1}, /* -rot */
{ 2, 3, 0, 0 , 7, 7,0,N_not_rote, 1}, /* -rot */
{ 2, 3, 0, 0 , 8, 8,0,N_not_rote, 1}, /* -rot */
{ 2, 3, 0, 0 , 3, 2,0,N_not_rote, 1}, /* -rot */
{ 2, 3, 0, 0 , 0, 3,0,N_not_rote, 1}, /* -rot */
{ 2, 3, 0, 0 , 4, 3,0,N_not_rote, 1}, /* -rot */
{ 2, 3, 0, 0 , 2, 4,0,N_not_rote, 1}, /* -rot */
{ 2, 3, 0, 0 , 3, 5,0,N_not_rote, 1}, /* -rot */
{ 1, 0, 1, 0 , 2, 1,0,N_two_drop, 1}, /* 2drop */
{ 1, 0, 1, 0 , 3, 0,0,N_two_drop, 1}, /* 2drop */
{ 1, 0, 1, 0 , 4, 2,0,N_two_drop, 1}, /* 2drop */
{ 1, 0, 1, 0 , 5, 3,0,N_two_drop, 1}, /* 2drop */
{ 1, 0, 1, 0 , 6, 4,0,N_two_drop, 1}, /* 2drop */
{ 1, 0, 1, 0 , 7, 5,0,N_two_drop, 1}, /* 2drop */
{ 1, 0, 1, 0 , 8, 6,0,N_two_drop, 1}, /* 2drop */
{ 1, 0, 1, 0 , 1, 1,0,N_two_drop, 1}, /* 2drop */
{ 1, 0, 1, 0 , 0, 1,0,N_two_drop, 1}, /* 2drop */
{ 1, 1, 1, 0 , 2, 0,0,N_or, 1}, /* or */
{ 1, 1, 1, 0 , 3, 2,0,N_or, 1}, /* or */
{ 1, 1, 1, 0 , 4, 3,0,N_or, 1}, /* or */
{ 1, 1, 1, 0 , 5, 4,0,N_or, 1}, /* or */
{ 1, 1, 1, 0 , 6, 5,0,N_or, 1}, /* or */
{ 1, 1, 1, 0 , 7, 6,0,N_or, 1}, /* or */
{ 1, 1, 1, 0 , 8, 7,0,N_or, 1}, /* or */
{-1, 1, 1, 0 , 1, 0,0,N_fetch_local_eight, 1}, /* @local2 */
{-1, 1, 1, 0 , 0, 2,0,N_fetch_local_eight, 1}, /* @local2 */
{-1, 1, 1, 0 , 2, 3,0,N_fetch_local_eight, 1}, /* @local2 */
{-1, 1, 1, 0 , 3, 4,0,N_fetch_local_eight, 1}, /* @local2 */
{-1, 1, 1, 0 , 4, 5,0,N_fetch_local_eight, 1}, /* @local2 */
{-1, 1, 1, 0 , 5, 6,0,N_fetch_local_eight, 1}, /* @local2 */
{-1, 1, 1, 0 , 6, 7,0,N_fetch_local_eight, 1}, /* @local2 */
{-1, 1, 1, 0 , 7, 8,0,N_fetch_local_eight, 1}, /* @local2 */
{-1, 1, 1, 0 , 8, 8,0,N_fetch_local_eight, 1}, /* @local2 */
{ 3, 2, 1, 1 , 2, 0,1,N_paren_plus_loop, 1}, /* (+loop) */
{ 3, 2, 1, 1 , 3, 0,1,N_paren_plus_loop, 1}, /* (+loop) */
{ 3, 2, 1, 1 , 4, 0,1,N_paren_plus_loop, 1}, /* (+loop) */
{ 3, 2, 1, 1 , 5, 0,1,N_paren_plus_loop, 1}, /* (+loop) */
{ 3, 2, 1, 1 , 6, 0,1,N_paren_plus_loop, 1}, /* (+loop) */
{ 3, 2, 1, 1 , 7, 0,1,N_paren_plus_loop, 1}, /* (+loop) */
{ 3, 2, 1, 1 , 8, 0,1,N_paren_plus_loop, 1}, /* (+loop) */
{ 0, 1, 0, 0 , 2, 2,0,N_one_plus, 1}, /* 1+ */
{ 0, 1, 0, 0 , 3, 3,0,N_one_plus, 1}, /* 1+ */
{ 0, 1, 0, 0 , 4, 4,0,N_one_plus, 1}, /* 1+ */
{ 0, 1, 0, 0 , 5, 5,0,N_one_plus, 1}, /* 1+ */
{ 0, 1, 0, 0 , 6, 6,0,N_one_plus, 1}, /* 1+ */
{ 0, 1, 0, 0 , 7, 7,0,N_one_plus, 1}, /* 1+ */
{ 0, 1, 0, 0 , 8, 8,0,N_one_plus, 1}, /* 1+ */
{ 0, 0, 1, 0 , 1, 1,0,N_rdrop, 1}, /* rdrop */
{ 0, 0, 1, 0 , 2, 2,0,N_rdrop, 1}, /* rdrop */
{ 0, 0, 1, 0 , 3, 3,0,N_rdrop, 1}, /* rdrop */
{ 0, 0, 1, 0 , 4, 4,0,N_rdrop, 1}, /* rdrop */
{ 0, 0, 1, 0 , 5, 5,0,N_rdrop, 1}, /* rdrop */
{ 0, 0, 1, 0 , 6, 6,0,N_rdrop, 1}, /* rdrop */
{ 0, 0, 1, 0 , 7, 7,0,N_rdrop, 1}, /* rdrop */
{ 0, 0, 1, 0 , 8, 8,0,N_rdrop, 1}, /* rdrop */
{ 2, 2, 2, 1 , 2, 0,1,N_paren_question_do, 1}, /* (?do) */
{ 2, 2, 2, 1 , 3, 0,1,N_paren_question_do, 1}, /* (?do) */
{ 2, 2, 2, 1 , 4, 0,1,N_paren_question_do, 1}, /* (?do) */
{ 2, 2, 2, 1 , 5, 0,1,N_paren_question_do, 1}, /* (?do) */
{ 2, 2, 2, 1 , 6, 0,1,N_paren_question_do, 1}, /* (?do) */
{ 2, 2, 2, 1 , 7, 0,1,N_paren_question_do, 1}, /* (?do) */
{ 2, 2, 2, 1 , 8, 0,1,N_paren_question_do, 1}, /* (?do) */
{ 1, 1, 1, 0 , 2, 0,0,N_max, 1}, /* max */
{ 1, 1, 1, 0 , 3, 2,0,N_max, 1}, /* max */
{ 1, 1, 1, 0 , 4, 3,0,N_max, 1}, /* max */
{ 1, 1, 1, 0 , 5, 4,0,N_max, 1}, /* max */
{ 1, 1, 1, 0 , 6, 5,0,N_max, 1}, /* max */
{ 1, 1, 1, 0 , 7, 6,0,N_max, 1}, /* max */
{ 1, 1, 1, 0 , 8, 7,0,N_max, 1}, /* max */
{ 0, 1, 0, 0 , 2, 2,0,N_two_star, 1}, /* 2* */
{ 0, 1, 0, 0 , 3, 3,0,N_two_star, 1}, /* 2* */
{ 0, 1, 0, 0 , 4, 4,0,N_two_star, 1}, /* 2* */
{ 0, 1, 0, 0 , 5, 5,0,N_two_star, 1}, /* 2* */
{ 0, 1, 0, 0 , 6, 6,0,N_two_star, 1}, /* 2* */
{ 0, 1, 0, 0 , 7, 7,0,N_two_star, 1}, /* 2* */
{ 0, 1, 0, 0 , 8, 8,0,N_two_star, 1}, /* 2* */
{ 2, 0, 1, 0 , 3, 1,0,N_two_store, 1}, /* 2! */
{ 2, 0, 1, 0 , 4, 0,0,N_two_store, 1}, /* 2! */
{ 2, 0, 1, 0 , 5, 2,0,N_two_store, 1}, /* 2! */
{ 2, 0, 1, 0 , 6, 3,0,N_two_store, 1}, /* 2! */
{ 2, 0, 1, 0 , 7, 4,0,N_two_store, 1}, /* 2! */
{ 2, 0, 1, 0 , 8, 5,0,N_two_store, 1}, /* 2! */
{ 1, 2, 2, 0 , 2, 0,0,N_paren_do, 1}, /* (do) */
{ 1, 2, 2, 0 , 3, 0,0,N_paren_do, 1}, /* (do) */
{ 1, 2, 2, 0 , 4, 0,0,N_paren_do, 1}, /* (do) */
{ 1, 2, 2, 0 , 5, 0,0,N_paren_do, 1}, /* (do) */
{ 1, 2, 2, 0 , 6, 0,0,N_paren_do, 1}, /* (do) */
{ 1, 2, 2, 0 , 7, 0,0,N_paren_do, 1}, /* (do) */
{ 1, 2, 2, 0 , 8, 0,0,N_paren_do, 1}, /* (do) */
{ 1, 1, 1, 0 , 2, 0,0,N_u_less_than, 1}, /* u< */
{ 1, 1, 1, 0 , 3, 2,0,N_u_less_than, 1}, /* u< */
{ 1, 1, 1, 0 , 4, 3,0,N_u_less_than, 1}, /* u< */
{ 1, 1, 1, 0 , 5, 4,0,N_u_less_than, 1}, /* u< */
{ 1, 1, 1, 0 , 6, 5,0,N_u_less_than, 1}, /* u< */
{ 1, 1, 1, 0 , 7, 6,0,N_u_less_than, 1}, /* u< */
{ 1, 1, 1, 0 , 8, 7,0,N_u_less_than, 1}, /* u< */
{ 0, 2, 1, 0 , 0, 2,0,N_count, 1}, /* count */
{ 0, 2, 1, 0 , 2, 3,0,N_count, 1}, /* count */
{ 0, 2, 1, 0 , 3, 4,0,N_count, 1}, /* count */
{ 0, 2, 1, 0 , 4, 5,0,N_count, 1}, /* count */
{ 0, 2, 1, 0 , 5, 6,0,N_count, 1}, /* count */
{ 0, 2, 1, 0 , 6, 7,0,N_count, 1}, /* count */
{ 0, 2, 1, 0 , 7, 8,0,N_count, 1}, /* count */
{ 0, 2, 1, 0 , 8, 8,0,N_count, 1}, /* count */
{ 0, 1, 0, 0 , 2, 2,0,N_zero_less_than, 1}, /* 0< */
{ 0, 1, 0, 0 , 3, 3,0,N_zero_less_than, 1}, /* 0< */
{ 0, 1, 0, 0 , 4, 4,0,N_zero_less_than, 1}, /* 0< */
{ 0, 1, 0, 0 , 5, 5,0,N_zero_less_than, 1}, /* 0< */
{ 0, 1, 0, 0 , 6, 6,0,N_zero_less_than, 1}, /* 0< */
{ 0, 1, 0, 0 , 7, 7,0,N_zero_less_than, 1}, /* 0< */
{ 0, 1, 0, 0 , 8, 8,0,N_zero_less_than, 1}, /* 0< */
{ 1, 1, 1, 0 , 2, 0,0,N_less_than, 1}, /* < */
{ 1, 1, 1, 0 , 3, 2,0,N_less_than, 1}, /* < */
{ 1, 1, 1, 0 , 4, 3,0,N_less_than, 1}, /* < */
{ 1, 1, 1, 0 , 5, 4,0,N_less_than, 1}, /* < */
{ 1, 1, 1, 0 , 6, 5,0,N_less_than, 1}, /* < */
{ 1, 1, 1, 0 , 7, 6,0,N_less_than, 1}, /* < */
{ 1, 1, 1, 0 , 8, 7,0,N_less_than, 1}, /* < */
{-1, 1, 1, 0 , 1, 0,0,N_fetch_local_four, 1}, /* @local1 */
{-1, 1, 1, 0 , 0, 2,0,N_fetch_local_four, 1}, /* @local1 */
{-1, 1, 1, 0 , 2, 3,0,N_fetch_local_four, 1}, /* @local1 */
{-1, 1, 1, 0 , 3, 4,0,N_fetch_local_four, 1}, /* @local1 */
{-1, 1, 1, 0 , 4, 5,0,N_fetch_local_four, 1}, /* @local1 */
{-1, 1, 1, 0 , 5, 6,0,N_fetch_local_four, 1}, /* @local1 */
{-1, 1, 1, 0 , 6, 7,0,N_fetch_local_four, 1}, /* @local1 */
{-1, 1, 1, 0 , 7, 8,0,N_fetch_local_four, 1}, /* @local1 */
{-1, 1, 1, 0 , 8, 8,0,N_fetch_local_four, 1}, /* @local1 */
{ 1, 1, 1, 0 , 2, 0,0,N_u_greater_or_equal, 1}, /* u>= */
{ 1, 1, 1, 0 , 3, 2,0,N_u_greater_or_equal, 1}, /* u>= */
{ 1, 1, 1, 0 , 4, 3,0,N_u_greater_or_equal, 1}, /* u>= */
{ 1, 1, 1, 0 , 5, 4,0,N_u_greater_or_equal, 1}, /* u>= */
{ 1, 1, 1, 0 , 6, 5,0,N_u_greater_or_equal, 1}, /* u>= */
{ 1, 1, 1, 0 , 7, 6,0,N_u_greater_or_equal, 1}, /* u>= */
{ 1, 1, 1, 0 , 8, 7,0,N_u_greater_or_equal, 1}, /* u>= */
{ 2, 2, 2, 1 , 2, 0,1,N_paren_plus_do, 1}, /* (+do) */
{ 2, 2, 2, 1 , 3, 0,1,N_paren_plus_do, 1}, /* (+do) */
{ 2, 2, 2, 1 , 4, 0,1,N_paren_plus_do, 1}, /* (+do) */
{ 2, 2, 2, 1 , 5, 0,1,N_paren_plus_do, 1}, /* (+do) */
{ 2, 2, 2, 1 , 6, 0,1,N_paren_plus_do, 1}, /* (+do) */
{ 2, 2, 2, 1 , 7, 0,1,N_paren_plus_do, 1}, /* (+do) */
{ 2, 2, 2, 1 , 8, 0,1,N_paren_plus_do, 1}, /* (+do) */
{ 1, 2, 0, 0 , 2, 2,0,N_u_m_star, 1}, /* um* */
{ 1, 2, 0, 0 , 3, 3,0,N_u_m_star, 1}, /* um* */
{ 1, 2, 0, 0 , 4, 4,0,N_u_m_star, 1}, /* um* */
{ 1, 2, 0, 0 , 5, 5,0,N_u_m_star, 1}, /* um* */
{ 1, 2, 0, 0 , 6, 6,0,N_u_m_star, 1}, /* um* */
{ 1, 2, 0, 0 , 7, 7,0,N_u_m_star, 1}, /* um* */
{ 1, 2, 0, 0 , 8, 8,0,N_u_m_star, 1}, /* um* */
{ 3, 2, 1, 0 , 4, 2,0,N_d_plus, 1}, /* d+ */
{ 3, 2, 1, 0 , 5, 3,0,N_d_plus, 1}, /* d+ */
{ 3, 2, 1, 0 , 6, 4,0,N_d_plus, 1}, /* d+ */
{ 3, 2, 1, 0 , 7, 5,0,N_d_plus, 1}, /* d+ */
{ 3, 2, 1, 0 , 8, 6,0,N_d_plus, 1}, /* d+ */
{ 1, 1, 1, 0 , 2, 0,0,N_star, 1}, /* * */
{ 1, 1, 1, 0 , 3, 2,0,N_star, 1}, /* * */
{ 1, 1, 1, 0 , 4, 3,0,N_star, 1}, /* * */
{ 1, 1, 1, 0 , 5, 4,0,N_star, 1}, /* * */
{ 1, 1, 1, 0 , 6, 5,0,N_star, 1}, /* * */
{ 1, 1, 1, 0 , 7, 6,0,N_star, 1}, /* * */
{ 1, 1, 1, 0 , 8, 7,0,N_star, 1}, /* * */
{ 0, 1, 0, 0 , 2, 2,0,N_zero_not_equals, 1}, /* 0<> */
{ 0, 1, 0, 0 , 3, 3,0,N_zero_not_equals, 1}, /* 0<> */
{ 0, 1, 0, 0 , 4, 4,0,N_zero_not_equals, 1}, /* 0<> */
{ 0, 1, 0, 0 , 5, 5,0,N_zero_not_equals, 1}, /* 0<> */
{ 0, 1, 0, 0 , 6, 6,0,N_zero_not_equals, 1}, /* 0<> */
{ 0, 1, 0, 0 , 7, 7,0,N_zero_not_equals, 1}, /* 0<> */
{ 0, 1, 0, 0 , 8, 8,0,N_zero_not_equals, 1}, /* 0<> */
{ 0, 1, 0, 0 , 2, 2,0,N_aligned, 1}, /* aligned */
{ 0, 1, 0, 0 , 3, 3,0,N_aligned, 1}, /* aligned */
{ 0, 1, 0, 0 , 4, 4,0,N_aligned, 1}, /* aligned */
{ 0, 1, 0, 0 , 5, 5,0,N_aligned, 1}, /* aligned */
{ 0, 1, 0, 0 , 6, 6,0,N_aligned, 1}, /* aligned */
{ 0, 1, 0, 0 , 7, 7,0,N_aligned, 1}, /* aligned */
{ 0, 1, 0, 0 , 8, 8,0,N_aligned, 1}, /* aligned */
{ 1, 1, 1, 0 , 2, 0,0,N_u_less_or_equal, 1}, /* u<= */
{ 1, 1, 1, 0 , 3, 2,0,N_u_less_or_equal, 1}, /* u<= */
{ 1, 1, 1, 0 , 4, 3,0,N_u_less_or_equal, 1}, /* u<= */
{ 1, 1, 1, 0 , 5, 4,0,N_u_less_or_equal, 1}, /* u<= */
{ 1, 1, 1, 0 , 6, 5,0,N_u_less_or_equal, 1}, /* u<= */
{ 1, 1, 1, 0 , 7, 6,0,N_u_less_or_equal, 1}, /* u<= */
{ 1, 1, 1, 0 , 8, 7,0,N_u_less_or_equal, 1}, /* u<= */
{ 1, 3, 1, 0 , 1, 0,0,N_i_tick, 1}, /* i' */
{ 1, 3, 1, 0 , 0, 2,0,N_i_tick, 1}, /* i' */
{ 1, 3, 1, 0 , 2, 3,0,N_i_tick, 1}, /* i' */
{ 1, 3, 1, 0 , 3, 4,0,N_i_tick, 1}, /* i' */
{ 1, 3, 1, 0 , 4, 5,0,N_i_tick, 1}, /* i' */
{ 1, 3, 1, 0 , 5, 6,0,N_i_tick, 1}, /* i' */
{ 1, 3, 1, 0 , 6, 7,0,N_i_tick, 1}, /* i' */
{ 1, 3, 1, 0 , 7, 8,0,N_i_tick, 1}, /* i' */
{ 1, 3, 1, 0 , 8, 8,0,N_i_tick, 1}, /* i' */
{ 1, 1, 1, 0 , 2, 0,0,N_greater_than, 1}, /* > */
{ 1, 1, 1, 0 , 3, 2,0,N_greater_than, 1}, /* > */
{ 1, 1, 1, 0 , 4, 3,0,N_greater_than, 1}, /* > */
{ 1, 1, 1, 0 , 5, 4,0,N_greater_than, 1}, /* > */
{ 1, 1, 1, 0 , 6, 5,0,N_greater_than, 1}, /* > */
{ 1, 1, 1, 0 , 7, 6,0,N_greater_than, 1}, /* > */
{ 1, 1, 1, 0 , 8, 7,0,N_greater_than, 1}, /* > */
{ 3, 4, 0, 0 , 4, 4,0,N_two_swap, 1}, /* 2swap */
{ 3, 4, 0, 0 , 5, 5,0,N_two_swap, 1}, /* 2swap */
{ 3, 4, 0, 0 , 6, 6,0,N_two_swap, 1}, /* 2swap */
{ 3, 4, 0, 0 , 7, 7,0,N_two_swap, 1}, /* 2swap */
{ 3, 4, 0, 0 , 8, 8,0,N_two_swap, 1}, /* 2swap */
{ 3, 4, 0, 0 , 1, 3,0,N_two_swap, 1}, /* 2swap */
{ 3, 4, 0, 0 , 0, 3,0,N_two_swap, 1}, /* 2swap */
{ 3, 4, 0, 0 , 2, 4,0,N_two_swap, 1}, /* 2swap */
{ 3, 4, 0, 0 , 4, 2,0,N_two_swap, 1}, /* 2swap */
{ 0, 1, 1, 0 , 1, 0,1,N_laddr_number, 1}, /* laddr# */
{ 0, 1, 1, 0 , 0, 2,1,N_laddr_number, 1}, /* laddr# */
{ 0, 1, 1, 0 , 2, 3,1,N_laddr_number, 1}, /* laddr# */
{ 0, 1, 1, 0 , 3, 4,1,N_laddr_number, 1}, /* laddr# */
{ 0, 1, 1, 0 , 4, 5,1,N_laddr_number, 1}, /* laddr# */
{ 0, 1, 1, 0 , 5, 6,1,N_laddr_number, 1}, /* laddr# */
{ 0, 1, 1, 0 , 6, 7,1,N_laddr_number, 1}, /* laddr# */
{ 0, 1, 1, 0 , 7, 8,1,N_laddr_number, 1}, /* laddr# */
{ 0, 1, 1, 0 , 8, 8,1,N_laddr_number, 1}, /* laddr# */
{ 1, 1, 1, 0 , 2, 0,0,N_less_or_equal, 1}, /* <= */
{ 1, 1, 1, 0 , 3, 2,0,N_less_or_equal, 1}, /* <= */
{ 1, 1, 1, 0 , 4, 3,0,N_less_or_equal, 1}, /* <= */
{ 1, 1, 1, 0 , 5, 4,0,N_less_or_equal, 1}, /* <= */
{ 1, 1, 1, 0 , 6, 5,0,N_less_or_equal, 1}, /* <= */
{ 1, 1, 1, 0 , 7, 6,0,N_less_or_equal, 1}, /* <= */
{ 1, 1, 1, 0 , 8, 7,0,N_less_or_equal, 1}, /* <= */
{ 0, 1, 0, 0 , 2, 2,0,N_invert, 1}, /* invert */
{ 0, 1, 0, 0 , 3, 3,0,N_invert, 1}, /* invert */
{ 0, 1, 0, 0 , 4, 4,0,N_invert, 1}, /* invert */
{ 0, 1, 0, 0 , 5, 5,0,N_invert, 1}, /* invert */
{ 0, 1, 0, 0 , 6, 6,0,N_invert, 1}, /* invert */
{ 0, 1, 0, 0 , 7, 7,0,N_invert, 1}, /* invert */
{ 0, 1, 0, 0 , 8, 8,0,N_invert, 1}, /* invert */
{ 2, 2, 2, 1 , 2, 0,1,N_paren_minus_do, 1}, /* (-do) */
{ 2, 2, 2, 1 , 3, 0,1,N_paren_minus_do, 1}, /* (-do) */
{ 2, 2, 2, 1 , 4, 0,1,N_paren_minus_do, 1}, /* (-do) */
{ 2, 2, 2, 1 , 5, 0,1,N_paren_minus_do, 1}, /* (-do) */
{ 2, 2, 2, 1 , 6, 0,1,N_paren_minus_do, 1}, /* (-do) */
{ 2, 2, 2, 1 , 7, 0,1,N_paren_minus_do, 1}, /* (-do) */
{ 2, 2, 2, 1 , 8, 0,1,N_paren_minus_do, 1}, /* (-do) */
{ 3, 6, 1, 0 , 4, 6,0,N_two_over, 1}, /* 2over */
{ 3, 6, 1, 0 , 5, 7,0,N_two_over, 1}, /* 2over */
{ 3, 6, 1, 0 , 6, 8,0,N_two_over, 1}, /* 2over */
{ 3, 6, 1, 0 , 7, 8,0,N_two_over, 1}, /* 2over */
{ 3, 6, 1, 0 , 8, 8,0,N_two_over, 1}, /* 2over */
{ 3, 6, 1, 0 , 1, 2,0,N_two_over, 1}, /* 2over */
{ 3, 6, 1, 0 , 0, 3,0,N_two_over, 1}, /* 2over */
{ 3, 6, 1, 0 , 2, 4,0,N_two_over, 1}, /* 2over */
{ 3, 6, 1, 0 , 3, 5,0,N_two_over, 1}, /* 2over */
{ 3, 6, 1, 0 , 4, 2,0,N_two_over, 1}, /* 2over */
{ 3, 6, 1, 0 , 5, 3,0,N_two_over, 1}, /* 2over */
{ 3, 6, 1, 0 , 6, 4,0,N_two_over, 1}, /* 2over */
{ 2, 2, 1, 0 , 3, 2,0,N_under_plus, 1}, /* under+ */
{ 2, 2, 1, 0 , 4, 3,0,N_under_plus, 1}, /* under+ */
{ 2, 2, 1, 0 , 5, 4,0,N_under_plus, 1}, /* under+ */
{ 2, 2, 1, 0 , 6, 5,0,N_under_plus, 1}, /* under+ */
{ 2, 2, 1, 0 , 7, 6,0,N_under_plus, 1}, /* under+ */
{ 2, 2, 1, 0 , 8, 7,0,N_under_plus, 1}, /* under+ */
{ 1, 2, 0, 0 , 2, 2,0,N_d_negate, 1}, /* dnegate */
{ 1, 2, 0, 0 , 3, 3,0,N_d_negate, 1}, /* dnegate */
{ 1, 2, 0, 0 , 4, 4,0,N_d_negate, 1}, /* dnegate */
{ 1, 2, 0, 0 , 5, 5,0,N_d_negate, 1}, /* dnegate */
{ 1, 2, 0, 0 , 6, 6,0,N_d_negate, 1}, /* dnegate */
{ 1, 2, 0, 0 , 7, 7,0,N_d_negate, 1}, /* dnegate */
{ 1, 2, 0, 0 , 8, 8,0,N_d_negate, 1}, /* dnegate */
{ 1, 1, 1, 0 , 2, 0,0,N_u_not_equals, 1}, /* u<> */
{ 1, 1, 1, 0 , 3, 2,0,N_u_not_equals, 1}, /* u<> */
{ 1, 1, 1, 0 , 4, 3,0,N_u_not_equals, 1}, /* u<> */
{ 1, 1, 1, 0 , 5, 4,0,N_u_not_equals, 1}, /* u<> */
{ 1, 1, 1, 0 , 6, 5,0,N_u_not_equals, 1}, /* u<> */
{ 1, 1, 1, 0 , 7, 6,0,N_u_not_equals, 1}, /* u<> */
{ 1, 1, 1, 0 , 8, 7,0,N_u_not_equals, 1}, /* u<> */
{ 0, 1, 0, 0 , 2, 2,0,N_zero_greater_than, 1}, /* 0> */
{ 0, 1, 0, 0 , 3, 3,0,N_zero_greater_than, 1}, /* 0> */
{ 0, 1, 0, 0 , 4, 4,0,N_zero_greater_than, 1}, /* 0> */
{ 0, 1, 0, 0 , 5, 5,0,N_zero_greater_than, 1}, /* 0> */
{ 0, 1, 0, 0 , 6, 6,0,N_zero_greater_than, 1}, /* 0> */
{ 0, 1, 0, 0 , 7, 7,0,N_zero_greater_than, 1}, /* 0> */
{ 0, 1, 0, 0 , 8, 8,0,N_zero_greater_than, 1}, /* 0> */
{ 2, 4, 1, 0 , 1, 0,0,N_j, 1}, /* j */
{ 2, 4, 1, 0 , 0, 2,0,N_j, 1}, /* j */
{ 2, 4, 1, 0 , 2, 3,0,N_j, 1}, /* j */
{ 2, 4, 1, 0 , 3, 4,0,N_j, 1}, /* j */
{ 2, 4, 1, 0 , 4, 5,0,N_j, 1}, /* j */
{ 2, 4, 1, 0 , 5, 6,0,N_j, 1}, /* j */
{ 2, 4, 1, 0 , 6, 7,0,N_j, 1}, /* j */
{ 2, 4, 1, 0 , 7, 8,0,N_j, 1}, /* j */
{ 2, 4, 1, 0 , 8, 8,0,N_j, 1}, /* j */
{ 0, 1, 0, 0 , 2, 2,0,N_float_plus, 1}, /* float+ */
{ 0, 1, 0, 0 , 3, 3,0,N_float_plus, 1}, /* float+ */
{ 0, 1, 0, 0 , 4, 4,0,N_float_plus, 1}, /* float+ */
{ 0, 1, 0, 0 , 5, 5,0,N_float_plus, 1}, /* float+ */
{ 0, 1, 0, 0 , 6, 6,0,N_float_plus, 1}, /* float+ */
{ 0, 1, 0, 0 , 7, 7,0,N_float_plus, 1}, /* float+ */
{ 0, 1, 0, 0 , 8, 8,0,N_float_plus, 1}, /* float+ */
{ 1, 0, 1, 0 , 1, 1,0,N_two_r_drop, 1}, /* 2rdrop */
{ 1, 0, 1, 0 , 2, 2,0,N_two_r_drop, 1}, /* 2rdrop */
{ 1, 0, 1, 0 , 3, 3,0,N_two_r_drop, 1}, /* 2rdrop */
{ 1, 0, 1, 0 , 4, 4,0,N_two_r_drop, 1}, /* 2rdrop */
{ 1, 0, 1, 0 , 5, 5,0,N_two_r_drop, 1}, /* 2rdrop */
{ 1, 0, 1, 0 , 6, 6,0,N_two_r_drop, 1}, /* 2rdrop */
{ 1, 0, 1, 0 , 7, 7,0,N_two_r_drop, 1}, /* 2rdrop */
{ 1, 0, 1, 0 , 8, 8,0,N_two_r_drop, 1}, /* 2rdrop */
{ 0, 0, 1, 0 , 0, 1,0,N_to_l, 1}, /* >l */
{ 0, 0, 1, 0 , 2, 0,0,N_to_l, 1}, /* >l */
{ 0, 0, 1, 0 , 3, 2,0,N_to_l, 1}, /* >l */
{ 0, 0, 1, 0 , 4, 3,0,N_to_l, 1}, /* >l */
{ 0, 0, 1, 0 , 5, 4,0,N_to_l, 1}, /* >l */
{ 0, 0, 1, 0 , 6, 5,0,N_to_l, 1}, /* >l */
{ 0, 0, 1, 0 , 7, 6,0,N_to_l, 1}, /* >l */
{ 0, 0, 1, 0 , 8, 7,0,N_to_l, 1}, /* >l */
{ 1, 1, 1, 0 , 2, 0,0,N_slash, 1}, /* / */
{ 1, 1, 1, 0 , 3, 2,0,N_slash, 1}, /* / */
{ 1, 1, 1, 0 , 4, 3,0,N_slash, 1}, /* / */
{ 1, 1, 1, 0 , 5, 4,0,N_slash, 1}, /* / */
{ 1, 1, 1, 0 , 6, 5,0,N_slash, 1}, /* / */
{ 1, 1, 1, 0 , 7, 6,0,N_slash, 1}, /* / */
{ 1, 1, 1, 0 , 8, 7,0,N_slash, 1}, /* / */
{ 0, 1, 0, 0 , 2, 2,0,N_two_slash, 1}, /* 2/ */
{ 0, 1, 0, 0 , 3, 3,0,N_two_slash, 1}, /* 2/ */
{ 0, 1, 0, 0 , 4, 4,0,N_two_slash, 1}, /* 2/ */
{ 0, 1, 0, 0 , 5, 5,0,N_two_slash, 1}, /* 2/ */
{ 0, 1, 0, 0 , 6, 6,0,N_two_slash, 1}, /* 2/ */
{ 0, 1, 0, 0 , 7, 7,0,N_two_slash, 1}, /* 2/ */
{ 0, 1, 0, 0 , 8, 8,0,N_two_slash, 1}, /* 2/ */
{ 0, 0, 0, 0 , 1, 1,1,N_abi_call, 1}, /* abi-call */
{ 0, 0, 0, 0 , 1, 1,1,N_semi_abi_code_exec, 1}, /* ;abi-code-exec */
{-1, 0, 0, 0 , 0, 1,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 2, 1,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 3, 1,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 4, 1,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 5, 1,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 6, 1,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 7, 1,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 8, 1,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 1, 0,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 2, 0,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 3, 0,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 4, 0,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 5, 0,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 6, 0,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 7, 0,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 8, 0,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 1, 2,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 0, 2,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 3, 2,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 4, 2,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 5, 2,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 6, 2,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 7, 2,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 8, 2,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 1, 3,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 0, 3,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 2, 3,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 4, 3,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 5, 3,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 6, 3,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 7, 3,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 8, 3,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 1, 4,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 0, 4,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 2, 4,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 3, 4,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 5, 4,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 6, 4,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 7, 4,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 8, 4,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 1, 5,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 0, 5,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 2, 5,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 3, 5,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 4, 5,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 6, 5,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 7, 5,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 8, 5,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 1, 6,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 0, 6,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 2, 6,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 3, 6,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 4, 6,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 5, 6,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 7, 6,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 8, 6,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 1, 7,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 0, 7,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 2, 7,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 3, 7,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 4, 7,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 5, 7,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 6, 7,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 8, 7,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 1, 8,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 0, 8,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 2, 8,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 3, 8,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 4, 8,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 5, 8,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 6, 8,0,N_noop, 1}, /* noop */
{-1, 0, 0, 0 , 7, 8,0,N_noop, 1}, /* noop */
};

struct super_state {
  struct super_state *next;
  PrimNum super;
};

#define HASH_SIZE 256

struct super_table_entry {
  struct super_table_entry *next;
  PrimNum *start;
  short length;
  struct super_state *ss_list; /* list of supers */
} *super_table[HASH_SIZE];
int max_super=2;

struct super_state *state_transitions=NULL;

static int hash_super(PrimNum *start, int length)
{
  int i, r;
  
  for (i=0, r=0; i<length; i++) {
    r <<= 1;
    r += start[i];
  }
  return r & (HASH_SIZE-1);
}

static struct super_state **lookup_super(PrimNum *start, int length)
{
  int hash=hash_super(start,length);
  struct super_table_entry *p = super_table[hash];

  /* assert(length >= 2); */
  for (; p!=NULL; p = p->next) {
    if (length == p->length &&
	memcmp((char *)p->start, (char *)start, length*sizeof(PrimNum))==0)
      return &(p->ss_list);
  }
  return NULL;
}

static void prepare_super_table()
{
  int i;
  int nsupers = 0;

  for (i=0; i<sizeof(super_costs)/sizeof(super_costs[0]); i++) {
    struct cost *c = &super_costs[i];
    if ((c->length < 2 || nsupers < static_super_number) &&
	c->state_in < maxstates && c->state_out < maxstates) {
      struct super_state **ss_listp= lookup_super(super2+c->offset, c->length);
      struct super_state *ss = malloc(sizeof(struct super_state));
      ss->super= i;
      if (c->offset==N_noop && i != N_noop) {
	if (is_relocatable(i)) {
	  ss->next = state_transitions;
	  state_transitions = ss;
	}
      } else if (ss_listp != NULL) {
	ss->next = *ss_listp;
	*ss_listp = ss;
      } else {
    int tmp = super2 + c->offset;
	int hash = hash_super(tmp, c->length);
	struct super_table_entry **p = &super_table[hash];
	struct super_table_entry *e = malloc(sizeof(struct super_table_entry));
	ss->next = NULL;
	e->next = *p;
	e->start =tmp;
	e->length = c->length;
	e->ss_list = ss;
	*p = e;
      }
      if (c->length > max_super)
	max_super = c->length;
      if (c->length >= 2)
	nsupers++;
    }
  }
}

Cell npriminfos=0;

static int cost_codesize(int prim)
{
  return priminfos[prim].length;
}

static int cost_ls(int prim)
{
  struct cost *c = super_costs+prim;

  return c->loads + c->stores;
}

static int cost_lsu(int prim)
{
  struct cost *c = super_costs+prim;

  return c->loads + c->stores + c->updates;
}

static int cost_nexts(int prim)
{
  return 1;
}

typedef int Costfunc(int);
Costfunc *ss_cost =  /* cost function for optimize_bb */
cost_codesize;

struct {
  Costfunc *costfunc;
  char *metricname;
  long sum;
} cost_sums[] = {
  { cost_codesize, "codesize", 0 },
  { cost_ls,       "ls",       0 },
  { cost_lsu,      "lsu",      0 },
  { cost_nexts,    "nexts",    0 }
};

#define MAX_BB 128 /* maximum number of instructions in BB */
#define INF_COST 1000000 /* infinite cost */
#define CANONICAL_STATE 0

struct waypoint {
  int cost;     /* the cost from here to the end */
  PrimNum inst; /* the inst used from here to the next waypoint */
  char relocatable; /* the last non-transition was relocatable */
  char no_transition; /* don't use the next transition (relocatability)
		       * or this transition (does not change state) */
};

void init_waypoints(struct waypoint ws[])
{
  int k;

  for (k=0; k<maxstates; k++)
    ws[k].cost=INF_COST;
}

void transitions(struct waypoint inst[], struct waypoint trans[])
{
  int k;
  struct super_state *l;
  
  for (k=0; k<maxstates; k++) {
    trans[k] = inst[k];
    trans[k].no_transition = 1;
  }
  for (l = state_transitions; l != NULL; l = l->next) {
    PrimNum s = l->super;
    int jcost;
    struct cost *c=super_costs+s;
    struct waypoint *wi=&(trans[c->state_in]);
    struct waypoint *wo=&(inst[c->state_out]);
    if (wo->cost == INF_COST)
      continue;
    jcost = wo->cost + ss_cost(s);
    if (jcost <= wi->cost) {
      wi->cost = jcost;
      wi->inst = s;
      wi->relocatable = wo->relocatable;
      wi->no_transition = 0;
      /* if (ss_greedy) wi->cost = wo->cost ? */
    }
  }
}

void printinst(struct cost *c)
{
  int i;
  static char *states="1023456789";

  putchar(states[c->state_in]);
  putchar('_');
  for (i=0; i<c->length; i++) {
    fputs(prim_names[super2[c->offset+i]],stdout);
    putchar('_');
  }
  putchar(states[c->state_out]);
  putchar(' ');
}

/* use dynamic programming to find the shortest paths within the basic
   block origs[0..ninsts-1] */
void optimize_rewrite(PrimNum origs[], int ninsts)
{
  int i,j;
  static struct waypoint inst[MAX_BB+1][MAX_STATE];  /* before instruction*/
  static struct waypoint trans[MAX_BB+1][MAX_STATE]; /* before transition */
  int nextdyn, nextstate, no_transition;
  
  init_waypoints(inst[ninsts]);
  inst[ninsts][CANONICAL_STATE].cost=0;
  transitions(inst[ninsts],trans[ninsts]);
  for (i=ninsts-1; i>=0; i--) {
    init_waypoints(inst[i]);
    for (j=1; j<=max_super && i+j<=ninsts; j++) {
      struct super_state **superp = lookup_super(origs+i, j);
      if (superp!=NULL) {
	struct super_state *supers = *superp;
	for (; supers!=NULL; supers = supers->next) {
	  PrimNum s = supers->super;
	  int jcost;
	  struct cost *c=super_costs+s;
	  struct waypoint *wi=&(inst[i][c->state_in]);
	  struct waypoint *wo=&(trans[i+j][c->state_out]);
	  int no_transition = wo->no_transition;
	  if (!(is_relocatable(s)) && !wo->relocatable) {
	    wo=&(inst[i+j][c->state_out]);
	    no_transition=1;
	  }
	  if (wo->cost == INF_COST) 
	    continue;
	  jcost = wo->cost + ss_cost(s);
	  if (jcost <= wi->cost) {
	    wi->cost = jcost;
	    wi->inst = s;
	    wi->relocatable = is_relocatable(s);
	    wi->no_transition = no_transition;
	    /* if (ss_greedy) wi->cost = wo->cost ? */
	  }
	}
      }
    }
    transitions(inst[i],trans[i]);
  }
  /* now rewrite the instructions */
  nextdyn=0;
  nextstate=CANONICAL_STATE;
  no_transition = ((!trans[0][nextstate].relocatable) 
		   ||trans[0][nextstate].no_transition);
  for (i=0; i<ninsts; i++) {
    if (i==nextdyn) {
      if (!no_transition) {
	/* process trans */
	PrimNum p = trans[i][nextstate].inst;
	struct cost *c = super_costs+p;
	assert(trans[i][nextstate].cost != INF_COST);
	assert(c->state_in==nextstate);
	printinst(c);
	nextstate = c->state_out;
      }
      {
	/* process inst */
	PrimNum p = inst[i][nextstate].inst;
	struct cost *c=super_costs+p;
	assert(c->state_in==nextstate);
	assert(inst[i][nextstate].cost != INF_COST);
	printinst(c);
	no_transition = inst[i][nextstate].no_transition;
	nextstate = c->state_out;
	nextdyn += c->length;
      }
    }
  }      
  if (!no_transition) {
    PrimNum p = trans[i][nextstate].inst;
    struct cost *c = super_costs+p;
    assert(c->state_in==nextstate);
    assert(trans[i][nextstate].cost != INF_COST);
    assert(i==nextdyn);
    printinst(c);
    nextstate = c->state_out;
  }
  printf("\n");
  assert(nextstate==CANONICAL_STATE);
}

#define MAX_INPUT_SIZE 100000

int main(int argc, char **argv, char **env)
{
#ifdef PERF
	startPerformanceCounters( argc > 1 ? argv[1] : NULL );
#endif
  PrimNum data[MAX_INPUT_SIZE];
  PrimNum *start = data;
  size_t input_size;
  int i;

  prepare_super_table();
  input_size = fread(data,sizeof(PrimNum),MAX_INPUT_SIZE,stdin);
  for (i = 0; i<input_size; i++)
    if (data[i] == -1) {
      optimize_rewrite(start, data+i-start);
      start = data+i+1;
    }
#ifdef PERF
	stopPerformanceCounters();
#endif
 return 0;
}
