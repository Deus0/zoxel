// the other players on a map (MapMarker, its PlayerLink theirs): where they are from the map's player, a block as
//  the map's view draws it (its texels flipped as maps are, zox_maps_flip_x and z), turned as they face (as the
//  map's arrow); off the map, on its edge the way they are
void map_marker_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSize);
    zox_sys_out(LayoutPosition);
    zox_sys_out(LocalRotation2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSize, marker_size);
        zox_sys_o(LayoutPosition, position);
        zox_sys_o(LocalRotation2, rotation);
        const entity body = zox_get_parent(world, e);
        const entity map = zox_valid(body) ? zox_get_parent(world, body) : 0;
        const entity view = zox_valid(body) ? zox_get_child_by_id(world, body, zox_id(MapView)) : 0;
        const entity player = zox_valid(map) ? zox_get_link(world, map, PlayerLink) : 0;
        const entity other = zox_get_link(world, e, PlayerLink);
        const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
        const entity other_character = zox_valid(other) ? zox_get_link(world, other, CharacterLink) : 0;
        const entity terrain = zox_valid(map) ? zox_get_link(world, map, TerrainLink) : 0;
        if (!zox_valid(view) || !zox_valid(character) || !zox_valid(other_character) || !zox_valid(terrain)) {
            continue;
        }
        // pixels a block: the view's size over the blocks it shows
        const int2 view_size = zox_getv(view, LayoutSize);
        const int texels = zox_getv(view, TextureSize).x;
        const int blocks = texels * zox_getv(view, MapBlocksPerTexel);
        if (blocks <= 0) {
            continue;
        }
        const float pixels_per_block = (float) view_size.x / blocks;
        const float scale = zox_getv(terrain, BlockScale);
        const float3 a = zox_getv(character, Position3D);
        const float3 b = zox_getv(other_character, Position3D);
        // a texel's row up the screen is along z (its sign flipped as the view draws it)
        float2 at = {
            (b.x - a.x) / scale * pixels_per_block * (zox_maps_flip_x ? -1 : 1),
            (b.z - a.z) / scale * pixels_per_block * (zox_maps_flip_z ? -1 : 1) };
        // off the map: on its edge, the way they are
        const float2 half = { view_size.x / 2.0f - marker_size->value.x / 2.0f, view_size.y / 2.0f - marker_size->value.y / 2.0f };
        const float fit = fminf(at.x != 0 ? half.x / fabsf(at.x) : 1e9f, at.y != 0 ? half.y / fabsf(at.y) : 1e9f);
        if (fit < 1) {
            at.x *= fit;
            at.y *= fit;
        }
        const int2 next = { (int) roundf(at.x), (int) roundf(at.y) };
        if (!int2_equals(position->value, next)) {
            position->value = next;
        }
        float3 euler = quaternion_to_euler(zox_getv(other_character, Rotation3D));
        if (zox_maps_flip_z) {
            euler.y += 180 * degrees_to_radians;
        }
        rotation->value = euler.y;
    }
    zox_sys_on_end();
} zoxd_system(map_marker_system);
