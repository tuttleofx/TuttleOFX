#include "formatters.hpp"


namespace tuttle {
namespace common {
namespace formatters {

boost::shared_ptr<Formatter> Formatter::get()
{
	return formatter;
}

boost::shared_ptr<Formatter> Formatter::formatter(new Formatter);

}
}
}
