#include <iostream>
#include <cassert>
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>

int main()
{
	typedef boost::vecS OutEdgeListS;
	typedef boost::vecS VertexListS;
	typedef boost::directedS DirectedS;

	typedef boost::adjacency_list_traits<
		OutEdgeListS,
		VertexListS,
		DirectedS
	> Traits;

	typedef
		boost::property<boost::vertex_index_t, int,
		boost::property<boost::vertex_color_t, boost::default_color_type,
		boost::property<boost::vertex_distance_t, double,
		boost::property<boost::vertex_predecessor_t, Traits::edge_descriptor
		>>>> VertexProperty;

	typedef
		boost::property<boost::edge_capacity_t, double,
		boost::property<boost::edge_residual_capacity_t, double,
		boost::property<boost::edge_reverse_t, Traits::edge_descriptor
		>>> EdgeProperty;

	typedef boost::adjacency_list<
		OutEdgeListS,
		VertexListS,
		DirectedS,
		VertexProperty,
		EdgeProperty
	> Graph;

	typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
	typedef boost::graph_traits<Graph>::edge_descriptor EdgeDescriptor;

	Graph g;

	//                              *
	// source -> (0)--<5.0>--(1)--<1.0>--(2)
	//            |           |     *     |
	//          <5.0>       <6.0>   *   <7.0>        ( ): vertex
	//            |           |     *     |          < >: capacity
	//           (3)--<4.0>--(4)--<1.0>--(5)         -,|: edge
	//            |           |     *     |           * : min-cut
	//          <5.0>   ****<1.0>****   <6.0>
	//            |     *     |           |
	//           (6)--<1.0>--(7)--<5.0>--(8) <- sink
	//                  *

#if 0
	VertexDescriptor v0 = add_vertex(g);
	VertexDescriptor v1 = add_vertex(g);
	VertexDescriptor v2 = add_vertex(g);
	VertexDescriptor v3 = add_vertex(g);
	VertexDescriptor v4 = add_vertex(g);
	VertexDescriptor v5 = add_vertex(g);
	VertexDescriptor v6 = add_vertex(g);
	VertexDescriptor v7 = add_vertex(g);
	VertexDescriptor v8 = add_vertex(g);

	typedef boost::property_value<EdgeProperty, boost::edge_capacity_t>::type EdgeCapacity;
	auto add_bidirectional_edge = [&](VertexDescriptor s, VertexDescriptor t, EdgeCapacity cap) {
		EdgeDescriptor fwd = add_edge(s, t, g).first;
		put(boost::edge_capacity, g, fwd, cap);

		EdgeDescriptor rev = add_edge(t, s, g).first;
		put(boost::edge_capacity, g, rev, cap);

		put(boost::edge_reverse, g, fwd, rev);
		put(boost::edge_reverse, g, rev, fwd);
	};
	add_bidirectional_edge(v0, v1, 5.0);
	add_bidirectional_edge(v1, v2, 1.0);
	add_bidirectional_edge(v3, v4, 4.0);
	add_bidirectional_edge(v4, v5, 1.0);
	add_bidirectional_edge(v6, v7, 1.0);
	add_bidirectional_edge(v7, v8, 5.0);
	add_bidirectional_edge(v0, v3, 5.0);
	add_bidirectional_edge(v3, v6, 5.0);
	add_bidirectional_edge(v1, v4, 6.0);
	add_bidirectional_edge(v4, v7, 1.0);
	add_bidirectional_edge(v2, v5, 7.0);
	add_bidirectional_edge(v5, v8, 6.0);

	VertexDescriptor src = v0;
	VertexDescriptor snk = v8;
#else
	for (int i = 0; i < 9; ++i) {
		add_vertex(g);
	}

	typedef boost::property_value<VertexProperty, boost::vertex_index_t>::type VertexIndex;
	typedef boost::property_value<EdgeProperty, boost::edge_capacity_t>::type EdgeCapacity;

	auto add_bidirectional_edge = [&](VertexIndex s_idx, VertexIndex t_idx, EdgeCapacity cap) {
		VertexDescriptor s = vertex(s_idx, g);
		VertexDescriptor t = vertex(t_idx, g);

		EdgeDescriptor fwd = add_edge(s, t, g).first;
		put(boost::edge_capacity, g, fwd, cap);

		EdgeDescriptor rev = add_edge(t, s, g).first;
		put(boost::edge_capacity, g, rev, cap);

		put(boost::edge_reverse, g, fwd, rev);
		put(boost::edge_reverse, g, rev, fwd);
	};
	add_bidirectional_edge(0, 1, 5.0);
	add_bidirectional_edge(1, 2, 1.0);
	add_bidirectional_edge(3, 4, 4.0);
	add_bidirectional_edge(4, 5, 1.0);
	add_bidirectional_edge(6, 7, 1.0);
	add_bidirectional_edge(7, 8, 5.0);
	add_bidirectional_edge(0, 3, 5.0);
	add_bidirectional_edge(3, 6, 5.0);
	add_bidirectional_edge(1, 4, 6.0);
	add_bidirectional_edge(4, 7, 1.0);
	add_bidirectional_edge(2, 5, 7.0);
	add_bidirectional_edge(5, 8, 6.0);

	VertexDescriptor src = vertex(0, g);
	VertexDescriptor snk = vertex(8, g);
#endif

	auto const flow = boost::boykov_kolmogorov_max_flow(g, src, snk);

	std::cout << "flow: " << flow << std::endl;

	BOOST_FOREACH(VertexDescriptor v, vertices(g)) {
		auto const index = get(boost::vertex_index, g, v);
		auto const color = get(boost::vertex_color, g, v);

		typedef boost::property_value<VertexProperty, boost::vertex_color_t>::type ColorType;
		typedef boost::color_traits<ColorType> ColorTraits;
		static auto const black = ColorTraits::black();
		std::cout
			<< "v" << index << ": "
			<< (color == black ? "source" : "sink")
			<< std::endl;
	}
}
