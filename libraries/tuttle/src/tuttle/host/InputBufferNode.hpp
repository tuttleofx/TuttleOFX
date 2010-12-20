#ifndef _TUTTLE_HOST_INPUTBUFFERNODE_HPP_
#define	_TUTTLE_HOST_INPUTBUFFERNODE_HPP_

#include "INode.hpp"
#include <tuttle/host/attribute/Param.hpp>
#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/ofx/attribute/OfxhClipImageDescriptor.hpp>
#include <tuttle/host/graph/ProcessVertexData.hpp>
#include <tuttle/host/graph/ProcessVertexAtTimeData.hpp>

namespace tuttle {
namespace host {

class InputBufferNode : public INode
{
public:
	/** @brief Enumerates the pixel depths supported */
	enum EBitDepth
	{
		eBitDepthCustom = -1, ///< some non standard bit depth
		eBitDepthNone = 0, ///< bit depth that indicates no data is present
		eBitDepthUByte = 1,
		eBitDepthUShort = 2,
		eBitDepthFloat = 3
	};

	/** @brief Enumerates the component types supported */
	enum EPixelComponent
	{
		ePixelComponentNone,
		ePixelComponentRGB,
		ePixelComponentRGBA,
		ePixelComponentAlpha,
		ePixelComponentCustom ///< some non standard pixel type
	};

	/** @brief Enumerates the ways a fielded image can be extracted from a clip */
	enum EFieldExtraction
	{
		eFieldExtractBoth,   /**< @brief extract both fields */
		eFieldExtractSingle, /**< @brief extracts a single field, so you have a half height image */
		eFieldExtractDoubled /**< @brief extracts a single field, but doubles up the field, so you have a full height image */
	};

	const std::string mapBitDepthEnumToString( const EBitDepth e )
	{
		switch(e)
		{
			case eBitDepthUByte:
				return kOfxBitDepthByte;
			case eBitDepthUShort:
				return kOfxBitDepthShort;
			case eBitDepthFloat:
				return kOfxBitDepthFloat;
			case eBitDepthNone:
				return kOfxBitDepthNone;
			case eBitDepthCustom:
				return "eBitDepthCustom";
		}
		BOOST_THROW_EXCEPTION( exception::Bug()
			<< exception::dev( "EBitDepth: " + boost::lexical_cast<std::string>(e) ) );
		return kOfxBitDepthNone;
	}

	/** @brief turns a pixel component string into and enum */
	EPixelComponent mapPixelComponentStringToEnum( const std::string& str )
	{
		if( str == kOfxImageComponentRGBA )
		{
			return ePixelComponentRGBA;
		}
		else if( str == kOfxImageComponentRGB )
		{
			return ePixelComponentRGB;
		}
		else if( str == kOfxImageComponentAlpha )
		{
			return ePixelComponentAlpha;
		}
		else if( str == kOfxImageComponentNone )
		{
			return ePixelComponentNone;
		}
		else
		{
			return ePixelComponentCustom;
		}
	}

	std::string mapPixelComponentEnumToString( const EPixelComponent e )
	{
		switch(e)
		{
			case ePixelComponentRGBA:
				return kOfxImageComponentRGBA;
			case ePixelComponentRGB:
				return kOfxImageComponentRGB;
			case ePixelComponentAlpha:
				return kOfxImageComponentAlpha;
			case ePixelComponentNone:
				return kOfxImageComponentNone;
			case ePixelComponentCustom:
				return "ePixelComponentCustom";
		}
		BOOST_THROW_EXCEPTION( exception::Bug()
			<< exception::dev( "EPixelComponent: " + boost::lexical_cast<std::string>(e) ) );
	}

public:
	InputBufferNode( );
	InputBufferNode( const InputBufferNode& orig );
	~InputBufferNode( );

	InputBufferNode* clone() const
	{
		return new InputBufferNode( *this );
	}
public:
	static const std::string _label;
	std::string _name;
	attribute::ClipImage _outputClip;

