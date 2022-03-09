#include "progmesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/vec3.hpp>
#include <vector>
#include <iostream>
template <class T>
bool Contains(std::vector<T> &haystack, T needle) {
	return ((std::find(haystack.begin(), haystack.end(), needle) != haystack.end()));
}
template <class T>
void Remove(std::vector<T> &haystack, T needle){
	//haystack.erase(std::remove(haystack.begin(), haystack.end(), needle), haystack.end());
	//haystack.erase(std::find(haystack.begin(), haystack.end(), needle), haystack.end());
	///todo find difference
	auto it = std::find(begin(haystack), end(haystack), needle);
	assert(it != end(haystack));
	haystack.erase(it);
	assert(!Contains(haystack, needle));

}
template <class T>
void AddUnique(std::vector<T> & haystack, T needle){
	if(!Contains(haystack,needle)){
		haystack.push_back(needle);
	}
}
//template<class T> int   Contains(const std::vector<T> & c, const T & t){ return (int)std::count(begin(c), end(c), t); }
//template<class T> int   IndexOf(const std::vector<T> & c, const T & v) { return (int)( std::find(begin(c), end(c), v) - begin(c) ); } // Note: Not presently called
//template<class T> T &   Add(std::vector<T> & c, T t)                   { c.push_back(t); return c.back(); }
//template<class T> T     Pop(std::vector<T> & c)                        { auto val = std::move(c.back()); c.pop_back(); return val; }
//template<class T> void  AddUnique(std::vector<T> & c, T t)             { if (!Contains(c, t)) c.push_back(t); }
/*template<class T> void  Remove(std::vector<T> & c, T t){
	auto it = std::find(begin(c), end(c), t);
	assert(it != end(c));
	c.erase(it);
	assert(!Contains(c, t));
}*/

class Vertex;
class Triangle;


Triangle::Triangle(Vertex *v0, Vertex *v1, Vertex *v2) {
	assert(v0!=v1 && v1!=v2 && v2!=v0);
	vertex[0]=v0;
	vertex[1]=v1;
	vertex[2]=v2;
	ComputeNormal();
	for(int i=0;i<3;i++) {
		vertex[i]->parent_tri.push_back(this);
		for(int j=0;j<3;j++) if(i!=j) {
			AddUnique(vertex[i]->neighbors, vertex[j]);
			//if(!Contains(vertex[i]->neighbors,vertex[j])){
			//	vertex[i]->neighbors.push_back(vertex[j]);
			//}
		}
	}
}

void Triangle::ComputeNormal() {
	glm::vec3 v0=vertex[0]->position;
	glm::vec3 v1=vertex[1]->position;
	glm::vec3 v2=vertex[2]->position;
	normal = glm::cross(v1-v0,v2-v1);
	if(glm::length(normal)==0)return;
	normal = normalize(normal);
}

Vertex::Vertex(glm::vec3 v, int _id) {
	position =v;
	id=_id;
}
std::vector<Vertex*> vertices;
std::vector<Triangle *> triangles;

Vertex::~Vertex() {
	assert(parent_tri.size() == 0);
	while(neighbors.size()) {
		Remove(neighbors[0]->neighbors,this);
		Remove(neighbors,neighbors[0]);
	}
	Remove(vertices,this);
}
void Vertex::RemoveIfNonNeighbor(Vertex *n) {
	// removes n from neighbor Array if n isn't a neighbor.
	if(!Contains(neighbors,n)) return;
	for (unsigned int i = 0; i<parent_tri.size(); i++) {
		if(parent_tri[i]->HasVertex(n)) return;
	}
	Remove(neighbors,n);
}

int Triangle::HasVertex(Vertex *v) {
	return (v==vertex[0] ||v==vertex[1] || v==vertex[2]);
}

Triangle::~Triangle() {
	Remove(triangles, this);
	for(int i=0;i<3;i++) {
		if(vertex[i]){
			Remove(vertex[i]->parent_tri,this);
		}
	}
	for (int i = 0; i<3; i++) {
		int i2 = (i+1)%3;
		if(!vertex[i] || !vertex[i2]) continue;
		vertex[i ]->RemoveIfNonNeighbor(vertex[i2]);
		vertex[i2]->RemoveIfNonNeighbor(vertex[i ]);
	}
}

void Triangle::ReplaceVertex(Vertex *vold, Vertex *vnew) {
	assert(vold && vnew);
	assert(vold==vertex[0] || vold==vertex[1] || vold==vertex[2]);
	assert(vnew!=vertex[0] && vnew!=vertex[1] && vnew!=vertex[2]);
	if(vold==vertex[0]){
		vertex[0]=vnew;
	}
	else if(vold==vertex[1]){
		vertex[1]=vnew;
	}
	else {
		assert(vold==vertex[2]);
		vertex[2]=vnew;
	}
	Remove(vold->parent_tri,this);
	assert(!Contains(vnew->parent_tri,this));
	vnew->parent_tri.push_back(this);
	for (int i = 0; i<3; i++) {
		vold->RemoveIfNonNeighbor(vertex[i]);
		vertex[i]->RemoveIfNonNeighbor(vold);
	}
	for (int i = 0; i<3; i++) {
		assert(Contains(vertex[i]->parent_tri,this)==1);
		for(int j=0;j<3;j++) if(i!=j) {
			AddUnique(vertex[i]->neighbors,vertex[j]);
			//if(!Contains(vertex[i]->neighbors,vertex[j])){
			//	vertex[i]->neighbors.push_back(vertex[j]);
			//}
		}
	}
	ComputeNormal();
}

