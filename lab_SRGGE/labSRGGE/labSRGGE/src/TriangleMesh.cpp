#include <iostream>
#include <vector>
#include <iterator>
#include <map>
#include "TriangleMesh.h"
#include <cmath>
#include <Eigen/eigen>

using namespace std;

TriangleMesh::TriangleMesh()
{
}

void TriangleMesh::addVertex(const glm::vec3 &position)
{
  vertices.push_back(position);
}

void TriangleMesh::addTriangle(int v0, int v1, int v2)
{
  triangles.push_back(v0);
  triangles.push_back(v1);
  triangles.push_back(v2);
}

void TriangleMesh::buildCube()
{
	float vertices[] = {-1, -1, -1,
                      1, -1, -1,
                      1,  1, -1,
                      -1,  1, -1,
                      -1, -1,  1,
                      1, -1,  1,
                      1,  1,  1,
                      -1,  1,  1
								};

	int faces[] = {3, 1, 0, 3, 2, 1,
                 5, 6, 7, 4, 5, 7,
                 7, 3, 0, 0, 4, 7,
                 1, 2, 6, 6, 5, 1,
                 0, 1, 4, 5, 4, 1,
                 2, 3, 7, 7, 6, 2
						  };

	int i;

	for(i=0; i<8; i+=1)
		addVertex(0.5f * glm::vec3(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));
	for(i=0; i<12; i++)
		addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
}

void TriangleMesh::buildPlane()
{
	float vertices[] = { -1, -1, -1,//0
						  1, -1, -1,//1
						- 1, -1,  1,//2
						  1, -1,  1 };

	int faces[] = { 0, 2, 1, 3, 1, 2};

	int i;

	for (i = 0; i < 4; i += 1)
		addVertex(0.5f * glm::vec3(vertices[3 * i], vertices[3 * i + 1], vertices[3 * i + 2]));
	for (i = 0; i < 2; i++)
		addTriangle(faces[3 * i], faces[3 * i + 1], faces[3 * i + 2]);
}

void TriangleMesh::sendToOpenGL(ShaderProgram &program)
{
	vector<float> data;
	
	for(unsigned int tri=0; tri<triangles.size(); tri+=3)
	{
	  glm::vec3 normal;
	  
	  normal = glm::cross(vertices[triangles[tri+1]] - vertices[triangles[tri]], 
	                      vertices[triangles[tri+2]] - vertices[triangles[tri]]);
    normal = glm::normalize(normal);
    for(unsigned int vrtx=0; vrtx<3; vrtx++)
    {
      data.push_back(vertices[triangles[tri + vrtx]].x);
      data.push_back(vertices[triangles[tri + vrtx]].y);
      data.push_back(vertices[triangles[tri + vrtx]].z);

      data.push_back(normal.x);
      data.push_back(normal.y);
      data.push_back(normal.z);
    }
	}

  // Send data to OpenGL
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 3, 6*sizeof(float), 0);
	normalLocation = program.bindVertexAttribute("normal", 3, 6*sizeof(float), (void *)(3*sizeof(float)));
}

void TriangleMesh::render() const
{
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(normalLocation);
	glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * 3 * triangles.size() / 3);
}

void TriangleMesh::free()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	
	vertices.clear();
	triangles.clear();
}

