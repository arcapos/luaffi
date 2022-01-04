/*
 * Copyright (c) 2017 - 2022 Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick.
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

#include <sys/types.h>

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <ffi.h>

#include "luaffi.h"

static ffi_type *type[] = {
	&ffi_type_void,		&ffi_type_uint8,	&ffi_type_sint8,
	&ffi_type_uint16,	&ffi_type_sint16,	&ffi_type_uint32,
	&ffi_type_sint32,	&ffi_type_uint64,	&ffi_type_sint64,
	&ffi_type_float,	&ffi_type_double,	&ffi_type_longdouble,
	&ffi_type_pointer,
	NULL
};

static const char *typename[] = {
	"void",			"uint8",		"int8",
	"uint16",		"int16",		"uint32",
	"int32",		"uint64",		"int64",
	"float",		"double",		"longdouble",
	"pointer",
	NULL
};

enum typesymbol {
	VOID = 0,		UINT8,			INT8,
	UINT16,			INT16,			UINT32,
	INT32,			UINT64,			INT64,
	FLOAT,			DOUBLE,			LONGDOUBLE,
	POINTER
};

static int
checktype(ffi_type *t)
{
	int n;

	for (n = 0; type[n]; n++)
		if (type[n] == t)
			return n;
	return -1;
}

static int
checkname(const char *name)
{
	int n;

	for (n = 0; typename[n]; n++)
		if (!strcmp(name, typename[n]))
			return n;
	return -1;
}

static int
luaffi_prep_cif(lua_State *L)
{
	ffi_func *func;
	void **symbol;
	ffi_type **arg_type;
	int rtype, n, nargs, t;
	const char *name;
	char *ref_type;

	symbol = luaL_checkudata(L, 1, DLSYM_METATABLE);
	rtype = luaL_checkoption(L, 2, NULL, typename);

	nargs = lua_gettop(L) - 2;
	if (nargs) {
		arg_type = malloc(nargs * sizeof(ffi_type *));
		ref_type = malloc(nargs);

		for (n = 0; n < nargs; n++) {
			name = luaL_checkstring(L, 3 + n);
			t = *name == '*' ? checkname(&name[1])
			    : checkname(name);
			if (t == -1) {
				free(arg_type);
				free(ref_type);
				return luaL_error(L, "unknown argument type");
			}
			ref_type[n] = *name == '*' ? 1 : 0;
			arg_type[n] = type[t];
		}
	} else {
		arg_type = NULL;
		ref_type = NULL;
	}

	func = lua_newuserdata(L, sizeof(ffi_func));
	func->symbol = *symbol;
	func->arg_type = arg_type;
	func->ref_type = ref_type;
	luaL_setmetatable(L, FFI_FUNC_METATABLE);

	lua_pushinteger(L, ffi_prep_cif(&func->cif, FFI_DEFAULT_ABI, nargs,
	    type[rtype], arg_type));
	return 2;
}

static int
luaffi_prep_cif_var(lua_State *L)
{
	ffi_func *func;
	void **symbol;
	ffi_type **arg_type;
	int rtype, n, nargs, t;
	unsigned int fixedargs;
	const char *name;
	char *ref_type;

	symbol = luaL_checkudata(L, 1, DLSYM_METATABLE);
	fixedargs = luaL_checkinteger(L, 2);
	rtype = luaL_checkoption(L, 3, NULL, typename);

	nargs = lua_gettop(L) - 3;
	if (nargs) {
		arg_type = malloc(nargs * sizeof(ffi_type *));
		ref_type = malloc(nargs);

		for (n = 0; n < nargs; n++) {
			name = luaL_checkstring(L, 4 + n);
			t = *name == '*' ? checkname(&name[1])
			    : checkname(name);
			if (t == -1) {
				free(arg_type);
				free(ref_type);
				return luaL_error(L, "unknown argument type");
			}
			ref_type[n] = *name == '*' ? 1 : 0;
			arg_type[n] = type[t];
		}
	} else {
		arg_type = NULL;
		ref_type = NULL;
	}

	func = lua_newuserdata(L, sizeof(ffi_func));
	func->symbol = *symbol;
	func->arg_type = arg_type;
	func->ref_type = ref_type;
	luaL_setmetatable(L, FFI_FUNC_METATABLE);

	lua_pushinteger(L, ffi_prep_cif_var(&func->cif, FFI_DEFAULT_ABI,
	    fixedargs, nargs, type[rtype], arg_type));
	return 2;
}

static int
luaffi_call(lua_State *L)
{
	ffi_func *func = luaL_checkudata(L, 1, FFI_FUNC_METATABLE);
	ffi_val *argval;
	ffi_val rvalue;
	void **args;
	int n;
	char *p;

	argval = malloc(func->cif.nargs * sizeof(ffi_arg));
	args = malloc(func->cif.nargs * sizeof(void *));

	for (n = 0; n < func->cif.nargs; n++) {
		switch (checktype(func->cif.arg_types[n])) {
		case UINT8:
			argval[n].uint8_val = luaL_checkinteger(L, 2 + n);
			break;
		case INT8:
			argval[n].int8_val = luaL_checkinteger(L, 2 + n);
			break;
		case UINT16:
			argval[n].uint16_val = luaL_checkinteger(L, 2 + n);
			break;
		case INT16:
			argval[n].int16_val = luaL_checkinteger(L, 2 + n);
			break;
		case UINT32:
			argval[n].uint32_val = luaL_checkinteger(L, 2 + n);
			break;
		case INT32:
			argval[n].int32_val = luaL_checkinteger(L, 2 + n);
			break;
		case UINT64:
			argval[n].uint64_val = luaL_checkinteger(L, 2 + n);
			break;
		case INT64:
			argval[n].int64_val = luaL_checkinteger(L, 2 + n);
			break;
		case FLOAT:
			argval[n].float_val = (float)luaL_checknumber(L, 2 + n);
			break;
		case DOUBLE:
			argval[n].double_val = luaL_checknumber(L, 2 + n);
			break;
		case POINTER:
			argval[n].ptr_val = luaL_checkstring(L, 2 + n);
			break;
		default:
			printf("unknown argument type\n");
			break;
		}
		args[n] = &argval[n];
	}

	ffi_call(&func->cif, func->symbol, &rvalue, args);

	free(args);
	free(argval);

	switch (checktype(func->cif.rtype)) {
	case UINT8:
		lua_pushinteger(L, rvalue.uint8_val);
		break;
	case INT8:
		lua_pushinteger(L, rvalue.int8_val);
		break;
	case UINT16:
		lua_pushinteger(L, rvalue.uint16_val);
		break;
	case INT16:
		lua_pushinteger(L, rvalue.int16_val);
		break;
	case UINT32:
		lua_pushinteger(L, rvalue.uint32_val);
		break;
	case INT32:
		lua_pushinteger(L, rvalue.int32_val);
		break;
	case UINT64:
		lua_pushinteger(L, rvalue.uint64_val);
		break;
	case INT64:
		lua_pushinteger(L, rvalue.int64_val);
		break;
	case FLOAT:
		lua_pushnumber(L, rvalue.float_val);
		break;
	case DOUBLE:
		lua_pushnumber(L, rvalue.double_val);
		break;
	case POINTER:
		lua_pushstring(L, rvalue.ptr_val);
		break;
	case VOID:
		return 0;
	default:
		printf("unknown return type\n");
		break;
	}
	return 1;
}

static int
luaffi_clear(lua_State *L)
{
	ffi_func *func = luaL_checkudata(L, 1, FFI_FUNC_METATABLE);

	free(func->arg_type);
	free(func->ref_type);
	func->arg_type = NULL;
	func->ref_type = NULL;
	return 0;
}

static struct ints {
	char	*name;
	int	 value;
} ints[] = {
	{ "DEFAULT_ABI",	FFI_DEFAULT_ABI },

	/* status codes */
	{ "OK",			FFI_OK },
	{ "BAD_TYPEDEF",	FFI_BAD_TYPEDEF },
	{ "BAD_ABI",		FFI_BAD_ABI },
	{ NULL,			0 }
};

