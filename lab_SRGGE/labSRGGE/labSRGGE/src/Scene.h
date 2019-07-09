#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "Camera.h"
#include "tileMapParser.h"
#include "tileMapParser.h"
#include "ShaderProgram.h"
#include "TriangleMesh.h"
#include "Text.h"
#include <fstream>
#include <string>
#include <unordered_set>


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	int maxCost = 1000000; //1M triangles???

	void init();
	bool loadMesh(const char *filename);
	bool loadMeshesToMap(const char *filename, int n);
	void update(int deltaTime);
	void render();

  Camera &getCamera();
  //TileMap & getTiles();
  
  void switchPolygonMode();

private:
	void initShaders();
	//void computeViewMatrix();

private:
	
  TileMap tilemap;
  vector<vector<int>> tiles;
  unordered_set<int> models;
  map<int, vector<int>> visibility;

  Camera camera;
	//TriangleMesh *mesh;
	std::vector<TriangleMesh> meshes;
	std::map<int, vector<TriangleMesh>> meshMap;

	ShaderProgram basicProgram;
	float currentTime, lastTime, FPS;
	int currentFrames, lastFrames;
	
	bool bPolygonFill;

	//void parseTileMap(const char * filename);

	//std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str);
	
	Text text;

};


#endif // _SCENE_INCLUDE

