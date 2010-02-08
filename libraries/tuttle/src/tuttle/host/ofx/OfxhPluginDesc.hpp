#ifndef _OFXH_PLUGINDESC_HPP_
#define _OFXH_PLUGINDESC_HPP_

#include <ofxCore.h>

#include <boost/algorithm/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/string.hpp>

namespace tuttle {
namespace host {
namespace ofx {


/**
 * C++ version of the information kept inside an OfxPlugin struct
 */
class OfxhPluginDesc
{
	typedef OfxhPluginDesc This;
protected:
	std::string _pluginApi; ///< the API I implement
	int _apiVersion; ///< the version of the API

	std::string _identifier; ///< the identifier of the plugin
	std::string _rawIdentifier; ///< the original identifier of the plugin
	int _versionMajor; ///< the plugin major version
	int _versionMinor; ///< the plugin minor version

public:
	OfxhPluginDesc();

	OfxhPluginDesc( const std::string& api,
	            int                apiVersion,
	            const std::string& identifier,
	            const std::string& rawIdentifier,
	            int                versionMajor,
	            int                versionMinor );

	/**
	 * constructor for the case where we have already loaded the plugin binary and
	 * are populating this object from it
	 */
	OfxhPluginDesc( OfxPlugin* ofxPlugin );

	virtual ~OfxhPluginDesc();

	bool operator==( const This& other ) const;
	bool operator!=( const This& other ) const { return !This::operator==(other); }

public:
	const std::string& getPluginApi() const
	{
		return _pluginApi;
	}

	int getApiVersion() const
	{
		return _apiVersion;
	}

	const std::string& getIdentifier() const
	{
		return _identifier;
	}

	const std::string& getRawIdentifier() const
	{
		return _rawIdentifier;
	}

	int getVersionMajor() const
	{
		return _versionMajor;
	}

	int getVersionMinor() const
	{
		return _versionMinor;
	}
	
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive &ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_NVP(_pluginApi);
		ar & BOOST_SERIALIZATION_NVP(_apiVersion);
//		ar & BOOST_SERIALIZATION_NVP(_identifier);
		ar & BOOST_SERIALIZATION_NVP(_rawIdentifier);
		ar & BOOST_SERIALIZATION_NVP(_versionMajor);
		ar & BOOST_SERIALIZATION_NVP(_versionMinor);
		
		if( typename Archive::is_loading() )
		{
			_identifier = _rawIdentifier;
			boost::to_lower( _identifier );
		}
	}
};

}
}
}

BOOST_SERIALIZATION_ASSUME_ABSTRACT(tuttle::host::ofx::OfxhPluginDesc)
//BOOST_CLASS_EXPORT(::tuttle::host::ofx::OfxhPluginDesc)

#endif

