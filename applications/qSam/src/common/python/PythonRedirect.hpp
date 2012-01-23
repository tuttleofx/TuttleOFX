#ifndef _PYTHONREDIRECT_HPP_
#define _PYTHONREDIRECT_HPP_

#define BUILD_PYTHON 1

#ifdef BUILD_PYTHON
 #include <Python.h>
#endif

#include <vector>
#include <string>

class PythonOutput
{
public:
	PythonOutput() {}
	virtual ~PythonOutput() {}

public:
	virtual void writeLog( char* str, bool isError = false ) = 0;
	virtual void writeErr( char* str )                       = 0;
	virtual void clearLog()                                  = 0;
};

/**
 * @brief Redirect python outputs into PythonLog subclasses
 */
class PythonRedirect
{
public:
	PythonRedirect();
	virtual ~PythonRedirect();

	//#ifdef BUILD_PYTHON
	bool init();
	#ifndef SWIG
	void               addPythonOutput( PythonOutput* inout_pythonLog );
	inline void        enableSimpleCommand( bool b )  { m_python_simpleCommand = b; }
	inline void        clearSimpleCommandResult()     { m_python_simpleCommandResult = ""; }
	inline std::string getSimpleCommandResult() const { return m_python_simpleCommandResult; }
	#endif

protected:
	#ifndef SWIG
	virtual bool        initLogFunctions();
	virtual const char* packageName() const;

	bool registerPyMethods( char* name, PyCFunction method, int flag, char* doc );

	static PyObject* emb_writeLog( PyObject* self, PyObject* args );
	static PyObject* emb_writeErr( PyObject* self, PyObject* args );
	static PyObject* emb_clearLog( PyObject* self, PyObject* args );
	#endif

protected:
	#ifndef SWIG
	static std::vector<PythonOutput*> m_pythonOutput;

	static bool m_python_simpleCommand;
	static std::string m_python_simpleCommandResult;

	std::vector<PyMethodDef> m_embMethods;
	#endif
	//#endif
};

#endif

