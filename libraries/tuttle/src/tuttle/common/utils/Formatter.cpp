#include "Formatter.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/exceptions.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/lexical_cast.hpp>


namespace tuttle {
namespace common {

boost::shared_ptr<Formatter> Formatter::get()
{
	if( _formatter.get() == NULL )
		_formatter.reset( new Formatter() );
	return _formatter;
}

boost::shared_ptr<Formatter> Formatter::_formatter;


Formatter::Formatter()
{
	init_logging();

	const char* envLevel = std::getenv("TUTTLE_LOG_LEVEL");
	if( envLevel == NULL )
	{
		setLogLevel( boost::log::trivial::warning );
	}
	else
	{
		setLogLevel( envLevel );
	}
}

void Formatter::init_logging()
{
#ifndef WITHOUT_BOOST_LOG
	namespace sinks = boost::log::sinks;
#if BOOST_VERSION >= 105500
	using boost::empty_deleter;
#else
	using boost::log::empty_deleter;
#endif

	 // Create a backend and attach a stream to it
	boost::shared_ptr< sinks::text_ostream_backend > backend = boost::make_shared< sinks::text_ostream_backend >();
	backend->add_stream( boost::shared_ptr< std::ostream >( &std::clog, empty_deleter() ) );
	//backend->add_stream( boost::shared_ptr< std::ostream >( new std::ofstream("sample.log") ) );

	// Enable auto-flushing after each log record written
	backend->auto_flush(true);

	// Wrap it into the frontend and register in the core.
	_sink = boost::make_shared< sink_t >( backend );

	// Specify format of the log records
	displayLogLevel( false );

	// Register the sink in the logging core
	boost::log::core::get()->add_sink( _sink );
#endif
}

void Formatter::setLogLevel( const EVerboseLevel level )
{
	switch( level )
	{
		case eVerboseLevelFatal: setLogLevel( boost::log::trivial::fatal ); break;
		case eVerboseLevelError: setLogLevel( boost::log::trivial::error ); break;
		case eVerboseLevelWarning: setLogLevel( boost::log::trivial::warning ); break;
		case eVerboseLevelInfo: setLogLevel( boost::log::trivial::info ); break;
		case eVerboseLevelDebug: setLogLevel( boost::log::trivial::debug ); break;
		case eVerboseLevelTrace: setLogLevel( boost::log::trivial::trace ); break;
		default:
			setLogLevel( boost::log::trivial::warning );
			TUTTLE_LOG_WARNING( "Unrecognized log level " << level << ", fallback to \"warning\"." );
			break;
	}
}

void Formatter::setLogLevel( const std::string& strLevel )
{
	static const std::vector<std::string> m = boost::assign::list_of
		("fatal")
		("error")
		("warning")
		("info")
		("debug")
		("trace");
	std::string lowerStrLevel = strLevel;
	boost::algorithm::to_lower(lowerStrLevel);
	
	std::vector<std::string>::const_iterator v = std::find(m.begin(), m.end(), lowerStrLevel);

	if( v == m.end() )
	{
		TUTTLE_LOG_WARNING( "Unrecognized log level " << quotes(strLevel) << ", fallback to \"warning\"." );
		setLogLevel( eVerboseLevelWarning );
	}
	else
	{
		setLogLevel( static_cast<EVerboseLevel>(std::distance(m.begin(), v)) );
	}
}

void Formatter::setLogLevel( const boost::log::trivial::severity_level level )
{
#ifndef WITHOUT_BOOST_LOG
	boost::log::core::get()->set_filter( boost::log::trivial::severity >= level );
#endif
}

void Formatter::displayLogLevel( bool display )
{
#ifndef WITHOUT_BOOST_LOG
	namespace expr = boost::log::expressions;
	_sink->reset_formatter();
	if( display )
	{
		_sink->set_formatter(
			expr::stream
			//<< Color::get()->_yellow  << "[" + logging::trivial::severity + "]" << Color::get()->_std << " "
			<< "["<< boost::log::trivial::severity << "]" << " "
			<< expr::smessage
		);
	}
	else
	{
		_sink->set_formatter ( expr::stream << expr::smessage );
	}
#endif
}

}
}
