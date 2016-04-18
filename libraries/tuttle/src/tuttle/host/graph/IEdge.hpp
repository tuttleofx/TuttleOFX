#ifndef _TUTTLE_HOST_IEDGE_HPP_
#define _TUTTLE_HOST_IEDGE_HPP_

#include <tuttle/common/utils/global.hpp>

#include <ofxAttribute.h>

#include <iostream>
#include <string>

namespace tuttle
{
namespace host
{
namespace graph
{

class IEdge
{
public:
    IEdge();
    IEdge(const std::string& out, const std::string& in, const std::string& inAttrName);
    IEdge(const IEdge& e);
    virtual ~IEdge() = 0;

    inline IEdge& operator=(const IEdge& e)
    {
        if(this == &e)
            return *this;
        _out = e._out;
        _in = e._in;
        _inAttrName = e._inAttrName;
        _name = e._name;
        _localId = e._localId;
        return *this;
    }

    inline const std::string& getOut() const { return _out; }
    inline const std::string& getIn() const { return _in; }
    inline const std::string& getInAttrName() const { return _inAttrName; }
    inline const std::string& getName() const { return _name; }

    virtual std::ostream& exportDotDebug(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const IEdge& v);

public:
    std::string _out;
    std::string _in;
    std::string _inAttrName;
    std::string _name;
    std::size_t _localId;
};
}
}
}

#endif
