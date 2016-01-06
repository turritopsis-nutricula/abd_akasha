#ifndef AKASHA_WORLD_TYPE__H
#define AKASHA_WORLD_TYPE__H

//std
#include <string>
#include <cstring>

//bullet
#include "btBulletDynamicsCommon.h"

//irrlicht
#include "irrlicht.h"




namespace akasha {

	//基礎定数
	constexpr const float ELEMENT_MASS = 25.f; //!< エレメント単位重さ
	constexpr const float ELEMENT_SIZE = 0.75f; //!< エレメント単位大きさ
	constexpr const float ELEMENT_THICK = 0.1f; //!< エレメント単位厚さ

	//基礎変数型
	typedef int ElementID;        //!< エレメントID
};


namespace akasha {

	//! charからwchar_t型への変換。deleteを忘れぬよう。
	inline const wchar_t* toWchar( const char* c ) {
		size_t len = (strlen(c) + 1)*2;
		wchar_t* wc = new wchar_t[ len ];
		//mbstowcs_s(&wLen, wc, len, c, _TRUNCATE);
		::mbstowcs(wc,c, len);
		return wc;
	}

	//! HSLカラークラス
	/** IrrのSColorHSLから。使いものにならないからこっちで作る
	*/
	class HSLColor
	{
		public:
			HSLColor ( irr::f32 h = 0.f, irr::f32 s = 0.f, irr::f32 l = 0.f )
				: Hue ( h ), Saturation ( s ), Luminance ( l ) {};

			HSLColor (const irr::video::SColor &color) {
				fromRGB( color );
			};

			HSLColor(const irr::video::SColorf &color){
				fromRGB( color.toSColor() );
			};

			void fromRGB(const irr::video::SColor &color);
			void toRGB(irr::video::SColor &color) const;

			HSLColor& operator *=(const irr::f32& t){
				Luminance *= t;
				return *this;
			};
			HSLColor operator*(const irr::f32& t){
				HSLColor c = HSLColor(*this);
				c *= t;
				return c;
			};
			irr::f32 Hue;
			irr::f32 Saturation;
			irr::f32 Luminance;

		private:
			inline irr::u32 toRGB1(irr::f32 rm1, irr::f32 rm2, irr::f32 rh) const;

	};

	inline void HSLColor::fromRGB(const irr::video::SColor &color)
	{

		irr::f32 r = color.getRed()/255.f;
		irr::f32 g = color.getGreen()/255.f;
		irr::f32 b =  color.getBlue()/255.f;

		const irr::f32 maxVal = (irr::f32)irr::core::max_(r, g, b);
		const irr::f32 minVal = (irr::f32)irr::core::min_(r, g, b);
		//Luminance = (maxVal/minVal)*0.5f;
		Luminance = (maxVal+minVal)*0.5f;
		if (irr::core::equals(maxVal, minVal))
		{
			Hue=0.f;
			Saturation=0.f;
			return;
		}

		const irr::f32 delta = maxVal-minVal;
		if ( Luminance <= 0.5f )
		{
			Saturation = (delta)/(maxVal+minVal);
		}
		else
		{
			Saturation = (delta)/(2.f-maxVal-minVal);
			//Saturation = (delta)/(310-maxVal-minVal);
		}

		if (maxVal==r)
			Hue = (g-b)/delta;
		else if (maxVal==g)
			Hue = 2+(b-r)/delta;
		else if (maxVal==b)
			Hue = 4+(r-g)/delta;

		Hue *= (60.0f * irr::core::DEGTORAD);
		while ( Hue < 0.f )
			Hue += 2.f * irr::core::PI;
	}


	inline void HSLColor::toRGB(irr::video::SColor &color) const
	{
		if (irr::core::iszero(Saturation)) // grey
		{
			irr::u8 c = (irr::u8) ( Luminance * 255.0 );
			color.setRed(c);
			color.setGreen(c);
			color.setBlue(c);
			return;
		}

		irr::f32 rm2;

		if ( Luminance <= 0.5f )
		{
			rm2 = Luminance + Luminance * Saturation;
		}
		else
		{
			rm2 = Luminance + Saturation - Luminance * Saturation;
		}

		const irr::f32 rm1 = 2.0f * Luminance - rm2;

		color.setRed ( toRGB1(rm1, rm2, Hue + (120.0f * irr::core::DEGTORAD )) );
		color.setGreen ( toRGB1(rm1, rm2, Hue) );
		color.setBlue ( toRGB1(rm1, rm2, Hue - (120.0f * irr::core::DEGTORAD) ) );
	}


	inline irr::u32 HSLColor::toRGB1(irr::f32 rm1, irr::f32 rm2, irr::f32 rh) const
	{
		while ( rh > 2.f * irr::core::PI )
			rh -= 2.f * irr::core::PI;

		while ( rh < 0.f )
			rh += 2.f * irr::core::PI;

		if (rh < 60.0f * irr::core::DEGTORAD )
			rm1 = rm1 + (rm2 - rm1) * rh / (60.0f * irr::core::DEGTORAD);
		else if (rh < 180.0f * irr::core::DEGTORAD )
			rm1 = rm2;
		else if (rh < 240.0f * irr::core::DEGTORAD )
			rm1 = rm1 + (rm2 - rm1) * ( ( 240.0f * irr::core::DEGTORAD ) - rh) /
				(60.0f * irr::core::DEGTORAD);

		return (irr::u32) (rm1 * 255.f);
	}



}

#endif //AKASHA_WORLD_TYPE__H
