#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

using namespace std;
using namespace glm;

#ifdef MAINPROGRAM 
#define EXTERN 
#else 
#define EXTERN extern 
#endif 

#ifdef MAINPROGRAM
int maxdepth = 5;
char output[100] = "test.png";
vec3 eye(0.0, 0.0, 5.0); // Initial eye position, also for resets
vec3 up(0.0, 1.0, 0.0); // Initial up position, also for resets
vec3 center(0.0, 0.0, 0.0); // Center look at point
float fovy =  90.0;
float attenuation[3] = {1, 0, 0};
vec3 ambient = vec3(0.2, 0.2, 0.2);
bool direction = false;
bool point = false;
#else
EXTERN char output[100];
EXTERN vec3 eye;
EXTERN vec3 up;
EXTERN vec3 center;
EXTERN float fovy;
EXTERN int maxdepth;
EXTERN float attenuation[3];
EXTERN vec3 ambient;
EXTERN bool direction;
EXTERN bool point;
#endif


EXTERN int width;
EXTERN int height;
EXTERN vec3 u, v, w;

EXTERN int maxverts;
typedef vec3 vertex;
/*EXTERN struct vertex {
	float x, y, z;
};*/
EXTERN vertex* vertexs;

EXTERN int maxvertnorms;
EXTERN struct vertexnormal {
	float x, y, z;
	float nx, ny, nz;
};
EXTERN vertexnormal* vertexnormals;

EXTERN struct tri {
	int v1, v2, v3;
	vec3 ambient;
	vec3 diffuse;
	vec3 emission;
	vec3 specular;
	mat4 transform;
};

EXTERN vector<tri> tris;

EXTERN struct trinormal {
	int v1, v2, v3;
};

EXTERN vector<trinormal> trinormals;

EXTERN struct sphere {
	float x, y, z, r;
	vec3 ambient;
	vec3 diffuse;
	vec3 emission;
	vec3 specular;
	mat4 transform;
};

EXTERN vector<sphere> spheres;


EXTERN vec3 directionalpos;
EXTERN vec3 directionalcolor;
EXTERN vec3 pointpos;
EXTERN vec3 pointcolor;

EXTERN vec3 diffuse;
EXTERN vec3 specular;
EXTERN vec3 emission;
EXTERN float shininess;

