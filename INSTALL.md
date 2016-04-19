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

Custom Makefile target:
* ofxPlugins: build all the OpenFX plugins.
* ofxIO: build readers and writers plugins.
* ofxDisplay: build plugins to view images as a color cube, an histogram, or in a simple GL view port.
* ofxGenerator: build plugins to generate basic inputs (checkerboard, bars, wheel...), images based on procedural geometry, or burn text on images.
* ofxProcess: build to process mathematics operations on color or geometry of images.
* ofxAnalysis: build to analyse (luminosity, channel values, Kurtosis, Skewness...) and compare images (psnr...).
* ofxDebug: build to display information about the host and all OpenFX plugins available.

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

* Extra logs.
* Initialize image buffers in red to highlight uninitialized pixels.
* Export "*.dot" files during the process with user graph and internal process graph in the current folder.
* Save all computed nodes as png files in the current folder.

