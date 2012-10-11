#ifndef _SAM_DO_NODE_DUMMY_HPP_
#define	_SAM_DO_NODE_DUMMY_HPP_

#include <sam/common/options.hpp>

#include <boost/program_options.hpp>

#include "global.hpp"

#define READER_DUMMY_SHORTNAME "r"
#define READER_DUMMY_NAME      "reader"
#define READER_DUMMY_FULLNAME  "dummy.reader"

#define WRITER_DUMMY_SHORTNAME "w"
#define WRITER_DUMMY_NAME      "writer"
#define WRITER_DUMMY_FULLNAME  "dummy.writer"

namespace bpo = boost::program_options;

namespace sam {
namespace samdo {

typedef std::vector<ttl::ofx::imageEffect::OfxhImageEffectPlugin*> NodeList;
typedef ttl::ofx::imageEffect::OfxhImageEffectPlugin::ContextSet   NodeContext;

struct Dummy
{
	void getFullName( std::string& inputNode );
	
	bpo::options_description getInfoOptions();
	bpo::options_description getConfOptions();
	bpo::options_description getOpenFXOptions();
	
	void getCommandLineParameters    ( bpo::variables_map& node_vm, const std::vector<std::string>& nodeArgs );
	void getParametersFromCommandLine( std::vector<std::string>& parameters, const std::vector<std::string>& nodeArgs );
	void getPathsFromCommandLine     ( std::vector<std::string>& paths, const std::vector<std::string>& nodeArgs );
	void getExtensionsFromCommandLine( std::vector<std::string>& extensions, const std::vector<std::string>& nodeArgs );
	
	void addDummyNodeInList( std::vector<std::string>& list );
	void addFullNameDummyNodeInList( std::vector<std::string>& list );
	void addShortNameDummyNodeInList( std::vector<std::string>& list );
	
	std::vector<std::string> getAllSupportedNodes( const std::string& context );
	std::vector<std::string> getSupportedExtensions( const std::string& context );
	
	void printAllSupportedNodes( const std::string& context );
	void printAllSupportedExtensions( const std::string& context );
	
	void displayHelp( const std::string& nodeFullName, const bpo::options_description &infoOptions, const bpo::options_description &confOptions );
	void displayHelp( const std::string& nodeFullName, const bpo::options_description &infoOptions, const bpo::options_description &confOptions, const bpo::options_description &expertOptions );
	
	void foundAssociateSpecificDummyNode( std::string& inputNode, const std::string& dummyNodeName, const NodeList& nodeList, const std::vector<std::string>& nodeArgs );
	void foundAssociateDummyNode( std::string& inputNode, const std::vector<ttl::ofx::imageEffect::OfxhImageEffectPlugin*>& nodeList, const std::vector<std::string>& nodeArgs );
};

}
}
#endif
