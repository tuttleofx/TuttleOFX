// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/gil/extension/io/bmp_io.hpp>

#include <boost/gil/extension/opencv/convert.hpp>

using namespace std;
using namespace boost;
using namespace gil;
using namespace opencv;

// Models a Unary Function
template <typename P>   // Models PixelValueConcept
struct mandelbrot_fn {
    typedef point2<ptrdiff_t>    point_t;

    typedef mandelbrot_fn        const_t;
    typedef P                    value_type;
    typedef value_type           reference;
    typedef value_type           const_reference;
    typedef point_t              argument_type;
    typedef reference            result_type;
    BOOST_STATIC_CONSTANT(bool, is_mutable=false);

    value_type                    _in_color,_out_color;
    point_t                       _img_size;
    static const int MAX_ITER=1000;        // max number of iterations

    mandelbrot_fn() {}
    mandelbrot_fn(const point_t& sz, const value_type& in_color, const value_type& out_color) : _in_color(in_color), _out_color(out_color), _img_size(sz) {}

    result_type operator()(const point_t& p) const {
        // normalize the coords to (-2..1, -1.5..1.5)
        // (actually make y -1.0..2 so it is asymmetric, so we can verify some view factory methods)
        double t=get_num_iter(point2<double>(p.x/(double)_img_size.x*3-2, p.y/(double)_img_size.y*3-1.0f));//1.5f));
        t=pow(t,0.2);

        value_type ret;
        for (int k=0; k<num_channels<P>::value; ++k)
            ret[k]=(typename channel_type<P>::type)(_in_color[k]*t + _out_color[k]*(1-t));
        return ret;
    }

private:
    double get_num_iter(const point2<double>& p) const {
        point2<double> Z(0,0);
        for (int i=0; i<MAX_ITER; ++i) {
            Z = point2<double>(Z.x*Z.x - Z.y*Z.y + p.x, 2*Z.x*Z.y + p.y);
            if (Z.x*Z.x + Z.y*Z.y > 4)
                return i/(double)MAX_ITER;
        }
        return 0;
    }
};

struct set_to_max
{
   template <typename CHANNEL > void operator()(CHANNEL& c) const
   {
      c = channel_traits< CHANNEL >::max_value();
   }
};

struct set_to_min
{
   template <typename CHANNEL > void operator()(CHANNEL& c) const
   {
      c = channel_traits< CHANNEL >::min_value();
   }
};


template< class PIXEL
        , class VIEW >
inline
PIXEL min_channel_values( const VIEW& view )
{
   PIXEL min;

   // initialize the min pixel with the max values
   static_for_each( min, set_to_max() );

   for( int y=0; y < view.height(); ++y )
   {
      VIEW::x_iterator x_it = view.row_begin( y );

      for( int x = 0; x < view.width(); ++x )
      {
         typename PIXEL::const_reference p = x_it[x];

         for( int i = 0; i < num_channels<PIXEL>::type::value; ++i )
         {
            if( dynamic_at_c( p, i ) < dynamic_at_c( min, i )) /// @todo: use ( p[i] < min[i] ), more readable...
            {
               dynamic_at_c( min, i ) = dynamic_at_c( p, i );
            }
         }
      }
   }

   return min;
}

template <typename T>
struct add_vector
{
   typedef std::vector<T> type;
};

template< class PIXEL
        , class VIEW >
inline
PIXEL min_channel_values_( const VIEW&  view
                        , const size_t percent = 1 )
{
   typedef mpl::transform1< color_space_type<VIEW>::type, add_vector<mpl::_1> >::type channels_t;

   typedef kth_semantic_element_type< PIXEL, 0 >::type channel_t; //channel_0 = semantic_at_c<0>( PIXEL() );
   channel_t max = channel_traits<channel_t>::max_value();

   return PIXEL();
}


template< class PIXEL
        , class VIEW >
inline
PIXEL max_channel_values( const VIEW& view )
{
   PIXEL max;

   // initialize the max pixel with the min values
   static_for_each( max, set_to_min() );

   // find the max values
   for( VIEW::iterator it = view.begin()
      ; it != view.end()
      ; ++it )
   {
      for( int i = 0; i < num_channels<PIXEL>::type::value; ++i )
      {
         if( dynamic_at_c( *it, i ) > dynamic_at_c( max, i ))
         {
            dynamic_at_c( max, i ) = dynamic_at_c( *it, i );
         }
      }
   }

   return max;
}

template< class PIXEL
        , class VIEW >
inline
boost::fusion::vector<PIXEL, PIXEL>
minmax_channel_values( const VIEW& view )
{
   PIXEL min, max;

   static_for_each( min, set_to_max() );
   static_for_each( max, set_to_min() );

   // find the max values
   for( VIEW::iterator it = view.begin()
      ; it != view.end()
      ; ++it )
   {
      for( int i = 0; i < num_channels<PIXEL>::type::value; ++i )
      {
         if( dynamic_at_c( *it, i ) < dynamic_at_c( min, i ))
         {
            dynamic_at_c( min, i ) = dynamic_at_c( *it, i );
         }

         if( dynamic_at_c( *it, i ) > dynamic_at_c( max, i ))
         {
            dynamic_at_c( max, i ) = dynamic_at_c( *it, i );
         }
      }
   }

   return boost::fusion::vector<PIXEL, PIXEL>( min, max );
}

struct calc_diff
{
   template <typename CHANNEL > void operator()( CHANNEL&       diff
                                               , const CHANNEL& min
                                               , const CHANNEL& max ) const
   {
      diff = max - min;
   }
};

typedef channel_type<rgb16_pixel_t>::type channel_t;

template< class SRC
        , class DST
        >
