#ifndef _FILENAMEMANAGER_HPP_
#define _FILENAMEMANAGER_HPP_

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/utils/color.hpp>

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
	eMaskTypeUndefined  = 0,			           // 0
	eMaskTypeDirectory  = 1,			           // 1<<0
	eMaskTypeFile       = eMaskTypeDirectory * 2,  // 1<<1
	eMaskTypeSequence   = eMaskTypeFile      * 2,  // 1<<2
	
	eMaskTypeDefault    = eMaskTypeSequence
};

enum EMaskOptions
{
	eMaskOptionsNone         = 0,			                  // 0
	eMaskOptionsProperties   = 1,			                  // show type of FileObject
	eMaskOptionsPath         = eMaskOptionsProperties   * 2,  // show path of FileObject
	eMaskOptionsAbsolutePath = eMaskOptionsPath         * 2,  // show absolute path of FileObject
	eMaskOptionsDotFile      = eMaskOptionsAbsolutePath * 2,  // show files which start with a dot (hidden files)
	eMaskOptionsColor        = eMaskOptionsDotFile      * 2,  // output with color
	
	eMaskOptionsDefault      = ( eMaskOptionsPath | eMaskOptionsColor )
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
		setColorActive( _options & eMaskOptionsColor );
	}
	FileObject( const EMaskOptions options )
	{
		_directory.clear();
		_type		= eMaskTypeUndefined;
		_options	= options;
		setColorActive( _options & eMaskOptionsColor );
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
	
	inline boost::filesystem::path getDirectory         () const				                { return _directory; }
	inline boost::filesystem::path getAbsoluteDirectory () const				                { return boost::filesystem::absolute(_directory); }
	inline void                    setDirectory         ( const boost::filesystem::path& p )	{ _directory = p; }
	void                           setDirectoryFromPath ( const boost::filesystem::path& p );
	
<<<<<<< HEAD
	EMaskOptions                   getMaskOptions	    () const				                { return _options; }
	EMaskType                      getMaskType	        () const				                { return _type; }
=======
	void                           setMaskOptions	( const EMaskOptions& options )    { _options = options; }
	void                           setMaskType	( const EMaskType& type )          { _type = type; }

	EMaskOptions                    getMaskOptions	() const                           { return _options; }
	EMaskType                       getMaskType	() const                           { return _type; }
>>>>>>> 6fa0f1abe0806f9834d74583cb6985782243fc01

	virtual inline void clear()
	{
		_directory.clear();
		_type    = eMaskTypeDefault;
		_options = eMaskOptionsDefault;
		initOutputColor ( _options & eMaskOptionsColor );
	}

private:
	void init( const boost::filesystem::path& directory, const EMaskType& type, const EMaskOptions& options )
	{
		_directory  = directory;
		_type       = type;
		_options    = options;

		setColorActive( _options & eMaskOptionsColor );
	}
	
	void setColorActive( bool activate = false )
	{
		if(activate)
		{
			_kColorStd      = kColorStd;
			_kColorFolder   = kColorFolder;
			_kColorFile     = kColorFile;
			_kColorSequence = kColorSequence;
			_kColorError    = kColorError;
		}
		else
		{
			_kColorStd      = "";
			_kColorFolder   = "";
			_kColorFile     = "";
			_kColorSequence = "";
			_kColorError    = "";
		}
	}

protected:
	
	inline bool showProperties  () const  { return _options & eMaskOptionsProperties; }
	inline bool showRelativePath() const  { return _options & eMaskOptionsPath; }
	inline bool showAbsolutePath() const  { return _options & eMaskOptionsAbsolutePath; }
	
	
	boost::filesystem::path _directory;     ///< directory

	EMaskType               _type;          ///< specify type of object
	EMaskOptions            _options;       ///< specify output options of object, common for each objects

	std::string             _kColorStd;
	std::string             _kColorFolder;
	std::string             _kColorFile;
	std::string             _kColorSequence;
	std::string             _kColorError;
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
	~Folder(){};
	
	std::ostream&				getCout( std::ostream& os ) const;
	
	std::vector<boost::filesystem::path>	getFiles() const;

	inline void clear()
	{
		FileObject::clear();
		_folderName.clear();
	}
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
	~File(){}
	
	std::ostream&				getCout( std::ostream& os ) const ;
	std::vector<boost::filesystem::path>	getFiles() const;
	inline std::string			getAbsoluteFilename	()			const { return ( getAbsoluteDirectory() / _filename ).string(); }

	inline void clear()
	{
		FileObject::clear();
		_filename.clear();
	}

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
		ePatternNone        = 0,
		ePatternStandard    = 1,
		ePatternCStyle      = ePatternStandard * 2,
		ePatternFrame       = ePatternCStyle   * 2,

		ePatternDefault	    = ePatternCStyle + ePatternStandard,
		ePatternAll	        = ePatternFrame  + ePatternCStyle + ePatternStandard
	};
	
	Sequence() : FileObject( )
	{
		TUTTLE_COUT("create ");
		_prefix.clear();
		_suffix.clear();
		_padding    = 0;
		_step       = 1;
		_firstTime  = 0;
		_lastTime   = 0;
		_nbFiles    = 0;
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
		_options = v._options;
	}

	Sequence( const boost::filesystem::path& directory, const Sequence& v, const EMaskOptions& options )
		: FileObject( options )
	{
		operator=( v );
		_options = options;
		setDirectory( directory );
	}
	
	~Sequence()
	{}
	
