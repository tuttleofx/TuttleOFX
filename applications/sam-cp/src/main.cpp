#include <tuttle/common/clip/Sequence.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include <algorithm>
#include <iostream>

namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;
namespace ttl = tuttle::common;

void copy_sequence( const ttl::Sequence s, const ttl::Sequence d )
{
	for( ttl::Sequence::Time t = s.getFirstTime(); t <= s.getLastTime(); t += s.getStep() )
	{
		bfs::path sFile = s.getAbsoluteFilenameAt(t);
		if( bfs::exists( sFile ) )
		{
			bfs::path dFile = d.getAbsoluteFilenameAt(t);
			bfs::copy_file( sFile, dFile );
		}
	}
}

void copy_sequence( const ttl::Sequence s, const bfs::path d )
{
	for( ttl::Sequence::Time t = s.getFirstTime(); t <= s.getLastTime(); t += s.getStep() )
	{
		bfs::path sFile = s.getFilenameAt(t);
		if( bfs::exists( s.getDirectory()/sFile ) )
		{
			bfs::copy_file( s.getDirectory() / sFile, d / sFile );
		}
	}
}

int Split(std::vector<std::string>& vecteur, std::string &chaine, char separateur)
{
	std::string::size_type stTemp = chaine.find(separateur);

	while(stTemp != std::string::npos)
	{
		vecteur.push_back(chaine.substr(0, stTemp));
		chaine = chaine.substr(stTemp + 1);
		stTemp = chaine.find(separateur);
	}
	
	vecteur.push_back(chaine);
	return vecteur.size();
}

int main( int argc, char** argv )
{

// 	int					researchMask	= ttl::eSequence || ttl::eDirectory || ttl::eFile ;	// by default copy sequences, directories, files
	ttl::MaskOptions			descriptionMask	= ttl::eNone;				// by default show nothing
	std::string				availableExtensions;
	std::vector<std::string>		paths;
	std::vector<std::string>		filters;
	std::string				outputPattern;
	
	// Declare the supported options.
	bpo::options_description mainOptions;
	mainOptions.add_options()
		("all,a"		, "do not ignore entries starting with .")
		("extension,e"		, bpo::value<std::string>(), "list with a specific pattern, ex: *.jpg,*.png")
		("help,h"		, "show this help")
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

	bpo::notify(vm);    

	if (vm.count("help"))
	{
	    TUTTLE_COUT( "TuttleOFX project [http://sites.google.com/site/tuttleofx]\n" );
	    TUTTLE_COUT( "NAME\n\tsam-cp - copy sequence(s) in a directory\n" );
	    TUTTLE_COUT( "SYNOPSIS\n\tsam-cp [options] sequence[s] outputDirectory\n" );
	    TUTTLE_COUT( "DESCRIPTION\n" << mainOptions );
	    return 1;
	}

	if (vm.count("extension"))
	{
	    std::string extensions = vm["extension"].as<std::string>();
	    Split( filters, extensions, 0x2C);
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

	bfs::path dstPath = paths.back();
	paths.pop_back();
	
	if(bfs::is_directory(dstPath.branch_path()))
	{
// 	    TUTTLE_COUT ("last file, not a directory: " << dstPath.branch_path() << " | " << dstPath.leaf());
// 	    if( bfs::is_directory(dstPath.branch_path()) && (!dstPath.leaf().find("#") || !dstPath.leaf().find("@")) ) // we check we have a path and a pattern
// 	    {
// 		TUTTLE_COUT ("continue");
		
		ttl::Sequence dstSeq(dstPath.branch_path(), descriptionMask);
		bool dstIsSeq = dstSeq.init( 0, 0, 1, ttl::Sequence::ePatternAll );
		
		BOOST_FOREACH( bfs::path srcPath, paths)
		{
			ttl::Sequence srcSeq(srcPath.branch_path(), descriptionMask );
			bool srcIsSeq = srcSeq.initFromDetection( srcPath.string(), ttl::Sequence::ePatternDefault );
			if( !srcIsSeq )
			{
				TUTTLE_CERR( "Input is not a sequence. (" << srcPath << ")" );
				return -1;
			}
			if( srcSeq.getNbFiles() == 0 )
			{
				TUTTLE_CERR( "No existing file for the input sequence. (" << srcPath << ")" );
				return -1;
			}
			if( dstIsSeq )
			{
				TUTTLE_COUT( "dst is seq" << srcSeq.getAbsoluteStandardPattern() << " -> " << dstSeq.getAbsoluteStandardPattern() << " (" << srcSeq.getNbFiles() << ")" );
				copy_sequence( srcSeq, dstSeq );
			}
			else{
				TUTTLE_COUT( "dst no seq" << srcSeq.getAbsoluteStandardPattern() << " -> " << dstPath.branch_path() / srcSeq.getStandardPattern() << " (" << srcSeq.getNbFiles() << ")"  );
				copy_sequence( srcSeq, dstPath.branch_path() );
			}
		}

	}
	
	/*
	try
	{
		
		bool dstIsDir = bfs::is_directory( dstPath );
		ttl::Sequence dstSeq;
		bool dstIsSeq = dstSeq.init( dstPath, 0, 0, 1, ttl::Sequence::ePatternAll );

		if( !dstIsDir && !dstIsSeq )
		{
			TUTTLE_CERR( "Your destination must be a sequence or a directory." );
			return -1;
		}
		if( allSrc.size() > 1 && ! dstIsDir )
		{
			TUTTLE_CERR( "To copy multiple sequences, your destination must be a directory." );
			return -1;
		}
		BOOST_FOREACH( bfs::path srcPath, allSrc )
		{
			ttl::Sequence srcSeq;
			bool srcIsSeq = srcSeq.initFromDetection( srcPath, ttl::Sequence::ePatternDefault );
			if( ! srcIsSeq )
			{
				TUTTLE_CERR( "Input is not a sequence. (" << srcPath << ")" );
				return -1;
			}
			if( srcSeq.getNbFiles() == 0 )
			{
				TUTTLE_CERR( "No existing file for the input sequence. (" << srcPath << ")" );
				return -1;
			}
			if( dstIsSeq )
			{
				TUTTLE_COUT( srcSeq.getAbsoluteStandardPattern() << " -> " << dstSeq.getAbsoluteStandardPattern() << " (" << srcSeq.getNbFiles() << ")" );
				copy_sequence( srcSeq, dstSeq );
			}
			else
			{
				TUTTLE_COUT( srcSeq.getAbsoluteStandardPattern() << " -> " << dstPath / srcSeq.getStandardPattern() << " (" << srcSeq.getNbFiles() << ")"  );
				copy_sequence( srcSeq, dstPath );
			}
		}
	}
	catch( const bfs::filesystem_error& e )
	{
		TUTTLE_CERR( e.what() );
	}
	catch(... )
	{
		TUTTLE_CERR( boost::current_exception_diagnostic_information() );
		return -1;
	}*/
	
	
	return 0;
}

