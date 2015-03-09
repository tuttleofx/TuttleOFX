# scons: pluginBlur

from pyTuttle import tuttle

def setUp():
	tuttle.core().preload(False)


def testDoubleParamProps():
	g = tuttle.Graph()
	blur = g.createNode( "tuttle.blur", size={1.0:[80.0, 40.0], 9.0:0.0} )
	
	blurSize = blur.getParam("size")
        
        # You can also access all OFX properties directly
	print("Hint:", blurSize.getProperties().getStringProperty("OfxParamPropHint"))

        # Print all properties
        for p in blurSize.getProperties():
            print(p.getType(), p.getName(), p.getDimension(), p.getStringValue())

