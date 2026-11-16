// tst: furniture_rotation, a bed turned to each of four directions (items/fun/multi_block.c, block data)
//  - placed with a direction: its filler in its turned cell, the filler's data its offset back to the base
//  - its mesh as drawn (its TransformMatrix) covers its base and filler cells, no more
//  - rays straight down onto its base and its filler hit the bed's voxels (the filler's through its base's model)
//  - breaking one bed's filler takes its base, breaking another's base takes its filler
//  - run: bash bsh/tst.sh furniture_rotation
#define zox_tst_frot_beds 4
byte zox_tst_frot_stage = 0;
int zox_tst_frot_frames = 0;
int3 zox_tst_frot_bases[zox_tst_frot_beds];

static inline int3 zox_tst_frot_filler(byte direction, int3 base) {
    return int3_add(base, block_direction_offset(direction, (int3) { 1, 0, 0 }));
}

byte zox_tst_furniture_rotation_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    const float scale = zox_getv(terrain, BlockScale);
    zox_tst_frot_frames++;
    zox_tst_progress(zox_tst_frot_stage * 10000 + zox_tst_frot_frames);
    if (zox_tst_frot_stage == 0) {
        zox_tst_waiting = "air for four beds";
        const int length = octree_size(zox_getv(terrain, NodeDepth));
        const int3 chunk = real_position_to_chunk_position(zox_getv(character, Position3D), length, scale);
        const int3 origin = { chunk.x * length, chunk.y * length, chunk.z * length };
        const int2 spots[zox_tst_frot_beds] = { { 3, 3 }, { 9, 3 }, { 3, 10 }, { 9, 10 } };
        for (int y = 0; y < length * 2 - 4; y++) {
            int solid = 0;
            for (int i = 0; i < zox_tst_frot_beds; i++)
            for (int x = -1; x <= 1; x++) for (int h = 0; h < 4; h++) for (int z = -1; z <= 1; z++) {
                solid += terrain_voxel_at(world, terrain, (int3) { origin.x + spots[i].x + x, origin.y + y + h, origin.z + spots[i].y + z }) != 0;
            }
            if (solid) {
                continue;
            }
            const byte bed = zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockBed)), BlockIndex);
            for (byte i = 0; i < zox_tst_frot_beds; i++) {
                zox_tst_frot_bases[i] = (int3) { origin.x + spots[i].x, origin.y + y, origin.z + spots[i].y };
                zox_tst_check(zox_tst_place_supported(world, terrain, realm, zox_tst_frot_bases[i], bed, i), "bed [%i] wasn't placed", i);
            }
            zox_tst_frot_stage = 1;
            zox_tst_frot_frames = 0;
            return 0;
        }
        return zox_tst_frot_frames > 600;
    }
    if (zox_tst_frot_stage == 1) {
        zox_tst_waiting = "the beds' block voxes and meshes";
        if (zox_tst_frot_frames < 10) {
            return 0;
        }
        for (byte i = 0; i < zox_tst_frot_beds; i++) {
            byte3 local;
            const entity chunk = terrain_chunk_local(world, terrain, zox_tst_frot_bases[i], &local);
            const entity vode = chunk ? zox_tst_fplace_vode(world, chunk, local) : 0;
            const entity mesh = zox_valid(vode) ? zox_get_link(world, vode, InstanceLink) : 0;
            if (!zox_valid(mesh) || !zox_has(mesh, MeshVertices) || !zox_get(mesh, MeshVertices)->length) {
                return zox_tst_frot_frames > 3600;
            }
        }
        const byte filler = realm_filler_voxel(world, realm);
        const byte bed = zox_getv(zox_tst_fplace_block(world, realm, zox_id(BlockBed)), BlockIndex);
        for (byte i = 0; i < zox_tst_frot_beds; i++) {
            const int3 base = zox_tst_frot_bases[i];
            const int3 part = zox_tst_frot_filler(i, base);
            const byte part_data = terrain_block_data_at(world, terrain, part);
            zox_tst_check(terrain_voxel_at(world, terrain, base) == bed && terrain_block_data_at(world, terrain, base) == 1 + i,
                "bed [%i]'s base is [%i] with data [%i]", i, terrain_voxel_at(world, terrain, base), terrain_block_data_at(world, terrain, base));
            zox_tst_check(terrain_voxel_at(world, terrain, part) == filler && part_data &&
                int3_equals(int3_add(part, block_offset_decode(part_data)), base),
                "bed [%i]'s filler cell [%i %i %i] off its base holds [%i] data [%i]", i, part.x - base.x, part.y - base.y, part.z - base.z,
                terrain_voxel_at(world, terrain, part), part_data);
            // its mesh as drawn covers its two cells
            byte3 local;
            const entity chunk = terrain_chunk_local(world, terrain, base, &local);
            const entity vode = zox_tst_fplace_vode(world, chunk, local);
            const float4x4 matrix = zox_getv(vode, TransformMatrix);
            const MeshVertices* vertices = zox_get(zox_get_link(world, vode, InstanceLink), MeshVertices);
            float3 low = float4x4_multiply_float3(matrix, vertices->value[0]), high = low;
            for (int j = 1; j < vertices->length; j++) {
                const float3 v = float4x4_multiply_float3(matrix, vertices->value[j]);
                low = (float3) { fminf(low.x, v.x), fminf(low.y, v.y), fminf(low.z, v.z) };
                high = (float3) { fmaxf(high.x, v.x), fmaxf(high.y, v.y), fmaxf(high.z, v.z) };
            }
            const int3 cells_low = { int_min(base.x, part.x), base.y, int_min(base.z, part.z) };
            const int3 cells_high = { int_max(base.x, part.x) + 1, base.y + 1, int_max(base.z, part.z) + 1 };
            zox_log("[tst] bed [%i] mesh [%.2f %.2f %.2f] to [%.2f %.2f %.2f] blocks, cells [%i %i %i] to [%i %i %i]", i,
                low.x / scale, low.y / scale, low.z / scale, high.x / scale, high.y / scale, high.z / scale,
                cells_low.x, cells_low.y, cells_low.z, cells_high.x, cells_high.y, cells_high.z);
            const float slack = 0.1f * scale;
            zox_tst_check(low.x > cells_low.x * scale - slack && low.z > cells_low.z * scale - slack && high.x < cells_high.x * scale + slack &&
                high.z < cells_high.z * scale + slack && high.x - low.x > 1.5f * scale * (part.x != base.x) &&
                high.z - low.z > 1.5f * scale * (part.z != base.z), "bed [%i]'s mesh isn't over its two cells", i);
            // a ray down onto its base
            float3 from = zox_tst_fhits_middle(base, scale);
            from.y += 3 * scale;
            const RaycastVoxelData ray = zox_tst_fhits_ray(world, terrain, realm, from, 6 * scale);
            const float stop = from.y - ray.distance;
            zox_tst_check(ray.result != rayhit_none && ray.hit_block && zox_getv(ray.hit_block, BlockIndex) == bed &&
                stop >= base.y * scale - 0.01f && stop <= (base.y + 1) * scale + 0.01f,
                "a ray onto bed [%i]'s base hit [%s] at [%.2f] blocks", i, ray.hit_block ? zox_getn(ray.hit_block) : "none", stop / scale);
            // and onto its filler: into the bed's turned model, its mattress below the cell's top
            float3 over_part = zox_tst_fhits_middle(part, scale);
            over_part.y += 3 * scale;
            const RaycastVoxelData ray2 = zox_tst_fhits_ray(world, terrain, realm, over_part, 6 * scale);
            const float stop2 = over_part.y - ray2.distance;
            zox_log("[tst] ray onto bed [%i]'s filler hit [%s] at [%.2f] blocks", i, ray2.hit_block ? zox_getn(ray2.hit_block) : "none", stop2 / scale);
            zox_tst_check(ray2.hit_block && zox_getv(ray2.hit_block, BlockIndex) == bed && stop2 >= part.y * scale - 0.01f &&
                stop2 < (part.y + 1) * scale - 0.01f, "a ray onto bed [%i]'s filler hit [%s] at [%.2f] blocks, not the bed in its cell", i,
                ray2.hit_block ? zox_getn(ray2.hit_block) : "none", stop2 / scale);
        }
        // break bed 1's filler and bed 2's base
        terrain_queue_voxel(world, terrain, zox_tst_frot_filler(1, zox_tst_frot_bases[1]), 0, 0);
        terrain_queue_voxel(world, terrain, zox_tst_frot_bases[2], 0, 0);
        zox_tst_frot_stage = 2;
        zox_tst_frot_frames = 0;
        return 0;
    }
    if (zox_tst_frot_frames < 20) {
        return 0;
    }
    for (byte i = 1; i <= 2; i++) {
        const int3 base = zox_tst_frot_bases[i];
        const int3 part = zox_tst_frot_filler(i, base);
        zox_tst_check(!terrain_voxel_at(world, terrain, base) && !terrain_voxel_at(world, terrain, part),
            "breaking bed [%i]'s %s left [%i %i]", i, i == 1 ? "filler" : "base", terrain_voxel_at(world, terrain, base),
            terrain_voxel_at(world, terrain, part));
    }
    zox_tst_frot_stage = 3;
    return 1;
}

void zox_tst_furniture_rotation_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_frot_stage == 3, "it stopped at stage [%i]", zox_tst_frot_stage);
}

void zox_tsts_furniture_rotation(ecs* world) {
    zox_tst_add("furniture_rotation", zox_tst_furniture_rotation_ready, zox_tst_furniture_rotation_check, 180);
}
