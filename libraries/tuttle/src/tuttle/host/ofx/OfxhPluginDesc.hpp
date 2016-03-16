#ifndef _OFXH_PLUGINDESC_HPP_
#define _OFXH_PLUGINDESC_HPP_

#include <ofxCore.h>

#include <boost/algorithm/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/string.hpp>

namespace tuttle
{
namespace host
{
namespace ofx
{

struct OfxhPluginIdent
{
    OfxhPluginIdent()
        : _versionMinor(0)
        , _versionMajor(0)
    {
    }
    OfxhPluginIdent(const std::string& identifier, const std::string& rawId, int verMin, int verMax)
        : _identifier(identifier)
        , _rawIdentifier(rawId)
        , _versionMinor(verMin)
        , _versionMajor(verMax)
    {
    }

    bool operator<(const OfxhPluginIdent& d2) const
    {
        return _identifier < d2._identifier || (_identifier == d2._identifier && _versionMajor < d2._versionMajor) ||
               (_identifier == d2._identifier && _versionMajor == d2._versionMajor && _versionMinor < d2._versionMinor);
    }

    bool operator!=(const OfxhPluginIdent& d2) const
    {
        return _identifier != d2._identifier && _rawIdentifier != d2._rawIdentifier && _versionMinor != d2._versionMinor &&
               _versionMajor != d2._versionMajor;
    }

    std::string _identifier;
    std::string _rawIdentifier;
    int _versionMinor, _versionMajor;
};

/**
 * C++ version of the information kept inside an OfxPlugin struct
 */
class OfxhPluginDesc
{
public:
    typedef OfxhPluginDesc This;

protected:
    std::string _pluginApi; ///< the API I implement
    int _apiVersion;        ///< the version of the API

    OfxhPluginIdent _ident; ///< The plugin identity

public:
    OfxhPluginDesc();

    OfxhPluginDesc(const std::string& api, int apiVersion, const std::string& identifier, const std::string& rawIdentifier,
                   int versionMajor, int versionMinor);

    /**
     * constructor for the case where we have already loaded the plugin binary and
     * are populating this object from it
     */
    OfxhPluginDesc(OfxPlugin& ofxPlugin);

    virtual ~OfxhPluginDesc();

    bool operator==(const This& other) const;
    bool operator!=(const This& other) const { return !This::operator==(other); }

public:
    const std::string& getPluginApi() const { return _pluginApi; }

    int getApiVersion() const { return _apiVersion; }

    const OfxhPluginIdent& getIdentity() const { return _ident; }

    OfxhPluginIdent& getIdentity() { return _ident; }

    const std::string& getIdentifier() const { return _ident._identifier; }

    const std::string& getRawIdentifier() const { return _ident._rawIdentifier; }

    int getVersionMajor() const { return _ident._versionMajor; }

    int getVersionMinor() const { return _ident._versionMinor; }

    std::size_t getHash() const;

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& BOOST_SERIALIZATION_NVP(_pluginApi);
        ar& BOOST_SERIALIZATION_NVP(_apiVersion);
        //		ar & BOOST_SERIALIZATION_NVP(_ident._identifier);
        ar& BOOST_SERIALIZATION_NVP(_ident._rawIdentifier);
        ar& BOOST_SERIALIZATION_NVP(_ident._versionMajor);
        ar& BOOST_SERIALIZATION_NVP(_ident._versionMinor);

        if(typename Archive::is_loading())
        {
            _ident._identifier = _ident._rawIdentifier;
            boost::to_lower(_ident._identifier);
        }
    }
};
}
}
}

#ifndef SWIG
BOOST_SERIALIZATION_ASSUME_ABSTRACT(tuttle::host::ofx::OfxhPluginDesc)
#endif

#endif
