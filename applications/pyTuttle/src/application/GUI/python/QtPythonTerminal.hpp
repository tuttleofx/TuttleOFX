#ifndef __QTPYTHONTERMINAL_HPP__
#define __QTPYTHONTERMINAL_HPP__

#include "ui_QtPythonTerminal.h"

#include <python/EmbeddedPython.hpp>

#include <QtGui/QWidget>

class QtPythonTerminal : public QWidget
	, public PythonOutput
{
Q_OBJECT

public:
	QtPythonTerminal();
	~QtPythonTerminal();

public:
	typedef enum ELogType
	{
		eCommand,
		eOutput,
		eOutputError,
		eLogTypeSize,
	} ELogType;

public Q_SLOTS:
	void on_m_input_executeScript() { executeScript( m_ui.m_input->textToExecute() ); }

protected:
	void executeScript( const std::string in_command );

public:
	void writeCommandInHistory( const std::string& in_text );
	void writeInHistory( const char* str, const ELogType type );
	void writeInInput( const char* str );

	void writeLog( char* str, bool isError = false );
	void writeErr( char* str );
	void clearLog();

protected:
	void EmitScriptExecuted();
	void EmitScriptPathChanged( const char* path );

signals:
	void scriptExecuted();
	void scriptPathChanged( const char* );
	void scriptCommandManager();

protected:
	struct CommandAndResult
	{
		CommandAndResult( std::string command, std::string output = std::string( "" ), std::string output_error = std::string( "" ) )
			: m_command( command )
			, m_output( output )
			, m_output_error( output_error )
		{}

		CommandAndResult( const CommandAndResult& in_command )
			: m_command( in_command.m_command )
			, m_output( in_command.m_output )
			, m_output_error( in_command.m_output_error )
		{}

		CommandAndResult()
		{}

		~CommandAndResult()
		{}

		std::string m_command;      ///< la commande
		std::string m_output;       ///< sortie standard
		std::string m_output_error; ///< sortie d'erreur
	};

protected:
	EmbeddedPython m_embeddedPython;

private:
	Ui::QtPythonTerminal m_ui;
};

//
//#include <QtGui/QMenu>
//
//class QtCompleterListMenu : public QMenu
//{
//	Q_OBJECT
//public:
//	QtCompleterListMenu( std::list<std::string> comp, QWidget* parent );
//	~QtCompleterListMenu(){}
//
//};
//

#endif

