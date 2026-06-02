byte cache_blocks_data(iter* it, chunk3_textured_builder_data* data) {
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
        return 0;
    }
    zox_geter(manager, BlockLinks, blocks);
    if (!blocks->length) {
        return 0; // if failed to find terrain parents
    }
    data->solidity = malloc(blocks->length * sizeof(byte));
    for (int i = 0; i < blocks->length; i++) {
        entity block = blocks->value[i];
        if (!zox_valid(block)) {
            data->solidity[i] = 1;
            continue;
        }
        // solidity
        if (!zox_has(block, BlockModel)) {
            data->solidity[i] = 1;
        } else {
            data->solidity[i] = zox_gett_value(block, BlockModel) == zox_block_solid;
        }
    }
    return 1;
}
