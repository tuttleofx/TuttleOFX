#include <tuttle/common/clip/Sequence.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include <algorithm>
#include <iostream>

namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;
namespace bal = boost::algorithm;
namespace ttl = tuttle::common;

void copy_sequence( const ttl::Sequence s, const ttl::Sequence d, int offset = 0 )
{
    if(offset>0)
    {
        for( ttl::Sequence::Time t = s.getLastTime(); t >= s.getFirstTime(); t -= s.getStep() )
	{
                bfs::path sFile = s.getAbsoluteFilenameAt(t);
		if( bfs::exists( sFile ) )
		{
                        bfs::path dFile = d.getAbsoluteFilenameAt(t+offset);
			#ifndef MOVEFILES // copy file(s)
// 			#warning move in off
			if( bfs::exists( dFile ) )
			{
				TUTTLE_CERR("Could not copy: " << dFile.string() );
			}
			else
			{
				bfs::copy_file( sFile, dFile );
			}
			#else // move file(s)
// 			#warning move in on
			if( bfs::exists( dFile ) )
			{
				TUTTLE_CERR("Could not move: " << dFile.string() );
			}
			else
			{
				bfs::rename( sFile, dFile );
			}
			#endif
		}
	}
    }
    else
    {
        for( ttl::Sequence::Time t = s.getFirstTime(); t <= s.getLastTime(); t += s.getStep() )
        {
                bfs::path sFile = s.getAbsoluteFilenameAt(t);
                if( bfs::exists( sFile ) )
                {
                        bfs::path dFile = d.getAbsoluteFilenameAt(t+offset);
                        #ifndef MOVEFILES // copy file(s)
// 			#warning move in off
                        if( bfs::exists( dFile ) )
                        {
                                TUTTLE_CERR("Could not copy: " << dFile.string() );
                        }
                        else
                        {
                                bfs::copy_file( sFile, dFile );
                        }
                        #else // move file(s)
// 			#warning move in on
                        if( bfs::exists( dFile ) )
                        {
                                TUTTLE_CERR("Could not move: " << dFile.string() );
                        }
                        else
                        {
                                bfs::rename( sFile, dFile );
                        }
                        #endif
                }
        }
    }

}

void copy_sequence( const ttl::Sequence s, const bfs::path d, int offset = 0 )
{
    if(offset>0)
    {
        for( ttl::Sequence::Time t = s.getLastTime(); t >= s.getFirstTime(); t -= s.getStep() )
	{
		bfs::path sFile = s.getFilenameAt(t);
                bfs::path dFile = s.getFilenameAt(t+offset);
		if( bfs::exists( s.getDirectory()/sFile ) )
		{
			#ifndef MOVEFILE // copy file(s)
                        if( bfs::exists( d / dFile ) )
			{
				TUTTLE_CERR("Could not copy: " << (d / sFile).string() );
			}
			else
			{
                                bfs::copy_file( s.getDirectory() / sFile, d / dFile );;
			}
			#else // move file(s)
                        if( bfs::exists( d / dFile ) )
			{
				TUTTLE_CERR("Could not move: " << (d / sFile).string() );
			}
			else
			{
// 				try
// 				{
                                        bfs::rename( s.getDirectory() / sFile, d / dFile );;
// 				}
// 				catch()
// 				{
					TUTTLE_CERR ( boost::current_exception_diagnostic_information() );
// 				}
			}
			#endif
		}
	}
    }
    else
    {
        for( ttl::Sequence::Time t = s.getFirstTime(); t <= s.getLastTime(); t += s.getStep() )
        {
                bfs::path sFile = s.getFilenameAt(t);
                bfs::path dFile = s.getFilenameAt(t+offset);
                if( bfs::exists( s.getDirectory()/sFile ) )
                {
                        #ifndef MOVEFILE // copy file(s)
                        if( bfs::exists( d / dFile ) )
                        {
                                TUTTLE_CERR("Could not copy: " << (d / sFile).string() );
                        }
                        else
                        {
                                bfs::copy_file( s.getDirectory() / sFile, d / dFile );;
                        }
                        #else // move file(s)
                        if( bfs::exists( d / dFile ) )
                        {
                                TUTTLE_CERR("Could not move: " << (d / sFile).string() );
                        }
                        else
                        {
// 				try
// 				{
                                        bfs::rename( s.getDirectory() / sFile, d / dFile );;
// 				}
// 				catch()
// 				{
                                        TUTTLE_CERR ( boost::current_exception_diagnostic_information() );
// 				}
                        }
                        #endif
                }
        }
    }
}

