from pyTuttle.tuttle import *

if __name__ == "__main__":
	core().preload()

	compute( [
		NodeInit( "tuttle.checkerboard", size=[50,50] ),
		NodeInit( "tuttle.blur", size=[0.03, 0.05] ),
		NodeInit( "tuttle.swscale", height=500 ),
		NodeInit( "tuttle.pngwriter", filename="data/output.png" ),
		] )
