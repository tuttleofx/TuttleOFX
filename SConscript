Import( 'project', 'libs' )


# to enable/disable python expressions in tuttle host
project.TUTTLE_HOST_WITH_PYTHON_EXPRESSION = False


tuttleFlags = {
		'LIBPATH': [project.inOutputLib()],
		'CCFLAGS': project.CC['warning3'],
		'CPPDEFINES':
			[('TUTTLE_PLUGIN_PATH','"'+project.inOutputBin()+'"')],
	}

if project.env['mode'] == 'production' :
	tuttleFlags['CPPDEFINES'].append( 'TUTTLE_PRODUCTION' )
	if 'visibilityhidden' in project.CC:
		tuttleFlags['SHCCFLAGS'] = [project.CC['visibilityhidden']]

if 'sharedNoUndefined' in project.CC:
	tuttleFlags['SHLINKFLAGS'] = [project.CC['sharedNoUndefined']]

tuttle = project.ObjectLibrary( 'tuttle',
								envFlags=tuttleFlags )

project.commonLibs.append( tuttle )



SConscript( project.scanFiles( ['doc', 'libraries/openfxHack', 'libraries/tuttle', 'plugins', 'applications'], accept=['SConscript'] ) )

