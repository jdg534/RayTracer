#pragma once

#include "Vec3.h"

#include <string> // may need to replace when going for PS4

class Sphere
{
public:
	Vec3f center;                           /// position of the sphere
	float radius, radius2;                  /// sphere radius and radius^2
	Vec3f surfaceColor, emissionColor;      /// surface color and emission (light)
	float transparency, reflection;         /// surface transparency and reflectivity

	std::string m_id;

	Sphere(
		const Vec3f &c,
		const float &r,
		const Vec3f &sc,
		const float &refl = 0,
		const float &transp = 0,
		const Vec3f &ec = 0) :
		center(c), radius(r), radius2(r * r), surfaceColor(sc), emissionColor(ec),
		transparency(transp), reflection(refl)
	{ /* empty */
		m_id = "noId";
	}
	//[comment]
	// Compute a ray-sphere intersection using the geometric solution
	//[/comment]
	bool intersect(const Vec3f &rayorig, const Vec3f &raydir, float &t0, float &t1) const
	{
		Vec3f l = center - rayorig;
		float tca = l.dot(raydir);
		if (tca < 0) return false;
		float d2 = l.dot(l) - tca * tca;
		if (d2 > radius2) return false;
		float thc = sqrt(radius2 - d2);
		t0 = tca - thc;
		t1 = tca + thc;

		return true;
	}

	bool operator ==(const Sphere & a)
	{
		return m_id == a.m_id;
	}

	void operator =(const Sphere & a)
	{
		center = a.center;
		radius = a.radius;
		radius2 = a.radius2;
		surfaceColor = a.surfaceColor;
		emissionColor = a.emissionColor;
		transparency = a.transparency;
		reflection = a.reflection;
		m_id = a.m_id;
	}

	static bool diffMathOnly(const Sphere  & a, const Sphere & b)
	{
		if (a.center.x != b.center.x
			|| a.center.y != b.center.y
			|| a.center.z != b.center.z)
		{
			return true;
		}
		else if (a.radius != b.radius)
		{
			return true;
		}
		// skip radius2
		else if (a.surfaceColor.x != b.surfaceColor.x
			|| a.surfaceColor.y != b.surfaceColor.y
			|| a.surfaceColor.z != b.surfaceColor.z)
		{
			return true;
		}
		else if (a.emissionColor.x != b.emissionColor.x
			|| a.emissionColor.y != b.emissionColor.y
			|| a.emissionColor.z != b.emissionColor.z)
		{
			return true;
		}
		else if (a.transparency != b.transparency)
		{
			return true;
		}
		else if (a.reflection != b.reflection)
		{
			return true;
		}
		return false;
	}
};