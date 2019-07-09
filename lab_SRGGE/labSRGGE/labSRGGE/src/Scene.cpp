#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <cmath>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Scene.h"
#include "PLYReader.h"


Scene::Scene()
{
	//mesh = NULL;
}

Scene::~Scene()
{
	//if(mesh != NULL)
	//	delete mesh;
}


void Scene::init()
{
	initShaders();

	Camera camera;

	//read map and get tiles in a vector [i][j]
	tilemap.init("src/museum_models.csv");
	tiles = tilemap.getTiles();
	// with elements in models, send instructions to load those meshes // dont use 0 as index for rendering
	models = tilemap.getModels();
	visibility = tilemap.getVisibility();

	for ( auto it = models.begin(); it != models.end(); ++it) {
		int idx = *it;
		TriangleMesh mesh = TriangleMesh();
		TriangleMesh mesh1 = TriangleMesh();

		switch (idx) {
			case 1:
				mesh.buildCube();
				mesh.sendToOpenGL(basicProgram);
				meshes.push_back(mesh);

				meshMap.insert(pair<int, vector<TriangleMesh>>(1, meshes));
				meshes.clear();
				break;
			case 16:
				mesh1.buildPlane();
				mesh1.sendToOpenGL(basicProgram);
				meshes.push_back(mesh1);

				meshMap.insert(pair<int, vector<TriangleMesh>>(16, meshes));
				meshes.clear();
				break;
			case 18:
				loadMeshesToMap("models/frog.ply",18);
				break;

			case 20:
				loadMeshesToMap("models/maxplanck.ply", 20);
				break;
			case 32:
				loadMeshesToMap("models/Armadillo.ply", 32);
				break;
			case 36:
				loadMeshesToMap("models/moai.ply", 36);
				break;
			case 48:
				loadMeshesToMap("models/bunny.ply", 48);
				break;
			case 50:
				loadMeshesToMap("models/horse.ply", 50);
				break;
			case 34:
				loadMeshesToMap("models/happy.ply", 34);
				break;
			case 2:
				loadMeshesToMap("models/frog.ply", 2);
				break;
			case 11:
				//
				break;
			case 123:
				//
				break;
			case 1233:
				// 
				break;
			case 11234:
				loadMeshesToMap("models/lucy.ply", 11234);
				break;
			default:
				break;
		}
	}
	
	// in meshMap we keep all the meshes
	// key of the dictionary is the value in the tilemap
	// for all meshes differnet to wall and floor, the second element keeps all the meshes with differnet LODs for VC and QEM as well as the original mesh
	// meshMap[i][j] :
	// for i:
	//		1  == wall
	//		16 == floor
	//		18 == frog
	//		20 == maxplanck
	//		32 == armadillo
	//		36 == moai
	//		48 == bunny
	//		50 == horse
	//
	// for j:
	//		0 = original mesh
	//		1 = VC LOD 1
	//		2 = QEM LOD 1
	//		3 = VC LOD 2
	//		4 = QEM LOD 2
	//		5 = VC LOD 3
	//		6 = QEM LOD 3
	//		7 = VC LOD 4
	//		8 = QEM LOD 4

	currentTime = 0.0f;
	
	bPolygonFill = true;
	
	// Select which font you want to use
	if(!text.init("fonts/OpenSans-Regular.ttf"))
	//if(!text.init("fonts/OpenSans-Bold.ttf"))
	//if(!text.init("fonts/DroidSerif.ttf"))
		cout << "Could not load font!!!" << endl;
}

bool Scene::loadMesh(const char *filename)
{
	PLYReader reader;

	TriangleMesh mesh;
	std::vector<TriangleMesh> meshes;

	bool bSuccess = reader.readMesh(filename, mesh);
	if(bSuccess)
	  mesh.sendToOpenGL(basicProgram);

	meshes.push_back(mesh);
	
	return bSuccess;
}

