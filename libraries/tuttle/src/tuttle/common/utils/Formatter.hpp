#ifndef _TUTTLE_COMMON_FORMATTERS_HPP_
#define _TUTTLE_COMMON_FORMATTERS_HPP_

#include "color.hpp"

#include <fstream>
#include <iomanip>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#ifndef WITHOUT_BOOST_LOG

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#if BOOST_VERSION >= 105500
#include <boost/utility/empty_deleter.hpp>
#else
#include <boost/log/utility/empty_deleter.hpp>
#endif

#include <boost/log/expressions/formatters/stream.hpp>
#include <boost/log/expressions/attr.hpp>
#include <boost/log/expressions/message.hpp>

#else

namespace boost {
namespace log {
namespace trivial {
enum severity_level
{
    trace,
    debug,
    info,
    warning,
    error,
    fatal
};
}}}

#endif

namespace tuttle {
namespace common {

class Formatter : boost::noncopyable
{
#ifndef WITHOUT_BOOST_LOG
#ifndef SWIG
	typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > sink_t;
#endif
#endif

private:
	Formatter();

public:
	static boost::shared_ptr<Formatter> get();
	~Formatter() { }

private:
	void init_logging();

public:
	void setLogLevel( const boost::log::trivial::severity_level level );

	void setLogLevel( const int level );
	void setLogLevel( const std::string& level );

	void displayLogLevel( bool display );

public:
	static boost::shared_ptr< Formatter > _formatter;
#ifndef WITHOUT_BOOST_LOG
#ifndef SWIG
	boost::shared_ptr< sink_t > _sink;
#endif
#endif
};

}
}

#endif
