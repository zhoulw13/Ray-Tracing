#include "raytrace.h"
#include "variables.h"
#include <FreeImage.h>
#include <time.h>

const float eps = 1e-6;
const float eps2 = 1e-3;
int sgn(float x) { return x > eps ? 1 : x < -eps ? 1 : 0; };

void rayTrace() {
	FreeImage_Initialise();
	int pix = width * height;
	BYTE *pixels = new BYTE[3 * pix];

	hitinfo info;
	#pragma omp parallel for
	for (int i = 0; i < height; i++) {
		#pragma omp parallel for
		for (int j = 0; j < width; j++) {
			vec3 ray = rayThruPixel(float(i)+0.5, float(j)+0.5);
			vec3 hit = intersection(eye, ray, info);
			vec3 color = findColor(ray, hit, info, 1);
			pixels[3 * (i*width + j)] = min(color[2], 1.0f) * 255;
			pixels[3 * (i*width + j) + 1] = min(color[1], 1.0f) * 255;
			pixels[3 * (i*width + j) + 2] = min(color[0], 1.0f) * 255;
		}
		printf("%.2f\r", float(i) / height);
	}
	FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
	FreeImage_Save(FIF_PNG, img, output, 0);
	FreeImage_DeInitialise();
} 

vec3 rayThruPixel(float i, float j) {
	float alpha = tan(radians(fovy / 2)) * (j - (width / 2)) / (width / 2) * width / height;
	float beta = tan(radians(fovy / 2)) * (height / 2 - i) / (height / 2);
	vec3 ret = normalize(alpha*u+beta*v-w);
	return (ret);
}

vec3 intersection(vec3 origin, vec3 ray, hitinfo& info) {

	float min_dist = INT_MAX;

	int index = 0;
	int obj = 0;
	hitinfo t_info;

	int s = tris.size();
	for (int i = 0; i < s; i++) {
		float t = intersect(origin, ray, e_triangle, i, t_info);
		if (t > 0 && t < min_dist) {
			min_dist = t;
			index = i;
			obj = e_triangle;
			info = t_info;
		}
	}

	s = spheres.size();
	for (int i = 0; i < s; i++) {
		float t = intersect(origin, ray, e_sphere, i, t_info);
		if (t > 0 && t < min_dist) {
			min_dist = t;
			index = i;
			obj = e_sphere;
			info = t_info;
		}
	}

	return vec3(min_dist, obj, index);
}

float intersect(vec3 origin,vec3 ray, shape obj, int i, hitinfo& info) {
	float re = INT_MAX;

	mat4 trans;
	if (obj == e_triangle) {
		trans = tris[i].transform;
	}
	else if (obj == e_sphere) {
		trans = spheres[i].transform;
	}
	else {
		cerr << "NOT SUPPORTED SHAPE\n";
	}
	//apply tranform
	mat4 inv = inverse(trans);
	vec4 t0 = vec4(origin, 1.0);
	vec4 t1 = vec4(ray, 0.0);
	vec3 p0 = vec3(inv*t0);
	vec3 p1 = vec3(inv*t1);

	//intersect
	if (obj == e_triangle) {


		tri t = tris[i];
		vec3 A = vertexs[t.v1];
		vec3 B = vertexs[t.v2];
		vec3 C = vertexs[t.v3];

		vec3 n = normalize(cross(C - A, B - A));
		if (dot(n, p1) > 0) {
			info.normal = -n;
		}
		else {
			info.normal = n;
		}

		if (sgn(dot(p1, n)) == 0) {
			return INT_MAX;
		}

		vec3 E1 = A - B;
		vec3 E2 = A - C;
		vec3 S = A - p0;
		float x = mydet(p1, E1, E2);
		float beta = mydet(p1, S, E2)/x;
		float gamma = mydet(p1, E1, S)/x;
	
		// check validation
		if (beta < 0 || beta > 1 || gamma < 0 || gamma > 1 || (beta + gamma) > 1) {
			return INT_MAX;
		}
		else {
			re = mydet(S, E1, E2) / x;
			info.hitpoint = p0 + re*p1;
		}
	}
	else if (obj == e_sphere) {
		sphere s = spheres[i];
		vec3 C = vec3(s.x, s.y, s.z);
		float a = dot(p1, p1);
		float b = 2 * dot(p0 - C, p1);
		float c = dot(p0 - C, p0 - C) - pow(s.r, 2);
		float delta = pow(b, 2) - 4*a*c;
		if (delta > 0) {
			if (a*c > 0) {
				re = (-b - sqrt(delta)) / (2 * a);
			}
			else {
				re = (-b + sqrt(delta)) / (2 * a);
			}
			info.hitpoint = p0 + re*p1;
			info.normal = normalize(info.hitpoint - C);
		}
		else {
			re = INT_MAX;
		}

	}
	else {
		cerr << "NOT SUPPORTED SHAPE\n";
	}

	return re;
}

