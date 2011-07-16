#ifndef _FILENAMEMANAGER_HPP_
#define _FILENAMEMANAGER_HPP_

#include <tuttle/common/utils/global.hpp>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <list>
#include <cstddef>

#define PROPERTIES_WIDTH 3
#define NAME_WIDTH 50
#define NAME_WIDTH_WITH_DIR 80

#ifdef __LINUX__
static const std::string kColorStd	( "\E[0;0m"  );
static const std::string kColorFolder	( "\E[1;34m" );
static const std::string kColorFile	( "\E[0;32m" );
static const std::string kColorSequence	( "\E[0;32m" );
static const std::string kColorError	( "\E[0;31m" );
#else
static const std::string kColorStd	( "" );
static const std::string kColorFolder	( "" );
static const std::string kColorFile	( "" );
static const std::string kColorSequence	( "" );
static const std::string kColorError	( "" );
#endif

namespace tuttle {
namespace common {

namespace {
class FileStrings;
class FileNumbers;
}

/**
  * List all recognized pattern types.
  */
enum EMaskType
{
	eMaskTypeUndefined	= 0,			// 0
	eMaskTypeDirectory	= 1,			// 1<<0
	eMaskTypeFile		= eMaskTypeDirectory*2,		// 1<<1
	eMaskTypeSequence	= eMaskTypeFile*2,		// 1<<2
	
	eMaskTypeDefault = eMaskTypeSequence
};

enum EMaskOptions
{
	eMaskOptionsNone		= 0,			// 0
	eMaskOptionsProperties	= 1,			// show type of FileObject
	eMaskOptionsPath		= eMaskOptionsProperties*2,	// show path of FileObject
	eMaskOptionsAbsolutePath	= eMaskOptionsPath*2,	// show absolute path of FileObject
	eMaskOptionsDotFile	= eMaskOptionsAbsolutePath*2,	// show files which start with a dot (hidden files)
	eMaskOptionsColor		= eMaskOptionsDotFile*2,		// output with color
	
	eMaskOptionsDefault = ( eMaskOptionsPath | eMaskOptionsColor )
};

inline EMaskType operator~(const EMaskType& a)
{
  EMaskType b = (EMaskType) ( ~ int(a));
  return b;
}

inline EMaskType operator&=(EMaskType& a, const EMaskType& b)
{
  a = (EMaskType) (int(b) & int(a));
  return a;
}

inline EMaskType operator|=(EMaskType& a, const EMaskType& b)
{
  a = (EMaskType) (int(b) | int(a));
  return a;
}

inline EMaskOptions operator|=(EMaskOptions& a, const EMaskOptions& b)
{
  a = (EMaskOptions) (int(b) | int(a));
  return a;
}

inline EMaskOptions remove(EMaskOptions& a, const EMaskOptions& b)
{
  a = (EMaskOptions) (int(~b) & int(a));
  return a;
}


/**
 * @brief A container for files, directories and sequences.
 */
class FileObject
{
public:
	FileObject( )
	{
	  	_directory.clear();
		_type		= eMaskTypeUndefined;
		_options	= eMaskOptionsNone;
	}
	FileObject( const EMaskOptions options )
	{
		_directory.clear();
		_type		= eMaskTypeUndefined;
		_options	= options;
	}
	/**
	 * @brief Construct a FileObject with given informations.
	 */
	FileObject( const boost::filesystem::path& directory, const EMaskType& type, const EMaskOptions& options )
	{
		init( directory, type, options );
	}
	virtual ~FileObject(){};
	
	friend  std::ostream&				operator<<( std::ostream& os, const FileObject& fo );
	virtual std::ostream&				getCout   ( std::ostream& os ) const = 0;

	/**
	 * @todo: can we remove this?
	 */
	virtual std::vector<boost::filesystem::path>	getFiles() const =0;
	
	/// @todo 
	// virtual std::string getName() const = 0;
//	boost::filesystem::path getAbsoluteName() const
//	{
//		return getDirectory() / getName();
//	}
	
	inline boost::filesystem::path			getDirectory	() const				{ return _directory; }
	inline void					setDirectory	( const boost::filesystem::path& p )	{ _directory = p; }
	
	EMaskOptions					getMaskOptions	() const				{ return _options; }
	EMaskType					getMaskType	() const				{ return _type; }

private:
	void init( const boost::filesystem::path& directory, const EMaskType& type, const EMaskOptions& options )
	{
		_directory	= directory;
		_type		= type;
		_options	= options;
	}
	
protected:
	
	inline bool showProperties()	const	{ return _options & eMaskOptionsProperties; }
	inline bool showPath()		const	{ return _options & eMaskOptionsPath; }
	
	
	boost::filesystem::path	_directory;		///< directory

