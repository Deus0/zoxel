void fetch_children_blocks(
    ecs *world,
    BlockLinks* blocks,
    entity e,
    byte dbg_log)
{
    if (zox_has(e, Block)) {
        add_to_BlockLinks(blocks, e);
        if (dbg_log >= 2) {
            zox_log("...Found New Block [%s]", zox_get_name(e));
        }
    } else {
        if (dbg_log >= 2) {
            zox_log("...Seeking Blocks at [%s]", zox_get_name(e));
        }
    }
    entity children[zox_children_capacity];
    uint length = zox_get_children(
        world,
        e,
        children,
        zox_children_capacity);
    for (uint k = 0; k < length; k++) {
        entity e2 = children[k];
        fetch_children_blocks(
            world,
            blocks,
            e2,
            dbg_log);
    }
}

// NOTE: Sets all Realm Block Indexes after dirty
zox_sys2(RealmBlocksDirtySystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    //zox_sys_out(BlocksDirty);
    zox_sys_out(BlockLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        //zox_sys_o(BlocksDirty, dirty);
        zox_sys_o(BlockLinks, blocks);
        /*if (dirty->value != zox_blocks_dirty_indexes) {
            continue;
        }*/
        resize_BlockLinks(blocks, 0);
        fetch_children_blocks(world, blocks, e, dbg_log);
        if (dbg_log) {
            zox_log("Realm Blocks Dirty [%s] Blocks [%i]",
                zox_get_name(e),
                blocks->length);
        }
        for (int j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];
            zox_setv(block, BlockIndex, j + 1);
            if (dbg_log) {
                zox_log(" - Realm Block [%s]:[%i]",
                    zox_get_name(block),
                    j + 1);
            }
        }
        zox_remove(e, BlocksDirty);
        zox_add(e, BlocksTilemapUpdate);
        // dirty->value = zox_blocks_dirty_tilemaps;
    }
} zox_sys_end(RealmBlocksDirtySystem);