vec3 findColor(vec3 ray, vec3 hit, hitinfo info, int depth){
	if (hit[0] == INT_MAX || depth > maxdepth) {
		return vec3(0.0);
	}

	vec3 color;
	vec3 o_specular;
	vec3 o_diffuse;
	mat4 trans;
	if (hit[1] == e_triangle) {
		tri obj = tris[hit[2]];
		color = obj.ambient + obj.emission;
		o_specular = obj.specular;
		o_diffuse = obj.diffuse;
		trans = obj.transform;
	}
	else {
		sphere obj = spheres[hit[2]];
		color = obj.ambient + obj.emission;
		o_specular = obj.specular;
		o_diffuse = obj.diffuse;
		trans = obj.transform;
	}
	//transform hitpoint;
	vec3 real_hitpot = vec3(trans*vec4(info.hitpoint, 1.0)); 
	vec3 eyedir = -ray;
	vec3 hire_normal = normalize(vec3(transpose(inverse(trans))*vec4(info.normal, 0.0)));

	//shadow
	if (point) {
		vec3 light = normalize(pointpos - real_hitpot);
		vec3 t_hitpot = real_hitpot + light * eps2;
		hitinfo recur_info;
		vec3 shadow = intersection(t_hitpot, light, recur_info);

		//distance
		float r = length(pointpos - real_hitpot);
		if (shadow[0] == INT_MAX || r < shadow[0]) {
			vec3 halfvec = normalize(eyedir + light);

			float dDot = max(dot(light, hire_normal), float(0.0));
			float hDot = pow(max(dot(hire_normal, halfvec), float(0.0)), shininess);

			float atten_const = attenuation[0] + attenuation[1] * r + attenuation[2] * pow(r, 2);
			color += o_diffuse * pointcolor * dDot / atten_const;
			color += o_specular * pointcolor * hDot / atten_const;
		}
	}

	if (direction) {
		vec3 light = normalize(directionalpos);
		vec3 t_hitpot = real_hitpot + light * eps2;
		hitinfo recur_info;
		vec3 shadow = intersection(t_hitpot, light, recur_info);
		if (shadow[0] == INT_MAX) {
			vec3 halfvec = normalize(eyedir + light);

			float dDot = max(dot(light, hire_normal), float(0.0));
			float hDot = pow(max(dot(hire_normal, halfvec), float(0.0)), shininess);

			color += o_diffuse * directionalcolor * dDot;
			color += o_specular * directionalcolor * hDot;
		}
	}

	//recursive ray tracing
	vec3 ray2 = ray - 2 * dot(ray, hire_normal) * hire_normal;
	vec3 origin2 = real_hitpot + ray2 * eps2;
	hitinfo info2;
	vec3 hit2 = intersection(origin2, ray2, info2);
	color += o_specular * findColor(ray2, hit2, info2, depth+1);

	return color;
}

float mydet(vec3 a, vec3 b, vec3 c) {
	mat3 x = mat3(a, b, c);
	return determinant(x);
}