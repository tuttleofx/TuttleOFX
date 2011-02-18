#ifndef _TUTTLE_HOST_IVERTEX_HPP_
#define	_TUTTLE_HOST_IVERTEX_HPP_

#include <tuttle/host/INode.hpp>
#include <tuttle/host/exceptionsHost.hpp>
#include <tuttle/common/utils/global.hpp>

#include <iostream>
#include <set>

namespace tuttle {
namespace host {
namespace graph {

class IVertex
{
public:
	IVertex( const std::string& name = "Undefined" );

	IVertex( const std::string& name, INode& processNode );

	IVertex( const IVertex& v );

	virtual ~IVertex() = 0;

	IVertex& operator=( const IVertex& v )
	{
		_name           = v._name;
		_processNode    = v._processNode;
		_fake           = v._fake;
		_used           = v._used;
		return *this;
	}

	bool                  isFake() const                                     { return _fake; }
	void                  setUsed( const bool used = true )                  { _used = used; }
	bool                  isUsed() const                                     { return _used; }
	const std::string&    getName() const                                    { return _name; }
	INode&                getProcessNode()
	{
		if( !_processNode )
		{
			BOOST_THROW_EXCEPTION( exception::Bug()
				<< exception::dev() + "Process node not set on IVertex \"" + getName() + "\"." );
		}
		return *_processNode;
	}
	const INode&          getProcessNode() const
	{
		if( !_processNode )
		{
			BOOST_THROW_EXCEPTION( exception::Bug()
				<< exception::dev() + "Process node not set on IVertex \"" + getName() + "\"." );
		}
		return *_processNode;
	}
	void                  setProcessNode( INode* p )
	{
		_processNode = p;
	}

	virtual std::ostream& exportDotDebug( std::ostream& os ) const;
	friend std::ostream& operator<<( std::ostream& os, const IVertex& v );

public:
	std::string _name;
	
private:
	INode* _processNode;
	bool _fake;
	bool _used;
	static int _count;

public:
	int _id;

};

}
}
}

#endif

