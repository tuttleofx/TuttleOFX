import os
from sys import argv

import getBestPlugin
from pyTuttle import tuttle
tuttle.core().preload(False)

if len(argv) != 4:
    print "Script args : seqIn@.ext movOut.mov lutFile"
    exit()

in_seq = argv[1]
out_mov = argv[2]
lutfile = argv[3]

# Create graph
g = tuttle.Graph()

# Create reader node
reader = g.createNode(tuttle.getBestReader(os.path.splitext(in_seq)[-1]),
                      filename=in_seq).asImageEffectNode()

g.setup()
td = reader.getTimeDomain()
g.setupAtTime(td.min, [reader])

# Create writer node
writer = g.createNode("tuttle.avwriter",filename=out_mov).asImageEffectNode()

# Create LUT node
lut = g.createNode("tuttle.ocio.lut", filename=lutfile)

# Connect nodes
g.connect([reader, lut, writer])

# Set options and writer node
co = tuttle.ComputeOptions(int(td.min), int(td.max))
g.compute(writer, co)
