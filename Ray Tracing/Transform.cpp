// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include "Transform.h"
#include "variables.h"

// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis) 
{
    mat3 ret;
    // YOUR CODE FOR HW2 HERE
    // Please implement this.  Likely the same as in HW 1.

	vec3 a = glm::normalize(axis);
	float sine = sin(glm::radians(degrees)), cosine = cos(glm::radians(degrees));
	float x = a.x, y = a.y, z = a.z;

	mat3 aat = mat3(x*x, y*x, z*x,
		x*y, y*y, z*y,
		x*z, y*z, z*z);

	mat3 aStar = mat3(0, -z, y,
		z, 0, -x,
		-y, x, 0);

	ret = glm::mat3(cosine) + (1 - cosine) * aat + sine * aStar;

    return ret;
}

void Transform::left(float degrees, vec3& eye, vec3& up) 
{
    // YOUR CODE FOR HW2 HERE
    // Likely the same as in HW 1.  
	mat3 rotationMatrix = Transform::rotate(degrees, up);
	eye = eye * rotationMatrix;
}

void Transform::up(float degrees, vec3& eye, vec3& up) 
{
    // YOUR CODE FOR HW2 HERE 
    // Likely the same as in HW 1. 
	mat3 rotationMatrix = Transform::rotate(degrees, glm::cross(eye, up));
	eye = eye * rotationMatrix;
	up = up * rotationMatrix;
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{
    // YOUR CODE FOR HW2 HERE
    // Likely the same as in HW 1. 

	vec3 w = glm::normalize(eye-center);
	vec3 u = glm::normalize(glm::cross(up, w));
	vec3 v = glm::cross(w, u);
	mat4 r = mat4(u.x, u.y, u.z, 0,
		v.x, v.y, v.z, 0,
		w.x, w.y, w.z, 0,
		0, 0, 0, 1.0f);

	mat4 t = mat4(1, 0, 0, -glm::dot(u, eye),
		0, 1, 0, -glm::dot(v, eye),
		0, 0, 1, -glm::dot(w, eye),
		0, 0, 0, 1.0f);

	return glm::transpose(r*t);
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
    mat4 ret;
    // YOUR CODE FOR HW2 HERE
    // New, to implement the perspective transform as well.  
	float theta = fovy / 2;
	float d = 1/tan(glm::radians(theta));
	float A = -(zFar + zNear) / (zFar - zNear);
	float B = -2 * zFar*zNear / (zFar - zNear);
	ret = mat4(
		d/aspect, 0, 0, 0,
		0, d, 0, 0,
		0, 0, A, B,
		0, 0, -1, 0
		);
    return glm::transpose(ret);
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
    mat4 ret;
    // YOUR CODE FOR HW2 HERE
    // Implement scaling 
	ret[0] = vec4(sx, 0, 0, 0);
	ret[1] = vec4(0, sy, 0, 0);
	ret[2] = vec4(0, 0, sz, 0);
	ret[3] = vec4(0, 0, 0, 1);
    return ret;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
    mat4 ret;
    // YOUR CODE FOR HW2 HERE
    // Implement translation 
	ret[0] = vec4(1, 0, 0, tx);
	ret[1] = vec4(0, 1, 0, ty);
	ret[2] = vec4(0, 0, 1, tz);
	ret[3] = vec4(0, 0, 0, 1);
    return ret;
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec)
{
	vec3 x = cross(zvec, up);
	vec3 y = cross(zvec, x);
	vec3 ret = normalize(y);
    return ret; 
}

void Transform::computeAxis(const vec3 &eye, const vec3 &center, const vec3 &up) {

	w = normalize(eye - center);
	u = normalize(cross(w, up));
	v = cross(w, u);
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
