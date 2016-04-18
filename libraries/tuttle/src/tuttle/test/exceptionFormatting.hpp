#ifndef _TUTTLE_TEST_EXCEPTIONFORMATTING_HPP_
#define _TUTTLE_TEST_EXCEPTIONFORMATTING_HPP_

#include "boost/test/unit_test_monitor.hpp"
#include "boost/test/unit_test_suite.hpp"
#include "boost/test/test_tools.hpp"
#include "boost/exception/exception.hpp"
#include "boost/exception/diagnostic_information.hpp"

inline void translate(boost::exception const& e)
{
    BOOST_FAIL(boost::diagnostic_information(e));
}

/**
 * Register a display function for boost_test to display boost_exception information.
 */
struct Exception_fixture
{
    Exception_fixture()
    {
        ::boost::unit_test::unit_test_monitor.register_exception_translator<boost::exception>(&translate);
    }
};

BOOST_GLOBAL_FIXTURE(Exception_fixture);

#endif
