/*
 * Copyright (c) 2017 Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick.
 * All rights reserved.
 *
 * This source code is the proprietary confidential property of Micro Systems
 * Marc Balmer, and is provided to licensee solely for documentation and
 * educational purposes. Reproduction, publication, or distribution in any form
 * to any party other than the licensee is strictly prohibited.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* FFI for Lua */

#ifndef __LUAFFI__
#define __LUAFFI__

#define LUA_FFILIBNAME		"ffi"

#define DLSYM_METATABLE		"dynamic linker symbol"	/* from luaunix/dl.h */
#define FFI_FUNC_METATABLE	"ffi callable function"
#define	FFI_CIF_METATABLE	"ffi call interface"

typedef struct _ffi_func {
	void		*symbol;
	ffi_type	**arg_type;
	char		*ref_type;
	ffi_cif	 	 cif;
} ffi_func;

typedef union {
	uint8_t		uint8_val;
	int8_t		int8_val;
	uint16_t	uint16_val;
	int16_t		int16_val;
	uint32_t	uint32_val;
	int32_t		int32_val;
	uint64_t	uint64_val;
	int64_t		int64_val;
	double		double_val;
	long double	longdouble_val;
	float		float_val;
	void *		ptr_val;
} ffi_val;

extern int luaopen_ffi(lua_State *L);

#endif /* __ LUAFFI__ */
