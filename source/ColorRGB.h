#pragma once
#include "MathHelpers.h"

namespace dae
{
	struct ColorRGB
	{
		float r{};
		float g{};
		float b{};

		void MaxToOne()
		{
			const float maxValue = std::max(r, std::max(g, b));
			if (maxValue > 1.f)
				*this /= maxValue;
		}

		static ColorRGB Lerp(const ColorRGB& c1, const ColorRGB& c2, float factor)
		{
			return { Lerpf(c1.r, c2.r, factor), Lerpf(c1.g, c2.g, factor), Lerpf(c1.b, c2.b, factor) };
		}

		#pragma region ColorRGB (Member) Operators
		const ColorRGB& operator+=(const ColorRGB& c)
		{
			r += c.r;
			g += c.g;
			b += c.b;

			return *this;
		}

		const ColorRGB& operator+(const ColorRGB& c)
		{
			return *this += c;
		}

		ColorRGB operator+(const ColorRGB& c) const
		{
			return { r + c.r, g + c.g, b + c.b };
		}

		const ColorRGB& operator-=(const ColorRGB& c)
		{
			r -= c.r;
			g -= c.g;
			b -= c.b;

			return *this;
		}

		const ColorRGB& operator-(const ColorRGB& c)
		{
			return *this -= c;
		}

		ColorRGB operator-(const ColorRGB& c) const
		{
			return { r - c.r, g - c.g, b - c.b };
		}

		const ColorRGB& operator*=(const ColorRGB& c)
		{
			r *= c.r;
			g *= c.g;
			b *= c.b;

			return *this;
		}

		const ColorRGB& operator*(const ColorRGB& c)
		{
			return *this *= c;
		}

		ColorRGB operator*(const ColorRGB& c) const
		{
			return { r * c.r, g * c.g, b * c.b };
		}

		const ColorRGB& operator/=(const ColorRGB& c)
		{
			r /= c.r;
			g /= c.g;
			b /= c.b;

			return *this;
		}

		const ColorRGB& operator/(const ColorRGB& c)
		{
			return *this /= c;
		}

		const ColorRGB& operator*=(float s)
		{
			r *= s;
			g *= s;
			b *= s;

			return *this;
		}

		const ColorRGB& operator*(float s)
		{
			return *this *= s;
		}

		ColorRGB operator*(float s) const
		{
			return { r * s, g * s,b * s };
		}

		const ColorRGB& operator/=(float s)
		{
			r /= s;
			g /= s;
			b /= s;

			return *this;
		}

		const ColorRGB& operator/(float s)
		{
			return *this /= s;
		}
		#pragma endregion
	};

	//ColorRGB (Global) Operators
	inline ColorRGB operator*(float s, const ColorRGB& c)
	{
		return c * s;
	}

	namespace colors
	{
		static ColorRGB Red{ 1,0,0 };
		static ColorRGB Blue{ 0,0,1 };
		static ColorRGB Green{ 0,1,0 };
		static ColorRGB Yellow{ 1,1,0 };
		static ColorRGB Cyan{ 0,1,1 };
		static ColorRGB Magenta{ 1,0,1 };
		static ColorRGB White{ 1,1,1 };
		static ColorRGB Black{ 0,0,0 };
		static ColorRGB Gray{ 0.5f,0.5f,0.5f };
	}

	namespace fresnel
	{
		static ColorRGB Plastic		{ 0.040f, 0.040f, 0.040f };

		static ColorRGB Titanium	{ 0.542f, 0.497f, 0.449f };
		static ColorRGB Chromium	{ 0.549f, 0.556f, 0.554f };
		static ColorRGB Iron		{ 0.562f, 0.565f, 0.578f };
		static ColorRGB Nickel		{ 0.660f, 0.609f, 0.526f };
		static ColorRGB Platinum	{ 0.673f, 0.637f, 0.585f };
		static ColorRGB Copper		{ 0.955f, 0.638f, 0.538f };
		static ColorRGB Palladium	{ 0.733f, 0.697f, 0.652f };
		static ColorRGB Mercury		{ 0.781f, 0.780f, 0.778f };
		static ColorRGB Brass		{ 0.910f, 0.778f, 0.423f };
		static ColorRGB Zinc		{ 0.664f, 0.824f, 0.850f };
		static ColorRGB Gold		{ 1.000f, 0.782f, 0.344f };
		static ColorRGB Aluminum	{ 0.913f, 0.922f, 0.924f };
		static ColorRGB Silver		{ 0.972f, 0.960f, 0.915f };
	}
}