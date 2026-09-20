// NOTE: Adds biome blocks onto realm
zox_sys2(BiomeBlocksDirtySystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        if (generate->value != zox_dirty_end) {
            continue;
        }
        entity realm = zox_get_link(world, e, RealmLink);
        if (dbg_log) {
            uint realm_blocks = zox_get_children_count_by_id(
                world,
                realm,
                zox_id(Block));
            uint biome_blocks = zox_get_children_count_by_id(
                world,
                e,
                zox_id(Block));
            zox_log("Biome Blocks Dirty [%s]:[%i] with Realm Blocks [%i]",
                zox_get_name(e),
                biome_blocks,
                realm_blocks);
        }
        zox_add(realm, BlocksDirty);
    }
} zox_sys_end(BiomeBlocksDirtySystem);
