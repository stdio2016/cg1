#pragma once
#ifndef MESH_INCLUDED
#define MESH_INCLUDED
#include <vector>
#include <string>
#include <map>
#include "Vec3.h"
struct Material {
public:
	float Ka[4]; // ambient coefficient
	float Kd[4]; // diffuse coefficient
	float Ks[4]; // specular coefficient
	float Ns; // Phone specular component (shininess)

	float d; // transparency (d = 1 means opaque)
	std::string map_Ka; // ambient texture, unused
	std::string map_Kd; // diffuse texture, unused
	std::string map_Ks; // specular texture, unused

	Material();
	void Init(void);
};

class Mesh
{
public:
	struct Vertex
	{
	public:
		size_t v; // vertex index in vList
		size_t t; // texture coordinate index in tList
		size_t n; // normal vector index in nList
		Vertex() {}
		Vertex(size_t v_idx, size_t t_idx, size_t n_idx) {
			v = v_idx;
			t = t_idx;
			n = n_idx;
		}
	};

	struct FACE
	{
	public:
		Vertex v[3]; // assume 3 vertices for each face
		int m; // material index
		FACE(Vertex v1, Vertex v2, Vertex v3, int material) {
			v[0] = v1;
			v[1] = v2;
			v[2] = v3;
			m = material;
		}
		inline const Vertex& operator[] (size_t i) const { return v[i]; }
		inline Vertex& operator[] (size_t i) { return v[i]; }
	};

	// create an empty mesh
	Mesh(void);

	// initialize mesh
	void Init(void);

	// load mesh from file
	Mesh(const char *filename);
	void LoadMesh(std::string filename);
	
	// destructor of mesh
	~Mesh();
	
	std::vector<Material> mList; // material list
	std::map<std::string, size_t> matMap; // material name -> material index

	std::vector<Vec3> vList; // vertex position list
	std::vector<Vec3> tList; // texture coordinate list
	std::vector<Vec3> nList; // normal vector list
	
	std::vector<FACE> faceList; // array of all faces/polygons

	size_t mTotal; // number of materials
	size_t vTotal; // number of vertices
	size_t tTotal; // number of texture coordinate
	size_t nTotal; // number of normal vectors
	size_t fTotal; // number of faces
private:
	void LoadMaterial(std::string filename);
};
#endif