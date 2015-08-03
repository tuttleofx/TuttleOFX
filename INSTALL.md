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
