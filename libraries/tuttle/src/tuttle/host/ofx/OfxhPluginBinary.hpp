#ifndef OFXH_PLUGINBINARY_HPP
#define OFXH_PLUGINBINARY_HPP

#include "OfxhPluginHandle.hpp"
#include "OfxhPlugin.hpp"
#include "OfxhBinary.hpp"

#include <boost/serialization/serialization.hpp>
#include <boost/ptr_container/serialize_ptr_vector.hpp>

namespace tuttle {
namespace host {
namespace ofx {

/**
 * class that represents a binary file which holds plugins
 */
class OfxhPluginBinary
{
typedef OfxhPluginBinary This;
/// has a set of plugins inside it and which it owns
/// These are owned by a PluginCache
friend class OfxhPluginHandle;

protected:
	OfxhBinary _binary; ///< our binary object, abstracted layer ontop of OS calls, defined in OfxhBinary.hpp
	std::string _filePath; ///< full path to the file
	std::string _bundlePath; ///< path to the .bundle directory
	boost::ptr_vector<OfxhPlugin> _plugins; ///< my plugins
	time_t _fileModificationTime; ///< used as a time stamp to check modification times, used for caching
	size_t _fileSize; ///< file size last time we check, used for caching
	bool _binaryChanged; ///< whether the timestamp/filesize in this cache is different from that in the actual binary

public:
	/// create one from the cache.  this will invoke the Binary() constructor which
	/// will stat() the file.

	explicit OfxhPluginBinary( const std::string& file, const std::string& bundlePath, time_t mtime, size_t size )
		: _binary( file ),
		_filePath( file ),
		_bundlePath( bundlePath ),
		_fileModificationTime( mtime ),
		_fileSize( size ),
		_binaryChanged( false )
	{
		if( _fileModificationTime != _binary.getTime() || _fileSize != _binary.getSize() )
		{
			_binaryChanged = true;
		}
	}

	/// constructor which will open a library file, call things inside it, and then
	/// create Plugin objects as appropriate for the plugins exported therefrom

	explicit OfxhPluginBinary( const std::string& file, const std::string& bundlePath, OfxhPluginCache* cache )
		: _binary( file ),
		_filePath( file ),
		_bundlePath( bundlePath ),
		_binaryChanged( false )
	{
		loadPluginInfo( cache );
	}

	/// dtor
	virtual ~OfxhPluginBinary();


	bool operator==( const This& other ) const
	{
		if( _binary != other._binary ||
			_filePath != other._filePath ||
			_bundlePath != other._bundlePath ||
			_plugins != other._plugins ||
			_fileModificationTime != other._fileModificationTime ||
			_fileSize != other._fileSize ||
			_binaryChanged != other._binaryChanged )
			return false;
		return true;
	}
	bool operator!=( const This& other ) const { return !This::operator==(other); }


	time_t getFileModificationTime() const
	{
		return _fileModificationTime;
	}

	size_t getFileSize() const
	{
		return _fileSize;
	}

	const std::string& getFilePath() const
	{
		return _filePath;
	}

	const std::string& getBundlePath() const
	{
		return _bundlePath;
	}

	bool hasBinaryChanged() const
	{
		return _binaryChanged;
	}

	bool isLoaded() const
	{
		return _binary.isLoaded();
	}

	void addPlugin( OfxhPlugin* pe )
	{
		_plugins.push_back( pe );
	}

	void loadPluginInfo( OfxhPluginCache* );

	/// how many plugins?
	int getNPlugins() const
	{
		return (int) _plugins.size();
	}

	/// get a plugin
	OfxhPlugin& getPlugin( int idx )
	{
		return _plugins[idx];
	}

	/// get a plugin
	const OfxhPlugin& getPlugin( int idx ) const
	{
		return _plugins[idx];
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive &ar, const unsigned int version )
	{
		ar.register_type( static_cast<OfxhPlugin*>(NULL) );
//		ar.register_type( static_cast<OfxhImageEffectPlugin*>(NULL) );

//		ar & BOOST_SERIALIZATION_NVP(_binary); /// @todo tuttle: serialize !!!!!!!!!!!!!!
		ar & BOOST_SERIALIZATION_NVP(_filePath);
		ar & BOOST_SERIALIZATION_NVP(_bundlePath);
//		ar & boost::serialization::make_nvp("first_plugin", getPlugin(0));
		ar & BOOST_SERIALIZATION_NVP(_plugins);
		ar & BOOST_SERIALIZATION_NVP(_fileModificationTime);
		ar & BOOST_SERIALIZATION_NVP(_fileSize);
		ar & BOOST_SERIALIZATION_NVP(_binaryChanged);
	}
};

}
}
}

#endif

