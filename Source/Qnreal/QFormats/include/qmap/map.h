//
// Created by tgoeh on 10.11.2023.
//
#pragma once

#include <string>
#include <vector>
#include <functional>

#include "types.h"
#include "map_file.h"
#include "brush.h"
#include "entities.h"

namespace qformats::map
{
	using polygonGatherCb = std::function<void(std::vector<FacePtr>, int)>;
	using getTextureBoundsCb = std::function<textureBounds(const char *textureName)>;

	class QMap
	{
	public:
		QMap() = default;
		~QMap() = default;

		void LoadFile(const std::string &filename, getTextureBoundsCb getTextureBounds = nullptr);
		void GenerateGeometry(bool clipBrushes = true);
		void GatherPolygons(int entityID, const polygonGatherCb &);

		std::vector<FacePtr> GetPolygonsByTexture(int entityID, const std::string &texName);
		const std::vector<std::string> &Wads() { return map_file->wads; };
		bool HasWads() { return !map_file->wads.empty(); };

		const std::vector<std::string> &GetTexturesNames() { return map_file->textures; };
		QMapFile *MapData() { return map_file.get(); };
		void SetFaceTypeByTextureID(const std::string &texture, Face::eFaceType type);

		const SolidEntity *WorldSpawn() { return map_file->worldSpawn; }
		const std::vector<SolidEntityPtr> &GetSolidEntities() { return map_file->solidEntities; };
		const std::vector<PointEntityPtr> &GetPointEntities() { return map_file->pointEntities; };
		std::vector<PointEntityPtr> GetPointEntitiesByClass(const std::string &className);

	private:
		bool getPolygonsByTextureID(int entityID, int texID, std::vector<FacePtr> &list);

		std::map<int, Face::eFaceType> textureIDTypes;
		std::map<int, textureBounds> textureIDBounds;
		std::shared_ptr<QMapFile> map_file;
	};
}