private:
	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 * @warning the directory must be set
	 */
	void init( const std::string& prefix, const std::size_t padding, const std::string& suffix, const Time firstTime, const Time lastTime, const Time step = 1, const bool strictPadding = false );

	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 * @warning the directory must be set
	 */
	bool init( const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EPattern accept = ePatternDefault );

	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 * @warning the directory must be set
	 */
	bool init( const Time firstTime, const Time lastTime, const Time step, const EPattern accept = ePatternDefault );
	
public:
	/**
	 * @brief Construct a sequence from a pattern and given informations.
	 * @warning No check on your filesystem.
	 * @warning the directory must be set
	 */
	bool initFromPattern( const boost::filesystem::path& dir, const std::string& pattern, const Time firstTime, const Time lastTime, const Time step, const EMaskOptions options = eMaskOptionsDefault, const EPattern accept = ePatternDefault );

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
	
	inline std::string            getAbsoluteFilenameAt       ( const Time time )  const;
	inline std::string            getFilenameAt                ( const Time time )  const;
	inline std::string            getFirstFilename             ()        const { return getFilenameAt( getFirstTime() ); }
	inline std::string            getAbsoluteFirstFilename    ()        const { return ( getAbsoluteDirectory() / getFilenameAt( getFirstTime() ) ).string(); }
	inline std::string            getAbsoluteLastFilename     ()        const { return ( getAbsoluteDirectory() / getFilenameAt( getLastTime()  ) ).string(); }

	/// @return pattern character in standard style
	inline char                  getPatternCharacter          ()        const { return getPadding() ? '#' : '@'; }
	/// @return a string pattern using standard style
	inline std::string            getStandardPattern           ()        const { return getPrefix() + std::string( getPadding() ? getPadding() : 1, getPatternCharacter() ) + getSuffix(); }
	inline std::string            getAbsoluteStandardPattern ()        const { return (getAbsoluteDirectory() / getStandardPattern()).string(); }
	/// @return a string pattern using C Style
	inline std::string            getCStylePattern             ()        const
	{
		if( getPadding() )
			return getPrefix() + "%0" + boost::lexical_cast<std::string>( getPadding() ) + "d" + getSuffix();
		else
			return getPrefix() + "%d" + getSuffix();
	}

	inline std::string            getAbsoluteCStylePattern ()           const { return (getDirectory() / getCStylePattern()).string(); }

	inline std::pair<Time, Time>  getRange	                      ()        const { return std::pair<Time, Time>( getFirstTime(), getLastTime() ); }
	inline std::size_t            getStep                       ()        const { return _step; }
	inline Time	                   getFirstTime                 ()        const { return _firstTime; }
	inline Time	                   getLastTime                  ()        const { return _lastTime; }
	inline std::size_t            getDuration                   ()        const { return getLastTime() - getFirstTime() + 1; }
	inline Time	                   getNbFiles                    ()        const { return _nbFiles; }
	inline std::size_t            getPadding                    ()        const { return _padding; }
	inline bool                  isStrictPadding               ()        const { return _strictPadding; }
	inline bool                  hasMissingFile                ()        const { return getNbMissingFiles() != 0; }
	inline std::size_t            getNbMissingFiles            ()        const
	{
		if( !getStep() )
			return 0;
		return ( ( ( getLastTime() - getFirstTime() ) / getStep() ) + 1 ) - getNbFiles();
	}
	/// @brief filename without frame number
	inline std::string            getIdentification            ()        const { return _prefix + _suffix; }
	inline std::string            getPrefix                     ()        const { return _prefix; }
	inline std::string            getSuffix                     ()        const { return _suffix; }

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
	bool retrieveInfosFromPattern( const std::string& pattern, const EPattern& accept, std::string& prefix, std::string& suffix, std::size_t& padding, bool& strictPadding ) const;

private:
	friend std::list<Sequence> buildSequence(  const boost::filesystem::path& directory, const FileStrings& id, std::list<FileNumbers>& nums, const EMaskOptions& desc );
	
	std::ostream&				getCout( std::ostream& os ) const ;
	std::vector<boost::filesystem::path>	getFiles() const;
protected:
	inline void clear()
	{
		FileObject::clear();
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
	return ( getAbsoluteDirectory() / getFilenameAt( time ) ).string();
}

inline bool Sequence::initFromDetection( const EPattern& accept )
{
	return this->initFromDetection( getDirectory().string(), accept );
}


}
}

#endif
