// tst: lod_chunk_lights, far (coarse lod) chunks are lit like near ones, not dark in the distance
//  - waits for the player, then for the terrain's meshes and lights to settle
//  - groups terrain meshes by their RenderDepth, a coarse group's average light is near the detailed one's
//    (coarse meshes read stale open light nodes, darklight, before getv_nearby_max_LightNode)
//  - a chunk beside a different lod one isn't dark (lighting between lods)
//  - coarse lods aren't dark (under 1.5%, dark home interiors are real), runs on --medium terrain
//  - run: bash bsh/tst.sh lod_chunk_lights
byte zox_tst_player_spawn_ready(ecs* world);   // in _.c, after this is included
double zox_tst_lod_lights_start = 0;
#define zox_tst_lod_lights_settle 6.0
#define zox_tst_lod_depths 8

byte zox_tst_lod_chunk_lights_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    if (!zox_tst_lod_lights_start) {
        zox_tst_lod_lights_start = zox_current_time;
    }
    zox_tst_waiting = "the terrain's lights and meshes to settle";
    return zox_current_time - zox_tst_lod_lights_start >= zox_tst_lod_lights_settle;
}

// a terrain chunk's mesh average light and render depth
static inline byte zox_tst_lod_mesh(ecs* world, entity chunk, float* average, byte* depth, int* dark, int* vertices) {
    double total = 0;
    int count = 0;
    iter it2 = zox_children(world, chunk);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity mesh = it2.entities[j];
            if (!zox_has(mesh, ChunkMesh) || !zox_has(mesh, MeshColorRGBs) || !zox_has(mesh, RenderDepth)) {
                continue;
            }
            const MeshColorRGBs* colors = zox_get(mesh, MeshColorRGBs);
            for (int k = 0; k < colors->length; k++) {
                total += colors->value[k].r;
                *dark += colors->value[k].r <= darklight;
            }
            count += colors->length;
            *vertices += colors->length;
            *depth = zox_getv(mesh, RenderDepth);
        }
    }
    *average = count ? (float) (total / count) : 0;
    return count > 0;
}

void zox_tst_lod_chunk_lights_check(ecs* world) {
    double totals[zox_tst_lod_depths] = { 0 };
    int counts[zox_tst_lod_depths] = { 0 };
    int dark_borders = 0, borders = 0, ignored = 0;
    int darks[zox_tst_lod_depths] = { 0 }, vertices[zox_tst_lod_depths] = { 0 };
    iter it2 = zox_children(world, local_terrain);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity chunk = it2.entities[j];
            float average;
            byte depth = 0;
            int dark = 0, verts = 0;
            if (!zox_has(chunk, Chunk3) || !zox_tst_lod_mesh(world, chunk, &average, &depth, &dark, &verts) || depth >= zox_tst_lod_depths) {
                continue;
            }
            totals[depth] += average;
            darks[depth] += dark;
            vertices[depth] += verts;
            counts[depth]++;
            // beside a chunk of another lod: a border between lods
            const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
            for (byte n = 0; n < chunk_neighbors_length; n++) {
                float neighbor_average;
                byte neighbor_depth;
                if (zox_valid(neighbors->value[n]) && zox_tst_lod_mesh(world, neighbors->value[n], &neighbor_average, &neighbor_depth, &ignored, &ignored) &&
                    neighbor_depth != depth) {
                    borders++;
                    dark_borders += average <= darklight + 1;
                    break;
                }
            }
        }
    }
    byte finest = 0, coarsest = 255;
    for (byte d = 0; d < zox_tst_lod_depths; d++) {
        if (!counts[d]) {
            continue;
        }
        zox_log("[tst] [metric] lod_chunk_lights depth [%i] chunks [%i] average light [%.1f] dark vertices [%.1f%%]", d, counts[d],
            totals[d] / counts[d], vertices[d] ? 100.0f * darks[d] / vertices[d] : 0);
        finest = d > finest ? d : finest;
        coarsest = d < coarsest ? d : coarsest;
    }
    zox_log("[tst] [metric] lod_chunk_lights [%i] chunks beside another lod, [%i] dark", borders, dark_borders);
    zox_tst_check(counts[finest], "no terrain meshes");
    zox_tst_check(coarsest < finest, "only one lod depth [%i], no coarse chunks to compare", finest);
    if (coarsest < finest && counts[finest]) {
        const float fine = totals[finest] / counts[finest], rough = totals[coarsest] / counts[coarsest];
        zox_tst_check(rough >= fine * 0.6f, "coarse chunks (depth %i) are dark, [%.1f] to the near ones' [%.1f]", coarsest, rough, fine);
    }
    zox_tst_check(!dark_borders, "[%i] of [%i] chunks beside another lod are dark", dark_borders, borders);
    // coarse lods aren't dark: under 1.5% (homes are dark inside now, roofed, a little is real), the exact stale
    //  read check is the pure tst lod_lights
    for (byte d = 0; d < finest; d++) {
        if (vertices[d]) {
            const float share = 100.0f * darks[d] / vertices[d];
            zox_tst_check(share <= 1.5f, "depth [%i] has [%.2f%%] dark vertices, coarse lods read stale lights", d, share);
        }
    }
}
