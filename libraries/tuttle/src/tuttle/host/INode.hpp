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

	typedef std::map<std::string, std::set<OfxTime> > InputsTimeMap;

	virtual InputsTimeMap getTimesNeeded( const OfxTime time ) const = 0;

	/**
	 * @brief Begin of the a new frame range to process. Initilize this node.
	 * @remark depth first search
	 */
	virtual void beginSequence( graph::ProcessOptions& processOptions ) = 0;
	/**
	 * @group preProcess1
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @remark depth first search
	 * @{
	 */
	virtual void preProcess1( graph::ProcessOptions& processOptions )     {}
	/**
	 * @}
	 * @group preProcess2
	 * @brief Initialization pass to propagate informations from outputs to inputs.
	 * @remark breadth first search
	 * @{
	 */
	virtual void preProcess2_reverse( graph::ProcessOptions& processOptions )     {}
	/**
	 * @}
	 * @group preProcess3
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @remark depth first search
	 * @{
	 */
	virtual void preProcess3( graph::ProcessOptions& processOptions )     {}
	/**
	 * @}
	 * @brief Fill ProcessInfo to compute statistics for the current process,
	 *        like memory used by this node, by all input nodes, etc.
	 * @remark depth first search
	 */
	virtual void preProcess_infos( graph::ProcessInfos& nodeInfos ) const = 0;
	/**
	 * @brief Process this node. All inputs are compute.
	 * @remark depth first search
	 */
	virtual void process( graph::ProcessOptions& processOptions ) = 0;
	/**
	 * @brief The process of all nodes is done for one frame, now finalize this node.
	 * @remark depth first search
	 */
	virtual void postProcess( graph::ProcessOptions& processOptions ) = 0;
	/**
	 * @brief End of the whole frame range process, now finalize this node.
	 * @remark depth first search
	 */
	virtual void endSequence( graph::ProcessOptions& processOptions ) = 0;
	#endif
};

}
}

#endif