bool Scene::loadMeshesToMap(const char *filename, int n)
{
	PLYReader reader;

	TriangleMesh mesh;
	vector<TriangleMesh> meshes, decimatedMeshes;

	bool bSuccess = reader.readMesh(filename, mesh);
	if (bSuccess)
		mesh.sendToOpenGL(basicProgram);

	meshes.push_back(mesh);

	for (int i = 1; i < 5; i++) {
		decimatedMeshes = mesh.clusterVertices(i);
		meshes.push_back(decimatedMeshes[0]);
		meshes.push_back(decimatedMeshes[1]);
	}

	for (int i = 1; i < 9; i++) {
		meshes[i].sendToOpenGL(basicProgram);
	}

	meshMap.insert(pair<int, std::vector<TriangleMesh>>(n,meshes));

	return bSuccess;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
	FPS = 1.0f / deltaTime * 1000.0f;
}

void Scene::render()
{
	glm::vec3 cameraPosition = camera.getCameraPosition();
	int camIndex = -(round(cameraPosition.z) + round(cameraPosition.x)*32.0f);

	glm::mat3 normalMatrix;
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 projection = camera.getProjectionMatrix();

	normalMatrix = glm::inverseTranspose(camera.getViewMatrix());

	basicProgram.use();
	basicProgram.setUniformMatrix4f("projection", projection);	
	basicProgram.setUniformMatrix3f("normalMatrix", normalMatrix);
	
	basicProgram.setUniform1i("bLighting", bPolygonFill?1:0);
	if(bPolygonFill)
	{
  	basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} else
	{
  		basicProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.5f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		meshMap[1][0].render(); ////////////////////////////////////////////////////cambiar esto para todos 
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_POLYGON_OFFSET_FILL);
  		basicProgram.setUniform4f("color", 0.0f, 0.0f, 0.0f, 1.0f);
	}
	

	bool totalCost = false;
	bool currentMaxBenefit;
	int cost = 0;
	float d = glm::distance(glm::vec3(-1.0f), glm::vec3(1.0f));

	// store which level of detail for each mesh to render
	vector<int> L(visibility[camIndex].size(), 8);
	int LODstop = 0;
	vector<float> MeshPotentialBenefit(visibility[camIndex].size(), -1.0f);
	vector<float> MeshPotentialCost(visibility[camIndex].size(), 1.0f);

	// first loop to calculate the cost of rendering the scene. Not taking into account the walls or floor
	for (int cell = 0; cell < visibility[camIndex].size(); cell++) {
		int j = int(float(visibility[camIndex][cell]) / 32.0);
		int i = visibility[camIndex][cell] % 32;
		//stop criteria if max LOD is reached for the while loop
		if (tiles[i][j] == 1 || tiles[i][j] == 16) LODstop += 1;
		if (tiles[i][j] != 1 && tiles[i][j] != 16) {
			// first we store the baseline cost for the whole scene
			int meshCost = meshMap[tiles[i][j]][L[cell]].getTriangles().size();
			cost += meshCost;

			// store potential upgrading cost for each mesh
			MeshPotentialCost[cell] = meshMap[tiles[i][j]][L[cell] - 2].getTriangles().size();

			// store the potential benefit for each mesh
			// first calculate the distance from the cell to the viewpoint
			float D = glm::distance(cameraPosition, glm::vec3((float)-i, 0.0f, (float)-j));

			// for selecting level of detail, we assign a dummy variable
			int a;
			if (L[cell] = 8) a = 1;
			else if (L[cell] = 6) a = 2;
			else if (L[cell] = 4) a = 3;
			else if (L[cell] = 2) a = 4;
			// in this vector we store potential upgrading benefit for each cell
			MeshPotentialBenefit[cell] = (float)(pow(2, a)) * D / d;
		}
	}

	float maxImprovement = -100000.0;
	int cellMax;
				
	while (!totalCost) {
		for (int cell = 0; cell < visibility[camIndex].size(); cell++) {
			if (L[cell] != 2) {
				// compare and store max improvement and its index
				if (MeshPotentialBenefit[cell] / MeshPotentialCost[cell] > maxImprovement) {
					maxImprovement = MeshPotentialBenefit[cell];
					cellMax = cell;
				}
			}
		}
		// access the position of the cell with max benefit
		int j = int(float(visibility[camIndex][cellMax]) / 32.0);
		int i = visibility[camIndex][cellMax] % 32;

		// compute what would be the cost to be added. If we reached Max LOD, stop the loop		 
 		int decCost = meshMap[tiles[i][j]][L[cellMax]].getTriangles().size();
		int incCost = meshMap[tiles[i][j]][L[cellMax] - 2].getTriangles().size();
		cost = cost - decCost + incCost;

		// if it is above the threshold, stop the loop, else update level of detail
		if (cost > maxCost) totalCost = true;
		else {
			// upgrade LOD
			L[cellMax] -= 2;

			// update LODstop 
			if (L[cellMax] == 2) {
				LODstop += 1;
			}
			else {
				// update cost 
				MeshPotentialCost[cellMax] = incCost;

				//update potential benefit
				MeshPotentialBenefit[cellMax] *= 2;
			}
			// in case we achieve maxLOD for all models in the scene, we stop
			if (LODstop == visibility[camIndex].size()) totalCost = true;
		}
		maxImprovement = -100000.0;
	}

	for (int cell = 0; cell < visibility[camIndex].size(); cell++) {
		int j = int(float(visibility[camIndex][cell]) / 32.0);
		int i = visibility[camIndex][cell] % 32;

		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-0.5,0.0,-0.5));
		model = glm::translate(model, glm::vec3(-i, 0.0, -j));
		// wall
		if (tiles[i][j] == 1) {
			//model = glm::rotate(model, 0.0f, glm::vec3(0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 3.0f, 1.0f));
			glm::mat4 MVM = view * model;
			basicProgram.setUniformMatrix4f("modelview", MVM);
			meshMap[1][0].render();
			//meshes[0].render();
		}
		// floor
		else if (tiles[i][j] == 16) {
			glm::mat4 MVM = view * model;
			basicProgram.setUniformMatrix4f("modelview", MVM);
			basicProgram.setUniform4f("color", 0.95f, 0.85f, 0.95f, 1.0f);
			meshMap[16][0].render();
			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
		}
		// frog
		else if (tiles[i][j] == 18) {
			glm::mat4 MVM = view * model;
			basicProgram.setUniformMatrix4f("modelview", MVM);
			meshMap[16][0].render();
			if (L[cell] == 8) basicProgram.setUniform4f("color", 0.2f, 0.2f, 1.0f, 1.0f);
			if (L[cell] == 6) basicProgram.setUniform4f("color", 0.2f, 0.1f, 0.2f, 1.0f);
			if (L[cell] == 4) basicProgram.setUniform4f("color", 0.1f, 0.2f, 0.2f, 1.0f);
			if (L[cell] == 2) basicProgram.setUniform4f("color", 0.2f, 0.2f, 0.2f, 1.0f);

			meshMap[18][L[cell]].render();
			//basicProgram.setUniform4f("color", 1.0f, 0.2f, 0.2f, 1.0f);
			//meshMap[18][3].render();
			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
		}
		// maxplanck
		else if (tiles[i][j] == 20) {
			glm::mat4 MVM = view * model;
			basicProgram.setUniformMatrix4f("modelview", MVM);
			meshMap[16][0].render();
			if (L[cell] == 8) basicProgram.setUniform4f("color", 0.2f, 0.2f, 1.0f, 1.0f);
			if (L[cell] == 6) basicProgram.setUniform4f("color", 0.2f, 0.1f, 0.2f, 1.0f);
			if (L[cell] == 4) basicProgram.setUniform4f("color", 0.1f, 0.2f, 0.2f, 1.0f);
			if (L[cell] == 2) basicProgram.setUniform4f("color", 0.2f, 0.2f, 0.2f, 1.0f);

			meshMap[20][L[cell]].render();
			//basicProgram.setUniform4f("color", 0.2f, 1.0f, 0.2f, 1.0f);
			//meshMap[20][3].render();
			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
		}
		// armadillo
		else if (tiles[i][j] == 32) {
			//model = glm::rotate(model, 0.0f, glm::vec3(0.0f));
			glm::mat4 MVM = view * model;
			basicProgram.setUniformMatrix4f("modelview", MVM);
			meshMap[16][0].render();
			if (L[cell] == 8) basicProgram.setUniform4f("color", 0.2f, 0.2f, 1.0f, 1.0f);
			if (L[cell] == 6) basicProgram.setUniform4f("color", 0.2f, 0.1f, 0.2f, 1.0f);
			if (L[cell] == 4) basicProgram.setUniform4f("color", 0.1f, 0.2f, 0.2f, 1.0f);
			if (L[cell] == 2) basicProgram.setUniform4f("color", 0.2f, 0.2f, 0.2f, 1.0f);

			meshMap[32][L[cell]].render();

			//basicProgram.setUniform4f("color", 0.2f, 0.2f, 1.0f, 1.0f);
			//meshMap[32][3].render();
			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
		}
		// moai
		else if (tiles[i][j] == 36) {
			//model = glm::rotate(model, 0.0f, glm::vec3(0.0f));
			glm::mat4 MVM = view * model;
			basicProgram.setUniformMatrix4f("modelview", MVM);
			meshMap[16][0].render();
			if (L[cell] == 8) basicProgram.setUniform4f("color", 0.2f, 0.2f, 1.0f, 1.0f);
			if (L[cell] == 6) basicProgram.setUniform4f("color", 0.2f, 0.1f, 0.2f, 1.0f);
			if (L[cell] == 4) basicProgram.setUniform4f("color", 0.1f, 0.2f, 0.2f, 1.0f);
			if (L[cell] == 2) basicProgram.setUniform4f("color", 0.2f, 0.2f, 0.2f, 1.0f);
			
			meshMap[36][L[cell]].render();
			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
		}
		// bunny
		else if (tiles[i][j] == 48) {
			//model = glm::rotate(model, 0.0f, glm::vec3(0.0f));
			glm::mat4 MVM = view * model;
			basicProgram.setUniformMatrix4f("modelview", MVM);
			meshMap[16][0].render();
			basicProgram.setUniform4f("color", 0.2f, 0.2f, 1.0f, 1.0f);
			meshMap[48][3].render();
			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
		}
		// horse
		else if (tiles[i][j] == 50) {
			//model = glm::rotate(model, 0.0f, glm::vec3(0.0f));
			glm::mat4 MVM = view * model;
			basicProgram.setUniformMatrix4f("modelview", MVM);
			meshMap[16][0].render();

			if (L[cell] == 8) basicProgram.setUniform4f("color", 0.2f, 0.2f, 1.0f, 1.0f);
			if (L[cell] == 6) basicProgram.setUniform4f("color", 0.2f, 0.1f, 0.2f, 1.0f);
			if (L[cell] == 4) basicProgram.setUniform4f("color", 0.1f, 0.2f, 0.2f, 1.0f);
			if (L[cell] == 2) basicProgram.setUniform4f("color", 0.2f, 0.2f, 0.2f, 1.0f);

			meshMap[50][L[cell]].render();
			//basicProgram.setUniform4f("color", 0.2f, 0.2f, 1.0f, 1.0f);
			//meshMap[50][3].render();
			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
		}
		// happy
		else if (tiles[i][j] == 34) {
			//model = glm::rotate(model, 0.0f, glm::vec3(0.0f));
			glm::mat4 MVM = view * model;
			basicProgram.setUniformMatrix4f("modelview", MVM);
			meshMap[16][0].render();

			if (L[cell] == 8) basicProgram.setUniform4f("color", 0.2f, 0.2f, 1.0f, 1.0f);
			if (L[cell] == 6) basicProgram.setUniform4f("color", 0.2f, 0.1f, 0.2f, 1.0f);
			if (L[cell] == 4) basicProgram.setUniform4f("color", 0.1f, 0.2f, 0.2f, 1.0f);
			if (L[cell] == 2) basicProgram.setUniform4f("color", 0.2f, 0.2f, 0.2f, 1.0f);

			meshMap[34][L[cell]].render();
			//basicProgram.setUniform4f("color", 0.2f, 0.2f, 1.0f, 1.0f);
			//meshMap[34][3].render();
			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
		}
	}

	//for (int i = 0; i < tiles.size(); i++)
	//{
	//	for (int j= 0; j < tiles[i].size(); j++)
	//	{
	//		model = glm::mat4(1.0);
	//		model = glm::translate(model, glm::vec3(-i, 0.0, -j));
	//		//// wall
	//		//if (tiles[i][j] == 1) {
	//		//	//model = glm::rotate(model, 0.0f, glm::vec3(0.0f));
	//		//	model = glm::scale(model, glm::vec3(1.0f, 3.0f, 1.0f));
	//		//	glm::mat4 MVM = view * model;
	//		//	basicProgram.setUniformMatrix4f("modelview", MVM);
	//		//	meshMap[1][0].render();
	//		//	//meshes[0].render();
	//		//}
	//		// floor
	//		if (tiles[i][j] == 16) {
	//			glm::mat4 MVM = view * model;
	//			basicProgram.setUniformMatrix4f("modelview", MVM);
	//			basicProgram.setUniform4f("color", 0.95f, 0.85f, 0.95f, 1.0f);
	//			meshMap[16][0].render();
	//			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
	//		}
	//		// dragon
	//		else if (tiles[i][j] == 2) {
	//			model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//			glm::mat4 MVM = view * model;
	//			basicProgram.setUniformMatrix4f("modelview", MVM);
	//			meshMap[16][0].render();
	//			basicProgram.setUniform4f("color", 1.0f, 0.2f, 0.2f, 1.0f);
	//			meshMap[2][0].render();
	//			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
	//		}
	//		// dragon VC1
	//		else if (tiles[i][j] == 18) { 
	//			model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//			glm::mat4 MVM = view * model;
	//			basicProgram.setUniformMatrix4f("modelview", MVM);
	//			meshMap[16][0].render();
	//			basicProgram.setUniform4f("color", 0.8f, 0.2f, 0.2f, 1.0f);
	//			meshMap[2][2].render();
	//			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
	//		}  
	//		// dragon VC2
	//		else if (tiles[i][j] == 19) { 
	//			//model = glm::rotate(model, 0.0f, glm::vec3(0.0f));
	//			model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//			glm::mat4 MVM = view * model;				
	//			basicProgram.setUniformMatrix4f("modelview", MVM);
	//			meshMap[16][0].render();
	//			basicProgram.setUniform4f("color", 0.6f, 0.2f, 0.2f, 1.0f);
	//			meshMap[2][4].render();
	//			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
	//		}
	//		// dragon VC3
	//		else if (tiles[i][j] == 20) {
	//			//model = glm::rotate(model, 0.0f, glm::vec3(0.0f));
	//			model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//			glm::mat4 MVM = view * model;
	//			basicProgram.setUniformMatrix4f("modelview", MVM);
	//			meshMap[16][0].render();
	//			basicProgram.setUniform4f("color", 0.4f, 0.2f, 0.2f, 1.0f);
	//			meshMap[2][6].render();
	//			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
	//		}
	//		// dragon VC4
	//		else if (tiles[i][j] == 21) {
	//			//model = glm::rotate(model, 0.0f, glm::vec3(0.0f));
	//			model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//			glm::mat4 MVM = view * model;
	//			basicProgram.setUniformMatrix4f("modelview", MVM);
	//			meshMap[16][0].render();
	//			basicProgram.setUniform4f("color", 0.2f, 0.2f, 0.2f, 1.0f);
	//			meshMap[2][8].render();
	//			basicProgram.setUniform4f("color", 0.9f, 0.9f, 0.95f, 1.0f);
	//		}
	//		else {
	//			//model = glm::rotate(model, 0.0f, glm::vec3(0.0f));
	//			glm::mat4 MVM = view * model;
	//			basicProgram.setUniformMatrix4f("modelview", MVM);
	//			meshMap[16][0].render();
	//			basicProgram.setUniform4f("color", 0.2f, 0.2f, 1.0f, 1.0f);
	//		}
	//	};
	//};

	string fpsToRender = "FPS: "+ std::to_string(FPS);
	string costTriangles = "Rendered triangles: " + std::to_string(cost);
	string MaxTriangles = "Max triangles: " + std::to_string(maxCost);
	text.render("Mesh renderer", glm::vec2(20, 20), 16, glm::vec4(0, 0, 0, 1));
	text.render(fpsToRender, glm::vec2(20, 40), 16, glm::vec4(0, 0, 0, 1));
	text.render(costTriangles, glm::vec2(20, 60), 16, glm::vec4(0, 0, 0, 1));
	text.render(MaxTriangles, glm::vec2(20, 80), 16, glm::vec4(0, 0, 0, 1));
}

Camera &Scene::getCamera()
{
  return camera;
}

void Scene::switchPolygonMode()
{
  bPolygonFill = !bPolygonFill;
}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/basic.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/basic.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	basicProgram.init();
	basicProgram.addShader(vShader);
	basicProgram.addShader(fShader);
	basicProgram.link();
	if(!basicProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << basicProgram.log() << endl << endl;
	}
	basicProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}

