// NOTE: Links a Tunk to a Region it is in
zox_sys2(TunkRegionLinkSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TunkPosition);
    zox_sys_out(RegionLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TunkPosition, position);
        zox_sys_o(RegionLink, region);
        if (zox_valid(region->value)) {
            continue;
        }
        int2 region_position = tunk_position_to_region_position(position->value);
        // int2 region_position = (int2) { position->value.x / region_dividor, position->value.y / region_dividor };
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            continue;
        }
        zox_geter(terrain, RegionLinks, regions);
        entity find_region = int2_hashmap_get(regions->value, region_position);
        if (find_region) {
            if (dbg_log) {
               zox_log("Region Found for: Tunk (%s)[%ix%i] => Region (%s)[%ix%i]", zox_get_name(e), position->value.x, position->value.y, zox_get_name(find_region), region_position.x, region_position.y);
            }
            region->value = find_region;
        } else {
            zox_logw("Failure finding Region [%ix%i]", region_position.x, region_position.y);
        }
    }
} zox_sys_end(TunkRegionLinkSystem);
