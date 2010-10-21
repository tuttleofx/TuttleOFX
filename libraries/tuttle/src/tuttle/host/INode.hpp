#ifndef _TUTTLE_HOST_INODE_HPP_
#define _TUTTLE_HOST_INODE_HPP_

#include <ofxCore.h>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

namespace tuttle {
namespace host {

namespace attribute {
class Attribute;
}
namespace graph {
class VertexProcessData;
class VertexAtTimeProcessData;
class VertexAtTimeProcessInfo;
}

class INode
{
public:
	enum ENodeType
	{
		eNodeTypeUnknown,
		eNodeTypeImageEffect,
		eNodeTypeParam,
		eNodeTypeGraph,
	};
	
	typedef std::set<OfxTime> TimesSet;
	typedef std::map<std::string, TimesSet> InputsTimeMap;


public:
	INode() {}
	INode( const INode& e ) {}
	virtual ~INode()                                = 0;
	virtual INode*              clone() const       = 0;
	virtual const std::string& getName() const     = 0;
	virtual const ENodeType    getNodeType() const = 0;

	virtual attribute::Attribute& getAttribute( const std::string& name ) = 0;
	//	const attribute::Attribute& getAttribute( const std::string& name ) const { return const_cast<ProcessNode*>(this)->getAttribute( name ); }
	virtual attribute::Attribute&       getSingleInputAttribute()       = 0;
	virtual const attribute::Attribute& getSingleInputAttribute() const = 0;

	#ifndef SWIG
	virtual void connect( const INode&, attribute::Attribute& ) = 0;

	virtual bool getTimeDomain( OfxRangeD& range ) const = 0;

	/**
	 * @brief Begin of the a new frame range to process. Initilize this node.
	 * @param[in] processOptions
	 * @remark called on each node without predefined order.
	 */
	virtual void beginSequence( graph::VertexProcessData& processOptions ) = 0;

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
	virtual void preProcess1( graph::VertexAtTimeProcessData& processOptions ) {}
	
	/**
	 * @brief Initialization pass to propagate informations from outputs to inputs.
	 * @param[in] processOptions
	 * @remark Called on each node in a REVERSE depth first search order. So you have the guarantee that it has been called on each output nodes before. Output nodes are those who used the result of the current node.
	 */
	virtual void preProcess2_reverse( graph::VertexAtTimeProcessData& processOptions ) {}

	/**
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	virtual void preProcess3( graph::VertexAtTimeProcessData& processOptions ) {}

	/**
	 * @brief The node can declare to be an identity operation.
	 * In this case, the node is not processed and the rendering engine direcly use the indicated input clip at a particular time.
	 *
	 * @param[in] processOptions
	 * @param[out] clip the input clip to use as identity
	 * @param[out] time the time to use as identity
	 * @return if the node is an identity operation
	 */
	virtual bool isIdentity( const graph::VertexAtTimeProcessData& processOptions, std::string& clip, OfxTime& time ) const = 0;
	
	/**
	 * @brief Fill ProcessInfo to compute statistics for the current process,
	 *        like memory used by this node, by all input nodes, etc.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	virtual void preProcess_infos( const OfxTime time, graph::VertexAtTimeProcessInfo& nodeInfos ) const = 0;

	/**
	 * @brief Process this node. All inputs are compute.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	virtual void process( graph::VertexAtTimeProcessData& processOptions ) = 0;

	/**
	 * @brief The process of all nodes is done for one frame, now finalize this node.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	virtual void postProcess( graph::VertexAtTimeProcessData& processOptions ) = 0;

	/**
	 * @brief End of the whole frame range process, now finalize this node.
	 * @param[in] processOptions
	 * @remark called on each node without predefined order.
	 */
	virtual void endSequence( graph::VertexProcessData& processOptions ) = 0;
	#endif
};

}
}

#endif

