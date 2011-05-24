
#ifndef _TUTTLE_COLOR_CONVERSION_HPP
#define _TUTTLE_COLOR_CONVERSION_HPP

/*
namespace ttlc = tuttle::common::color;

ttlc::colorspace_convert<
ttlc::colorspace<ttlc::layout::RGB, ttlc::primaries::rec709, ttlc::gradation::linear, ttlc::nopremult>,
ttlc::colorspace<ttlc::layout::XYZ, ttlc::primaries::rec601, ttlc::gradation::log, ttlc::nopremult>
>( inputView, outputView )

typedef ttlc::colorspace<ttlc::layout::RGB, ttlc::primaries::rec709, ttlc::gradation::linear, ttlc::nopremult> colorspaceRec709;


ttlc::colorspace_convert<
ttlc::colorspaceRec709,
ttlc::colorspace<ttlc::layout::XYZ, ttlc::primaries::rec601, ttlc::gradation::log, ttlc::nopremult>
>( inputView, outputView )
*/

namespace tuttle {
namespace plugin {
namespace color{

	namespace gradation{
		struct linear{};
		struct sRGB{};
		struct cineon{};
		struct gamma{};
		struct panalog{};
		struct redLog{};
		struct viperLog{};
		struct redSpace{};
		struct alexaLogC{};
	}

	namespace layout{
		struct RGB{};
		struct YUV{};
		struct YPrPb{};
		struct HSV{};
		struct HSL{};
		struct Lab{};
		struct Luv{};
		struct XYZ{};
		struct Yxy{};
	}

	namespace primaries{
		struct sRGBRec709{};
		struct rec709{};
		struct rec601{};
		struct DCI{};
		struct ACES{};
		struct ProPhoto{};
		struct adobe98{};
	}






template <typename GradationLaw, typename Layout, typename Primaries, typename Premultiplication>
struct ttlc_colorspace {};

template <typename SrcView, typename DstP, typename colorspaceIn, typename colorspaceOut>
bool colorspace_convert( const SrcView& src, DstP& dest )
{

	return false;
}


}
}
}
#endif
