#define BOOST_TEST_MODULE internalGraph_tests
#include <tuttle/test/main.hpp>

#include <tuttle/host/graph/InternalGraph.hpp>

#include <iostream>
#include <string>

namespace tuttle
{
namespace test
{

class DummyVertex
{
public:
    typedef std::string Key;

public:
    DummyVertex() {}

    DummyVertex(const std::string& name)
        : _name(name)
    {
    }

    DummyVertex(const DummyVertex& v)
        : _name(v.getName())
    {
    }

    virtual ~DummyVertex() {}

    Key getKey() const { return _name; }
    const std::string& getName() const { return _name; }

    // operators
    DummyVertex& operator=(const DummyVertex& v)
    {
        if(this == &v)
            return *this;
        _name = v.getName();
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const DummyVertex& v)
    {
        os << v.getName();
        return os;
    }

private:
    std::string _name;
};

class DummyEdge
{
public:
    DummyEdge() {}

    DummyEdge(const std::string& name)
        : _name(name)
    {
    }

    DummyEdge(const DummyEdge& e)
        : _name(e.getName())
    {
    }

    virtual ~DummyEdge() {}

    const std::string& getName() const { return _name; }
    const std::string& getInAttrName() const { return _inAttrName; }
    void setName(const std::string s) { _name = s; }

    // operators
    DummyEdge& operator=(const DummyEdge& e)
    {
        if(this == &e)
            return *this;
        _name = e.getName();
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const DummyEdge& v)
    {
        os << v.getName();
        return os;
    }

private:
    std::string _name;
    std::string _inAttrName;
};

class DummyAttribute
{
public:
    DummyAttribute() {}

    DummyAttribute(const std::string& name)
        : _name(name)
    {
    }

    DummyAttribute(const DummyAttribute& e)
        : _name(e.name())
    {
    }

    virtual ~DummyAttribute() {}

    const std::string& name() const { return _name; }
    void setName(const std::string s) { _name = s; }

    friend std::ostream& operator<<(std::ostream& os, const DummyAttribute& v)
    {
        os << v.name();
        return os;
    }

private:
    std::string _name;
};
}
}

BOOST_AUTO_TEST_SUITE(tuttle_internalGraph_suite)

using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE(internalGraph_add_one_vertex)
{
    using namespace tuttle::test;
    using namespace tuttle::host;

    typedef graph::InternalGraph<DummyVertex, DummyEdge> DummyGraph;
    typedef DummyGraph::vertex_descriptor DummyVertexDescriptor;

    DummyGraph graph;

    DummyVertex A("nodeA");
    DummyVertexDescriptor ADesc = graph.addVertex(A);

    BOOST_CHECK_EQUAL(graph.getVertexCount(), 1);
    BOOST_CHECK_EQUAL(graph.instance(ADesc).getName(), "nodeA");
}

BOOST_AUTO_TEST_CASE(connect_two_nodes)
{
    using namespace tuttle::test;
    using namespace tuttle::host;

    typedef graph::InternalGraph<DummyVertex, DummyEdge> DummyGraph;
    typedef DummyGraph::vertex_descriptor DummyVertexDescriptor;
    typedef DummyGraph::edge_descriptor DummyEdgeDescriptor;
    DummyGraph graph;

    DummyVertex A("nodeA");
    DummyVertex B("nodeB");
    DummyVertexDescriptor aDesc = graph.addVertex(A);
    DummyVertexDescriptor bDesc = graph.addVertex(B);

    DummyEdge a_to_b("edgeAtoB");
    DummyEdgeDescriptor AtoBDesc = graph.addEdge(aDesc, bDesc, a_to_b);

    BOOST_CHECK_EQUAL(graph.instance(aDesc).getName(), "nodeA");
    BOOST_CHECK_EQUAL(graph.getVertexCount(), 2);
    BOOST_CHECK_EQUAL(graph.getEdgeCount(), 1);
    BOOST_CHECK_EQUAL(graph.instance(bDesc).getName(), "nodeB");
    BOOST_CHECK_EQUAL(graph.instance(AtoBDesc).getName(), "edgeAtoB");
}

BOOST_AUTO_TEST_CASE(delete_one_node)
{
    using namespace tuttle::test;
    using namespace tuttle::host;

    typedef graph::InternalGraph<DummyVertex, DummyEdge> DummyGraph;
    typedef DummyGraph::vertex_descriptor DummyVertexDescriptor;
    typedef DummyGraph::edge_descriptor DummyEdgeDescriptor;
    DummyGraph graph;

    DummyVertex a("nodeA");
    DummyVertex b("nodeB");
    DummyVertex c("nodeC");
    DummyVertex d("nodeD");

    DummyEdge a_to_b("A to B");
    DummyEdge b_to_c("B to C");
    DummyEdge b_to_d("B to D");
    DummyEdge c_to_d("C to D");

    DummyVertexDescriptor aDesc = graph.addVertex(a);
    DummyVertexDescriptor bDesc = graph.addVertex(b);
    DummyVertexDescriptor cDesc = graph.addVertex(c);
    DummyVertexDescriptor dDesc = graph.addVertex(d);

    graph.addEdge(aDesc, bDesc, a_to_b); // A -> B
    graph.addEdge(bDesc, cDesc, b_to_c); // B -> C
    graph.addEdge(cDesc, dDesc, c_to_d); // C -> D

    BOOST_CHECK_EQUAL(graph.getVertexCount(), 4);
    BOOST_CHECK_EQUAL(graph.getEdgeCount(), 3);

    graph.removeVertex(cDesc);

    BOOST_CHECK_EQUAL(graph.getVertexCount(), 3);
    BOOST_CHECK_EQUAL(graph.getEdgeCount(), 1);

    graph.addEdge(graph.getVertexDescriptor(b.getKey()), graph.getVertexDescriptor(d.getKey()), b_to_d); // B -> D

    BOOST_CHECK_EQUAL(graph.getVertexCount(), 3);
    BOOST_CHECK_EQUAL(graph.getEdgeCount(), 2);
}

BOOST_AUTO_TEST_CASE(detect_cycle)
{
    using namespace tuttle::test;
    using namespace tuttle::host;

    typedef graph::InternalGraph<DummyVertex, DummyEdge> DummyGraph;
    typedef DummyGraph::vertex_descriptor DummyVertexDescriptor;
    typedef DummyGraph::edge_descriptor DummyEdgeDescriptor;
    DummyGraph graph;

    DummyVertex A("nodeA");
    DummyVertex B("nodeB");
    DummyVertex C("nodeC");

    DummyEdge AtoB("AtoB");
    DummyEdge BtoC("BtoC");
    DummyEdge CtoA("CtoA");

    DummyVertexDescriptor ADesc = graph.addVertex(A);
    DummyVertexDescriptor BDesc = graph.addVertex(B);
    DummyVertexDescriptor CDesc = graph.addVertex(C);

    // A -> B : ok
    BOOST_CHECK_NO_THROW(graph.addEdge(ADesc, BDesc, AtoB));
    // B -> C : ok
    BOOST_CHECK_NO_THROW(graph.addEdge(BDesc, CDesc, BtoC));
    // C -> A : cycle -> exception
    BOOST_CHECK_THROW(graph.addEdge(CDesc, ADesc, CtoA), exception::Logic);
}

BOOST_AUTO_TEST_SUITE_END()
