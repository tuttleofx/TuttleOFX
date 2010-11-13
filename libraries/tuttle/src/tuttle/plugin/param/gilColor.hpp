#ifndef _TUTTLE_PLUGIN_PARAM_GILCOLOR_HPP_
#define _TUTTLE_PLUGIN_PARAM_GILCOLOR_HPP_

#include <ofxsParam.h>
#include <boost/gil/color_base_algorithm.hpp>
#include <boost/gil/rgba.hpp>
#include <boost/gil/channel.hpp>

namespace tuttle {
namespace plugin {

template <typename Pixel>
void setRGBAParamValuesAtTime( OFX::RGBAParam& param, const OfxTime time, const Pixel& pixel  )
{
	using namespace boost::gil;
	param.setValueAtTime(
		time,
		get_color( pixel, red_t() ),
		get_color( pixel, green_t() ),
		get_color( pixel, blue_t() ),
		get_color( pixel, alpha_t() )
		);
}
template <typename Pixel>
void setRGBAParamValuesAtTime( OFX::RGBAParam* param, const OfxTime time, const Pixel& pixel  )
{
	setRGBAParamValuesAtTime<Pixel>( *param, time, pixel  );
}
template <typename Pixel>
void setRGBParamValuesAtTime( OFX::RGBAParam& param, const OfxTime time, const Pixel& pixel  )
{
	using namespace boost::gil;
	typedef typename boost::gil::channel_type<Pixel>::type Channel;
	param.setValueAtTime(
		time,
		get_color( pixel, red_t() ),
		get_color( pixel, green_t() ),
		get_color( pixel, blue_t() ),
		channel_traits<Channel>::max_value()
		);
}
template <typename Pixel>
void setRGBParamValuesAtTime( OFX::RGBAParam* param, const OfxTime time, const Pixel& pixel  )
{
	setRGBParamValuesAtTime<Pixel>( *param, time, pixel  );
}

template <typename Pixel>
void setRGBAParamValues( OFX::RGBAParam& param, const Pixel& pixel  )
{
	using namespace boost::gil;
	param.setValue(
		get_color( pixel, red_t() ),
		get_color( pixel, green_t() ),
		get_color( pixel, blue_t() ),
		get_color( pixel, alpha_t() )
		);
}
template <typename Pixel>
void setRGBAParamValues( OFX::RGBAParam* param, const Pixel& pixel  )
{
	setRGBAParamValues( *param, pixel  );
}

template <typename Pixel>
void setRGBParamValues( OFX::RGBAParam& param, const Pixel& pixel  )
{
	using namespace boost::gil;
	typedef typename boost::gil::channel_type<Pixel>::type Channel;
	param.setValue(
		get_color( pixel, red_t() ),
		get_color( pixel, green_t() ),
		get_color( pixel, blue_t() ),
		channel_traits<Channel>::max_value()
		);
}
template <typename Pixel>
void setRGBParamValues( OFX::RGBAParam* param, const Pixel& pixel  )
{
	setRGBParamValues<Pixel>( *param, pixel  );
}

template <typename Pixel>
void setRGBParamValuesAtTime( OFX::RGBParam& param, const OfxTime time, const Pixel& pixel  )
{
	using namespace boost::gil;
	param.setValueAtTime(
		time,
		get_color( pixel, red_t() ),
		get_color( pixel, green_t() ),
		get_color( pixel, blue_t() )
		);
}
template <typename Pixel>
void setRGBParamValuesAtTime( OFX::RGBParam* param, const OfxTime time, const Pixel& pixel  )
{
	setRGBParamValuesAtTime<Pixel>( *param, time, pixel  );
}

template <typename Pixel>
void setRGBParamValues( OFX::RGBParam& param, const Pixel& pixel  )
{
	using namespace boost::gil;
	param.setValue(
		get_color( pixel, red_t() ),
		get_color( pixel, green_t() ),
		get_color( pixel, blue_t() )
		);
}
template <typename Pixel>
void setRGBParamValues( OFX::RGBParam* param, const Pixel& pixel  )
{
	setRGBParamValues<Pixel>( *param, pixel  );
}

template <typename Pixel>
void setRGBParamValuesAtTime( OFX::Double3DParam& param, const OfxTime time, const Pixel& pixel  )
{
	using namespace boost::gil;
	param.setValueAtTime(
		time,
		get_color( pixel, red_t() ),
		get_color( pixel, green_t() ),
		get_color( pixel, blue_t() )
		);
}

template <typename Pixel>
void setRGBParamValuesAtTime( OFX::Double3DParam* param, const OfxTime time, const Pixel& pixel  )
{
	setRGBParamValuesAtTime<Pixel>( *param, time, pixel  );
}

template <typename Pixel>
void setRGBParamValues( OFX::Double3DParam& param, const Pixel& pixel  )
{
	using namespace boost::gil;
	param.setValue(
		get_color( pixel, red_t() ),
		get_color( pixel, green_t() ),
		get_color( pixel, blue_t() )
		);
}

template <typename Pixel>
void setRGBParamValues( OFX::Double3DParam* param, const Pixel& pixel  )
{
	setRGBParamValues<Pixel>( *param, pixel );
}


template <typename Pixel>
void set0123ParamValuesAtTime( OFX::RGBAParam& param, const OfxTime time, const Pixel& pixel  )
{
	using namespace boost::gil;
	param.setValueAtTime(
		time,
		pixel[0],
		pixel[1],
		pixel[2],
		pixel[3]
		);
}
template <typename Pixel>
void set0123ParamValuesAtTime( OFX::RGBAParam* param, const OfxTime time, const Pixel& pixel  )
{
	set0123ParamValuesAtTime<Pixel>( *param, time, pixel  );
}
template <typename Pixel>
void set012ParamValuesAtTime( OFX::RGBAParam& param, const OfxTime time, const Pixel& pixel  )
{
	using namespace boost::gil;
	typedef typename boost::gil::channel_type<Pixel>::type Channel;
	param.setValueAtTime(
		time,
		pixel[0],
		pixel[1],
		pixel[2],
		channel_traits<Channel>::max_value()
		);
}
template <typename Pixel>
void set012ParamValuesAtTime( OFX::RGBAParam* param, const OfxTime time, const Pixel& pixel  )
{
	set012ParamValuesAtTime<Pixel>( *param, time, pixel  );
}

template <typename Pixel>
void set0123ParamValues( OFX::RGBAParam& param, const Pixel& pixel  )
{
	using namespace boost::gil;
	param.setValue(
		pixel[0],
		pixel[1],
		pixel[2],
		pixel[3]
		);
}
template <typename Pixel>
void set0123ParamValues( OFX::RGBAParam* param, const Pixel& pixel  )
{
	set0123ParamValues( *param, pixel  );
}

template <typename Pixel>
void set012ParamValues( OFX::RGBAParam& param, const Pixel& pixel  )
{
	using namespace boost::gil;
	typedef typename boost::gil::channel_type<Pixel>::type Channel;
	param.setValue(
		pixel[0],
		pixel[1],
		pixel[2],
		channel_traits<Channel>::max_value()
		);
}
template <typename Pixel>
void set012ParamValues( OFX::RGBAParam* param, const Pixel& pixel  )
{
	set012ParamValues<Pixel>( *param, pixel  );
}

template <typename Pixel>
void set012ParamValuesAtTime( OFX::RGBParam& param, const OfxTime time, const Pixel& pixel  )
{
	using namespace boost::gil;
	param.setValueAtTime(
		time,
		pixel[0],
		pixel[1],
		pixel[2]
		);
}
template <typename Pixel>
void set012ParamValuesAtTime( OFX::RGBParam* param, const OfxTime time, const Pixel& pixel  )
{
	set012ParamValuesAtTime<Pixel>( *param, time, pixel  );
}

template <typename Pixel>
void set012ParamValues( OFX::RGBParam& param, const Pixel& pixel  )
{
	using namespace boost::gil;
	param.setValue(
		pixel[0],
		pixel[1],
		pixel[2]
		);
}
template <typename Pixel>
void set012ParamValues( OFX::RGBParam* param, const Pixel& pixel  )
{
	set012ParamValues<Pixel>( *param, pixel  );
}

template <typename Pixel>
void set012ParamValuesAtTime( OFX::Double3DParam& param, const OfxTime time, const Pixel& pixel  )
{
	using namespace boost::gil;
	param.setValueAtTime(
		time,
		pixel[0],
		pixel[1],
		pixel[2]
		);
}

template <typename Pixel>
void set012ParamValuesAtTime( OFX::Double3DParam* param, const OfxTime time, const Pixel& pixel  )
{
	set012ParamValuesAtTime<Pixel>( *param, time, pixel  );
}

template <typename Pixel>
void set012ParamValues( OFX::Double3DParam& param, const Pixel& pixel  )
{
	using namespace boost::gil;
	param.setValue(
		pixel[0],
		pixel[1],
		pixel[2]
		);
}

template <typename Pixel>
void set012ParamValues( OFX::Double3DParam* param, const Pixel& pixel  )
{
	set012ParamValues<Pixel>( *param, pixel );
}




}
}

#endif
