#include <QtGui/QTextCursor>

#include "QtCommandInputEdit.hpp"
#include <QtGui/QKeyEvent>
#include <QtGui/QCompleter>
#include <QtGui/QScrollBar>
#include <QtGui/QTextBlock>
#include <QtGui/QStringListModel>
#include <QtGui/QTextDocumentFragment>
#include <iostream>

QtCommandInputEdit::QtCommandInputEdit( QWidget* parent )
	: QTextEdit( parent )
	, m_completer( NULL )
	, m_navigation( 0 )
{
	initCompleter();
	setAutoFormatting( QTextEdit::AutoAll );
	setTabStopWidth( 20 );
}

void QtCommandInputEdit::scriptExecuted()
{
	m_navigation = false;
}

void QtCommandInputEdit::keyPressEvent( QKeyEvent* e )
{
	if( m_completer->popup()->isVisible() )
	{
		// The following keys are forwarded by the completer to the widget
		switch( e->key() )
		{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
				e->ignore();
				return; // let the completer do default behavior
			default:
				break;
		}
	}

	if( ( e->key() == Qt::Key_Enter ) || ( e->key() == Qt::Key_Return && e->modifiers() == Qt::ControlModifier ) )
	{
		Q_EMIT executeScript();
		scriptExecuted();
		return;
	}
	else if( e->modifiers() == Qt::ControlModifier )
	{
		switch( e->key() )
		{
			case Qt::Key_Space:
				autocomplete();
				return;
			case Qt::Key_E:
			{
				removeLine();
				return;
			}
		}
	}
	else if( e->modifiers() & Qt::MetaModifier )
	{
		switch( e->key() )
		{
			case Qt::Key_Q:
			{
				std::cout << ":::::::::::::::" << std::endl;
				std::cout << "MetaModifier::Q" << std::endl;
				QTextCursor tc = textCursor();
				std::cout << "anchor: " << tc.anchor() << std::endl;
				std::cout << "position: " << tc.position() << std::endl;
				std::cout << "selectionStart: " << tc.selectionStart() << std::endl;
				std::cout << "selectionEnd: " << tc.selectionEnd() << std::endl;
				std::cout << ":::::::::::::::" << std::endl;

				return;
			}
			case Qt::Key_Down:
				navigateCommandNext();
				return;
			case Qt::Key_Up:
				navigateCommandPrevious();
				return;
			case Qt::Key_Left:
				navigateOutputPrevious();
				return;
			case Qt::Key_Right:
				navigateOutputNext();
				return;
			case Qt::Key_A:
			{
				moveCursor( QTextCursor::StartOfBlock );
				return;
			}
			case Qt::Key_E:
			{
				moveCursor( QTextCursor::EndOfLine );
				return;
			}
			case Qt::Key_W:
			{
				moveCursor( QTextCursor::NextWord );
				return;
			}
			case Qt::Key_B:
			{
				moveCursor( QTextCursor::PreviousWord );
				return;
			}
			case Qt::Key_H:
			{
				moveCursor( QTextCursor::PreviousCharacter );
				return;
			}
			case Qt::Key_J:
			{
				moveCursor( QTextCursor::NextCell );
				return;
			}
			case Qt::Key_K:
			{
				moveCursor( QTextCursor::PreviousCell );
				return;
			}
			case Qt::Key_L:
			{
				moveCursor( QTextCursor::NextCharacter );
				return;
			}
			case Qt::Key_Less:  // add tabulation
			{
				removeTab();
				return;
			}
			case Qt::Key_Greater:  // remove tabulation
			{
				addTab();
				return;
			}
		}
	}
	else if( e->modifiers() & Qt::AltModifier && e->modifiers() & Qt::ShiftModifier ) // Alt + Shift
	{
		switch( e->key() )
		{
			case Qt::Key_Left:  // add tabulation
			{
				removeTab();
				return;
			}
			case Qt::Key_Right:  // remove tabulation
			{
				addTab();
				return;
			}
		}
	}

	QTextEdit::keyPressEvent( e );

	if( m_completer->popup()->isVisible() )
	{
		// maybe we can use the same list, but there is many particular cases
		//		if( e->key() == Qt::Key_Backspace || e->text().contains( QRegExp( "\\w" ) ) )
		//			completerUpdate( );
		//		if( e->text().contains( "." ) )
		autocomplete();      // ask to python each time...
	}
}

void QtCommandInputEdit::addTab()
{
	QTextCursor tc = textCursor();

	if( !tc.hasSelection() )  // cas simple pas de selection
	{
		tc.movePosition( QTextCursor::StartOfBlock );
		tc.insertText( "\t" );
	}
	else // if selection... need to add tabulation for each line
	{
		tc.beginEditBlock();

		int begin = tc.selectionStart();
		int end   = tc.selectionEnd();

		//// Select the correct area
		// move position and anchor to startOfBlock
		tc.setPosition( begin, QTextCursor::MoveAnchor );
		if( !tc.atBlockStart() )
			++begin; // to replace the selection at the end
		tc.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor );
		int begin_move = begin - tc.position(); // offset to compensate the StartOfBlock shift
		// move position a end
		tc.setPosition( end, QTextCursor::KeepAnchor );

		QString str = tc.selection().toPlainText();

		str.insert( 0, QString( "\t" ) );
		str.replace( QString( "\n" ), QString( "\n\t" ) );
		tc.removeSelectedText(); // replace old text by str
		tc.insertText( str );

		// put the original selection (lost by insertText())
		tc.setPosition( begin, QTextCursor::MoveAnchor );
		tc.setPosition( begin + str.size() - begin_move, QTextCursor::KeepAnchor );
		this->setTextCursor( tc );
		tc.endEditBlock();
	}
}

