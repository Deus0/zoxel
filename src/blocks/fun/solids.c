// Make sure BlockManagerLink is first one
byte* blocks_fetch_solids(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(BlockManagerLink);

    entity manager = 0;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(BlockManagerLink, blocker);

        if (!zox_valid(blocker->value)) {
            continue;
        }

        manager = blocker->value;

        break;
    }

    if (!manager) {
        return NULL;
    }

    zox_geter(manager, BlockLinks, blocks);
    if (!blocks->length) {
        return NULL;
    }

    // byte solidity[blocks->length];
    // build_data.solidity = solidity;
    // int uvs[blocks->length * 6];
    // build_data.uvs = uvs;
    // data->solidity = byte[blocks->length];
    // data->uvs = int[blocks->length * 6];
    byte* solids = malloc(blocks->length * sizeof(byte));

    for (int i = 0; i < blocks->length; i++) {
        entity block = blocks->value[i];

        if (!zox_valid(block) || !zox_has(block, BlockModel)) {
            solids[i] = 1;
            continue;
        }

        solids[i] = zox_gett_value(block, BlockModel) == zox_block_solid;
    }

    return solids;
}