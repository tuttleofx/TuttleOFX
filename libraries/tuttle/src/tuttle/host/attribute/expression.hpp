#ifndef _TUTTLE_HOST_ATTRIBUTE_EXPRESSION_HPP_
#define _TUTTLE_HOST_ATTRIBUTE_EXPRESSION_HPP_

#include <tuttle/host/exceptions.hpp>

#ifdef TUTTLE_HOST_WITH_PYTHON_EXPRESSION
#include <boost/python.hpp>
#else
#include <boost/lexical_cast.hpp>
#endif

namespace tuttle
{
namespace host
{
namespace attribute
{

#ifdef TUTTLE_HOST_WITH_PYTHON_EXPRESSION
namespace expression_details
{

inline boost::python::object pythonObjectFromExpression(const std::string& expression)
{
    using namespace boost;
    std::string fullExp("from math import *\n"
                        "import os\n"
                        "import sys\n");

    python::object main = python::import("__main__");
    python::object global(main.attr("__dict__"));
    python::exec(fullExp.c_str(), global);
    return python::eval(expression.c_str(), global, global);
}

template <typename Type>
inline Type extractValueFromPythonObject(const boost::python::object& result)
{
    using namespace boost;
    try
    {
        Type res = python::extract<Type>(result) BOOST_EXTRACT_WORKAROUND;
        return res;
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(exception::Value() << exception::user() + "Can't extract value from expression."
                                                 << exception::dev() + boost::current_exception_diagnostic_information());
    }
}

} // end namespace

template <typename Type>
inline Type extractValueFromExpression(const std::string& expression)
{
    using namespace boost;
    try
    {
        python::object result = expression_details::pythonObjectFromExpression(expression);
        return expression_details::extractValueFromPythonObject<Type>(result);
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(exception::Value()
                              << exception::user() + "Syntaxe error for expression: \"" + expression + "\""
                              << exception::dev() + boost::current_exception_diagnostic_information());
    }
}

template <>
inline std::string extractValueFromExpression<std::string>(const std::string& expression)
{
    using namespace boost;
    typedef std::string Type;
    try
    {
        python::object result = expression_details::pythonObjectFromExpression(expression);
        return expression_details::extractValueFromPythonObject<Type>(result);
    }
    catch(...)
    {
        return expression;
    }
}

#else

template <typename Type>
inline Type extractValueFromExpression(const std::string& expression)
{
    try
    {
        return boost::lexical_cast<Type>(expression);
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(exception::Value()
                              << exception::user() + "Syntaxe error for expression: \"" + expression + "\""
                              << exception::dev() + boost::current_exception_diagnostic_information());
    }
}

#endif
}
}
}

#endif
