#ifndef _FILENAMEMANAGER_HPP
#define	_FILENAMEMANAGER_HPP

#include "ofxCore.h"

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
namespace tuttle {
namespace plugin {

namespace {
struct FileDesc
{
	FileDesc(std::string filename, const boost::regex & regex)
	{
		boost::cmatch matches;
		boost::regex_match( filename.c_str(), matches, regex );

		_prefix = std::string( matches[2].first, matches[2].second );
		_postfix = std::string( matches[4].first, matches[4].second );
		_filenameLong = filename;
		_sortingFilename = _prefix + _postfix;

		std::string num = std::string( matches[3].first, matches[3].second );
		std::istringstream strmLastNum(num);
		strmLastNum >> _num;

		std::ostringstream o;
		o << _num;
		size_t lnum = o.str().length();

		_numFill = (size_t)std::abs((double)num.length() - lnum);
	}
	std::string _prefix;
	std::string _postfix;
	std::string _filenameLong;
	std::string _sortingFilename;
	size_t _num;
	size_t _numFill;
};

struct FileSort {
	bool operator() (const FileDesc &c1, const FileDesc &c2)
	{
		if (c1._sortingFilename != c2._sortingFilename)
		{
			return c1._sortingFilename < c2._sortingFilename;
		}
		else
		{
			return c1._num < c2._num;
		}
	}
};

}

struct FilenamesGroup
{
	std::vector< std::string > _filenames;
	std::string _prefix;
	std::string _postfix;
	size_t _first;
	size_t _last;
	size_t _step;
	size_t _numFill;
	std::string _fillCar;
};

class FilenameManager {
public:
	FilenameManager(): _numFill(0), _step(0), _first(0), _last(0), _currentPos(0) {}
	FilenameManager(const boost::filesystem::path & directory, const std::string pattern, const bool dirbase = false, const size_t start = 0, const size_t step = 1 );
	FilenameManager(const boost::filesystem::path & directory, const bool dirbase = false, const size_t start = 0, const size_t step = 1 );
	virtual ~FilenameManager();
	bool  reset(const boost::filesystem::path& filepath, const bool dirbase = false, const size_t start = 0, const size_t step = 1);
	const std::string getCurrentFilename(const ssize_t nGroup = -1);
	const std::string getNextFilename(const ssize_t nGroup = -1);
	const std::string getFilenameAt(const OfxTime time, const ssize_t nGroup = -1);
	const size_t numGroups() const;
	const OfxRangeI getRange(const ssize_t nGroup = -1);
protected:
	std::vector< FilenamesGroup > matchingGroups(const boost::filesystem::path & directory, const boost::regex & regex);

protected:
	boost::filesystem::path _path;					///< Path
	std::string _pattern;							///< Current pattern
	const boost::regex _regexp;						///< Internal pattern regexp
	std::vector< FilenamesGroup > _matchList;		///< Currently unused, but usefull to automatically get a pattern from a directory
	std::string _prefixDir;							///< Store directory prefix
	std::string _prefixFile;						///< Store filename prefix
	std::string _postfixFile;						///< Store postfix
	std::string _fillCar;							///< Filling character
	size_t _numFill;								///< Number of filling char
	std::size_t _step;								///< Step
	std::size_t _first;								///< Starting num
	std::size_t _last;								///< Ending num
	ssize_t _currentPos;							///< Used by getNextFilename()
};

}
}

#endif	/* _FILENAMEMANAGER_HPP */
