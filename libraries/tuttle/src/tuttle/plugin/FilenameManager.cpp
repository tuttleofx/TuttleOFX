#include "FilenameManager.hpp"
#include "PluginException.hpp"

#include <limits>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <list>
#include <boost/filesystem/operations.hpp>

namespace tuttle {
namespace plugin {

using namespace boost::filesystem;
using namespace boost;
using namespace std;


namespace {

struct FileDesc
{
	FileDesc(const boost::filesystem::path & filepath, const boost::regex & regex)
	{
		boost::cmatch matches;
		std::string filename = filepath.filename();
		boost::regex_match( filename.c_str(), matches, regex );

		_prefix = std::string( matches[1].first, matches[1].second );
		_postfix = std::string( matches[3].first, matches[3].second );
		_filenameLong = filepath.filename();
		_sortingFilename = filepath.parent_path().string() + "/" + _prefix + _postfix;

		std::string num = std::string( matches[2].first, matches[2].second );
		std::istringstream strmLastNum(num);
		strmLastNum >> _num;
		_numFill = (size_t)num.length();
	}
	std::string _prefix;
	std::string _postfix;
	std::string _filenameLong;
	std::string _sortingFilename;
	size_t _num;
	size_t _numFill;
};

struct FileSort
{
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


///@todo: windows filename check
FilenameManager::FilenameManager(const path & directory, const std::string pattern,
                                 const bool dirbase /* = false */, const size_t start /*= 0*/, const size_t step /* = 1 */)
: _numFill(0), _step(0), _first(0), _last(0), _currentPos(0)
{

	path p;
	if (!directory.empty() && directory.string()[directory.string().length()-1] != '/')
	{
		p = directory.string() + "/";
	}
	else
	{
		p = directory;
	}
	reset( p.string() + pattern, dirbase, start, step);
}

FilenameManager::FilenameManager(const boost::filesystem::path & filepath,
                                 const bool dirbase /* = false */, const size_t start /*= 0*/, const size_t step /* = 1 */)
: _numFill(0), _step(0), _first(0), _last(0), _currentPos(0)
{
	reset(filepath, dirbase, start, step);
}

FilenameManager::~FilenameManager()
{
}

bool FilenameManager::reset(boost::filesystem::path filepath, const bool dirbase, const size_t start, const size_t step)
{
	if (filepath.empty())
	{
		return true;
	}
	if(filepath.parent_path().empty())
	{
		filepath = boost::filesystem::initial_path().string() + "/" + filepath.string();
	}
	// Default
	_pattern = filepath.filename();
	_numFill = 0;
	_step = step;
	_first = start;
	_last = start;
	_currentPos = 0;
	_matchList.clear();

	// max number of digits for size_t
	static const size_t max = std::numeric_limits<size_t>::digits10+1;

	const regex esc("[\\^\\.\\$\\|\\(\\)\\[\\]\\*\\+\\?\\/\\\\]");
	string directory = filepath.parent_path().string() + "/";

	string escPattern = regex_replace(filepath.filename(), esc, "\\\\\\1&", boost::match_default | boost::format_sed);
	_fillCar = "0";
	// Nuke style ( eg. filename_%04d.png )
	if ( regex_match(filepath.filename(), regex(".*%[0-9]+d.*")) )
	{
		// Parse to get filling information
		cmatch matches;
		string fn = filepath.filename();
		regex_match( fn.c_str(), matches, regex("(.*?\\.?)%([0-9]|\\w)?([0-9]*)d(\\..*)") );
		_prefixDir  = directory;
		_prefixFile = string( matches[1].first, matches[1].second );
		_fillCar    = string( matches[2].first, matches[2].second );
		istringstream strm(std::string( matches[3].first, matches[3].second ));
		strm >> _numFill;
		_postfixFile = std::string( matches[4].first, matches[4].second );

		if (dirbase)
		{
			// Parse filenames sequence
			regex rx = regex("(.*)%([0-9])([0-9])+d(.*(\\\\\\.)?.*?)");
			std::ostringstream o;
			o << "(\\1)(\\\\\\d{\\3, " << max << "})(\\4\\)\\$";
			_pattern = regex_replace(escPattern, rx, o.str(), boost::match_default | boost::format_sed);
		}
	}
	// Common style with padding ( eg. filename_####.png )
	else if ( regex_match(filepath.filename(), regex(".*?\\[?#+\\]?.*")) )
	{
		// Parse to get filling information
		cmatch matches;
		// Detect filename sequence based
		string fn = filepath.filename();
		regex_match( fn.c_str(), matches, regex("(.*?\\.?)\\[?(#+)\\]?(\\..*)") );
		_fillCar = "0";
		_prefixDir   = directory;
		_prefixFile  = string( matches[1].first, matches[1].second );
		_numFill     = (string( matches[2].first, matches[2].second )).length();
		_postfixFile = string( matches[3].first, matches[3].second );

		if (dirbase)
		{
			// Parse filenames sequence
			regex rx = regex("(.*?)(\\\\\\[)?#+(\\\\\\])?(.*(\\\\\\.)?.*?)");
			std::ostringstream o;
			o << "(\\1)(\\\\\\d{" << _numFill << ", " << max << "})(\\4\\)\\$";
			_pattern = regex_replace(escPattern, rx, o.str(), boost::match_default | boost::format_sed);
		}
	}
	// Common style without padding ( eg. filename_@.png )
	else if ( regex_match(filepath.filename(), regex(".*?@+.*")) )
	{
		// Parse to get filling information
		cmatch matches;
		// Detect filename sequence based
		string fn = filepath.filename();
		regex_match( fn.c_str(), matches, regex("(.*?\\.?)(@+)(\\..*)") );
		_fillCar = "0";
		_numFill = 0;
		_prefixDir = directory;
		_prefixFile = std::string( matches[1].first, matches[1].second );
		_postfixFile = std::string( matches[3].first, matches[3].second );

		if (dirbase)
		{
			// Parse filenames sequence
			regex rx = regex("(.*)@+(.*(\\\\\\.)?.*?)");
			std::ostringstream o;
			o << "(\\1)(\\\\\\d{1, " << max << "})(\\2\\)\\$";
			_pattern = regex_replace(escPattern, rx, o.str(), boost::match_default | boost::format_sed);
		}
	}
	else
	{
		// Detect filename sequence based
		string fn = filepath.filename();
//		if (!boost::regex_match( fn.c_str(), matches, regex("(.*?\\.?)(\\d+)(\\..*)") ))
//		{
			boost::cmatch matches;
			// Parse to get filling information
			// Detect filename sequence based
			regex_match( fn.c_str(), matches, regex("(.*?\\.?)(\\..*)") );
			_prefixDir = directory;
			_prefixFile = std::string( matches[1].first, matches[1].second );
			_postfixFile = filepath.extension();
			_fillCar = "";
			_numFill = 0;
//		}
//		else
//		{
//			_prefixDir = directory;
//			_prefixFile = std::string( matches[1].first, matches[1].second );
//			_postfixFile = std::string( matches[3].first, matches[3].second );
//			string num = string( matches[2].first, matches[2].second );
//			std::istringstream strm(num);
//			strm >> _currentPos;
//			_fillCar = "0";
//			_numFill = num.length();
//			_first = _currentPos;
//			_last = _currentPos;
//		}
		return false;
	}
	if (dirbase)
	{
		_matchList = matchingGroups(directory, regex( _pattern ) );
		if (_matchList.size() > 1)
		{
			COUT_WARNING("Multiple sequences found in directory: " << directory << endl << "Basing on first pattern.");
		}
		else if (_matchList.size() == 0)
		{
//			COUT_WARNING("No pattern was found, disabling file directory pattern searching");
			return false;
		}

		FilenamesGroup g = _matchList.front();
		_prefixDir = directory;
		_prefixFile = g._prefix;
		_postfixFile = g._postfix;
		_step = g._step;
		_first = g._first;
		_last = g._last;
		_numFill = g._numFill;
		_currentPos = _first;
	}
	return false;
}

const std::string FilenameManager::getCurrentFilename(const ssize_t nGroup /* = -1 */)
{
	return getFilenameAt(double(_currentPos), nGroup);
}

const std::string FilenameManager::getNextFilename(const ssize_t nGroup /* = -1 */)
{
	string str = getCurrentFilename(nGroup);
	_currentPos++;
	return str;
}

const std::string FilenameManager::getFilenameAt(const OfxTime time, const ssize_t nGroup /* = -1 */)
{
	if (_pattern == "")
	{
		return "";
	}
	_currentPos = ssize_t(time);
	// Check for fractionnal part
	if (time != double(_currentPos) )
	{
		COUT_WARNING("Warning ! Passing fractionnal time value. Frame will be overwritten.");
	}

	// Scale & translate
	size_t t = _currentPos * _step;
	std::ostringstream o;

	if (nGroup >= 0 && nGroup < (ssize_t)_matchList.size())
	{
		o << _prefixDir << _prefixFile << nGroup << "_";
		if (_numFill)
		{
			o << setw(_numFill) << setfill(_fillCar[0]) << t;
		}
		else
		{
			if (_fillCar.length())
			{
				o << t;
			}
		}
		o << _postfixFile;
	}
	else
	{
		o << _prefixDir << _prefixFile;
		if (_numFill)
		{
			o << setw(_numFill) << setfill(_fillCar[0]) << t;
		}
		else
		{
			if (_fillCar.length())
			{
				o << t;
			}
		}
		o << _postfixFile;
	}
	return o.str();
}

const size_t FilenameManager::numGroups() const
{
	return _matchList.size();
}

const OfxRangeI FilenameManager::getRange(const ssize_t nGroup /* = -1 */)
{
	OfxRangeI range;
	if (nGroup >= 0 && nGroup < (ssize_t)_matchList.size())
	{
		range.min = _matchList[nGroup]._first;
		// Unscaled end time offset
		range.max = _matchList[nGroup]._last / _matchList[nGroup]._step;
	}
	else
	{
		range.min = _first;
		// Unscaled end time offset
		range.max = _last / _step;
	}
	return range;
}

vector< FilenamesGroup > FilenameManager::matchingGroups(const boost::filesystem::path & directory, const boost::regex & regex)
{
	vector< FilenamesGroup > groups;
	vector<string> l;
	list<FileDesc> filelist;
	string lastPrefix, lastPostfix;
	size_t lastNum = 0;
	size_t lastStep = 0;
	size_t first = 0;
	size_t lastNumFill = 0;
	if( exists( directory ) )
	{
		// Get directory files list
		directory_iterator end;
		for( directory_iterator iter(directory); iter != end; ++iter )
		{
			// Check if file
			if ( !is_directory( *iter ) )
			{
				if ( regex_match(iter->path().filename(), regex) )
				{
					filelist.push_back( FileDesc(iter->path().external_file_string(), regex) );
				}
			}
		}
		// Sort by groups
		filelist.sort( FileSort() );

		for(list<FileDesc>::const_iterator it = filelist.begin(); it != filelist.end(); ++it)
		{
			if (l.size() == 0)
			{
				first = it->_num;
			}
			// Detect group change
			else if ( l.size() && (lastPrefix != it->_prefix || lastPostfix != it->_postfix) )
			{
				FilenamesGroup g;
				g._filenames = l;
				g._first = first;
				g._last = lastNum;
				g._step = lastStep;
				g._prefix = it->_prefix;
				g._postfix = it->_postfix;
				g._numFill = it->_numFill;
				groups.push_back(g);
				l.clear();
				lastStep = 0;
				lastNum = 0;
			}
			else if ( l.size() && lastStep == 0 )
			{
				lastStep = (size_t)std::abs((int)(it->_num - lastNum));
			}
			else if ( l.size() )
			{
				if (std::abs((int)(it->_num - lastNum)) != (int)lastStep)
				{
					COUT_WARNING("Multiple steps in filename pattern (Image not found ?): " << it->_filenameLong);
				}
			}
			lastPrefix = it->_prefix;
			lastPostfix = it->_postfix;
			lastNum = it->_num;
			lastNumFill = it->_numFill;
			l.push_back(it->_filenameLong);
		}
	}
	// Sort filenames
	if (l.size())
	{
		FilenamesGroup g;
		g._filenames = l;
		g._first = first;
		g._last = lastNum;
		g._step = lastStep;
		g._prefix = lastPrefix;
		g._postfix = lastPostfix;
		g._numFill = lastNumFill;
		groups.push_back(g);
	}
	return groups;
}

}
}
