#include <lua.h>
#include <lauxlib.h>

#define bin2hex_STATICSIZE 256
static char hexchars[] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};
static int bin2hex(lua_State* L)
{
	typedef unsigned char byte;
	const byte* bin;
	size_t size, i;
	char buffer[bin2hex_STATICSIZE];
	char* hex;
	bin = (const byte*)luaL_checklstring(L, 1, &size);
	/* 1 bytes for 2 chars */
	if (size <= bin2hex_STATICSIZE/2)
		hex = buffer;
	else
		hex = (char*)lua_newuserdata(L, size * 2);
	for (i=0; i<size; ++i)
	{
		byte a;
		a = bin[i*1+0];
		hex[i*2+0] = hexchars[(a>>4)&0xf];
		hex[i*2+1] = hexchars[a&0xf];
	}
	lua_pushlstring(L, hex, size*2);
	return 1;
}

#define bin2base32_STATICSIZE 256
static char base32chars[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', '2', '3', '4', '5', '6', '7',
};
#define b00001 0x01
#define b00011 0x03
#define b00111 0x07
#define b01111 0x0f
#define b11111 0x1f
#define b11110 0x1e
#define b11100 0x1c
#define b11000 0x18
#define b10000 0x10
static int bin2base32(lua_State* L)
{
	typedef unsigned char byte;
	const byte* bin;
	size_t size, i;
	char buffer[bin2base32_STATICSIZE];
	char* base32;
	bin = (const byte*)luaL_checklstring(L, 1, &size);
	/* 5 bytes for 8 chars */
	if (size % 5 != 0)
		return luaL_argerror(L, 1, "string length must be a multiple of 5");
	if (size / 5 <= bin2base32_STATICSIZE / 8)
		base32 = buffer;
	else
		base32 = (char*)lua_newuserdata(L, size * 2);
	for (i=0; i<size/5; ++i)
	{
		byte a, b, c, d, e;
		a = bin[i*5+0];
		b = bin[i*5+1];
		c = bin[i*5+2];
		d = bin[i*5+3];
		e = bin[i*5+4];
		base32[i*8+0] = base32chars[(a>>3)&b11111];	                /* 5 bits from a */
		base32[i*8+1] = base32chars[(a<<2)&b11100 | (b>>6)&b00011]; /* 3 bits from a and 2 bits from b */
		base32[i*8+2] = base32chars[(b>>1)&b11111];                 /* 5 bits from b */
		base32[i*8+3] = base32chars[(b<<4)&b10000 | (c>>4)&b01111]; /* 1 bit from b and 4 bits from c */
		base32[i*8+4] = base32chars[(c<<1)&b11110 | (d>>7)&b00001]; /* 4 bits from c and 1 bit from d */
		base32[i*8+5] = base32chars[(d>>2)&b11111];                 /* 5 bits from d */
		base32[i*8+6] = base32chars[(d<<3)&b11000 | (e>>5)&b00111]; /* 2 bits from d and 3 bits from e */
		base32[i*8+7] = base32chars[(e<<0)&b11111];                 /* 5 bits from e */
	}
	lua_pushlstring(L, base32, size*8/5);
	return 1;
}

static luaL_Reg functions[] = {
	{"bin2hex", bin2hex},
	{"bin2base32", bin2base32},
	{0, 0},
};

LUALIB_API int luaopen_module(lua_State* L)
{
	luaL_register(L, lua_tostring(L, 1), functions);
	return 0;
}

