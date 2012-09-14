![TuttleOFX](https://github.com/tuttleofx/TuttleOFX/raw/master/plugins/_scripts/ImageEffectApi/Resources/L_ProjectName_.png "TuttleOFX")TuttleOFX
========================
**Project under early development.**
  
TuttleOFX project is an image processing framework based on [OpenFX plugin format](http://openfx.sourceforge.net/).  
You can going to official website for more informations : [https://sites.google.com/site/tuttleofx/](https://sites.google.com/site/tuttleofx/).

The project is composed of 4 main parts:
> - **Tuttle Host Library**, an OpenFX host that allows you to manipulate a graph of OpenFX nodes.  
> - **Tuttle Plugin Library**, a library to facilitate the creation of OpenFX plugins.  
> - **Tuttle plugins**, a set of OpenFX plugins for image processing and image/video IO.  
> - **Sam**, a set of command line tools.  



___
License
-------
>see [**COPYING.md**](COPYING.md)

___
Compilation
-----------

- Getting the source
>    `git clone git://github.com/tuttleofx/TuttleOFX.git`  
>    `cd TuttleOFX`  
>    `git submodule update -i`  

- External libraries
>
>    1. Build from source code  
>       You can get all external libraries source code using a python script:  
>       `cd 3rdParty`  
>       `python init.py`  
>       And follow build instructions for each library.
>
>    2. Use packaging tools or installers
>    See the website instructions to install all dependencies. There are command lines (apt-get, macports, ...) to install all dependencies for some standard distributions, and links to windows installers.  
>    [Extern libraries](https://sites.google.com/site/tuttleofx/development/build/libraries)

- Compilation

See [**INSTALL.md**](INSTALL.md)

___
Plugin creation
---------------

There is a script to help you making new plugins.
> `python $TUTTLE_ROOT/plugins/_scripts/newPlugin.py`  

This script asks you information about your plugin and create a new plugin source tree in your current directory.


Tested compilers
----------------
- Linux
 + GCC 4.1.2 on OpenSUSE 11.2 64bits
 + GCC 4.4.1 on OpenSUSE 11.2 64bits
 + GCC 4.6.1 on Ubuntu 11.10 32/64bits
 + Clang 2.9 on Ubuntu 11.10 32/64bits
- MacOS
 + GCC 4.2.1 64bits
- Windows
 + Visual C++ 2010 on Windows XP 32bits
 + GCC-MINGW 3.4.5, 4.3.0 on Windows XP 32bits


More information 
----------------
Check TuttleOFX website for more information. 
>[http://sites.google.com/site/tuttleofx](http://sites.google.com/site/tuttleofx)

TuttleOFX public repository.
>[http://github.com/tuttleofx/TuttleOFX](http://github.com/tuttleofx/TuttleOFX)
	
OpenFX plugin standard.
>[http://openfx.sourceforge.net](http://openfx.sourceforge.net)


Contact
-------
Developers mailing-list (public)
> [tuttleofx-dev@googlegroups.com](mailto:tuttleofx-dev@googlegroups.com)  
> [http://groups.google.com/group/tuttleofx-dev](http://groups.google.com/group/tuttleofx-dev)

Users mailing-list (public)
> [tuttleofx-user@googlegroups.com](mailto:tuttleofx-user@googlegroups.com)  
> [http://groups.google.com/group/tuttleofx-user](http://groups.google.com/group/tuttleofx-user)

Contact the team in private
> [tuttleofx-team@googlegroups.com](mailto:tuttleofx-team@googlegroups.com)

The github issues module for bug report
> [http://github.com/tuttleofx/TuttleOFX/issues](http://github.com/tuttleofx/TuttleOFX/issues)
