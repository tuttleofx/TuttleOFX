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
	, _mouseDown( false )
	, _multiSelectionEnabled( true )
	, _creatingSelection( false )
	, _manipulator( infos )
{
}

InteractScene::~InteractScene()
{}

bool InteractScene::draw( const OFX::DrawArgs& args )
{
	bool result = false;

	result |= drawSelection( args );

	IsActiveFunctorVector::iterator itActive = _isActive.begin();
	ColorVector::iterator itColor = _colors.begin();
	for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
	     it != itEnd;
	     ++it, ++itActive, ++itColor )
	{
		if( itActive->active() )
		{
			OfxRGBAColourD color = itColor->getColor( args.time );
			glColor4d( color.r, color.g, color.b, color.a );
			result |= it->draw( args );
		}
	}
	return result;
}

bool InteractScene::penMotion( const OFX::PenArgs& args )
{
	if( !_mouseDown )
		return false;

	if( _creatingSelection )
	{
		// create selection
		TUTTLE_COUT("create a selection");
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

	if( _selected.size() == 0 )
	{
		TUTTLE_COUT_INFOS;
		return false;
	}
	TUTTLE_COUT_INFOS;

	const Point2 penPosition = ofxToGil( args.penPosition );
	switch( _motionType._mode )
	{
		case eMotionTranslate:
		{
			translate( penPosition - _beginPenPosition );
			break;
		}
		case eMotionRotate:
		{
			TUTTLE_COUT_INFOS;
			// todo
			//rotate( center, angle );
			break;
		}
		case eMotionScale:
		{
			TUTTLE_COUT_INFOS;
			// todo
			//scale( angle, factor );
			break;
		}
		case eMotionNone:
		{
			TUTTLE_COUT_INFOS;
			break;
		}
	}
	return true;
}

bool InteractScene::penDown( const OFX::PenArgs& args )
{
	const Point2 penPosition = ofxToGil( args.penPosition );
	_beginPenPosition = penPosition;
	
//	TUTTLE_COUT_X( 20, "-" );
//	TUTTLE_COUT("penDown");
	bool result = false;
	_mouseDown = true;
	_motionType._mode = eMotionNone;
	_motionType._axis = eAxisNone;
	SelectedObject oneSelectedObj;

//	if( _hasSelection  )
//	{
//		_motionType = _manipulator.intersect( args );
//		if( _motionType._mode != eMotionNone )
//		{
//			result = true;
//		}
//	}
	if( !result )
	{
		IsActiveFunctorVector::iterator itActive = _isActive.begin();
		for( InteractObjectsVector::iterator it = _objects.begin(), itEnd = _objects.end();
			 it != itEnd;
			 ++it, ++itActive )
		{
			if( ! itActive->active() )
				continue;
			MotionType m = it->intersect( args );
			if( m._axis != eAxisNone )
			{
				// first time
				if( _motionType._axis == eAxisNone )
				{
					oneSelectedObj = SelectedObject( &(*it), it->getPosition() );
					_motionType = m;
				}
				else if( m._axis == eAxisXY ) // if we already register an object X or Y and we found an XY intersection
				{
					oneSelectedObj = SelectedObject( &(*it), it->getPosition() );
					_motionType = m;
				}
				result = true;
				if( m._axis == eAxisXY )
					break;
			}
		}
	}

	if( _hasSelection )
	{
		if( result )
		{
			bool objInSelection = false;
			// compute the offset for each object
			for( SelectedObjectVector::iterator it = _selected.begin(), itEnd = _selected.end();
				 it != itEnd;
				 ++it )
			{
				it->second = it->first->getPosition();
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
			_creatingSelection = true;
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

	if( _creatingSelection )
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
				it->setSelected(true);
				_selected.push_back( SelectedObject( &(*it), it->getPosition() ) );
				_hasSelection = true;
				result = true;
			}
			else
			{
				it->setSelected(false);
			}
		}
//		TUTTLE_COUT_VAR( _selected.size() );
		_creatingSelection = false;
	}

	_mouseDown = false;
	_creatingSelection = false;
	
	_params.endEditBlock();

//	TUTTLE_COUT_X( 20, "-" );
	return result;
}


bool InteractScene::drawSelection( const OFX::DrawArgs& args )
{
	bool result = false;
	if( _creatingSelection )
	{
		glColor4d( 1.0, 1.0, 1.0, 0.5 );
		overlay::displayRect( _selectionRect );
		glColor4d( 1.0, 1.0, 1.0, 1.0 );
		result = true;
	}
	else if( _hasSelection /*&& _manipulator*/ )
	{
		result |= _manipulator.draw( args );
	}
	return result;
}

void InteractScene::translate( const Point2& vec )
{
	switch( _axis )
	{
		case eAxisXY:
		{
			for( SelectedObjectVector::iterator it = _selected.begin(), itEnd = _selected.end();
			     it != itEnd;
			     ++it )
			{
				it->first->setPositionXY( it->second + vec );
			}
			break;
		}
		case eAxisX:
		{
			for( SelectedObjectVector::iterator it = _selected.begin(), itEnd = _selected.end();
			     it != itEnd;
			     ++it )
			{
				it->first->setPositionX( it->second.x + vec.x );
			}
			break;
		}
		case eAxisY:
		{
			for( SelectedObjectVector::iterator it = _selected.begin(), itEnd = _selected.end();
			     it != itEnd;
			     ++it )
			{
				it->first->setPositionY( it->second.y + vec.y );
			}
			break;
		}
		case eAxisNone:
		{
			break;
		}
	}
}

void InteractScene::rotate( const Point2& center, const Scalar angle )
{
	for( SelectedObjectVector::iterator it = _selected.begin(), itEnd = _selected.end();
		 it != itEnd;
		 ++it )
	{
//		Point2 nPt = rotate( it->second, center, angle );
		Point2 nPt = it->second;
		it->first->setPositionXY( nPt );
	}
}

void InteractScene::scale( const Point2& center, const Scalar factor )
{
	switch( _axis )
	{
		case eAxisXY:
		{
			for( SelectedObjectVector::iterator it = _selected.begin(), itEnd = _selected.end();
			     it != itEnd;
			     ++it )
			{
				it->first->setPositionXY( it->second );
			}
			break;
		}
		case eAxisX:
		{
			for( SelectedObjectVector::iterator it = _selected.begin(), itEnd = _selected.end();
			     it != itEnd;
			     ++it )
			{
				it->first->setPositionX( it->second.x );
			}
			break;
		}
		case eAxisY:
		{
			for( SelectedObjectVector::iterator it = _selected.begin(), itEnd = _selected.end();
			     it != itEnd;
			     ++it )
			{
				it->first->setPositionY( it->second.y );
			}
			break;
		}
		case eAxisNone:
		{
			break;
		}
	}
}

}
}
}
