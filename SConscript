Import( 'project' )
Import( 'libs' )


tuttleFlags = { 'LIBPATH': [ project.inOutputLib() ] }

if 'sharedNoUndefined' in project.CC:
	tuttleFlags['SHLINKFLAGS'] = [project.CC['sharedNoUndefined']]
if 'visibilityhidden' in project.CC:
	tuttleFlags['SHCCFLAGS'] = [project.CC['visibilityhidden']]

tuttle = project.ObjectLibrary( 'tuttle', includes=[ project.env['EXTLIBRARIES'], project.env['LIBRARIES'] ], envFlags=tuttleFlags )

project.commonLibs.append( tuttle )



SConscript( project.scanFiles( ['libraries', 'plugins', 'applications'], accept=['SConscript'] ) )

