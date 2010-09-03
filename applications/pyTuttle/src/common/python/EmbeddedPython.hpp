#ifndef _EMBEDDEDPYTHON_HPP
#define _EMBEDDEDPYTHON_HPP

#include "PythonRedirect.hpp"

#include <Python.h>

#include <iostream>
#include <string>
#include <list>

/**
 * @brief Centralised Python interface
 *
 * @todo launch commands or scripts inside different python contexts
 * @todo threads creation
 */
class EmbeddedPythonBase
{

public:
	EmbeddedPythonBase()
	{
		initialize();
	}

	virtual ~EmbeddedPythonBase()
	{
		uninitialize();
	}

private:
	void initialize();
	void uninitialize();

public:
	virtual int pyRun_SimpleString( const std::string in_command );
	virtual int pyRun_SimpleFile( const std::string in_fileName );
	//	std::list<std::string> auto_complete( const std::string& in_cmd );

};

//______________________________________________________________________________

/**
 * @brief Centralised Python interface
 *        Save commands history
 */
class EmbeddedPython : public EmbeddedPythonBase
	, public PythonOutput
{
public:
	EmbeddedPython() : EmbeddedPythonBase()
		, PythonOutput()
	{
		m_pythonRedirect.init();
		m_pythonRedirect.addPythonOutput( this );
	}

	virtual ~EmbeddedPython() {}

public:
	int pyRun_SimpleString( const std::string in_command )
	{
		m_pythonRedirect.clearSimpleCommandResult();
		m_commands.push_back( CommandAndResult( in_command ) );
		//		std::cout << "command: " << m_commands.back().m_command << std::endl;
		return EmbeddedPythonBase::pyRun_SimpleString( in_command );
	}

	int pyRun_SimpleFile( const std::string in_fileName )
	{
		m_pythonRedirect.clearSimpleCommandResult();
		//m_commands.push_back( CommandAndResult( in_fileName ) ); // mettre le contenu du fichier ??
		return EmbeddedPythonBase::pyRun_SimpleFile( in_fileName );
	}

	std::list<std::string> getAutocompletion( const std::string in_cmd );

public:
	void addPythonOutput( PythonOutput* inout_pythonLog )
	{
		m_pythonRedirect.addPythonOutput( inout_pythonLog );
	}

	void writeLog( char* str, bool isError = false );
	void writeErr( char* str );
	void clearLog();

public:
	struct CommandAndResult
	{

		CommandAndResult( std::string command, std::string output = std::string( "" ), std::string output_error = std::string( "" ) )
			: m_command( command )
			, m_output( output )
			, m_output_error( output_error ) {}

		CommandAndResult( const CommandAndResult& in_command )
			: m_command( in_command.m_command )
			, m_output( in_command.m_output )
			, m_output_error( in_command.m_output_error ) {}

		CommandAndResult() {}

		~CommandAndResult() {}

		std::string m_command; ///< command
		std::string m_output; ///< standard output
		std::string m_output_error; ///< erreur output
	};

	std::list<CommandAndResult> m_commands; ///< list of commands with their result

protected:
	PythonRedirect m_pythonRedirect;
};

/*
   // sub classe pour les threads
   class EmbeddedPython
   {
    public:
        class PythonThread
        {
            PythonThread(){}
            ~PythonThread(){}

        };
        std::vector<PythonThread> m_threadsList;
   };
 */

/*
   class EmbeddedPythonSingleton : public EmbeddedPython, public Singleton<EmbeddedPythonSingleton>
   {
    SINGLETON_BLABLA
   public:
    EmbeddedPythonSingleton(){};
    ~EmbeddedPythonSingleton(){};
   public:
   //	static EmbeddedPythonSingleton& get()
   //	{
   //		return *this;
   //	}
   };
 */

#endif

