#include "Sequence.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/functional/hash.hpp>
#include <boost/throw_exception.hpp>
#include <boost/mem_fn.hpp>

#include <iostream>
#include <map>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <list>

namespace tuttle {
namespace common {

using namespace boost::lambda;
namespace fs = boost::filesystem;

namespace {
static const boost::regex regexFileSequence(
	"(.*)"              // anything but without priority
	"([0-9]+)" // one frame number, can be positive or negative values ( -0012 or +0012 or 0012)
	"(.*)"              // anything
//	"(.*"              // anything but without priority
//	"[_\\.]?)"          // if multiple numbers, the number surround with . _ get priority (eg. seq1shot04myimage.0123.jpg -> 0123)
//	"([\\-\\+]?[0-9]+)" // one frame number, can be positive or negative values ( -0012 or +0012 or 0012)
//	"([_\\.]?"          // if multiple numbers, the number surround with . _ get priority (eg. seq1shot04myimage.0123.jpg -> 0123)
//	//".*?\\.?"            //
//	".*?)"              // anything
	);

// common used pattern with # or @
static const boost::regex regexPatternStandard(
	"(.*?)"             // anything but without priority
	"\\[?"              // if pattern is myimage[####].jpg, don't capture []
	"(#+|@+)"           // we capture all # or @
	"\\]?"              // possible end of []
	"(.*?)"             // anything
	);
// C style pattern
static const boost::regex regexPatternCStyle(
	"(.*?)"             // anything but without priority
	"\\[?"              // if pattern is myimage[%04d].jpg, don't capture []
	"%([0-9]*)d"        // we capture the padding value (eg. myimage%04d.jpg)
	"\\]?"              // possible end of []
	"(.*?)"             // anything
	);
// image name
static const boost::regex regexPatternFrame(
	"(.*?"              // anything but without priority
	"[_\\.]?)"          // if multiple numbers, the number surround with . _ get priority (eg. seq1shot04myimage.0123.jpg -> 0123)
	"\\[?"              // if pattern is myimage[0001].jpg, don't capture []
	"([\\-\\+]?[0-9]+)" // one frame number, can be positive or negative values ( -0012 or +0012 or 0012)
	"\\]?"              // possible end of []
	"([_\\.]?"          // if multiple numbers, the number surround with . _ get priority (eg. seq1shot04myimage.0123.jpg -> 0123)
	".*\\.?"            //
	".*?)"              // anything
	);

}

Sequence::Sequence()
: _strictPadding(0)
, _padding(0)
, _step(1)
, _firstTime(0)
, _lastTime(0)
, _nbFrames(0)
{
}

Sequence::Sequence( const boost::filesystem::path& seqPath, const EPattern& accept )
: _strictPadding(0)
, _padding(0)
, _step(1)
, _firstTime(0)
, _lastTime(0)
, _nbFrames(0)
{
	if( ! init( seqPath, accept ) )
	{
		BOOST_THROW_EXCEPTION( std::logic_error("Unrecognized pattern.") );
	}
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

/**
 * @return if the filename can be a inside a sequence.
 */
bool Sequence::isASequenceFilename( const std::string& filename )
{
	return boost::regex_match( filename.c_str(), regexFileSequence );
}

bool Sequence::isIn( const std::string& filename, Time& time )
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

Sequence::EPattern Sequence::checkPattern( const std::string& pattern )
{
	if( regex_match( pattern.c_str(), regexPatternStandard ) )
	{
		return ePatternStandard;
	}
	else if( regex_match( pattern.c_str(), regexPatternCStyle ) )
	{
		return ePatternCStyle;
	}
	else if( regex_match( pattern.c_str(), regexPatternFrame ) )
	{
		return ePatternFrame;
	}
	return ePatternNone;
}

/**
 * @brief This function creates a regex from the pattern,
 *        and init internal values.
 */
bool Sequence::initFromPattern( const std::string& pattern, const EPattern& accept )
{
	boost::cmatch matches;
	if( (accept & ePatternStandard) &&
	    regex_match( pattern.c_str(), matches, regexPatternStandard ) )
	{
		std::string paddingStr( matches[2].first, matches[2].second );
		_padding = paddingStr.size();
		_strictPadding = ( paddingStr[0] == '#' );
	}
	else if( (accept & ePatternCStyle) &&
	         regex_match( pattern.c_str(), matches, regexPatternCStyle ) )
	{
		std::string paddingStr( matches[2].first, matches[2].second );
		_padding = paddingStr.size() == 0 ? 0 : boost::lexical_cast<std::size_t>( paddingStr ); // if no padding value: %d -> padding = 0
		_strictPadding = false;
	}
	else if( (accept & ePatternFrame) &&
	         regex_match( pattern.c_str(), matches, regexPatternFrame ) )
	{
		std::string frame( matches[2].first, matches[2].second );
		Time t = boost::lexical_cast<Time>( frame );
		_padding = frame.size();
		// init begin/end to the pattern frame value
		_firstTime = _lastTime = t;
		_strictPadding = false;
	}
	else
	{
		// this is a file, not a sequence
		return false;
	}
	
	_prefix = std::string( matches[1].first, matches[1].second );
	_suffix = std::string( matches[3].first, matches[3].second );
	return true;
}

bool Sequence::init( const boost::filesystem::path& directory, const std::string& pattern, const EPattern& accept )
{
	clear();
	_directory = directory;

	if( ! initFromPattern( pattern, accept ) )
		return false; // not regognize as a pattern, maybe a still file

	if( ! exists( directory ) )
		return true; // an empty sequence

	std::list<Time> allTimes;

	fs::directory_iterator itEnd;
	for( fs::directory_iterator iter(directory); iter != itEnd; ++iter )
	{
		// we don't make this check, which can take long time on big sequences (>1000 files)
		// depending on your filesystem, we may need to do a stat() for each file
//		if( fs::is_directory( iter->status() ) )
//			continue; // skip directories

		Time time;
		if( isIn( iter->filename(), time ) )
		{
			// create a big list of all times in our sequence
			allTimes.push_back( time );
		}
	}

	if( allTimes.size() < 2 )
	{
		if( allTimes.size() == 1 )
		{
			_firstTime = _lastTime = allTimes.front();
		}
		return true; // an empty sequence
	}

	allTimes.sort();

	_step = extractStep( allTimes );

	_firstTime = allTimes.front();
	_lastTime = allTimes.back();
	_nbFrames = allTimes.size();

	return true; // a real file sequence
}


// Internal structures to detect sequence inside a directory
namespace {

/**
 * @brief numbers inside a filename.
 * Each number can be a time inside a sequence.
 */
typedef std::vector<std::size_t> SeqNumbers;

/**
 * @brief Unique identification for a sequence
 */
class SeqId
{
public:
	typedef SeqId This;
	typedef std::vector<std::string> Vec;
    Vec getId() { return _id; }
	bool operator==( const This& v ) const
	{
		if( _id.size() != v._id.size() )
			return false;
		for( Vec::const_iterator i = _id.begin(), iEnd = _id.end(), vi = v._id.begin();
		     i != iEnd;
		     ++i, ++vi )
		{
			if( i != vi)
				return false;
		}
		return true;
	}
    std::size_t getHash() const
    {
        std::size_t seed = 0;
		BOOST_FOREACH( const Vec::value_type& i, _id )
		{
			boost::hash_combine( seed, i );
			boost::hash_combine( seed, 1 ); // not like the hash of the concatenation of _id
		}
		return seed;
    }
	friend std::ostream& operator<<( std::ostream& os, const This& p );
	
private:
	Vec _id;
};

std::ostream& operator<<( std::ostream& os, const SeqId& p )
{
	os << "[";
	std::for_each( p._id.begin(), p._id.end(), os << boost::lambda::_1 << "," );
	os << "]";
	return os;
}

// How we can replace this with a wrapper?
// Like boost::function, boost::bind,...
struct SeqIdHash : std::unary_function<SeqId, std::size_t>
{
    std::size_t operator()(const SeqId& p) const
    {
        return p.getHash();
    }
};

std::size_t seqConstruct( const std::string& str, SeqId& id, SeqNumbers& nums )
{
	static const boost::regex re("[\\-\\+]?\\d+");
	static const int subs[] = { -1, 0, }; // get before match and current match
	boost::sregex_token_iterator m( str.begin(), str.end(), re, subs );
	boost::sregex_token_iterator end;
	while( m != end )
	{
		// begin with string id, can be an empty string if str begins with a number
		id.getId().push_back( *m++ );
		if( m != end )
			nums.push_back( boost::lexical_cast<std::size_t>(*m++) );
	}
	return id.getId().size();
}

}

void example()
{
	typedef boost::unordered_map<SeqId, std::list<SeqNumbers>, SeqIdHash> SeqIdMap;
	SeqIdMap m;
	SeqId id;
	SeqNumbers nums;
	seqConstruct("myImage.10_32.frame3.jpg", id, nums);

	m.at(id).push_back( nums );
}


std::vector<Sequence> sequencesInDir( const boost::filesystem::path& directory )
{
	typedef Sequence::Time Time;

	COUT_VAR(regexFileSequence);
	
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
		if( boost::regex_match( iter->filename().c_str(), matches, regexFileSequence ) )
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
						  boost::bind(&SeqTmpMap::value_type::second, boost::lambda::_1) ) );

	return output;
}


std::vector<Sequence> sequencesInDir( const boost::filesystem::path& directory, const boost::regex& filter )
{
	return sequencesInDir( directory );
}

std::ostream& operator<<( std::ostream& os, const Sequence& v )
{
	os << "dir:" << v.getDirectory() << std::endl;
	os << "first file:" << v.getAbsoluteFirstFilename() << std::endl;
	os << "last file:" << v.getAbsoluteLastFilename() << std::endl;
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
