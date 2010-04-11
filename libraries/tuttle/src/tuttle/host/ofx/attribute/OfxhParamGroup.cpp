#include "OfxhParamGroup.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

/**
 * setChildrens have to clone each source instance recursively
 */
void OfxhParamGroup::setChildrens( const OfxhParamSet* childrens )
{
	deleteChildrens();

	/// @todo tuttle: use clone ?
	for( ParamList::const_iterator it = childrens->getParamList().begin(), itEnd = childrens->getParamList().end();
	     it != itEnd;
	     ++it )
	{
		_paramList.push_back( it->clone() );
	}
}

void OfxhParamGroup::addChildren( OfxhParam* children )
{
	children->setParamSetInstance( this );
	_paramList.push_back( children );
}

OfxhParamSet* OfxhParamGroup::getChildrens() const
{
	return (OfxhParamSet*)this;
}


}
}
}
}

