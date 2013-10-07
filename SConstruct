EnsureSConsVersion( 1, 2 )
EnsurePythonVersion( 2, 5 )

import os
import sys

sys.path.append('tools')
from sconsProject import SConsProject


class Tuttle( SConsProject ):
	'''
	The project compilation object.
	
	This class extends the default SConsProject object with some specific
	utilities for OpenFX.
	'''

	def getOfxPlatformName( self ) :
		'''Get the standard openfx name of the current platform.
		One of the followings :
			* MacOS - for Apple Macintosh OS X (compiled 32 bit)
			* Win32 - for Microsoft Windows (compiled 32 bit)
			* IRIX - for SGI IRIX plug-ins (compiled 32 bit)
			* IRIX64 - for SGI IRIX plug-ins (compiled 64 bit)
			* Linux-x86 - for Linux on intel x86 CPUs (compiled 32 bit)
			* Linux-x86-64 - for Linux on intel x86 CPUs running AMD's 64 bit extensions
		'''
		if self.osname == "posix" :
			if self.sysplatform.find("linux") >= 0 :
				if self.bits == 64:
					return 'Linux-x86-64'
				return 'Linux-x86'
			elif self.sysplatform.find("cygwin") >= 0 :
				if self.bits == 64:
					return 'Linux-x86-64'
				return 'Linux-x86'
			elif self.sysplatform.find("darwin") >= 0 :
				return 'MacOS'
			elif self.sysplatform.find("irix") >= 0:
				if self.bits == 64:
					return 'IRIX64'
				return 'IRIX'
		elif self.osname == "nt":
			if self.sysplatform.startswith("win"):
				return 'Win' + str(self.bits)
			elif self.sysplatform.find("cygwin") >= 0:
				return 'Win' + str(self.bits)

		raise "Unknown Platform (%s,%s)" % (osname,sysplatform)

	def getOutputOfxResources( self, pluginFilename ):
		return self.dir_output_plugin + os.sep + pluginFilename + '.ofx.bundle/Contents/Resources'

	def getOutputOfxPlugin( self, pluginFilename ):
		'''Return plugin name in the output directory "xxx/bin/pluginname.ofx.bundle/Contents/ofxplatformname/pluginname.ofx" .'''
		return os.path.join( self.dir_output_plugin, pluginFilename + '.ofx.bundle/Contents', self.getOfxPlatformName(), pluginFilename + '.ofx' )

	def retrieveOfxPluginVersions( self, filename ):
		'''
		Return plugins versions (major, minor).
		
		This information is extracted from the version defined inside the plugin main file.
		'''
		versionMajor = -1
		versionMinor = -1
		f = open( filename, 'r' )
		try:
			line = f.readline()
			while line:
				if line.startswith('#define OFXPLUGIN_VERSION_M'):
					l = line.split()
					if len(l) != 3:
						raise ValueError( 'Syntax error declaring the OFX plugin version in file : "'+filename+'"' )
					if l[1].endswith('MAJOR'):
						versionMajor = int(l[2])
						if versionMinor != -1:
							break;
					if l[1].endswith('MINOR'):
						versionMinor = int(l[2])
						if versionMajor != -1:
							break;
				line = f.readline()
			if versionMajor == -1 or versionMinor == -1:
				raise ValueError( 'Version not found in OFX plugin file : "'+filename+'"' )
		except ValueError as e:
			raise ValueError( str(e) + '\n' +
							   'Valid syntax is : \n'+
							   '#define OFXPLUGIN_VERSION_MAJOR 1\n'+
							   '#define OFXPLUGIN_VERSION_MINOR 0\n' )
		finally:
			f.close()
		return ( versionMajor, versionMinor )

	def createOfxPlugin( self,
			sources=[], libraries=[], dirs=[],
			mainFile='src/mainEntry.cpp', includes=[],
			env=None, localEnvFlags={}, replaceLocalEnvFlags={}, externEnvFlags={}, globalEnvFlags={},
			dependencies=[], install=True
			):
		'''
		Create an openfx plugin from sources files and libraries dependencies.
		'''
		pluginName = self.getName() # eg. 'Foo'
		versions = self.retrieveOfxPluginVersions( os.path.join(self.getRealAbsoluteCwd(), mainFile) )
		pluginFilename = pluginName + '-' + str(versions[0]) + '.' + str(versions[1]) # eg. 'Foo-1.0'
		if self.env['mode'] == 'production' and versions[0] <= 0:
			print '''Don't create "''' + pluginFilename + '" in "production" mode.'
			return None
		allLocalEnvFlags = localEnvFlags.copy()  # duplicate the user dictionary
		if self.linux:
			ofxLinkflags = ['-Wl,--version-script=' + project.inTopDir('libraries/openfxHack/Support/include/linuxSymbols')]
		elif self.macos:
			ofxLinkflags = ['-exported_symbols_list', project.inTopDir('libraries/openfxHack/Support/include/osxSymbols')]
		else:
			ofxLinkflags = []
		if 'SHLINKFLAGS' in allLocalEnvFlags:
			allLocalEnvFlags['SHLINKFLAGS'].append(ofxLinkflags)
		else:
			allLocalEnvFlags['SHLINKFLAGS'] = ofxLinkflags
		pluginInstall = self.SharedLibrary( pluginName,
				sources=sources, dirs=dirs, libraries=libraries, includes=includes,
				env=env, localEnvFlags=allLocalEnvFlags, replaceLocalEnvFlags=replaceLocalEnvFlags, externEnvFlags=externEnvFlags, globalEnvFlags=globalEnvFlags,
				dependencies=dependencies, installAs=self.getOutputOfxPlugin(pluginFilename), install=install,
				public=False )

		envLocal = self.env

		resources_dir =  os.path.join( self.getRealAbsoluteCwd(), 'Resources' )
		if os.path.isdir( resources_dir ):
			plugin_resources = envLocal.InstallAs( self.getOutputOfxResources(pluginFilename), resources_dir )
			envLocal.Depends( plugin_resources, pluginInstall )
			envLocal.Alias( pluginName,   plugin_resources )

		envLocal.Alias( pluginName, pluginInstall )
		envLocal.Alias( 'ofxplugins', pluginName )
		envLocal.Alias( 'all', 'ofxplugins' )

		return pluginInstall


#______________________________________________________________________________#

# Create the object available in all SConscripts
project = Tuttle()
Export('project')
Export({'libs':project.libs})

# Load SConscript files
project.begin()
project.SConscript()
project.end()



