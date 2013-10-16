#!/usr/bin/python
import sys
import os
from ctypes import *

#Shared libraries should be loaded in python context before the plugins.

TUTTLE_LIB_DIR = "/path/to/tuttle_inst/lib"

#The list above is provided to give you an idea of what have to be loaded.
#This is unlikely to work without some modifications according to your own build / installation of Tuttle.
libs = [
	"libraw.so",
	"libboost_system.so",
	"libboost_chrono.so0",
	"libboost_thread.so",
	"libboost_timer.so",
	"libboost_serialization.so",
	"libboost_wserialization.so",
	"libboost_regex.so",
	"libboost_python.so",
	"libboost_program_options.so",
	"libboost_thread.so",
	"libboost_filesystem.so",
	"libboost_date_time.so",
	"libboost_log.so",
	"libx264.so",
	"libicudata.so",
	"libicuuc.so",
	"libicui18n.so",
	"libIlmCtlMath.so",
	"libIlmCtl.so",
	"libIlmCtlSimd.so",
	"libopenjpeg.so",
	"libOpenImageIO.so",
	"libavutil.so",
	"libavcodec.so",
	"libavformat.so",
	"libswscale.so",
	"libavdevice.so",
	"libavresample.so",
	"libswresample.so",
	"libavfilter.so",
	"libpostproc.so",
	"liblcms2.so",
	"libturbojpeg.so",
	]

for lib in libs:
	try:
		lib_path = os.path.join( TUTTLE_LIB_DIR, lib )
		cdll.LoadLibrary( lib_path )
	except Exception, e:
		print("ERROR: unable to load library:", lib_path)
		print e
