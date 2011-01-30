#include "InteractScene.hpp"
#include "overlay.hpp"
#include <tuttle/plugin/image/ofxToGil.hpp>
#include <tuttle/common/utils/global.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

InteractScene::InteractScene( OFX::ParamSet& params, const InteractInfos& infos )
	: _params( params )
	, _infos( infos )
	, _multiSelectionEnabled( true )
	, _mouseDown( false )
	, _beginSelection( false )
{
}

InteractScene::~InteractScene()
{}

bool InteractScene::draw( const OFX::DrawArgs& args )
{
	bool result = false;
	if( _beginSelection )
	{
		glColor4d( 1.0, 1.0, 1.0, 0.5 );
		overlay::displayRect( _selectionRect );
		glColor4d( 1.0, 1.0, 1.0, 1.0 );
		result = true;
	}
	IsActiveFunctorVector::iterator itActive = _isActive.begin();
	for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
	     it != itEnd;
	     ++it, ++itActive )
	{
		if( itActive->active() )
		{
			result |= it->draw( args );
		}
	}
	return result;
}

bool InteractScene::penMotion( const OFX::PenArgs& args )
{
	if( !_mouseDown )
		return false;

	if( _beginSelection )
	{
		// create selection
//		TUTTLE_COUT("create a selection");
		_selectionRect.x2 = args.penPosition.x;
		_selectionRect.y2 = args.penPosition.y;
		_hasSelection = false;

		IsActiveFunctorVector::iterator itActive = _isActive.begin();
		for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
			 it != itEnd;
			 ++it, ++itActive )
		{
			if( ! itActive->active() )
				continue;
			if( it->isIn( _selectionRect ) )
			{
				it->setSelected(true);
				_hasSelection = true;
			}
			else
			{
				it->setSelected(false);
			}
		}
		return true;
	}

	bool moveSomething = false;
	Point2 move        = ofxToGil( args.penPosition );
	switch( _moveType )
	{
		case eMoveTypeXY:
		{
			for( SelectedObjectsLinkVector::iterator it = _selected.begin(), itEnd = _selected.end();
			     it != itEnd;
			     ++it )
			{
				moveSomething |= it->first->moveXYSelected( move + it->second );
			}
			break;
		}
		case eMoveTypeX:
		{
			for( SelectedObjectsLinkVector::iterator it = _selected.begin(), itEnd = _selected.end();
			     it != itEnd;
			     ++it )
			{
				moveSomething |= it->first->moveXSelected( move.x + it->second.x );
			}
			break;
		}
		case eMoveTypeY:
		{
			for( SelectedObjectsLinkVector::iterator it = _selected.begin(), itEnd = _selected.end();
			     it != itEnd;
			     ++it )
			{
				moveSomething |= it->first->moveYSelected( move.y + it->second.y );
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
	const Point2 penPosition = ofxToGil( args.penPosition );
//	TUTTLE_COUT_X( 20, "-" );
//	TUTTLE_COUT("penDown");
	bool result = false;
	_mouseDown = true;
	_moveType  = eMoveTypeNone;
	SelectedObject oneSelectedObj;

	IsActiveFunctorVector::iterator itActive = _isActive.begin();
	for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
	     it != itEnd;
	     ++it, ++itActive )
	{
		if( ! itActive->active() )
			continue;
		EMoveType m;
		if( ( m = it->intersect( args ) ) != eMoveTypeNone )
		{
			// first time
			if( _moveType == eMoveTypeNone )
			{
				oneSelectedObj = SelectedObject( &(*it), it->getDistance( penPosition ) );
				_moveType = m;
			}
			else if( m == eMoveTypeXY ) // if we already register an object X or Y and we found an XY intersection
			{
				oneSelectedObj = SelectedObject( &(*it), it->getDistance( penPosition ) );
				_moveType = m;
			}
			result = true;
			if( m == eMoveTypeXY )
				break;
		}
	}

	if( _hasSelection )
	{
		if( result )
		{
			bool objInSelection = false;
			// compute the offset for each object
			for( SelectedObjectsLinkVector::iterator it = _selected.begin(), itEnd = _selected.end();
				 it != itEnd;
				 ++it )
			{
				it->second = it->first->getDistance( penPosition );
				if( it->first == oneSelectedObj.first )
				{
					objInSelection = true;
				}
			}
			if( !objInSelection )
			{
				_hasSelection = false;
			}
		}
		else
		{
			_hasSelection = false;
		}
	}
	if( ! _hasSelection )
	{
		for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
			 it != itEnd;
			 ++it )
		{
			it->setSelected(false);
		}
		_selected.clear();
		if( result )
		{
			_selected.push_back( oneSelectedObj );
		}
	}

	if( _multiSelectionEnabled )
	{
		if( !result )
		{
			_hasSelection = false;
			_beginSelection = true;
		}
	}
	_mouseDown = true;
	_selectionRect.x1 = args.penPosition.x;
	_selectionRect.y1 = args.penPosition.y;
	_selectionRect.x2 = args.penPosition.x;
	_selectionRect.y2 = args.penPosition.y;
	
	if( _multiSelectionEnabled || result )
	{
		_params.beginEditBlock( "InteractObjectsGroup" );
		return true;
	}
	return false;
}

bool InteractScene::penUp( const OFX::PenArgs& args )
{
//	TUTTLE_COUT("penUp");
	bool result = false;

	if( _beginSelection )
	{
		_selectionRect.x2 = args.penPosition.x;
		_selectionRect.y2 = args.penPosition.y;

		_selected.clear();
//		TUTTLE_COUT_VAR4( _selectionRect.x1, _selectionRect.y1, _selectionRect.x2, _selectionRect.y2 );
		IsActiveFunctorVector::iterator itActive = _isActive.begin();
		for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
			 it != itEnd;
			 ++it, ++itActive )
		{
			if( ! itActive->active() )
				continue;
			if( it->isIn( _selectionRect ) )
			{
				static const Point2 noOffset = Point2(0.0,0.0);
				it->setSelected(true);
				_selected.push_back( SelectedObject( &(*it), noOffset ) );
				_hasSelection = true;
			}
			else
			{
				it->setSelected(false);
			}
		}
//		TUTTLE_COUT_VAR( _selected.size() );
		_beginSelection = false;
	}

	_mouseDown = false;
	_beginSelection = false;
	
	_params.endEditBlock();

//	TUTTLE_COUT_X( 20, "-" );
	return result;
}

}
}
}
