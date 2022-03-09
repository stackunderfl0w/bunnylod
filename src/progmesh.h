#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/vec3.hpp>
#include <vector>
#include <algorithm>
class Vertex;
class Triangle;

struct Vertex {
	glm::vec3          position; // location of point in euclidean space
	int              id;       // place of vertex in original Array
	std::vector<Vertex *>   neighbors; // adjacent vertices
	std::vector<Triangle *> parent_tri;     // adjacent triangles
	float            collapse_cost;  // cached cost of collapsing edge
	Vertex *         collapse_target; // candidate vertex for collapse
	Vertex(glm::vec3 v,int _id);
	~Vertex();
	void             RemoveIfNonNeighbor(Vertex *n);
};
struct Triangle {
	Vertex *         vertex[3]; // the 3 points that make this tri
	glm::vec3          normal;    // unit vector othogonal to this parent_tri
	Triangle(Vertex *v0,Vertex *v1,Vertex *v2);
	~Triangle();
	void             ComputeNormal();
	void             ReplaceVertex(Vertex *vold,Vertex *vnew);
	int              HasVertex(Vertex *v);
};

class tridata {
public:
	int	v[3];  // indices to vertex Array
	// texture and vertex normal info removed for this demo
};;
float ComputeEdgeCollapseCost(Vertex *u,Vertex *v);
void ComputeEdgeCostAtVertex(Vertex *v);
void Init_mesh(std::vector<glm::vec3> &vert,std::vector<tridata> &tri,std::vector<int> &map, std::vector<int> &permutation,std::vector<Vertex*> *&v2,std::vector<Triangle*> *&t2);

void Collapse(Vertex *u,Vertex *v);
