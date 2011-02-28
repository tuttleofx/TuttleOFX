#ifndef _FILENAMEMANAGER_HPP_
#define _FILENAMEMANAGER_HPP_

#include <tuttle/common/utils/global.hpp>

#include <ofxCore.h>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <list>
#include <cstddef>

namespace tuttle {
namespace common {

namespace {
class FileStrings;
class FileNumbers;
}

/**
 * @brief A sequence of numeroted files.
 */
class Sequence
{
public:
	typedef Sequence This;
	typedef std::ssize_t Time;

public:
	/**
	 * List all recognized pattern types.
	 */
	enum EPattern
	{
		ePatternNone     = 0,
		ePatternStandard = 1,
		ePatternCStyle   = ePatternStandard * 2,
		ePatternFrame    = ePatternCStyle * 2,

		ePatternDefault  = ePatternCStyle + ePatternStandard,
		ePatternAll      = ePatternFrame + ePatternCStyle + ePatternStandard
	};

public:
	Sequence();
	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 */
	Sequence( const boost::filesystem::path& directory, const std::string& prefix, const std::size_t padding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step = 1, const bool strictPadding = false )
	{
		init( directory, prefix, padding, suffix, firstTime, lastTime, step, strictPadding );
	}

	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 */
	Sequence( const boost::filesystem::path& directory, const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EPattern accept = ePatternDefault )
	{
		init( directory, pattern, firstTime, lastTime, step, accept );
	}

	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 */
	Sequence( const boost::filesystem::path& seqPath, const Time firstTime, const Time lastTime, const Time step, const EPattern accept = ePatternDefault )
	{
		init( seqPath, firstTime, lastTime, step, accept );
	}

	/**
	 * @brief Construct a sequence from a pattern and detect range, nbFrames on your filesystem.
	 * @param[in] file: a sequence identifier (eg. "/custom/dir/myImages.####.jpg")
	 * @param[in] accept: types of recognized patterns
	 * @warning search on your filesystem, to detect the range.
	 */
	Sequence( const boost::filesystem::path& seqPath, const EPattern accept = ePatternDefault );

	Sequence( const Sequence& v ) { operator=( v ); }
	virtual ~Sequence();

	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 */
	void init( const boost::filesystem::path& directory, const std::string& prefix, const std::size_t padding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step = 1, const bool strictPadding = false );

	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 */
	bool init( const boost::filesystem::path& directory, const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EPattern accept = ePatternDefault );
	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 */
	bool init( const boost::filesystem::path& seqPath, const Time firstTime, const Time lastTime, const Time step, const EPattern accept = ePatternDefault );

	/**
	 * @brief Init from directory and pattern.
	 * @warning search on your filesystem, to detect the range.
	 */
	bool initFromDetection( const boost::filesystem::path& directory, const std::string& pattern, const EPattern accept = ePatternDefault );

	/**
	 * @brief Init from full pattern.
	 * @warning search on your filesystem, to detect the range.
	 */
	inline bool initFromDetection( const boost::filesystem::path& seqPath, const EPattern& accept = ePatternDefault );

	inline boost::filesystem::path getDirectory() const                             { return _directory; }
	inline void                    setDirectory( const boost::filesystem::path& p ) { _directory = p; }
	inline std::string             getAbsoluteFilenameAt( const Time time ) const;
	inline std::string             getFilenameAt( const Time time ) const;
	inline std::string             getAbsoluteFirstFilename() const { return ( _directory / getFilenameAt( getFirstTime() ) ).string(); }
	inline std::string             getAbsoluteLastFilename() const  { return ( _directory / getFilenameAt( getLastTime() ) ).string(); }

	/// @return pattern character in standard style
	inline char getPatternCharacter() const { return getPadding() ? '#' : '@'; }
	/// @return a string pattern using standard style
	inline std::string getStandardPattern() const { return getPrefix() + std::string( getPadding() ? getPadding() : 1, getPatternCharacter() ) + getSuffix(); }
	inline std::string getAbsoluteStandardPattern() const { return (getDirectory() / getStandardPattern()).string(); }
	/// @return a string pattern using C Style
	inline std::string getCStylePattern() const
	{
		if( getPadding() )
			return getPrefix() + "%0" + boost::lexical_cast<std::string>( getPadding() ) + "d" + getSuffix();
		else
			return getPrefix() + "%d" + getSuffix();
	}
	inline std::string getAbsoluteCStylePattern() const { return (getDirectory() / getCStylePattern()).string(); }

