// Flecs wrapper tests, checking the claims in doc/links.md and doc/children.md
//  - included from src/core/tst/_.c, as "_" is below the tst harness
//  - registered from import_core, as test relations and tags need the world
#include "links.c"
#include "children.c"

void zox_tsts_flecs(ecs* world) {
    zox_components_tst_links(world);
    zox_components_tst_children(world);
    zox_tst_add("links_api", zox_tst_links_api_ready, zox_tst_links_api_check, 30);
    zox_tst_add("children", zox_tst_children_ready, zox_tst_children_check, 30);
}
