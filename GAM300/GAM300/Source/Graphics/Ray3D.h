/*!***************************************************************************************
\file			Ray3D.h
\project
\author         Sean Ngo

\par			Course: GAM300
\date           15/10/2023

\brief
    This file contains the declarations of the following:
    1.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef RAY3D_H
#define RAY3D_H

#include <glm/glm.hpp>

class Ray3D
{
public:

	struct PlaneParams
	{
		const glm::vec3& delta;
		const glm::vec3& min;
		const glm::vec3& max;
		float& tMin;
		float& tMax;
	};

	// Constructor
	Ray3D() : origin{}, direction{} {}
	Ray3D(const glm::vec3& _origin, const glm::vec3& _direction) : origin{ _origin }, direction{ _direction } {}

	glm::vec3 Lerp(const float& t) const { return(origin + t * direction); }

	// Checks if ray intersects object
	bool TestRayOBB(
		const glm::mat4& _modelMatrix, // Transformation applied to the mesh (which will thus be also applied to its bounding box)
		const glm::vec3& _min,	// Minimum X,Y,Z coords of the mesh when not transformed at all.
		const glm::vec3& _max,	// Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
		float& _intDistance)	// Output : distance between ray_origin and the intersection with the OBB)
	{
		// Intersection method from Real-Time Rendering and Essential Mathematics for Games

		direction = glm::normalize(direction);

		float tMin = 0.0f;
		float tMax = 100000.0f;

		glm::vec3 oBBPositionWorldSpace(_modelMatrix[3].x, _modelMatrix[3].y, _modelMatrix[3].z);

		glm::vec3 delta = oBBPositionWorldSpace - origin;

		PlaneParams plane{ delta, _min, _max, tMin, tMax };

		// Test intersection with the 2 planes perpendicular to the OBB's X axis
		if (!IBPlanes(_modelMatrix, plane, 0))
			return false;

		// Test intersection with the 2 planes perpendicular to the OBB's Y axis
		if (!IBPlanes(_modelMatrix, plane, 1))
			return false;

		// Test intersection with the 2 planes perpendicular to the OBB's Z axis
		if (!IBPlanes(_modelMatrix, plane, 2))
			return false;

		_intDistance = tMin;
		return true;
	}

	glm::vec3 origin;
	glm::vec3 direction;

private:
	// Intersection between planes based on objects axis
	bool IBPlanes(const glm::mat4& _modelMatrix, PlaneParams& _plane, const int& _i)
	{
		glm::vec3 zaxis(_modelMatrix[_i].x, _modelMatrix[_i].y, _modelMatrix[_i].z);
		float e = glm::dot(zaxis, _plane.delta);
		float f = glm::dot(direction, zaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + _plane.min[_i]) / f; // Intersection with the "left" plane
			float t2 = (e + _plane.max[_i]) / f; // Intersection with the "right" plane
			// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

			// We want t1 to represent the nearest intersection, 
			// so if it's not the case, invert t1 and t2
			if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < _plane.tMax) // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
				_plane.tMax = t2;
			if (t1 > _plane.tMin) // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
				_plane.tMin = t1;

			// And here's the trick :
			// If "far" is closer than "near", then there is NO intersection.
			// See the images in the tutorials for the visual explanation.
			if (_plane.tMin > _plane.tMax)
				return false;

		}
		else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + _plane.min[_i] > 0.0f || -e + _plane.max[_i] < 0.0f)
				return false;
		}

		return true;
	}
};

#endif // !RAY3D_H
