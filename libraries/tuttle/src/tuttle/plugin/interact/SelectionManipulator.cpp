#include "SelectionManipulator.hpp"

namespace tuttle {
namespace plugin {
namespace interact {

SelectionManipulator::SelectionManipulator( const InteractInfos& infos )
	: _infos( infos )
	, _selectionPos( 0, 0 )
{
}


SelectionManipulator::~SelectionManipulator()
{
}

}
}
}
