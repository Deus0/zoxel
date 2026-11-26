zox_tag(PolyTerrain);
// Row-major grid vertices, indexed by x + z * (cells.x + 1).
zoxc_arrayd(PolyHeights, float);
// Cell counts along x and z. Size2 holds the size in metres.
zoxc_int2(PolyCells);

void zox_components_poly_terrains(ecs* world) {
    zoxd_tag(PolyTerrain);
    zoxd_arrayd(PolyHeights);
    zoxd_int2(PolyCells);
}
