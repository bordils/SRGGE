#ifndef _TRIANGLE_MESH_INCLUDE
#define _TRIANGLE_MESH_INCLUDE


#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include <Eigen/eigen>


using namespace std;


// Class TriangleMesh renders a very simple room with textures


class TriangleMesh
{

public:
	TriangleMesh();

	void addVertex(const glm::vec3 &position);
	void addTriangle(int v0, int v1, int v2);

	void buildCube();
	void buildPlane();
	
	void sendToOpenGL(ShaderProgram &program);
	void render() const;
	void free();

	vector<glm::vec3>& getVertices();
	vector<int>& getTriangles();

	vector<TriangleMesh> clusterVertices(int n);
	//TriangleMesh QEMclustering(TriangleMesh m, int n);

	vector<glm::vec3> vertices;
	vector<int> triangles;

private:
	Eigen::Matrix4d computeQ(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);

  //vector<glm::vec3> vertices;
  //vector<int> triangles;

	GLuint vao;
	GLuint vbo;
	GLint posLocation, normalLocation;
	
};


#endif // _TRIANGLE_MESH_INCLUDE


