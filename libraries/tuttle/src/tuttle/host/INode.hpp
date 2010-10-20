#ifndef _TUTTLE_HOST_INODE_HPP_
#define _TUTTLE_HOST_INODE_HPP_

#include "graph/ProcessOptions.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <set>

namespace tuttle {
namespace host {

namespace attribute {
class Attribute;
}

class INode
{
public:
	enum ENodeType
	{
		eNodeTypeImageEffect,
		eNodeTypeParam,
		eNodeTypeGraph,
	};

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

	typedef std::set<OfxTime> TimesSet;
	typedef std::map<std::string, TimesSet> InputsTimeMap;

	virtual InputsTimeMap getTimesNeeded( const OfxTime time ) const = 0;

	/**
	 * @brief The node can declare to be an identity operation.
	 * In this case, the node is not processed and the rendering engine direcly use the indicated input clip at a particular time.
	 *
	 * @param[in] processOptions
	 * @param[out] clip the input clip to use as identity
	 * @param[out] time the time to use as identity
	 * @return if the node is an identity operation
	 */
	virtual bool isIdentity( const graph::ProcessOptions& processOptions, std::string& clip, OfxTime& time ) const = 0;

	/**
	 * @brief Begin of the a new frame range to process. Initilize this node.
	 * @param[in] processOptions
	 * @remark depth first search
	 */
	virtual void beginSequence( graph::ProcessOptions& processOptions ) = 0;
	/**
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @param[in] processOptions
	 * @remark depth first search
	 */
	virtual void preProcess1( graph::ProcessOptions& processOptions ) {}
	/**
	 * @brief Initialization pass to propagate informations from outputs to inputs.
	 * @param[in] processOptions
	 * @remark breadth first search
	 */
	virtual void preProcess2_reverse( graph::ProcessOptions& processOptions ) {}
	/**
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @param[in] processOptions
	 * @remark depth first search
	 */
	virtual void preProcess3( graph::ProcessOptions& processOptions ) {}
	/**
	 * @brief Fill ProcessInfo to compute statistics for the current process,
	 *        like memory used by this node, by all input nodes, etc.
	 * @param[in] processOptions
	 * @remark depth first search
	 */
	virtual void preProcess_infos( const OfxTime time, graph::ProcessInfos& nodeInfos ) const = 0;
	/**
	 * @brief Process this node. All inputs are compute.
	 * @param[in] processOptions
	 * @remark depth first search
	 */
	virtual void process( graph::ProcessOptions& processOptions ) = 0;
	/**
	 * @brief The process of all nodes is done for one frame, now finalize this node.
	 * @param[in] processOptions
	 * @remark depth first search
	 */
	virtual void postProcess( graph::ProcessOptions& processOptions ) = 0;
	/**
	 * @brief End of the whole frame range process, now finalize this node.
	 * @param[in] processOptions
	 * @remark depth first search
	 */
	virtual void endSequence( graph::ProcessOptions& processOptions ) = 0;
	#endif
};

}
}

#endif

