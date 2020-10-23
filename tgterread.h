//////////////////////////////////////////////////////////////////////////////////////////

/*!
	\file       tgterread.h
	\brief      Contains a function for reading Terragen TER files.
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

class ResultOf_ReadTgTerFile
{
public:
	bool succeeded;
	std::string filename;
	std::string errorString;

	ResultOf_ReadTgTerFile() : succeeded(false)
	{
	}

	ResultOf_ReadTgTerFile(bool success, std::string filename, std::string error_string)
		: succeeded(success), filename(filename), errorString(error_string)
	{
	}
};

//--------------------------------------------------------------------------------------//

inline ResultOf_ReadTgTerFile
	ReadTgTerFile(
		const char* filename,
		const int readmode,
		TgTerHeader* header,
		TgTerAlts* destination,
		TgTerAltRange* optional_alt_range)
{

	/*
	readmode 0: Reads dimensions and metadata (scale and planetary context) into header,
	            but does not read elevations or calculate exact min/max elevations.
	            If optional_alt_range and destination structs are supplied, min/max alts
	            are estimated and recorded in optional_alt_range but their reliability
	            depends on the file.

	readmode 1: Reads elevations into existing array pointed to by destination,
	            reads meta data (scale and planetary context) into header,
	            but does not read dimensions.
	            If optional_alt_range struct is supplied, min/max altitudes are
	            calculated by looping over the alts in destination and recorded in
	            optional_alt_range.

	If readmode is 1, the alts array must already be allocated with dimensions in header.
	This function does not set the dimensions or resize the array itself.
	Typical usage is to call this function with a readmode of 0 to get dimensions and
	record them in the header data structure, then allocate some memory for the alt array
	(not done by this function), then finally call this function again with a readmode of
	1 to fill the array.
	*/

	FILE* fp = fopen(filename,"rb");

	if (!fp)
	{
		return ResultOf_ReadTgTerFile(false, filename, "Unable to open terrain file");
	}

	char buf[17 * sizeof(char)];
	buf[16] = '\0';

	fread(buf,16,sizeof(char),fp);
	if (strncmp(buf,"TERRAGENTERRAIN ",16))
	{
		fclose(fp);
		return ResultOf_ReadTgTerFile(false, filename, "This is not a Terragen terrain file");
	}

	uint16_t pad;
	uint16_t xpts = 0;
	uint16_t ypts = 0;
	uint16_t size;
	float xscale = 30.f;
	float yscale = 30.f;
	float zscale = 30.f;
	float radius = 6370.f;
	uint16_t curvemode = 0;
	int16_t heightscale = 0;
	int16_t baseheight = 0;

	bool done = false;
	while (!done)
	{
		fread(buf, 4, sizeof(char), fp );
		buf[4] = '\0';

		if (!strcmp(buf,"SIZE"))
		{
			TgTer_GetIntel_UShort(fp,&size);
			TgTer_GetIntel_UShort(fp,&pad);
			if (xpts == 0) xpts = size + 1;
			if (ypts == 0) ypts = size + 1;
		}

		else if (!strcmp(buf,"XPTS"))
		{
			TgTer_GetIntel_UShort(fp,&xpts);
			TgTer_GetIntel_UShort(fp,&pad);
		}

		else if (!strcmp(buf,"YPTS"))
		{
			TgTer_GetIntel_UShort(fp,&ypts);
			TgTer_GetIntel_UShort(fp,&pad);
		}

		else if (!strcmp(buf,"SCAL"))
		{
			TgTer_GetIntel_Float(fp,&xscale);
			TgTer_GetIntel_Float(fp,&yscale);
			TgTer_GetIntel_Float(fp,&zscale);
		}

		else if (!strcmp(buf,"CRAD"))
		{
			TgTer_GetIntel_Float(fp,&radius);
		}

		else if (!strcmp(buf,"CRVM"))
		{
			TgTer_GetIntel_UShort(fp,&curvemode);
			TgTer_GetIntel_UShort(fp,&pad);
		}

		else if (!strcmp(buf,"ALTW"))
		{
			TgTer_GetIntel_UShort(fp,(uint16_t*)&heightscale);
			TgTer_GetIntel_UShort(fp,(uint16_t*)&baseheight);

			if (readmode == 1)	//reading heightfield
			{
				const float destmult = destination->readMultiplier;
				int maxi = header->pointsX * header->pointsY;
				for (int i = 0; i < maxi; ++i)
				{
					int16_t altw = 0;
					TgTer_GetIntel_UShort(fp, (uint16_t*)&altw);
					destination->alts[i * destination->stride] =
						(baseheight + altw * (heightscale/65536.f)) * destmult;
				}
			}

			done = 1;
		}

		else if (!strcmp(buf,"EOF "))
		{
			done = 1;
		}
	}

	fclose(fp);

	if (readmode == 0)	//gaining info
	{
		header->pointsX = xpts;
		header->pointsY = ypts;
		if (optional_alt_range && destination)
		{
			const float destmult = destination->readMultiplier;
			optional_alt_range->minAlt = (baseheight - 0.5f * heightscale) * destmult;
			optional_alt_range->maxAlt = (baseheight + 0.5f * heightscale) * destmult;
		}
	}

	header->scaleM[0] = xscale;
	header->scaleM[1] = yscale;
	header->scaleM[2] = zscale;
	header->planetCurveRadiusKm = radius;
	header->planetCurveMode = curvemode;

	if (readmode == 1)	//reading heighfield
	{
		if (optional_alt_range)
		{
			//compute altitude range from the data (it happens in the constructor)
			*optional_alt_range = TgTerAltRange(header, destination);
		}
	}

	return ResultOf_ReadTgTerFile(true, filename, "");
}

//--------------------------------------------------------------------------------------//

//////////////////////////////////////////////////////////////////////////////////////////
