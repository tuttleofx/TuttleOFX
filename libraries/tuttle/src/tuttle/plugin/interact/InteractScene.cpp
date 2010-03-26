#include "InteractScene.hpp"
#include <tuttle/common/utils/global.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

InteractScene::InteractScene( OFX::ParamSet& params, const InteractInfos& infos )
: _params( params )
, _infos( infos )
, _mouseDown( false )
{
}

InteractScene::~InteractScene( ) { }


bool InteractScene::draw( const OFX::DrawArgs& args )
{
	COUT_INFOS;
	bool result = false;
	IsActiveFunctorVector::iterator itActive = _isActive.begin();
	for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
		 it != itEnd;
		 ++it, ++itActive )
	{
		if( itActive->active() )
			result |= it->draw(args);
	}
	COUT_INFOS;
	return result;
}

bool InteractScene::penMotion( const OFX::PenArgs& args )
{
	COUT_INFOS;
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
	Point2 move = ofxToGil( args.penPosition );
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
	COUT_INFOS;
	return moveSomething;
}

bool InteractScene::penDown( const OFX::PenArgs& args )
{
	COUT_INFOS;
	bool result = false;
	_mouseDown = true;
	_moveType = eMoveTypeNone;
	_params.beginEditBlock("InteractObjectsGroup");

//		for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
//		     it != itEnd;
//		     ++it )
//		{
//			result |= it->penDown(args);
//		}
	COUT_VAR2(args.penPosition.x, args.penPosition.y);
	Point2 penPosition = ofxToGil( args.penPosition );
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
				if( _moveType == eMoveTypeNone )
				{
					_selected.push_back( &(*it) );
					_moveType = m;
				}
				else if( m == eMoveTypeXY ) // if we already register an object X or Y and we found an XY intersection
				{
					_selected.clear();
					_selected.push_back( &(*it) );
					_moveType = m;
				}
				result = true;
				if( m == eMoveTypeXY )
					break;
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
	COUT_INFOS;
	return result;
}

bool InteractScene::penUp( const OFX::PenArgs& args )
{
	COUT_INFOS;
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
	COUT_INFOS;
	return result;
}

}
}
}
