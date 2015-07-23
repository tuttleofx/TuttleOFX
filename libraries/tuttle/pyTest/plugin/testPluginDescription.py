from pyTuttle import tuttle

from nose.tools import *


def setUp():
	tuttle.core().preload(False)


def testPluginDescProps():
	"""
	Test the 'SupportedExtensions' and 'Evaluation' properties of plugins pluginTurboJpeg and pluginImageMagick.
	"""
	# test evaluation for pluginTurboJpeg
	turboJpeg_plugin = tuttle.core().getImageEffectPluginCache().getPluginByLabel("TuttleTurboJpegReader")
	turboJpeg_plugin.loadAndDescribeActions()
	turboJpeg_desc = turboJpeg_plugin.getDescriptor()

	assert( turboJpeg_desc.getProperties().hasProperty( 'TuttleOfxImageEffectPropSupportedExtensions', True ) )
	assert( turboJpeg_desc.getProperties().hasProperty( 'TuttleOfxImageEffectPropEvaluation', True ) )
	turboJpeg_evaluation = turboJpeg_desc.getProperties().getDoubleProperty( "TuttleOfxImageEffectPropEvaluation" )
	assert_equal(turboJpeg_evaluation, 90)

	# test evaluation for pluginImageMagick
	imageMagick_plugin = tuttle.core().getImageEffectPluginCache().getPluginByLabel("TuttleImageMagickReader")
	imageMagick_plugin.loadAndDescribeActions()
	imageMagick_desc = imageMagick_plugin.getDescriptor()

	assert( imageMagick_desc.getProperties().hasProperty( 'TuttleOfxImageEffectPropSupportedExtensions', True ) )
	assert( imageMagick_desc.getProperties().hasProperty( 'TuttleOfxImageEffectPropEvaluation', True ) )
	imageMagick_evaluation = imageMagick_desc.getProperties().getDoubleProperty( "TuttleOfxImageEffectPropEvaluation" )
	assert_equal(imageMagick_evaluation, 0)