#include "logs.c"
#include "prefab.c"
#include "fetch.c"

void fetch_first_solidity(ecs* world, iter* it, const VoxLink* VoxLink_, byte* solidity) {
    entity first_terrain = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxLink, parent);
        if (zox_valid(parent->value)) {
            first_terrain = parent->value;
            break;
        }
    }
    if (!first_terrain) {
        zox_log_error("first terrain not found: %i", it->count);
        return;
    }
    if (!zox_has(first_terrain, RealmLink)) {
        zox_log_error("first terrain not found: terrain missing RealmLink [%s]", zox_get_name(first_terrain));
        return;
    }

    zox_geter_value(first_terrain, RealmLink, entity, realm);
    if (!zox_valid(realm)) {
        zox_log_error("Realm Invalid in Lighting");
        return;
    }

    zox_geter(realm, BlockLinks, blocks);
    for (int i = 0; i < blocks->length; i++) {
        entity block = blocks->value[i];
        if (!zox_valid(block) || !zox_has(block, BlockCollider)) {
            solidity[i] = 1;
        } else {
            solidity[i] = zox_gett_value(block, BlockCollider) == zox_block_solid;
        }
    }
}
