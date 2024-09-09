#include "qmap/map_file.h"

#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <regex>

namespace qformats::map
{
	static std::vector<std::string> rexec_vec(std::string line, const std::string &regexstr)
	{
		std::stringstream results;
		std::regex re(regexstr, std::regex::icase);

		std::vector<std::string> matches;
		const std::sregex_token_iterator end;
		for (std::sregex_token_iterator it(line.begin(), line.end(), re, 1); it != end; it++)
		{
			matches.push_back(*it);
		}

		return matches;
	}

	inline std::string &ltrim(std::string &s, const char *t = " ")
	{
		s.erase(0, s.find_first_not_of(t));
		return s;
	}

	void QMapFile::Parse(const std::string &filename)
	{
		struct parsedObject
		{
			std::stringstream lines;
			parsedObject *parent = nullptr;
			std::vector<parsedObject *> children;
		};

		auto strstr = std::fstream(filename);
		std::vector<parsedObject *> objects;
		parsedObject *current = nullptr;
		for (std::string line; std::getline(strstr, line);)
		{
			std::erase(line, '\r');
			if (line.empty())
			{
				continue;
			}
			line = ltrim(line);

			// HACK: we better analyze the face UV's to figure out format.
			if (line == "// Format: Valve")
			{
				mapVersion = VALVE_VERSION;
			}
			if (line.starts_with("//"))
			{
				continue;
			}
			if (line == "{")
			{
				parsedObject *newobj = new parsedObject;
				if (current == nullptr)
				{
					objects.push_back(newobj);
					current = newobj;
				}
				else
				{
					newobj->parent = current;
					current->children.push_back(newobj);
					current = newobj;
				}
				continue;
			}
			if (line == "}")
			{
				if (current != nullptr)
				{
					current = current->parent;
				}
				continue;
			}
			if (current != nullptr)
			{
				current->lines << line << std::endl;
			}
		}

		for (auto *obj : objects)
		{
			BaseEntity *ent = nullptr;
			if (!obj->children.empty())
			{
				auto sEnt = std::make_shared<SolidEntity>();
				solidEntities.push_back(sEnt);
				ent = static_cast<BaseEntity *>(sEnt.get());
			}
			else
			{
				auto pEnt = std::make_shared<PointEntity>();
				pointEntities.push_back(pEnt);
				ent = static_cast<BaseEntity *>(pEnt.get());
			}
			for (std::string line; std::getline(obj->lines, line);)
			{
				parse_entity_attributes(line, ent);
			}
			for (auto *subObj : obj->children)
			{
				parse_entity_planes(subObj->lines, reinterpret_cast<SolidEntity *>(ent));
			}
		}

		// TODO: use shared_ptr + weak_ptr
		for (auto obj : objects)
		{
			for (auto child : obj->children)
			{
				delete child;
			}
			delete obj;
		}

		strstr.close();
	}

	void QMapFile::parse_wad_string(const std::string &wstr)
	{
		std::istringstream ss(wstr);

		for (std::string item; std::getline(ss, item, ';');)
		{
			wads.push_back(item.substr(item.find_last_of("/\\") + 1));
		}
	}

	void QMapFile::parse_entity_attributes(std::string l, BaseEntity *ent)
	{

		auto matches = rexec_vec(l, R"(\"([\s\S]*?)\")");
		if (matches.empty())
		{
			return;
		}

		if (matches[0] == "mapversion")
		{
			mapVersion = stoi(matches[1]);
			return;
		}
		if (matches[0] == "wad")
		{
			parse_wad_string(matches[1]);
			return;
		}
		if (matches[0] == "classname")
		{
			ent->classname = matches[1];
			if (ent->classname == "worldspawn")
			{
				worldSpawn = reinterpret_cast<SolidEntity *>(ent);
			}
			return;
		}
		if (matches[0] == "origin")
		{
			if (matches.size() <= 1 || matches[1].empty())
			{
				return;
			}
			std::stringstream stream(matches[1]);
			stream >> ent->origin[0] >> ent->origin[1] >> ent->origin[2];
			return;
		}
		if (matches[0] == "angle")
		{
			std::stringstream stream(matches[1]);
			stream >> ent->angle;
			return;
		}
		if (matches[0] == "_tb_name")
		{
			auto bent = reinterpret_cast<SolidEntity *>(ent);
			bent->tbName = matches[1];
			return;
		}
		if (matches[0] == "_tb_type")
		{
			auto bent = reinterpret_cast<SolidEntity *>(ent);
			bent->tbType = matches[1];
			return;
		}
		if (matches[0] == "_phong")
		{
			auto bent = reinterpret_cast<SolidEntity *>(ent);
			bent->hasPhongShading = (matches[1] == "1");
			return;
		}
		ent->attributes.emplace(matches[0], matches[1]);
	}

	void QMapFile::parse_entity_planes(std::stringstream &lines, SolidEntity *ent)
	{
		Brush brush;
		for (std::string line; std::getline(lines, line);)
		{
			std::string chars = "()[]";
			for (unsigned int i = 0; i < chars.length(); ++i)
			{
				line.erase(std::remove(line.begin(), line.end(), chars[i]), line.end());
			}

			std::stringstream l(line);
			std::array<fvec3, 3> facePoints{};
			l >> facePoints[0][0] >> facePoints[0][1] >> facePoints[0][2];
			l >> facePoints[1][0] >> facePoints[1][1] >> facePoints[1][2];
			l >> facePoints[2][0] >> facePoints[2][1] >> facePoints[2][2];

			std::string texture;
			l >> texture;

			ValveUV valveUV{};
			StandardUV standardUV{};
			switch (mapVersion)
			{
			case VALVE_VERSION:
				l >> valveUV.u[0] >> valveUV.u[1] >> valveUV.u[2] >> valveUV.u[3];
				l >> valveUV.v[0] >> valveUV.v[1] >> valveUV.v[2] >> valveUV.v[3];
				break;
			default:
				l >> standardUV.u >> standardUV.v;
				break;
			}

			float rotation{}, scaleX{}, scaleY{};
			l >> rotation >> scaleX >> scaleY;

			auto face = mapVersion == STANDARD_VERSION ? std::make_shared<Face>(facePoints, getOrAddTexture(texture),
																				standardUV, rotation, scaleX, scaleY)
													   : std::make_shared<Face>(facePoints, getOrAddTexture(texture), valveUV, rotation, scaleX, scaleY);

			brush.faces.push_back(face);
		}
		ent->brushes.push_back(brush);
	}

	size_t QMapFile::getOrAddTexture(const std::string &texture)
	{
		for (int i = 0; i < textures.size(); i++)
		{
			if (textures[i] == texture)
				return i;
		}
		textures.push_back(texture);
		size_t ret = textures.size() - 1;
		return ret;
	}
}
