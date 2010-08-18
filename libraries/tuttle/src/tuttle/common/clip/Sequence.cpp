#include "Sequence.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include <limits>
#include <iomanip>
#include <list>

namespace tuttle {
namespace common {

namespace fs = boost::filesystem;

Sequence::Sequence()
: _strictPadding(0)
, _padding(0)
, _step(1)
, _firstTime(0)
, _lastTime(0)
, _nbFrames(0)
{
}

Sequence::Sequence( const boost::filesystem::path& seqPath )
: _strictPadding(0)
, _padding(0)
, _step(1)
, _firstTime(0)
, _lastTime(0)
, _nbFrames(0)
{
	init( seqPath );
}

Sequence::~Sequence()
{
}

/**
 * @brief Extract step from a sorted list of time values.
 */
std::size_t Sequence::extractStep( const std::list<Time>& times )
{
	// init with first step
	std::size_t step = *(++times.begin()) - times.front(); // times[1] - times[0]
	// to get the step we use so smallest step, others are considered as missing frames
	// no check if multiple steps inside a sequence... ignored case.
	for( std::list<Time>::const_iterator itA = times.begin(), itB = ++times.begin(), itEnd = times.end();
	     itB != itEnd;
	     ++ itA, ++itB )
	{
		std::size_t localStep = *itB - *itA;
		step = std::min( step, localStep );
	}
	return step;
}

bool Sequence::isInSequence( const std::string& filename, Time& time )
{
	std::size_t min = _prefix.size() + _suffix.size();
	if( filename.size() <= min )
		return false;

	if( filename.substr(0,_prefix.size()) != _prefix ||
		filename.substr(filename.size()-_suffix.size(),_suffix.size()) != _suffix )
		return false;

	try
	{
		time = boost::lexical_cast<Time>(
			filename.substr( _prefix.size(),
						     filename.size()-_suffix.size()-_prefix.size() ) );
	}
	catch(...)
	{
		return false;
	}
	return true;
}

void Sequence::init( const boost::filesystem::path& directory, const std::string& pattern )
{
	clear();
	initFromPattern( pattern );

	if( ! exists( directory ) )
		return;

	std::list<Time> allFrames;

	fs::directory_iterator itEnd;
	for( fs::directory_iterator iter(directory); iter != itEnd; ++iter )
	{
		// skip directories
		if( fs::is_directory( iter->status() ) )
			continue;
		
		Time time;
		if( isInSequence( iter->filename(), time ) )
		{
			allFrames.push_back( time );
		}
	}
	
	if( allFrames.size() < 2 ) // not really a sequence...
	{
		if( allFrames.size() == 1 )
		{
			_firstTime = _lastTime = allFrames.front();
		}
		return;
	}

	allFrames.sort();

	_step = extractStep( allFrames );

	_firstTime = allFrames.front();
	_lastTime = allFrames.back();
	_nbFrames = allFrames.size();
}


/**
 * @brief This function creates a regex from the pattern,
 *        and init internal values.
 */
void Sequence::initFromPattern( const std::string& pattern )
{
	// common used pattern with # or @
	static const boost::regex standardRegex(
		"(.*?)"             // anything but without priority
	    "\\[?"              // if pattern is myimage[####].jpg, don't capture []
		"(#+|@+)"           // we capture all # or @
		"\\]?"              // possible end of []
		"(.*?)"             // anything
		);
	// C style pattern
	static const boost::regex cstyleRegex(
		"(.*?)"             // anything but without priority
	    "\\[?"              // if pattern is myimage[%04d].jpg, don't capture []
		"%([0-9]*)d"        // we capture the padding value (eg. myimage%04d.jpg)
		"\\]?"              // possible end of []
		"(.*?)"             // anything
		);
	// image name
	static const boost::regex frameRegex(
		"(.*?"              // anything but without priority
		"[_\\.]?)"          // if multiple numbers, the number surround with . _ get priority (eg. seq1shot04myimage.0123.jpg -> 0123)
	    "\\[?"              // if pattern is myimage[0001].jpg, don't capture []
		"([\\-\\+]?[0-9]+)" // one frame number, can be positive or negative values ( -0012 or +0012 or 0012)
		"\\]?"              // possible end of []
		"([_\\.]?"          // if multiple numbers, the number surround with . _ get priority (eg. seq1shot04myimage.0123.jpg -> 0123)
	    ".*\\.?"            //
		".*?)"              // anything
		);

	boost::cmatch matches;
	if( regex_match( pattern.c_str(), matches, standardRegex ) )
	{
		COUT_INFOS;
		std::string paddingStr( matches[2].first, matches[2].second );
		COUT_VAR(paddingStr);
		_padding = paddingStr.size();
		_strictPadding = ( paddingStr[0] == '#' );
	}
	else if( regex_match( pattern.c_str(), matches, cstyleRegex ) )
	{
		COUT_INFOS;
		std::string paddingStr( matches[2].first, matches[2].second );
		COUT_VAR(paddingStr);
		_padding = paddingStr.size() == 0 ? 0 : boost::lexical_cast<std::size_t>( paddingStr ); // if no padding value: %d -> padding = 0
		_strictPadding = false;
	}
	else if( regex_match( pattern.c_str(), matches, frameRegex ) )
	{
		COUT_INFOS;
		std::string frame( matches[2].first, matches[2].second );
		COUT_VAR(frame);
		Time t = boost::lexical_cast<Time>( frame );
		_padding = frame.size();
		// init begin/end to the pattern frame value
		_firstTime = _lastTime = t;
		_strictPadding = false;
	}
	else
	{
		BOOST_THROW_EXCEPTION( std::logic_error("Unrecognized pattern.") );
	}
	COUT_INFOS;
	_prefix = std::string( matches[1].first, matches[1].second );
	_suffix = std::string( matches[3].first, matches[3].second );
}


std::vector<Sequence> sequencesInDir( const boost::filesystem::path& directory )
{
	typedef Sequence::Time Time;
	static const boost::regex expression(
		"(.*"              // anything but without priority
		"[_\\.]?)"          // if multiple numbers, the number surround with . _ get priority (eg. seq1shot04myimage.0123.jpg -> 0123)
		"([\\-\\+]?[0-9]+)" // one frame number, can be positive or negative values ( -0012 or +0012 or 0012)
		"([_\\.]?"          // if multiple numbers, the number surround with . _ get priority (eg. seq1shot04myimage.0123.jpg -> 0123)
	    ".*?\\.?"            //
		".*?)"              // anything
		);
	std::vector<Sequence> output;

	if( ! exists( directory ) )
		return output;

	typedef std::pair<Sequence, std::list<Time> > SeqTmp;
	typedef std::map<std::string, SeqTmp> SeqTmpMap;
	SeqTmpMap sequences;

	fs::directory_iterator itEnd;
	for( fs::directory_iterator iter(directory); iter != itEnd; ++iter )
	{
		// skip directories
		if( fs::is_directory( iter->status() ) )
			continue;

		boost::cmatch matches;
		if( boost::regex_match( iter->filename().c_str(), matches, expression ) )
		{
			std::string prefix( matches[1].first, matches[1].second );
			std::string timeStr( matches[2].first, matches[2].second );
			std::string suffix( matches[3].first, matches[3].second );
			std::string id( prefix+suffix );
			COUT_VAR( iter->filename() );
			COUT_VAR( prefix );
			COUT_VAR( suffix );
			COUT_VAR( timeStr );
			Time time = boost::lexical_cast<Time>( timeStr );

			SeqTmpMap::iterator it = sequences.find( id );
			if( it == sequences.end() )
			{
				Sequence s;
				s._directory = directory;
				s._prefix = prefix;
				s._suffix = suffix;
				s._padding = timeStr.size();
				s._firstTime = time;
				s._lastTime = time;
				s._nbFrames = 1;
				sequences[id] = SeqTmp( s, std::list<Time>() );
			}
			else
			{
				it->second.second.push_back( time );
				// no check for different padding... ignored case.
			}
		}
	}

	BOOST_FOREACH( SeqTmpMap::value_type p, sequences )
	{
		SeqTmp& sTmp = p.second;
		Sequence& s = sTmp.first;
		std::list<Time>& t = sTmp.second;
		t.sort();
		s._firstTime = t.front();
		s._lastTime = t.back();
		s._nbFrames = t.size();
		s._step = Sequence::extractStep( t );
	}

	output.reserve( sequences.size() );
	
	std::transform( sequences.begin(), sequences.end(), std::back_inserter(output),
				 boost::bind(&SeqTmpMap::value_type::second_type::first,
						  boost::bind(&SeqTmpMap::value_type::second, _1) ) );

	return output;
}


std::vector<Sequence> sequencesInDir( const boost::filesystem::path& directory, const boost::regex& filter )
{
	return sequencesInDir( directory );
}

std::ostream& operator<<( std::ostream& os, const Sequence& v )
{
	os << "dir:" << v.getDirectory() << std::endl;
	os << "first file:" << v.getFirstFilename() << std::endl;
	os << "last file:" << v.getLastFilename() << std::endl;
	os << "step:" << v.getStep() << std::endl;
	os << "first time:" << v.getFirstTime() << std::endl;
	os << "last time:" << v.getLastTime() << std::endl;
	os << "padding:" << v.getPadding() << std::endl;
	os << "strict padding:" << v.isStrictPadding() << std::endl;
	os << "has missing frames:" << v.hasMissingFrames() << std::endl;
	os << "identification:" << v.getIdentification() << std::endl;
	os << "prefix:" << v.getPrefix() << std::endl;
	os << "suffix:" << v.getSuffix() << std::endl;
	return os;
}


}
}
