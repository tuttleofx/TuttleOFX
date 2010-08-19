#ifndef _FILENAMEMANAGER_HPP_
#define	_FILENAMEMANAGER_HPP_

#include <tuttle/common/utils/global.hpp>

#include <ofxCore.h>

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

#include <cmath>
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstddef>

namespace tuttle {
namespace common {

/**
 * @brief A sequence of numeroted files.
 */
class Sequence
{
public:
	typedef Sequence This;
	typedef ssize_t Time;
public:
	/**
	 * List all recognized pattern types.
	 */
	enum EPattern
	{
		ePatternNone     = 0,
		ePatternStandard = 1,
		ePatternCStyle   = ePatternStandard*2,
		ePatternFrame    = ePatternCStyle*2,

		ePatternDefault  = ePatternCStyle+ePatternStandard,
		ePatternAll      = ePatternFrame+ePatternCStyle+ePatternStandard
	};
public:
	Sequence();
	/**
	 * @param[in] file: a sequence identifier (eg. "/custom/dir/myImages.####.jpg")
	 * @param[in] accept: types of recognized patterns
	 */
	Sequence( const boost::filesystem::path& seqPath, const EPattern& accept = ePatternDefault );
	Sequence( const Sequence& v ) { operator=(v); }
	virtual ~Sequence();

	/**
	 * @brief Partial initialization, using only pattern informations.
	 * @warning You don't have all informations like range, directory, etc.
	 */
	bool initFromPattern( const std::string& pattern, const EPattern& accept = ePatternDefault );

	/**
	 * @brief Init from directory and pattern.
	 */
	bool init( const boost::filesystem::path& directory, const std::string& pattern, const EPattern& accept = ePatternDefault );
	/**
	 * @brief Init from full pattern.
	 */
	inline bool init( const boost::filesystem::path& seqPath, const EPattern& accept = ePatternDefault );
	
	inline boost::filesystem::path getDirectory() const { return _directory; }
	inline void setDirectory( const boost::filesystem::path& p ) { _directory = p; }
	inline std::string getAbsoluteFilenameAt( const Time time ) const;
	inline std::string getFilenameAtA( const Time time ) const;
	inline std::string getAbsoluteFirstFilename() const { return getFilenameAtA( getFirstTime() ); }
	inline std::string getAbsoluteLastFilename() const { return getFilenameAtA( getLastTime() ); }

	inline std::pair<Time, Time> getRange() const { return std::pair<Time, Time>( getFirstTime(), getLastTime() ); }
	inline std::size_t getStep() const { return _step; }
	inline Time getFirstTime() const { return _firstTime; }
	inline Time getLastTime() const { return _lastTime; }
	inline std::size_t getPadding() const { return _padding; }
	inline bool isStrictPadding() const { return _strictPadding; }
	inline bool hasMissingFrames() const { return (_nbFrames / _step) != static_cast<std::size_t>(getLastTime()-getFirstTime()); }
	/// @brief filename without frame number
	inline std::string getIdentification() const { return _prefix + _suffix; }
	inline std::string getPrefix() const { return _prefix; }
	inline std::string getSuffix() const { return _suffix; }

	/**
	 * @brief Check if the filename is inside the sequence and return it's time value.
	 * @param[out] time: the time extract from the filename (only if contained in the sequence)
	 * @return if the filename is contained inside the sequence
	 */
	bool isIn( const std::string& filename, Time& time );

	static bool isASequenceFilename( const std::string& filename );
	static EPattern checkPattern( const std::string& pattern );


protected:
	inline void clear();
	static std::size_t extractStep( const std::list<Time>& times );

private:
	friend std::ostream& operator<<( std::ostream& os, const This& v );
	friend std::vector<Sequence> sequencesInDir( const boost::filesystem::path& directory );

protected:
	boost::filesystem::path _directory;

	std::string _prefix;         ///< filename prefix
	std::string _suffix;         ///< filename suffix

	bool _strictPadding;         ///< 
	std::size_t _padding;        ///< padding, no padding if 0, fixed padding otherwise
	std::size_t _step;           ///< step
	Time _firstTime;             ///< begin time
	Time _lastTime;              ///< end time
	std::size_t _nbFrames;       ///< number of frames

	static const char _fillCar = '0'; ///< Filling character
};


std::vector<Sequence> sequencesInDir( const boost::filesystem::path& directory );
std::vector<Sequence> sequencesInDir( const boost::filesystem::path& directory, const boost::regex& filter );


inline std::string Sequence::getFilenameAtA( const Time time ) const
{
	std::ostringstream o;
	o << _prefix << std::setw(_padding) << std::setfill(_fillCar) << time << _suffix;
	return o.str();
}

inline std::string Sequence::getAbsoluteFilenameAt( const Time time ) const
{
	return (_directory / getFilenameAtA( time )).file_string();
}

inline bool Sequence::init( const boost::filesystem::path& seqPath, const EPattern& accept )
{
	boost::filesystem::path dir = seqPath.parent_path();
	if( dir.empty() ) // relative path
		dir = boost::filesystem::current_path();
	
	return this->init( dir, seqPath.filename() );
}

inline void Sequence::clear()
{
	_directory.clear();
	_prefix.clear();
	_suffix.clear();
	_padding = 0;
	_step = 1;
	_firstTime = 0;
	_lastTime = 0;
	_nbFrames = 0;
}



}
}

#endif
