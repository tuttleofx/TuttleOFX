/**
 * @file MergeProcess.hpp
 * @brief
 * @author
 * @date    27/11/09 13:42
 *
 */
#ifndef MERGE_DEFINITIONS_HPP
#define MERGE_DEFINITIONS_HPP

#include "ColorMerge.hpp"
#include <tuttle/common/utils/global.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

struct FunctorATop
: public boost::gil::merge_functor<boost::mpl::true_>
{
	template <typename DstV, typename AlphaA, typename AlphaB,
			  typename SrcAV, typename SrcBV>
	inline static DstV merge( const AlphaA& a, const AlphaB& b,
							  const SrcAV& A, const SrcBV& B )
	{
		// Ab + B(1-a)
		return (DstV)( A * b + B * ( 1 - a ) );
	}
};

struct FunctorAverage
: public boost::gil::merge_functor<boost::mpl::false_>
{
	template <typename DstV, typename SrcAV, typename SrcBV>
	inline static DstV merge( const SrcAV& srcA, const SrcBV& srcB )
	{
		// (A + B) / 2
		return (DstV)( ( ((float)srcA) + srcB ) / 2.0f );
	}
};

struct FunctorPlus
: public boost::gil::merge_functor<boost::mpl::false_>
{
	template <typename DstV, typename SrcAV, typename SrcBV>
	inline static DstV merge( const SrcAV& srcA, const SrcBV& srcB )
	{
		// A + B
		return (DstV)( srcA + srcB );
	}
};

// Descriptors name
const static std::string kMergeSourceA    = "SourceA";
const static std::string kMergeSourceB    = "SourceB";
const static std::string kMergeHelpButton = "Help";
const static std::string kMergeFunction   = "Merging function";

static const bool kSupportTiles              = false;
static const bool kSupportTemporalClipAccess = false;

// Plugin internal data
const static std::string kMergeHelpString = "<b>Clip merging</b> plugin is used to merge two clips A and B.  <br />";
enum EMergeFunction {
	eMergeFunctionATop, eMergeFunctionAverage,
	eMergeFunctionColorBurn, eMergeFunctionColorDodge,
	eMergeFunctionConjointOver, eMergeFunctionCopy,
	eMergeFunctionDifference, eMergeFunctionDisjointOver,
	eMergeFunctionDivide, eMergeFunctionExclusion,
	eMergeFunctionFrom, eMergeFunctionGeometric,
	eMergeFunctionHardLight, eMergeFunctionHypot,
	eMergeFunctionIn, eMergeFunctionMask,
	eMergeFunctionMatte, eMergeFunctionMax,
	eMergeFunctionMin, eMergeFunctionMinus,
	eMergeFunctionMultiply, eMergeFunctionOut,
	eMergeFunctionOver, eMergeFunctionOverlay,
	eMergeFunctionPlus, eMergeFunctionScreen,
	eMergeFunctionStencil, eMergeFunctionUnder,
	eMergeFunctionXOR
};
}
}
}

#endif  // MERGE_DEFINITIONS_HPP
