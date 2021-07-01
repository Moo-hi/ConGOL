#pragma once
#include <fan/graphics/graphics.hpp>

/// <summary>
/// 
/// Bundle of some utility functions
/// 
/// </summary>

static class Utils {
public:
    static inline fan::vec2i RoundToLowerPerfectSquare(fan::vec2i xy) {
        if (xy.x > xy.y) return (fan::vec2i(xy.y, xy.y));
        if (xy.y > xy.x) return (fan::vec2i(xy.x, xy.x));
        fan::print("Error, func: RoundToLowerPerfectSquare");
        return fan::vec2i(1, 1);
    }
};