#ifndef _TUTTLE_EDGE_HPP_
#define _TUTTLE_EDGE_HPP_

#include <string>

class Edge
{
	public:
		Edge(){}

		Edge( const std::string& name )
			: _name(name){}

		Edge(const Edge& e)
			: _name(e.name()){}

		virtual ~Edge()
		{}

		const std::string& name() const	{ return _name; }
		void setName(const std::string s) { _name = s; }

		friend std::ostream& operator<<(std::ostream& os, const Edge& v);

	private:
		std::string _name;
};


#endif

