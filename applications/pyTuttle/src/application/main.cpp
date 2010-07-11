#include <QtGui/QApplication>

#include <GUI/python/QtPythonTerminal.hpp>

#include <iostream>
#include <string>

using namespace std;

int main( int argc, char** argv )
{
	QApplication qapp( argc, argv );
	
	QtPythonTerminal m_PythonWindow;
	
	/*
	QString command0("import os,sys\n");
	command0 += QString("sys.path.append(os.getenv('DUTRUC2_PYTHON_PACKAGES'))\n");
	PyRun_SimpleString(command0.toLatin1().data());

	QString command("\nfrom commandManager import *\ncmdManager = commandManager.Get()");
	PyRun_SimpleString(command.toLatin1().data());
	QString command2("\nfrom data import *\n");
	PyRun_SimpleString(command2.toLatin1().data());
	*/
	m_PythonWindow.show();
	
	qapp.exec();
	
	return 0;
}

