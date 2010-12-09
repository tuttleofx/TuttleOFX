EnsureSConsVersion( 1, 2 )
EnsurePythonVersion( 2, 5 )

import os
import sys

sys.path.append('tools')
from sconsProject import SConsProject


class Tuttle( SConsProject ):

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
		return self.dir_output_bin + os.sep + pluginFilename + '.ofx.bundle/Contents/Resources'

	def getOutputOfxPlugin( self, pluginFilename ):
		'''Return plugin name in the output directory "xxx/bin/pluginname.ofx.bundle/Contents/ofxplatformname/pluginname.ofx" .'''
		return os.path.join( self.dir_output_bin, pluginFilename + '.ofx.bundle/Contents', self.getOfxPlatformName(), pluginFilename + '.ofx' )

	def retrieveOfxPluginVersions( self, filename ):
		'''Return plugins versions (major, minor)'''
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

	def createOfxPlugin( self, sources=[], libs=[], dirs=[], mainFile='src/mainEntry.cpp' ):
		'''
		Create an openfx plugin from sources files list and libraries list.
		'''
		pluginName = self.getName() # eg. 'Foo'
		versions = self.retrieveOfxPluginVersions( os.path.join(self.getRealAbsoluteCwd(), mainFile) )
		pluginFilename = pluginName + '-' + str(versions[0]) + '.' + str(versions[1]) # eg. 'Foo-1.0'
		if self.env['mode'] == 'production' and versions[0] <= 0:
			print '''Don't create "''' + pluginFilename + '" in "production" mode.'
			return
		allSources = []
		if sources:
			allSources = sources
		if dirs:
			allSources.extend( self.scanFiles( dirs ) )

		envLocal = self.createEnv( libs )
		envLocal.Append( CPPPATH = dirs )
		plugin = envLocal.SharedLibrary( target=pluginName, source=allSources )
		pluginInstall = envLocal.InstallAs( self.getOutputOfxPlugin(pluginFilename), plugin[0] )

		resources_dir =  self.getRealAbsoluteCwd() + os.sep + 'Resources'
		if os.path.isdir( resources_dir ) :
			plugin_resources = Command( self.getOutputOfxResources(pluginFilename), resources_dir, Copy("$TARGET", "$SOURCE"))
			envLocal.Depends( plugin_resources, pluginInstall )
			envLocal.Alias( pluginName,   plugin_resources )

		envLocal.Alias( pluginName, pluginInstall )
		envLocal.Alias( 'ofxplugins', pluginName )
		envLocal.Alias( 'all', 'ofxplugins' )

		if self.windows:
			mode = 'Release'
			if envLocal['mode'] is "debug":
				mode = 'Debug'
			visual_project = envLocal.MSVSProject(
				target = os.path.join('visualc', pluginName + envLocal['MSVSPROJECTSUFFIX']),
				srcs = allSources,
				#incs = barincs,
				#localincs = barlocalincs,
				#resources = barresources,
				#misc = barmisc,
				buildtarget = plugin,
				variant = [mode]
				)
			envLocal.Alias( 'visualProject',   visual_project )


#______________________________________________________________________________#

project = Tuttle()
Export('project')
Export({'libs':project.libs})

#______________________________________________________________________________#

project.begin()
project.SConscript()
project.end()



