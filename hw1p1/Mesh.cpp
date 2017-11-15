#define _CRT_SECURE_NO_WARNINGS
#include "Mesh.h"
#include <cstdio>

// set mtl file location
const char *obj_database = "";

Material::Material() {
	Init();
}

void Material::Init() {
	for (size_t i = 0; i < 4; i++) {
		Ka[i] = Kd[i] = Ks[i] = 1.0f;
	}
	Ns = 0.0f;
	d = 1.0f;
}

Mesh::Mesh() {
	Init();
}

void Mesh::Init() {
	vTotal = nTotal = tTotal = fTotal = mTotal = 0;

	vList.clear();
	nList.clear();
	tList.clear();
	mList.clear();
	faceList.clear();

	Vec3 default_value = Vec3(1, 1, 1);
	vList.push_back(default_value);
	nList.push_back(default_value);
	tList.push_back(default_value);

	Material default_material;
	mList.push_back(default_material);
	mTotal++;
	// default material is mList[0]
	mList[0].Ka[0] = 0; mList[0].Ka[1] = 0; mList[0].Ka[2] = 0; mList[0].Ka[3] = 1;
	mList[0].Kd[0] = 1; mList[0].Kd[1] = 1; mList[0].Kd[2] = 1; mList[0].Kd[3] = 1;
	mList[0].Ks[0] = 0.8f; mList[0].Ks[1] = 0.8f; mList[0].Ks[2] = 0.8f; mList[0].Ks[3] = 1;
	mList[0].Ns = 32;
}

Mesh::Mesh(const char *filename) {
	LoadMesh(filename);
}

Mesh::~Mesh() {

}

static void skipRestOfLine(FILE *f) {
	int ch;
	do {
		ch = fgetc(f);
	} while (ch != '\n' && ch != EOF);
}

void Mesh::LoadMesh(std::string filename) {
	// remove old content
	Init();

	// open obj file
	FILE *f;
	f = fopen(filename.c_str(), "r");
	if (NULL == f) {
		printf("Cannot open file \"%s\"\n", filename.c_str());
		return;
	}
	printf("loading mesh %s\n", filename.c_str());
	while (!feof(f)) {
		char cmd[50];
		cmd[0] = '\0';
		if (fscanf(f, "%40s", cmd) != 1) break;
		float x, y, z;
		int currentMaterial = 0;

		if (strcmp(cmd, "g") == 0) {
			// read name
			// however the program doesn't use name, so do nothing
		}
		else if (strcmp(cmd, "mtllib") == 0) {
			char name[50];
			if (fscanf(f, "%40s", name) == 1) {
				LoadMaterial(std::string(obj_database) + std::string(name));
			}
		}
		else if (strcmp(cmd, "v") == 0) {
			
			if (fscanf(f, "%f %f %f", &x, &y, &z) == 3) {
				vList.push_back(Vec3(x, y, z));
			}
		}
		else if (strcmp(cmd, "vn") == 0) {
			if (fscanf(f, "%f %f %f", &x, &y, &z) == 3) {
				nList.push_back(Vec3(x, y, z));
			}
		}
		else if (strcmp(cmd, "vt") == 0) {
			if (fscanf(f, "%f %f", &x, &y) == 2) {
				tList.push_back(Vec3(x, y, 0));
			}
		}
		else if (strcmp(cmd, "usemtl") == 0) {
			char name[50];
			if (fscanf(f, "%40s", name) == 1) {
				std::string s(name);
				currentMaterial = matMap[s];
			}
		}
		else if (strcmp(cmd, "f") == 0) {
			// the program only supports 3 vertices in a triangle
			Vertex p[3];
			for (int i = 0; i < 3; i++) {
				p[i] = Vertex(0, 0, 0);
				char part[50];
				part[0] = 0;
				if (fscanf(f, "%40s", part) <= 0) continue;
				int vnt[3] = { 0,0,0 };
				int y = 0, n = 0;
				for (int j = 0; part[j] && n < 2; j++) {
					if (part[j] == '/') { // find a /
						part[j] = '\0';
						vnt[n++] = atoi(&part[y]);
						y = j + 1;
					}
				}
				if (part[y]) {
					vnt[n] = atoi(&part[y]);
				}
				p[i] = Vertex(vnt[0], vnt[1], vnt[2]);
			}
			faceList.push_back(FACE(p[0], p[1], p[2], currentMaterial));
		}
		else if (cmd[0] == '#') {
			// comment
		}
		else {
			printf("unknown tag %s\n", cmd);
		}
		skipRestOfLine(f);
	}
	if (f != NULL) {
		fclose(f);
	}
	vTotal = vList.size();
	nTotal = nList.size();
	tTotal = tList.size();
	fTotal = faceList.size();
	printf("loaded vertex x %d, normal x %d, texture x %d, face x %d\n", vTotal-1, nTotal-1, tTotal-1, fTotal);
}

void Mesh::LoadMaterial(std::string filename) {
	// open mtl file
	FILE *f;
	f = fopen(filename.c_str(), "r");
	if (NULL == f) {
		printf("Cannot open file \"%s\"\n", filename.c_str());
		return;
	}
	printf("loading material %s\n", filename.c_str());
	size_t currentMat;
	float r, g, b;
	while (!feof(f)) {
		char cmd[50];
		cmd[0] = '\0';
		if (fscanf(f, "%40s", cmd) != 1) break;

		if (strcmp(cmd, "#") == 0) {
			// comment
		}
		else if (strcmp(cmd, "newmtl") == 0) {
			char name[50];
			if (fscanf(f, "%40s", name) == 1) {
				Material mat;
				mList.push_back(mat);
				currentMat = mTotal++;
				matMap[name] = currentMat;
			}
		}
		else if (strcmp(cmd, "Ka") == 0) {
			if (fscanf(f, "%f %f %f", &r, &g, &b) == 3) {
				mList[currentMat].Ka[0] = r;
				mList[currentMat].Ka[1] = g;
				mList[currentMat].Ka[2] = b;
				mList[currentMat].Ka[3] = 1;
			}
		}
		else if (strcmp(cmd, "Kd") == 0) {
			if (fscanf(f, "%f %f %f", &r, &g, &b) == 3) {
				mList[currentMat].Kd[0] = r;
				mList[currentMat].Kd[1] = g;
				mList[currentMat].Kd[2] = b;
				mList[currentMat].Kd[3] = 1;
			}
		}
		else if (strcmp(cmd, "Ks") == 0) {
			if (fscanf(f, "%f %f %f", &r, &g, &b) == 3) {
				mList[currentMat].Ks[0] = r;
				mList[currentMat].Ks[1] = g;
				mList[currentMat].Ks[2] = b;
				mList[currentMat].Ks[3] = 1;
			}
		}
		else if (strcmp(cmd, "d") == 0) {
			if (fscanf(f, "%f", &r) == 1) {
				mList[currentMat].d = r;
			}
		}
		else if (strcmp(cmd, "Ns") == 0) {
			if (fscanf(f, "%f", &r) == 1) {
				mList[currentMat].Ns = r;
			}
		}
		else if (strcmp(cmd, "Tr") == 0) {
			if (fscanf(f, "%f", &r) == 1) {
				mList[currentMat].d = 1.0f - r;
			}
		}
		else if (strcmp(cmd, "d") == 0) {
			if (fscanf(f, "%f", &r) == 1) {
				mList[currentMat].d = r;
			}
		}
		else {
			printf("unknown tag %s\n", cmd);
		}
		skipRestOfLine(f);
	}
	fclose(f);
}