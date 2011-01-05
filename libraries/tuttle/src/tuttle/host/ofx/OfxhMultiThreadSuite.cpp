#include "OfxhMultiThreadSuite.hpp"
#include "OfxhCore.hpp"

#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/tss.hpp>
#include <boost/bind.hpp>

struct OfxMutex
{
	boost::recursive_mutex _mutex;
};

namespace tuttle {
namespace host {
namespace ofx {

namespace {

struct ThreadSpecificData
{
	ThreadSpecificData( unsigned int threadIndex ) : _index( 0 ) {}
	unsigned int _index;
};

boost::thread_specific_ptr<ThreadSpecificData> ptr;

void launchThread( OfxThreadFunctionV1 func,
                   unsigned int        threadIndex,
                   unsigned int        threadMax,
                   void*               customArg )
{
	ptr.reset( new ThreadSpecificData( threadIndex ) );
	func( threadIndex, threadMax, customArg );
}

OfxStatus multiThread( OfxThreadFunctionV1 func,
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
			group.create_thread( boost::bind( launchThread, func, i, nThreads, customArg ) );
		}
		group.join_all();
	}
	return kOfxStatOK;
}

OfxStatus multiThreadNumCPUs( unsigned int* const nCPUs )
{
//	*nCPUs = 1; /// @todo tuttle: needs to have an option to disable multithreading (force only one cpu).
	*nCPUs = boost::thread::hardware_concurrency();
	TUTTLE_TCOUT( "nCPUs: " << *nCPUs );
	return kOfxStatOK;
}

OfxStatus multiThreadIndex( unsigned int* const threadIndex )
{
	//	*threadIndex = boost::this_thread::get_id(); //	we don't want a global thead id, but the thead index inside a node multithread process.
	if( ptr.get() != NULL )
	{
		*threadIndex = 0;
		return kOfxStatFailed;
	}
	*threadIndex = ptr->_index;
	return kOfxStatOK;
}

int multiThreadIsSpawnedThread( void )
{
	return ptr.get() != NULL;
}

/**
 * @todo tuttle: support lockCount init value.
 */
OfxStatus mutexCreate( OfxMutexHandle* mutex, const int lockCount )
{
	*mutex = new OfxMutex();
	return kOfxStatOK;
}

OfxStatus mutexDestroy( OfxMutexHandle mutex )
{
	if( mutex == NULL )
		return kOfxStatErrBadHandle;
	delete mutex;
	mutex = NULL;
	return kOfxStatOK;
}

OfxStatus mutexLock( OfxMutexHandle mutex )
{
	if( mutex == NULL )
		return kOfxStatErrBadHandle;
	mutex->_mutex.lock();
	return kOfxStatOK;
}

OfxStatus mutexUnLock( OfxMutexHandle mutex )
{
	if( mutex == NULL )
		return kOfxStatErrBadHandle;
	mutex->_mutex.unlock();
	return kOfxStatOK;
}

OfxStatus mutexTryLock( OfxMutexHandle mutex )
{
	if( mutex == NULL )
		return kOfxStatErrBadHandle;
	if( mutex->_mutex.try_lock() )
		return kOfxStatOK;
	return kOfxStatFailed;
}

struct OfxMultiThreadSuiteV1 gSingleThreadedSuite =
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

}

void* getMultithreadSuite( const int version )
{
	if( version == 1 )
		return &gSingleThreadedSuite;
	return NULL;
}

}
}
}