void down_sample( const SRC& src_view
                , const DST& dst_view  )
{
   assert( src_view.dimensions() == dst_view.dimensions() );

   // @todo Take care of signed images. Bransform them into unsigned images
   // by adding half the value range to the channels.

   typedef SRC::value_type src_pixel_t;
   typedef DST::value_type dst_pixel_t;
   typedef channel_type<dst_pixel_t>::type dst_channel_t;

   boost::fusion::vector<src_pixel_t,src_pixel_t> minmax = minmax_channel_values<src_pixel_t>( src_view );

   const src_pixel_t& min = at_c<0>( minmax );
   const src_pixel_t& max = at_c<1>( minmax );

   // calculate the diff
   src_pixel_t diff;
   static_for_each( diff, min, max, calc_diff() );


   // sample down
   dst_channel_t dst_max = channel_traits< dst_channel_t >::max_value();

   for( int y=0; y < src_view.height(); ++y )
   {
      SRC::x_iterator src_it = src_view.row_begin( y );
      DST::x_iterator dst_it = dst_view.row_begin( y );

      for( int x = 0; x < src_view.width(); ++x )
      {
         typename src_pixel_t::const_reference src = src_it[x];
         typename dst_pixel_t::reference       dst = dst_it[x];

         for( int i = 0; i < num_channels<src_pixel_t>::type::value; ++i )
         {
            if( dynamic_at_c( diff, i ) == 0 )
            {
               dynamic_at_c( dst, i ) = 0;
            }
            else
            {
               dynamic_at_c( dst, i ) = static_cast<dst_channel_t>( dst_max * ( static_cast<float>( dynamic_at_c( src, i ) 
                                                                                                  - dynamic_at_c( min, i )) 
                                                                              / static_cast<float>( dynamic_at_c( diff, i ))));
            } // else

         } //for

      } //for

   } // for
}

// channel_wise calculation.

// @todo need better name
template< typename DST_MAX >
struct foo
{
   foo( const DST_MAX& dst_max )
   : _dst_max( dst_max ){}

   template < typename DST_CHANNEL
            , typename SRC_CHANNEL >
   void operator()( DST_CHANNEL&  dst
                  , SRC_CHANNEL&  src
                  , SRC_CHANNEL&  min
                  , SRC_CHANNEL&  diff )
   const
   {
      if( diff == 0 )
      {
         dst = 0;

         return;
      }

      float d = ( static_cast<float>( _dst_max )
                * ( ( static_cast<float>( src ) - static_cast<float>( min ))
                  / static_cast<float>( diff )));

      dst_channel = static_cast<DST_CHANNEL>( dst );
   }

   DST_MAX _dst_max;
};

// pixel_wise calculation.

// @todo need better name
template< typename SRC_VIEW
        , typename SRC_PIXEL
        , typename DST_VIEW
        , typename DST_MAX
        >
struct do_it
{
   do_it( const DST_VIEW&  dst_view
        , const DST_MAX&   dst_max
        , const SRC_PIXEL& min
        , const SRC_PIXEL& diff      )
   : _dst_view( dst_view )
   , _min( min )
   , _diff( diff )
   , _op( dst_max )
   {
      _dst_it = dst_view.begin();
   }

   void operator()( SRC_PIXEL& src ) 
   {
      static_for_each( *_dst_it
                     , src
                     , _min
                     , _diff
                     , _op    );
      ++_dst_it;
   }

   typename DST_VIEW::iterator _dst_it;

   foo<DST_MAX> _op;

   DST_VIEW _dst_view;

   SRC_PIXEL _min;
   SRC_PIXEL _diff;
};

int main(int argc, char* argv[])
{
   {
      rgb8_image_t dst( 10, 10 );

      min_channel_values<rgb8_image_t::value_type>( view( dst ));
   }

   {
/*
      bits8 max = 255;
      rgb16_pixel_t min;
      rgb16_pixel_t diff;


      rgb16_image_t src( 640, 480 );
      rgb8_image_t dst( 640, 480 );


      do_it< rgb16_view_t
           , rgb16_pixel_t
           , rgb8_view_t
           , bits8
           > d( view( dst ), max, min, diff );

      for_each_pixel( view( src ), d );

      bmp_write_view( ".\\red.bmp", view( dst ));
*/
   }

   {
      typedef rgb16_image_t src_image_t;
      typedef src_image_t::view_t src_view_t;
      typedef src_view_t::value_type src_pixel_t;
      typedef channel_type<src_pixel_t>::type src_channel_t;
      src_channel_t max_value = channel_traits<src_channel_t>::max_value();
      

      typedef mandelbrot_fn<src_pixel_t> deref_t;
      typedef deref_t::point_t           point_t;

      typedef virtual_2d_locator<deref_t,false> locator_t;
      typedef image_view<locator_t> my_virt_view_t;

      function_requires<PixelLocatorConcept<locator_t> >();
      gil_function_requires<StepIteratorConcept<locator_t::x_iterator> >();

      point_t dims( 640, 480 );
      my_virt_view_t mandel( dims
                           , locator_t( point_t( 0, 0 )
                           , point_t( 1 , 1)
                           , deref_t( dims
                                    , src_pixel_t( max_value,         0, 0 )
                                    , src_pixel_t(         0, max_value, 0 ))));


      src_image_t img( dims );
      copy_pixels( mandel, view( img ));

      // @todo How to compute the unsigned xxx8_image_t from src_image_t?
      rgb8_image_t eight_bit_img( dims );

      down_sample( view( img )
                 , view( eight_bit_img ));

      bmp_write_view( ".\\mandelbrot.bmp", view( eight_bit_img ));
   }

  	return 0;
}
