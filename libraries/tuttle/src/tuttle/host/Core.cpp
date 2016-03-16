#include "Core.hpp"

#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>
#include <tuttle/host/memory/MemoryPool.hpp>
#include <tuttle/host/memory/MemoryCache.hpp>

#include <tuttle/common/system/system.hpp>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#ifdef TUTTLE_HOST_WITH_PYTHON_EXPRESSION
#include <boost/python.hpp>
#endif

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // memset

namespace tuttle
{
namespace host
{

namespace
{
memory::MemoryPool pool;
memory::MemoryCache cache;
}

Core::Core()
    : _imageEffectPluginCache(_host)
    , _memoryPool(pool)
    , _memoryCache(cache)
    , _isPreloaded(false)
    , _formatter(tuttle::common::Formatter::get())
{
#ifdef TUTTLE_HOST_WITH_PYTHON_EXPRESSION
    Py_Initialize();
#endif
    _pluginCache.setCacheVersion("tuttleV1");

    // register the image effect cache with the global plugin cache
    _pluginCache.registerAPICache(_imageEffectPluginCache);

    _memoryPool.updateMemoryAuthorizedWithRAM();
    //	preload();
}

Core::~Core()
{
}

void Core::preload(const bool useCache)
{
    _isPreloaded = true;

    //	typedef boost::archive::binary_oarchive OArchive;
    //	typedef boost::archive::binary_iarchive IArchive;
    //	typedef boost::archive::text_oarchive OArchive;
    //	typedef boost::archive::text_iarchive IArchive;
    typedef boost::archive::xml_oarchive OArchive;
    typedef boost::archive::xml_iarchive IArchive;

    std::string cacheFile;
    if(useCache)
    {
        cacheFile = (getPreferences().getTuttleHomePath() / "tuttlePluginCacheSerialize.xml").string();

        TUTTLE_LOG_DEBUG("plugin cache file = " << cacheFile);

        if(boost::filesystem::exists(cacheFile))
        {
            try
            {
                std::ifstream ifsb(cacheFile.c_str(), std::ios::in);
                {
                    TUTTLE_LOG_DEBUG("Read plugins cache.");
                    IArchive iArchive(ifsb);
                    iArchive >> BOOST_SERIALIZATION_NVP(_pluginCache);
                    // Destructor for an archive should be called before the stream is closed. It restores any altered stream
                    // facets to thier state before the the archive was opened.
                }
            }
            catch(std::exception& e)
            {
                TUTTLE_LOG_WARNING("Error when reading plugins cache file (" << e.what() << ").");
                // Clear the plugins cache to be sure that we don't stay in an unknown state.
                _pluginCache.clearPluginFiles();

                // As the plugins cache will be declared dirty, the cache file will be recreated.
            }
        }
    }
    _pluginCache.scanPluginFiles();
    if(useCache && _pluginCache.isDirty())
    {
        // generate unique name for writing
        boost::uuids::random_generator gen;
        boost::uuids::uuid u = gen();
        const std::string tmpCacheFile(cacheFile + ".writing." + boost::uuids::to_string(u) + ".xml");

        TUTTLE_LOG_DEBUG("Write plugins cache " << tmpCacheFile);
        try
        {
            // Serialize into a temporary file
            {
                std::ofstream ofsb(tmpCacheFile.c_str(), std::ios::out);
                {
                    OArchive oArchive(ofsb);
                    oArchive << BOOST_SERIALIZATION_NVP(_pluginCache);
                    // Destructor for an archive should be called before the stream is closed. It restores any altered stream
                    // facets to thier state before the the archive was opened.
                }
            }
            // Replace the cache file
            boost::filesystem::rename(tmpCacheFile, cacheFile);
        }
        catch(std::exception& e)
        {
            TUTTLE_LOG_WARNING("Error when writing plugins cache file (" << e.what() << ").");
            try
            {
                // Try to remove the bad temporary cache file.
                if(boost::filesystem::exists(tmpCacheFile))
                {
                    boost::filesystem::remove(tmpCacheFile);
                }
            }
            catch(std::exception& e)
            {
            }
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Core& v)
{
    os << "Core {" << std::endl;
    os << v.getImageEffectPluginCache();
    os << "}" << std::endl;
    return os;
}
}
}
