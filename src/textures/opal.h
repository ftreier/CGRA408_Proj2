#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_TEXTURES_DAMASCUSSTEEL_H
#define PBRT_TEXTURES_DAMASCUSSTEEL_H

#include <random>
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
		OpalTexture(std::unique_ptr<TextureMapping3D> mapping, int octaves, float omega, float scale, float smootheness, float colorVariant, Spectrum mainColor, Spectrum col1, Spectrum col2)
			: _mapping(std::move(mapping)),
			  _octaves(octaves),
			  _omega(omega),
			  _scale(scale),
			  _smootheness(smootheness),
			  _colorVariant(colorVariant),
			  _mainColor(mainColor),
			  _col1(col1),
			  _col2(col2)
		{
			random_device randomDevice;
			_randGen = mt19937(randomDevice());
			_randDev = uniform_real_distribution<float>(-0.1f, 0.1f);
		}

		Spectrum Evaluate(const SurfaceInteraction &si) const override
		{
			random_device randomDevice; 
			auto randGen = mt19937(randomDevice());
			
			Vector3f dpdx, dpdy;
			Point3f p = _mapping->Map(si, &dpdx, &dpdy);
			p *= _scale;
			float opal = sin(FBm(p, _scale * dpdx, _scale * dpdy, _omega, _octaves));

			float opalX3 = opal * 3;
			float c1Factor = std::max(0.f, std::min(1.f, opal * 2));
			//float c1Factor = std::max(0.f, std::min(1.f, opal));
			//float c1Factor = std::max(1.f, std::min(0.f, opal));
			float c2Factor = std::max(0.f, std::min(1.f, abs(opal * 2)));
			float mainFactor = 1 - std::min(1.f, abs(opal) * 3);
			//return _mainColor * mainFactor + _col1 * c1Factor + _col2 * c2Factor;
			
			float f = std::min(1.f, float(abs(opal) - 0.2) * 5);
			Spectrum col;
			if (opal > 0.4)
			{
				col = _col1;
				//return _col1 + _mainColor * (1 - opal) * _smootheness;
			}
			else if(opal > 0.2)
			{
				col = _col1 * f + _mainColor * (1-f);
			}
			else if (opal < -0.4)
			{
				col = _col2;
				//return _col2 + _mainColor * (1 - abs(opal)) * _smootheness;
			}
			else if(opal < -0.2)
			{
				col = _col2 * f + _mainColor * (1 - f);
//				return _col2 + _mainColor;
			}
			else
			{
				col = _mainColor;
				//return _mainColor + (_col1 * std::min(0.f, opal) + _col2 * std::min(0.f, abs(opal))) * _smootheness;
			}

			//float t = _randDev(randGen);

			float t[3] = { _randDev(randGen), _randDev(randGen), _randDev(randGen) };

			return col + Spectrum::FromRGB(t) * _colorVariant;
			//if (opal > 0.3)
			//{
			//	return _col1;
			//	//return _col1 + _mainColor * (1 - opal) * _smootheness;
			//}
			//else if (opal < -0.3)
			//{
			//	return _col2;
			//	//return _col2 + _mainColor * (1 - abs(opal)) * _smootheness;
			//}
			//else
			//{
			//	return _mainColor;
			//	//return _mainColor + (_col1 * std::min(0.f, opal) + _col2 * std::min(0.f, abs(opal))) * _smootheness;
			//}
			//Float marble = p.y + _smootheness * FBm(p, _scale * dpdx, _scale * dpdy, _omega, _octaves);
			  //		  Float t = .5f + .5f * std::sin(marble);
			  //		  // Evaluate marble spline at _t_
			  //		  static Float c[][3] = {
			  //			  { .58f, .58f, .6f },{ .58f, .58f, .6f },{ .58f, .58f, .6f },
			  //			  { .5f, .5f, .5f },{ .6f, .59f, .58f },{ .58f, .58f, .6f },
			  //			  { .58f, .58f, .6f },{ .2f, .2f, .33f },{ .58f, .58f, .6f },
			  //		  };
			  //#define NC_2 sizeof(c) / sizeof(c[0])
			  //#define NSEG_2 (NC_2 - 3)
			  //		  int first = std::floor(t * NSEG_2);
			  //		  t = (t * NSEG_2 - first);
			  //		  Spectrum c0 = Spectrum::FromRGB(c[first]);
			  //		  Spectrum c1 = Spectrum::FromRGB(c[first + 1]);
			  //		  Spectrum c2 = Spectrum::FromRGB(c[first + 2]);
			  //		  Spectrum c3 = Spectrum::FromRGB(c[first + 3]);
			  //		  // Bezier spline evaluated with de Castilejau's algorithm
			  //		  Spectrum s0 = (1.f - t) * c0 + t * c1;
			  //		  Spectrum s1 = (1.f - t) * c1 + t * c2;
			  //		  Spectrum s2 = (1.f - t) * c2 + t * c3;
			  //		  s0 = (1.f - t) * s0 + t * s1;
			  //		  s1 = (1.f - t) * s1 + t * s2;
			  //		  // Extra _scale of 1.5 to increase _smootheness among colors
			  //		  return 1.5f * ((1.f - t) * s0 + t * s1);

		  }

	private:
		// OpalTexture Private Data
		std::unique_ptr<TextureMapping3D> _mapping;
		const int _octaves;
		const float _omega;
		const float _scale;
		const float _smootheness;
		const float _colorVariant;
		const Spectrum _mainColor;
		const Spectrum _col1;
		const Spectrum _col2;

		uniform_real_distribution<float> _randDev;
		mt19937 _randGen;
};

Texture<Float> *CreateOpalFloatTexture(const Transform &tex2world, const TextureParams &tp);
OpalTexture *CreateOpalSpectrumTexture(const Transform &tex2world, const TextureParams &tp);

}  // namespace pbrt

#endif  // PBRT_TEXTURES_DAMASCUSSTEEL_H
