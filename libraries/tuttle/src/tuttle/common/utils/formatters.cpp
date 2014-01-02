#include "formatters.hpp"


namespace tuttle {
namespace common {
namespace formatters {

boost::shared_ptr<Formatter> Formatter::get()
{
	return _formatter;
}

boost::shared_ptr<Formatter> Formatter::_formatter(new Formatter);


void Formatter::init_logging()
{
	namespace sinks = boost::log::sinks;

	boost::shared_ptr< sinks::text_ostream_backend > backend = boost::make_shared< sinks::text_ostream_backend >();
	backend->add_stream( boost::shared_ptr< std::ostream >( &std::clog, boost::log::empty_deleter() ));
	//backend->add_stream( boost::shared_ptr< std::ostream >( new std::ofstream("sample.log") ) );

	backend->auto_flush(true);

	//sink(  new sink_t( backend ) );

	_sink = boost::make_shared< sink_t >( backend );

	displayLogLevel( false );

	boost::log::core::get()->add_sink( _sink );
}

void Formatter::setLogLevel( const boost::log::trivial::severity_level level )
{
	boost::log::core::get()->set_filter( boost::log::trivial::severity >= level );
}

void Formatter::displayLogLevel( bool display )
{
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
}

}
}
}
