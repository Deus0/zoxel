/*
AABB entity
AABB voxel
    |
    + smallest overlap axis (voxel is skipped if that face is buried)
        |
        + collision normal

Tunneling: this system runs AFTER the movement system, so it only sees where the entity
ended up. When the frame's movement is bigger than a safe step, the move is redone from
where the entity started (position - velocity * dt, drag corrected), one axis at a time (y, then x, then z),
checking every voxel layer on the way. A sweep never carries the entity deeper into a block
it is already standing in: it pushes it back out instead. Then the normal push-out runs.
Last line of defence: if the entity's centre still ends up inside a solid block, it is
walked back the way it came (or straight up) until its box is clear.
*/

// A frame's movement up to this fraction of min(half block, smallest entity half extent)
// is handled by the push-out alone; bigger moves are swept. Keep it below 1: deeper than
// the entity half width and a seam between two voxels makes the "buried face" rule
// discard BOTH voxels, which is itself a way to fall through.
#ifndef collision_step_scale
#define collision_step_scale 0.75f
#endif

// Set to 1 to log every entity every frame (to see exactly what happens when something falls through).
#ifndef collision_debug
    #define collision_debug 0
#endif

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Voxel 0 and blocks with an air collider are passable.
// A block without a BlockCollider counts as solid, an invalid block as passable.
static inline byte collision_is_voxel_solid(ecs* world, const BlockLinks* blocks, byte voxel) {
    if (!voxel) {
        return 0;
    }
    entity block = blocks->value[voxel - 1];
    if (!zox_valid(block)) {
        return 0;
    }
    if (!zox_has(block, BlockCollider)) {
        return 1;
    }
    return zox_getv(block, BlockCollider) != zox_block_air;
}

// Remembers the last chunk we looked at. Neighbouring voxels almost always share
// a chunk, so this skips the hashmap lookup and the component fetches.
typedef struct {
    byte loaded; // cache holds a lookup for `position`
    byte found;  // a chunk with voxels exists at `position`
    byte depth;
    int3 position;
    entity chunk;
    const VoxelNode* octree;
} collision_chunk_cache;

// Returns 1 if there is a chunk with voxels at chunk_position (and fills the cache), else 0.
static inline byte collision_chunk_select(ecs* world, collision_chunk_cache* cache,
        const ChunkLinks* chunks, int3 chunk_position) {
    if (cache->loaded && int3_equals(cache->position, chunk_position)) {
        return cache->found;
    }
    cache->loaded = 1;
    cache->position = chunk_position;
    cache->chunk = int3_hashmap_get(chunks->value, chunk_position);
    entity chunk = cache->chunk;
    cache->found = zox_valid(chunk) && zox_has(chunk, VoxelNode);
    if (cache->found) {
        cache->octree = zox_get(chunk, VoxelNode);
        cache->depth = zox_getv(chunk, NodeDepth);
    }
    return cache->found;
}

// Everything the collision functions need to know about the terrain an entity is on.
typedef struct {
    ecs* world;
    const BlockLinks* blocks;
    const ChunkLinks* chunks;
    byte depth;        // terrain NodeDepth
    float block_scale; // terrain BlockScale
} collision_terrain;

typedef struct {
    float3 correction; // push-out to apply to the position
    byte hit_ground;   // touched the top face of a block
} collision_result;

static inline float collision_axis_get(float3 v, byte axis) {
    return axis == 0 ? v.x : (axis == 1 ? v.y : v.z);
}

static inline float* collision_axis_ptr(float3* v, byte axis) {
    return axis == 0 ? &v->x : (axis == 1 ? &v->y : &v->z);
}

static inline int collision_axis_get_int(int3 v, byte axis) {
    return axis == 0 ? v.x : (axis == 1 ? v.y : v.z);
}

static inline int* collision_axis_int(int3* v, byte axis) {
    return axis == 0 ? &v->x : (axis == 1 ? &v->y : &v->z);
}

