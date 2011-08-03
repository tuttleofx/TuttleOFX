#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import stat
import urllib
import subprocess

fileExec = "../bjam.sh"
fileExecWindows = "../bjam.bat"

def detectCPUs():
 """
 Detects the number of CPUs on a system. Cribbed from pp.
 """
 # Linux, Unix and MacOS:
 if hasattr(os, "sysconf"):
     if os.sysconf_names.has_key("SC_NPROCESSORS_ONLN"):
         # Linux & Unix:
         ncpus = os.sysconf("SC_NPROCESSORS_ONLN")
         if isinstance(ncpus, int) and ncpus > 0:
             return ncpus
     else: # OSX:
         return int(os.popen2("sysctl -n hw.ncpu")[1].read())
 # Windows:
 if os.environ.has_key("NUMBER_OF_PROCESSORS"):
         ncpus = int(os.environ["NUMBER_OF_PROCESSORS"]);
         if ncpus > 0:
             return ncpus
 return 1 # Default

osname		= os.name.lower()
sysplatform	= sys.platform.lower()
windows		= osname == "nt" and sysplatform.startswith("win")
macos		= sysplatform.startswith("darwin")
linux		= not windows and not macos
unix		= not windows
is_64bits	= sys.maxsize > 2**32
haveGcc		= False

if linux :
  if os.system("gcc -v 2&> /dev/null")==0 :
    haveGcc = True
  if os.path.exists(fileExec)==False :
    f = open(fileExec, "w")
    f.write("./3rdParty/boost/bjam ")
    if haveGcc :
      f.write("--toolset=gcc --disable-icu ")
    if is_64bits :
      f.write("cxxflags=-fPIC address-model=64 ")
    f.write("-j%s $* " % detectCPUs())
    f.close()
    os.chmod(fileExec,stat.S_IXUSR+stat.S_IRUSR+stat.S_IWUSR)


if windows :
  if os.path.exists(fileExecWindows)==False :
    f = open(fileExecWindows, "w")
    f.write("@echo off \n")
    f.write("set BOOST_BUILD_PATH=%cd%\\3rdParty\\boost\\tools\\build\\v2 \n")
    f.write("set BOOST_PATH=%cd%\\3rdParty\\boost \n")
    f.write("set BOOST_ROOT=%cd%\\3rdParty\\boost\\boost \n")
    f.write("@echo on \n")
    f.write("3rdParty\\boost\\bjam --toolset=msvc --disable-icu ")
    if is_64bits :
      f.write("address-model=64 ")
    f.write("-j%s " % detectCPUs())
    f.write("%* " )
    f.close()
    os.chmod( fileExecWindows, stat.S_IXUSR + stat.S_IRUSR + stat.S_IWUSR )

print "linux\t\t%s" % (linux)
print "windows\t\t%s" % (windows)
print "unix\t\t%s" % (unix)
print "platerform\t%s" % (sysplatform)
print "osname\t\t%s" % (osname)
print "is 64 bits\t%s" % (is_64bits)

print "has gcc\t\t%s" % (haveGcc)
print "cpus\t\t%s" % detectCPUs()