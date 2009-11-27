/*
 * lut_reader.cpp
 *
 *  Created on: 14 sept. 2009
 *      Author: Eloi Du Bois
 */

#include "lut_reader.h"

#include <algorithm>
#include <sstream>
#include <iostream>

// BOOST
#include <boost/filesystem/path.hpp>
#include <boost/tokenizer.hpp>
#include <boost/regex.h>
#include <boost/spirit.hpp>
#include <boost/spirit/core.hpp>
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace boost;
using namespace boost::spirit;
namespace fs = boost::filesystem;
using namespace std;

namespace tuttle
{

bool LutReader::read(const fs::path & filename)
{
    stringstream ss;
    fs::ifstream file;
    file.open( filename, std::ios::in );
    ss << file.rdbuf();

    gram_3dl g( *this );
    parse_info<> info = parse( ss.str().c_str(), g >> eps_p );

    // Normalize values
    if ( _data.size() > 0 )
    {
        double max = *( max_element( _data.begin(), _data.end() ) );
        double norm = 1.0 / std::pow( 2.0, std::ceil( std::log( max ) / std::log( 2.0 ) ) );
        for( vector<double>::iterator it = _data.begin(); it != _data.end(); ++it )
        {
            (*it) *= norm;
        }
    }

    return !info.full;
}

} // namespace tuttle
