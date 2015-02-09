
#include <CGAL/Surface_mesh.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Eigen_solver_traits.h>
#include <CGAL/Mean_curvature_skeleton_functions.h>
#include <CGAL/iterator.h>
#include <CGAL/Bbox_3.h>

#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <fstream>
#include <map>

typedef CGAL::Simple_cartesian<double>                               Kernel;
typedef Kernel::Point_3                                              Point;
typedef Kernel::Vector_3                                             Vector;
typedef CGAL::Surface_mesh<Point> Polyhedron;

typedef boost::graph_traits<Polyhedron>::vertex_descriptor           vertex_descriptor;
typedef boost::graph_traits<Polyhedron>::vertex_iterator             vertex_iterator;
typedef boost::graph_traits<Polyhedron>::halfedge_descriptor         halfedge_descriptor;

typedef boost::property_map<Polyhedron,CGAL::vertex_point_t>::type PPmap;

struct Skeleton_vertex_info
{
  std::size_t id;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Skeleton_vertex_info> Graph;

typedef boost::graph_traits<Graph>::vertex_descriptor                  vertex_desc;
typedef boost::graph_traits<Graph>::vertex_iterator                    vertex_iter;
typedef boost::graph_traits<Graph>::edge_iterator                      edge_iter;

typedef std::map<vertex_desc, std::vector<int> >                       Correspondence_map;
typedef boost::associative_property_map<Correspondence_map>            GraphCorrelationPMap;

typedef std::map<vertex_desc, Point>                                   GraphPointMap;
typedef boost::associative_property_map<GraphPointMap>                 GraphPointPMap;

// The input of the skeletonization algorithm must be a pure triangular closed
// mesh and has only one component.
bool is_mesh_valid(Polyhedron& pMesh)
{
  if (! is_closed(pMesh))
  {
    std::cerr << "The mesh is not closed.";
    return false;
  }
  if (! is_pure_triangle(pMesh))
  {
    std::cerr << "The mesh is not a pure triangle mesh.";
    return false;
  }
  /*
  // the algorithm is only applicable on a mesh
  // that has only one connected component
  std::size_t num_component;
  CGAL::Counting_output_iterator output_it(&num_component);
  CGAL::internal::extract_connected_components(pMesh, output_it);
  ++output_it;
  if (num_component != 1)
  {
    std::cerr << "The mesh is not a single closed mesh. It has " 
              << num_component << " components.";
    return false;
  }
  */
  return true;
}

// This example extracts a medially centered skeleton from a given mesh.
int main()
{
  Polyhedron mesh;
  std::ifstream input("data/sindorelax.off");

  if ( !input || !(input >> mesh) || mesh.is_empty() ) {
    std::cerr << "Cannot open data/sindorelax.off" << std::endl;
    return 1;
  }
  if (!is_mesh_valid(mesh)) {
    return 1;
  }

  PPmap ppmap = get(CGAL::vertex_point, mesh);
  Graph g;
  GraphPointMap points_map;
  GraphPointPMap points(points_map);

  Correspondence_map corr_map;
  GraphCorrelationPMap corr(corr_map);


  CGAL::extract_mean_curvature_flow_skeleton(mesh, g, points, corr);
#if 0
  vertex_iterator vb, ve;

  std::cout << "vertices: " << boost::num_vertices(g) << "\n";
  std::cout << "edges: " << boost::num_edges(g) << "\n";

  // Output all the edges.
  edge_iter ei, ei_end;
  for (boost::tie(ei, ei_end) = boost::edges(g); ei != ei_end; ++ei)
  {
    Point s = points[source(*ei, g)];
    Point t = points[target(*ei, g)];
    std::cout << s << " " << t << "\n";
  }

  std::vector<vertex_descriptor> id_to_vd;
  id_to_vd.clear();
  id_to_vd.resize(num_vertices(mesh));
  std::size_t id=0;
  for (boost::tie(vb, ve) = vertices(mesh); vb != ve; ++vb)
  {
    vertex_descriptor v = *vb;
    id_to_vd[id++] = v;
  }

  // Output skeletal points and the corresponding surface points
  vertex_iter gvb, gve;
  for (boost::tie(gvb, gve) = boost::vertices(g); gvb != gve; ++gvb)
  {
    vertex_desc i = *gvb;
    Point skel = points[i];
    std::cout << skel << ": ";

    for (size_t j = 0; j < corr[i].size(); ++j)
    {
      Point surf = ppmap[id_to_vd[corr[i][j]]];
      std::cout << surf << " ";
    }
    std::cout << "\n";
  }
#endif
  return 0;
}