	EMaskType		_type;			///< specify type of object
	EMaskOptions		_options;		///< specify output options of object, common for each objects
};

std::list<boost::shared_ptr<FileObject> > fileObjectsInDir( const boost::filesystem::path& directory, const EMaskType mask = eMaskTypeDefault, const EMaskOptions desc = eMaskOptionsDefault );
std::list<boost::shared_ptr<FileObject> > fileObjectsInDir( const boost::filesystem::path& directory, std::vector<std::string>& filters, const EMaskType mask = eMaskTypeDefault, const EMaskOptions desc = eMaskOptionsDefault );


class Folder : public FileObject
{
public:
	Folder( const boost::filesystem::path& directory, const std::string folderName, const EMaskOptions& options)
		: FileObject( directory, eMaskTypeDirectory, options)
	{
		_folderName = folderName;
	};
	virtual ~Folder(){};
	
	std::ostream&				getCout( std::ostream& os ) const;
	
	std::vector<boost::filesystem::path>	getFiles() const;
private:
	std::ostream&		getProperties( std::ostream& os, const boost::filesystem::path& directory);
	
	std::string 		_folderName;
};

class File : public FileObject
{
public:
	
	File( const boost::filesystem::path& directory, const std::string& filename, const EMaskOptions& options)
		: FileObject( directory, eMaskTypeFile, options )
	{
		_filename = filename;
	};
	virtual ~File(){};
	
	std::ostream&				getCout( std::ostream& os ) const ;
	std::vector<boost::filesystem::path>	getFiles() const;
	inline std::string			getAbsoluteFilename	()			const { return ( _directory / _filename ).string(); }
private:
	std::string		_filename;
};

/**
 * @brief A sequence of numbered files.
 */
class Sequence : public FileObject
{
public:
	typedef std::ssize_t Time;
	
	/**
	 * List all recognized pattern types.
	 */
	enum EPattern
	{
		ePatternNone		= 0,
		ePatternStandard	= 1,
		ePatternCStyle		= ePatternStandard * 2,
		ePatternFrame		= ePatternCStyle   * 2,

		ePatternDefault		= ePatternCStyle + ePatternStandard,
		ePatternAll		= ePatternFrame  + ePatternCStyle + ePatternStandard
	};
	
	Sequence() : FileObject()
	{
		_prefix.clear();
		_suffix.clear();
		_padding	= 0;
		_step		= 1;
		_firstTime	= 0;
		_lastTime	= 0;
		_nbFiles	= 0;
	}

	Sequence( const boost::filesystem::path& directory, const std::string& prefix, const std::size_t padding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step = 1, const EMaskOptions options = eMaskOptionsDefault, const bool strictPadding = false )
		: FileObject( directory, eMaskTypeSequence, options )
	{
		init( prefix, padding, suffix, firstTime, lastTime, step, strictPadding );
	}
	
	Sequence( const boost::filesystem::path& directory, const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EMaskOptions options = eMaskOptionsDefault, const EPattern accept = ePatternDefault )
		: FileObject( directory, eMaskTypeSequence, options )
	{
		init( pattern, firstTime, lastTime, step, accept );
	}
	
	Sequence( const boost::filesystem::path& directory, const Time firstTime, const Time lastTime, const Time step, const EMaskOptions options = eMaskOptionsDefault, const EPattern accept = ePatternDefault )
		: FileObject( directory, eMaskTypeSequence, options )
	{
		init( firstTime, lastTime, step, accept );
	}

	/**
	 * @todo check if we put a pattern with full path: /home/foo/images/foo.####.jpg
	 */
	Sequence( const boost::filesystem::path& directory, const EMaskOptions options = eMaskOptionsDefault, const EPattern accept = ePatternDefault );
	
	Sequence( const Sequence& v )
		: FileObject( v._options )
	{
		operator=( v );
	}
	
	virtual ~Sequence()
        {}
	
	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 */
	void init( const std::string& prefix, const std::size_t padding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step = 1, const bool strictPadding = false );

	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 */
	bool init( const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EPattern accept = ePatternDefault );
	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 */
	bool init( const Time firstTime, const Time lastTime, const Time step, const EPattern accept = ePatternDefault );
	
	/**
	 * @brief Init from directory and pattern.
	 * @warning search on your filesystem, to detect the range.
	 */
	bool initFromDetection( const std::string& pattern, const EPattern accept = ePatternDefault );

	/**
	 * @brief Init from full pattern.
	 * @warning search on your filesystem, to detect the range.
	 */
	inline bool initFromDetection( const EPattern& accept = ePatternDefault );
	
	inline std::string		getAbsoluteFilenameAt		( const Time time )	const;
	inline std::string		getFilenameAt			( const Time time )	const;
	inline std::string		getFirstFilename		()			const { return getFilenameAt( getFirstTime() ); }
	inline std::string		getAbsoluteFirstFilename	()			const { return ( _directory / getFilenameAt( getFirstTime() ) ).string(); }
	inline std::string		getAbsoluteLastFilename		()			const { return ( _directory / getFilenameAt( getLastTime()  ) ).string(); }

