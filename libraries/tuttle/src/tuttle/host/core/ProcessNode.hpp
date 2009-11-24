#ifndef _TUTTLE_PROCESSNODE_HPP_
#define _TUTTLE_PROCESSNODE_HPP_

#include <tuttle/host/ofx/Attribute.hpp>

#include <iostream>
#include <string>
#include <vector>


namespace tuttle{
	namespace host{
		namespace core{

class ProcessNode
{
	public:
		ProcessNode(){}

		ProcessNode( const std::string& name )
			: _name(name){}

		ProcessNode(const ProcessNode& e)
			: _name(e.name()){}

		virtual ~ProcessNode(){}

		//TODO: replace by virtual ProcessNode* clone() const =0;
		virtual ProcessNode* clone() const {
			return (new ProcessNode(*this));
		}

		const std::string& name() const	{ return _name; }
		void setName(const std::string s) { _name = s; }

		friend std::ostream& operator<<(std::ostream& os, const ProcessNode& v);

	private:
		std::string _name;
		std::vector<Attribute*> _attributes;
};

} // namespace core
} // namespace host
} // namespace tuttle

#endif

