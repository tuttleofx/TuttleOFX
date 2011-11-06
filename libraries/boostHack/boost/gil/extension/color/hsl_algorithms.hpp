#ifndef HSL_ALGORITHMS_HPP
#define HSL_ALGORITHMS_HPP

#include <boost/gil/gil_all.hpp>

namespace boost { namespace gil {
namespace toolbox {

template< typename HSL_VIEW >
void shift_hue( const HSL_VIEW& img
              , const bits32f   value )
{
   // make sure src and dst are views
   // make sure src and dst are either hsl or hsv

   for( int y = 0; y < img.height(); ++y )
   {
      HSL_VIEW::x_iterator img_it = img.row_begin( y );

      for( int x = 0; x < img.width(); ++x )
      {
         bits32f& hue = get_color( img_it[x], boost::gil::hsl_color_space::hue_t() );

         hue += value;

         if( hue > 1.f )
         {
            hue -= 1.f;
         }

         if( hue < 0.f )
         {
            hue = 0.f;
         }
      }
   }
}


} //namespace toolbox
} //namespace gil
} //namespace boost

#endif // HSL_ALGORITHMS_HPP