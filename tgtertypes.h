//////////////////////////////////////////////////////////////////////////////////////////

/*!
	\file       tgtertypes.h
	\brief      Contains supporting data types for working with Terragen TER files.
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

class TgTerHeader
{
public:
	unsigned int pointsX;           // eg. 513
	unsigned int pointsY;           // eg. 513
	float scaleM[3];                // Point spacing in metres.
	                                // The default is {30.0f, 30.0f, 30.0f}
	                                // Try to use X=Y=Z for max compatibility.
	float planetCurveRadiusKm;      // Default is 6370.0f
	unsigned int planetCurveMode;   // Default is 0

	TgTerHeader(unsigned int num_points_x, unsigned int num_points_y)
	  : pointsX(num_points_x),
		pointsY(num_points_y),
		planetCurveRadiusKm(6370.0f),
		planetCurveMode(0)
	{
		scaleM[0] = 30.0f;
		scaleM[1] = 30.0f;
		scaleM[2] = 30.0f;
	}
};

//--------------------------------------------------------------------------------------//

class TgTerAlts
{
public:
	float* alts;                    // Pointer to an array of altitude values.
	unsigned int stride;            // Use 1 if the source/destination is an array of
	                                // tightly packed floats.
	float readMultiplier;           // Multiplier applied to alts when reading from files.
	                                // If alts are wanted in meters this should be
	                                // header->scaleM[Z], else use 1.0f to stay in point coords.
	                                // Usually readMultiplier = 1.0f/writeMultiplier.
	float writeMultiplier;          // Multiplier applied to alts when writing to files.
	                                // If source alts are in meters this should be
	                                // 1.0f/header->scaleM[Z], else use 1.0f to write raw values.
	                                // Usually writeMultiplier = 1.0f/readMultiplier.

	TgTerAlts(float* altitudes, unsigned int data_stride,
	          float alt_read_multiplier, float alt_write_multiplier)
	  : alts(altitudes),
		stride(data_stride),
		readMultiplier(alt_read_multiplier),
		writeMultiplier(alt_write_multiplier)
	{
	}
};

//--------------------------------------------------------------------------------------//

class TgTerAltRange
{
public:
	float minAlt;
	float maxAlt;

	TgTerAltRange(const float min_altitude, const float max_altitude)
		: minAlt(min_altitude), maxAlt(max_altitude)
	{
	}

	TgTerAltRange(const TgTerHeader* header, const TgTerAlts* data)
	{
		const float* alts = data->alts;

		minAlt = maxAlt = alts[0];
		int maxi = header->pointsX * header->pointsY * data->stride;
		for (int i = 0; i < maxi; i += data->stride)
		{
			if (alts[i] < minAlt) minAlt = alts[i];
			if (alts[i] > maxAlt) maxAlt = alts[i];
		}
	}
};

//--------------------------------------------------------------------------------------//

//////////////////////////////////////////////////////////////////////////////////////////
