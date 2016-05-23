#ifndef _LUTENGINE_LUTREADER_HPP_
#define _LUTENGINE_LUTREADER_HPP_

#include <tuttle/plugin/memory/OfxAllocator.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/function.hpp>

#include <vector>

namespace tuttle
{

using namespace boost;
namespace fs = boost::filesystem;

class LutReader
{
public:
    typedef std::vector<double, tuttle::plugin::OfxAllocator<double> > VectorDouble;

protected:
    VectorDouble _data;
    VectorDouble _steps;
    bool _bReadOk;

public:
    LutReader()
        : _bReadOk(false)
    {
    }
    virtual ~LutReader() {}

    /// Set 3DL filename
    bool read(const fs::path& filename);
    /// Is a file read success
    const bool readOk() const { return _bReadOk; }
    VectorDouble& data() { return _data; }
    VectorDouble& steps() { return _steps; }
};

// 3DL File parser
struct gram_3dl : public boost::spirit::classic::grammar<gram_3dl>
{
    gram_3dl(LutReader& lutReader)
        : _lutReader(lutReader)
    {
    }

    template <typename ScannerT>
    struct definition
    {
        definition(const gram_3dl& self)
        {
            using namespace boost::spirit::classic;
            gram_3dl_spec = *gram_comment >> gram_line_quantiz >> +(gram_comment | gram_rgb | space_p);
            gram_rgb = *blank_p >> *(uint_p[push_back_a(self._lutReader.data())] % *blank_p) >> +space_p;
            gram_line_quantiz = *blank_p >> *(uint_p[push_back_a(self._lutReader.steps())] % *blank_p) >> +space_p;
            gram_comment = *blank_p >> ch_p('#') >> *(anychar_p - '\n') >> +space_p;
        }

        boost::spirit::classic::rule<ScannerT> gram_test, gram_comment, gram_line_quantiz, gram_rgb, gram_3dl_spec;
        boost::spirit::classic::rule<ScannerT> const& start() const { return gram_3dl_spec; }
    };

    LutReader& _lutReader;
};
}

#endif
