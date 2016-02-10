#include "ThumbnailDiskCache.hpp"

#include <tuttle/host/memory/MemoryCache.hpp>
#include <tuttle/host/Graph.hpp>
#include <tuttle/host/Node.hpp>
#include <tuttle/host/io.hpp>

#include <boost/functional/hash.hpp>
#include <boost/filesystem/operations.hpp>

#include <ctime>


namespace tuttle {
namespace host {

::boost::shared_ptr<attribute::Image> loadImage( const std::string& imagePath )
{
	memory::MemoryCache outputCache;

	compute(
		outputCache,
		list_of
		( NodeInit(io::getBestReader(imagePath))
			.setParam("filename", imagePath.c_str()) )
		);
	
	return outputCache.get(0);
}

::boost::shared_ptr<attribute::Image> loadAndGenerateThumbnail( const std::string& imagePath, const std::string& thumbnailToCreate, const int thumbnailMaxSize )
{
	memory::MemoryCache outputCache;
	memory::MemoryCache internCache;

	Graph graph;
	std::vector<INode*> nodes = graph.addConnectedNodes(
		list_of
		( NodeInit(io::getBestReader(imagePath))
			.setParam("filename", imagePath.c_str()) )
		( NodeInit("tuttle.resize")
			.setParam("size", thumbnailMaxSize, thumbnailMaxSize)
			.setParam("keepRatio", true) )
		( NodeInit(io::getBestWriter(thumbnailToCreate))
			.setParam("filename", thumbnailToCreate.c_str()) )
		);
	graph.setup();

	OfxRangeD timeDomain = nodes.back()->getTimeDomain();
	OfxTime time = timeDomain.min;

	ComputeOptions cOptions;
	cOptions.setTimeRange(time, time);

	graph.compute(
		outputCache,
			NodeListArg(),
			cOptions,
			internCache
		);
	return outputCache.get(0);
}

const std::string ThumbnailDiskCache::s_thumbnailExtension(".png");
const int ThumbnailDiskCache::s_thumbnailMaxSize(256);

std::string ThumbnailDiskCache::keyToThumbnailPath( const KeyType& key ) const
{
	return _diskCacheTranslator.keyToAbsolutePath( key ).replace_extension(s_thumbnailExtension).string();
}

std::string ThumbnailDiskCache::getThumbnailPath( const boost::filesystem::path& imagePath ) const
{
	return keyToThumbnailPath( buildKey(imagePath) );
}

bool ThumbnailDiskCache::containsUpToDate( const boost::filesystem::path& imagePath ) const
{
	std::time_t thumbnailLastWriteTime; // thumbnail cached file time
	if( ! _diskCacheTranslator.contains( getThumbnailPath(imagePath), thumbnailLastWriteTime ) )
		return false;

	boost::system::error_code error;
	if( ! boost::filesystem::exists(imagePath, error) )
		return false;

	const bool upToDate = (thumbnailLastWriteTime == boost::filesystem::last_write_time(imagePath));
	return upToDate;
}

ThumbnailDiskCache::TImage ThumbnailDiskCache::retrieveThumbnail( const KeyType key ) const
{
	return loadImage( _diskCacheTranslator.keyToAbsolutePath( key ).replace_extension(s_thumbnailExtension).string() );
}

ThumbnailDiskCache::TImage ThumbnailDiskCache::create( KeyType& key, const boost::filesystem::path& imagePath )
{
	key = buildKey(imagePath);
	const boost::filesystem::path thumbnailPath = _diskCacheTranslator.create( key ).replace_extension(s_thumbnailExtension);

	// Load full image as thumbnail
	TImage thumbnail = loadAndGenerateThumbnail( imagePath.string(), thumbnailPath.string(), s_thumbnailMaxSize );

	// Set the last write time to the same value as the source image
	std::time_t lastWriteTimeSourceImg = boost::filesystem::last_write_time(imagePath); // read last write time
	boost::filesystem::last_write_time( thumbnailPath, lastWriteTimeSourceImg ); // set last write time of the thumbnail

	return thumbnail;
}

ThumbnailDiskCache::KeyType ThumbnailDiskCache::buildKey( const boost::filesystem::path& imagePath ) const
{
	KeyType key = 0;
	boost::hash_combine( key, imagePath );
	return key;
}

ThumbnailDiskCache::TImage ThumbnailDiskCache::getThumbnail( KeyType& key, const boost::filesystem::path& imagePath )
{
	if( ! containsUpToDate(imagePath) )
	{
		return create( key, imagePath );
	}

	// Load an existing thumbnail
	key = buildKey(imagePath);
	const boost::filesystem::path thumbnailPath = keyToThumbnailPath(key);

	return loadImage( thumbnailPath.string() );
}

}
}

