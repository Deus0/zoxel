// a map's view (MapView): the land round the player, a texel blocks_per_texel blocks, from its tunks' top down
//  colours (tunk_map_colors), flipped as maps are (zox_maps_flip_x, zox_maps_flip_z)
//  - redrawn when the player moves a texel (per texel, it moved a whole tunk at a time), when a tunk's colours
//    change (TunkMapsVersion), and every map_view_retry seconds while a tunk it shows isn't there or made yet
//  - no texture made per tunk, no tunk's colours made twice: moving across a tunk only makes the newly seen ones'
const double map_view_retry = 0.25;

static inline int map_floor_div(int a, int b) {
    return a >= 0 ? a / b : -((-a + b - 1) / b);
}

// draws the view round block at, returns 1 if every tunk it shows had its colours
byte draw_map_view(ecs* world, entity terrain, int2 at, int texels, int blocks_per_texel, TextureData* data) {
    const int tunk_length = octree_size(zox_getv(terrain, NodeDepth));
    const TunkLinks* tunks = zox_get(terrain, TunkLinks);
    const int sx = zox_maps_flip_x ? -1 : 1, sz = zox_maps_flip_z ? -1 : 1;
    byte whole = 1;
    int2 last_tunk = int2_hidden;
    const TunkMapColors* colors = NULL;
    int side = 0;
    for (int ty = 0; ty < texels; ty++) {
        for (int tx = 0; tx < texels; tx++) {
            const int2 block = {
                at.x + (tx - texels / 2) * blocks_per_texel * sx,
                at.y + (ty - texels / 2) * blocks_per_texel * sz
            };
            const int2 tunk_position = { map_floor_div(block.x, tunk_length), map_floor_div(block.y, tunk_length) };
            if (!int2_equals(tunk_position, last_tunk)) {
                last_tunk = tunk_position;
                const entity tunk = int2_hashmap_get(tunks->value, tunk_position);
                colors = tunk_map_colors(world, tunk) ? zox_get(tunk, TunkMapColors) : NULL;
                side = colors ? (int) sqrtf((float) colors->length) : 0;
                whole &= colors != NULL;
            }
            color c = color_black;
            if (colors) {
                const int lx = (block.x - tunk_position.x * tunk_length) * side / tunk_length;
                const int lz = (block.y - tunk_position.y * tunk_length) * side / tunk_length;
                c = colors->value[lx + lz * side];
            }
            data->value[tx + ty * texels] = c;
        }
    }
    return whole;
}

void map_view_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MapBlocksPerTexel);
    zox_sys_out(MapDrawnAt);
    zox_sys_out(MapDrawnVersion);
    zox_sys_out(MapDrawnWhole);
    zox_sys_out(MapDrawnTime);
    zox_sys_out(TextureData);
    zox_sys_out(TextureSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MapBlocksPerTexel, blocks_per_texel);
        zox_sys_o(MapDrawnAt, drawn_at);
        zox_sys_o(MapDrawnVersion, drawn_version);
        zox_sys_o(MapDrawnWhole, drawn_whole);
        zox_sys_o(MapDrawnTime, drawn_time);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureSize, size);
        const entity map = zox_get_parent(world, zox_get_parent(world, e));
        const entity terrain = zox_valid(map) ? zox_get_link(world, map, TerrainLink) : 0;
        const entity player = zox_valid(map) ? zox_get_link(world, map, PlayerLink) : 0;
        const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
        if (!zox_valid(terrain) || !zox_valid(character) || !zox_has(terrain, TunkLinks)) {
            continue;
        }
        const float scale = zox_getv(terrain, BlockScale);
        const float3 position = zox_getv(character, Position3D);
        const int bpt = blocks_per_texel->value;
        // the texel it's on: a block, floored to blocks per texel
        const int2 texel = {
            map_floor_div((int) floorf(position.x / scale), bpt),
            map_floor_div((int) floorf(position.z / scale), bpt)
        };
        const int version = zox_has(terrain, TunkMapsVersion) ? zox_getv(terrain, TunkMapsVersion) : 0;
        const byte moved = !int2_equals(texel, drawn_at->value) || version != drawn_version->value;
        if (!moved && (drawn_whole->value || zox_current_time - drawn_time->value < map_view_retry)) {
            continue;
        }
        const int tunk_length = octree_size(zox_getv(terrain, NodeDepth));
        const int texels = (2 * zox_getv(map, MapZoom) + 1) * tunk_length / bpt;
        if (size->value.x != texels || data->length != texels * texels) {
            size->value = int2_single(texels);
            resize_TextureData(data, texels * texels);
        }
        drawn_whole->value = draw_map_view(world, terrain, int2_multiply1(texel, bpt), texels, bpt, data);
        drawn_at->value = texel;
        drawn_version->value = version;
        drawn_time->value = zox_current_time;
        zox_add(e, TextureDirty);
    }
    zox_sys_on_end();
} zoxd_system(map_view_system);
