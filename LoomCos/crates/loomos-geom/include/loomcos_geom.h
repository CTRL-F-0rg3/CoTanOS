#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 2D Point in floating-point space
typedef struct {
    float x;
    float y;
} LoomGeomPoint;

// 2D Rectangle bounding box
typedef struct {
    float x;
    float y;
    float width;
    float height;
} LoomGeomRect;

// --- Geometric Query Functions (Powered by Rust) ---

// Checks if a point falls strictly inside a rectangle
bool loom_geom_rect_contains_point(const LoomGeomRect* rect, float px, float py);

// Checks if two rectangles intersect with each other
bool loom_geom_rect_intersects(const LoomGeomRect* a, const LoomGeomRect* b);

// Computes the bounding intersection (overlap) of two rectangles
bool loom_geom_rect_intersection(const LoomGeomRect* a, const LoomGeomRect* b, LoomGeomRect* out_result);

// Computes the bounding union of two rectangles
void loom_geom_rect_union(const LoomGeomRect* a, const LoomGeomRect* b, LoomGeomRect* out_result);

#ifdef __cplusplus
}
#endif