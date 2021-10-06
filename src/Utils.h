#pragma once
#include <fan/graphics/graphics.hpp>

/// <summary>
/// 
/// Bundle of some utility functions
/// 
/// </summary>

static class Utils {
public:
    // Floors the given vec2i to a value that is perfectly square
    static inline fan::vec2i FloorToPerfectSquare(fan::vec2i xy) {
        if (xy.x > xy.y) return (fan::vec2i(xy.y, xy.y));
        if (xy.y > xy.x) return (fan::vec2i(xy.x, xy.x));
        fan::print("Error, func: FloorToPerfectSquare");
        return fan::vec2i(1, 1);
    }
};