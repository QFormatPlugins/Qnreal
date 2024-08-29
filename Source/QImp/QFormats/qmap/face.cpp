#include "qmap/face.h"
#include <iostream>

namespace qformats::map
{
	static constexpr double CMP_EPSILON_DISTANCE = 0.001;

	void Face::initPlane()
	{
		fvec3 v0v1 = planePoints[1] - planePoints[0];
		fvec3 v1v2 = planePoints[2] - planePoints[1];
		planeNormal = normalize(cross(v1v2, v0v1));
		planeDist = dot(planeNormal, planePoints[0]);
	}

	FacePtr Face::Copy() const
	{
		auto newp = std::make_shared<Face>();
		newp->vertices = vertices;
		newp->indices = indices;
		newp->planeNormal = planeNormal;
		newp->planeDist = planeDist;
		newp->valveUV = valveUV;
		newp->scaleY = scaleY;
		newp->scaleX = scaleY;
		newp->hasValveUV = hasValveUV;
		newp->min = min;
		newp->max = max;
		return newp;
	}

	void Face::UpdateNormals()
	{
		for (int i = 0; i < indices.size(); i += 3)
		{
			// get the three vertices that make the faces
			const auto &p1 = vertices[indices[i + 0]].point;
			const auto &p2 = vertices[indices[i + 1]].point;
			const auto &p3 = vertices[indices[i + 2]].point;

			auto v1 = p2 - p1;
			auto v2 = p3 - p1;
			auto normal = normalize(cross(v1, v2));

			vertices[indices[i + 0]].normal = normal;
			vertices[indices[i + 1]].normal = normal;
			vertices[indices[i + 2]].normal = normal;
		}
	}

	Face::eFaceClassification Face::Classify(const Face *other)
	{
		bool bFront = false, bBack = false;
		for (int i = 0; i < (int)other->vertices.size(); i++)
		{
			double dist = dot(planeNormal, other->vertices[i].point) - planeDist;
			if (dist > CMP_EPSILON_DISTANCE)
			{
				if (bBack)
				{
					return eFaceClassification::SPANNING;
				}

				bFront = true;
			}
			else if (dist < -CMP_EPSILON_DISTANCE)
			{
				if (bFront)
				{
					return eFaceClassification::SPANNING;
				}

				bBack = true;
			}
		}

		if (bFront)
		{
			return eFaceClassification::FRONT;
		}
		else if (bBack)
		{
			return eFaceClassification::BACK;
		}

		return eFaceClassification::ON_PLANE;
	}

	bool Face::getIntersection(const fvec3 &start, const fvec3 &end, fvec3 &out_intersectionPt, float &out_percentage)
	{
		fvec3 dir = normalize(end - start);
		float num, denom;

		denom = dot(planeNormal, dir);

		if (fabs(denom) < epsilon)
		{
			return false;
		}

		float dist = tue::math::dot(planeNormal, start) - planeDist;

		num = -dist;
		out_percentage = num / denom;
		out_intersectionPt = start + (dir * out_percentage);
		out_percentage = out_percentage / tue::math::length(end - start);
		return true;
	}

	Face::eFaceClassification Face::ClassifyPoint(const fvec3 &v)
	{
		double dist = tue::math::dot(planeNormal, v) - planeDist;
		if (dist > epsilon)
		{
			return Face::eFaceClassification::FRONT;
		}
		if (dist < -epsilon)
		{
			return Face::eFaceClassification::BACK;
		}
		return Face::eFaceClassification::ON_PLANE;
	}

	void Face::UpdateAB()
	{
		if (vertices.empty())
			return;

		min = vertices[0].point;
		max = vertices[0].point;

		for (const auto &vert : vertices)
		{
			if (vert.point[0] < min[0])
				min[0] = vert.point[0];

			if (vert.point[1] < min[1])
				min[1] = vert.point[1];

			if (vert.point[2] < min[2])
				min[2] = vert.point[2];

			if (vert.point[0] > max[0])
				max[0] = vert.point[0];

			if (vert.point[1] > max[1])
				max[1] = vert.point[1];

			if (vert.point[2] > max[2])
				max[2] = vert.point[2];
		}
	}

	bool Face::operator==(const Face &other) const
	{
		if (vertices.size() != other.vertices.size() || planeDist != other.planeDist || planeNormal != other.planeNormal)
			return false;

		for (int i = 0; i < vertices.size(); i++)
		{
			if (vertices[i].point != other.vertices[i].point)
				return false;

			if (vertices[i].uv[0] != other.vertices[i].uv[0])
				return false;

			if (vertices[i].uv[1] != other.vertices[i].uv[1])
				return false;
		}

		if (textureID == other.textureID)
			return true;

		return true;
	}

