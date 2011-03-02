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
#include <boost/shared_ptr.hpp>

#include <iostream>
#include <map>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <list>
#include <boost/timer.hpp>
#include <set>
#include <list>

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
class FileNumbers
{
public:
	typedef FileNumbers This;
	typedef Sequence::Time Time;
	typedef std::pair<Time, std::string> Pair;
	typedef std::vector<Pair> Vec;

public:
	FileNumbers()
	{
		// we preverse reserve and take memory,
		// that realloc and takes time.
		_numbers.reserve( 10 );
	}

public:
	void push_back( const std::string& s )
	{
		Time t;

		try
		{
			t = boost::lexical_cast<Time>( s );
		}
		catch(... )
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
	std::size_t        getPadding( const std::size_t& i ) const
	{
		const std::string& s = _numbers[i].second;

		if( s.size() == 1 )
			return 0;
		return s[0] == '0' ? s.size() : 0;
	}
	Time        getTime( const std::size_t& i ) const { return _numbers[i].first; }
	
	std::size_t size() const                          { return _numbers.size(); }

	bool operator<( const This& v )
	{
		// can't have multiple size, if multiple size they must have a
		// different SeqId
		BOOST_ASSERT( _numbers.size() == v._numbers.size() );
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
	bool rangeEquals( const This& v, const size_t begin, const size_t end ) const
	{
		for( size_t i = begin; i < end; ++i )
		{
			const Pair& me = this->_numbers[i];
			const Pair& other = v._numbers[i];

			//me.second.size() != other.second.size() ) // we don't check the padding...
			if( me.first != other.first )
				return false;
		}
		return true;
	}

	friend std::ostream& operator<<( std::ostream& os, const This& p );

private:
	Vec _numbers;
};

std::ostream& operator<<( std::ostream& os, const FileNumbers& p )
{
	os << "[";
	//	std::for_each( p._numbers.begin(), p._numbers.end(), os <<
	//		boost::bind( &SeqNumbers::Vec::value_type::second, boost::lambda::_1 ) << "," );
	BOOST_FOREACH( const FileNumbers::Vec::value_type & v, p._numbers )
	{
		os << v.second << ",";
	}
	os << "]";
	return os;
}

/**
 * @brief Unique identification for a sequence
 */
class FileStrings
{
public:
	typedef FileStrings This;
	typedef std::vector<std::string> Vec;

public:
	Vec&       getId()       { return _id; }
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

	const std::string& operator[]( const std::size_t i ) const { return _id[i]; }
	
	std::size_t getHash() const
	{
		std::size_t seed = 0;

		BOOST_FOREACH( const Vec::value_type & i, _id )
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

std::ostream& operator<<( std::ostream& os, const FileStrings& p )
{
	os << "[";
	std::for_each( p._id.begin(), p._id.end(), os << boost::lambda::_1 << "," );
	os << "]";
	return os;
}

// How we can replace this with a wrapper?
// Like boost::function, boost::bind,...
struct SeqIdHash : std::unary_function<FileStrings, std::size_t>
{
	std::size_t operator()( const FileStrings& p ) const
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
TUTTLE_FORCEINLINE std::size_t seqConstruct( const std::string& str, FileStrings& id, FileNumbers& nums )
{
	static const std::size_t max = std::numeric_limits<std::size_t>::digits10;
	static const boost::regex re( "[\\-\\+]?\\d*?\\d{1,"+boost::lexical_cast<std::string>(max)+"}" );
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
// 		TUTTLE_TCOUT_VAR(str);
// 		TUTTLE_TCOUT_VAR(id);
// 		TUTTLE_TCOUT_VAR(nums);
/*
	std::string extension = id.getId().at(id.getId().size()-1); 
// 	TUTTLE_COUT( extension );
// 	TUTTLE_COUT( (int)extension.find_first_of(".") );
	if( extension.find_first_of(".")==0 )
	    return nums.size();
	else
	    return 0;*/
	return nums.size();
}

/**
 * @brief extract the padding from a list of frame numbers
 * @param[in] timesStr list of frame numbers in string format
 */
std::size_t extractPadding( const std::list<std::string>& timesStr )
{
	BOOST_ASSERT( timesStr.size() > 0 );
	const std::size_t padding = timesStr.front().size();
	BOOST_FOREACH( const std::string& s, timesStr )
	{
		if( padding != s.size() )
			return 0;
	}
	return padding;
}

std::size_t extractPadding( const std::list<FileNumbers>& times, const std::size_t i )
{
	BOOST_ASSERT( times.size() > 0 );
	const std::size_t padding = times.front().getString(i).size();
	BOOST_FOREACH( const FileNumbers& s, times )
	{
		if( padding != s.getString(i).size() )
			return 0;
	}
	return padding;
}

/**
 * @brief return if the padding is strict (at least one frame begins with a '0' padding character).
 * @param[in] timesStr list of frame numbers in string format
 * @param[in] padding previously detected padding
 */
bool extractIsStrictPadding( const std::list<std::string>& timesStr, const std::size_t padding )
{
	if( padding == 0 )
		return false;
	
	BOOST_FOREACH( const std::string& s, timesStr )
	{
		if( s[0] == '0' )
			return true;
	}
	return false;
}

bool extractIsStrictPadding( const std::list<FileNumbers>& times, const std::size_t i, const std::size_t padding )
{
	if( padding == 0 )
		return false;

	BOOST_FOREACH( const FileNumbers& s, times )
	{
		if( s.getString(i)[0] == '0' )
			return true;
	}
	return false;
}

template<typename T>
T greatestCommonDivisor( T a, T b )
{
	T r;
	while( (r = a % b) != 0 )
	{
		a = b;
		b = r;
	}
	return b;
}

/**
 * @brief Find the biggest common step from a set of all steps.
 */
std::size_t extractStep( const std::set<std::size_t>& steps )
{
	if( steps.size() == 1 )
		return *steps.begin();

	std::set<std::size_t> allSteps;
	for( std::set<std::size_t>::const_iterator itA = steps.begin(), itB = ++steps.begin(), itEnd = steps.end();
	     itB != itEnd;
	     ++itA, ++itB )
	{
		allSteps.insert( greatestCommonDivisor( *itB, *itA ) );
	}
	return extractStep( allSteps );
}

/**
 * @brief Extract step from a sorted list of time values.
 */
std::size_t extractStep( const std::list<Sequence::Time>& times )
{
	if( times.size() <= 1 )
		return 1;

	std::set<std::size_t> allSteps;

	for( std::list<Sequence::Time>::const_iterator itA = times.begin(), itB = ++times.begin(), itEnd = times.end();
	     itB != itEnd;
	     ++itA, ++itB )
	{
		allSteps.insert( *itB - *itA );
	}
	return extractStep( allSteps );
}

/**
 * @brief Extract step from a sorted list of time values.
 */
std::size_t extractStep( const std::list<FileNumbers>& times, const std::size_t i )
{
	if( times.size() <= 1 )
		return 1;

	std::set<std::size_t> allSteps;

	for( std::list<FileNumbers>::const_iterator itA = times.begin(), itB = ++times.begin(), itEnd = times.end();
	     itB != itEnd;
	     ++itA, ++itB )
	{
		allSteps.insert( itB->getTime( i ) - itA->getTime( i ) );
	}
	return extractStep( allSteps );
}

/// @}

}

Sequence::Sequence( const boost::filesystem::path& directory, const MaskOptions options, const EPattern accept ) : FileObject( directory, eSequence, options )
{
	clear();
	initFromDetection( accept );
}

bool Sequence::isIn( const std::string& filename, Time& time, std::string& timeStr )
{
	std::size_t min = _prefix.size() + _suffix.size();

	if( filename.size() <= min )
		return false;

	if( filename.substr( 0, _prefix.size() ) != _prefix ||
	    filename.substr( filename.size() - _suffix.size(), _suffix.size() ) != _suffix )
		return false;

	try
	{
		timeStr = filename.substr( _prefix.size(), filename.size() - _suffix.size() - _prefix.size() );
		time = boost::lexical_cast<Time>( timeStr );
	}
	catch(... )
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

	if( ( accept & ePatternStandard ) &&
	    regex_match( pattern.c_str(), matches, regexPatternStandard ) )
	{
		std::string paddingStr( matches[2].first, matches[2].second );
		padding       = paddingStr.size();
		strictPadding = ( paddingStr[0] == '#' );
	}
	else if( ( accept & ePatternCStyle ) &&
	         regex_match( pattern.c_str(), matches, regexPatternCStyle ) )
	{
		std::string paddingStr( matches[2].first, matches[2].second );
		padding       = paddingStr.size() == 0 ? 0 : boost::lexical_cast<std::size_t>( paddingStr ); // if no padding value: %d -> padding = 0
		strictPadding = false;
	}
	else if( ( accept & ePatternFrame ) &&
	         regex_match( pattern.c_str(), matches, regexPatternFrame ) )
	{
		std::string frame( matches[2].first, matches[2].second );
		//		Time t = boost::lexical_cast<Time>( frame );
		padding       = frame.size();
		strictPadding = false;
	}
	else
	{
		// this is a file, not a sequence
		return false;
	}

	prefix = std::string( matches[1].first, matches[1].second );
	prefix = prefix.erase ( 0, _directory.string().size()+1 );
	suffix = std::string( matches[3].first, matches[3].second );
	
// 	TUTTLE_COUT( "initFromPattern "<< _directory <<" prefix=" << prefix << " suffix=" << suffix);
	return true;
}

void Sequence::init( const std::string& prefix, const std::size_t padding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step, const bool strictPadding )
{
	clear();
	_prefix        = prefix;
	_padding       = padding;
	_suffix        = suffix;
	_firstTime     = firstTime;
	_lastTime      = lastTime;
	_step          = step;
	_strictPadding = strictPadding;
	_nbFiles       = 0;
}

bool Sequence::init( const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EPattern accept )
{
	clear();
	if( !initFromPattern( pattern, accept, _prefix, _suffix, _padding, _strictPadding ) )
		return false; // not regognize as a pattern, maybe a still file

	_firstTime     = firstTime;
	_lastTime      = lastTime;
	_step          = step;
	_nbFiles       = 0;
	return true;
}

bool Sequence::init( const Time first, const Time last, const Time step, const EPattern accept )
{
	boost::filesystem::path dir = _directory.parent_path();

	if( dir.empty() ) // relative path
		dir = boost::filesystem::current_path();
	_directory = dir;
	return this->init( _directory.filename(), first, last, step, accept );
}

bool Sequence::initFromDetection( const std::string& pattern, const EPattern accept )
{
	clear();

	if( !initFromPattern( pattern, accept, _prefix, _suffix, _padding, _strictPadding ) )
		return false; // not regognize as a pattern, maybe a still file

	if( !exists( _directory ) )
		return true; // an empty sequence

	std::list<std::string> allTimesStr;
	std::list<Time> allTimes;

	fs::directory_iterator itEnd;
	for( fs::directory_iterator iter( _directory ); iter != itEnd; ++iter )
	{
		// we don't make this check, which can take long time on big sequences (>1000 files)
		// depending on your filesystem, we may need to do a stat() for each file
		//		if( fs::is_directory( iter->status() ) )
		//			continue; // skip directories

		Time time;
		std::string timeStr;
		// if the file is inside the sequence
		if( isIn( iter->filename(), time, timeStr ) )
		{
			// create a big list of all times in our sequence
			allTimesStr.push_back( timeStr );
			allTimes.push_back( time );
		}
// 		TUTTLE_COUT("initFrome detection 2 " <<  iter->filename());
	}
// 	TUTTLE_COUT("initFrome detection 2 end for");
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
	_padding = extractPadding( allTimesStr );
	_strictPadding = extractIsStrictPadding( allTimesStr, _padding );

	_firstTime = allTimes.front();
	_lastTime  = allTimes.back();
	_nbFiles   = allTimes.size();

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
std::list<Sequence> buildSequence( const boost::filesystem::path& directory, const FileStrings& id, std::list<FileNumbers>& nums, const MaskOptions& desc )
{
	typedef Sequence::Time Time;
	nums.sort();

	BOOST_ASSERT( nums.size() > 0 );
	// assert all FileNumbers have the same size...
	BOOST_ASSERT( nums.front().size() == nums.back().size() );

	std::size_t len = nums.front().size();
	//	TUTTLE_TCOUT_VAR(len);

	// detect which part is the sequence number
	// for the moment, accept only one sequence
	// but we can easily support multi-sequences
	std::vector<std::size_t> allIds; // list of ids (with 0<id<len) with value changes
	for( std::size_t i = 0; i < len; ++i )
	{
		const Time t = nums.front().getTime( i );
		BOOST_FOREACH( const FileNumbers& sn, nums )
		{
			if( sn.getTime( i ) != t )
			{
				allIds.push_back( i );
				break;
			}
		}
	}
//	unsigned int i = 0;
//	BOOST_FOREACH( const std::list<FileNumbers>::value_type& sn, nums )
//	{
//		TUTTLE_TCOUT( "seq " << i++ << " : " <<  sn);
//	}
	std::size_t idChangeBegin = 0;
	std::size_t idChangeEnd = 0;
	if( allIds.size() == 0 )
	{
		idChangeBegin = idChangeEnd = len-1;
	}
	else
	{
		idChangeBegin = allIds.front();
		idChangeEnd = allIds.back();
	}
	//	TUTTLE_TCOUT_VAR( idNum );

	Sequence seqCommon( directory, desc );
	// fill information in the sequence...
	
	for( std::size_t i = 0; i < idChangeBegin; ++i )
	{
		seqCommon._prefix += id[ i ];
		seqCommon._prefix += nums.front().getString( i );
	}
	seqCommon._prefix += id[ idChangeBegin ];
	for( std::size_t i = idChangeEnd + 1; i < len; ++i )
	{
		seqCommon._suffix += id[ i ];
		seqCommon._suffix += nums.front().getString( i );
	}
	seqCommon._suffix += id[ len ];

	std::list<Sequence> result;

	if( allIds.size() <= 1 )
	{
		// standard case, one sequence detected
		seqCommon._firstTime = nums.front().getTime( idChangeEnd );
		seqCommon._lastTime  = nums.back().getTime( idChangeEnd );
		seqCommon._nbFiles   = nums.size();

		seqCommon._step = extractStep( nums, idChangeEnd );
		seqCommon._padding = extractPadding( nums, idChangeEnd );
		seqCommon._strictPadding = extractIsStrictPadding( nums, idChangeEnd, seqCommon._padding );
		result.push_back(seqCommon);
		return result;
	}

	// it's a multi-sequence...
	
	const FileNumbers* previous = &nums.front();
	Sequence s = seqCommon;
	s._prefix += previous->getString( idChangeBegin );
	for( std::size_t i = idChangeBegin+1; i < idChangeEnd; ++i )
	{
		s._prefix += id[i];
		s._prefix += previous->getString( i );
	}
	s._prefix += id[ idChangeEnd ];
	result.push_back( s );

	std::list<Time> times;
	std::list<std::string> timesStr;
	std::size_t iCurrent = 0;
	std::size_t iBegin = 0;
	BOOST_FOREACH( const FileNumbers& sn, nums )
	{
		if( ! sn.rangeEquals( *previous, idChangeBegin, idChangeEnd ) )
		{
			// update the last added sequence
			result.back()._nbFiles = times.size();
			result.back()._firstTime = times.front();
			result.back()._lastTime = times.back();
			result.back()._step = extractStep(times);
			result.back()._padding = extractPadding(timesStr);
			result.back()._strictPadding = extractIsStrictPadding(timesStr, result.back()._padding);
			times.clear();

			// create a new sequence initilized with "sn" values
			iBegin = iCurrent;
			s = seqCommon;
			s._prefix += sn.getString( idChangeBegin );
			for( std::size_t i = idChangeBegin+1; i < idChangeEnd; ++i )
			{
				s._prefix += id[i];
				s._prefix += sn.getString( i );
			}
			s._prefix += id[ idChangeEnd ];
			s._padding   = sn.getPadding( idChangeEnd );
			result.push_back( s );
			previous = &sn;
		}
		times.push_back( sn.getTime(idChangeEnd) );
		timesStr.push_back( sn.getString(idChangeEnd) );
		++iCurrent;
	}
	// update the last added sequence
	result.back()._nbFiles = times.size();
	result.back()._firstTime = times.front();
	result.back()._lastTime = times.back();
	result.back()._step = extractStep(times);
	result.back()._padding = extractPadding(timesStr);
	result.back()._strictPadding = extractIsStrictPadding(timesStr, result.back()._padding);
	
	return result;
}

bool isNotFilter( std::string filename, std::vector<std::string>& filters)
{
	if(filters.size()==0)
		return true;
	
	for(uint i=0; i<filters.size(); i++)
	{
		std::string filter(filters.at(i));
		filter = boost::regex_replace( filter, boost::regex( "\\*" ), "(.*)"  );
		filter = boost::regex_replace( filter, boost::regex( "\\?" ), "(.)"  );

// 		TUTTLE_COUT(filename << " | " << filter <<" | " << regex_match(filename, boost::regex(filter)));
		if (regex_match(filename, boost::regex(filter)))
			return true;
	}
	return false;
}

std::list<boost::shared_ptr<FileObject>> fileObjectsInDir( const boost::filesystem::path& directory, int mask, const MaskOptions& desc )
{
	std::vector<std::string> filters;
	return fileObjectsInDir( directory, mask, desc, filters );
}

std::list<boost::shared_ptr<FileObject>> fileObjectsInDir( const boost::filesystem::path& directory, int mask, const MaskOptions& desc, std::vector<std::string>& filters )
{
	std::list<boost::shared_ptr<FileObject>> output;
	
	std::list<boost::shared_ptr<FileObject>>	outputDirectories;
	std::list<boost::shared_ptr<FileObject>>	outputFiles;
	std::list<boost::shared_ptr<FileObject>>	outputSequences;
	
	if( !exists( directory ) )
		return output;
	
	// variables for sequence detection
	typedef boost::unordered_map<FileStrings, std::list<FileNumbers>, SeqIdHash> SeqIdMap;
	SeqIdMap	sequences;
	FileStrings	id; // an object uniquely identify a sequence
	FileNumbers	nums; // the list of numbers inside one filename
	
	// for all files in the directory
	fs::directory_iterator itEnd;
	for( fs::directory_iterator iter( directory ); iter != itEnd; ++iter )
	{
		// clear previous infos
		id.clear();
		nums.clear(); // (clear but don't realloc the vector inside)
		
// 		TUTTLE_COUT("dir " << iter->filename());
		
		if( !(iter->filename()[0]==0x2E) || (desc & eDotFile)  ) //0x2e == . for the test if we ask to show hidden files and if it is hidden
		{
// 			TUTTLE_COUT("hidden file " << iter->filename());

			// detect if is a folder
			if( fs::is_directory( iter->status() ) )
			{
//				TUTTLE_COUT("d\t"<< iter->filename());
				boost::shared_ptr<Folder> d( new Folder( directory, iter->filename(), desc ) );
				outputDirectories.push_back( d );
			}
			else // it's a file or a file of a sequence
			{
				if( isNotFilter( iter->filename(), filters) ) // filtering of entries with filters strings
				{
					// if at least one number detected
					if( seqConstruct( iter->filename(), id, nums ) )
					{
						const SeqIdMap::iterator it( sequences.find( id ) );
						if( it != sequences.end() ) // is already in map
						{
							// append the list of numbers
							sequences.at( id ).push_back( nums );
						}
						else
						{
							// create an entry in the map
							std::list<FileNumbers> li;
							li.push_back( nums );
							sequences.insert( SeqIdMap::value_type( id, li ) );
						}
					}
					
					else
					{
// 						TUTTLE_COUT("f\t"<< iter->filename());
						boost::shared_ptr<File> f( new File( directory, iter->filename(), desc ) );
						outputFiles.push_back( f );
					}
				}
			}
		}
	}
	
	
	// add sequences in the output list
	BOOST_FOREACH( SeqIdMap::value_type & p, sequences )
	{
		const std::list<Sequence> ss = buildSequence( directory, p.first, p.second, desc );
		BOOST_FOREACH( const std::list<Sequence>::value_type & s, ss )
		{
			// don't detect sequence of directories
			if( !fs::is_directory( s.getAbsoluteFirstFilename() ) )
			{
				if(s.getNbFiles()==1) // if it's a sequence of 1 file, it isn't a sequence but only a file
				{
				    boost::shared_ptr<File> file( new File( s.getDirectory(), s.getFirstFilename(), s.getMaskOptions() ) );
				    outputFiles.push_back( file );
				}
				else
				{
				    boost::shared_ptr<Sequence> seq( new Sequence( s ) );
				    outputSequences.push_back( seq );
				}
			}
		}
	}

	if(mask & eDirectory)
	{
		output.merge( outputDirectories );
	}
	// add files in the output list
	if(mask & eFile)
	{
		output.merge( outputFiles );
	}
	// add sequences in the output list
	if(mask & eSequence)
	{
		output.merge( outputSequences );
	}
// 	TUTTLE_COUT("find "<< output.size() <<" elements");
	return output;
}

// not work actually ....
std::ostream& Folder::getProperties( std::ostream& os, const boost::filesystem::path& directory)
{
	os << "d " << boost::filesystem::file_size( directory ) ;
	return os;
}

std::ostream& operator<<( std::ostream& os, const FileObject& fo )
{
	fo.getCout(os);
	return os;
}

std::ostream& Folder::getCout( std::ostream& os ) const
{
	os << std::left;
	if( showProperties() )
	{
		os <<std::setw(PROPERTIES_WIDTH) << "d ";
	}
	if( showPath() )
	{
		std::string path = (_directory / _folderName).string();
		os << std::setw(NAME_WIDTH_WITH_DIR) << FOLDER_COLOR + path + STD_COLOR;
	}
	else
	{
		os << std::setw(NAME_WIDTH) << FOLDER_COLOR + _folderName + STD_COLOR ;
	}
	return os;
}

std::ostream& File::getCout( std::ostream& os ) const
{
      os << std::left;
      if( showProperties() )
      {
	      os <<std::setw(PROPERTIES_WIDTH) << "f ";// + boost::filesystem::file_size( _directory/_filename );
      }
      if( showPath() )
      {
	      std::string path = (_directory / _filename).string();
	      os << std::setw(NAME_WIDTH_WITH_DIR) << FILE_COLOR + path  + STD_COLOR;
      }
      else
      {
	      os << std::setw(NAME_WIDTH) << FILE_COLOR + _filename + STD_COLOR ;
      }
      return os;
}

std::ostream& Sequence::getCout( std::ostream& os ) const
{
	os << std::left;
	if( showProperties() )
	{
		os <<std::setw(PROPERTIES_WIDTH) << "s";
	}
	if( showPath() )
	{
		std::string path = (_directory / getStandardPattern()).string();
		os << std::setw(NAME_WIDTH_WITH_DIR) << SEQUENCE_COLOR + path + STD_COLOR ;
	}
	else
	{
		os << std::setw(NAME_WIDTH) << SEQUENCE_COLOR + getStandardPattern() + STD_COLOR ;
	}
	os << " [" << getFirstTime() << ":" << getLastTime();
	if( getStep() != 1 )
		os << "x" << getStep();
	os << "] " << getNbFiles() << " file" << ( ( getNbFiles() > 1 ) ? "s" : "" );
	if( hasMissingFile() )
		os << ", "  << MISSING_FILE_IN_SEQUENCE_COLOR<< getNbMissingFiles() << " missing file" << ( ( getNbMissingFiles() > 1 ) ? "s" : "" ) << STD_COLOR;
	return os;
}

}
}