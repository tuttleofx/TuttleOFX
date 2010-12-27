Import( 'project' )
Import( 'libs' )


tuttleFlags = { 'LIBPATH': [project.inOutputLib()],
                'CCFLAGS': project.CC['warning3'],
                'CPPDEFINES':[('TUTTLE_PLUGIN_PATH','"'+project.inOutputBin()+'"')],
               }

if project.env['mode'] == 'production' :
	tuttleFlags['CPPDEFINES'].append( 'TUTTLE_PRODUCTION' )

if 'sharedNoUndefined' in project.CC:
	tuttleFlags['SHLINKFLAGS'] = [project.CC['sharedNoUndefined']]
if 'visibilityhidden' in project.CC:
	tuttleFlags['SHCCFLAGS'] = [project.CC['visibilityhidden']]

tuttle = project.ObjectLibrary( 'tuttle',
								envFlags=tuttleFlags )

project.commonLibs.append( tuttle )



SConscript( project.scanFiles( ['libraries/openfxHack', 'libraries/tuttle', 'plugins', 'applications'], accept=['SConscript'] ) )

