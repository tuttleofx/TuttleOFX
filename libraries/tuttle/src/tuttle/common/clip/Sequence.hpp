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

static const std::string kColorStd	( "\E[0;0m"  );
static const std::string kColorFolder	( "\E[1;34m" );
static const std::string kColorFile	( "\E[0;32m" );
static const std::string kColorSequence	( "\E[0;32m" );
static const std::string kColorError	( "\E[0;31m" );


namespace tuttle {
namespace common {

namespace {
class FileStrings;
class FileNumbers;
}

/**
  * List all recognized pattern types.
  */
enum MaskType
{
	eUndefined	= 0,			// 0
	eDirectory	= 1,			// 1<<0
	eFile		= eDirectory*2,		// 1<<1
	eSequence	= eFile*2		// 1<<2
};

enum MaskOptions
{
	eNone		= 0,			// 0
	eProperties	= 1,			// show type of FileObject
	ePath		= eProperties*2,	// show path of FileObject
        eAbsolutePath	= eProperties*2,	// show absolute path of FileObject
        eDotFile	= eAbsolutePath*2,	// show files which start with a dot (hidden files)
	eColor		= eDotFile*2		// output with color
};

inline MaskType operator~(const MaskType& a)
{
  MaskType b = (MaskType) ( ~ int(a));
  return b;
}

inline MaskType operator&=(MaskType& a, const MaskType& b)
{
  a = (MaskType) (int(b) & int(a));
  return a;
}

inline MaskType operator|=(MaskType& a, const MaskType& b)
{
  a = (MaskType) (int(b) | int(a));
  return a;
}

inline MaskOptions operator|=(MaskOptions& a, const MaskOptions& b)
{
  a = (MaskOptions) (int(b) | int(a));
  return a;
}

inline MaskOptions remove(MaskOptions& a, const MaskOptions& b)
{
  a = (MaskOptions) (int(~b) & int(a));
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
		_type		= eUndefined;
		_options	= eNone;
	}
	FileObject( const MaskOptions options )
	{
		_directory.clear();
		_type		= eUndefined;
		_options	= options;
	}
	/**
	 * @brief Construct a FileObject with given informations.
	 */
	FileObject( const boost::filesystem::path& directory, const MaskType& type, const MaskOptions& options )
	{
		init( directory, type, options );
	}
	virtual ~FileObject(){};
	
	friend  std::ostream&				operator<<( std::ostream& os, const FileObject& fo );
	virtual std::ostream&				getCout   ( std::ostream& os ) const = 0;
	
	virtual std::vector<boost::filesystem::path>	getFiles() const =0;
	
	inline boost::filesystem::path			getDirectory	() const				{ return _directory; }
	inline void					setDirectory	( const boost::filesystem::path& p )	{ _directory = p; }
	
	MaskOptions					getMaskOptions	() const				{ return _options; }
	MaskType					getMaskType	() const				{ return _type; }

private:
	void init( const boost::filesystem::path& directory, const MaskType& type, const MaskOptions& options )
	{
		_directory	= directory;
		_type		= type;
		_options	= options;
	}
	
protected:
	
	inline bool showProperties()	const	{ return _options & eProperties; }
	inline bool showPath()		const	{ return _options & ePath; }
	
	
	boost::filesystem::path	_directory;		///< directory

	MaskType		_type;			///< specify type of object
	MaskOptions		_options;		///< specify output options of object, common for each objects
};

std::list<boost::shared_ptr<FileObject> > fileObjectsInDir( const boost::filesystem::path& directory, int mask, const MaskOptions& desc );
std::list<boost::shared_ptr<FileObject> > fileObjectsInDir( const boost::filesystem::path& directory, int mask, const MaskOptions& desc, std::vector<std::string>& filters );


class Folder : public FileObject
{
public:
	Folder( const boost::filesystem::path& directory, const std::string folderName, const MaskOptions& options) : FileObject( directory, eDirectory, options)
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
	
	File( const boost::filesystem::path& directory, const std::string& filename, const MaskOptions& options) : FileObject( directory, eFile, options )
	{
		_filename = filename;
	};
	virtual ~File(){};
	
	std::ostream&				getCout( std::ostream& os ) const ;
	std::vector<boost::filesystem::path>	getFiles() const;
private:
	std::string		_filename;
};

/**
 * @brief A sequence of numeroted files.
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

	// constructors and desctructors
	Sequence( const boost::filesystem::path& directory, const std::string& prefix, const std::size_t padding, const std::string& suffix, const MaskOptions options, const Time firstTime, const Time lastTime, const Time step = 1, const bool strictPadding = false ) : FileObject( directory, eSequence, options )
	{
		init( prefix, padding, suffix, firstTime, lastTime, step, strictPadding );
	}
	
	Sequence( const boost::filesystem::path& directory, const std::string& pattern, const MaskOptions options, const Time firstTime, const Time lastTime, const Time step, const EPattern accept = ePatternDefault ) : FileObject( directory, eSequence, options )
	{
		init( pattern, firstTime, lastTime, step, accept );
	}
	
	Sequence( const boost::filesystem::path& directory, const MaskOptions options, const Time firstTime, const Time lastTime, const Time step, const EPattern accept = ePatternDefault ) : FileObject( directory, eSequence, options )
	{
		init( firstTime, lastTime, step, accept );
	}

	Sequence( const boost::filesystem::path& directory, const MaskOptions options, const EPattern accept = ePatternDefault );
	
	Sequence( const Sequence& v ):FileObject( v._options ) { operator=( v ); }

	
	virtual ~Sequence(){};
	
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
	friend std::list<Sequence> buildSequence(  const boost::filesystem::path& directory, const FileStrings& id, std::list<FileNumbers>& nums, const MaskOptions& desc );
	
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

	o << _prefix << std::setw( _padding ) << std::setfill( _fillCar ) << time << _suffix;
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
