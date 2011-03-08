#ifndef _TUTTLE_HOST_INODE_HPP_
#define _TUTTLE_HOST_INODE_HPP_

#include <tuttle/host/exceptions.hpp>

#include <ofxCore.h>
#include <ofxAttribute.h>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

namespace tuttle {
namespace host {

namespace ofx {
namespace attribute {
class OfxhParam;
class OfxhParamSet;
}
}
namespace attribute {
class Attribute;
class Clip;
class ClipImage;
}
namespace graph {
class ProcessVertexAtTimeData;
class ProcessVertexData;
class ProcessVertexAtTimeInfo;
}

class ImageEffectNode;
class InputBufferNode;


class INode
{
public:
	typedef INode This;
	typedef std::set<OfxTime> TimesSet;
	typedef std::map<std::string, TimesSet> InputsTimeMap;
	enum ENodeType
	{
		eNodeTypeUnknown,
		eNodeTypeImageEffect,
		eNodeTypeParam,
		eNodeTypeGraph,
		eNodeTypeBuffer,
	};
	
public:
	INode() {}
	INode( const INode& e ) {}
	virtual ~INode()                                = 0;
	virtual INode*              clone() const       = 0;
	virtual const std::string& getName() const     = 0;
	virtual void setName( const std::string& name ) = 0;
	virtual const ENodeType    getNodeType() const = 0;

	ImageEffectNode& asImageEffectNode();
	const ImageEffectNode& asImageEffectNode() const;

	InputBufferNode& asInputBufferNode();
	const InputBufferNode& asInputBufferNode() const;

	virtual attribute::Attribute& getAttribute( const std::string& name ) = 0;
	//	const attribute::Attribute& getAttribute( const std::string& name ) const { return const_cast<ProcessNode*>(this)->getAttribute( name ); }
	virtual attribute::Attribute&       getSingleInputAttribute()       = 0;
	virtual const attribute::Attribute& getSingleInputAttribute() const = 0;
	
	virtual const ofx::attribute::OfxhParam& getParam( const std::string& name ) const = 0;
	virtual ofx::attribute::OfxhParam&       getParam( const std::string& name ) = 0;

	virtual attribute::ClipImage&       getClip( const std::string& name ) = 0;
	virtual const attribute::ClipImage& getClip( const std::string& name ) const = 0;

	attribute::ClipImage&       getOutputClip()       { return getClip( kOfxOutputAttributeName ); }
	const attribute::ClipImage& getOutputClip() const { return getClip( kOfxOutputAttributeName ); }

	virtual ofx::attribute::OfxhParamSet& getParamSet() = 0;
	virtual const ofx::attribute::OfxhParamSet& getParamSet() const = 0;

	#ifndef SWIG
	virtual void connect( const INode&, attribute::Attribute& ) = 0;

	virtual bool getTimeDomain( OfxRangeD& range ) const = 0;

	/**
	 * @brief Begin of the a new frame range to process. Initilize this node.
	 * @param[in] processOptions
	 * @remark called on each node without predefined order.
	 */
	virtual void beginSequence( graph::ProcessVertexData& processOptions ) = 0;

	/**
	 * @brief Asks the plugin all times it needs for each of it's input clips.
	 * @param[in] time
	 */
	virtual InputsTimeMap getTimesNeeded( const OfxTime time ) const = 0;

	/**
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	virtual void preProcess1( graph::ProcessVertexAtTimeData& processOptions ) {}
	
	/**
	 * @brief Initialization pass to propagate informations from outputs to inputs.
	 * @param[in] processOptions
	 * @remark Called on each node in a REVERSE depth first search order. So you have the guarantee that it has been called on each output nodes before. Output nodes are those who used the result of the current node.
	 */
	virtual void preProcess2_reverse( graph::ProcessVertexAtTimeData& processOptions ) {}

	/**
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	virtual void preProcess3( graph::ProcessVertexAtTimeData& processOptions ) {}

	/**
	 * @brief The node can declare to be an identity operation.
	 * In this case, the node is not processed and the rendering engine direcly use the indicated input clip at a particular time.
	 *
	 * @param[in] processOptions
	 * @param[out] clip the input clip to use as identity
	 * @param[out] time the time to use as identity
	 * @return if the node is an identity operation
	 */
	virtual bool isIdentity( const graph::ProcessVertexAtTimeData& processOptions, std::string& clip, OfxTime& time ) const = 0;
	
	/**
	 * @brief Fill ProcessInfo to compute statistics for the current process,
	 *        like memory used by this node, by all input nodes, etc.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	virtual void preProcess_infos( const OfxTime time, graph::ProcessVertexAtTimeInfo& nodeInfos ) const = 0;

	/**
	 * @brief Process this node. All inputs are compute.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	virtual void process( graph::ProcessVertexAtTimeData& processOptions ) = 0;

	/**
	 * @brief The process of all nodes is done for one frame, now finalize this node.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	virtual void postProcess( graph::ProcessVertexAtTimeData& processOptions ) = 0;

	/**
	 * @brief End of the whole frame range process, now finalize this node.
	 * @param[in] processOptions
	 * @remark called on each node without predefined order.
	 */
	virtual void endSequence( graph::ProcessVertexData& processOptions ) = 0;

	virtual std::ostream& print( std::ostream& os ) const = 0;

	friend std::ostream& operator<<( std::ostream& os, const This& v );
	
protected:
	typedef graph::ProcessVertexData Data;
	typedef graph::ProcessVertexAtTimeData DataAtTime;
	typedef std::map<OfxTime,DataAtTime*> DataAtTimeMap;

	Data* _data;
	DataAtTimeMap _dataAtTime;

public:
	void setProcessData( Data* data );
	void setProcessData( DataAtTime* dataAtTime );
	
	Data& getData();
	const Data& getData() const;
	const DataAtTime& getData( const OfxTime time ) const;
	DataAtTime& getData( const OfxTime time );

	#endif
};


inline std::string mapNodeTypeEnumToString( const INode::ENodeType e )
{
	switch( e )
	{
		case INode::eNodeTypeUnknown:
			return "NodeTypeUnknown";
		case INode::eNodeTypeImageEffect:
			return "NodeTypeImageEffect";
		case INode::eNodeTypeParam:
			return "NodeTypeParam";
		case INode::eNodeTypeGraph:
			return "NodeTypeGraph";
		case INode::eNodeTypeBuffer:
			return "NodeTypeBuffer";
	}
	BOOST_THROW_EXCEPTION( exception::Bug()
		<< exception::dev() + "Unrecognized ENodeType enum. (" + e + ")." );
}


}
}

#endif

