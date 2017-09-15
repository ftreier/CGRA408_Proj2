
/*
	pbrt source code is Copyright(c) 1998-2016
						Matt Pharr, Greg Humphreys, and Wenzel Jakob.

	This file is part of pbrt.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are
	met:

	- Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.

	- Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
	IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
	TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */


// textures/marble.cpp*
#include "textures/opal.h"

namespace pbrt
{

	// MarbleTexture Method Definitions
	Texture<Float> *CreateOpalFloatTexture(const Transform &tex2world, const TextureParams &tp)
	{
		return nullptr;
	}

	OpalTexture *CreateOpalSpectrumTexture(const Transform &tex2world, const TextureParams &tp)
	{
		// Initialize 3D texture _mapping _map_ from _tp_
		std::unique_ptr<TextureMapping3D> map(new IdentityMapping3D(tex2world));
		float mc[3] = { 147.0 / 255.0, 160.0 / 255.0, 223.0 / 255.0 };
		auto mainColor = tp.FindSpectrum("mainColor", Spectrum::FromRGB(mc));
		float c1[3] = { 240.0 / 255.0, 162.0 / 255.0, 177.0 / 255.0 };
		auto col1 = tp.FindSpectrum("col1", Spectrum::FromRGB(c1));
		float c2[3] = { 132.0 / 255.0, 235.0 / 255.0, 162.0 / 255.0 };
		auto col2 = tp.FindSpectrum("col2", Spectrum::FromRGB(c2));

		return new OpalTexture(std::move(map), tp.FindInt("octaves", 8), tp.FindFloat("roughness", .5f),
			tp.FindFloat("scale", 1.f), tp.FindFloat("smoothness", .2f), tp.FindFloat("colorVariant", .5f), mainColor, col1, col2);
	}

}  // namespace pbrt
