/*
 * Copyright (C) 2021 Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
