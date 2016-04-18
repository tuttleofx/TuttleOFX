#ifndef _OFXH_MAJORPLUGIN_HPP_
#define _OFXH_MAJORPLUGIN_HPP_

#include "OfxhPlugin.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{

class OfxhMajorPlugin
{
public:
    typedef OfxhMajorPlugin This;

private:
    std::string _id;
    int _major;

public:
    OfxhMajorPlugin()
        : _major(0)
    {
    }

    OfxhMajorPlugin(const std::string& id, int major)
        : _id(id)
        , _major(major)
    {
    }

    OfxhMajorPlugin(OfxhPlugin& iep)
        : _id(iep.getIdentifier())
        , _major(iep.getVersionMajor())
    {
    }

    const std::string& getId() const { return _id; }

    int getMajor() const { return _major; }

    bool operator<(const This& other) const
    {
        if(_id < other._id)
            return true;

        if(_id > other._id)
            return false;

        if(_major < other._major)
            return true;

        return false;
    }
};
}
}
}

#endif
