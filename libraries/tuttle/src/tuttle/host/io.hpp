#ifndef _TUTTLE_HOST_IO_HPP_
#define _TUTTLE_HOST_IO_HPP_

#include <string>
#include <vector>

namespace tuttle
{
namespace host
{
namespace io
{

/**
 * Choose a plugin which supports the given file extension.
 * This plugin has the best evaluation among available plugins.
 * @return  the node identifier
 */
std::vector<std::string> getIOPluginsForExtension(const std::string& extension, const std::string& context);

/**
 * Get a file extension of the given file name.
 * @return the file extension
 */
std::string getFileExtension(const std::string& filename);

/**
 * @param extension
 * @return the reader nodes identifiers that support the given file extension
 */
std::vector<std::string> _getReaders(const std::string& extension);

/**
 * @param filename can be a filename or an extension
 * @return the reader nodes identifiers that support the extension of the given file
 */
std::vector<std::string> getReaders(const std::string& filename);

/**
 * @param filename can be a filename or an extension
 * @return the reader node identifier that supports the extension of the given file
 */
std::string getBestReader(const std::string& filename);

/**
 * @param extension
 * @return the writer nodes identifiers that support the given file extension
 */
std::vector<std::string> _getWriters(const std::string& extension);

/**
 * @param filename can be a filename or an extension
 * @return the writer nodes identifiers that support the extension of the given file.
 */
std::vector<std::string> getWriters(const std::string& filename);

/**
 *
 * @param filename can be a filename or an extension
 * @return the writer node identifier that supports the extension of the given file.
 */
std::string getBestWriter(const std::string& filename);
}
}
}

#endif
