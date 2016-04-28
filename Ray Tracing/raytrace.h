#ifndef RAYTRACE
#define RAYTRACE
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;


enum shape {e_triangle, e_sphere};

struct hitinfo {
	vec3 normal;
	vec3 hitpoint;
};


void rayTrace();
vec3 rayThruPixel(float i, float j);
vec3 intersection(vec3 origin, vec3 ray, hitinfo& info);
float intersect(vec3 origin, vec3 ray, shape obj, int i, hitinfo& info);
vec3 findColor(vec3 ray, vec3 hit, hitinfo info, int depth);
float mydet(vec3 a, vec3 b, vec3 c);