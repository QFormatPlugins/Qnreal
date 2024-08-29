#pragma once

#include "types.h"
#include "face.h"
#include <vector>
#include <memory>

namespace qformats::map
{
	template <class T>
	using boolRet = std::pair<bool, T>;
	class Brush
	{
	public:
		Brush() = default;
		bool DoesIntersect(const Brush &other);
		void buildGeometry(const std::map<int, Face::eFaceType> &faceTypes, const std::map<int, textureBounds> &texBounds);
		void GetBiggerBBox(fvec3 &min, fvec3 &max);
		bool IsBlockVolume() const { return isBlockVolume; }
		[[nodiscard]] inline const std::vector<FacePtr> &GetFaces() const { return faces; }
		fvec3 min{};
		fvec3 max{};

	private:
		std::vector<FacePtr> faces;

		void generatePolygons(const std::map<int, Face::eFaceType> &faceTypes, const std::map<int, textureBounds> &texBounds);
		void windFaceVertices();
		std::vector<FacePtr> clipToBrush(const Brush &other);
		void indexFaceVertices();
		void calculateAABB();
		Vertex mergeDuplicate(int from, Vertex &v);
		boolRet<Vertex> intersectPlanes(const FacePtr &a, const FacePtr &b, const FacePtr &c);
		static bool isLegalVertex(const Vertex &v, const std::vector<FacePtr> &faces);
		FacePtr clipToList(FaceIter first, const FaceIter &firstEnd, FaceIter second, const FaceIter &secondEnd);

		bool isBlockVolume = false;

		friend class QMapFile;
		friend class SolidEntity;
	};
}
