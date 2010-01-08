#include "OfxhMultiThreadSuite.hpp"
#include "OfxhCore.hpp"

#include <tuttle/common/utils/global.hpp>

 #include <boost/thread/thread.hpp>
 #include <boost/bind.hpp>

typedef struct OfxMutex
{
	boost::mutex _mutex;
} OfxMutex;

namespace tuttle {
namespace host {
namespace ofx {

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
 */

static OfxStatus multiThread( OfxThreadFunctionV1 func,
                              const unsigned int  nThreads,
                              void*               customArg )
{
	if( nThreads == 0 )
	{
		return kOfxStatErrValue;
	}
	else if( nThreads == 1 )
	{
		func( 0, 1, customArg );
	}
	else
	{
		boost::thread_group group;
		for( unsigned int i = 0; i < nThreads; ++i )
		{
			group.create_thread(boost::bind(func, i, nThreads, customArg));
		}
		group.join_all();
	}
	return kOfxStatOK;
}

static OfxStatus multiThreadNumCPUs( unsigned int* const nCPUs )
{
	*nCPUs = 1;
//	*nCPUs = boost::thread::hardware_concurrency();
//	COUT( "nCPUs: " << *nCPUs );
	return kOfxStatOK;
}

static OfxStatus multiThreadIndex( unsigned int* const threadIndex )
{
//	*threadIndex = boost::this_thread::get_id();
	*threadIndex = 0;
	return kOfxStatOK;
}

static int multiThreadIsSpawnedThread( void )
{
//	return boost::this_thread::;
	return false;
}

static OfxStatus mutexCreate( OfxMutexHandle* mutex, const int lockCount )
{
	*mutex = new OfxMutex();
	return kOfxStatOK;
}

static OfxStatus mutexDestroy( OfxMutexHandle mutex )
{
	delete mutex;
	mutex = NULL;
	return kOfxStatOK;
}

static OfxStatus mutexLock( OfxMutexHandle mutex )
{
	// do nothing single threaded
	// @todo tuttle multithread
//	boost::mutex::scoped_lock lock(mutex->_mutex);
	return kOfxStatOK;
}

static OfxStatus mutexUnLock( OfxMutexHandle mutex )
{
	// do nothing single threaded
	return kOfxStatOK;
}

static OfxStatus mutexTryLock( OfxMutexHandle mutex )
{
	// do nothing single threaded
//	boost::mutex::scoped_try_lock lock(mutex->_mutex);
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

