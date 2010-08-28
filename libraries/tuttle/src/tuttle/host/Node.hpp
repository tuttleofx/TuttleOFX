#ifndef _TUTTLE_HOST_NODE_HPP_
#define _TUTTLE_HOST_NODE_HPP_

#include "graph/ProcessOptions.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace tuttle {
namespace host {

namespace attribute {
class Attribute;
}

class Node
{
public:
	enum ENodeType
	{
		eNodeTypeImageEffect,
		eNodeTypeParam,
		eNodeTypeGraph,
	};

public:
	Node() {}
	Node( const Node& e ) {}
	virtual ~Node() = 0;
	virtual Node*            clone() const              = 0;
	virtual const std::string&      getName() const            = 0;
	virtual const ENodeType  getNodeType() const = 0;

	virtual attribute::Attribute& getAttribute( const std::string& name ) = 0;
//	const attribute::Attribute& getAttribute( const std::string& name ) const { return const_cast<ProcessNode*>(this)->getAttribute( name ); }
	virtual attribute::Attribute& getSingleInputAttribute() = 0;
	virtual const attribute::Attribute& getSingleInputAttribute() const = 0;

#ifndef SWIG
	virtual void connect( const Node&, attribute::Attribute& ) = 0;

	/**
	 * @brief Begin of the a new frame range to process. Initilize this node.
	 * @remark depth first search
	 */
	virtual void begin( graph::ProcessOptions& processOptions )                 = 0;
	/**
	 * @group preProcess1
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @remark depth first search
	 * @{
	 */
	virtual void preProcess1_initialize( graph::ProcessOptions& processOptions ) {}
	virtual void preProcess1_finish( graph::ProcessOptions& processOptions ) {}
	/**
	 * @}
	 * @group preProcess2
	 * @brief Initialization pass to propagate informations from outputs to inputs.
	 * @remark breadth first search
	 * @{
	 */
	virtual void preProcess2_initialize( graph::ProcessOptions& processOptions ) {}
	virtual void preProcess2_finish( graph::ProcessOptions& processOptions ) {}
	/**
	 * @}
	 * @group preProcess3
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @remark depth first search
	 * @{
	 */
	virtual void preProcess3_initialize( graph::ProcessOptions& processOptions ) {}
	virtual void preProcess3_finish( graph::ProcessOptions& processOptions ) {}
	/**
	 * @}
	 * @brief Fill ProcessInfo to compute statistics for the current process,
	 *        like memory used by this node, by all input nodes, etc.
	 * @remark depth first search
	 */
	virtual void preProcess_infos( graph::ProcessInfos& nodeInfos ) const       = 0;
	/**
	 * @brief Process this node. All inputs are compute.
	 * @remark depth first search
	 */
	virtual void process( graph::ProcessOptions& processOptions )               = 0;
	/**
	 * @brief The process of all nodes is done for one frame, now finalize this node.
	 * @remark depth first search
	 */
	virtual void postProcess( graph::ProcessOptions& processOptions )           = 0;
	/**
	 * @brief End of the whole frame range process, now finalize this node.
	 * @remark depth first search
	 */
	virtual void end( graph::ProcessOptions& processOptions )                   = 0;
#endif
};

}
}

#endif

