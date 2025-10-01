#ifndef SphereSelectionTool_H
#define SphereSelectionTool_H
#include "Vec3.h"


struct SphereSelectionTool
{
	float radius;
	Vec3 center;
	bool isAdding;
	bool isActive;

	SphereSelectionTool() : radius(1.0), center(0.0, 0.0, 0.0), isAdding(false), isActive(false) {}


	void initSphere(const Vec3& pCenter, const float &pRadius)
	{
		// init sphere with Vec3 center and radius
	}

	void updateSphere(float pRadius)
	{
		// update radius 
	}

	bool contains (const Vec3& p)
	{
		// is point p in sphere (center_x, center_y, center_z), radius) ?
		return false;
	}


	void draw() {
	    if(!isActive) return;
	    // draw Sphere
	}
};
#endif