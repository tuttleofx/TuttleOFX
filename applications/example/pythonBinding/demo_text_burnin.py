import os
import sys

## Set here your args
in_seq = # path to seq in
out_mov = # path to mov
mainPreset = "prores-hq"
lutfile = # path to lut
fontFile="/usr/share/fonts/truetype/arial.ttf"

# set Tuttle env
import envTuttle
from pyTuttle import tuttle

# Create graph
tuttle.core().preload(False)
g = tuttle.Graph()
# Create reader node
reader = g.createNode(tuttle.getBestReader(os.path.splitext(in_seq)[-1]),
                      filename=in_seq).asImageEffectNode()

g.setup()
td = reader.getTimeDomain()
g.setupAtTime(td.min, [reader])

# Create writer node
writer = g.createNode("tuttle.avwriter",filename=out_mov,
                      mainPreset=mainPreset).asImageEffectNode()

# Create LUT node
lut = g.createNode("tuttle.ocio.lut", filename=lutfile)

# Create text node
######### /!\ In next release (Tuttle v9.0), tuttleArgs().time will not work
#text_burnin = "'{args.time_frame}'.format(args=args)"
text_burnin = "str(tuttleArgs().time)"
######

text = g.createNode("tuttle.text", text=text_burnin, textSize=20, expression=1,
                    fontFile=fontFile, color=(1,0,0,1), vAlign="bottom")

# Connect nodes
g.connect([reader, lut, text,writer])

# Set options and writer node
co = tuttle.ComputeOptions(int(td.min), int(td.max))
#co.setVerboseLevel(tuttle.eVerboseLevelTrace)

g.compute(writer, co)
