#include "ComputeOptions.hpp"

#include <boost/lexical_cast.hpp>


namespace tuttle {
namespace host {

TimeRange::TimeRange( const OfxRangeD& range, const int step )
	: _begin( boost::lexical_cast<int>(range.min) )
	, _end( boost::lexical_cast<int>(range.max) )
	, _step( step )
{
}

}
}
