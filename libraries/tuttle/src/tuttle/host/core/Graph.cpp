#include "Graph.hpp"

#include <iostream>
#include <sstream>

namespace tuttle {
namespace host {
namespace core {

Graph::Graph()
{
}

EffectInstance* Graph::createNode( const std::string& id ) throw(std::logic_error)
{
	ofx::imageEffect::ImageEffectPlugin* plug = Core::instance().getImageEffectPluginById( id );
	if( !plug )
		throw std::logic_error( "Plugin not found. plug" );
	ofx::imageEffect::Instance* plugInst = plug->createInstance( kOfxImageEffectContextFilter, NULL );
	if( !plugInst )
		throw std::logic_error( "Plugin not found. plugInst" );
	EffectInstance* node = dynamic_cast<EffectInstance*>(plugInst);
	if( !node )
		throw std::logic_error( "Plugin not found." );

	std::stringstream s;
	s << node->getName() << ++_instanceCount[node->getName()];
	node->setName( s.str() );

	addToGraph( *node );

	return node;
}

void Graph::addToGraph( EffectInstance& node )
{
//	_graph;
}

void Graph::deleteNode( const EffectInstance& node )
{

}

void Graph::connectNodes( const EffectInstance& out, const ofx::attribute::AttributeInstance& outAttr, const EffectInstance& in, const ofx::attribute::AttributeInstance& inAttr )
{

}


}
}
}

