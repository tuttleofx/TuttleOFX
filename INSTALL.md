TuttleOFX compilation with scons tool
=====

We call $TUTTLEOFX the TuttleOFX root directory.
> export TUTTLEOFX=\`pwd\`

___
### A - External libraries  

You have the choice between:
> 1. install libraries on your system using standard packages.  
> 2. download all external libraries, with a dedicated script.  
___

**A - 1 install libraries on your system using standard packages**  
You need to install all libraries, see website for the full list:  
http://sites.google.com/site/tuttleofx/development/build/libraries  

When all libraries are installed, create a configuration file.  
Use the precreated configuration file, depending on your operating system:  
> cp $TUTTLEOFX/tools/sconf/ubuntu.sconf host.sconf  
> cp $TUTTLEOFX/tools/sconf/macos.sconf host.sconf  
> cp $TUTTLEOFX/tools/sconf/windows.sconf host.sconf  

**A - 2 download all external libraries**  
> cd $TUTTLEOFX/3rdParty  

and download libraries  
> python init.py  

now you need to compile all these libraries by yourself,  
because all these libraries have their own build system,  
their own dependencies, their own flags you may want to choose, etc.  

copy the configuration file to use local libraries
> cp $TUTTLEOFX/tools/sconf/local.sconf host.sconf

### B - Building

> scons

### C - Use it !

> export OFX_PLUGIN_PATH=$TUTTLEOFX/dist/\`hostname\`/gcc-\`gcc -dumpversion\`  
> export PATH=$OFX_PLUGIN_PATH/bin:$PATH  

If you use custom installation of libraries, you need to make them available  
> export LD_LIBRARY_PATH=/path/to/custom/install/libraries:$LD_LIBRARY_PATH  

### D - Finished ! Launch applications !  

you can try  
> sam ls  
> sam do -h  

or try plugins in external OpenFX Host applications  

### Installation  
install is an option and not a target, so you can specify  
the targets you want to install  
> scons INSTALLPATH=/custom/path/to/tuttlePackage install=True  

### Some build options for particular purposes  

see scons documentation [http://www.scons.org/documentation.php](http://www.scons.org/documentation.php)  

**some scons features commonly used** 

Usefull scons command line options and properties  
> scons -h                        # help: all project options  
> scons -H                        # scons help: all scons options  

Build a particular target, like  
> scons python # to only compile tuttle python wrapping  
> scons sam-ls # to only compile sam-ls application  

Clean the project
> scons -c  

And clean a specific target  
> scons sam-ls -c  

Compile only one directory by compiling from within this directory  
notice the flag -u to search a SConstruct in upper directories  
> cd $TUTTLEOFX/plugins/image/filter  
> scons -u  

other solution, from the root directory  
> scons plugins/image/filter  

you can add default flags to scons,  
for example it's usefull to always add "-u" flag  
> export SCONSFLAGS='-u'  

all options from .sconf files can be overloaded by command line options  
