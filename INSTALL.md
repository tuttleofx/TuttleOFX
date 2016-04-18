# TuttleOFX


## External dependencies

**The only required dependency is boost.**
Other dependencies offer more plugins. See how to install them on your platform:

 * [Ubuntu](INSTALL-ext-ubuntu.md)
 * [Fedora](INSTALL-ext-fedora.md)
 * [MacOS](INSTALL-ext-macos.md)
 * [Windows](INSTALL-ext-windows.md)


## Compilation 

```bash
./configure
make
make install
```


## Additionnal dependencies for sam

* __clint__

Clint is a module filled with a set of awesome tools for developing commandline applications.

https://pypi.python.org/pypi/clint/ <br/>
Suggested version: v0.4.1

* __argcomplete__

Argcomplete provides easy, extensible command line tab completion of arguments for your Python script.

https://pypi.python.org/pypi/argcomplete <br/>
Suggested version: v0.9.0


## Tested compilers

- Linux
 + GCC 4.1 on OpenSUSE 11.2 64bits
 + GCC 4.4 on OpenSUSE 11.2 64bits
 + GCC 4.6 on Ubuntu 12.04 64bits
 + GCC 4.8 on Ubuntu 14.04 64bits
 + Clang 3.4 on Ubuntu 12.04 64bits
- MacOS
 + Clang 4.2 64bits
- Windows
 + Visual C++ 2010 on Windows XP 32bits
 + GCC-MINGW 3.4.5, 4.3.0 on Windows XP 32bits


## Launch tests

Some test (both C++ and python) need input images to work. You can get them from TuttleOFX-data repository:

```
git clone https://github.com/tuttleofx/TuttleOFX-data.git
```

- C++ tests

Tests of host ( __libraries/tuttle/tests__ ) and plugins ( __tests__ directory in each plugin, with test files prefixed by "plugin_") with Boost Unit Test Framework.
After compiling the project, executables are in __testBin__ directory.

- Python tests

Test of host ( __libraries/tuttle/pyTest__ ) with nosetests tool.

```
nosetests libraries/tuttle/pyTest
```


## Build in debug

If you build TuttleOFX in debug (build option __-DCMAKE_BUILD_TYPE=Debug__), it enables the following host features:

* Create "*.dot" file during the process to get informations about the graph.
* Log several times during the process.
* Output all nodes in the graph as png.
* Initialize output buffer of all nodes with red to highlight uninitialized pixels.
