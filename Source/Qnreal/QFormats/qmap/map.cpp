#include "qmap/map.h"

#include <iostream>

namespace qformats::map
{
	void QMap::LoadFile(const std::string &filename, getTextureBoundsCb getTextureBounds)
	{
		map_file = std::make_shared<QMapFile>();
		map_file->Parse(filename);
		if (getTextureBounds != nullptr)
		{
			for (int i = 0; i < map_file->textures.size(); i++)
			{
				textureIDBounds[i] = getTextureBounds(map_file->textures[i].c_str());
			}
		}
	}

	void QMap::GenerateGeometry(bool clipBrushes)
	{
		for (const auto &se : map_file->solidEntities)
		{
			se->generateMesh(textureIDTypes, textureIDBounds);
			if (clipBrushes)
			{
				se->csgUnion();
			}
		}
	}

	void QMap::SetFaceTypeByTextureID(const std::string &texture, Face::eFaceType type)
	{
		if (map_file == nullptr)
			return;

		for (int i = 0; i < map_file->textures.size(); i++)
		{
			if (map_file->textures[i].find(texture) != std::string::npos)
			{
				this->textureIDTypes[i] = type;
				return;
			}
		}
	}

	std::vector<PointEntityPtr> QMap::GetPointEntitiesByClass(const std::string &className)
	{
		std::vector<PointEntityPtr> ents;
		for (auto pe : map_file->pointEntities)
		{
			if (pe->classname.find(className) != std::string::npos)
			{
				ents.push_back(pe);
			}
		}
		return ents;
	}

	bool QMap::getPolygonsByTextureID(int entityID, int texID, std::vector<FacePtr> &list)
	{
		if (map_file->solidEntities.size() >= entityID || entityID < 0)
		{
			return false;
		}

		for (auto &b : map_file->solidEntities[entityID].get()->brushes)
		{
			for (auto &p : b.GetFaces())
			{
				if (p->textureID == texID)
				{
					list.push_back(p);
				}
			}
		}
		return !list.empty();
	}

	std::vector<FacePtr> QMap::GetPolygonsByTexture(int entityID, const std::string &findName)
	{
		int id = -1;
		for (int i = 0; i < map_file->textures.size(); i++)
		{
			if (map_file->textures[i] == findName)
			{
				id = i;
				break;
			}
		}
		std::vector<FacePtr> polyList;
		if (id == -1)
		{
			return polyList;
		}

		getPolygonsByTextureID(entityID, id, polyList);
		return polyList;
	}

	void QMap::GatherPolygons(int entityID, const polygonGatherCb &cb)
	{
		if (map_file->solidEntities.size() >= entityID || entityID < 0)
		{
			return;
		}

		for (int i = 0; i < map_file->textures.size(); i++)
		{
			std::vector<FacePtr> polyList;
			if (getPolygonsByTextureID(entityID, i, polyList))
			{
				cb(polyList, i);
			}
		}
	}
}
