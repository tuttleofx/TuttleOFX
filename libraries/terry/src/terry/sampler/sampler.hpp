#ifndef _TERRY_SAMPLER_HPP_
#define _TERRY_SAMPLER_HPP_

#include "nearestNeighbor.hpp"
#include "bilinear.hpp"
#include "bicubic.hpp"
#include "bc.hpp"
#include "gaussian.hpp"
#include "lanczos.hpp"
#include "details.hpp"

#include <map>

namespace terry {
using namespace boost::gil;
namespace sampler {


void getMapOfFilters( std::map< size_t, std::string >& mapOfFilters )
{
/*
		mapOfFilters.insert(  0, (std::string) "Nearest-Neighbor"	);
		mapOfFilters.insert(  1, (std::string) "Bilinear"			);
		mapOfFilters.insert(  2, (std::string) "Bicubic"			);
		mapOfFilters.insert(  3, (std::string) "BC sampler"			);
		mapOfFilters.insert(  4, (std::string) "Catmul Rom"			);
		mapOfFilters.insert(  5, (std::string) "Keys"				);
		mapOfFilters.insert(  6, (std::string) "Simon"				);
		mapOfFilters.insert(  7, (std::string) "Rifman"				);
		mapOfFilters.insert(  8, (std::string) "Mitchell"			);
		mapOfFilters.insert(  9, (std::string) "Parzen"				);
		mapOfFilters.insert( 10, (std::string) "Lanczos"			);
		mapOfFilters.insert( 11, (std::string) "Gaussian"			);
*/

}


/// \brief A sampler that sets the destination pixel without interpolation from the closest pixels from the source.
// http://www.paulinternet.nl/?page=bicubic



}
}

#endif

