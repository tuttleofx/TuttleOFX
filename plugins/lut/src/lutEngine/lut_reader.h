#ifndef LUT_READER_H
#define LUT_READER_H

#include <vector>
#include <boost/filesystem/path.hpp>
#include <boost/regex.h>
#include <boost/spirit.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/core.hpp>
#include <boost/function.hpp>


using namespace std;
using namespace boost;
namespace fs = boost::filesystem;
using namespace boost::spirit;

namespace tuttle
{

class LutReader
{
protected:
	vector<double> _data;
	vector<double> _steps;

public:
	LutReader( ) { }
	virtual ~LutReader( ) { }

	// Set 3DL filename
	bool read(const fs::path & filename);

	vector<double> & data()  { return _data; }
	vector<double> & steps() { return _steps; }
};

// 3DL File parser
struct gram_3dl : public grammar<gram_3dl>
{
    gram_3dl( LutReader & lutReader )
    : _lutReader( lutReader )
    {
    }

    template <typename ScannerT>
        struct definition
        {
            definition( const gram_3dl & self )
            {
                gram_3dl_spec     = *gram_comment >> gram_line_quantiz >> +( gram_comment | gram_rgb | space_p );
                gram_rgb          = *blank_p >> *( uint_p[ push_back_a( self._lutReader.data() ) ] % *blank_p ) >> +space_p;
                gram_line_quantiz = *blank_p >> *( uint_p[ push_back_a( self._lutReader.steps() ) ] % *blank_p ) >> +space_p;
                gram_comment      = *blank_p >> ch_p( '#' ) >> *( anychar_p - '\n' ) >> +space_p;
            }

            rule<ScannerT> gram_test, gram_comment, gram_line_quantiz, gram_rgb, gram_3dl_spec;
            rule<ScannerT> const& start() const { return gram_3dl_spec; }
        };

    LutReader & _lutReader;
};

}  // namespace tuttle

#endif // LUT_READER_H
