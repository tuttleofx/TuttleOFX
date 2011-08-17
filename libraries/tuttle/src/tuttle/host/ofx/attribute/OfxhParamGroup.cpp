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
	for( ParamVector::const_iterator it = childrens->getParamVector().begin(), itEnd = childrens->getParamVector().end();
	     it != itEnd;
	     ++it )
	{
		_paramVector.push_back( it->clone() );
	}
}

void OfxhParamGroup::addChildren( OfxhParam* children )
{
	children->setParamSetInstance( this );
	_paramVector.push_back( children );
}

OfxhParamSet* OfxhParamGroup::getChildrens() const
{
	return (OfxhParamSet*)this;
}

}
}
}
}

