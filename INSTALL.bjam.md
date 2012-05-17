TuttleOFX  - Install HOWTO - BJam
=================================

Why Boost.Build ?
-----------------

TuttleOFX relies heavily on the Boost library, so instead of using one of
the many multiplatform build system available like Scons, CMake, Autotools
we're simply using Boost.Build, the build system embedded within Boost.

Boost.Build is powerful yet very expressive and terse. It's a weapon of
choice when building large pieces of code with a lot of dependencies.

An excellent introduction to Boost.Build can be found [here](http://www.highscore.de/cpp/boostbuild/).

And the official documentation is available [online](http://www.boost.org/boost-build2/doc/html/).


Setting up the build process
----------------------------

>- We call $TUTTLEOFX the TuttleOFX root directory.  
>>     export TUTTLEOFX=`pwd`  

>- Fetching external libraries  
>>     cd $TUTTLEOFX/3rdParty
>- now download the libraries
>>     python init.py

>- create the command builder bjam-gcc.sh or/and bjam-clang.sh or bjam.bat  
>>     python makeBjamCommand.py

>- configure 3rdParty libraries: freetype, libcaca, jpeg, imagemagick, webp and openjpeg libraries
>>     python configure.py

## Building the building tool : bjam

BJam is Boost library's build tool. TuttleOFX uses it also as a project builder.  
In order to build bjam and boost libraries:  
>     cd $TUTTLEOFX/3rdParty/boost  

>+ under linux platform:  
>>     ./bootstrap.sh  

>+ under windows platform:  
>>     .\bootstrap.bat

## Setting up environment variables
>In order to use bjam, these environment variables have to be set:  
>>- PATH	: to locate bjam executable  
>>>     export PATH=$TUTTLEOFX/3rdParty/boost:$PATH 

>>- BOOST\_ROOT : to locate boost 
>>>     export BOOST_BUILD_PATH=$TUTTLEOFX/3rdParty/boost
 
>>- BOOST\_BUILD\_PATH : to locate boost.build scripts  
>>>     export BOOST_BUILD_PATH=$TUTTLEOFX/3rdParty/boost/tools/build/v2  


## Specify your libraries

>>     cd $TUTTLEOFX 

> **copy projet-config file - do not modify this file**
>>     cd $TUTTLEOFX  
>>     cp tools/bjamf/project-config.jam project-config.jam

> **copy 3rdParty-config file**
>>     cd $TUTTLEOFX  
>>     cp tools/bjamf/3rdParty-config.jam 3rdParty-config.jam
> this file is to switch between 3rdParty library and installed library on machine  
> 1: use the 3rdParty directory  
> 0: use the library specified in user-config.jam

> **copy configuration files**  
>>     cp tools/bjamf/user-config.linux.jam user-config.jam  
>> you can edit external library using these template:  
>>>      lib libname : : <file>/directory/library_name.ext  
>>>                  : : <include>/include_directory ;  

## Building TuttleOFX
* In order to build TuttleOFX library (tuttle) and command line tool (sam), you can:  
>* on Linux with gcc  
>`./bjam-gcc.sh /sam-tools /plugins`  
>* on Linux with clang  
>`./bjam-clang.sh /sam-tools /plugins`  
>* on Windows with msvc  
>`./bjam.bat /sam-tools /plugins`  

* or you can build only a specific plugin, using  
>* on Linux with gcc  
>`./bjam-gcc.sh /plugin/image/process/color/Invert`  
>* on Linux with clang  
>`./bjam-clang.sh /plugin/image/process/color/Invert`  
>* on Windows with msvc  
>`./bjam.bat /plugin/image/process/color/Invert` 

>**Warning**  
> By default Boost BJam build **debug** version.

* Run it with one or more of the following command line arguments  

    --user-config=XXX  
        __Specifies the configuration file__  
        
    --help  
        __Display help message__  
        
    --clean  
        __Remove targets instead of building__  

    --no-XXX  
       __With XXX being one of plugin-jpg, plugin-dpx, plugin-oiio__
        
    tests  
        __Build & run unit test suite__  
        
    debug -or- release  
        __Select the build variant ( debug is default )__

    -a  
        __Build all targets, even if they are current__  

    toolset  
        __Specify toolset builder [msvc, gcc, clang, ...]__
#### 3rdParty libraries 

* [`Boost 1.49.0`](http://www.boost.org/users/history/version_1_49_0.html)
* [`Qt 4.8`](http://qt-project.org/downloads)

>#### Processing 3rdParty libraries
>* [`CTL 1.4.1` - Color Transformation Language](http://ampasctl.sourceforge.net/) based on
>>* [`Ilmbase 1.0.1`](http://www.openexr.com/downloads.html)  
>>* [`OpenEXR 1.6.1`](http://www.openexr.com/downloads.html)

>* [`OCIO v1.0.6-0` - OpenColorIO](http://opencolorio.org/) based on  
>>* [`TinyXml 2.6.1`](http://www.grinninglizard.com/tinyxml/)
>>* [`Yaml 0.2.6`](http://yaml.org/)
>* [`Freetype 2.4.3`](http://www.freetype.org/)
>* [`libcaca 0.99 beta17`](http://caca.zoy.org/wiki/libcaca)

>#### Input/Output 3rdParty libraries
>* [`ImageMagick`](http://www.imagemagick.org/)
>* [`OIIO RB-1.0` - OpenImageIO](https://github.com/OpenImageIO/oiio/archives/RB-1.0)
>* [`OpenEXR 1.6.1`](http://www.openexr.com/downloads.html)
>* [`OpenJpeg 1.4 sources r697`](www.openjpeg.org)
>* [`LibTIFF 3.9.5`](http://www.libtiff.org/)
>* [`WebP 0.1.3`](https://developers.google.com/speed/webp)
>* [`LibRaw 0.14.5`](http://www.libraw.org/)
>* [`libpng 1.2.41`](http://www.libpng.org/pub/png/libpng.html)
>* [`FFMpeg`](http://ffmpeg.org/)
>* [`Jasper 1.900.1`](http://www.ece.uvic.ca/~frodo/jasper/)