	inline std::pair<Time, Time> getRange() const          { return std::pair<Time, Time>( getFirstTime(), getLastTime() ); }
	inline std::size_t           getStep() const           { return _step; }
	inline Time                  getFirstTime() const      { return _firstTime; }
	inline Time                  getLastTime() const       { return _lastTime; }
	inline std::size_t           getDuration() const       { return getLastTime() - getFirstTime() + 1; }
	inline Time                  getNbFiles() const        { return _nbFiles; }
	inline std::size_t           getPadding() const        { return _padding; }
	inline bool                  isStrictPadding() const   { return _strictPadding; }
	inline bool                  hasMissingFile() const    { return getNbMissingFiles() != 0; }
	inline std::size_t           getNbMissingFiles() const
	{
		if( !getStep() )
			return 0;
		return ( ( ( getLastTime() - getFirstTime() ) / getStep() ) + 1 ) - getNbFiles();
	}
	/// @brief filename without frame number
	inline std::string getIdentification() const { return _prefix + _suffix; }
	inline std::string getPrefix() const         { return _prefix; }
	inline std::string getSuffix() const         { return _suffix; }

	/**
	 * @brief Check if the filename is inside the sequence and return it's time value.
	 * @param[out] time: the time extract from the filename (only if contained in the sequence)
	 * @return if the filename is contained inside the sequence
	 */
	bool isIn( const std::string& filename, Time& time, std::string& timeStr );

	static EPattern checkPattern( const std::string& pattern );

	bool operator<( const This& other ) const
	{
		return getAbsoluteStandardPattern() < other.getAbsoluteStandardPattern();
	}

protected:
	inline void clear();
	/**
	 * @brief Partial initialization, using only pattern informations.
	 * @warning You don't have all informations like range, directory, etc.
	 */
	bool initFromPattern( const std::string& pattern, const EPattern& accept, std::string& prefix, std::string& suffix, std::size_t& padding, bool& strictPadding );

private:
	friend std::ostream& operator<<( std::ostream& os, const This& v );
	friend std::list<Sequence> buildSequence(  const boost::filesystem::path& directory, const FileStrings& id, std::list<FileNumbers>& nums );
	friend std::vector<Sequence> sequencesInDir( const boost::filesystem::path& directory );

protected:
	boost::filesystem::path _directory;

	std::string _prefix;         ///< filename prefix
	std::string _suffix;         ///< filename suffix

	bool       _strictPadding;  ///<
	std::size_t _padding;        ///< padding, no padding if 0, fixed padding otherwise
	std::size_t _step;           ///< step
	Time        _firstTime;      ///< begin time
	Time        _lastTime;       ///< end time
	std::size_t _nbFiles;        ///< number of frames

	static const char _fillCar = '0'; ///< Filling character
};

/**
 * @brief Search all sequences in a directory.
 */
std::vector<Sequence> sequencesInDir( const boost::filesystem::path& directory );
/**
 * @brief Search all sequences in a directory, but filter all files before with a regex.
 * @todo TODO !
 */
std::vector<Sequence> sequencesInDir( const boost::filesystem::path& directory, const boost::regex& filter );

inline std::string Sequence::getFilenameAt( const Time time ) const
{
	std::ostringstream o;

	o << _prefix << std::setw( _padding ) << std::setfill( _fillCar ) << time << _suffix;
	return o.str();
}

inline std::string Sequence::getAbsoluteFilenameAt( const Time time ) const
{
	return ( _directory / getFilenameAt( time ) ).string();
}

inline bool Sequence::initFromDetection( const boost::filesystem::path& seqPath, const EPattern& accept )
{
	boost::filesystem::path dir = seqPath.parent_path();

	if( dir.empty() ) // relative path
		dir = boost::filesystem::current_path();

	return this->initFromDetection( dir, seqPath.filename().string(), accept );
}

inline void Sequence::clear()
{
	_directory.clear();
	_prefix.clear();
	_suffix.clear();
	_padding   = 0;
	_step      = 1;
	_firstTime = 0;
	_lastTime  = 0;
	_nbFiles   = 0;
}

}
}

#endif
