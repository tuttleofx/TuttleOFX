#ifndef GIL_PIXEL_BY_CHANNEL_HPP
#define GIL_PIXEL_BY_CHANNEL_HPP

#include <boost/gil/gil_config.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/color_base_algorithm.hpp>

namespace boost { namespace gil {

/**
 * @brief Allows to use a per channel fonctor.
 */
template<template<class> class Func>
struct transform_pixel_by_channel_t
{
	template<typename Pixel>
	Pixel operator()( const Pixel& a ) const
	{
		Pixel result;
		static_for_each(
			a,
			result,
			Func<typename channel_type<Pixel>::type>()
			);
		return result;
	}
	template<typename Pixel>
	Pixel operator()( const Pixel& a, const Pixel& b ) const
	{
		Pixel result;
		static_for_each(
			a,
			b,
			result,
			Func<typename channel_type<Pixel>::type>()
			);
		return result;
	}
	template<typename Pixel>
	Pixel operator()( const Pixel& a, const Pixel& b, const Pixel& c ) const
	{
		Pixel result;
		static_for_each(
			a,
			b,
			c,
			result,
			Func<typename channel_type<Pixel>::type>()
			);
		return result;
	}
	template<typename Pixel>
	Pixel operator()( const Pixel& a, const Pixel& b, const Pixel& c, const Pixel& d ) const
	{
		Pixel result;
		static_for_each(
			a,
			b,
			c,
			d,
			result,
			Func<typename channel_type<Pixel>::type>()
			);
		return result;
	}
};


} }  // namespace boost::gil

#endif
