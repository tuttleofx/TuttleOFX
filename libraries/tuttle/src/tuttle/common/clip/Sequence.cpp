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
#include <boost/assert.hpp>
#include <boost/progress.hpp>

#include <iostream>
#include <map>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <list>
#include <boost/timer.hpp>

namespace tuttle {
namespace common {

using namespace boost::lambda;
namespace fs = boost::filesystem;

namespace {

/// All regex to recognize a pattern
/// @{

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

/// @}


/// Internal structures to detect sequence inside a directory
/// @{

/**
 * @brief Numbers inside a filename.
 * Each number can be a time inside a sequence.
 */
class SeqNumbers
{
public:
	typedef SeqNumbers This;
	typedef Sequence::Time Time;
	typedef std::pair<Time, std::string> Pair;
	typedef std::vector<Pair> Vec;
public:
	SeqNumbers()
	{
		// we preverse reserve and take memory,
		// that realloc and takes time.
		_numbers.reserve(10);
	}
public:
	void push_back( const std::string& s )
	{
		Time t;
		try
		{
			t = boost::lexical_cast<Time>(s);
		}
		catch(...)
		{
			// can't retrieve the number,
			// the number inside the string is probably
			// ouf of range for Time type.
			t = 0;
		}
		_numbers.push_back( Pair( t, s ) );
	}
	void clear()
	{
		_numbers.clear();
	}
	const std::string& getString( const std::size_t& i ) const { return _numbers[i].second; }
	std::size_t getPadding( const std::size_t& i ) const
	{
		const std::string& s = _numbers[i].second;
		if( s.size() == 1 )
			return 0;
		return s[0] == '0' ? s.size() : 0;
	}
	Time getTime( const std::size_t& i ) const { return _numbers[i].first; }
	std::size_t size() const { return _numbers.size(); }

	bool operator<( const This& v )
	{
		// can't have multiple size, if multiple size they must have a
		// different SeqId
		BOOST_ASSERT( _numbers.size() != v._numbers.size() );
		for( Vec::const_iterator i = _numbers.begin(), iEnd = _numbers.end(), vi = v._numbers.begin();
		     i != iEnd;
		     ++i, ++vi )
		{
			if( i->first < vi->first )
				return true;
			else if( i->first > vi->first )
				return false;
		}
		return false; // equals
	}
	friend std::ostream& operator<<( std::ostream& os, const This& p );
private:
	Vec _numbers;
};

std::ostream& operator<<( std::ostream& os, const SeqNumbers& p )
{
	os << "[";
//	std::for_each( p._numbers.begin(), p._numbers.end(), os <<
//		boost::bind( &SeqNumbers::Vec::value_type::second, boost::lambda::_1 ) << "," );
	BOOST_FOREACH( const SeqNumbers::Vec::value_type& v, p._numbers )
	{
		os << v.second << ",";
	}
	os << "]";
	return os;
}

/**
 * @brief Unique identification for a sequence
 */
class SeqId
{
public:
	typedef SeqId This;
	typedef std::list<std::string> Vec;

public:
    Vec& getId() { return _id; }
    const Vec& getId() const { return _id; }

