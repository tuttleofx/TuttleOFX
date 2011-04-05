#include "EmbeddedPython.hpp"

#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

void EmbeddedPythonBase::initialize()
{
	Py_Initialize();

	this->pyRun_SimpleString(
	    "#-*- coding: utf-8 -*- \n" // using UTF-8 encoding
	    "import sys\n"              //
	    "import os\n"               //
	    "import rlcompleter\n"      // for completion
	    "completer = rlcompleter.Completer()\n"
	    );
	setvbuf( stdout, (char*) NULL, _IONBF, BUFSIZ );
}

void EmbeddedPythonBase::uninitialize()
{
	this->pyRun_SimpleString( "sys.stderr = 0" );
	this->pyRun_SimpleString( "sys.stdout = 0" );
	Py_Finalize();
}

int EmbeddedPythonBase::pyRun_SimpleString( const std::string in_command )
{
	PyRun_SimpleString( in_command.c_str() );
	return 0;
}

int EmbeddedPythonBase::pyRun_SimpleFile( const std::string in_fileName )
{
	FILE* file = fopen( in_fileName.c_str(), "r" /*r+*/ );

	PyRun_SimpleFile( file, in_fileName.c_str() );
	if( file != NULL )
		fclose( file ); // needed ?
	return 0;
}

//______________________________________________________________________________

void EmbeddedPython::writeLog( char* str, bool isError )
{
	CommandAndResult& current_command = m_commands.back();

	//	std::cout << "writeLog : " << str << std::endl;

	current_command.m_output += str;
	current_command.m_output += "\n";
}

void EmbeddedPython::writeErr( char* str )
{
	CommandAndResult& current_command = m_commands.back();

	//	std::cout << "writeErr : " << str << std::endl;

	current_command.m_output_error += str;
	current_command.m_output_error += "\n";
}

void EmbeddedPython::clearLog()
{
	//	std::cout << "clearLog" << std::endl;
}

std::list<std::string> EmbeddedPython::getAutocompletion( const std::string in_cmd )
{
	int n = 0;

	std::list<std::string> autocompletion_list;
	m_pythonRedirect.enableSimpleCommand( true );
	std::string result;
	do
	{
		std::ostringstream run;
		run << "print completer.complete('''" << in_cmd << "'''," << n << ")";
		this->pyRun_SimpleString( run.str().c_str() );
		result = m_pythonRedirect.getSimpleCommandResult();
		boost::algorithm::trim( result );

		if( result.size() == 0 || result == "None" )
		{
			break;
		}
		autocompletion_list.push_back( result );
	}
	while( ++n < 100 );
	m_pythonRedirect.enableSimpleCommand( false );
	return autocompletion_list;
}

/*
 #completer = rlcompleter.Completer()
   texte = 'p'
   id = 0
   i = True
   while True :
    i = completer.complete( texte, id )
    id = id + 1
    if i == None :
        break
    print i
 */

