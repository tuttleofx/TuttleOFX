from pyTuttle import tuttle
tuttle.core().preload(False)
tuttle.core().getFormatter().setLogLevel(tuttle.eVerboseLevelTrace)

class CallbackText(tuttle.Callback):
	"""
	Callback for the text node, to burn timeCode.
	"""
	def __init__(self, fps):
		super(CallbackText, self).__init__()
                self._fps = fps

	def call(self, node, time_):
		params = node.getParamSet()
		h = int(time_ / (3600 * self._fps ))
		m = int(time_ / (60 * self._fps ) % 60)
		s = int(time_ / self._fps % 60)
		ss = int(time_ % self._fps)
		timeCode = '{0:02d}:{1:02d}:{2:02d}:{3:02d}'.format( h, m, s, ss )
		params.getParam("text").setValue(timeCode)


outputFile = "mire.mov"
fps = 25
startedFrame = 0
endedFrame = 25

# Create graph
graph = tuttle.Graph()
nodes = []

# Create nodes
nodes.append(graph.createNode("tuttle.colorbars", format="HD"))
text = graph.createNode("tuttle.text", font="utopia", vAlign="bottom", textSize=80)
nodes.append(text)
nodes.append(graph.createNode("tuttle.avwriter", filename=outputFile, v_customFps=fps))

# Create text callback to print timecode
callback = CallbackText(fps)
text.setBeforeRenderCallback(callback)

# Compute graph
graph.connect(nodes)
options = tuttle.ComputeOptions()
options.addTimeRange(startedFrame, endedFrame)
graph.compute(options)
