from pyTuttle import tuttle
import time


def setUp():
	print("testComputeTime setUp")
	tuttle.core().preload(False)


def testComputeTime():
	nbFrames = 10
	N = 0
	print("testComputeTime %d" % N); N += 1
	gA = tuttle.Graph()
	gA.createNode( "tuttle.avreader", filename="TuttleOFX-data/video/bars_100.avi", v_colorspace=2 )

	print("testComputeTime %d" % N); N += 1
	t0 = time.clock()
	time0 = time.time()
	print("testComputeTime %d" % N); N += 1
	gA.compute( tuttle.ComputeOptions(0, nbFrames) )
	t1 = time.clock()
	time1 = time.time()
	
	print("testComputeTime %d" % N); N += 1
	rangeCompute_duration = t1-t0
	rangeCompute_timeDuration = time1-time0
	
	print("testComputeTime %d" % N); N += 1

	gB = tuttle.Graph()
	
	gB.createNode( "tuttle.avreader", filename="TuttleOFX-data/video/bars_100.avi", v_colorspace=2 )
	print("testComputeTime %d" % N); N += 1
	t0 = time.clock()
	time0 = time.time()
	print("compute frame by frame")
	for i in range(0, nbFrames):
		print("compute frame %d" % i)
		gB.compute( tuttle.ComputeOptions(i) )
	t1 = time.clock()
	time1 = time.time()
	
	print("testComputeTime %d" % N); N += 1
	frameByFrameCompute_duration = t1-t0
	frameByFrameCompute_timeDuration = time1-time0
	
	print("testComputeTime %d" % N); N += 1
	diff = frameByFrameCompute_duration - rangeCompute_duration
	diffTime = frameByFrameCompute_timeDuration - rangeCompute_timeDuration
	
	print("_"*10)
	print("rangeCompute_duration:", rangeCompute_duration)
	print("frameByFrameCompute_duration:", frameByFrameCompute_duration)
	print("diff duration:", diff)
	
	print("-"*10)
	print("rangeCompute_timeDuration:", rangeCompute_timeDuration)
	print("frameByFrameCompute_timeDuration:", frameByFrameCompute_timeDuration)
	print("diff time duration:", diffTime)
	
	print("_"*10)
		
	print("testComputeTime %d" % N); N += 1
	#assert(False)
