TuttleOFX
=========


# External dependencies

**The only required dependency is boost.**
Other dependencies offer more plugins. See how to install them on your platform:

 * [Ubuntu](INSTALL-ext-ubuntu.md)
 * [Fedora](INSTALL-ext-fedora.md)
 * [MacOS](INSTALL-ext-macos.md)
 * [Windows](INSTALL-ext-windows.md)


# Compilation 

```bash
./configure
make
make install
```

# Launch tests

Some test (both C++ and python) need input images to work. You can get them from TuttleOFX-data repository:
```
git clone https://github.com/tuttleofx/TuttleOFX-data.git
```

* C++ tests
Tests of host (```libraries/tuttle/tests```) and plugins (```tests``` directory in each plugin, with test files prefixed by "plugin_") with Boost Unit Test Framework.
After compiling the project, executables are in ```testBin``` directory.

* Python tests
Test of host (```libraries/tuttle/pyTest```) with nosetests tool.
```
nosetests libraries/tuttle/pyTest
```
