#ifndef _TUTTLE_VERTEX_HPP_
#define _TUTTLE_VERTEX_HPP_

#include <string>
#include <iostream>

class Vertex
{
	public:
		Vertex(){}

		Vertex( const std::string& name )
			: _name(name){}

		Vertex(const Vertex& v)
			: _name(v.name()){}

		virtual ~Vertex()
		{}

		const std::string& name() const	{ return _name; }
		void setName(const std::string s) { _name = s; }

		friend std::ostream& operator<<(std::ostream& os, const Vertex& v);

	private:
		std::string _name;

};

#endif