int main( int argc, char** argv )
{
        ttl::MaskOptions			descriptionMask     = ttl::eNone;	// by default show nothing
	std::string				availableExtensions;
	std::vector<std::string>		paths;
	std::vector<std::string>		filters;
	std::string				outputPattern;
        bool					verbose             = false;
        int                                     offset              = 0;
	
	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
		("help,h"		, "show this help")
                ("offset,o"             , bpo::value<int>(), "retime the sequence with the given offset. ex: -o 1, -o \"-10\"")
		("verbose,v"		, "explain what is being done")
	;
	
	// describe hidden options
	bpo::options_description hidden;
	hidden.add_options()
		("input-dir", bpo::value< std::vector<std::string> >(), "input directories")
	;
	
	// define default options 
	bpo::positional_options_description pod;
	pod.add("input-dir", -1);
	
	bpo::options_description cmdline_options;
	cmdline_options.add(mainOptions).add(hidden);

	bpo::positional_options_description pd;
	pd.add("", -1);
	
	//parse the command line, and put the result in vm
	bpo::variables_map vm;
	bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(pod).run(), vm);

        // get environnement options and parse them
        #ifndef MOVEFILE // copy file(s)
            if( std::getenv("SAM_CP_OPTIONS") != NULL)
        #else
            if( std::getenv("SAM_MV_OPTIONS") != NULL)
        #endif
        {
            std::vector<std::string> envOptions;
            #ifndef MOVEFILE // copy file(s)
                std::string env = std::getenv("SAM_CP_OPTIONS");
            #else
                std::string env = std::getenv("SAM_MV_OPTIONS");
            #endif
            envOptions.push_back( env );
            bpo::store(bpo::command_line_parser(envOptions).options(cmdline_options).positional(pod).run(), vm);
        }
	bpo::notify(vm);    

	if (vm.count("help"))
	{
	    TUTTLE_COUT( "TuttleOFX project [http://sites.google.com/site/tuttleofx]\n" );
	    #ifndef MOVEFILES
	    TUTTLE_COUT( "NAME\n\tsam-cp - copy sequence(s) in a directory\n" );
            TUTTLE_COUT( "SYNOPSIS\n\tsam-cp [options] sequence[s] [outputDirectory][outputSequence]\n" );
	    #else
	    TUTTLE_COUT( "NAME\n\tsam-mv - move sequence(s) in a directory\n" );
            TUTTLE_COUT( "SYNOPSIS\n\tsam-mv [options] sequence[s] [outputDirectory][outputSequence]\n" );
	    #endif
	    TUTTLE_COUT( "DESCRIPTION\n" << mainOptions );
	    return 1;
	}

	if (vm.count("extension"))
	{
	    bal::split( filters, vm["extension"].as<std::string>(), bal::is_any_of(","));
	}
	
	if (vm.count("all"))
	{
	    // add .* files
	    descriptionMask |= ttl::eDotFile;
	}
	
	// defines paths
	if (vm.count("input-dir"))
	{
	    paths = vm["input-dir"].as< std::vector<std::string> >();
	}
	
	if(paths.size()<2)
	{
	    TUTTLE_COUT("not sequence and/or directory are specified");
	    return 1;
	}

        if (vm.count("offset"))
        {
            offset  = vm["offset"].as<int >();
        }

	if (vm.count("verbose"))
	{
	    verbose = true;
	}

	bfs::path dstPath = paths.back();
	paths.pop_back();
        std::string sequencePattern;
	
	try
	{
                if( !bfs::is_directory(dstPath) )
                {
                    if( bfs::is_directory(dstPath.branch_path() ) )
                    {
                        sequencePattern = dstPath.leaf().string();
                        dstPath         = dstPath.branch_path();
                    }
                    else
                    {
                        TUTTLE_CERR( "Your destination must contain a directory" );
                        return -1;
                    }
                }
                else
                {
                    if( paths.size()>1 )
                    {
                        TUTTLE_CERR( "To copy multiple sequences, your destination must be a directory" );
                        return -1;
                    }
                }

                ttl::Sequence dstSeq(dstPath, descriptionMask);

                bool dstIsSeq;
                if( sequencePattern.find("#") || sequencePattern.find("@") )
                {
        // 		TUTTLE_COUT("Init with pattern: " << dstPath.leaf());
                        dstIsSeq = dstSeq.init( (dstPath.string()+"/"+sequencePattern ), 0, 0, 1, ttl::Sequence::ePatternAll );
                }
                else
                {
        // 		TUTTLE_COUT("Init without pattern");
                        dstIsSeq = dstSeq.init( 0, 0, 1, ttl::Sequence::ePatternAll );
                }


                BOOST_FOREACH( bfs::path srcPath, paths)
                {
                        ttl::Sequence srcSeq(srcPath.branch_path(), descriptionMask );
                        bool srcIsSeq = srcSeq.initFromDetection( srcPath.string(), ttl::Sequence::ePatternDefault );
                        if( !srcIsSeq )
                        {
                                TUTTLE_CERR( "Input is not a sequence. (" << srcPath << ")" );
                        }
                        else
                        {
                                if( srcSeq.getNbFiles() == 0 )
                                {
                                        TUTTLE_CERR( "No existing file for the input sequence. (" << srcPath << ")" );
                                }
                                else
                                {
                                        if( dstIsSeq )
                                        {
                                                if(verbose)
                                                  TUTTLE_COUT( srcSeq.getAbsoluteStandardPattern() << " -> " << dstSeq.getAbsoluteStandardPattern() << " (" << srcSeq.getNbFiles() << ") " );
                                                copy_sequence( srcSeq, dstSeq, offset );
                                        }
                                        else{
                                                if(verbose)
                                                  TUTTLE_COUT( srcSeq.getAbsoluteStandardPattern() << " -> " << dstPath / srcSeq.getStandardPattern() << " (" << srcSeq.getNbFiles() << ")"  );
                                                copy_sequence( srcSeq, dstPath, offset );
                                        }
                                }
                        }
                }
	}
	catch (bfs::filesystem_error &ex)
	{
		TUTTLE_COUT( ex.what() );
	}
	catch(... )
	{
	    TUTTLE_CERR ( boost::current_exception_diagnostic_information() );
	}

	return 0;
}

