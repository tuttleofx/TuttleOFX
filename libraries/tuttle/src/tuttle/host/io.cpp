#include "io.hpp"

#include <tuttle/host/Graph.hpp>
#include <tuttle/host/Core.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPluginCache.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

#include <algorithm>
#include <exception>
#include <stdexcept>

namespace tuttle
{
namespace host
{
namespace io
{

typedef boost::tuples::tuple<double, int, std::string> Tuple;

std::vector<std::string> getIOPluginsForExtension(const std::string& extension, const std::string& context)
{
    std::vector<Tuple> tupleArray;

    // Normalize the input extension, so the searchExtension should be something like "jpg".
    std::string searchExtension;
    if(!boost::starts_with(extension, "."))
    {
        searchExtension = extension;
    }
    else
    {
        searchExtension = extension.substr(1, extension.size() - 1);
    }
    boost::algorithm::to_lower(searchExtension);

    // get array of tuple( evaluation, supportedExtensions, nodeIdentifier )
    ofx::imageEffect::OfxhImageEffectPluginCache::MapPluginsByID nodeMap =
        core().getImageEffectPluginCache().getPluginsByID();
    BOOST_FOREACH(const ofx::imageEffect::OfxhImageEffectPluginCache::MapPluginsByID::value_type& node, nodeMap)
    {
        try
        {
            node.second->loadAndDescribeActions();
            const ofx::imageEffect::OfxhImageEffectNodeDescriptor& desc = node.second->getDescriptor();
            if(node.second->supportsContext(context))
            {
                double evaluation = desc.getProperties().getDoubleProperty("TuttleOfxImageEffectPropEvaluation");
                int supportedExtensionsSize =
                    desc.getProperties().fetchStringProperty("TuttleOfxImageEffectPropSupportedExtensions").getDimension();
                const std::vector<std::string> supportedExtensions =
                    desc.getProperties().fetchStringProperty("TuttleOfxImageEffectPropSupportedExtensions").getValues();

                for(size_t indexExtension = 0; indexExtension < supportedExtensions.size(); ++indexExtension)
                {
                    if(supportedExtensions.at(indexExtension) == searchExtension)
                    {
                        Tuple tuple(-evaluation, supportedExtensionsSize, node.second->getRawIdentifier());
                        tupleArray.push_back(tuple);
                        break;
                    }
                }
            }
        }
        catch(std::exception& e)
        {
            // Ignore the exception.
        }
    }

    // get nodeIdentifier of the plugin with the best evaluation
    std::sort(tupleArray.begin(), tupleArray.end());

    std::vector<std::string> results;
    for(size_t indexTuple = 0; indexTuple < tupleArray.size(); ++indexTuple)
    {
        results.push_back(tupleArray.at(indexTuple).get<2>());
    }
    return results;
}

std::string getFileExtension(const std::string& filename)
{
    size_t pos = filename.find_last_of(".");
    if(pos == std::string::npos)
    {
        BOOST_THROW_EXCEPTION(exception::File(filename)
                              << exception::user() + "Filename has no extension \"" + filename + "\"."
                              << exception::dev() + "Filename has no extension \"" + filename + "\".");
    }
    return filename.substr(pos);
}

std::vector<std::string> _getReaders(const std::string& extension)
{
    return getIOPluginsForExtension(extension, kOfxImageEffectContextReader);
}

std::vector<std::string> getReaders(const std::string& filename)
{
    return _getReaders(getFileExtension(filename));
}

std::string getBestReader(const std::string& filename)
{
    const std::string extension = getFileExtension(filename);

    if(!boost::filesystem::exists(filename))
    {
        std::vector<std::string> results = _getReaders(extension);
        if(results.size() == 0)
        {
            BOOST_THROW_EXCEPTION(exception::File(filename)
                                  << exception::user() + "Unknown file extension \"" + extension + "\"."
                                  << exception::dev() + "Unknown file extension \"" + extension + "\".");
        }
        return results[0];
    }
    else
    {
        std::vector<std::string> results = _getReaders(extension);
        BOOST_FOREACH(const std::string readerId, results)
        {
            try
            {
                Graph graph;
                ImageEffectNode& readerIn = graph.createNode(readerId).asImageEffectNode();
                readerIn.getParam("filename").setValue(filename);
                graph.setup();
                OfxRangeD timeRange = readerIn.getTimeDomain();
                graph.setupAtTime(timeRange.min);
                readerIn.getRegionOfDefinition(timeRange.min);
                return readerId;
            }
            catch(std::exception& e)
            {
                // Ignore the exception, if we can't load this file with this reader plugin,
                // we will try with the other ones.
            }
        }
        const std::string errorMessage("No OFX plugin found to read file '" + filename + "'.");
        TUTTLE_LOG_ERROR(errorMessage);
        BOOST_THROW_EXCEPTION(exception::File(filename)
                              << exception::user() + errorMessage
                              << exception::dev() + "File is not supported. We have tried to load it with: " +
                                     boost::algorithm::join(results, ", "));
    }
    BOOST_THROW_EXCEPTION(exception::Bug());
}

std::vector<std::string> _getWriters(const std::string& extension)
{
    return getIOPluginsForExtension(extension, kOfxImageEffectContextWriter);
}

std::vector<std::string> getWriters(const std::string& filename)
{
    return _getWriters(getFileExtension(filename));
}

std::string getBestWriter(const std::string& filename)
{
    std::string extension = getFileExtension(filename);

    std::vector<std::string> results = _getWriters(extension);
    if(results.size() == 0)
    {
        const std::string errorMessage("No OFX plugin found to write file '" + filename + "' (extension '" + extension +
                                       "').");
        TUTTLE_LOG_ERROR(errorMessage);
        BOOST_THROW_EXCEPTION(exception::File(filename) << exception::user() + errorMessage
                                                        << exception::dev() + errorMessage);
    }
    return results[0];
}
}
}
}
