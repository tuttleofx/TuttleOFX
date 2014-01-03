#ifndef _COMMON_FORMATTERS_HPP_
#define _COMMON_FORMATTERS_HPP_

#include "color.hpp"

#include <fstream>
#include <iomanip>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>


#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/empty_deleter.hpp>
#include <boost/log/expressions/formatters/stream.hpp>
#include <boost/log/expressions/attr.hpp>
#include <boost/log/expressions/message.hpp>


namespace tuttle {
namespace common {
namespace formatters {

class Formatter : boost::noncopyable
{
	typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > sink_t;

private:
	Formatter();

public:
	static boost::shared_ptr<Formatter> get();
	~Formatter() { }
	
	void init_logging();
	
	void setLogLevel( const boost::log::trivial::severity_level level );
	
	void setLogLevel_int( const int level );
	void setLogLevel_string( const std::string& level );

	void displayLogLevel( bool display );

public:
	static boost::shared_ptr< Formatter > _formatter;
	boost::shared_ptr< sink_t > _sink;
};

}
}
}
#endif
