#include "qmap/entities.h"

namespace qformats::map
{

	void SolidEntity::generateMesh(const std::map<int, Face::eFaceType> &faceTypes, const std::map<int, textureBounds> &texBounds)
	{
		if (!brushes.empty())
		{
			min = brushes[0].min;
			max = brushes[0].max;
		}
		for (auto &b : brushes)
		{
			b.buildGeometry(faceTypes, texBounds);
			b.GetBiggerBBox(min, max);
		}
		center = CalculateCenterFromBBox(min, max);
	}

	void SolidEntity::csgUnion()
	{
		if (!brushes.empty())
		{
			min = brushes[0].min;
			max = brushes[0].max;
		}
		for (auto &b1 : brushes)
		{
			auto cpBrush = b1;
			for (auto &b2 : brushes)
			{
				if (&b1 == &b2 || b2.faces.empty())
				{
					continue;
				}

				if (!b1.DoesIntersect(b2) || (b1.IsBlockVolume() || b2.IsBlockVolume()))
				{
					continue;
				}

				auto clippedFaces = cpBrush.clipToBrush(b2);
				cpBrush.faces = clippedFaces;
			}
			if (!cpBrush.faces.empty())
			{
				clippedBrushes.push_back(cpBrush);
				cpBrush.GetBiggerBBox(min, max);
				stats_clippedFaces += b1.faces.size() - cpBrush.faces.size();
			}
		}
		center = CalculateCenterFromBBox(min, max);
		if (brushes.size() > 0 && clippedBrushes.size() > 0)
		{
			wasClipped = true;
		}
	}
}