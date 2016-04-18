#ifndef _TUTTLE_HOST_EDGE_HPP_
#define _TUTTLE_HOST_EDGE_HPP_

#include "IEdge.hpp"
#include "UVertex.hpp"

#include <tuttle/common/utils/global.hpp>

#include <string>

namespace tuttle
{
namespace host
{
namespace graph
{

/**
 * @brief Edge of the user graph.
 */
class UEdge : public IEdge
{
public:
    UEdge() {}

    UEdge(const UVertex::Key& out, const UVertex::Key& in, const std::string& inAttrName);

    ~UEdge() {}

    friend std::ostream& operator<<(std::ostream& os, const UEdge& v);
};
}
}
}

#endif
