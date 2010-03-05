#include "InteractScene.hpp"
#include <tuttle/common/utils/global.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

InteractScene::InteractScene( OFX::ParamSet& params, const InteractInfos& infos )
: _params(params)
, _infos(infos)
, _mouseDown(false)
{
	_selected.reserve(50);
}

InteractScene::~InteractScene( ) { }


bool InteractScene::draw( const OFX::DrawArgs& args )
{
	bool result = false;
	IsActiveFunctorVector::iterator itActive = _isActive.begin();
	for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
		 it != itEnd;
		 ++it, ++itActive )
	{
		if( itActive->active() )
			result |= it->draw(args);
	}
	return result;
}

bool InteractScene::penMotion( const OFX::PenArgs& args )
{
//		bool result = false;
//		for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
//		     it != itEnd;
//		     ++it )
//		{
//			result |= it->penMotion(args);
//		}
//		if( _multiselection )
//		{
//			bool isIn = false;
//			for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
//				 it != itEnd;
//				 ++it )
//			{
//				if( it->selectIfIsIn(_selectionRect) )
//				{
//					_selected.push_back( *it );
//					isIn |= true;
//				}
//			}
//			return true;
//		}
//		else
	
	if( ! _mouseDown )
		return false;

	bool moveSomething = false;
	Point2 move = _infos.pointCanonicalXYToNormalizedXXc( ofxToGil( args.penPosition ) );
	switch( _moveType )
	{
		case eMoveTypeXY:
			for( InteractObjectsVectorLink::iterator it = _selected.begin(), itEnd = _selected.end();
				 it != itEnd;
				 ++it )
			{
				moveSomething |= (*it)->moveXYSelected( move );
			}
			break;
		case eMoveTypeX:
			for( InteractObjectsVectorLink::iterator it = _selected.begin(), itEnd = _selected.end();
				 it != itEnd;
				 ++it )
			{
				moveSomething |= (*it)->moveXSelected( move );
			}
			break;
		case eMoveTypeY:
			for( InteractObjectsVectorLink::iterator it = _selected.begin(), itEnd = _selected.end();
				 it != itEnd;
				 ++it )
			{
				moveSomething |= (*it)->moveYSelected( move );
			}
			break;
		case eMoveTypeNone:
			break;
	}
	return moveSomething;
}

bool InteractScene::penDown( const OFX::PenArgs& args )
{
	bool result = false;
	_mouseDown = true;
	_params.beginEditBlock("InteractObjectsGroup");

//		for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
//		     it != itEnd;
//		     ++it )
//		{
//			result |= it->penDown(args);
//		}
	COUT_VAR2(args.penPosition.x, args.penPosition.y);
	bgil::point2<double> penPosition = _infos.pointCanonicalXYToNormalizedXXc( ofxToGil( args.penPosition ) );
	IsActiveFunctorVector::iterator itActive = _isActive.begin();
	for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
		 it != itEnd;
		 ++it, ++itActive )
	{
		if( itActive->active() )
		{
			EMoveType m;
			if( (m = it->selectIfIntesect( penPosition )) != eMoveTypeNone )
			{
				_selected.push_back( &(*it) );
				_moveType = m;
				result = true;
			}
		}
	}
//		if( _multiselectionEnabled )
//		{
//			_multiselection = true;
//			_moveType = eMoveTypeXY;
//			return true;
//		}
	_mouseDown = result;
	return result;
}

bool InteractScene::penUp( const OFX::PenArgs& args )
{
	_mouseDown = false;
	bool result = false;
	IsActiveFunctorVector::iterator itActive = _isActive.begin();
	for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
		 it != itEnd;
		 ++it, ++itActive )
	{
		if( itActive->active() )
			it->unselect();
//			result |= it->penUp(args);
	}
	_params.endEditBlock();
	_selected.clear();
	return result;
}

}
}
}
