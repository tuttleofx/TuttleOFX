Import( 'project' )
Import( 'libs' )

class TuttleProjectChecker(libs.base.BaseLibChecker):
	'''
	Global configuration for all the project.
	'''

	def __init__( self ):
		self.name  = 'tuttle'

	def initOptions(self, putois, opts):
		opts.Add( BoolVariable( 'with_'+self.name, 'enabled compilation with '+self.name, True  ) )
		return True

	def configure(self, putois, env):
		if not self.enabled(env):
			return True
		env.Append( CPPPATH = [ env['EXTLIBRARIES'], env['LIBRARIES'] ] )
		env.Append( LIBPATH = [ putois.inOutputLib() ] )
		if 'sharedNoUndefined' in project.CC:
			env.Append( SHLINKFLAGS = [project.CC['sharedNoUndefined']] )
		if 'visibilityhidden' in project.CC:
			env.Append( SHCCFLAGS = [project.CC['visibilityhidden']] )
		return True


libs.tuttleProject = TuttleProjectChecker()

project.commonLibs.append(libs.tuttleProject)

SConscript( project.scanFiles( ['libraries', 'plugins', 'applications'], accept=['SConscript'] ) )

