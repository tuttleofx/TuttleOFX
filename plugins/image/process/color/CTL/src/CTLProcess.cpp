#include <CtlSimdInterpreter.h>
#include <CtlModuleSet.h>
#include <CtlModule.h>
#include <CtlLContext.h>
#include <CtlSymbolTable.h>
#include <CtlParser.h>
#include <CtlExc.h>
//

#include "CTLPlugin.hpp"
#include "CTLDefinitions.hpp"
#include "CTLProcess.hpp"

// Ctl code
#if 0
    #include <iostream>
    #define debug(x) (cout << x << endl)
#else
    #define debug(x)
#endif

namespace Ctl {
using namespace std;
using namespace Iex;
using namespace IlmThread;
// we redeclare locally... (because in lib CTL, it's declared into a cpp file)
struct Interpreter::Data
{
    SymbolTable		symtab;
    ModuleSet		moduleSet;
    Mutex		mutex;
};
}

namespace tuttle {
namespace plugin {
namespace ctl {

using namespace std;
using namespace Iex;
using namespace IlmThread;
using namespace Ctl;

void loadModule( Ctl::Interpreter& interpreter, const std::string &moduleName, const std::string& code )
{
    debug ("Interpreter::loadModule (moduleName = " << moduleName << ")");

    Lock lock( interpreter._data->mutex );
    loadModuleRecursive( interpreter, moduleName, code );
}


void loadModuleRecursive( Ctl::Interpreter& interpreter, const std::string &moduleName, const std::string& code )
{
    debug ("Interpreter::loadModuleRecursive "
	   "(moduleName = " << moduleName << ")");

    if( interpreter.moduleIsLoadedInternal(moduleName) )
    {
	debug ("\talready loaded");
	return;
    }

    //
    // Input source code of the module comes from a string instead of a file.
    //
	const std::string fileName = moduleName;
    std::istringstream codeStream( code );

    debug ("\tloading from code \"" << fileName << "\"");

    Module *module = 0;
    LContext *lcontext = 0;

    try
    {
	//
	// Create a Module, an Lcontext and a Parser
	//

	module = interpreter.newModule (moduleName, fileName);	
	interpreter._data->moduleSet.addModule (module);
	lcontext = interpreter.newLContext (codeStream, module, interpreter._data->symtab);
	Parser parser (*lcontext, interpreter);

	//
	// Parse the source code and generate executable code
	// for the module
	//

	debug ("\tparsing input");
	SyntaxNodePtr syntaxTree = parser.parseInput ();

	if (syntaxTree && lcontext->numErrors() == 0)
	{
	    debug ("\tgenerating code");
	    syntaxTree->generateCode (*lcontext);
	}

	if (lcontext->numErrors() > 0)
	{
	    lcontext->printDeclaredErrors();
	    THROW (LoadModuleExc,
		   "Failed to load CTL module \"" << moduleName << "\".");
	}

	//
	// Run the module's initialization code
	//

	debug ("\trunning module initialization code");
	module->runInitCode();

	//
	// Cleanup: the LContext and the module's local symbols
	// are no longer needed, but we keep the global symbols.
	//

	debug ("\tcleanup");
	delete lcontext;
	interpreter._data->symtab.deleteAllLocalSymbols (module);
    }
    catch (...)
    {
	//
	// Something went wrong while loading the module, clean up
	//

	delete lcontext;
	interpreter._data->symtab.deleteAllSymbols (module);
	interpreter._data->moduleSet.removeModule (moduleName);
	throw;
    }
}


}
}
}
