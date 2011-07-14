#include <QtGui/QApplication>

#include <GUI/python/QtPythonTerminal.hpp>

#include <iostream>
#include <string>

int main( int argc, char** argv )
{
	QApplication qapp( argc, argv );

	QtPythonTerminal m_PythonWindow;
	m_PythonWindow.show();

	qapp.exec();

	return 0;
}

