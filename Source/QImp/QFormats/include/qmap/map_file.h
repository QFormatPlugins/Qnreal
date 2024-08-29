#pragma once
#include <string>
#include <map>
#include <array>
#include <vector>
#include "types.h"
#include "brush.h"
#include "entities.h"

namespace qformats::map
{
	const int STANDARD_VERSION = 100;
	const int VALVE_VERSION = 220;

	class QMapFile
	{
	public:
		QMapFile()
		{
			worldSpawn = nullptr;
		};
		void Parse(const std::string& filename);
		const std::string& VersionString()
		{
			return mapVersionStr;
		};
		int Version()
		{
			return mapVersion;
		};

	private:
		void parse_entity_attributes(std::string l, BaseEntity* ent);
		void parse_entity_planes(std::stringstream& lines, SolidEntity* ent);
		void parse_wad_string(const std::string& wads);

	private:
		size_t getOrAddTexture(const std::string& texture);

		int mapVersion = STANDARD_VERSION;
		std::string mapVersionStr = "100";
		SolidEntity* worldSpawn;
		std::vector<SolidEntityPtr> solidEntities;
		std::vector<PointEntityPtr> pointEntities;
		std::vector<std::string> textures;
		std::vector<std::string> wads;
		friend class QMap;
	};
}
