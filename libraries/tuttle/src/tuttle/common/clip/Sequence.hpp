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
	Sequence();
	/**
	 * @param[in] file: a real file name from the sequence (eg. "/custom/dir/myImages.0010.jpg"),
	 *                  or a sequence identifier (eg. "/custom/dir/myImages.####.jpg")
	 */
	Sequence( const boost::filesystem::path& seqPath );
	Sequence( const Sequence& v ) { operator=(v); }
	virtual ~Sequence();

	void init( const boost::filesystem::path& directory, const std::string& pattern );
	inline void init( const boost::filesystem::path& seqPath );
	
	inline boost::filesystem::path getDirectory() const { return _directory; }
	inline boost::filesystem::path getAbsoluteFilePathAt( const Time time ) const;
	inline std::string getFilenameAt( const Time time ) const;
	inline boost::filesystem::path getFirstFilename() const { return getFilenameAt( getFirstTime() ); }
	inline boost::filesystem::path getLastFilename() const { return getFilenameAt( getLastTime() ); }

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
	 * @brief Check if filename is inside the sequence and return it's time value.
	 * @param[out] time: the time extract from the filename (only if contained in the sequence)
	 * @return if the filename is contained inside the sequence
	 */
	bool isInSequence( const std::string& filename, Time& time );

protected:
	inline void clear();
	void initFromPattern( const std::string& pattern );
	static std::size_t extractStep( const std::list<Time>& times );

private:
	friend std::ostream& operator<<( std::ostream& os, const This& v );
	friend std::vector<Sequence> sequencesInDir( const boost::filesystem::path& directory );

protected:
	boost::filesystem::path _directory;

	std::string _prefix;         ///< filename prefix
	std::string _suffix;        ///< filename suffix

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


inline std::string Sequence::getFilenameAt( const Time time ) const
{
	std::ostringstream o;
	o << _prefix << std::setw(_padding) << std::setfill(_fillCar) << time << _suffix;
	return o.str();
}

inline boost::filesystem::path Sequence::getAbsoluteFilePathAt( const Time time ) const
{
	return _directory / getFilenameAt( time );
}

inline void Sequence::init( const boost::filesystem::path& seqPath )
{
	boost::filesystem::path dir = seqPath.parent_path();
	if( dir.empty() ) // relative path
		dir = boost::filesystem::current_path();
	this->init( dir, seqPath.filename() );
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
