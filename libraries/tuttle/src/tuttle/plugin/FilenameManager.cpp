#include "FilenameManager.hpp"
#include "tuttle/common/utils/global.hpp"
#include "PluginException.hpp"

#include <limits>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <list>

using namespace boost::filesystem;
using namespace boost;
using namespace std;

namespace tuttle {
namespace plugin {

///@todo: windows filename check
FilenameManager::FilenameManager(const path & directory, const std::string pattern,
                                 const bool dirbase /* = false */, const size_t start /*= 0*/, const size_t step /* = 1 */)
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
{
	reset(filepath, dirbase, start, step);
}

FilenameManager::~FilenameManager()
{
}

bool FilenameManager::reset(const boost::filesystem::path& filepath, const bool dirbase, const size_t start, const size_t step)
{
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
	const regex unesc("[\\\\]");
	string directory = filepath.parent_path().string() + "/";
	string escPattern = regex_replace(filepath.string(), esc, "\\\\\\1&", boost::match_default | boost::format_sed);
	_fillCar = "0";
	// After effects style ( eg. filename_[####].png )
	if ( regex_match(filepath.filename(), regex(".*\\[#+\\].*")) )
	{
		// Parse to get filling information
		cmatch matches;
		regex_match( filepath.string().c_str(), matches, regex("(.*\\/)(.*?)\\[(#+)\\](.*)") );
		_prefixDir = regex_replace(std::string( matches[1].first, matches[1].second ), unesc, "\\\\\\1&", match_default | format_sed);
		_prefixFile = regex_replace(std::string( matches[2].first, matches[2].second ), unesc, "\\\\\\1&", match_default | format_sed);
		_fillCar = "0";
		_numFill = (string( matches[3].first, matches[3].second )).length();
		_postfixFile = regex_replace(string( matches[4].first, matches[4].second ), unesc, "\\\\\\1&", match_default | format_sed);

		if (dirbase)
		{
			// Parse filenames sequence
			regex rx = regex( "(.*\\\\/)(.*)\\\\\\[#+\\\\\\](.*\\..+)" );
			std::ostringstream o;
			o << "(\\1)(\\2)\\(\\\\\\d{1, " << max << "}\\)(\\3)\\$";
			_pattern = regex_replace(escPattern, rx, o.str(), match_default | format_sed);
		}
	}
	// Nuke style ( eg. filename_%04d.png )
	else if ( regex_match(filepath.filename(), regex(".*%[0-9]+d.*")) )
	{
		// Parse to get filling information
		cmatch matches;
		regex_match( filepath.string().c_str(), matches, regex("(.*\\/)(.*?)%([0-9]|\\w)?([0-9]*)d(.*)") );
		_prefixDir  = string( matches[1].first, matches[1].second );
		_prefixFile = string( matches[2].first, matches[2].second );
		_fillCar    = string( matches[3].first, matches[3].second );
		istringstream strm(std::string( matches[4].first, matches[4].second ));
		strm >> _numFill;
		_postfixFile = std::string( matches[5].first, matches[5].second );

		if (dirbase)
		{
			// Parse filenames sequence
			regex rx = regex( "(.*\\\\/)(.*)%[0-9]+d(.*\\..+)" );
			std::ostringstream o;
			o << "(\\1)(\\2)\\(\\\\\\d{1, " << max << "}\\)(\\3)\\$";
			_pattern = regex_replace(escPattern, rx, o.str(), boost::match_default | boost::format_sed);
		}
	}
	// Common style with padding ( eg. filename_####.png )
	else if ( regex_match(filepath.filename(), regex(".*?#+.*")) )
	{
		// Parse to get filling information
		cmatch matches;
		regex_match( filepath.string().c_str(), matches, regex("(.*\\/)(.*?)(#+)(.*)") );
		_fillCar = "0";
		_prefixDir   = string( matches[1].first, matches[1].second );
		_prefixFile  = string( matches[2].first, matches[2].second );
		_numFill     = (string( matches[3].first, matches[3].second )).length();
		_postfixFile = string( matches[4].first, matches[4].second );

		if (dirbase)
		{
			// Parse filenames sequence
			regex rx = regex( "(.*\\\\/)(.*?)#+(.*\\..+)" );
			std::ostringstream o;
			o << "(\\1)(\\2)\\(\\\\\\d{1, " << max << "}\\)(\\3)\\$";
			_pattern = regex_replace(escPattern, rx, o.str(), boost::match_default | boost::format_sed);
		}
	}
	// Common style without padding ( eg. filename_@.png )
	else if ( regex_match(filepath.filename(), regex(".*?@+.*")) )
	{
		// Parse to get filling information
		cmatch matches;
		regex_match( filepath.string().c_str(), matches, regex("(.*\\/)(.*?)(@+)(.*)") );
		_fillCar = "0";
		_numFill = 0;
		_prefixDir = std::string( matches[1].first, matches[1].second );
		_prefixFile = std::string( matches[2].first, matches[2].second );
		_postfixFile = std::string( matches[4].first, matches[4].second );

		if (dirbase)
		{
			// Parse filenames sequence
			regex rx = regex( "(.*\\\\/)(.*?)@+(.*\\..+)" );
			std::ostringstream o;
			o << "(\\1)(\\2)\\(\\\\\\d{1, " << max << "}\\)(\\3\\)\\$";
			_pattern = regex_replace(escPattern, rx, o.str(), boost::match_default | boost::format_sed);
		}
	}
	else
	{
		boost::cmatch matches;
		boost::regex_match( filepath.string().c_str(), matches, regex("(.*\\/)(.*?)(\\d*)(\\..*)") );
		_prefixDir = std::string( matches[1].first, matches[1].second );
		_prefixFile = std::string( matches[2].first, matches[2].second );
		_postfixFile = std::string( matches[4].first, matches[4].second );
		string num = string( matches[3].first, matches[3].second );
		std::istringstream strm(num);
		strm >> _currentPos;
		ostringstream o;
		o << _currentPos;
		_fillCar = "0";
		_numFill = num.length() - o.str().length();
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
	_currentPos = ssize_t(time);
	// Check for fractionnal part
	if (time != double(_currentPos) )
	{
		COUT_WARNING("Warning ! Passing fractionnal time value. Frame will be overwritten.");
	}

	// Scale & translate
	size_t t = _currentPos * _step;
	std::ostringstream o, pad;
	pad << t;
	size_t fillw = pad.str().length() + _numFill;
	if (nGroup >= 0 && nGroup < (ssize_t)_matchList.size())
	{
		o << _prefixDir << _prefixFile << nGroup << "_" << setw(fillw) << setfill(_fillCar[0]) << t << _postfixFile;
	}
	else
	{
		o << _prefixDir << _prefixFile << setw(fillw) << setfill(_fillCar[0]) << t << _postfixFile;
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
				if ( regex_match(iter->path().external_file_string(), regex) )
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
