Import( 'project', 'libs' )


# to enable/disable python expressions in tuttle host
project.TUTTLE_HOST_WITH_PYTHON_EXPRESSION = False



# define global flags for the whole project
# depending on the platform and compilation mode
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



# load all SConscript files (in the correct order)
SConscript(
	project.scanFiles( [
			'libraries/boostHack',
			'libraries/openfxHack',
			'libraries/terry',
			'libraries/tuttle',
			'plugins',
			'applications',
			'doc',
		], accept=['SConscript'] ) )


