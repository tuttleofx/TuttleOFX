This folder is intented to receive third party libraries


Install dependencies
====================

Before trying to build all dependencies manually,
check our website if there is no standard package or installer for you platform:
[Install dependencies for each platform](http://www.tuttleofx.org/development/build/libraries)

Then you may need to build missing dependencies manually.
Remember that the project is large but is **designed in a modular way**.
**The only needed library is boost.** Other dependencies are used from
plugins, so you **could use the project without all plugins!**
Check what you need to avoid loosing time in building plugins
that you will not use.


Download dependencies
=====================

There is a python script to download almost all dependencies of the project with the
recommanded versions.
> python init.py

You can specify arguments to download only specific dependencies.
For example, to download boost and openimageio:
> python init.py boost openimageio


Help for building dependencies
==============================

We can't explain how to build all dependencies on all platforms.
So it's just some tips for some libraries...


Boost
-----

```
python init.py boost
cd boost
./bootstrap
./b2
```


OpenEXR
-------

```
git clone https://github.com/openexr/openexr.git
cd openexr/
git checkout -b v1.7.1 v1.7.1

cd IlmBase
./bootstrap
./configure --prefix=$PWD/../install
make install
cd ../OpenEXR
./bootstrap
./configure --prefix=$PWD/../install
make install
```


OpenEXR v2
----------

```
git clone https://github.com/openexr/openexr.git
cd openexr/

cd IlmBase
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/../../install
make install
cd ../../OpenEXR
mkdir build && cd build
cmake .. -DCMAKE_CXX_FLAGS=-I$PWD/../../install/include/OpenEXR -DCMAKE_SHARED_LINKER_FLAGS=-L$PWD/../../install/lib -DCMAKE_INSTALL_PREFIX=$PWD/../../install
make install -i
```


CTL
---

We currently use a customized version of the CTL.

If you build TuttleOFX on master (version <= 0.8) :
```
git clone https://github.com/tuttleofx/CTL.git
git checkout tuttle_master
cd CTL
./configure
make
```

If you build TuttleOFX on develop (version > 0.8) :
```
- First build aces_container :
	git clone https://github.com/ampas/aces_container.git
	cd aces_container
	mkdir build && cd build
	# Could choose the install folder: -DCMAKE_INSTALL_PREFIX=$PWD/../install
	cmake ..
	make install
- Then build CTL :
	cd ..
	git clone https://github.com/tuttleofx/CTL.git
	cd CTL
	git checkout tuttle_develop
	mkdir build && cd build
	cmake ..
	make install

```


SeExpr
------

```
sudo apt-get install bison flex sed
git clone https://github.com/wdas/SeExpr.git
cd SeExpr
git checkout rel-1.0.1 -b rel-1.0.1
make
sudo make install
```


libav
-----

You need to add the enable-shared flag, to be able to use it from a plugin.
```
git clone git://git.libav.org/libav.git
cd libav
./configure --enable-shared
make
```
