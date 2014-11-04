# Install tools

* [cmake](http://www.cmake.org/download)

* [git](http://code.google.com/p/msysgit/downloads/list)
Use the latest version of "Full installer for official Git for Windows".
If not done automatically, add git binaries to your PATH: `C:\Program Files\Git\cmd`

* [python](http://www.python.org/download)
If not done automatically, add python path to your PATH environment variable: `C:\Python27`
Add all python external scripts directory to your PATH environment variable (needed for scons, for example):

* Visual C++ Express
[Visual 2013](http://www.visualstudio.com/fr-fr/downloads#d-express-windows-desktop)
[SDK](http://msdn.microsoft.com/en-us/windows/hardware/hh852363)
Add compilator binaries into your PATH:
`C:\Program Files\Microsoft Visual Studio 13.0\VC\bin`
To build the project use the visualc++ shell OR execute this script before:
`C:\Program Files\Microsoft Visual Studio 10.0\bin\vcvars32.bat`


# Install libraries

* boost
[Using non-official build](http://boost.teeks99.com)
If you want to build from [source code](http://sourceforge.net/projects/boost/files/boost):
```dos
cd boost_1_47_0
.\bootstrap
.\b2 --toolset=msvc-10.0 --build-type=complete
```

* [bz2](http://gnuwin32.sourceforge.net/packages/bzip2.htm)

* [PThread](ftp://sourceware.org/pub/pthreads-win32)
>  Note: Use pthreads-w32-LATEST-release.exe

* [GLEW](http://glew.sourceforge.net/index.html)

* [zlib](http://sourceforge.net/projects/gnuwin32/files/zlib)
>  Note: You may need to edit "C:\Program Files\gnuwin32\include\zconf.h" and change the #if on line 287 to 0 instead of 1 unless you have unistd.h.

* [Png](http://downloads.sourceforge.net/gnuwin32)

* [Jpeg](http://sourceforge.net/projects/gnuwin32/files/jpeg)

* [Freetype](http://sourceforge.net/projects/gnuwin32/files/freetype)

* [OpenJpeg](http://code.google.com/p/openjpeg/downloads/list)
Run the installer.
If you want to build from source code:
```dos
cd openjpeg
copyopj_config.h.in.user libopenjpeg/opj_config.h
```
Open "DllOpenJPEG.vcproj".
Set Release mode and Build.

* [Freeglut](http://www.transmissionzero.co.uk/software/freeglut-devel)

* [FFMpeg](http://ffmpeg.zeranoe.com/builds)
You need to install the Dev package.


# Other libraries must be compiled manually:

* [OpenEXR](http://www.openexr.com/downloads.html)  (not tested)

* [CTL](http://sourceforge.net/projects/ampasctl/files/openexr_ctl)  (not tested)

* [OpenImageIO](https://sites.google.com/site/openimageio/building-oiio-on-windows)  (not tested)

* [Raw](http://www.libraw.org/docs/Install-LibRaw-eng.html)  (not tested)

* [libcaca](http://caca.zoy.org/wiki/libcaca)  (not tested)

