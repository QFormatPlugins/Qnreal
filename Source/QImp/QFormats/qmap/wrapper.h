#pragma once

#include <qmap/map.h>

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#if defined(__NT__)
#define API_EXPORT EXTERNC __stdcall
#elif defined(__UNIX__)
#define API_EXPORT EXTERNC
#elif defined(__APPLE__)
#define API_EXPORT EXTERNC __attribute__((visibility("default")))
#endif

#define CoTaskMemAlloc(p) malloc(p)
#define CoTaskMemFree(p) free(p)

struct vec4
{
    float x;
    float y;
    float z;
    float w;
};

struct vec3
{
    float x;
    float y;
    float z;
};

struct vec2
{
    float x;
    float y;
};

struct vert
{
    vec3 pos;
    vec3 normal;
    vec4 tangent;
    vec2 uv;
};

static vec4 toVec4(fvec4 from)
{
    vec4 out;
    out.x = from[0];
    out.y = from[1];
    out.z = from[2];
    out.w = from[3];
    return out;
}

static vec3 toVec3(fvec3 from)
{
    vec3 out;
    out.x = from[0];
    out.y = from[1];
    out.z = from[2];
    return out;
}

static vec2 toVec2(fvec2 from)
{
    vec2 out;
    out.x = from[0];
    out.y = from[1];
    return out;
}

struct attribute
{
    const char *key;
    const char *value;
};

struct face
{
    int textureID;
    vert **vertices;
    unsigned short **indices;
};

struct brush
{
    bool isBlockVolume;
    face **faces;
};

struct QMapSolidEntity
{
    const char *classname;
    vec3 center;
    vec3 min;
    vec3 max;
};

struct QMapWrapper
{
    qformats::map::QMap *ptr;
    QMapSolidEntity worldspawn;
    int solidEntityCount;
    int pointEntityCount;
    bool clipBrushes;
};
