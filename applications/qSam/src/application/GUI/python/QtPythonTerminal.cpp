#include "QtPythonTerminal.hpp"

#include <QtGui/QMenu>
#include <QtGui/QListView>

#include <iostream>
#include <boost/algorithm/string.hpp>

QtPythonTerminal::QtPythonTerminal()
//	: m_command_iterator( NULL )
//	, m_output_iterator ( NULL )
{
	m_ui.setupUi( this );

	// register to python as listener...
	m_embeddedPython.addPythonOutput( this );
	m_ui.m_input->setEmbeddedPython( &m_embeddedPython );
}

QtPythonTerminal::~QtPythonTerminal() {}

void QtPythonTerminal::executeScript( const std::string in_command )
{
	//	std::cout << "in_command:" << in_command << std::endl;

	// write command in "m_history"
	writeCommandInHistory( in_command );

	// execute command
	int ret = m_embeddedPython.pyRun_SimpleString( in_command );

	// empty text box "m_input" if no selection
	QTextCursor textCursor = m_ui.m_input->textCursor();
	if( !ret && !textCursor.hasSelection() )
		m_ui.m_input->clear();

	// warns that the script has been executed
	EmitScriptExecuted();
}

void QtPythonTerminal::writeCommandInHistory( const std::string& in_text )
{
	m_ui.m_history->setTextColor( QColor( 0, 0, 255 ) );
	QString command = "> ";
	command += in_text.c_str();
	command.replace( "\n", "\n> " );
	command += "\n";
	writeInHistory( command.toLatin1().data(), eCommand );
	m_ui.m_history->setTextColor( QColor( 0, 0, 0 ) );
}

void QtPythonTerminal::writeInHistory( const char* str, const ELogType type )
{
	const QColor colorCommand( 0, 0, 255 );
	const QColor colorOutput( 0, 0, 0 );
	const QColor colorOutputError( 255, 0, 0 );

	switch( type )
	{
		case eCommand:
			m_ui.m_history->setTextColor( colorCommand );
			break;
		case eOutputError:
			m_ui.m_history->setTextColor( colorOutputError );
			break;
		case eOutput:
		default:
			m_ui.m_history->setTextColor( colorOutput );
			break;
	}

	m_ui.m_history->append( str ); //insert
}

void QtPythonTerminal::writeInInput( const char* str )
{
	m_ui.m_input->setTextColor( QColor( 0, 0, 0 ) );
	m_ui.m_input->setPlainText( str );
}

void QtPythonTerminal::writeLog( char* str, bool isError )
{
	using namespace boost::algorithm;
	std::string s = str;

	std::string is_empty = s;
	boost::algorithm::erase_all( is_empty, " " );
	boost::algorithm::erase_all( is_empty, "\t" );
	boost::algorithm::erase_all( is_empty, "\n" );
	if( is_empty.size() == 0 )
		return;

	//	CommandAndResult& current_command = m_commands.back( );
	//	std::cout << "_________________________________" << std::endl;
	//	std::cout << "writeLog : " << s << std::endl;

	//	current_command.m_output += s;
	//	current_command.m_output += "\n";

	ELogType type = isError ? eOutputError : eOutput;
	writeInHistory( str, type );
}

void QtPythonTerminal::writeErr( char* str )
{
	//	CommandAndResult& current_command = m_commands.back( );
	std::cout << "writeErr : " << str << std::endl;

	//	current_command.m_output_error += str;
	//	current_command.m_output_error += "\n";

	writeInHistory( str, eOutputError );
}

void QtPythonTerminal::clearLog()
{
	m_ui.m_history->clear();
}

void QtPythonTerminal::EmitScriptExecuted()
{
	Q_EMIT scriptExecuted();
}

void QtPythonTerminal::EmitScriptPathChanged( const char* path )
{
	//	setScriptPath( path );
	Q_EMIT scriptPathChanged( path );
}

/*
   QStringList QtPythonTerminal::autocompleteCommand( std::string& cmd )
   {
    char run[255];
    int n =0;
    QStringList list;
    resultString="";
    do {
        snprintf(run,255,"print completer.complete(\"%s\",%d)\n",cmd.ascii(),n);
        PyRun_SimpleString(run);
        resultString=resultString.stripWhiteSpace(); //strip trialing newline
        if (resultString!="None")
        {
            list.append(resultString);
            resultString="";
        }
        else
        {
            resultString="";
            break;
        }
        n++;
    } while (true);
    return list;
   }
 */