int
luaopen_ffi(lua_State *L)
{
	struct luaL_Reg luaffi[] = {
		{ "prep_cif",		luaffi_prep_cif },
		{ "prep_cif_var",	luaffi_prep_cif_var },
		{ "call",		luaffi_call },
		{ NULL, NULL }
	};
	struct luaL_Reg ffi_func_methods[] = {
		{ "__call",		luaffi_call },
		{ "__close",		luaffi_clear },
		{ "__gc",		luaffi_clear },
		{ NULL,			NULL }
	};
	int n;

	luaL_newlib(L, luaffi);

	if (luaL_newmetatable(L, FFI_FUNC_METATABLE)) {
		luaL_setfuncs(L, ffi_func_methods, 0);

		lua_pushliteral(L, "__index");
		lua_pushvalue(L, -2);
		lua_settable(L, -3);

		lua_pushliteral(L, "__metatable");
		lua_pushliteral(L, "must not access this metatable");
		lua_settable(L, -3);
	}
	lua_pop(L, 1);

	lua_pushliteral(L, "_COPYRIGHT");
	lua_pushliteral(L, "Copyright (C) 2017 - 2022 "
	    "micro systems marc balmer");
	lua_settable(L, -3);
	lua_pushliteral(L, "_DESCRIPTION");
	lua_pushliteral(L, "FFI for Lua");
	lua_settable(L, -3);
	lua_pushliteral(L, "_VERSION");
	lua_pushliteral(L, "ffi 1.0.2");
	lua_settable(L, -3);

	for (n = 0; ints[n].name; n++) {
		lua_pushinteger(L, ints[n].value);
		lua_setfield(L, -2, ints[n].name);
	}
	return 1;
}
