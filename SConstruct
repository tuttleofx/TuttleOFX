EnsureSConsVersion( 1, 2 )
EnsurePythonVersion( 2, 5 )

import os
import sys

sys.path.append('tools')
from sconsProject import SConsProject


class Tuttle( SConsProject ):
    dir_libraries  = os.getcwd() + os.sep + 'libraries'
    
    def SConscript(self, dirs=[], exports=['project']):
        '''
        To include SConscript from SConstruct, this automatically define variantdirs.
        '''
        if not dirs:
            SConscript( self.inBuildDir(self.getAbsoluteCwd('SConscript')), exports=exports )
        else:
            for d in dirs:
                SConscript( dirs=self.inBuildDir(d), exports=exports )

    def createOptions(self, filename, args) :
        opts = Putois.createOptions(self,filename,args)
        opts.Add( PathVariable( 'LIBRARIES',    'Directory of intern libraries', self.dir_libraries ) )
        return opts

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
            elif self.sysplatform.find("irix") >= 0 :
                if self.bits == 64:
                    return 'IRIX64'
                return 'IRIX'
        elif self.osname == "nt":
            if self.sysplatform.startswith("win"):
                return 'Win' + str(self.bits)
            elif self.sysplatform.find("cygwin") >= 0 :
                return 'Win' + str(self.bits)
        
        raise "Unknown Platform (%s,%s)" % (osname,sysplatform)

    def getOutputOfxResources( self ) :
        name = self.getName()
        return self.dir_output_bin + os.sep + name + '.ofx.bundle/Contents/Resources'

    def getOutputOfxPlugin( self ) :
        '''Retourne le nom du plugin dans le repertoire de sortie "..../bin/pluginname.ofx.bundle/Contents/ofxplatformname/pluginname.ofx" .'''
        name = self.getName()
        return self.dir_output_bin + os.sep + name + '.ofx.bundle/Contents' + os.sep + self.getOfxPlatformName() + os.sep + name + '.ofx'

    def createOfxPlugin( self, sources=[], libs=[], dirs=[] ):
        '''
        Cree un plugin OpenFX, a partir d'une liste de sources et d'une liste de librairies.
        '''
        pluginName = self.getName()
        libs.append( self.libs.tuttle )
        if dirs:
            sources.extend( self.scanFiles( dirs ) )
        env_local = self.createEnv( libs )
        env_local.AppendUnique( CCFLAGS = self.CC['visibilityhidden'] )
        plugin = env_local.SharedLibrary( target=pluginName, source=sources )
        env_local.InstallAs( self.getOutputOfxPlugin(), plugin )
        
        resources_dir =  self.getRealAbsoluteCwd() + os.sep + 'Resources'
        if os.path.isdir( resources_dir ) :
            plugin_resources = Command( self.getOutputOfxResources(), resources_dir, Copy("$TARGET", "$SOURCE"))
            #Copy( self.getOutputOfxResources(), resources_dir )
            env_local.Alias( pluginName,   plugin_resources )
        
        env_local.Alias( pluginName,   self.getOutputOfxPlugin() )
        env_local.Alias( 'ofxplugins', pluginName )
        env_local.Alias( 'all', 'ofxplugins' )
        
        if env_local['WINDOWS']:
            if env_local['debug']:
                mode = 'Debug'
            else:
                mode = 'Release'
            visual_project = env_local.MSVSProject(
                target = 'visual' + os.sep + pluginName + env_local['MSVSPROJECTSUFFIX'],
                srcs = sources,
                #incs = barincs,
                #localincs = barlocalincs,
                #resources = barresources,
                #misc = barmisc,
                buildtarget = plugin,
                variant = ['Release']
                )
            env_local.Alias( 'visualProject',   visual_project )


#______________________________________________________________________________#

project = Tuttle()
libs = project.libs
Export('project')
Export('libs')

#______________________________________________________________________________#


project.begin()
project.SConscript()
project.end()