	fvec4 Face::calcStandardTangent()
	{
		float du = dot(planeNormal, UP_VEC);
		float dr = dot(planeNormal, RIGHT_VEC);
		float df = dot(planeNormal, FORWARD_VEC);
		float dua = abs(du);
		float dra = abs(dr);
		float dfa = abs(df);

		fvec3 uAxis{0};
		float vSign = 0.0f;

		if (dua >= dra && dua >= dfa)
		{
			uAxis = FORWARD_VEC;
			vSign = copysignf(1.0, du);
		}
		else if (dra >= dua && dra >= dfa)
		{
			uAxis = FORWARD_VEC;
			vSign = -copysignf(1.0, dr);
		}
		else if (dfa >= dua && dfa >= dra)
		{
			uAxis = RIGHT_VEC;
			vSign = copysignf(1.0, df);
		}

		vSign *= copysignf(1.0, scaleY);
		uAxis = tue::transform::rotation_vec(uAxis, (float)((-rotation * vSign) * (180.0 / M_PI)));
		return fvec4(uAxis[0], uAxis[1], uAxis[2], vSign);
	}

	fvec4 Face::calcValveTangent()
	{
		fvec3 uAxis = normalize(valveUV.u.xyz());
		fvec3 vAxis = normalize(valveUV.v.xyz());
		float vSign = copysignf(1.0, dot(cross((fvec3)planeNormal, uAxis), vAxis));
		return fvec4(uAxis[0], uAxis[1], uAxis[2], vSign);
	}

	fvec2 Face::calcStandardUV(fvec3 vertex, int texW, int texH)
	{
		fvec2 uvOut{0};

		float du = abs(dot(planeNormal, UP_VEC));
		float dr = abs(dot(planeNormal, RIGHT_VEC));
		float df = abs(dot(planeNormal, FORWARD_VEC));

		if (du >= dr && du >= df)
			uvOut = fvec2(vertex[0], -vertex[1]);
		else if (dr >= du && dr >= df)
			uvOut = fvec2(vertex[0], -vertex[2]);
		else if (df >= du && df >= dr)
			uvOut = fvec2(vertex[1], -vertex[2]);

		float angle = rotation * (M_PI / 180);
		uvOut = fvec2(uvOut[0] * cos(angle) - uvOut[1] * sin(angle), uvOut[0] * sin(angle) + uvOut[1] * cos(angle));

		uvOut[0] /= texW;
		uvOut[1] /= texH;

		uvOut[0] /= scaleX;
		uvOut[1] /= scaleY;

		uvOut[0] += standardUV.u / texW;
		uvOut[1] += standardUV.v / texH;
		return uvOut;
	}

	fvec2 Face::calcValveUV(fvec3 vertex, int texW, int texH)
	{
		fvec2 uvOut{0};
		fvec3 uAxis = valveUV.u.xyz();
		fvec3 vAxis = valveUV.v.xyz();
		float uShift = valveUV.u[3];
		float vShift = valveUV.v[3];

		uvOut[0] = dot(uAxis, vertex);
		uvOut[1] = dot(vAxis, vertex);

		uvOut[0] /= texW;
		uvOut[1] /= texH;

		uvOut[0] /= scaleX;
		uvOut[1] /= scaleY;

		uvOut[0] += uShift / texW;
		uvOut[1] += vShift / texH;

		return uvOut;
	}

	std::pair<FacePtr, FacePtr> Face::splitFace(const Face *other)
	{
		std::vector<Face::eFaceClassification> pCF;
		pCF.resize(other->vertices.size());
		for (int i = 0; i < other->vertices.size(); i++)
			pCF[i] = ClassifyPoint(other->vertices[i].point);

		FacePtr front = std::make_shared<Face>();
		FacePtr back = std::make_shared<Face>();

		for (int i = 0; i < other->vertices.size(); i++)
		{
			switch (pCF[i])
			{
			case FRONT:
			{
				front->vertices.push_back(other->vertices[i]);
				break;
			}
			case BACK:
			{
				back->vertices.push_back(other->vertices[i]);
				break;
			}
			case ON_PLANE:
				std::cout << "on plane" << std::endl;
				{
					front->vertices.push_back(other->vertices[i]);
					back->vertices.push_back(other->vertices[i]);
					break;
				}
			case SPANNING:
				break;
			}

			int j = i + 1;
			bool ignore = false;

			if (j == (other->vertices.size() - 1))
				j = 0;

			if ((pCF[i] == Face::eFaceClassification::ON_PLANE) && (pCF[j] != Face::eFaceClassification::ON_PLANE))
			{
				ignore = true;
			}
			else if ((pCF[j] == Face::eFaceClassification::ON_PLANE) && (pCF[i] != Face::eFaceClassification::ON_PLANE))
			{
				ignore = true;
			}

			if ((!ignore) && (pCF[i] != pCF[j]))
			{
				Vertex v{};
				float p;
				getIntersection(other->vertices[i].point, other->vertices[j].point, v.point, p);

				v.uv[0] = other->vertices[j].uv[0] - other->vertices[i].uv[0];
				v.uv[1] = other->vertices[j].uv[1] - other->vertices[i].uv[1];

				v.uv[0] = other->vertices[i].uv[0] + (p * v.uv[0]);
				v.uv[1] = other->vertices[i].uv[1] + (p * v.uv[1]);

				front->vertices.push_back(v);
				back->vertices.push_back(v);
			}
		}
		return {front, back};
	}
}