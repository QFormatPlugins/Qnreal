#include "wrapper.h"

#include <iostream>
#include <string>
#include <vector>

API_EXPORT qformats::map::QMap *LoadMap(const char *mapFile, qformats::map::getTextureBoundsCb getTextureBounds)
{
    auto map = new qformats::map::QMap();
    map->LoadFile(mapFile, getTextureBounds);
    return map;
}

API_EXPORT void GenerateGeometry(qformats::map::QMap *ptr, int *outEntCount, bool clipBrushes = false)
{
    ptr->GenerateGeometry(clipBrushes);
    *outEntCount = ptr->GetSolidEntities().size();
}

API_EXPORT void GetTextures(qformats::map::QMap *ptr, void(add)(const char *tex))
{
    for (auto texName : ptr->GetTexturesNames())
    {
        add(texName.c_str());
    }
}

API_EXPORT void GetEntityAttributes(qformats::map::QMap *ptr, int idx, qformats::map::SolidEntity **entOut, void(add)(const char *first, const char *second))
{
    if (entOut == nullptr || ptr == nullptr || ptr->GetSolidEntities().size() < idx)
    {
        return;
    }

    *entOut = ptr->GetSolidEntities()[idx].get();
    for (const auto &pair : (*entOut)->attributes)
    {
        add(pair.first.c_str(), pair.second.c_str());
    }
    return;
}

API_EXPORT QMapSolidEntity GetSolidEntityData(qformats::map::SolidEntity *ptr)
{
    QMapSolidEntity outData{};
    if (ptr != nullptr)
    {
        outData.classname = ptr->classname.c_str();
        outData.center = toVec3(ptr->GetCenter());
        outData.min = toVec3(ptr->GetMin());
        outData.max = toVec3(ptr->GetMax());
    }
    return outData;
}

API_EXPORT void GetSolidEntityBrushes(qformats::map::SolidEntity *ent, void(add)(int idx, vec3 min, vec3 max))
{
    const auto brushes = ent->GetClippedBrushes();
    for (int i = 0; i < brushes.size(); i++)
    {
        add(i, toVec3(brushes[i].min), toVec3(brushes[i].max));
    }
}

API_EXPORT void GetBrushFaces(qformats::map::SolidEntity *ent, int brushIdx, void(add)(int idx, int texID))
{
    if (ent == nullptr || ent->GetClippedBrushes().size() < brushIdx)
    {
        return;
    }
    const auto &brush = ent->GetClippedBrushes()[brushIdx];
    const auto nativeFaces = brush.GetFaces();

    for (int i = 0; i < nativeFaces.size(); i++)
    {
        add(i, nativeFaces[i]->TextureID());
    }
}

API_EXPORT void GetFaceData(qformats::map::SolidEntity *ent, int brushIdx, int faceIdx, vert **verts, unsigned short **indices, int *vertCount, int *indexCount)
{
    if (ent == nullptr || ent->GetClippedBrushes().size() < brushIdx)
    {
        return;
    }
    const auto &brush = ent->GetClippedBrushes()[brushIdx];
    const auto &face = brush.GetFaces()[faceIdx];

    *vertCount = face->GetVertices().size();
    *indexCount = face->GetIndices().size();

    if (vertCount == 0)
    {
        return;
    }

    *verts = (vert *)CoTaskMemAlloc(*vertCount * sizeof(vert));
    for (int i = 0; i < *vertCount; i++)
    {
        const auto &vert = face->GetVertices()[i];
        (*verts)[i].pos = toVec3(vert.point);
        (*verts)[i].normal = toVec3(vert.normal);
        (*verts)[i].tangent = toVec4(vert.tangent);
        (*verts)[i].uv = toVec2(vert.uv);
    }

    *indices = (unsigned short *)CoTaskMemAlloc(*indexCount * sizeof(unsigned short));
    for (int i = 0; i < *indexCount; i++)
    {
        (*indices)[i] = face->GetIndices()[i];
    }
}

API_EXPORT void DestroyMap(qformats::map::QMap *ptr)
{
    delete ptr;
}