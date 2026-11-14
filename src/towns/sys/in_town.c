// a player's character inside a town's walls gets InTown and TownLink (its town), both removed when it leaves:
//  observers greet it (a toast) and play the town's music (huds, gam/zoxel)
//  - later every character, i.e. town buffs
void in_town_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        const entity terrain = zox_get_link(world, e, TerrainLink);
        if (!zox_valid(terrain)) {
            continue;
        }
        const float scale = zox_getv(terrain, BlockScale);
        const int2 block = { (int) floorf(position->value.x / scale), (int) floorf(position->value.z / scale) };
        const entity town = town_at(world, block);
        const entity old = zox_get_link(world, e, TownLink);
        // its link first, InTown once it reads back (a later frame), so InTown's observers read their town
        if (town != old) {
            if (town) {
                zox_set_link(world, e, TownLink, town);
            } else {
                zox_unlink(world, e, TownLink, old);
                zox_remove(e, InTown);
            }
        } else if (town && !zox_has(e, InTown)) {
            zox_add(e, InTown);
        }
    }
    zox_sys_on_end();
} zoxd_system(in_town_system);