void QtCommandInputEdit::removeTab()
{
	QTextCursor tc = textCursor();

	if( !tc.hasSelection() )  // simple case, no selection
	{
		tc.movePosition( QTextCursor::StartOfBlock );
		tc.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
		if( tc.selectedText() == "\t" )
			tc.deleteChar();
	}
	else // if selection... need to remove tabulation for each line
	{
		tc.beginEditBlock();

		int begin = tc.selectionStart();
		int end   = tc.selectionEnd();
		int gap   = 0; // recreate the selection : shift of 1 for begin or not

		//// Select the correct area
		// move position and anchor to startOfBlock
		tc.setPosition( begin, QTextCursor::MoveAnchor );
		tc.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor );
		int begin_move = begin - tc.position(); // offset to compensate the StartOfBlock shift
		// move position a end
		tc.setPosition( end, QTextCursor::KeepAnchor );

		QString str = tc.selection().toPlainText();
		if( str[0] == '\t' )
		{
			str.remove( 0, 1 );
			if( begin_move )
				++gap;
		}
		str.replace( QString( "\n\t" ), QString( "\n" ) );
		tc.removeSelectedText(); // replace old text by str
		tc.insertText( str );

		// put the original selection (lost by insertText())
		tc.setPosition( begin - gap, QTextCursor::MoveAnchor );
		tc.setPosition( begin + str.size() - begin_move, QTextCursor::KeepAnchor );
		this->setTextCursor( tc );
		tc.endEditBlock();
	}
}

void QtCommandInputEdit::removeLine()
{
	QTextCursor tc = textCursor();

	tc.beginEditBlock();
	if( !tc.hasSelection() )  // simple case, no selection
	{
		tc.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor );
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
		tc.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
		//		tc.movePosition( QTextCursor::NextBlock, QTextCursor::KeepAnchor );
	}
	else
	{
		int begin = tc.selectionStart();
		int end   = tc.selectionEnd();
		tc.setPosition( begin, QTextCursor::MoveAnchor );
		tc.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor );
		tc.setPosition( end, QTextCursor::KeepAnchor );
		tc.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
		tc.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
		//		tc.movePosition( QTextCursor::NextBlock, QTextCursor::KeepAnchor );
	}
	//	tc.select( QTextCursor::BlockUnderCursor );
	tc.removeSelectedText();
	tc.endEditBlock();
}

void QtCommandInputEdit::setCompleterList( QStringList& words )
{
	m_completer->setModel( new QStringListModel( words, m_completer ) );

	completerUpdate();
}

void QtCommandInputEdit::completerUpdate()
{
	QString completionPrefix = wordUnderCursor();

	//	std::cout << "completionPrefix: " << completionPrefix.toStdString( ) << std::endl;
	//	std::cout << "m_completer->completionPrefix(): " << m_completer->completionPrefix( ).toStdString( ) << std::endl;

	//	if( completionPrefix != m_completer->completionPrefix( ) )
	//	{
	m_completer->setCompletionPrefix( completionPrefix );

	QTextCursor tc = textCursor();
	tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, completionPrefix.size() );

	// Specify the size or use the widget width
	QRect cr = cursorRect( tc );
	cr.setWidth( m_completer->popup()->sizeHintForColumn( 0 ) + m_completer->popup()->verticalScrollBar()->sizeHint().width() );

	m_completer->complete( cr ); // popup it up!
	//	}
}

void QtCommandInputEdit::initCompleter()
{
	m_completer = new QCompleter( this );

	m_completer->setWidget( this );
	m_completer->setCompletionMode( QCompleter::PopupCompletion );
	m_completer->setModelSorting( QCompleter::CaseInsensitivelySortedModel );
	m_completer->setCaseSensitivity( Qt::CaseInsensitive );
	m_completer->setWrapAround( false );
	QObject::connect( m_completer, SIGNAL( activated( const QString & ) ), this, SLOT( insertCompletion( const QString & ) ) );
}

void QtCommandInputEdit::insertCompletion( const QString& completion )
{
	if( m_completer->widget() != this )
		return;

	int extra = completion.length() - m_completer->completionPrefix().length();
	if( extra == 0 ) // if the completion is full
		return;

	QTextCursor tc = textCursor();
	tc.beginEditBlock();
	//	tc.movePosition( QTextCursor::Left );
	QTextCursor tmp = textCursor();
	tmp.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
	if( tmp.selectedText().contains( QRegExp( "\\w" ) ) ) // if the next character if a word type
		tc.movePosition( QTextCursor::EndOfWord, QTextCursor::KeepAnchor ); // replace to the end of the word
	tc.insertText( completion.right( extra ) );
	setTextCursor( tc );
	tc.endEditBlock();
}

