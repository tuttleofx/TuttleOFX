TuttleOFX - version beta
========================
**Project under early development.**
  
TuttleOFX project is an image processing framework based on [OpenFX plugin format](http://openfx.sourceforge.net/).  
You can going to official website for more informations :[https://sites.google.com/site/tuttleofx/](https://sites.google.com/site/tuttleofx/).

The project is composed of 4 main parts:  
> - **Tuttle Host Library**, an OpenFX host that allows you to manipulate a graph of OpenFX nodes.  
> - **Tuttle Plugin Library**, a library to facilitate the creation of OpenFX plugins.  
> - **Tuttle plugins**, a set of OpenFX plugins for image processing and image/video IO.  
> - **Sam**, a set of command line tools.  




___
License
-------
>see COPYING file

___
Compilation
-----------

- Getting the source
>    `git clone git://github.com/tuttleofx/TuttleOFX.git`  
>    `cd TuttleOFX`  
>    `git submodule update -i`  

- External libraries  
> 1. You can get all external libraries using a python script:  
>>    `cd 3rdParty`  
>>    `python init.py`  
>
> 2. Use libraries installed on your computer.  
In this case, you need to use the scons build system.
	
- Compilation
> 2 building mode can be used:
> - [**Boost BJam V2**](http://www.boost.org/boost-build2/) : see INSTALL.bjam
> - [**Scons**](http://www.scons.org/) : see INSTALL.scons

___
Plugin creation
---------------

There is a script to help you making new plugins.
> `python $TUTTLE_ROOT/plugins/_scripts/newPlugin.py`  

This script asks you informations about your plugin and create a new plugin source tree in your current directory.


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

TuttleOfx public repository.
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
