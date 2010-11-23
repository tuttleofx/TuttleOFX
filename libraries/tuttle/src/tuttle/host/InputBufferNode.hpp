#ifndef _TUTTLE_HOST_INPUTBUFFERNODE_HPP_
#define	_TUTTLE_HOST_INPUTBUFFERNODE_HPP_

#include "INode.hpp"
#include <tuttle/host/attribute/Param.hpp>
#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/graph/ProcessVertexData.hpp>
#include <tuttle/host/graph/ProcessVertexAtTimeData.hpp>

namespace tuttle {
namespace host {

class InputBufferNode : public INode
{
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
	
	const std::string& getLabel() const     { return _label; }
	const std::string& getName() const     { return _name; }
	void setName( const std::string& name ) { _name = name; }
	const ENodeType    getNodeType() const { return eNodeTypeBuffer; }

	attribute::Attribute& getAttribute( const std::string& name );
	attribute::Attribute&       getSingleInputAttribute();
	const attribute::Attribute& getSingleInputAttribute() const;
	const ofx::attribute::OfxhParam& getParam( const std::string& name ) const;
	ofx::attribute::OfxhParam&       getParam( const std::string& name );

	ofx::attribute::OfxhClipImage&       getClip( const std::string& name );
	const ofx::attribute::OfxhClipImage& getClip( const std::string& name ) const;

	ofx::attribute::OfxhParamSet& getParamSet();
	const ofx::attribute::OfxhParamSet& getParamSet() const;

	#ifndef SWIG
	void connect( const INode&, attribute::Attribute& ) {}

	OfxRectD getRegionOfDefinition( const OfxTime time ) const
	{
		return getData(time)._apiImageEffect._renderRoD;
	}

	bool getTimeDomain( OfxRangeD& range ) const { return false; }

	/**
	 * @brief Begin of the a new frame range to process. Initilize this node.
	 * @param[in] processOptions
	 * @remark called on each node without predefined order.
	 */
	void beginSequence( graph::ProcessVertexData& processOptions ) {}

	/**
	 * @brief Asks the plugin all times it needs for each of it's input clips.
	 * @param[in] time
	 */
	InputsTimeMap getTimesNeeded( const OfxTime time ) const { return InputsTimeMap(); }

	/**
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	void preProcess1( graph::ProcessVertexAtTimeData& processOptions ) {}

	/**
	 * @brief Initialization pass to propagate informations from outputs to inputs.
	 * @param[in] processOptions
	 * @remark Called on each node in a REVERSE depth first search order. So you have the guarantee that it has been called on each output nodes before. Output nodes are those who used the result of the current node.
	 */
	void preProcess2_reverse( graph::ProcessVertexAtTimeData& processOptions ) {}

	/**
	 * @brief Initialization pass to propagate informations from inputs to outputs.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	void preProcess3( graph::ProcessVertexAtTimeData& processOptions ) {}

	/**
	 * @brief The node can declare to be an identity operation.
	 * In this case, the node is not processed and the rendering engine direcly use the indicated input clip at a particular time.
	 *
	 * @param[in] processOptions
	 * @param[out] clip the input clip to use as identity
	 * @param[out] time the time to use as identity
	 * @return if the node is an identity operation
	 */
	bool isIdentity( const graph::ProcessVertexAtTimeData& processOptions, std::string& clip, OfxTime& time ) const { return false; }

	/**
	 * @brief Fill ProcessInfo to compute statistics for the current process,
	 *        like memory used by this node, by all input nodes, etc.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	void preProcess_infos( const OfxTime time, graph::ProcessVertexAtTimeInfo& nodeInfos ) const {}

	/**
	 * @brief Process this node. All inputs are compute.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	void process( graph::ProcessVertexAtTimeData& processOptions ) {}

	/**
	 * @brief The process of all nodes is done for one frame, now finalize this node.
	 * @param[in] processOptions
	 * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each input nodes before.
	 */
	void postProcess( graph::ProcessVertexAtTimeData& processOptions ) {}

	/**
	 * @brief End of the whole frame range process, now finalize this node.
	 * @param[in] processOptions
	 * @remark called on each node without predefined order.
	 */
	void endSequence( graph::ProcessVertexData& processOptions ) {}

	std::ostream& print( std::ostream& os ) const;

	friend std::ostream& operator<<( std::ostream& os, const This& v );
	#endif

private:

};

}
}

#endif

