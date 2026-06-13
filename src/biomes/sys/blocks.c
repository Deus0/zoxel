// NOTE: Adds biome blocks onto realm
zox_sys2(BiomeBlocksDirtySystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(RealmLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(RealmLink, realm);
        if (generate->value != zox_dirty_end) {
            continue;
        }
        if (dbg_log) {
            uint realm_blocks = zox_get_children_count_by_id(world, realm->value, zox_id(Block));
            uint biome_blocks = zox_get_children_count_by_id(world, e, zox_id(Block));
            zox_log("Biome Blocks Dirty [%s]:[%i] with Realm Blocks [%i]", zox_get_name(e), biome_blocks, realm_blocks);
        }
        zox_set(realm->value, BlocksDirty, { zox_dirty_trigger });
    }
} zox_sys_end(BiomeBlocksDirtySystem);
