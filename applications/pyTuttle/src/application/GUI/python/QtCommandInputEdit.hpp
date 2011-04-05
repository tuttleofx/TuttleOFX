#ifndef C_INPUT_EDIT_H
#define C_INPUT_EDIT_H

#include <python/EmbeddedPython.hpp>

#include <string>

#include <QtGui/QTextEdit>
#include <QtGui/QCompleter>

class QtCommandInputEdit : public QTextEdit
{
Q_OBJECT

public:
	QtCommandInputEdit( QWidget* parent = 0 );

public:
	void        setEmbeddedPython( EmbeddedPython* python ) { m_embeddedPython = python; }
	const char* textToExecute();
	QString     charUnderCursor() const;
	QString     wordUnderCursor() const;
	std::string wordUnderCursor_std() const;

signals:
	void executeScript();

private:
	void scriptExecuted();

protected:
	void keyPressEvent( QKeyEvent* e );
	void focusInEvent( QFocusEvent* e );

protected:
	bool navigateNext();
	bool navigatePrevious();
	void navigateCommandNext();
	void navigateCommandPrevious();
	void navigateOutputNext();
	void navigateOutputPrevious();

	void autocomplete();

protected:
	void addTab();
	void removeTab();
	void removeLine();

private Q_SLOTS:
	void insertCompletion( const QString& completion );

private:
	void setCompleterList( QStringList& words );
	void completerUpdate();
	void initCompleter();

private:
	EmbeddedPython* m_embeddedPython;
	QCompleter* m_completer;
	std::list<EmbeddedPython::CommandAndResult>::iterator m_navigation_iterator; // Lorsque l'on navigue dans l'historique des CommandAndResult
	QString m_navigation_saveCommand; // Lorsque l'on navigue dans l'historique, on garde la commande en cours
	bool m_navigation; // True : lorsque l'on navigue dans l'historique, False sinon
};

#endif //C_INPUT_EDIT_H