const char* QtCommandInputEdit::textToExecute()
{
	QTextCursor textCursor = this->textCursor();

	if( textCursor.hasSelection() )
		return ( char* )( textCursor.selectedText().toLatin1().data() );
	else
		return ( char* )( this->toPlainText().toLatin1().data() );
}

QString QtCommandInputEdit::charUnderCursor() const
{
	QTextCursor tc = textCursor();

	tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
	QString str = tc.selectedText();
	return str;
}

QString QtCommandInputEdit::wordUnderCursor() const
{
	QTextCursor tc = textCursor();
	int tc_pos     = tc.columnNumber();

	tc.select( QTextCursor::LineUnderCursor );

	QString str = tc.selectedText();
	str.truncate( tc_pos );
	//	QStringList list = str.split( QRegExp( "\\s+" ) ); // cut on spaces
	QStringList list = str.split( QRegExp( "[^\\w\\.]" ) ); // cut if it's neither an alphabetic character nor '.'

	if( list.isEmpty() )
		return QString( "" );

	return list.last();
}

std::string QtCommandInputEdit::wordUnderCursor_std() const
{
	return wordUnderCursor().toStdString();
}

void QtCommandInputEdit::focusInEvent( QFocusEvent* e )
{
	if( m_completer )
		m_completer->setWidget( this );
	QTextEdit::focusInEvent( e );
}

bool QtCommandInputEdit::navigateNext()
{
	if( !m_navigation )
	{
		//		std::cout << "navigateCommandNext - no navigation" << std::endl;
		return false;
	}
	++m_navigation_iterator;
	if( m_navigation_iterator == m_embeddedPython->m_commands.end() )
	{
		//		std::cout << "navigateCommandNext - list end" << std::endl;
		m_navigation = false;
		setTextColor( QColor( 0, 0, 0 ) );
		setPlainText( m_navigation_saveCommand );
		return false;
	}
	return true;
}

bool QtCommandInputEdit::navigatePrevious()
{
	if( !m_navigation )  // not currently in the history
	{
		//		std::cout << "navigateCommandPrevious - begin history" << std::endl;
		m_navigation             = true;
		m_navigation_saveCommand = toPlainText();
		m_navigation_iterator    = m_embeddedPython->m_commands.end();
	}
	if( m_navigation_iterator == m_embeddedPython->m_commands.begin() )
	{
		//		std::cout << "navigateCommandPrevious - list begining" << std::endl;
		return false;
	}
	//	std::cout << "navigateCommandPrevious" << std::endl;
	--m_navigation_iterator;
	return true;
}

void QtCommandInputEdit::navigateCommandNext()
{
	if( navigateNext() )
	{
		// std::cout << "navigateCommandNext" << std::endl;
		setTextColor( QColor( 0, 0, 0 ) );
		setPlainText( ( *m_navigation_iterator ).m_command.c_str() );
	}
}

void QtCommandInputEdit::navigateCommandPrevious()
{
	if( navigatePrevious() )
	{
		setTextColor( QColor( 0, 0, 0 ) );
		setPlainText( ( *m_navigation_iterator ).m_command.c_str() );
		//	QTextCursor tc = textCursor( );
		//	tc.insertText( m_navigation_iterator.m_command.c_str( ) );
	}
}

void QtCommandInputEdit::navigateOutputNext()
{
	if( navigateNext() )
	{
		// std::cout << "navigateCommandNext" << std::endl;
		setTextColor( QColor( 0, 0, 0 ) );
		setPlainText( ( *m_navigation_iterator ).m_output.c_str() );
	}
}

void QtCommandInputEdit::navigateOutputPrevious()
{
	if( navigatePrevious() )
	{
		setTextColor( QColor( 0, 0, 0 ) );
		setPlainText( ( *m_navigation_iterator ).m_output.c_str() );
		//	QTextCursor tc = textCursor( );
		//	tc.insertText( m_navigation_iterator.m_command.c_str( ) );
	}
}

void QtCommandInputEdit::autocomplete()
{
	std::string text = wordUnderCursor_std();

	//	std::cout << "autocomplete : " << text << std::endl;
	std::list<std::string> comp = m_embeddedPython->getAutocompletion( text );

	if( comp.size() == 0 )
		return;

	QStringList wordList;
	std::list<std::string>::iterator it     = comp.begin();
	std::list<std::string>::iterator it_end = comp.end();
	for(; it != it_end; it++ )
	{
		//		std::cout << "AUTOCOMPLETE: " << *it << std::endl;
		wordList.append( ( *it ).c_str() );
	}
	setCompleterList( wordList );
	/*
	   QListView *listBox;
	   listBox->clear();
	   listBox->insertStringList( wordList );
	   QPoint point = textCursorPoint();
	   //	adjustListBoxSize(qApp->desktop()->height() - point.y(), width() / 2);
	   listBox->move(point);
	   listBox->show();
	   listBox->raise();
	   listBox->setActiveWindow();
	 */
}

