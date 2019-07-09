#include "tileMapParser.h"
//#include <algorithm>
//#include <fstream>
//#include <iostream>
//#include <iterator>
//#include <sstream>
//#include <string>
//#include <vector>

using namespace std;

TileMap::TileMap() {


}

TileMap::~TileMap() 
{
}

bool TileMap::init(const char* filename) {

	ifstream stream;
	stream.open(filename, ios::in);
	//stream.open(filename, std::ifstream::in);
	if (!stream.is_open()) cout << "ERORR: Map File  not Open" << endl;

	for (string line; getline(stream, line); )
	{
		replace(line.begin(), line.end(), ',', ' ');
		istringstream in(line);
		tiles.push_back(
			vector<int>(istream_iterator<int>(in),
				istream_iterator<int>()));
	}
	stream.close();

	ifstream visibility_file;
	visibility_file.open("src/museum_visibility.txt", ios::in);
	if (!visibility_file.is_open()) cout << "ERROR: Visibility File not Open" << endl;

	int i = 0;
	for (string line; getline(visibility_file, line); )
	{
		vector<int> indexes;
		indexes = split(line, ", ");
		visibility[i] = indexes;
		i++;
	}
	
	return true;
}

vector<vector<int>> TileMap::getTiles() {
	return tiles;
}

unordered_set<int> TileMap::getModels()
{
	for (int i = 0; i < tiles.size(); i++) {
		for (int j = 0; j < tiles[i].size(); j++) {
			models.insert(tiles[i][j]);
		}
	}

	return models;
}

map<int, vector<int>> TileMap::getVisibility()
{
	// "visibility.txt"ASADSF
	return visibility;
}

vector<int> TileMap::split(const string & str, const string & delim) {
	vector<int> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == std::string::npos) pos = str.length();
		std::string token = str.substr(prev, pos - prev);
		if (!token.empty()) tokens.push_back(stoi(token));
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	return tokens;
}