	OfxRectD _rod;
	EPixelComponent _pixelComponent;
	EBitDepth _bitDepth;
	char* _rawBuffer;
	
	const std::string& getLabel() const     { return _label; }
	const std::string& getName() const     { return _name; }
	void setName( const std::string& name ) { _name = name; }
	const ENodeType    getNodeType() const { return eNodeTypeBuffer; }

	attribute::Attribute& getAttribute( const std::string& name );
	attribute::Attribute&       getSingleInputAttribute();
	const attribute::Attribute& getSingleInputAttribute() const;
	const ofx::attribute::OfxhParam& getParam( const std::string& name ) const;
	ofx::attribute::OfxhParam&       getParam( const std::string& name );

	attribute::ClipImage&       getClip( const std::string& name );
	const attribute::ClipImage& getClip( const std::string& name ) const;

	ofx::attribute::OfxhParamSet& getParamSet();
	const ofx::attribute::OfxhParamSet& getParamSet() const;






	void setClipRawBuffer( char* rawBuffer )
	{
		_rawBuffer = rawBuffer;
	}

	/** @brief set how fielded images are extracted from the clip defaults to eFieldExtractDoubled */
	void setClipFieldExtraction( EFieldExtraction v )
	{
		switch( v )
		{
			case eFieldExtractBoth:
				_outputClip.getEditableProperties().setStringProperty( kOfxImageClipPropFieldExtraction, kOfxImageFieldBoth );
				break;

			case eFieldExtractSingle:
				_outputClip.getEditableProperties().setStringProperty( kOfxImageClipPropFieldExtraction, kOfxImageFieldSingle );
				break;

			case eFieldExtractDoubled:
				_outputClip.getEditableProperties().setStringProperty( kOfxImageClipPropFieldExtraction, kOfxImageFieldDoubled );
				break;
		}
	}

	/** @brief set which components  is used, defaults to none set, this must be called at least once! */
	void setClipComponent( const EPixelComponent v )
	{
		_pixelComponent = v;
		switch( v )
		{
			case ePixelComponentRGBA:
				setClipComponent( kOfxImageComponentRGBA );
				break;
			case ePixelComponentRGB:
				setClipComponent( kOfxImageComponentRGB );
				break;
			case ePixelComponentAlpha:
				setClipComponent( kOfxImageComponentAlpha );
				break;
			case ePixelComponentCustom:
				break;
			case ePixelComponentNone:
				setClipComponent( kOfxImageComponentNone );
				break;
		}
	}

	std::size_t getClipNbComponents() const
	{
		switch( _pixelComponent )
		{
			case ePixelComponentRGBA:
				return 4;
			case ePixelComponentRGB:
				return 3;
			case ePixelComponentAlpha:
				return 1;
			case ePixelComponentCustom:
			case ePixelComponentNone:
				break;
		}
		return 0;
	}

	/** @brief set which component is used, defaults to none set, this must be called at least once! */
	void setClipBitDepth( const EBitDepth v )
	{
		_bitDepth = v;
		switch( v )
		{
			case eBitDepthFloat:
				setClipBitDepth( kOfxBitDepthFloat );
				break;
			case eBitDepthUShort:
				setClipBitDepth( kOfxBitDepthShort );
				break;
			case eBitDepthUByte:
				setClipBitDepth( kOfxBitDepthByte );
				break;
			case eBitDepthCustom:
			case eBitDepthNone:
				setClipBitDepth( kOfxBitDepthNone );
				break;
		}
	}

	std::size_t getClipBitDepthSize() const
	{
		switch( _bitDepth )
		{
			case eBitDepthFloat:
				return 4;
			case eBitDepthUShort:
				return 2;
			case eBitDepthUByte:
				return 1;
			case eBitDepthCustom:
			case eBitDepthNone:
				break;
		}
		return 0;
	}

	void setClipRod( const OfxRectD rod )
	{
		_rod = rod;
	}

