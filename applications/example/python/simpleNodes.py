from pyTuttle.tuttle import *

core().preload()

compute( [
	NodeInit( "tuttle.pngreader", "data/input.png" ),
	NodeInit( "tuttle.blur", size=[0.03, 0.05] ),
	NodeInit( "tuttle.pngwriter", filename="data/output.png" ),
	] )
