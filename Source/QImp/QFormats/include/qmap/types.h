#pragma once

#include <tue/transform.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <array>
#include <map>
#include <tue/vec.hpp>

using tue::fvec2;
using tue::fvec3;
using tue::fvec4;

using tue::math::cross;
using tue::math::dot;
using tue::math::normalize;

static std::ostream &operator<<(std::ostream &stream, const fvec3 &v)
{
	stream << "(" << v[0] << " " << v[1] << " " << v[2] << ")";
	return stream;
}

inline float dist3(const fvec3 &a, const fvec3 &b)
{
	fvec3 diff = b - a;
	return sqrtf(dot(diff, diff));
};

namespace qformats::map
{
	const double epsilon = 1e-5; // Used to compensate for floating point inaccuracy.
	const double scale = 128;	 // Scale
								 // MAP FILE

	struct StandardUV
	{
		float u;
		float v;
	};
	struct ValveUV
	{
		fvec4 u;
		fvec4 v;
	};

	// GEOMETRY
	struct Vertex
	{
		fvec3 point;
		fvec3 normal;
		fvec2 uv;
		fvec4 tangent;

		friend std::ostream &operator<<(std::ostream &stream, const Vertex &v)
		{
			stream << "(" << v.point[0] << " " << v.point[1] << " " << v.point[2] << ")";
			return stream;
		}

		inline bool inList(const std::vector<Vertex> &list)
		{
			for (auto const &v : list)
			{
				if (v.point == point)
					return true;
			}
			return false;
		}
	};

	struct textureBounds
	{
		float width;
		float height;
	};
}
