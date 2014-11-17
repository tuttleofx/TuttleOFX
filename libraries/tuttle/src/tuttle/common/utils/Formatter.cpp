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
	return _formatter;
}

boost::shared_ptr<Formatter> Formatter::_formatter(new Formatter);


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
		setLogLevel_string( envLevel );
	}
}

void Formatter::init_logging()
{
#ifndef WITHOUT_BOOST_LOG
	namespace sinks = boost::log::sinks;

	boost::shared_ptr< sinks::text_ostream_backend > backend = boost::make_shared< sinks::text_ostream_backend >();
	backend->add_stream( boost::shared_ptr< std::ostream >( &std::clog, boost::log::empty_deleter() ));
	//backend->add_stream( boost::shared_ptr< std::ostream >( new std::ofstream("sample.log") ) );

	backend->auto_flush(true);

	//sink(  new sink_t( backend ) );

	_sink = boost::make_shared< sink_t >( backend );

	displayLogLevel( false );

	boost::log::core::get()->add_sink( _sink );
#endif
}

void Formatter::setLogLevel_int( const int level )
{
	switch( level )
	{
		case 0: setLogLevel( boost::log::trivial::trace ); break;
		case 1: setLogLevel( boost::log::trivial::debug ); break;
		case 2: setLogLevel( boost::log::trivial::info ); break;
		case 3: setLogLevel( boost::log::trivial::warning ); break;
		case 4: setLogLevel( boost::log::trivial::error ); break;
		case 5: setLogLevel( boost::log::trivial::fatal ); break;
		default:
			setLogLevel( boost::log::trivial::warning );
			TUTTLE_LOG_WARNING( "Unrecognized log level " << level << ", fallback to \"warning\" (3)." );
			break;
	}
}

int logLevel_stringToInt( const std::string& level )
{
	static const std::vector<std::string> m = boost::assign::list_of
		("trace")
		("debug")
		("info")
		("warning")
		("error")
		("fatal");
	std::string lowerStrLevel = level;
	boost::algorithm::to_lower(lowerStrLevel);
	
	std::vector<std::string>::const_iterator v = std::find(m.begin(), m.end(), lowerStrLevel);
	
	if( v == m.end() )
	{
		TUTTLE_LOG_WARNING( "Unrecognized log level " << quotes(level) << ", fallback to \"warning\" (3)." );
		return 3;
	}
	return std::distance(m.begin(), v);
}

void Formatter::setLogLevel_string( const std::string& level )
{
	int levelInt = 0;
	try
	{
		// level is a string containing an integer
		levelInt = boost::lexical_cast<int>(level);
		setLogLevel_int( levelInt );
	}
	catch(const boost::bad_lexical_cast &)
	{
		// level is a string containing an level like "warning"
		levelInt = logLevel_stringToInt(level);
	}
	setLogLevel_int( levelInt );
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
