#include "InteractScene.hpp"
#include <tuttle/plugin/image/ofxToGil.hpp>
#include <tuttle/common/utils/global.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

InteractScene::InteractScene( OFX::ParamSet& params, const InteractInfos& infos )
	: _params( params )
	, _infos( infos )
	, _mouseDown( false )
{}

InteractScene::~InteractScene() {}

bool InteractScene::draw( const OFX::DrawArgs& args )
{
	bool result                              = false;
	IsActiveFunctorVector::iterator itActive = _isActive.begin();

	for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
	     it != itEnd;
	     ++it, ++itActive )
	{
		if( itActive->active() )
			result |= it->draw( args );
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
	//				if( it->isIn(_selectionRect) )
	//				{
	//					_selected.push_back( *it );
	//					isIn |= true;
	//				}
	//			}
	//			return true;
	//		}
	//		else

	if( !_mouseDown )
		return false;

	bool moveSomething = false;
	Point2 move        = ofxToGil( args.penPosition );
	switch( _moveType )
	{
		case eMoveTypeXY:
		{
			for( InteractObjectsVectorLink::iterator it = _selected.begin(), itEnd = _selected.end();
			     it != itEnd;
			     ++it )
			{
				moveSomething |= ( *it )->moveXYSelected( move + _moveOffset );
			}
			break;
		}
		case eMoveTypeX:
		{
			for( InteractObjectsVectorLink::iterator it = _selected.begin(), itEnd = _selected.end();
			     it != itEnd;
			     ++it )
			{
				moveSomething |= ( *it )->moveXSelected( move.x + _moveOffset.x );
			}
			break;
		}
		case eMoveTypeY:
		{
			for( InteractObjectsVectorLink::iterator it = _selected.begin(), itEnd = _selected.end();
			     it != itEnd;
			     ++it )
			{
				moveSomething |= ( *it )->moveYSelected( move.y + _moveOffset.y );
			}
			break;
		}
		case eMoveTypeNone:
		{
			break;
		}
	}
	return moveSomething;
}

bool InteractScene::penDown( const OFX::PenArgs& args )
{
	bool result = false;

	_mouseDown = true;
	_moveType  = eMoveTypeNone;
	_params.beginEditBlock( "InteractObjectsGroup" );

//		for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
//		     it != itEnd;
//		     ++it )
//		{
//			result |= it->penDown(args);
//		}
	Point2 penPosition = ofxToGil( args.penPosition );
	IsActiveFunctorVector::iterator itActive = _isActive.begin();
	for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
	     it != itEnd;
	     ++it, ++itActive )
	{
		if( itActive->active() )
		{
			EMoveType m;
			Point2 offset = Point2(0.0,0.0);
			if( ( m = it->intersect( args, offset ) ) != eMoveTypeNone )
			{
				// first time
				if( _moveType == eMoveTypeNone )
				{
					_selected.push_back( &( *it ) );
					_moveType = m;
					_moveOffset = offset;
				}
				else if( m == eMoveTypeXY ) // if we already register an object X or Y and we found an XY intersection
				{
					_selected.clear();
					_selected.push_back( &( *it ) );
					_moveType = m;
					_moveOffset = offset;
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
	return result;
}

bool InteractScene::penUp( const OFX::PenArgs& args )
{
	_mouseDown = false;
	bool result                              = false;
	IsActiveFunctorVector::iterator itActive = _isActive.begin();
	for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
	     it != itEnd;
	     ++it, ++itActive )
	{
		if( itActive->active() )
		{
			it->setSelected(false);
//			result |= it->penUp(args);
		}
	}
	_selected.clear();
	_moveOffset = Point2(0.0,0.0);
	_params.endEditBlock();
	return result;
}

}
}
}
