#ifndef _TUTTLE_VERTEX_HPP_
#define _TUTTLE_VERTEX_HPP_

#include <string>
#include <iostream>

#include <tuttle/host/core/ProcessNode.hpp>

namespace tuttle{
	namespace host{
		namespace graph{

class Vertex
{
	public:
		Vertex(){}

		Vertex(	const std::string& name,
				core::ProcessNode * processNode,
				const std::string& attributeName)
			: _name(name)
			, _attributeName(attributeName)
			, _processNode(processNode){
		}

		Vertex(Vertex& v)
			: _name(v.name())
			, _attributeName(v.attributeName())
			, _processNode(v.processNode()){
		}

		virtual ~Vertex()
		{}

		// operators
		Vertex& operator=(const Vertex &v)
		{
			if (this == &v) return *this;
			_name = v.name();
			_attributeName = v.attributeName();
			_processNode = v.processNode();
			return *this;
		}

		const std::string& name() const	{ return _name; }
		const std::string& attributeName() const { return _attributeName; }
		core::ProcessNode* processNode() const { return _processNode; }
		void setProcessNode(core::ProcessNode * p) { _processNode=p; }

		friend std::ostream& operator<<(std::ostream& os, const Vertex& v);

	private:
		std::string _name;
		std::string _attributeName;
		core::ProcessNode* _processNode;
};


} // namespace graph
} // namespace host
} // namespace tuttle

#endif

