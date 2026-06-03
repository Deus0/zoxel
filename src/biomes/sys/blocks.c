// NOTE: Adds biome blocks onto realm
zox_sys2(BiomeBlocksDirtySystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(RealmLink);
    zox_sys_in(BlockLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(RealmLink, realm);
        zox_sys_i(BlockLinks, blocks);
        if (generate->value != zox_dirty_end) {
            continue;
        }
        zox_muter(realm->value, BlockLinks, realm_blocks);
        for (int j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];
            add_to_BlockLinks(realm_blocks, block);
            if (dbg_log) {
                zox_log(" - Biome Block [%s]:[%i]", zox_get_name(block), realm_blocks->length);
            }
        }
        zox_set(realm->value, BlocksDirty, { zox_dirty_trigger });
    }
} zox_sys_end(BiomeBlocksDirtySystem);
