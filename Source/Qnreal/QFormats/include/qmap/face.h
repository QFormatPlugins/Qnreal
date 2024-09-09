#pragma once

#include "types.h"
#include "qmath.h"
#include <memory>
#include <tuple>

namespace qformats::map
{
	class Face;
	using FacePtr = std::shared_ptr<Face>;
	using FaceIter = std::vector<FacePtr>::const_iterator;

	class Face
	{
	public:
		enum eFaceClassification
		{
			FRONT = 0,
			BACK,
			ON_PLANE,
			SPANNING,
		};

		enum eFaceType
		{
			SOLID = 0,
			CLIP,
			SKIP,
			NODRAW
		};

	public:
		Face() = default;

		Face(const std::array<fvec3, 3>& points, int textureID, StandardUV uv, float rotation, float scaleX,
		     float scaleY)
			: planePoints(points), standardUV(uv), textureID(textureID), rotation(rotation), scaleX(scaleX),
			  scaleY(scaleY)
		{
			initPlane();
		};

		Face(const std::array<fvec3, 3>& points, int textureID, ValveUV uv, float rotation, float scaleX,
		     float scaleY)
			: planePoints(points), valveUV(uv), textureID(textureID), rotation(rotation), scaleX(scaleX),
			  scaleY(scaleY), hasValveUV(true)
		{
			initPlane();
		};
		Face::eFaceClassification Classify(const Face* other);
		Face::eFaceClassification ClassifyPoint(const fvec3& v);
		void UpdateAB();
		void UpdateNormals();
		[[nodiscard]] FacePtr Copy() const;

		[[nodiscard]] int TextureID() const
		{
			return textureID;
		};

		const fvec3& GetPlaneNormal() const { return planeNormal; }
		const float& GetPlaneDist() const { return planeDist; }
		eFaceType Type() const { return type; }
		const std::vector<Vertex>& GetVertices() { return vertices; }
		const std::vector<unsigned short>& GetIndices() { return indices; }

		fvec3 center{}, min{}, max{};
		bool operator==(const Face& arg_) const;

	private:
		fvec4 CalcTangent()
		{
			return hasValveUV ? calcValveTangent() : calcStandardTangent();
		};

		fvec2 CalcUV(fvec3 vertex, int texW, int texH)
		{
			return hasValveUV ? calcValveUV(vertex, texW, texH) : calcStandardUV(vertex, texW, texH);
		};

		void initPlane();
		fvec4 calcStandardTangent();
		fvec4 calcValveTangent();
		fvec2 calcStandardUV(fvec3 vertex, int texW, int texH);
		fvec2 calcValveUV(fvec3 vertex, int texW, int texH);
		bool getIntersection(const fvec3& start, const fvec3& end, fvec3& out_intersectionPt, float& out_percentage);
		std::pair<FacePtr, FacePtr> splitFace(const Face* other);

		std::array<fvec3, 3> planePoints{};
		std::vector<Vertex> vertices;
		std::vector<unsigned short> indices;
		fvec3 planeNormal{};
		float planeDist{};
		StandardUV standardUV{};
		ValveUV valveUV{};
		int textureID{};
		float rotation{};
		float scaleX{};
		float scaleY{};
		eFaceType type = SOLID;
		bool hasValveUV{};

		friend class Brush;
		friend class QMap;
	};
}