// Smallest overlap axis, same tie order everywhere: x, then y, then z.
static inline byte collision_smallest_axis(float3 overlap) {
    if (overlap.x <= overlap.y && overlap.x <= overlap.z) {
        return 0;
    }
    if (overlap.y <= overlap.z) {
        return 1;
    }
    return 2;
}

// World position of block (0,0,0)'s lower corner. Block k on an axis spans
// [origin + k * scale, origin + (k + 1) * scale).
static inline float3 collision_block_origin(float block_scale) {
    const int3 zero = { 0, 0, 0 };
    return block_position_to_real_position(zero, block_scale);
}

// Which block a coordinate is in. Rounds DOWN, so negative coordinates land in the right
// block (a plain (int) cast puts -0.3 in block 0 instead of -1 and misses the ground below 0).
static inline int collision_block_index(float value, float origin, float block_scale) {
    const float f = (value - origin) / block_scale;
    int i = (int) f;
    if ((float) i > f) {
        i--;
    }
    return i;
}

static inline int3 collision_block_position(float3 position, float3 origin, float block_scale) {
    int3 block_position;
    block_position.x = collision_block_index(position.x, origin.x, block_scale);
    block_position.y = collision_block_index(position.y, origin.y, block_scale);
    block_position.z = collision_block_index(position.z, origin.z, block_scale);
    return block_position;
}

// Is there a solid voxel at this block position? Missing chunks count as open air.
// Used for every solidity check (hit voxels AND their neighbours), so they can't disagree.
static inline byte collision_block_solid(const collision_terrain* terrain, collision_chunk_cache* cache,
        int3 block_position) {
    ecs* world = terrain->world;
    const int3 chunk_position = block_position_to_chunk_position(block_position, terrain->depth);
    if (!collision_chunk_select(world, cache, terrain->chunks, chunk_position)) {
        return 0;
    }
    const byte3 local_position = block_position_to_local_position(block_position, terrain->depth, cache->depth);
    const byte voxel = getv_VoxelNode(cache->octree, cache->depth, local_position);
    return collision_is_voxel_solid(world, terrain->blocks, voxel);
}

// Is the block containing this point solid?
static inline byte collision_point_solid(const collision_terrain* terrain, collision_chunk_cache* cache,
        float3 position) {
    const float3 origin = collision_block_origin(terrain->block_scale);
    return collision_block_solid(terrain, cache, collision_block_position(position, origin, terrain->block_scale));
}

