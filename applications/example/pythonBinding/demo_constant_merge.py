import os
from sys import argv

import getBestPlugin
from pyTuttle import tuttle
tuttle.core().preload(False)
tuttle.core().getFormatter().setLogLevel(tuttle.eVerboseLevelTrace)

if len(argv) != 3:
    print "Script args : seqIn@.ext seqOut@.ext"
    exit()

in_seq = argv[1]
out_seq = argv[2]
fontFile="/usr/share/fonts/truetype/arial.ttf"

# Create graph
g = tuttle.Graph()

# Create constant node
# Merge needs entry in the same channel layout / bit depth
constant = g.createNode('tuttle.constant',
                        size=(1920,1080),
                        explicitConversion='8i', 
                        channel='rgba'
                        )
# Create reader node
# Merge needs entry in the same layout / bit depth
reader = g.createNode('tuttle.oiioreader',
                      filename=in_seq,
                      bitDepth='8i',
                      channel='rgba'
                      ).asImageEffectNode()

g.setup()
td = reader.getTimeDomain()
g.setupAtTime(td.min, [reader])

# Create writer node
(base, ext) = os.path.splitext(out_seq)
writerName = getBestPlugin.getBestWriter(ext)
writer = g.createNode(writerName, filename=out_seq)

# Create text nodes
text_burnin = "blurred text"
text_blur = g.createNode("tuttle.text", text=text_burnin, textSize=20,
                         fontFile=fontFile, color=(1,0,0,1),
                         vAlign="bottom")
text = g.createNode("tuttle.text", text=text_burnin, textSize=20,
                    fontFile=fontFile, color=(1,1,0,1), vAlign="bottom")
blur = g.createNode('tuttle.blur', size=(4,4))

# Connect nodes

# Connect and merge text and blurred text
text_merge = g.createNode('tuttle.merge',
                          rod='intersection',
                          mergingFunction='over',
                          )
g.connect([text_blur, blur])
g.connect(blur, text_merge.getClip('B'))
g.connect(text, text_merge.getClip('A'))

# Connect and merge text and constant
constant_text_merge = g.createNode('tuttle.merge',
                                   rod='B',
                                   mergingFunction='over',
                                   )
g.connect(constant, constant_text_merge.getClip('B'))
g.connect(text_merge, constant_text_merge.getClip('A'))

# Connect and merge reader with the rest
# Create merge
image_merge = g.createNode('tuttle.merge',
                           rod='union',
                           mergingFunction='over',
                           offsetB=(0,-(1080-804)/2)
                           )
g.connect(reader, image_merge.getClip('A'))
g.connect(constant_text_merge, image_merge.getClip('B'))

# Write the result
g.connect([image_merge, writer])

# Set options and writer node
co = tuttle.ComputeOptions(int(td.min), int(td.max))

g.compute(writer, co)
