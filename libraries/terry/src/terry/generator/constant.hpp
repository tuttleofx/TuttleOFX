#ifndef _TERRY_GENERATOR_CONSTANT_HPP_
#define _TERRY_GENERATOR_CONSTANT_HPP_

#include <boost/gil/utilities.hpp>

#include <cmath>

namespace terry
{
namespace generator
{

// Models a Unary Function
template <typename P>
// Models PixelValueConcept
struct ConstantFunctor
{
    typedef boost::gil::point2<double> point_t;
    typedef ConstantFunctor const_t;
    typedef P value_type;
    typedef value_type reference;
    typedef value_type const_reference;
    typedef point_t argument_type;
    typedef reference result_type;
    BOOST_STATIC_CONSTANT(bool, is_mutable = false);

    value_type _color;

    ConstantFunctor() {}
    ConstantFunctor(const value_type& color)
        : _color(color)
    {
    }

    result_type operator()(const point_t&) const { return _color; }
};

template <typename Pixel>
struct ConstantColorViewFactory
{
    typedef terry::generator::ConstantFunctor<Pixel> ConstantFunctorT;
    typedef typename ConstantFunctorT::point_t Point;
    typedef boost::gil::virtual_2d_locator<ConstantFunctorT, false> Locator;
    typedef boost::gil::image_view<Locator> ConstantVirtualView;

    static ConstantVirtualView getView(const Pixel& color)
    {
        boost::function_requires<PixelLocatorConcept<Locator> >();
        gil_function_requires<StepIteratorConcept<typename Locator::x_iterator> >();

        return ConstantVirtualView(Point(1, 1), Locator(Point(0, 0), Point(1, 1), ConstantFunctorT(color)));
    }
};

template <typename Pixel>
typename ConstantColorViewFactory<Pixel>::ConstantVirtualView constantColorView(const Pixel& color)
{
    return ConstantColorViewFactory<Pixel>::getView(color);
}
}
}

#endif
