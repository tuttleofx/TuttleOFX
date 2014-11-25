Import( 'project', 'libs' )

### To enable/disable python expressions in tuttle host
project.TUTTLE_HOST_WITH_PYTHON_EXPRESSION = True

### Define global flags for the whole project
# depending on the platform and compilation mode
tuttleFlags = {
		'LIBPATH': [project.inOutputLib()],
		'CCFLAGS': project.CC['warning3'],
		'CPPDEFINES':
			[
				('TUTTLE_PLUGIN_PATH','"'+project.inOutputPlugin()+'"'),
			],
	}

if project.env['mode'] == 'production' :
	# In 'production' mode set a flag TUTTLE_PRODUCTION
	tuttleFlags['CPPDEFINES'].append( 'TUTTLE_PRODUCTION' )

# If your compiler as a visibility flag, hide all plugin internal things
# Note that this is necessary even in debug mode, to avoid multiply defined symbols
# when several plugins are loaded (this doesn't remove debugging symbols).
if 'visibilityhidden' in project.CC:
	tuttleFlags['SHCCFLAGS'] = [project.CC['visibilityhidden']]

# If your compiler as a flag to mark undefined flags as error in shared libraries
if 'sharedNoUndefined' in project.CC:
	tuttleFlags['SHLINKFLAGS'] = [project.CC['sharedNoUndefined']]

# Creates a dependency target without associated code or compiled object,
# but only associated with compilation flags
tuttle = project.ObjectLibrary( 'tuttleDefault', envFlags=tuttleFlags )
# Set this object library as a default library for all targets
project.commonLibs.append( tuttle )

### Define BOOST_ALL_DYN_LINK if windows isn't the platform. Dynamic link is required on linux for Boost log.
import os
import sys
windows = os.name.lower() == "nt" and sys.platform.lower().startswith("win")

if not windows:
	tuttleFlags['CPPDEFINES'].append( 'BOOST_ALL_DYN_LINK' )


### Load all SConscript files (in the correct order)
SConscript(
		project.scanFiles( [
				'libraries/boostHack',
				'libraries/openfxHack',
				'libraries/terry',
				'libraries/sequenceParser',
			], accept=['SConscript'] ) +
		['libraries/tuttle/SConscript'] +
		project.scanFiles( [
				# Generators are included first,
				# as some other plugins use them for tests.
				'plugins/image/generator',
				'plugins/image/Dummy',
				'plugins/image/io',
				'plugins/image/process',
				'plugins/param',
				'plugins/private',
				'applications',
				'libraries/tuttle/tests',
				'libraries/tuttle/pyTest',
				'doc',
			], accept=['SConscript'])
	)


