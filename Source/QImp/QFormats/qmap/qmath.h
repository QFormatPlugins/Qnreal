#pragma once

#include <cmath>
#include "qmap/types.h"

namespace qformats::map
{
    const float CMP_EPSILON = 0.008;
#ifndef M_PI
    const float M_PI = 3.14159265358979323846;
#endif
    const auto UP_VEC = fvec3{0, 0, 1};
    const auto RIGHT_VEC = fvec3{0, 1, 0};
    const auto FORWARD_VEC = fvec3{1, 0, 0};

    class QMath
    {
    public:
        static fvec4 CalcTangent(bool isValve, const MapFileFace &face)
        {
            return isValve ? calcValveTangent(face) : calcStandardTangent(face);
        };
        static fvec2 CalcUV(bool isValve, fvec3 vertex, const MapFileFace &face, int texW, int texH)
        {
            return isValve ? calcValveUV(vertex, face, texW, texH) : calcStandardUV(vertex, face, texW, texH);
        };

        static fvec4 calcStandardTangent(const MapFileFace &face);
        static fvec4 calcValveTangent(const MapFileFace &face);
        static fvec2 calcStandardUV(fvec3 vertex, const MapFileFace &face, int texW, int texH);
        static fvec2 calcValveUV(fvec3 vertex, const MapFileFace &face, int texW, int texH);
    };
}