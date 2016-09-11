from pyTuttle import tuttle

from nose.tools import *


def setUp():
	tuttle.core().preload(False)


def testPluginDescProps():
	"""
	Test the 'SupportedExtensions' and 'Evaluation' properties of plugins pluginTurboJpeg and pluginImageMagick.
	"""
	# test evaluation for pluginTurboJpeg
	oiio_plugin = tuttle.core().getImageEffectPluginCache().getPluginByLabel("TuttleOpenImageIOReader")
	oiio_plugin.loadAndDescribeActions()
	oiio_desc = oiio_plugin.getDescriptor()

	assert( oiio_desc.getProperties().hasProperty( 'TuttleOfxImageEffectPropSupportedExtensions', True ) )
	assert( oiio_desc.getProperties().hasProperty( 'TuttleOfxImageEffectPropEvaluation', True ) )
	oiio_evaluation = oiio_desc.getProperties().getDoubleProperty( "TuttleOfxImageEffectPropEvaluation" )
	assert_equal(oiio_evaluation, 40)

	# test evaluation for pluginImageMagick
	imageMagick_plugin = tuttle.core().getImageEffectPluginCache().getPluginByLabel("TuttleImageMagickReader")
	imageMagick_plugin.loadAndDescribeActions()
	imageMagick_desc = imageMagick_plugin.getDescriptor()

	assert( imageMagick_desc.getProperties().hasProperty( 'TuttleOfxImageEffectPropSupportedExtensions', True ) )
	assert( imageMagick_desc.getProperties().hasProperty( 'TuttleOfxImageEffectPropEvaluation', True ) )
	imageMagick_evaluation = imageMagick_desc.getProperties().getDoubleProperty( "TuttleOfxImageEffectPropEvaluation" )
	assert_equal(imageMagick_evaluation, 0)
