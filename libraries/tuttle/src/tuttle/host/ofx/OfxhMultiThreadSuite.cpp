#include "OfxhMultiThreadSuite.hpp"
#include "OfxhCore.hpp"

namespace tuttle {
namespace host {
namespace ofx {

// boost thread simple example
/*
 #include <boost/thread/thread.hpp>
 #include <boost/thread/thread_group.hpp>
 #include <vector>
// Une variable globale qui pourra être encapsulée au besoin
boost::thread_specific_ptr<int> value;
// Une variable globale avec son mutex, le tout devrait être dans une classe englobante
std::vector<unsigned int> traitement;
boost::mutex mutexTraitement;

void uneFonctionPlusieursThreads()
{
	{
		boost::mutex::scoped_lock lock(mutexTraitement);
		// Accède aux données de traitement en toute sécurité
	}
	value.reset(new int); // On crée des données spécifiques au thread
	
	// Traitement sur value
}

int main()
{
	boost::thread_group group;
	for(unsigned int i = 0; i < 10; ++i)
	{
		group.create_thread(&uneFonctionPlusieursThreads);
	}
	group.join_all();
}
 */

static OfxStatus multiThread( OfxThreadFunctionV1 func,
                              unsigned int        nThreads,
                              void*               customArg )
{
	func( 0, 1, customArg );
	return kOfxStatOK;
}

static OfxStatus multiThreadNumCPUs( unsigned int* nCPUs )
{
	*nCPUs = 1;
	return kOfxStatOK;
}

static OfxStatus multiThreadIndex( unsigned int* threadIndex )
{
	threadIndex = 0;
	return kOfxStatOK;
}

static int multiThreadIsSpawnedThread( void )
{
	return false;
}

static OfxStatus mutexCreate( const OfxMutexHandle* mutex, int lockCount )
{
	// do nothing single threaded
	mutex = 0;
	return kOfxStatOK;
}

static OfxStatus mutexDestroy( const OfxMutexHandle mutex )
{
	// do nothing single threaded
	return kOfxStatOK;
}

static OfxStatus mutexLock( const OfxMutexHandle mutex )
{
	// do nothing single threaded
	return kOfxStatOK;
}

static OfxStatus mutexUnLock( const OfxMutexHandle mutex )
{
	// do nothing single threaded
	return kOfxStatOK;
}

static OfxStatus mutexTryLock( const OfxMutexHandle mutex )
{
	// do nothing single threaded
	return kOfxStatOK;
}

static struct OfxMultiThreadSuiteV1 gSingleThreadedSuite =
{
	multiThread,
	multiThreadNumCPUs,
	multiThreadIndex,
	multiThreadIsSpawnedThread,
	mutexCreate,
	mutexDestroy,
	mutexLock,
	mutexUnLock,
	mutexTryLock
};

void* getMultithreadSuite( const int version )
{
	if( version == 1 )
		return &gSingleThreadedSuite;
	return NULL;
}

}
}
}

