#ifndef _TILEMAP_INCLUDE
#define _TILEMAP_INCLUDE

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>

using namespace std;

class TileMap
{
public:
	//c casillas del tilemap
	vector<vector<int>> tiles;
	unordered_set<int> models;
	map<int, vector<int>> visibility;

	
	

	TileMap();
	~TileMap();

	bool init(const char *filename);
	vector<int> split(const string & str, const string & delim);

	vector<vector<int>> getTiles();
	unordered_set<int> getModels();
	map<int, vector<int>> getVisibility();
	
};

#endif