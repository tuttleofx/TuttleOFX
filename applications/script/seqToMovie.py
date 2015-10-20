#!/usr/bin/env python

import os
import time
import argparse

from pyTuttle import tuttle


def getReaderId(inputSequence):
	"""
	Get tuttle node id of best reader for the given sequence.
	"""
        readerNodeId = 0
	try:
		readerNodeId = tuttle.getBestReader(inputSequence)
                return readerNodeId
	except Exception:
		raise ValueError("Could not read input. Please give a filename as first argument.")


def getSequence(inputSequence):
	"""
	Get sequence from the given file path.
	"""
	from pySequenceParser import sequenceParser
	sequence = sequenceParser.Sequence()
        isSequence = sequenceParser.browseSequence(sequence, inputSequence)
        if isSequence:
            return sequence
        raise ValueError("Given input '%s' is not recognized as an image sequence." % inputSequence)


def interactiveMode():
	"""
	Manage script in case of interactive mode: shell asks questions instead of reading options.
	"""
	print("===== Start interactive mode: press enter to select default choice =====")

	### Video preset
	videoPresets = [
		("H264", "h264-lq"),
		("DNxHD 120", "dnxhd120"),
		("PRORES", "proresproxy"),
	]
	videoPresetsChoices = "Select output video presets:\n"
	for i, (preset, _) in enumerate(videoPresets):
		videoPresetsChoices += " (%s) %s\n" % (i, preset)
	videoPresetsChoices += " Which video preset: "

	videoPresetSelected = raw_input(videoPresetsChoices)
	if videoPresetSelected.isdigit():
		_, args.videoPreset = videoPresets[int(videoPresetSelected)] if int(videoPresetSelected) < len(videoPresets) else videoPresets[0]
	elif videoPresetSelected is "":
		_, args.videoPreset = videoPresets[0]
	else:
		args.videoPreset = videoPresetSelected
	print("=> Output video preset: %s \n" % args.videoPreset)

	### FPS
	_fps = raw_input("Select output fps:\n (0) 25 \n Which fps: ")
	if _fps in ['', "0"] or not _fps.isdigit():
		args.fps = 25
	else:
		args.fps = int(_fps)
	print("=> Output Fps: %d \n" % args.fps)

	### Width
	_width = raw_input("Select output video width in pixels:\n (0) Same as input\n (1) HD (1920x1080)\n Which width: ")
	if _width in ["0", ""] or not _width.isdigit():
		args.width = None
	elif _width in ["1"]:
		args.width = "HD"
	else:
		args.width = int(_width)

	str_width = "same as input"
	if args.width is 1:
		str_width = "1920x1080"
	elif args.width > 1:
		str_width = args.width
	print("=> Output width (in pixels): %s \n" % str_width)

	### Ratio
	_ratio = str.lower(raw_input("Select output ratio:\n (0) Same as input\n (1) 4/3\n (2) 16/9\n Which ratio: "))
	if _ratio in ["0", ""]:
		args.ratio = None
	elif _ratio in ["1", "4/3"]:
		args.ratio = "1.33"
	elif _ratio in ["2", "16/9"]:
		args.ratio = "1.85"
	else:
		args.ratio = _ratio
	print("=> Output ratio: %s \n" % (args.ratio if args.ratio else "same as input" ))

	### LUT
        lutSelected = raw_input("Choose a lut file:\n (0) No lut file \n Lut file: ")
	if lutSelected.isdigit() or lutSelected is "":
		args.lut = False
		args.lutPath = ""
	else:
		args.lut = True
		args.lutPath = lutSelected
	print("=> LUT: %s \n" % (args.lutPath if args.lut else "No lut will be applied"))

	### Text
	_text = str.lower(raw_input("Select if you want tags on output:\n (0) No tag\n (1) Tag image "))
	if _text in ["0", ""]:
		args.text = False
	else:
		args.text = True
	print("=> Tag on output: %s \n" % args.text)