	/// @return pattern character in standard style
	inline char			getPatternCharacter		()			const { return getPadding() ? '#' : '@'; }
	/// @return a string pattern using standard style
	inline std::string		getStandardPattern		()			const { return getPrefix() + std::string( getPadding() ? getPadding() : 1, getPatternCharacter() ) + getSuffix(); }
	inline std::string		getAbsoluteStandardPattern	()			const { return (getDirectory() / getStandardPattern()).string(); }
	/// @return a string pattern using C Style
	inline std::string		getCStylePattern		()			const
	{
		if( getPadding() )
			return getPrefix() + "%0" + boost::lexical_cast<std::string>( getPadding() ) + "d" + getSuffix();
		else
			return getPrefix() + "%d" + getSuffix();
	}

	inline std::string		getAbsoluteCStylePattern	()			const { return (getDirectory() / getCStylePattern()).string(); }

	inline std::pair<Time, Time>	getRange			()			const { return std::pair<Time, Time>( getFirstTime(), getLastTime() ); }
	inline std::size_t		getStep				()			const { return _step; }
	inline Time			getFirstTime			()			const { return _firstTime; }
	inline Time			getLastTime			()			const { return _lastTime; }
	inline std::size_t		getDuration			()			const { return getLastTime() - getFirstTime() + 1; }
	inline Time			getNbFiles			()			const { return _nbFiles; }
	inline std::size_t		getPadding			()			const { return _padding; }
	inline bool			isStrictPadding			()			const { return _strictPadding; }
	inline bool			hasMissingFile			()			const { return getNbMissingFiles() != 0; }
	inline std::size_t		getNbMissingFiles		()			const
	{
		if( !getStep() )
			return 0;
		return ( ( ( getLastTime() - getFirstTime() ) / getStep() ) + 1 ) - getNbFiles();
	}
	/// @brief filename without frame number
	inline std::string		getIdentification		()			const { return _prefix + _suffix; }
	inline std::string		getPrefix			()			const { return _prefix; }
	inline std::string		getSuffix			()			const { return _suffix; }

	/**
	 * @brief Check if the filename is inside the sequence and return it's time value.
	 * @param[out] time: the time extract from the filename (only if contained in the sequence)
	 * @return if the filename is contained inside the sequence
	 */
	bool isIn( const std::string& filename, Time& time, std::string& timeStr );

	static EPattern checkPattern( const std::string& pattern );

	bool operator<( const Sequence& other ) const
	{
		return getAbsoluteStandardPattern() < other.getAbsoluteStandardPattern();
	}

protected:

	/**
	 * @brief Partial initialization, using only pattern informations.
	 * @warning You don't have all informations like range, directory, etc.
	 */
	bool initFromPattern( const std::string& pattern, const EPattern& accept, std::string& prefix, std::string& suffix, std::size_t& padding, bool& strictPadding );

private:
	friend std::list<Sequence> buildSequence(  const boost::filesystem::path& directory, const FileStrings& id, std::list<FileNumbers>& nums, const EMaskOptions& desc );
	
	std::ostream&				getCout( std::ostream& os ) const ;
	std::vector<boost::filesystem::path>	getFiles() const;
protected:
	inline void clear()
	{
		_prefix.clear();
		_suffix.clear();
		_padding	= 0;
		_step		= 1;
		_firstTime	= 0;
		_lastTime	= 0;
		_nbFiles	= 0;
	}
	
	
	std::string _prefix;			///< filename prefix
	std::string _suffix;			///< filename suffix

	bool		_strictPadding;		///<
	std::size_t	_padding;		///< padding, no padding if 0, fixed padding otherwise
	std::size_t	_step;			///< step
	Time		_firstTime;		///< begin time
	Time		_lastTime;		///< end time
	std::size_t	_nbFiles;		///< number of frames

	static const char _fillCar = '0'; ///< Filling character
};


inline std::string Sequence::getFilenameAt( const Time time ) const
{
	std::ostringstream o;

	if( time >= 0 )
	{
		// "prefix.0001.jpg"
		o << _prefix << std::setw( _padding ) << std::setfill( _fillCar ) << time << _suffix;
	}
	else
	{
		// "prefix.-0001.jpg" (and not "prefix.000-1.jpg")
		o << _prefix << "-" << std::setw( _padding ) << std::setfill( _fillCar ) << std::abs(time) << _suffix;
	}
	return o.str();
}

inline std::string Sequence::getAbsoluteFilenameAt( const Time time ) const
{
	return ( _directory / getFilenameAt( time ) ).string();
}

inline bool Sequence::initFromDetection( const EPattern& accept )
{
	boost::filesystem::path dir = _directory.parent_path();

	if( dir.empty() ) // relative path
		dir = boost::filesystem::current_path();

	return this->initFromDetection( _directory.filename().string(), accept );
}


}
}

#endif
