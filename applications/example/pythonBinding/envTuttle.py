#!/usr/bin/python
import os
import sys

def setEnvTuttle():
	#Set TUTTLE_ROOT
	TUTTLE_ROOT = ''
	os.environ[ 'LIBAV_DATADIR' ]   = os.path.join( TUTTLE_ROOT, 'libav-presets' )
	os.putenv ( 'OFX_PLUGIN_PATH',    os.path.join( TUTTLE_ROOT, 'plugin' ) )
	os.environ[ 'TUTTLE_HOME' ]     = os.path.join( os.environ['HOME'], '.tuttleofx' )

	sys.path.append( os.path.join( TUTTLE_ROOT, 'python' ) )


	import loadTuttleLibraries
