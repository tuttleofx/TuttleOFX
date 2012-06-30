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
	InputBufferNode( );
	~InputBufferNode( );

private:
	InputBufferNode( const InputBufferNode& orig );
	
public:
	InputBufferNode* clone() const
	{
		return new InputBufferNode( *this );
	}
	
public:
	static const std::string _label;
	std::string _name;
#ifndef SWIG
	attribute::ClipImage _outputClip;
	
	memory::CACHE_ELEMENT _imageCache;
#endif
	ofx::property::OfxhSet _emptyProps;
	
	bool operator==( const INode& other ) const;
	bool operator==( const InputBufferNode& other ) const;
	
	const std::string& getLabel() const { return _label; }
	const std::string& getName() const { return _name; }
	void setName( const std::string& name ) { _name = name; }
	const ENodeType    getNodeType() const { return eNodeTypeBuffer; }

	std::vector<int> getVersion() const;

	const ofx::property::OfxhSet& getProperties() const { return _emptyProps; }
	ofx::property::OfxhSet&       getEditableProperties() { return _emptyProps; }

	attribute::Attribute& getAttribute( const std::string& name );
	attribute::Attribute&       getSingleInputAttribute();
	const attribute::Attribute& getSingleInputAttribute() const;
	ofx::attribute::OfxhParam&       getParam( const std::string& name );
	const ofx::attribute::OfxhParam& getParam( const std::string& name ) const;
	ofx::attribute::OfxhParam&       getParamByScriptName( const std::string& name, const bool acceptPartialName = false );
	const ofx::attribute::OfxhParam& getParamByScriptName( const std::string& name, const bool acceptPartialName = false ) const;
	ofx::attribute::OfxhParam&       getParam( const std::size_t index );
	const ofx::attribute::OfxhParam& getParam( const std::size_t index ) const;

	attribute::ClipImage&       getClip( const std::string& name, const bool acceptPartialName = false );
	const attribute::ClipImage& getClip( const std::string& name, const bool acceptPartialName = false ) const;

	ofx::attribute::OfxhParamSet& getParamSet();
	const ofx::attribute::OfxhParamSet& getParamSet() const;

	ofx::attribute::OfxhClipImageSet& getClipImageSet();
	const ofx::attribute::OfxhClipImageSet& getClipImageSet() const;

	void setRawImageBuffer(
			char* rawBuffer,
			const OfxRectD& rod,
			const ofx::imageEffect::EPixelComponent components,
			const ofx::imageEffect::EBitDepth bitDepth,
			const int rowDistanceBytes,
			const attribute::Image::EImageOrientation orientation = attribute::Image::eImageOrientationFromTopToBottom );
	
	void setRawImageBuffer(
			unsigned char* rawBuffer,
			const OfxRectD& rod,
			const ofx::imageEffect::EPixelComponent components,
			const int rowDistanceBytes,
			const attribute::Image::EImageOrientation orientation = attribute::Image::eImageOrientationFromTopToBottom )
	{
		setRawImageBuffer( reinterpret_cast<char*>(rawBuffer),
			rod,
			components,
			ofx::imageEffect::eBitDepthUByte,
			rowDistanceBytes,
			orientation );
	}
	void setRawImageBuffer(
			unsigned short* rawBuffer,
			const OfxRectD& rod,
			const ofx::imageEffect::EPixelComponent components,
			const int rowDistanceBytes,
			const attribute::Image::EImageOrientation orientation = attribute::Image::eImageOrientationFromTopToBottom )
	{
		setRawImageBuffer( reinterpret_cast<char*>(rawBuffer),
			rod,
			components,
			ofx::imageEffect::eBitDepthUShort,
			rowDistanceBytes,
			orientation );
	}
	void setRawImageBuffer(
			float* rawBuffer,
			const OfxRectD& rod,
			const ofx::imageEffect::EPixelComponent components,
			const int rowDistanceBytes,
			const attribute::Image::EImageOrientation orientation = attribute::Image::eImageOrientationFromTopToBottom )
	{
		setRawImageBuffer( reinterpret_cast<char*>(rawBuffer),
			rod,
			components,
			ofx::imageEffect::eBitDepthFloat,
			rowDistanceBytes,
			orientation );
	}
	
#if 0
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

	std::size_t getClipBitDepthSize() const
	{
		using namespace ofx::imageEffect;
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


private:

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
	void setPixelComponent( const ofx::imageEffect::EPixelComponent v )
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

	/** @brief set pixels bit depth */
	void setBitDepth( const ofx::imageEffect::EBitDepth v )
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
	
	void setRod( const OfxRectD& rod )
	{
		_rod = rod;
	}
	
	void setRowBytes( const int rowDistanceBytes )
	{
		_rowAbsDistanceBytes = rowDistanceBytes;
		_outputClip.getEditableProperties().setDoubleProperty( kOfxImagePropRowBytes, rowDistanceBytes );
	}
	
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
#endif


public:
	#ifndef SWIG
	void connect( const INode&, attribute::Attribute& ) {}

	OfxRectD getRegionOfDefinition( const OfxTime time ) const
	{
		return getData(time)._apiImageEffect._renderRoD;
	}

	void getTimeDomain( OfxRangeD& range ) const { range.min=kOfxFlagInfiniteMin; range.max=kOfxFlagInfiniteMax; }

	/**
	 * @brief Begin of the a new frame range to process. Initilize this node.
	 * @param[in] vData
	 * @remark called on each node without predefined order.
	 */
	void beginSequence( graph::ProcessVertexData& vData )
	{}

	/**
	 * @brief Asks the plugin all times it needs for each of it's input clips.
	 * @param[in] time
	 */
	ClipTimesSetMap getTimesNeeded( const OfxTime time ) const { return ClipTimesSetMap(); }

	/**
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @param[in] vData
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	void preProcess1( graph::ProcessVertexAtTimeData& vData )
	{
		const OfxRectI rod = _imageCache->getROD();
		vData._apiImageEffect._renderRoD.x1 = rod.x1;
		vData._apiImageEffect._renderRoD.x2 = rod.x2;
		vData._apiImageEffect._renderRoD.y1 = rod.y1;
		vData._apiImageEffect._renderRoD.y2 = rod.y2;
		vData._apiImageEffect._renderRoI = vData._apiImageEffect._renderRoD;
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
	void preProcess_infos( const graph::ProcessVertexAtTimeData& vData, const OfxTime time, graph::ProcessVertexAtTimeInfo& nodeInfos ) const {}

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

