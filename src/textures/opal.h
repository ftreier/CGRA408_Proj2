#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_TEXTURES_OPAL_H
#define PBRT_TEXTURES_OPAL_H

#include "pbrt.h"
#include "texture.h"
#include "paramset.h"

using namespace std;
namespace pbrt {

// OpalTexture Declarations
class OpalTexture : public Texture<Spectrum>
{
	public:
	// OpalTexture Public Methods
		OpalTexture(unique_ptr<TextureMapping3D> mapping, int octaves, float omega, float scale, float smootheness, float colorVariant, Spectrum mainColor, Spectrum col1, Spectrum col2)
			: _mapping(move(mapping)),
			  _octaves(octaves),
			  _omega(omega),
			  _scale(scale),
			  _smootheness(smootheness),
			  _colorVariant(colorVariant),
			  _mainColor(mainColor),
			  _col1(col1),
			  _col2(col2)
		{
		}

		Spectrum Evaluate(const SurfaceInteraction &si) const override
		{
			Vector3f dpdx, dpdy;
			Point3f p = _mapping->Map(si, &dpdx, &dpdy);
			p *= _scale;
			float opal = sin(FBm(p, _scale * dpdx, _scale * dpdy, _omega, _octaves));

			float f = min(1.f, float(abs(opal) - 0.2) * 5);
			Spectrum col;
			if (opal > 0.4)
			{
				col = _col1 + _col1 * _colorVariant * (opal - 0.4);
			}
			else if (opal > 0.2)
			{
				col = _col1 * f + _mainColor * (1 - f);
			}
			else if (opal < -0.4)
			{
				col = _col2 + _col2 * _colorVariant * (abs(opal) - 0.4);
			}
			else if (opal < -0.2)
			{
				col = _col2 * f + _mainColor * (1 - f);
			}
			else
			{
				col = _mainColor + _mainColor * _colorVariant * abs(opal);
			}

			return col;
		}

	private:
		// OpalTexture Private Data
		unique_ptr<TextureMapping3D> _mapping;
		const int _octaves;
		const float _omega;
		const float _scale;
		const float _smootheness;
		const float _colorVariant;
		const Spectrum _mainColor;
		const Spectrum _col1;
		const Spectrum _col2;
};

Texture<Float> *CreateOpalFloatTexture(const Transform &tex2world, const TextureParams &tp);
OpalTexture *CreateOpalSpectrumTexture(const Transform &tex2world, const TextureParams &tp);

}  // namespace pbrt

#endif  // PBRT_TEXTURES_OPAL_H