void Init_mesh(std::vector<glm::vec3> &vert,std::vector<tridata> &tri,std::vector<int> &map, std::vector<int> &permutation,std::vector<Vertex*> *&v2,std::vector<Triangle*> *&t2){
	v2=&vertices;
	t2=&triangles;
	for (unsigned int i = 0; i<vert.size(); i++) {
		vertices.push_back(new Vertex(vert[i],i));
	}
	for (unsigned int i = 0; i<tri.size(); i++) {
		triangles.push_back(new Triangle(
				vertices[tri[i].v[0]],
				vertices[tri[i].v[1]],
				vertices[tri[i].v[2]] ));
	}

	//compute costs for colapsing each vertex
	for (unsigned int i = 0; i<vertices.size(); i++) {
		ComputeEdgeCostAtVertex(vertices[i]);
	}

	/*for (int i = 0; i < vertices.size(); ++i) {
		std::cout<<i<<" "<<vertices[i]->collapse_cost<<"\n";
	}*/
	/*for (int i = 0; i < vertices.size(); ++i) {
		std::cout<<i<<" "<<vertices[i]<<"\n";
	}*/
	permutation.resize(vertices.size());  // allocate space
	map.resize(vertices.size());          // allocate space
	while(vertices.size()) {
		Vertex *mn=vertices[0];
		for (unsigned int i = 0; i<vertices.size(); i++) {
			if(vertices[i]->collapse_cost < mn->collapse_cost) {
				mn = vertices[i];
			}
		}
		permutation[mn->id] = (int)vertices.size() - 1;
		// keep track of vertex to which we collapse to
		map[vertices.size() - 1] = (mn->collapse_target) ? mn->collapse_target->id : -1;
		Collapse(mn,mn->collapse_target);
	}
	for (unsigned int i = 0; i<map.size(); i++) {
		map[i] = (map[i]==-1)?0:permutation[map[i]];
	}


}

void ComputeEdgeCostAtVertex(Vertex *v) {
	if (v->neighbors.size() == 0) {
		// v doesn't have neighbors so it costs nothing to collapse
		v->collapse_target=NULL;
		v->collapse_cost=-0.01f;
		return;
	}
	v->collapse_cost = 1000000;
	v->collapse_target=NULL;
	// search all neighboring edges for "least cost" edge
	for (unsigned int i = 0; i<v->neighbors.size(); i++) {
		float dist = ComputeEdgeCollapseCost(v,v->neighbors[i]);
		if(dist<v->collapse_cost) {
			v->collapse_target=v->neighbors[i];  // candidate for edge collapse
			v->collapse_cost=dist;             // cost of the collapse
		}
	}
}
float ComputeEdgeCollapseCost(Vertex *u,Vertex *v) {
	float edgelength = length(v->position - u->position);
	float curvature=0;

	// find the triangles that are on the edge uv
	std::vector<Triangle *> sides;
	for (unsigned int i = 0; i<u->parent_tri.size(); i++) {
		if(u->parent_tri[i]->HasVertex(v)){
			sides.push_back(u->parent_tri[i]);
		}
	}
	// use the triangle facing most away from the sides
	// to determine our curvature term
	for (unsigned int i = 0; i<u->parent_tri.size(); i++) {
		float mincurv=1; // curve for face i and closer side to it
		for (unsigned int j = 0; j<sides.size(); j++) {
			float dotprod = dot(u->parent_tri[i]->normal , sides[j]->normal);	  // use dot product of face normals.
			mincurv = std::min(mincurv,(1-dotprod)/2.0f);
		}
		curvature = std::max(curvature, mincurv);
	}
	// the more coplanar the lower the curvature term
	return edgelength * curvature;
}

void Collapse(Vertex *u,Vertex *v){
	// Collapse the edge uv by moving vertex u onto v
	// Actually remove tris on uv, then update tris that
	// have u to have v, and then remove u.
	if(!v) {
		// u is a vertex all by itself so just delete it
		delete u;
		return;
	}
	std::vector<Vertex *>tmp;
	// make tmp a Array of all the neighbors of u
	for (unsigned int i = 0; i<u->neighbors.size(); i++) {
		tmp.push_back(u->neighbors[i]);
	}
	// delete triangles on edge uv:
	{
		auto i = u->parent_tri.size();
		while (i--) {
			if (u->parent_tri[i]->HasVertex(v)) {
				delete(u->parent_tri[i]);
			}
		}
	}
	// update remaining triangles to have v instead of u
	{
		auto i = u->parent_tri.size();
		while (i--) {
			u->parent_tri[i]->ReplaceVertex(u, v);
		}
	}
	delete u;
	// recompute the edge collapse costs for neighboring vertices
	for (unsigned int i = 0; i<tmp.size(); i++) {
		ComputeEdgeCostAtVertex(tmp[i]);
	}
}