class ProgressHandle(tuttle.IProgressHandle):
	"""
	Progress handle to get progress of process.
	"""
	def __init__(self):
		super(ProgressHandle, self).__init__()

		self._counter = 0

	def beginSequence(self):
		"""
		Called before the beginning of the process.
		Print what the script will do based on what the user indicated in the command-line.
		"""
		print("===== Resume =====")
		print(" > Make the movie '%s' from sequence '%s'" % (args.outputFile, args.inputSequence))
		print(" > Output video preset: %s" % args.videoPreset)
		print(" > Output video fps: %s" % args.fps)
		if args.width:
			print(" > Output video width (in pixels): %s" % args.width)
		if args.ratio:
			print(" > Output video ratio: %s" % args.ratio)
		if args.lut:
			print(" > Output lut applied: %s" % args.lutPath)
		if args.text:
			print(" > Output tag applied: date / image sequence name")
		print("===== Start process =====")

	def setupAtTime(self):
		"""
		Called when setting up an image.
		"""
		pass

	def processAtTime(self):
		"""
		Called before processing an image.
		"""
		print(" > %s%d%s" % (sequence._prefix, self._counter, sequence._suffix))
		self._counter += 1

	def endSequence(self):
		"""
		Called at the end of the process.
		"""
		print("===== End process =====")
		print(" > Output movie file: %s" % args.outputFile)


### Create command-line interface
parser = argparse.ArgumentParser(
	prog='Sequence to Movie',
	description='''Python script to convert an image sequence to a movie.''',
	)
# requirements
parser.add_argument('inputSequence', help='The input sequence to process.')
parser.add_argument('outputFile', help='The output video filename.')
# options
parser.add_argument("-i", "--interactive", dest="interactive", default=False, action="store_true", help="shell asks questions instead of reading options")
parser.add_argument("-v", "--videoPreset", dest="videoPreset", type=str, help="set output preset : x264-hq, prores-hq, MpegII-intra-hq...")
parser.add_argument("-f", "--fps", dest="fps", type=float, default=25, help="set output movie framerate (default=25)")
parser.add_argument("-w", "--width", dest="width", type=int, help="set output horizontal movie width in pixels: 1920, 1280... (default=same as input)")
parser.add_argument("-r", "--ratio", dest="ratio", type=float, help="set output movie ratio: 1.33 (4/3), 1.77 (16/9), 1.85, 2.35 (cinemascope)... (default=same as input)")
parser.add_argument("-l", "--lut", dest="lut", default=False, action="store_true", help="set lut appied to output movie (disable by default)")
parser.add_argument("-lp", "--lutPath", dest="lutPath", help="path of lut appied to output movie")
parser.add_argument("-t", "--text", dest="text", default=False, action="store_true", help="burn date and sequence name on output movie (disable by default)")

### Parse command-line
args = parser.parse_args()

### Get sequence
sequence = getSequence(args.inputSequence)

if args.interactive:
	interactiveMode()

### Initialize Tuttle graph
tuttle.core().preload(False)
tuttle.core().getFormatter().setLogLevel(tuttle.eVerboseLevelFatal)
graph = tuttle.Graph()
nodes = []
# Set verbose level
options = tuttle.ComputeOptions()
# Set progress handle
progress = ProgressHandle()
options.setProgressHandle(progress)

### Create reader node
nodes.append(graph.createNode(getReaderId(args.inputSequence), filename=args.inputSequence))

# Depend on input, manage color conversion
fileExtension = os.path.splitext(args.inputSequence)[1].lower()
if fileExtension == ".exr":
	nodes.append(graph.createNode("tuttle.gamma", gamma=2.2))
elif fileExtension == ".dpx":
	nodes.append(graph.createNode("tuttle.ocio.lut", filename=args.lutPath))

### Create resize node
if args.width:
	if args.width is "HD":
		nodes.append(graph.createNode("tuttle.resize", format="HD"))
	else:
		nodes.append(graph.createNode("tuttle.resize", width=args.width))

### Create crop node
if args.ratio:
	nodes.append(graph.createNode("tuttle.crop", preset=str(args.ratio)))

### Create lut node
if args.lut:
	nodes.append(graph.createNode("tuttle.ocio.lut", filename=args.lutPath))

### Create text node
if args.text:
	nodes.append(graph.createNode("tuttle.text", text=str(time.strftime("%Y / %m / %d")), vAlign="bottom", hAlign="left", font="utopia"))
       	nodes.append(graph.createNode("tuttle.text", text=args.inputSequence, vAlign="bottom", hAlign="right", font="utopia", bold=True))

### Create writer node
nodes.append(graph.createNode("tuttle.avwriter", filename=args.outputFile, v_preset=(args.videoPreset if args.videoPreset is not None else 'custom'), v_customFps=args.fps))

### Process
if nodes:
	graph.connect(nodes)
	graph.compute(options)
