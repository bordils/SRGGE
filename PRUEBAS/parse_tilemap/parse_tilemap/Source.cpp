#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <random>
#include <cmath>

#define PI 3.14159f

using namespace std;

void main() {
	
	// casillas:
	// structure where we will store the position (i,j)
	// ----00 01 02 03 ... 0n
	// ----10 11 12 13 ... 1n
	// ----20 21 22 23 ... 2n
	// ----30 31 32 33 ... 3n
	//     ...      ...
	//     m0 m1 m2 m3 ... mn

	// each position will store a value that will represent a different mesh
	vector<vector<int>> casillas;

	// file to read
	ifstream stream;

	// file to store visibility after computation
	ofstream output_file;

	stream.open("museum.csv", ios::in);
	if (!stream.is_open()) cout << "ERORR: File  not Open" << endl;

	// read lines and store values
	for (string line; getline(stream, line); )
	{
		replace(line.begin(), line.end(), ',', ' ');
		istringstream in(line);
		casillas.push_back(
			vector<int>(istream_iterator<int>(in),
						istream_iterator<int>()));
	}
	// close reading value
	stream.close();

	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////

	// generates random floats between 0.0 and 1.0
	std::uniform_real_distribution<double> random(0.0, 1.0);
	// generator for the above random distribution. call it: generator(random)
	std::default_random_engine generator; 

	//size of the map
	int colNumber = 32;
	// index = colNumber* j + i;    ///// casillas[i][j]
	int idOrigen, idVisited;

	// structure to store the result of the ray tracing algorithm
	// output.first = colnumber * x + y
	// output.second = colnumber * x + y de las celdas visibles todas ellas en filica
	map<int, unordered_set<int>> visibility;
	map<int, unordered_set<int>>::iterator it0, it1;

	// loop through every cell
	for (int i = 0; i < casillas.size(); i++) {
		for (int j = 0; j < casillas[i].size(); j++) {
			cout << "cell: " << i << ", " << j << endl;

			// index of the departing cell in the grid
			idOrigen = colNumber * i + j;
			
			// create an instance in the map where each cell sees itself
			//unordered_set<int> c;
			//c.insert(idOrigen);
			//visibility.insert(pair<int, unordered_set<int>>(idOrigen, c));
			it0 = visibility.find(idOrigen);
			if (it0 == visibility.end()) {
				unordered_set<int> c;
				c.insert(idOrigen);
				visibility.insert(pair<int, unordered_set<int>>(idOrigen, c));
			}
				


			// check that the cell is not a wall. If it's a wall no ray tracing needed.
			if (casillas[i][j] != 1) {
				// for each cell, cast 300k - 1M rays. For debugging purposes, cast 10k rays.
				for (int ray = 0; ray < 1000000; ray++) {
					// random starting point in the cell (x,y) in [0,1]x[0,1] will always be in the cell
					double x = i + random(generator);
					double y = j + random(generator); 
					// theta == random direction of the ray traced
					double theta = 2 * PI * random(generator);

					double dirX, dirY, tMaxX, tMaxY, stepX, stepY;

					dirX = cos(theta);
					dirY = sin(theta);

					

					// check if they ray cuts first with H or V axes
					//==============================================================================================================

					// (1-x) == distance to the limit of the cell in the H direction
					// (1-y) == distance to the limit of the cell in the v direction
					// ( i + (1 - x)) / cos(theta) rate at which we will reach the limit of the cell in H direction
					// ( j + (1 - y)) / sin(theta) rate at which we will reach the limit of the cell in V direction
					// if cos(theta) = 0, means that it is a V ray, -> it will never move horizontally -> it will never cut a V axis
					// if sin(theta) = 0, means that it is a H ray, -> it will never move vertically -> it will never cut a H axis

					tMaxX = (dirX != 0) ? (i + 1 - x) / dirX : 1000000000.0;
					tMaxY = (dirY != 0) ? (j + 1 - y) / dirY : 1000000000.0;

					// check the sign of theta, because the advance direction would be in the opposite direction and thus ray has to change
					tMaxX = (dirX < 0) ? (x - i) / dirX : tMaxX;
					tMaxY = (dirY < 0) ? (y - j) / dirY : tMaxY;

					stepX = (dirX >= 0) ? 1.0 : -1.0;
					stepY = (dirY >= 0) ? 1.0 : -1.0;

					// stop criteria
					bool loop = true;

					// ray loop
					while (loop) {
						// check if it cuts first with V or H limit
						if (abs(tMaxX) < abs(tMaxY)) {
							// cuts first with V axis, we add +1 to the cell in the H direction
							x += stepX;
							// updating the following cutting distance to compare which limit will cross first H or V in the following step
							tMaxX += 1.0 / dirX;
						}
						else {
							// cuts first with H axis, we add +1 to the cell in the V direction
							y += stepY;
							// updating the following cutting distance to compare which limit will cross first H or V in the following step
							tMaxY += 1.0 / dirY;
						}

						// if it's in the boundaries, we get the index of the cell we are now on
						idVisited = colNumber * floor(y) + floor(x);
						// find the origin cell with the iterator
						it1 = visibility.find(idOrigen);
						// (unordered_set checks if that was already added and doesn't add it if it was already existing)
						it1->second.insert(idVisited);

						if (floor(x) < 0 || floor(y) < 0 || floor(x) > casillas[j].size() || floor(y) > casillas.size() || casillas[floor(x)][floor(y)] == 1)
							loop = false;
					}
				}
			}
		}
	}
	
	// output file where we will add visibility relation. In this case for debugging purposes we get couples of coordinates of origin cells and visible cells.
	// debug error, accesing something out of range with iterator in the second for loop
	std::ofstream out("museum_visibility.txt", ios::out);
	if (out.is_open()) {
		for (int i = 0; i < visibility.size(); i++) {
			it0 = visibility.find(i);
			for (int j : it0->second) {
				out << j <<  ", " ;
			}
			out << endl;
		}
	}
	out.close();
}

