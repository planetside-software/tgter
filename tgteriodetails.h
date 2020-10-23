//////////////////////////////////////////////////////////////////////////////////////////

/*!
	\file       tgteriodetails.h
	\brief      Contains low level functions used by tgterread.h and tgterwrite.h.
*/

//--------------------------------------------------------------------------------------//

/*
MIT License

Copyright (c) 2020 Matt Fairclough

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

//--------------------------------------------------------------------------------------//

#pragma once

//--------------------------------------------------------------------------------------//

#include <stdio.h>
#include <stdint.h>

//--------------------------------------------------------------------------------------//

void TgTer_PutIntel_Byte(FILE* outf, unsigned char val);
void TgTer_GetIntel_Byte(FILE* inf, unsigned char *val);

void TgTer_PutIntel_UShort(FILE* outf, uint16_t val);
void TgTer_PutMoto_UShort(FILE* outf, uint16_t val);
void TgTer_GetIntel_UShort(FILE* inf, uint16_t *val);
void TgTer_GetMoto_UShort(FILE* inf, uint16_t *val);

void TgTer_CorrectReadIntel_UShort(uint16_t *val);
void TgTer_CorrectReadMoto_UShort(uint16_t *val);

void TgTer_PutIntel_UInt32(FILE* outf, uint32_t val);
void TgTer_PutMoto_UInt32(FILE* outf, uint32_t val);
void TgTer_GetIntel_UInt32(FILE* inf, uint32_t *val);
void TgTer_GetMoto_UInt32(FILE* inf, uint32_t *val);

void TgTer_CorrectReadIntel_UInt32(uint32_t *val);

void TgTer_PutIntel_Float(FILE* outf, float val);
void TgTer_PutMoto_Float(FILE* outf, float val);
void TgTer_GetIntel_Float(FILE* inf, float* val);
void TgTer_GetMoto_Float(FILE* inf, float* val);

//--------------------------------------------------------------------------------------//

inline void TgTer_PutIntel_Byte(FILE* outf, unsigned char val)
{
	unsigned char buf[1];

	buf[0] = val;
	fwrite(buf,1,1,outf);
}

inline void TgTer_GetIntel_Byte(FILE* inf, unsigned char *val)
{
	unsigned char buf[1];

	fread(buf,1,1,inf);
	*val = buf[0];
}

inline void TgTer_PutIntel_UShort(FILE* outf, uint16_t val)
{
	unsigned char buf[2];

	buf[0] = (unsigned char)(val>>0);
	buf[1] = (unsigned char)(val>>8);
	fwrite(buf,2,1,outf);
}

inline void TgTer_PutMoto_UShort(FILE* outf, uint16_t val)
{
	unsigned char buf[2];

	buf[0] = (unsigned char)(val>>8);
	buf[1] = (unsigned char)(val>>0);
	fwrite(buf,2,1,outf);
}

inline void TgTer_GetIntel_UShort(FILE* inf, uint16_t *val)
{
	unsigned char buf[2];

	fread(buf,2,1,inf);
	*val = (((uint16_t)buf[0]) << 0)
	     + (((uint16_t)buf[1]) << 8);
}

inline void TgTer_GetMoto_UShort(FILE* inf, uint16_t *val)
{
	unsigned char buf[2];

	fread(buf,2,1,inf);
	*val = (((uint16_t)buf[0]) << 8)
	     + (((uint16_t)buf[1]) << 0);
}

inline void TgTer_CorrectReadIntel_UShort(uint16_t *val)
{
	//does nothing on Intel compatibles
	//but reverses bytes if necessary on other platforms
	unsigned char buf[2];
	*((uint16_t*)buf) = *val;
	*val = (((uint16_t)buf[0]) << 0)
	     + (((uint16_t)buf[1]) << 8);
}

inline void TgTer_CorrectReadMoto_UShort(uint16_t *val)
{
	//does nothing on Motorola order machines
	//but reverses bytes if necessary on other platforms
	unsigned char buf[2];
	*((uint16_t*)buf) = *val;
	*val = (((uint16_t)buf[0]) << 8)
	     + (((uint16_t)buf[1]) << 0);
}

inline void TgTer_PutIntel_UInt32(FILE* outf, uint32_t val)
{
	unsigned char buf[4];

	buf[0] = (unsigned char)(val>>0);
	buf[1] = (unsigned char)(val>>8);
	buf[2] = (unsigned char)(val>>16);
	buf[3] = (unsigned char)(val>>24);
	fwrite(buf,4,1,outf);
}

inline void TgTer_PutMoto_UInt32(FILE* outf, uint32_t val)
{
	unsigned char buf[4];

	buf[0] = (unsigned char)(val>>24);
	buf[1] = (unsigned char)(val>>16);
	buf[2] = (unsigned char)(val>>8);
	buf[3] = (unsigned char)(val>>0);
	fwrite(buf,4,1,outf);
}

inline void TgTer_GetIntel_UInt32(FILE* inf, uint32_t *val)
{
	unsigned char buf[4];
	
	fread(buf,4,1,inf);
	*val = (((uint32_t)buf[0]) << 0)
	     + (((uint32_t)buf[1]) << 8)
	     + (((uint32_t)buf[2]) << 16)
	     + (((uint32_t)buf[3]) << 24);
}

inline void TgTer_GetMoto_UInt32(FILE* inf, uint32_t *val)
{
	unsigned char buf[4];

	fread(buf,4,1,inf);
	*val = (((uint32_t)buf[0]) << 24)
	     + (((uint32_t)buf[1]) << 16)
	     + (((uint32_t)buf[2]) << 8)
	     + (((uint32_t)buf[3]) << 0);
}

inline void TgTer_CorrectReadIntel_UInt32(uint32_t *val)
{
	//does nothing on Intel compatibles
	//but reverses bytes if necessary on other platforms
	unsigned char buf[4];
	*((uint32_t*)buf) = *val;
	*val = (((uint32_t)buf[0]) << 0)
	     + (((uint32_t)buf[1]) << 8)
	     + (((uint32_t)buf[2]) << 16)
	     + (((uint32_t)buf[3]) << 24);
}

inline void TgTer_PutIntel_Float(FILE* outf, float val)
{
	unsigned char buf[4];
	uint32_t* lval = (uint32_t*)&val;

	buf[0] = (unsigned char)((*lval)>>0);
	buf[1] = (unsigned char)((*lval)>>8);
	buf[2] = (unsigned char)((*lval)>>16);
	buf[3] = (unsigned char)((*lval)>>24);
	fwrite(buf,4,1,outf);
}

inline void TgTer_PutMoto_Float(FILE* outf, float val)
{
	unsigned char buf[4];
	uint32_t* lval = (uint32_t*)&val;

	buf[0] = (unsigned char)((*lval)>>24);
	buf[1] = (unsigned char)((*lval)>>16);
	buf[2] = (unsigned char)((*lval)>>8);
	buf[3] = (unsigned char)((*lval)>>0);
	fwrite(buf,4,1,outf);
}

inline void TgTer_GetIntel_Float(FILE* inf, float* val)
{
	unsigned char buf[4];
	uint32_t* lval = (uint32_t*)val;

	fread(buf,4,1,inf);
	*lval = (((uint32_t)buf[0]) << 0)
	      + (((uint32_t)buf[1]) << 8)
	      + (((uint32_t)buf[2]) << 16)
	      + (((uint32_t)buf[3]) << 24);
}

inline void TgTer_GetMoto_Float(FILE* inf, float* val)
{
	unsigned char buf[4];
	uint32_t* lval = (uint32_t*)val;

	fread(buf,4,1,inf);
	*lval = (((uint32_t)buf[0]) << 24)
	      + (((uint32_t)buf[1]) << 16)
	      + (((uint32_t)buf[2]) << 8)
	      + (((uint32_t)buf[3]) << 0);
}

//--------------------------------------------------------------------------------------//

//////////////////////////////////////////////////////////////////////////////////////////