vector<TriangleMesh> TriangleMesh::clusterVertices(int n)
{
	// vamos a devolver una mesh con VC y otra con QEM para el mismo LOD
	vector<TriangleMesh> meshes;
	// cada una de las meshes
	TriangleMesh lodMesh, QEMmesh;

	// el LOD va a ser función de 2**(8-n)
	int size = pow(2, 10 - n);

	// BoundingBox definidos cuando importamos la mesh
	glm::vec3 BBmin = glm::vec3(-1.0f);
	glm::vec3 BBmax = glm::vec3(1.0f);
	// avgPoint almacena la posición del nuevo vertice usando VC
	glm::vec3 avgPoint;

	// gridPos almacena la posicion minima del cubo que forma cada celda
	glm::vec3 GridPos;

	// cell guarda:
	//	[1] vector de los indices de los vertices de la mesh original
	//	[2]	los indices de la nueva mesh en VC
	//	[3] los indices de la nueva mesh en QEM
	//	[4] la matriz Q = sum(K) de cada celda (la suma de los planos K de cada vertice dentro de la celda)
	struct cell {
		vector<int> oldIndexes;
		int newIndexVC;
		int newIndexQEM;
		Eigen::Matrix4f Q;
	};

	// en este mapa guardamos el indice de la celda + info de la cleda
	map<int, cell>  vertInCell; 
	map<int, cell>::iterator  it, it0, it1, it2;

	// recorremos los indices para ver en que celda cae
	for (int i = 0; i < vertices.size(); i++) {
		// esta es la posicion en la celda
		GridPos = floor((float)size * (vertices[i] - BBmin) / (glm::distance(BBmax, BBmin)));
		// este es el indice de la celda
		int index = int(GridPos[0] + (float)size * GridPos[1] + (float)size * (float)size * GridPos[2]);

		// buscamos en el mapa si ya tenemos este indice
		it = vertInCell.find(index);
		if (it == vertInCell.end()) {
			// si no existe, creamos una celda
			// pasamos el indice de la vertice del mesh original a oldIndexes
			cell c;
			c.oldIndexes.push_back(i);
			// insertamos en el mapa el indice de la celda y la info de la celda
			vertInCell.insert(pair<int, cell> (index, c));
		}
		else
			// si ya existe, simplemente añadimos el indice del vertice de la mesh original a oldIndexes
			it->second.oldIndexes.push_back(i);
	}

	// recorremos el mapa para calcular el punto medio de los vertices
	for (auto& x : vertInCell) {
		// iniciamos el punto en el origen al princpicio del bucle
		avgPoint = glm::vec3(0.0f);

		// recorremos los indices almacenados en los puntos para pasar la coordenada para hacer la media
		for (int i = 0; i < x.second.oldIndexes.size(); i++) {
			avgPoint += vertices[x.second.oldIndexes[i]];
		}
		avgPoint /= (float)x.second.oldIndexes.size();

		// mandamos a la VCmesh el nuevo vertice
		lodMesh.vertices.push_back(glm::vec3(avgPoint));
		// mandamos su índice al mapa
		x.second.newIndexVC = lodMesh.vertices.size()-1;
	}
	
	//cout << endl;
	//cout << "VERTICES AFTER VERTEX CLUSTERING" << endl;

	//for (int e = 0; e < lodMesh.vertices.size(); e++) {
	//	std::cout << "vertice " << e << ": " << lodMesh.vertices[e].x << ", " << lodMesh.vertices[e].y << ", " << lodMesh.vertices[e].z << endl;
	//}

	// chequeamos que triangulos se quedan y cuales desaparecen
	// iteramos cada 3 vertices para recorrer cada triangulo
	for (int vert = 0; vert < triangles.size() - 2; vert += 3) {
		// primer vertice
		glm::vec3 v0 = vertices[triangles[vert]];
		// posicion en el grid
		v0 = floor(((float)size * (v0 - BBmin)) / (glm::distance(BBmax, BBmin)));
		// indice en el grid
		int iv0 = int(v0[0] + (float)size * v0[1] + (float)size * (float)size * v0[2]);


		// segundo vertice
		glm::vec3 v1 = vertices[triangles[vert + 1]];
		// posicion en el grid
		v1 = floor(((float)size * (v1 - BBmin)) / (glm::distance(BBmax, BBmin)));
		// indice en el grid
		int iv1 = int(v1[0] + (float)size * v1[1] + (float)size * (float)size * v1[2]);

		// tercer vertice
		glm::vec3 v2 = vertices[triangles[vert + 2]];
		// posicion en el grid
		v2 = floor(((float)size * (v2 - BBmin)) / (glm::distance(BBmax, BBmin)));
		// indice en el grid
		int iv2 = int(v2[0] + (float)size * v2[1] + (float)size * (float)size * v2[2]);
		
		// buscamos el iterador en cada vertice
		it0 = vertInCell.find(iv0);
		it1 = vertInCell.find(iv1);
		it2 = vertInCell.find(iv2);

		// si cada vertice del triangulo está en una celda diferente, pasamos cada vertice 
		if (iv0 != iv1 && iv0 != iv2 && iv1 != iv2) {
			// en la mesh nueva, pasamos el indice del nuevo vertice de la nueva mesh!
			lodMesh.triangles.push_back(it0->second.newIndexVC);
			lodMesh.triangles.push_back(it1->second.newIndexVC);
			lodMesh.triangles.push_back(it2->second.newIndexVC);
		}
	}

	//cout << endl;
	//for (int t = 0; t < lodMesh.triangles.size() - 2; t += 3) {
	//	cout << "triangle " << t << ": " << lodMesh.triangles[t] << ", " << lodMesh.triangles[t + 1] << ", " << lodMesh.triangles[t + 2] << endl;
	//}


	for (auto& x : vertInCell) {
		Eigen::Vector4f b(0, 0, 0, 1);
		Eigen::Vector4f vSol(0, 0, 0, 0);

		x.second.Q(3, 0) = 0;
		x.second.Q(3, 1) = 0;
		x.second.Q(3, 2) = 0;
		x.second.Q(3, 3) = 1;

		glm::vec3 vertexSol;

		//if (abs(x.second.Q.determinant()) < 0.00001) {
		//	vertexSol = lodMesh.vertices[x.second.newIndexVC]; // no es vsol pero es pareccido
		//}
		//else {
		//	vSol = x.second.Q.fullPivLu().solve(b);
		//	vertexSol = glm::vec3(vSol(0),vSol(1),vSol(2));
		//}

		vertexSol = glm::vec3(vSol(0), vSol(1), vSol(2));

		glm::vec3 v = floor(((float)size * (v - BBmin)) / (glm::distance(BBmax, BBmin)));
		int iv = int(v[0] + (float)size * v[1] + (float)size * (float)size * v[2]);

		if(iv != x.first) 
			vertexSol = lodMesh.vertices[x.second.newIndexVC];

		QEMmesh.vertices.push_back(vertexSol);
		x.second.newIndexQEM = QEMmesh.vertices.size() - 1;
	}

	//cout << endl;
	//cout << "VERTICES AFTER QEM" << endl;

	//for (int e = 0; e < QEMmesh.vertices.size(); e++) {
	//	std::cout << "vertice " << e << ": " << QEMmesh.vertices[e].x << ", " << QEMmesh.vertices[e].y << ", " << QEMmesh.vertices[e].z << endl;
	//}


	//// ahora hay que chequear que triangulos se quedan y cuales desaparecen
	for (int vert = 0; vert < triangles.size() - 2; vert += 3) {
		glm::vec3 v0 = vertices[triangles[vert]];
		v0 = floor(((float)size * (v0 - BBmin)) / (glm::distance(BBmax, BBmin)));
		int iv0 = int(v0[0] + (float)size * v0[1] + (float)size * (float)size * v0[2]);

		glm::vec3 v1 = vertices[triangles[vert + 1]];
		v1 = floor(((float)size * (v1 - BBmin)) / (glm::distance(BBmax, BBmin)));
		int iv1 = int(v1[0] + (float)size * v1[1] + (float)size * (float)size * v1[2]);

		glm::vec3 v2 = vertices[triangles[vert + 2]];
		v2 = floor(((float)size * (v2 - BBmin)) / (glm::distance(BBmax, BBmin)));
		int iv2 = int(v2[0] + (float)size * v2[1] + (float)size * (float)size * v2[2]);

		it0 = vertInCell.find(iv0);
		it1 = vertInCell.find(iv1);
		it2 = vertInCell.find(iv2);

		if (iv0 != iv1 && iv0 != iv2 && iv1 != iv2) {
			QEMmesh.triangles.push_back(it0->second.newIndexQEM);
			QEMmesh.triangles.push_back(it1->second.newIndexQEM);
			QEMmesh.triangles.push_back(it2->second.newIndexQEM);
		}
	}

	//cout << endl;
	//for (int t = 0; t < QEMmesh.triangles.size() - 2; t += 3) {
	//	cout << "triangle " << t << ": " << QEMmesh.triangles[t] << ", " << QEMmesh.triangles[t + 1] << ", " << QEMmesh.triangles[t + 2] << endl;
	//}

	meshes.push_back(lodMesh);
	meshes.push_back(QEMmesh);
	/*cout << "meshes cargadas" << meshes.size() << endl;*/
	return meshes;
}

Eigen::Matrix4d TriangleMesh::computeQ(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
	Eigen::Matrix4d Q;
	glm::vec3 n = glm::normalize(glm::cross(v1 - v0, v2 - v0));
	double a = (double)n[0];
	double b = (double)n[1];
	double c = (double)n[2];
	double d = -a * (double)v0[0] - b * (double)v0[1] - c * (double)v0[2];
	
	Q << a*a, a*b, a*c, a*d,
		 b*a, b*b, b*c, b*d,
		 c*a, c*b, c*c, c*d,
		 d*a, d*b, d*c, d*d;

	return Q;
}

vector<glm::vec3>& TriangleMesh::getVertices() {
	return vertices;
}
	
vector<int>& TriangleMesh::getTriangles() {
	return triangles;
}