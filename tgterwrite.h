//////////////////////////////////////////////////////////////////////////////////////////

/*!
	\file       tgterwrite.h
	\brief      Contains functions for writing Terragen TER files and raw heightmap files.
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

#include "tgtertypes.h"
#include "tgteriodetails.h"

//--------------------------------------------------------------------------------------//

class ResultOf_WriteTgTerFile
{
public:
	bool succeeded;
	std::string filename;
	std::string errorString;

	ResultOf_WriteTgTerFile() : succeeded(false)
	{
	}

	ResultOf_WriteTgTerFile(bool success, std::string filename, std::string error_string)
		: succeeded(success), filename(filename), errorString(error_string)
	{
	}
};

typedef ResultOf_WriteTgTerFile ResultOf_WriteRawFile;

#define TGTER_MAX(a, b) (a > b ? a : b)
#define TGTER_MIN(a, b) (a < b ? a : b)

//--------------------------------------------------------------------------------------//

inline ResultOf_WriteTgTerFile
	WriteTgTerFile(const char* filename, const TgTerHeader* header, const TgTerAlts* source)
{

	FILE* of = fopen(filename,"wb");

	if (!of)
	{
		return ResultOf_WriteTgTerFile(false, filename, "Unable to open output file");
	}

	fwrite("TERRAGENTERRAIN ", 16, 1, of);

	fwrite("SIZE", 4, 1, of);
	TgTer_PutIntel_UShort(of, TGTER_MIN(header->pointsX, header->pointsY) - 1);
	TgTer_PutIntel_UShort(of, 0);

	fwrite("XPTS", 4, 1, of);
	TgTer_PutIntel_UShort(of, header->pointsX);
	TgTer_PutIntel_UShort(of, 0);

	fwrite("YPTS", 4, 1, of);
	TgTer_PutIntel_UShort(of, header->pointsY);
	TgTer_PutIntel_UShort(of, 0);

	fwrite("SCAL", 4, 1, of);
	TgTer_PutIntel_Float(of, header->scaleM[0]);
	TgTer_PutIntel_Float(of, header->scaleM[1]);
	TgTer_PutIntel_Float(of, header->scaleM[2]);

	fwrite("CRAD", 4, 1, of);
	TgTer_PutIntel_Float(of, header->planetCurveRadiusKm);

	fwrite("CRVM", 4, 1, of);
	TgTer_PutIntel_UShort(of, header->planetCurveMode);
	TgTer_PutIntel_UShort(of, 0);


	fwrite("ALTW", 4, 1, of);

	//compute altitude range from the data (it happens in the constructor)
	TgTerAltRange altRange(header, source);

	const float minalt = altRange.minAlt * source->writeMultiplier;
	const float maxalt = altRange.maxAlt * source->writeMultiplier;

	int16_t altscale1,altscale2;
	int16_t basealt,altscale;

	//choose appropriate base and scale values
	basealt = (int)( floor( (minalt + maxalt) / 2 ) + 0.5f );
	altscale1 = ( (int)(ceil(maxalt)) - basealt ) * 2;
	altscale2 = ( basealt - (int)(floor(minalt)) ) * 2;
	if (altscale1 > altscale2)
	{
		altscale = altscale1;
	}
	else
	{
		altscale = altscale2;
	}

	//write base and scale values
	TgTer_PutIntel_UShort(of, altscale);
	TgTer_PutIntel_UShort(of, basealt);

	//now compute and write map elevation values,
	//adjusting for base and scale values that will be used when the file is read
	//
	//using: filevalue = (alt - basealt) / (altscale / 65536)

	const float scalar = 65536.0f / altscale;

	int maxi = header->pointsX * header->pointsY * source->stride;
	for (int i = 0; i < maxi; i += source->stride)
	{
		TgTer_PutIntel_UShort(of, (int16_t)((source->alts[i] * source->writeMultiplier - basealt) * scalar));
	}

	if ((header->pointsX * header->pointsY) % 2 > 0)
	{
		TgTer_PutIntel_UShort(of, 0);
	}

	fwrite("EOF ", 4, 1, of);

	fclose(of);

	return ResultOf_WriteTgTerFile(true, filename, "");
}

//--------------------------------------------------------------------------------------//

inline ResultOf_WriteRawFile
	WriteRawFile(const char* filename, const TgTerHeader* header, const TgTerAlts* source)
{
	//Very simple raw 16-bit format with little-endian (e.g. Intel) byte order. It lacks
	//the scaling information that Terragen TER files have, so scale-preservation and
	//round-tripping are harder. But it is more widely supported.

	FILE* of = fopen(filename,"wb");

	if (!of)
	{
		return ResultOf_WriteRawFile(false, filename, "Unable to open output file");
	}

	//compute altitude range from the data (it happens in the constructor)
	TgTerAltRange altRange(header, source);

	const float minalt = altRange.minAlt * source->writeMultiplier;
	const float maxalt = altRange.maxAlt * source->writeMultiplier;
	const float scalar = 65535.9f / TGTER_MAX((maxalt-minalt), 1e-6f);

	int maxi = header->pointsX * header->pointsY * source->stride;
	for (int i = 0; i < maxi; i += source->stride)
	{
		float elev_f = (source->alts[i] * source->writeMultiplier - minalt) * scalar;
		TgTer_PutIntel_UShort(of, (uint16_t)floorf(elev_f));
	}

	fclose(of);

	return ResultOf_WriteRawFile(true, filename, "");
}

//--------------------------------------------------------------------------------------//

//////////////////////////////////////////////////////////////////////////////////////////
