#include "OfxhInteractSuite.hpp"
#include "OfxhInteract.hpp"


namespace tuttle {
namespace host {
namespace ofx {
namespace interact {

static OfxStatus interactSwapBuffers( OfxInteractHandle handle )
{
	interact::OfxhInteract* interactInstance = reinterpret_cast<interact::OfxhInteract*>( handle );

	if( interactInstance )
		return interactInstance->swapBuffers();
	else
		return kOfxStatErrBadHandle;
}

static OfxStatus interactRedraw( OfxInteractHandle handle )
{
	interact::OfxhInteract* interactInstance = reinterpret_cast<interact::OfxhInteract*>( handle );

	if( interactInstance )
		return interactInstance->redraw();
	else
		return kOfxStatErrBadHandle;
}

static OfxStatus interactGetPropertySet( OfxInteractHandle handle, OfxPropertySetHandle* property )
{
	interact::OfxhInteractBase* interact = reinterpret_cast<interact::OfxhInteractBase*>( handle );

	if( interact )
	{
		*property = interact->getPropHandle();
		return kOfxStatOK;
	}
	return kOfxStatErrBadHandle;
}

/// the interact suite
static OfxInteractSuiteV1 gSuite = {
	interactSwapBuffers,
	interactRedraw,
	interactGetPropertySet
};

/// function to get the interact suite
void* getInteractSuite( const int version )
{
	if( version == 1 )
		return static_cast<void*>(&gSuite);
	return NULL;
}

}
}
}
}

