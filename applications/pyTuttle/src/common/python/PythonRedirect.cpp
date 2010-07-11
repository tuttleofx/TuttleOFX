#include "PythonRedirect.hpp"

#include <string>

//#ifdef BUILD_PYTHON

//PythonLog* PythonRedirect::m_log = NULL;

std::vector<PythonOutput*> PythonRedirect::m_pythonOutput;
bool                       PythonRedirect::m_python_simpleCommand = 0;
std::string                PythonRedirect::m_python_simpleCommandResult;

PythonRedirect::PythonRedirect( )
{
	
}

PythonRedirect::~PythonRedirect( )
{

}

/**
 * @brief La classe qui doit etre appelee pour les differentes redirections.
 */
void PythonRedirect::addPythonOutput( PythonOutput* inout_pythonLog )
{
	m_pythonOutput.push_back( inout_pythonLog );
}

/**
 * @brief Initialise les redirections.
 *        Need Python to be intialized
 */
bool PythonRedirect::init( )
{
	initLogFunctions( );

	Py_InitModule( (char*) packageName( ), &m_embMethods[0] );

	std::string importCmd;

	importCmd = "import sys\n";
	importCmd += "sys.path.append('./tuttlePython/')\n";

	importCmd += "import os\n"
		"pwd = os.getcwd()\n";

	importCmd += "import ";
	importCmd += packageName( );
	importCmd += "\n";

	// redirection des messages de python
	importCmd += "class Redirect:\n",
		importCmd += "	def write(self, str):\n",
		importCmd += std::string( "		" ) + std::string( packageName( ) ) + std::string( ".writeLog(str)\n\n" );
	importCmd += "	def isatty(self):\n",
		importCmd += "		pass\n",
		importCmd += "class Redirect_err:\n",
		importCmd += "	def write(self, str):\n",
		importCmd += std::string( "		" ) + std::string( packageName( ) ) + std::string( ".writeErr(str)\n\n" );
	importCmd += "	def isatty(self):\n",
		importCmd += "		pass\n",
		importCmd += "sys.stdout = Redirect()\n";
	importCmd += "sys.stderr = Redirect_err()\n";

	int retVal = PyRun_SimpleString( importCmd.c_str( ) );

	return retVal;
}

/**
 * @brief Le nom avec lequel une librairie exportee pour Python est disponible pour cette classe.
 */
const char * PythonRedirect::packageName( ) const
{
	return "PythonRedirect";
	//	return "myApp";
}

bool PythonRedirect::registerPyMethods( char * name, PyCFunction method, int flags, char * doc )
{
	PyMethodDef newMeth;
	newMeth.ml_name = name;
	newMeth.ml_meth = method;
	newMeth.ml_flags = flags;
	newMeth.ml_doc = doc;

	m_embMethods.push_back( newMeth );
	return true;
}

PyObject* PythonRedirect::emb_writeLog( PyObject * /*self*/, PyObject *args )
{
	char * str;
	PyArg_ParseTuple( args, "s", &str );

	if( m_python_simpleCommand )
	{
		m_python_simpleCommandResult += str;
	}
	else
	{
		std::vector<PythonOutput*>::iterator it     = m_pythonOutput.begin( );
		std::vector<PythonOutput*>::iterator it_end = m_pythonOutput.end( );
		for(; it != it_end; it++ )
			(*it)->writeLog( str );
	}
	
	return Py_BuildValue( "s", args );
}

PyObject* PythonRedirect::emb_writeErr( PyObject * /*self*/, PyObject *args )
{
	char * str;
	PyArg_ParseTuple( args, "s", &str );

	if( m_python_simpleCommand )
	{
		m_python_simpleCommandResult = "";
	}
	else
	{
		std::vector<PythonOutput*>::iterator it     = m_pythonOutput.begin( );
		std::vector<PythonOutput*>::iterator it_end = m_pythonOutput.end( );
		for(; it != it_end; it++ )
			(*it)->writeErr( str );
	}
	
	return Py_BuildValue( "s", args );
}

PyObject* PythonRedirect::emb_clearLog( PyObject * /*self*/, PyObject * /*args*/ )
{
	std::vector<PythonOutput*>::iterator it     = m_pythonOutput.begin( );
	std::vector<PythonOutput*>::iterator it_end = m_pythonOutput.end( );
	for(; it != it_end; it++ )
		(*it)->clearLog();

	return Py_BuildValue( "" );
}

bool PythonRedirect::initLogFunctions( )
{
	registerPyMethods( "writeLog", emb_writeLog, METH_VARARGS, "Write the string to the Log window" );
	registerPyMethods( "writeErr", emb_writeErr, METH_VARARGS, "Write the string to the Log window" );
	registerPyMethods( "clearLog", emb_clearLog, METH_VARARGS, "Clear the Log window" );
	registerPyMethods( NULL, NULL, 0, NULL );

	return true;
}


//#endif