	void clear()
	{
		_id.clear();
	}
	bool operator==( const This& v ) const
	{
		if( _id.size() != v._id.size() )
		{
			return false;
		}
		for( Vec::const_iterator i = _id.begin(), iEnd = _id.end(), vi = v._id.begin();
		     i != iEnd;
		     ++i, ++vi )
		{
			if( *i != *vi )
			{
				return false;
			}
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

/**
 * @brief Construct id and nums from a filename.
 * @param[in] str: the string to process (filename)
 * @param[out] id: list of strings
 * @param[out] nums: list of integers
 * @return number of decteted numbers
 */
TUTTLE_FORCEINLINE std::size_t seqConstruct( const std::string& str, SeqId& id, SeqNumbers& nums )
{
	static const boost::regex re("[\\-\\+]?\\d+");
	static const int subs[] = { -1, 0, }; // get before match and current match
	boost::sregex_token_iterator m( str.begin(), str.end(), re, subs );
	boost::sregex_token_iterator end;
	while( m != end )
	{
		// begin with string id, can be an empty string if str begins with a number
		id.getId().push_back( *m++ );
		if( m != end ) // if end with a string and not a number
		{
			nums.push_back( *m++ );
		}
	}
	if( id.getId().size() == nums.size() )
	{
		id.getId().push_back( "" ); // we end with an empty string
	}
//	TCOUT_VAR(str);
//	TCOUT_VAR(id);
//	TCOUT_VAR(nums);
	return nums.size();
}

/**
 * @brief Extract step from a sorted list of time values.
 */
std::size_t extractStep( const std::list<Sequence::Time>& times )
{
	if( times.size() == 1 )
		return 1;
	// init with first step
	std::size_t step = *(++times.begin()) - times.front(); // times[1] - times[0]
	// to get the step we use so smallest step, others are considered as missing frames
	// no check if multiple steps inside a sequence... ignored case.
	for( std::list<Sequence::Time>::const_iterator itA = times.begin(), itB = ++times.begin(), itEnd = times.end();
	     itB != itEnd;
	     ++ itA, ++itB )
	{
		std::size_t localStep = *itB - *itA;
		step = std::min( step, localStep );
	}
	return step;
}

/**
 * @brief Extract step from a sorted list of time values.
 */
std::size_t extractStep( const std::list<SeqNumbers>& times, const std::size_t i )
{
	if( times.size() == 1 )
		return 1;
	// init with first step
	std::size_t step = (*(++times.begin())).getTime(i) - times.front().getTime(i); // times[1] - times[0]
	// to get the step we use so smallest step, others are considered as missing frames
	// no check if multiple steps inside a sequence... ignored case.
	for( std::list<SeqNumbers>::const_iterator itA = times.begin(), itB = ++times.begin(), itEnd = times.end();
	     itB != itEnd;
	     ++ itA, ++itB )
	{
		std::size_t localStep = itB->getTime(i) - itA->getTime(i);
		step = std::min( step, localStep );
	}
	return step;
}

/// @}


}




Sequence::Sequence()
{
	clear();
}

Sequence::Sequence( const boost::filesystem::path& seqPath, const EPattern accept )
{
	clear();
	if( ! initFromDetection( seqPath, accept ) )
	{
		BOOST_THROW_EXCEPTION( std::logic_error("Unrecognized pattern.") );
	}
}

Sequence::~Sequence()
{
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
 * @param[in] pattern
 * @param[in] accept
 * @param[out] prefix
 * @param[out] suffix
 * @param[out] padding
 * @param[out] strictPadding
 */
bool Sequence::initFromPattern( const std::string& pattern, const EPattern& accept, std::string& prefix, std::string& suffix, std::size_t& padding, bool& strictPadding )
{
	boost::cmatch matches;
	if( (accept & ePatternStandard) &&
	    regex_match( pattern.c_str(), matches, regexPatternStandard ) )
	{
		std::string paddingStr( matches[2].first, matches[2].second );
		padding = paddingStr.size();
		strictPadding = ( paddingStr[0] == '#' );
	}
	else if( (accept & ePatternCStyle) &&
	         regex_match( pattern.c_str(), matches, regexPatternCStyle ) )
	{
		std::string paddingStr( matches[2].first, matches[2].second );
		padding = paddingStr.size() == 0 ? 0 : boost::lexical_cast<std::size_t>( paddingStr ); // if no padding value: %d -> padding = 0
		strictPadding = false;
	}
	else if( (accept & ePatternFrame) &&
	         regex_match( pattern.c_str(), matches, regexPatternFrame ) )
	{
		std::string frame( matches[2].first, matches[2].second );
//		Time t = boost::lexical_cast<Time>( frame );
		padding = frame.size();
		strictPadding = false;
	}
	else
	{
		// this is a file, not a sequence
		return false;
	}
	
	prefix = std::string( matches[1].first, matches[1].second );
	suffix = std::string( matches[3].first, matches[3].second );
	return true;
}

void Sequence::init( const boost::filesystem::path& directory, const std::string& prefix, const std::size_t padding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step, const bool strictPadding )
{
	clear();
	_directory = directory;
	_prefix = prefix;
	_padding = padding;
	_suffix = suffix;
	_firstTime = firstTime;
	_lastTime = lastTime;
	_step = step;
	_strictPadding = strictPadding;
	_nbFiles = 0;
}

bool Sequence::init( const boost::filesystem::path& directory, const std::string& pattern, const Time first, const Time last, const Time step, const EPattern accept )
{
	if( ! initFromPattern( pattern, accept, _prefix, _suffix, _padding, _strictPadding ) )
		return false; // not regognize as a pattern, maybe a still file

	init( directory, _prefix, _padding, _suffix, first, last, step, _strictPadding );
	return true;
}

bool Sequence::init( const boost::filesystem::path& seqPath, const Time first, const Time last, const Time step, const EPattern accept )
{
	boost::filesystem::path dir = seqPath.parent_path();
	if( dir.empty() ) // relative path
		dir = boost::filesystem::current_path();

	return this->init( dir, seqPath.filename(), first, last, step, accept );
}

bool Sequence::initFromDetection( const boost::filesystem::path& directory, const std::string& pattern, const EPattern accept )
{
	clear();
	_directory = directory;

	if( ! initFromPattern( pattern, accept, _prefix, _suffix, _padding, _strictPadding ) )
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
	_nbFiles = allTimes.size();

	return true; // a real file sequence
}

/**
 * @brief Create a sequence
 * @param[in] id: the sequence identifier
 * @param[inout] nums: the list of sequence of number inside each filename
 * @warning We modify nums in place, because we don't use it after
 *          so there is no reason to create a copy.
 * @return a sequence object with all informations
 */
Sequence buildSequence( const boost::filesystem::path& directory, const SeqId& id, std::list<SeqNumbers>& nums )
{
	typedef Sequence::Time Time;
	nums.sort();

	BOOST_ASSERT( nums.size() > 1 );
	// assert all SeqNumber has the same size...
	BOOST_ASSERT( nums.front().size() == nums.back().size() );

	std::size_t len = nums.front().size();
//	TCOUT_VAR(len);

	// detect which part is the sequence number
	// for the moment, accept only one sequence
	// but we can easily support multi-sequences
	std::vector<std::size_t> allIds; // list of ids (with 0<id<len) with value changes
	for( std::size_t i = 0; i<len; ++i )
	{
		const Time t = nums.front().getTime(i);
		BOOST_FOREACH( std::list<SeqNumbers>::value_type& sn, nums )
		{
			if( sn.getTime(i) != t )
			{
				allIds.push_back( i );
				break;
			}
		}
	}
//	unsigned int i = 0;
//	BOOST_FOREACH( const std::list<SeqNumbers>::value_type& sn, nums )
//	{
//		TCOUT( "seq " << i++ << " : " <<  sn);
//	}
	std::size_t idNum;
//	TCOUT_VAR(allIds.size());
	if( allIds.size() == 0 )
	{
		idNum = 0;
	}
	else if( allIds.size() == 1 )
	{
		idNum = allIds[0];
	}
	else
	{
		BOOST_FOREACH( const std::size_t& s, allIds )
		{
			COUT_VAR(s);
		}
		BOOST_THROW_EXCEPTION( std::logic_error( "Multi-sequence unsupported." ) );
	}
//	TCOUT_VAR( idNum );

	Sequence s;
	// fill information in the sequence...
	s._directory = directory;
	SeqId::Vec::const_iterator idIt = id.getId().begin();
	for( std::size_t i = 0; i<idNum; ++i )
	{
		s._prefix += *idIt++;
		s._prefix += nums.front().getString(i);
	}
	s._prefix += *idIt++;
	for( std::size_t i = idNum+1; i<len; ++i )
	{
		s._suffix += *idIt++;
		s._suffix += nums.front().getString(i);
	}
	s._suffix += *idIt++;
	BOOST_ASSERT( idIt == id.getId().end() );

	s._padding = nums.front().getPadding(idNum);
	s._strictPadding = (s._padding != 0);
	if( s._strictPadding )
	{
		BOOST_FOREACH( const std::list<SeqNumbers>::value_type& sn, nums )
		{
			// if multiple paddings
//			s._padding = std::max( sn.getString(idNum).size(), s._padding );
			if( sn.getString(idNum).size() != s._padding )
				s._strictPadding = 0;
//				BOOST_THROW_EXCEPTION( std::logic_error( "Multiple paddings inside the same sequence." ) );
		}
	}
	s._step = extractStep( nums, idNum );

	s._firstTime = nums.front().getTime(idNum);
	s._lastTime = nums.back().getTime(idNum);
	s._nbFiles = nums.size();
	return s;
}



std::vector<Sequence> sequencesInDir( const boost::filesystem::path& directory )
{
	typedef Sequence::Time Time;
	
	std::vector<Sequence> output;

	if( ! exists( directory ) )
		return output;

	typedef boost::unordered_map<SeqId, std::list<SeqNumbers>, SeqIdHash> SeqIdMap;
	SeqIdMap sequences;

//	TCOUT( "listdir begin" );

	SeqId id; // an object uniquely identify a sequence
	SeqNumbers nums; // the list of numbers inside one filename

	fs::directory_iterator itEnd;
	for( fs::directory_iterator iter(directory); iter != itEnd; ++iter )
	{
//		if( fs::is_directory( iter->status() ) )
//			continue; // skip directories

		// clear previous infos
		id.clear();
		nums.clear(); // (clear but don't realloc the vector inside)

		// if at least one number detected
		if( seqConstruct( iter->filename(), id, nums) )
		{
			const SeqIdMap::iterator it( sequences.find( id ) );
			if( it != sequences.end() ) // is already in map
			{
				// append the list of numbers
				sequences.at(id).push_back( nums );
			}
			else
			{
				// create an entry in the map
				std::list<SeqNumbers> li;
				li.push_back( nums );
				sequences.insert( SeqIdMap::value_type(id, li) );
			}
		}
	}

//	TCOUT( "listdir end" );

//	TCOUT_VAR( sequences.size() );
	
	output.reserve( sequences.size() );

	BOOST_FOREACH( SeqIdMap::value_type& p, sequences )
	{
		Sequence s = buildSequence( directory, p.first, p.second );

		// don't detect sequence of directories
		if( ! fs::is_directory( s.getAbsoluteFirstFilename() ) )
		{
			output.push_back( s );
		}
	}

	return output;
}



std::vector<Sequence> sequencesInDir( const boost::filesystem::path& directory, const boost::regex& filter )
{
	return sequencesInDir( directory );
}

std::ostream& operator<<( std::ostream& os, const Sequence& s )
{
	os << s.getDirectory() / s.getStandardPattern()
	   << " [" << s.getFirstTime() << ":" << s.getLastTime();
	if( s.getStep() != 1 )
		os << "x" << s.getStep();
	os << "]"
	   << " " << s.getNbFiles() << " file" << ((s.getNbFiles()>1) ?"s":"");
	if( s.hasMissingFile() )
	   os << ", " << s.getNbMissingFiles() << " missing file" << ((s.getNbMissingFiles()>1) ?"s":"");
	return os;
}


}
}
