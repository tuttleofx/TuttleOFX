// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef _wuline_hpp_
#define _wuline_hpp_

#include "blend.hpp"

template <typename view_t, typename value_type_t> inline 
void wuline(const view_t& view, const value_type_t& pixel, 
	int X0, int Y0, int X1, int Y1, int NumLevels=256, int IntensityBits=8)
{
	using namespace boost::gil;
	unsigned short IntensityShift, ErrorAdj, ErrorAcc;
	unsigned short ErrorAccTemp, Weighting, WeightingComplementMask;
	short DeltaX, DeltaY, Temp, XDir;

	if (Y0 > Y1) 
	{
  		Temp = Y0; Y0 = Y1; Y1 = Temp;
  		Temp = X0; X0 = X1; X1 = Temp;
	}

	view(X0,Y0) = pixel;

	if ((DeltaX = X1 - X0) >= 0) 
	{
  		XDir = 1;
	} 
	else 
	{
		XDir = -1;
		DeltaX = -DeltaX; 
	}

	if ((DeltaY = Y1 - Y0) == 0) 
	{
  		while (DeltaX-- != 0) 
		{
			X0 += XDir;
			view(X0,Y0) = pixel;
  		}
      		
		return;
	}
	
	if (DeltaX == 0) 
	{
		do 
		{
			Y0++;
			view(X0,Y0) = pixel;
		} 
		while (--DeltaY != 0);

		return;
	}

	if (DeltaX == DeltaY) 
	{
		do 
		{
			X0 += XDir;
			Y0++;
			view(X0,Y0) = pixel;
		} 
		while (--DeltaY != 0);

		return;
	}

	ErrorAcc = 0;  
	IntensityShift = 16 - IntensityBits;
	WeightingComplementMask = NumLevels - 1;

	if (DeltaY > DeltaX) 
	{
		ErrorAdj = ((unsigned long) DeltaX << 16) / (unsigned long) DeltaY;

		while (--DeltaY) 
		{
			ErrorAccTemp = ErrorAcc;   
			ErrorAcc += ErrorAdj;     
         	
			if (ErrorAcc <= ErrorAccTemp) 
				X0 += XDir;
         		
			Y0++;

			Weighting = ErrorAcc >> IntensityShift;
	
			value_type_t dst = pixel;
			static_for_each(dst,view(X0,Y0), 
				alpha24_blend((Weighting ^ WeightingComplementMask)));
			view(X0,Y0) = dst;

			dst = pixel;
			static_for_each(dst,view(X0 + XDir, Y0), alpha24_blend(Weighting));
			view(X0 + XDir, Y0) = dst;
  		}

		view(X1,Y1) = pixel;
		return;
	}

	ErrorAdj = ((unsigned long) DeltaY << 16) / (unsigned long) DeltaX;

	while (--DeltaX) 
	{
		ErrorAccTemp = ErrorAcc;   
		ErrorAcc += ErrorAdj;     
      		
		if (ErrorAcc <= ErrorAccTemp) 
			Y0++;

		X0 += XDir; 
      		
		Weighting = ErrorAcc >> IntensityShift;

		value_type_t dst = pixel;
		static_for_each(dst,view(X0,Y0), 
			alpha24_blend(Weighting ^ WeightingComplementMask));
		view(X0,Y0) = dst;
	
		dst = pixel;
		static_for_each(dst,view(X0, Y0 + 1), 
			alpha24_blend(Weighting));
		view(X0, Y0 + 1) = dst;
	}

	view(X1,Y1) = pixel;
}

#endif
