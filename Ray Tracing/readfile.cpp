#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

#include "variables.h"
#include "readfile.h"
#include "Transform.h"

void rightmultiply(const mat4 & M, stack<mat4> &transfstack)
{
	mat4 &T = transfstack.top();
	T = T * M;
}

bool readvals(stringstream &s, const int numvals, float* values)
{
	for (int i = 0; i < numvals; i++) {
		s >> values[i];
		if (s.fail()) {
			cout << "Failed reading value " << i << " will skip\n";
			return false;
		}
	}
	return true;
}

void readfile(const char* filename) {
	string str, cmd;
	ifstream in;
	in.open(filename);
	int vertex_count = 0, vertexnormal_count = 0;
	if (in.is_open()) {
		stack <mat4> transfstack;
		transfstack.push(mat4(1.0)); // identity

		getline(in, str);
		while (in) {
			if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
				// Ruled out comment and blank lines 
				stringstream s(str);
				s >> cmd;
				int i;
				float values[10]; // Position and color for light, colors for others
				// Up to 10 params for cameras.  
				bool validinput; // Validity of input 

				//size command
				if (cmd == "size") {
					validinput = readvals(s, 2, values);
					if (validinput) {
						width = (int)values[0];
						height = (int)values[1];
					}
				}

				//camera command
				else if (cmd == "camera") {
					validinput = readvals(s, 10, values); // 10 values eye cen up fov
					if (validinput) {

						eye = vec3(values[0], values[1], values[2]);
						center = vec3(values[3], values[4], values[5]);
						up = vec3(values[6], values[7], values[8]);

						up = Transform::upvector(up, eye-center);

						Transform::computeAxis(eye, center, up);

						fovy = values[9];

					}
				}

				else if (cmd == "maxdepth") {
					validinput = readvals(s, 1, values);
					if (validinput) {

						maxdepth = values[0];
					}
				}

				else if (cmd == "output") {
					s >> output;
				}

				else if (cmd == "maxverts") {
					validinput = readvals(s, 1, values);
					if (validinput) {
						maxverts = values[0];
						vertexs = new vertex[maxverts];
					}
				}

				else if (cmd == "vertex") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						vertexs[vertex_count++] = vertex( values[0], values[1] , values[2] );
					}
				}

				else if (cmd == "maxvertnorms") {
					validinput = readvals(s, 1, values);
					if (validinput) {
						maxvertnorms = values[0];
						vertexnormals = new vertexnormal[maxvertnorms];
					}
				}

				else if (cmd == "vertexnormal") {
					validinput = readvals(s, 6, values);
					if (validinput) {
						vertexnormals[vertexnormal_count++] = vertexnormal{ values[0], values[1], values[2], values[3], values[4], values[5] };
					}
				}

				else if (cmd == "tri") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						tris.push_back(tri{ values[0], values[1], values[2], ambient, diffuse, emission, specular, transfstack.top() });
					}
				}

				else if (cmd == "trinormal") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						trinormals.push_back(trinormal{ values[0], values[1], values[2] });
					}
				}

				else if (cmd == "sphere") {
					validinput = readvals(s, 4, values);
					if (validinput) {
						spheres.push_back(sphere{ values[0], values[1], values[2], values[3], ambient, diffuse, emission, specular, transfstack.top() });
					}
				}

				else if (cmd == "directional") {
					validinput = readvals(s, 6, values);
					if (validinput) {
						direction = true;
						for (int i = 0; i < 3; i++) {
							directionalpos[i] = values[i];
							directionalcolor[i] = values[3 + i];
						}
					}
				}

				else if (cmd == "point") {
					validinput = readvals(s, 6, values);
					if (validinput) {
						point = true;
						for (int i = 0; i < 3; i++) {
							pointpos[i] = values[i];
							pointcolor[i] = values[3 + i];
						}
					}
				}

				else if (cmd == "attenuation") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						for (int i = 0; i < 3; i++) {
							attenuation[i] = values[i];
						}
					}
				}

				else if (cmd == "ambient") {
					validinput = readvals(s, 3, values); // colors 
					if (validinput) {
						for (i = 0; i < 3; i++) {
							ambient[i] = values[i];
						}
					}
				}

				else if (cmd == "diffuse") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						for (i = 0; i < 3; i++) {
							diffuse[i] = values[i];
						}
					}
				}

				else if (cmd == "specular") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						for (i = 0; i < 3; i++) {
							specular[i] = values[i];
						}
					}
				}

				else if (cmd == "emission") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						for (i = 0; i < 3; i++) {
							emission[i] = values[i];
						}
					}
				}

				else if (cmd == "shininess") {
					validinput = readvals(s, 1, values);
					if (validinput) {
						shininess = values[0];
					}
				}

				else if (cmd == "pushTransform") {
					transfstack.push(transfstack.top());
				}

				else if (cmd == "popTransform") {
					if (transfstack.size() <= 1) {
						cerr << "Stack has no elements. Cannot Pop\n";
					}
					else {
						transfstack.pop();
					}
				}

				//matrix transform
				else if (cmd == "translate") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						mat4 t = transpose(Transform::translate((float)values[0], (float)values[1], (float)values[2]));
						rightmultiply(t, transfstack);
					}
				}
				else if (cmd == "scale") {
					validinput = readvals(s, 3, values);
					if (validinput) {

						mat4 s = transpose(Transform::scale((float)values[0], (float)values[1], (float)values[2]));
						rightmultiply(s, transfstack);
					}
				}

				else if (cmd == "rotate") {
					validinput = readvals(s, 4, values);
					if (validinput) {
						vec3 axis = normalize(vec3(values[0], values[1], values[2]));
						mat4 r = transpose(glm::mat4(Transform::rotate(values[3], axis)));
						rightmultiply(r, transfstack);

					}
				}
				/*else if (cmd == "") {
					validinput = readvals(s, 1, values);
					if (validinput) {

					}
				}*/

			}
			getline(in, str);
		}

	}
	else {
		cerr << "Unable to Open Input Data File " << filename << "\n";
		throw 2;
	}
}