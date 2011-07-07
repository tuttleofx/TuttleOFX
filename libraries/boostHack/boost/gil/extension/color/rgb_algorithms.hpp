#ifndef RGB_ALGORITHMS_HPP
#define RGB_ALGORITHMS_HPP

#include <boost/gil/gil_all.hpp>

namespace boost { namespace gil {
namespace toolbox {

struct calc_negative
{
   template < typename DST_CHANNEL
            , typename SRC_CHANNEL
            > void operator()( DST_CHANNEL& dst 
                             , SRC_CHANNEL& src ) const
   {
      SRC_CHANNEL max = channel_traits< SRC_CHANNEL >::max_value();

      dst = max - src;
   }

   template < typename CHANNEL
            > void operator()( CHANNEL& c ) const
   {
      CHANNEL max = channel_traits< CHANNEL >::max_value();

      c = max - c;
   }
};

template< typename RGB_VIEW >
void negative( const RGB_VIEW& view )
{
   typedef typename channel_type< RGB_VIEW >::type channel_t;
   channel_t max = channel_traits<channel_t>::max_value();

   for( int y=0; y < view.height(); ++y )
   {
      typename RGB_VIEW::x_iterator it = view.row_begin( y );

      for( int x = 0; x < view.width(); ++x )
      {
         static_for_each( it[x], calc_negative() );
      }
   }
}

template< typename RGB_VIEW >
void negative( const RGB_VIEW& src
             , const RGB_VIEW& dst )
{
   // make sure src and dst have same dimensions.

   // for rgba do it only for rgb channels

   typedef typename channel_type< RGB_VIEW >::type channel_t;
   channel_t max = channel_traits<channel_t>::max_value();

   for( int y=0; y < src.height(); ++y )
   {
      typename RGB_VIEW::x_iterator src_it = src.row_begin( y );
      typename RGB_VIEW::x_iterator dst_it = dst.row_begin( y );

      for( int x = 0; x < src.width(); ++x )
      {
         static_for_each( dst_it[x], src_it[x], calc_negative() );
      }
   }
}

struct calc_brightness
{
   float _factor;

   template < typename DST_CHANNEL
            , typename SRC_CHANNEL
            > void operator()( DST_CHANNEL&       dst 
                             , const SRC_CHANNEL& src ) const
   {
      float d = static_cast<float>( src ) * _factor;

      if( d > channel_traits< DST_CHANNEL >::max_value() )
      {
         dst = channel_traits< DST_CHANNEL >::max_value();
      }
      else
      {
         dst = static_cast< DST_CHANNEL >( d );
      }
   }
};



template< typename RGB_VIEW >
void brightness( const RGB_VIEW& src
               , const RGB_VIEW& dst
               , float           factor )
{
   calc_brightness calc;
   calc._factor = factor;

   for( int y=0; y < src.height(); ++y )
   {
      typename RGB_VIEW::x_iterator src_it = src.row_begin( y );
      typename RGB_VIEW::x_iterator dst_it = dst.row_begin( y );

      for( int x = 0; x < src.width(); ++x )
      {
         static_for_each( dst_it[x], src_it[x], calc );
      }
   }
}

template< typename VALUE >
struct calc_brightness_
{
   VALUE _number;

   template < typename DST_CHANNEL
            , typename SRC_CHANNEL
            > void operator()( DST_CHANNEL&       dst 
                             , const SRC_CHANNEL& src ) const
   {
      VALUE d = static_cast< VALUE >( src ) + _number;

      if( d > channel_traits< DST_CHANNEL >::max_value() )
      {
         dst = channel_traits< DST_CHANNEL >::max_value();
      }
      else
      {
         dst = d;
      }
   }
};

template< typename RGB_VIEW
        , typename VALUE >
void brightness_( const RGB_VIEW& src
                , const RGB_VIEW& dst
                , VALUE           number )
{
   calc_brightness_<VALUE> calc;
   calc._number = number;

   for( int y=0; y < src.height(); ++y )
   {
      typename RGB_VIEW::x_iterator src_it = src.row_begin( y );
      typename RGB_VIEW::x_iterator dst_it = dst.row_begin( y );

      for( int x = 0; x < src.width(); ++x )
      {
         static_for_each( dst_it[x], src_it[x], calc );
      }
   }
}

template< typename RGB_VIEW >
void remove_channel( const RGB_VIEW& src
                   , unsigned int    c   )
{
   typedef typename channel_type< RGB_VIEW >::type channel_t;
   channel_t min_value = channel_traits<channel_t>::min_value();

   for( int y = 0; y < src.dimensions().y; ++y )
   {
      typename RGB_VIEW::x_iterator src_it = src.row_begin( y );

      for( int x = 0; x < src.dimensions().x; ++x )
      {
         dynamic_at_c( src_it[x], c ) = min_value;
      }
   }
}

template< typename CHANNEL
        , typename RGB_VIEW >
void remove_channel( const RGB_VIEW& src )
{
   typedef typename channel_type< RGB_VIEW >::type channel_t;
   channel_t min = channel_traits<channel_t>::min_value();

   for( int y = 0; y < src.dimensions().y; ++y )
   {
      typename RGB_VIEW::x_iterator src_it = src.row_begin( y );

      for( int x = 0; x < src.dimensions().x; ++x )
      {
         get_color( src_it[x], CHANNEL() ) = min;
      }
   }
}


template< typename RGB_VIEW >
void difference( const RGB_VIEW& src_1
               , const RGB_VIEW& src_2
               , const RGB_VIEW& dst    )
{
   if( src_1.dimensions() != src_2.dimensions() )
   {
      BOOST_THROW_EXCEPTION( std::runtime_error( "Source image have different dimensions." ) );
   }

   if( src_1.dimensions() != dst.dimensions() )
   {
      BOOST_THROW_EXCEPTION( std::runtime_error( "Destination image must have equal dimensions as the source images." ) );
   }

   for( int y = 0; y < src_1.dimensions().y; ++y )
   {
      typename RGB_VIEW::x_iterator src_1_it = src_1.row_begin( y );
      typename RGB_VIEW::x_iterator src_2_it = src_2.row_begin( y );

      typename RGB_VIEW::x_iterator dst_it = dst.row_begin( y );

      typedef typename channel_type<RGB_VIEW>::type channel_t;

      for( int x = 0; x < src_1.dimensions().x; ++x )
      {
			 assert(false);
		//TODO: what is calc_diff?
         //static_for_each( src_1_it[x], src_1_it[x], dst_it[x], calc_diff() );
      }
   }
}

} //namespace toolbox
} //namespace gil
} //namespace boost

#endif // RGB_ALGORITHMS_HPP