// Does a box centred at `position` overlap any solid block? Faces it only touches don't count.
static byte collision_box_blocked(const collision_terrain* terrain, collision_chunk_cache* cache,
        float3 position, float3 half_extents) {
    const float block_scale = terrain->block_scale;
    const float3 origin = collision_block_origin(block_scale);
    const float3 shrink = float3_single(block_scale * 0.001f);
    const float3 lower = float3_add(float3_subtract(position, half_extents), shrink);
    const float3 upper = float3_subtract(float3_add(position, half_extents), shrink);
    const int3 block_min = collision_block_position(lower, origin, block_scale);
    const int3 block_max = collision_block_position(upper, origin, block_scale);
    int3 block_position;
    for (block_position.x = block_min.x; block_position.x <= block_max.x; block_position.x++) {
        for (block_position.y = block_min.y; block_position.y <= block_max.y; block_position.y++) {
            for (block_position.z = block_min.z; block_position.z <= block_max.z; block_position.z++) {
                if (collision_block_solid(terrain, cache, block_position)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Push-out
// ---------------------------------------------------------------------------

// Pushes a box centred at `position` out of every solid voxel it overlaps.
// Returns the correction, it does not move anything itself.
static collision_result collision_resolve(const collision_terrain* terrain, collision_chunk_cache* cache,
        float3 position, float3 half_extents) {
    const float block_scale = terrain->block_scale;
    const float half_block = block_scale * 0.5f;
    const float3 to_center = float3_single(half_block);
    const float3 origin = collision_block_origin(block_scale);

    collision_result result;
    result.correction = float3_zero;
    result.hit_ground = 0;

    // Integer range of blocks the box touches.
    const int3 block_min = collision_block_position(float3_subtract(position, half_extents), origin, block_scale);
    const int3 block_max = collision_block_position(float3_add(position, half_extents), origin, block_scale);

    int3 block_position;
    for (block_position.x = block_min.x; block_position.x <= block_max.x; block_position.x++) {
        for (block_position.y = block_min.y; block_position.y <= block_max.y; block_position.y++) {
            for (block_position.z = block_min.z; block_position.z <= block_max.z; block_position.z++) {
                if (!collision_block_solid(terrain, cache, block_position)) {
                    continue;
                }

                // Overlap on each axis (entity half extents + voxel half extents - centre distance)
                const float3 block_center = float3_add(block_position_to_real_position(block_position, block_scale), to_center);
                const float3 delta = float3_subtract(position, block_center);
                const float3 overlap = {
                    (half_extents.x + half_block) - float_abs(delta.x),
                    (half_extents.y + half_block) - float_abs(delta.y),
                    (half_extents.z + half_block) - float_abs(delta.z)
                };
                if (overlap.x <= 0 || overlap.y <= 0 || overlap.z <= 0) {
                    continue; // touching, not intersecting
                }

                // The smallest overlap axis decides which face we hit
                const byte axis = collision_smallest_axis(overlap);
                const int side = collision_axis_get(delta, axis) > 0 ? 1 : -1; // direction we get pushed
                const float push = (float) side * collision_axis_get(overlap, axis);

                // Buried face = internal seam or an edge graze. The neighbouring voxel owns
                // this contact, so SKIP the voxel. Do not fall back to another axis: that
                // axis' overlap is usually a whole block, which launches the entity on top of it.
                int3 neighbor = block_position;
                *collision_axis_int(&neighbor, axis) += side;
                if (collision_block_solid(terrain, cache, neighbor)) {
                    continue;
                }

                // Keep the largest push per axis
                float* current = collision_axis_ptr(&result.correction, axis);
                if (float_abs(push) > float_abs(*current)) {
                    *current = push;
                }
                if (axis == 1 && side > 0) {
                    result.hit_ground = 1; // standing on the top face of a block
                }
            }
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
// Sweep (fast movement)
// ---------------------------------------------------------------------------

// Moves a box centred at `position` along one axis by *distance, checking every voxel
// layer on the way, nearest first. Returns 1 and shortens *distance when something blocks:
// - a solid voxel ahead: stop at its face
// - a solid voxel the box is already in, on the side it's moving towards (judged the same
//   way as collision_resolve): get pushed back out of it, never carried deeper.
static byte collision_sweep_axis(const collision_terrain* terrain, collision_chunk_cache* cache,
        float3 position, float3 half_extents, byte axis, float* distance) {
    const float block_scale = terrain->block_scale;
    const float half_block = block_scale * 0.5f;
    float d = *distance;
    if (d == 0 || block_scale <= 0) {
        return 0;
    }
    const float skin = block_scale * 0.001f;
    // Absurdly long moves are cut short: the entity stops early instead of skipping anything.
    const float max_distance = 256.0f * block_scale;
    if (d > max_distance) d = max_distance;
    if (d < -max_distance) d = -max_distance;
    *distance = d;
    const int direction = d > 0 ? 1 : -1;
    const float3 origin = collision_block_origin(block_scale);
    const float3 to_center = float3_single(half_block);

    float3 lower = float3_subtract(position, half_extents);
    float3 upper = float3_add(position, half_extents);
    // Shrink the box a little on the other axes, so a face we are only resting on or
    // sliding along (the floor under us while moving sideways) doesn't block the move.
    if (axis != 0) { lower.x += skin; upper.x -= skin; }
    if (axis != 1) { lower.y += skin; upper.y -= skin; }
    if (axis != 2) { lower.z += skin; upper.z -= skin; }
    // The face that leads the move, then stretch the box along the axis to cover the whole path.
    const float leading = d > 0 ? collision_axis_get(upper, axis) : collision_axis_get(lower, axis);
    if (d > 0) {
        *collision_axis_ptr(&upper, axis) += d;
    } else {
        *collision_axis_ptr(&lower, axis) += d;
    }

    const int3 block_min = collision_block_position(lower, origin, block_scale);
    const int3 block_max = collision_block_position(upper, origin, block_scale);
    const byte axis_a = axis == 0 ? 1 : 0;
    const byte axis_b = axis == 2 ? 1 : 2;
    const int a_min = collision_axis_get_int(block_min, axis_a);
    const int a_max = collision_axis_get_int(block_max, axis_a);
    const int b_min = collision_axis_get_int(block_min, axis_b);
    const int b_max = collision_axis_get_int(block_max, axis_b);
    // Start at the trailing side: layers the box is already in come first, then layers ahead.
    const int first = d > 0 ? collision_axis_get_int(block_min, axis) : collision_axis_get_int(block_max, axis);
    const int last = d > 0 ? collision_axis_get_int(block_max, axis) : collision_axis_get_int(block_min, axis);

    int3 block_position = block_min;
    for (int layer = first; ; layer += direction) {
        *collision_axis_int(&block_position, axis) = layer;
        // The face of this layer we would run into
        const float layer_lower = collision_axis_get(origin, axis) + (float) layer * block_scale;
        const float face = d > 0 ? layer_lower : layer_lower + block_scale;
        const byte ahead = d > 0 ? (face >= leading - skin) : (face <= leading + skin);
        byte hit = 0;
        for (int a = a_min; a <= a_max && !hit; a++) {
            *collision_axis_int(&block_position, axis_a) = a;
            for (int b = b_min; b <= b_max; b++) {
                *collision_axis_int(&block_position, axis_b) = b;
                if (!collision_block_solid(terrain, cache, block_position)) {
                    continue;
                }
                if (ahead) {
                    hit = 1;
                    break;
                }
                // The box is already inside this layer. Like collision_resolve, it only counts
                // if this axis is the smallest overlap, the block is on the side we're moving
                // towards, and the face we'd be pushed out through isn't buried.
                const float3 block_center = float3_add(block_position_to_real_position(block_position, block_scale), to_center);
                const float3 delta = float3_subtract(position, block_center);
                const float3 overlap = {
                    (half_extents.x + half_block) - float_abs(delta.x),
                    (half_extents.y + half_block) - float_abs(delta.y),
                    (half_extents.z + half_block) - float_abs(delta.z)
                };
                if (overlap.x <= 0 || overlap.y <= 0 || overlap.z <= 0) {
                    continue;
                }
                if (collision_smallest_axis(overlap) != axis) {
                    continue; // side contact, collision_resolve deals with it
                }
                const float delta_axis = collision_axis_get(delta, axis);
                if (d > 0 ? delta_axis >= 0 : delta_axis <= 0) {
                    continue; // block is behind the centre, we're moving away from it
                }
                int3 neighbor = block_position;
                *collision_axis_int(&neighbor, axis) -= direction;
                if (collision_block_solid(terrain, cache, neighbor)) {
                    continue; // buried face
                }
                hit = 1;
                break;
            }
        }
        if (hit) {
            // Ahead: stop at its face. Already inside: pushed back out to its face.
            float allowed = face - leading;
            if (d > 0 ? allowed > d : allowed < d) {
                allowed = d;
            }
            *distance = allowed;
            return 1;
        }
        if (layer == last) {
            break;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// System
// ---------------------------------------------------------------------------

// NOTE: Detects intersections with voxel chunks
void terrain_collision_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Rotation3D);
    zox_sys_in(Bounds3D);
    zox_sys_out(Position3D);
    zox_sys_out(Velocity3D);
    zox_sys_out(Grounded);
    // NOTE: the same dt the physics steps used: zox_delta_time, not flecs' it->delta_time
    //  (that one isn't clamped to max_zox_delta_time on a lag spike, or overridden), and 0
    //  when the physics skipped the frame
    const float sweep_dt = zox_delta_time > zox_physics_max_delta_time ? 0 : (float) zox_delta_time;
    // a body under drag k moved v_end * g / keep this frame (physics_step: v_end = v keep, moved = v g)
    const physics_step ground_step = physics_step_new(ground_friction3, sweep_dt);
    const physics_step air_step = physics_step_new(air_friction3, sweep_dt);
    const float move_scale_ground = ground_step.keep > 0 ? ground_step.g / ground_step.keep : sweep_dt;
    const float move_scale_air = air_step.keep > 0 ? air_step.g / air_step.keep : sweep_dt;
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Rotation3D, rotation);
        zox_sys_i(Bounds3D, bounds);
        zox_sys_o(Position3D, position);
        zox_sys_o(Velocity3D, velocity);
        zox_sys_o(Grounded, grounded);
        entity terrain = zox_get_link(world, e, TerrainLink);
        if (!zox_valid(terrain) || !zox_has(terrain, ChunkLinks) || !zox_has(terrain, BlockScale)) {
            zox_logw("Terrain Invalid");
            continue; // these shouldn't be here
        }
        entity realm = zox_get_link(world, terrain, RealmLink);
        if (!zox_valid(realm) || !zox_has(realm, BlockLinks)) {
            zox_loge("Blocks Manager missing Terrain [%s]", zox_getn(terrain));
            continue;
        }
        const BlockLinks* blocks = zox_get(realm, BlockLinks);
        const byte terrain_depth = zox_getv(terrain, NodeDepth);
        const float block_scale = zox_getv(terrain, BlockScale);
        if (block_scale <= 0) {
            continue;
        }
        const float half_block = block_scale * 0.5f;
        zox_geter(terrain, ChunkLinks, chunks);
        const collision_terrain terrain_data = { world, blocks, chunks, terrain_depth, block_scale };
        const float3 half_extents = float4_rotate_bounds(rotation->value, bounds->value);

        // the move this frame, worked back from the velocity, no stored state (teleports are safe)
        //  - an exact step (see physics/fun) isn't velocity * dt when drag slows it mid frame:
        //      under drag alone it moved v_end * g / keep (exact), see move_scale below
        //  - forces (gravity) leave it off by up to |a| dt^2 / 2: 5cm at 10fps, 1.4mm at 60fps
        //      the start is guessed behind the real one when the force is along the move (a fall,
        //      the sweep covers a little more) and ahead when against it (rising in a jump), well
        //      within the push-out's reach (collision_step_scale * half a block)
        const byte dragged = zox_has(e, Frictioned) && zox_has(e, Acceleration3D);
        const float move_scale = dragged ? (grounded->value ? move_scale_ground : move_scale_air) : sweep_dt;
        const float3 move = float3_scale(velocity->value, move_scale);
        float move_max = float_abs(move.x);
        if (float_abs(move.y) > move_max) move_max = float_abs(move.y);
        if (float_abs(move.z) > move_max) move_max = float_abs(move.z);
        float min_half = half_extents.x;
        if (half_extents.y < min_half) min_half = half_extents.y;
        if (half_extents.z < min_half) min_half = half_extents.z;
        const float max_step = collision_step_scale * (half_block < min_half ? half_block : min_half);

        const float3 end = position->value;
        float3 pos = end;
        collision_chunk_cache cache = { 0 };
        byte hit_ground = 0;
        byte bounce[3] = { 0, 0, 0 };
        const byte fast = move_max > max_step;

        // Fast: the movement system may have carried the entity through a wall or floor.
        // Go back to where it started and redo the move one axis at a time, stopping at
        // the first solid face. y goes first, so a falling entity lands before it slides.
        if (fast) {
            pos = float3_subtract(pos, move);
            const byte sweep_order[3] = { 1, 0, 2 };
            for (int o = 0; o < 3; o++) {
                const byte axis = sweep_order[o];
                const float wanted = collision_axis_get(move, axis);
                if (wanted == 0) {
                    continue;
                }
                float distance = wanted;
                const byte blocked = collision_sweep_axis(&terrain_data, &cache, pos, half_extents, axis, &distance);
                *collision_axis_ptr(&pos, axis) += distance;
                if (blocked) {
                    if (axis == 1 && wanted < 0) {
                        hit_ground = 1; // landed on the top face of a block
                    }
                    bounce[axis] = 1; // we were moving into that face
                }
            }
        }
#if collision_debug
        const float3 swept = pos;
#endif

        // Push out of anything still overlapping (at normal speed this is the only step, as before).
        const collision_result result = collision_resolve(&terrain_data, &cache, pos, half_extents);
        const float3 c = result.correction;
        pos = float3_add(pos, c);
        if (result.hit_ground) {
            hit_ground = 1;
        }
        // Only bounce velocity that points INTO the surface we were pushed out of.
        if (c.x * velocity->value.x < 0) bounce[0] = 1;
        if (c.y * velocity->value.y < 0) bounce[1] = 1;
        if (c.z * velocity->value.z < 0) bounce[2] = 1;

        // Last line of defence: the entity's centre must never be inside a solid block.
        // If it is, it got in some other way (a teleport, a spawn inside terrain, or a movement
        // system that moved it further than velocity * dt). Walk it back the way it came,
        // a quarter block at a time, until its box is clear. If that fails, try straight up.
        if (collision_point_solid(&terrain_data, &cache, pos)) {
            const float step_length = block_scale * 0.25f;
            const float3 stuck = pos;
            byte freed = 0;
            for (int attempt = 0; attempt < 2 && !freed; attempt++) {
                float3 back = { 0, step_length, 0 }; // straight up
                if (attempt == 0) {
                    if (move_max <= 0) {
                        continue;
                    }
                    const float scale = step_length / move_max;
                    back.x = -move.x * scale;
                    back.y = -move.y * scale;
                    back.z = -move.z * scale;
                }
                pos = stuck;
                for (int n = 0; n < 64; n++) {
                    pos = float3_add(pos, back);
                    if (!collision_box_blocked(&terrain_data, &cache, pos, half_extents)) {
                        freed = 1;
                        break;
                    }
                }
            }
            if (freed) {
                const float3 fix = float3_subtract(pos, stuck);
                if (fix.x * velocity->value.x < 0) bounce[0] = 1;
                if (fix.y * velocity->value.y < 0) bounce[1] = 1;
                if (fix.z * velocity->value.z < 0) bounce[2] = 1;
            } else {
                pos = stuck;
            }
        }
        position->value = pos;

        // Ground state: grounded -> leaving -> air
        if (hit_ground) {
            grounded->value = ground_state_grounded;
        } else if (grounded->value == ground_state_grounded) {
            grounded->value = ground_state_leaving;
        } else if (grounded->value == ground_state_leaving) {
            grounded->value = ground_state_air;
        }

#if collision_debug
        zox_logw("collision [%llu] dt %.4f vel %.3f %.3f %.3f | end %.3f %.3f %.3f | fast %i swept %.3f %.3f %.3f | final %.3f %.3f %.3f | ground %i",
            (unsigned long long) e, sweep_dt,
            velocity->value.x, velocity->value.y, velocity->value.z,
            end.x, end.y, end.z,
            fast, swept.x, swept.y, swept.z,
            pos.x, pos.y, pos.z,
            hit_ground);
#endif

        if (bounce[0]) velocity->value.x *= -bounce_lost_force.x;
        if (bounce[1]) velocity->value.y *= -bounce_lost_force.y;
        if (bounce[2]) velocity->value.z *= -bounce_lost_force.z;
    }
    zox_sys_on_end();
} zoxd_system(terrain_collision_system);
