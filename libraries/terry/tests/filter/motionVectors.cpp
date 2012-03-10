#include <terry/globals.hpp>
#include <terry/sampler/all.hpp>
#include <terry/filter/motionVectors.hpp>

#include <iostream>

#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( terry_filter_motionVector )

struct NoProgress
{
        void progressBegin( const int numSteps, const std::string& msg = "" ){}
        void progressEnd(){}
        bool progressForward( const int nSteps ){ return false; }
};



BOOST_AUTO_TEST_CASE( motionVector )
{
	using namespace terry;
	
	// empty input/output, the test only check the compilation
	terry::rgb32f_view_t inView;
	terry::gray32f_view_t xVecView, yVecView;
	terry::rgb32f_view_t outView;
	NoProgress progress;
	
	//terry::filter::applyLocalMaxima( inView, outView );
	filter::motionvectors_resample_pixels<sampler::bilinear_sampler>(
			inView, getBounds<std::ssize_t>( inView ),
			xVecView, yVecView, getBounds<std::ssize_t>( xVecView ),
			outView, getBounds<std::ssize_t>( outView ),
			getBounds<std::ssize_t>( outView ),
			sampler::eParamFilterOutCopy,
			progress );

}


BOOST_AUTO_TEST_SUITE_END()