	void updateClipInfos()
	{
//		_outputClip.getEditableProperties().setDoubleProperty( kOfxImagePropRowBytes, (_rod.x2 - _rod.x1) * getClipNbComponents() * getClipBitDepthSize() );
	}

private:
	/** @brief set which components is used, defaults to none set, this must be called at least once! */
	void setClipComponent( const std::string& v )
	{
//		_outputClip.getEditableProperties().setStringProperty( kOfxImageEffectPropSupportedComponents, v );
		_outputClip.getEditableProperties().setStringProperty( kOfxImageEffectPropComponents, v );
		_outputClip.getEditableProperties().setStringProperty( kOfxImageClipPropUnmappedComponents, v );
	}

	/** @brief set which bit depth is used, defaults to none set, this must be called at least once! */
	void setClipBitDepth( const std::string& v )
	{
//		_outputClip.getEditableProperties().setStringProperty( kOfxImageEffectPropSupportedPixelDepths, v );
		_outputClip.getEditableProperties().setStringProperty( kOfxImageEffectPropPixelDepth, v );
		_outputClip.getEditableProperties().setStringProperty( kOfxImageClipPropUnmappedPixelDepth, v );
	}









public:
	#ifndef SWIG
	void connect( const INode&, attribute::Attribute& ) {}

	OfxRectD getRegionOfDefinition( const OfxTime time ) const
	{
		return getData(time)._apiImageEffect._renderRoD;
	}

	bool getTimeDomain( OfxRangeD& range ) const { return false; }

	/**
	 * @brief Begin of the a new frame range to process. Initilize this node.
	 * @param[in] vData
	 * @remark called on each node without predefined order.
	 */
	void beginSequence( graph::ProcessVertexData& vData )
	{
		updateClipInfos();
	}

	/**
	 * @brief Asks the plugin all times it needs for each of it's input clips.
	 * @param[in] time
	 */
	InputsTimeMap getTimesNeeded( const OfxTime time ) const { return InputsTimeMap(); }

	/**
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @param[in] vData
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	void preProcess1( graph::ProcessVertexAtTimeData& vData )
	{
		vData._apiImageEffect._renderRoD = _rod;
		vData._apiImageEffect._renderRoI = _rod;
	}

	/**
	 * @brief Initialization pass to propagate informations from outputs to inputs.
	 * @param[in] vData
	 * @remark Called on each node in a REVERSE depth first search order. So you have the guarantee that it has been called on each output nodes before. Output nodes are those who used the result of the current node.
	 */
	void preProcess2_reverse( graph::ProcessVertexAtTimeData& vData ) {}

	/**
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @param[in] vData
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	void preProcess3( graph::ProcessVertexAtTimeData& vData ) {}

	/**
	 * @brief The node can declare to be an identity operation.
	 * In this case, the node is not processed and the rendering engine direcly use the indicated input clip at a particular time.
	 *
	 * @param[in] vData
	 * @param[out] clip the input clip to use as identity
	 * @param[out] time the time to use as identity
	 * @return if the node is an identity operation
	 */
	bool isIdentity( const graph::ProcessVertexAtTimeData& vData, std::string& clip, OfxTime& time ) const { return false; }

	/**
	 * @brief Fill ProcessInfo to compute statistics for the current process,
	 *        like memory used by this node, by all input nodes, etc.
	 * @param[in] vData
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	void preProcess_infos( const OfxTime time, graph::ProcessVertexAtTimeInfo& nodeInfos ) const {}

	/**
	 * @brief Process this node. All inputs are compute.
	 * @param[in] vData
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	void process( graph::ProcessVertexAtTimeData& vData );

	/**
	 * @brief The process of all nodes is done for one frame, now finalize this node.
	 * @param[in] vData
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	void postProcess( graph::ProcessVertexAtTimeData& vData ) {}

	/**
	 * @brief End of the whole frame range process, now finalize this node.
	 * @param[in] vData
	 * @remark called on each node without predefined order.
	 */
	void endSequence( graph::ProcessVertexData& vData ) {}

	std::ostream& print( std::ostream& os ) const;

	friend std::ostream& operator<<( std::ostream& os, const This& v );
	#endif

private:

};

}
}

#endif

