extern entity get_linked_character(ecs*, entity);
byte raycast_locks = 0;
// TODO: Handle out-of-bounds raycasting
// TODO: Raycast through character voxels too!
// NOTE: We Raycast Character Bounds | Terrain seperately
// NOTE: Characters Linked to Chunks, ones on bounds might overlap and confilct!
//  todo: seperate character raycast from terrain, and we just compare both distances

// Raycast through characters linked to chunks, excluding the caster itself
// Uses AABB intersection to find closest hit along the ray
CharacterRaycast raycast_character(ecs *world, float3 ray_origin, float3 ray_normal, entity caster, const ChunkEntities* entities) {
    CharacterRaycast ray = { .e = 0, .distance = FLT_MAX };
    if (!entities) {
        return ray;
    }
    for (int i = 0; i < entities->length; i++) {
        entity e = entities->value[i];
        if (!zox_valid(e) || caster == e || !zox_has(e, Position3D) || !zox_has(e, Bounds3D)) {
            continue;
        }
        zox_geter_value(e, Position3D, float3, position3);
        zox_geter_value(e, Rotation3D, float4, rotation3);
        zox_geter_value(e, Bounds3D, float3, bounds3);
        bounds character_bounds = {
            .center = position3,
            .extents = calculate_aabb_extents(bounds3, rotation3)
        };
        float tmin, tmax;
        if (point_in_aabb(ray_origin, character_bounds)) {
            tmin = 0.0f;
            tmax = FLT_MAX;
        } else if (!ray_intersects_aabb(ray_origin, ray_normal, character_bounds, &tmin, &tmax)) {
            continue;
        }
        if (tmin < ray.distance) {
            ray.distance = tmin;
            ray.point = float3_add(ray_origin, float3_scale(ray_normal, tmin));
            // data->normal = (int3) { 0, 0, 0 }; // You may want to calculate the actual normal at the intersection point
            ray.e = e;
        }
    }
    return ray;
}

// Update chunk info for current ray position, unlock previous chunk if locked
// Returns 0 if chunk is invalid or not loaded
// Updates character raycast only if no closer character hit found yet
byte update_chunk_for_raycast(
    ecs* world,
    entity* chunk,
    const VoxelNode** root_voctree,
    const ChunkLinks* chunk_links,
    int3 chunk_position,
    byte terrain_depth,
    float terrain_scalev,
    byte* chunk_depth,
    byte*  chunk_depth_reduction,
    byte3* chunk_size,
    float* chunk_scalev,
    CharacterRaycast* character_raycast,
    float3 ray_origin,
    float3 ray_normal,
    entity caster
) {
    // Terrain Pass
    if (chunk_links) {
        if (raycast_locks && *root_voctree) {
            read_unlock_VoxelNode(*root_voctree);
        }
        entity new_chunk = int3_hashmap_get(chunk_links->value, chunk_position);
        if (!zox_valid(new_chunk)) {
            return 0;
        }
        *chunk = new_chunk;
    }
    if (!zox_valid(*chunk)) {
        return 0;
    }
    *root_voctree = zox_get(*chunk, VoxelNode);
    if (!*root_voctree) {
        return 0;
    }
    if (raycast_locks) {
        read_lock_VoxelNode(*root_voctree);
    }
    *chunk_depth = zox_get_value(*chunk, RenderDepth);
    *chunk_size = byte3_single(powers_of_two[*chunk_depth]);
    *chunk_scalev = get_chunk_scale(*chunk_depth, terrain_depth, terrain_scalev);
    *chunk_depth_reduction = terrain_depth - *chunk_depth;
    // NOTE: Raycast Character per Terrain Chunk - Only do so when we havn't hit Character yet
    if (chunk_links && !character_raycast->e) {
        zox_geter(*chunk, ChunkEntities, entities);
        *character_raycast = raycast_character(world, ray_origin, ray_normal, caster, entities);
    }
    return 1;
}

// Main voxel raycast function using DDA traversal:
// - Traverses voxel chunks grid stepping through chunks dynamically
// - Handles recursive raycast into minivox subnodes
// - Compares character raycast hit vs terrain voxel hit and returns closest
// - Maintains chunk locking state for thread safety during voxel reads
// - Returns rayhit_* constants for type of hit or none
byte raycast_voxel_node(ecs *world,
    entity caster,
    const BlockLinks* voxels,
    const ChunkLinks* chunk_links,
    int3 chunk_position,
    float3 chunk_positionf,
    byte terrain_depth,
    int3 max_chunk_size,
    entity chunk,
    float3 ray_origin,
    float3 ray_normal,
    int3 hit_normal,
    float terrain_scalev,
    float ray_length,
    RaycastVoxelData* data,
    CharacterRaycast* character_raycast
) {
    if (!terrain_scalev) {
        zox_logw("terrain_scalev is 0, cannot divide by this");
        return 0;
    }
    // setup voxel data
    byte raycasting_terrain = voxels && voxels->length && chunk_links;
    byte3 max_chunk_sizeb3 = int3_to_byte3(max_chunk_size);
    const VoxelNode* root_voctree = NULL;
    VoxelNode* node_voxel = NULL;
    byte chunk_depth;
    byte chunk_depth_reduction; // terrain_depth - chunk_depth
    byte3 chunk_size;
    float chunk_scalev;
    float ray_distancef = 0;         // this is distance in voxel space
    // int3 hit_normal = int3_zero;
    byte result = 0;
    uint checks = 0;
    byte was_hitting = 0;
    // NOTE: This is called from Minivoxes
    if (zox_valid(chunk)) {
        if (!update_chunk_for_raycast(world, &chunk, &root_voctree, chunk_links, chunk_position, terrain_depth, terrain_scalev, &chunk_depth, &chunk_depth_reduction, &chunk_size, &chunk_scalev, character_raycast, ray_origin, ray_normal, caster)) {
            zox_log_error("Minivox Invalid Node");
            return 0;
        }
    }
    // position
    byte3 positionl;
    int3 positionv;
    byte3 positionl_last;
    int3 positionv_last;
    float3 positionf_last;
    entity chunk_last = chunk;
    VoxelNode *node_last = NULL;
    // zero for terrain raycasting
    float3 local_ray_origin = float3_subtract(ray_origin, chunk_positionf);
    // Convert Ray Origin to Terrain Local Voxel Position
    positionv = real_position_to_block_position(local_ray_origin, terrain_scalev);
    float3 ray_origin_scaled = float3_scale(local_ray_origin, 1.0f / terrain_scalev); // get float voxel position
    // NOTE: As positionv is terrain local, we need to increase our steps by chunk node reduction
    // Prepare stepping vectors for DDA traversal along the ray direction
    int3 step_direction = float3_to_int3(float3_sign(ray_normal));
    float3 ray_unit_size = (float3) {
        1.0f / float_abs(ray_normal.x),
        1.0f / float_abs(ray_normal.y),
        1.0f / float_abs(ray_normal.z)
    };
    // Calculate initial distances to voxel boundaries along each axis
    float3 ray_add = float3_zero;
    if (ray_normal.x < 0) {
        ray_add.x = (ray_origin_scaled.x - positionv.x);
    } else {
        ray_add.x = ((float) positionv.x + 1 - ray_origin_scaled.x);
    }
    if (ray_normal.y < 0) {
        ray_add.y = (ray_origin_scaled.y - (float) positionv.y);
    } else {
        ray_add.y = ((float) positionv.y + 1 - ray_origin_scaled.y);
    }
    if (ray_normal.z < 0) {
        ray_add.z = (ray_origin_scaled.z - (float) positionv.z);
    } else {
        ray_add.z = ((float) positionv.z + 1 - ray_origin_scaled.z);
    }
    float3_scale3_p(&ray_add, ray_unit_size);
    byte has_fetched = 0;
    // Loop stepping through voxels until ray length exceeded or max checks
    while (ray_distancef <= ray_length && checks < safety_checks_raycasting) {
        // Terrain!
        if (raycasting_terrain) {
            int3 new_chunk_position = block_position_to_chunk_position(positionv, terrain_depth);
            // Steps through chunks in our terrain!
            if (!has_fetched || !int3_equals(chunk_position, new_chunk_position)) {
                has_fetched = 1;
                // NOTE: we traverse through the void of space now
                update_chunk_for_raycast(world, &chunk, &root_voctree, chunk_links, new_chunk_position, terrain_depth, terrain_scalev, &chunk_depth, &chunk_depth_reduction, &chunk_size, &chunk_scalev, character_raycast, ray_origin, ray_normal, caster);
                chunk_position = new_chunk_position;
            }
            positionl = get_positionl_byte3(positionv, max_chunk_sizeb3);
            // NOTE: This fixes it for sub chunk nodes
            positionl = byte3_inverse_scale(positionl, (int) powers_of_two[chunk_depth_reduction]);
        }
        // Function Inside Minivoxes - Sub Entity Nodes
        else {
            if (int3_in_bounds(positionv, max_chunk_size)) {
                positionl = int3_to_byte3(positionv);
                if (!was_hitting) {
                    was_hitting = 1;
                }
            } else {
                positionl = (byte3) { 255, 255, 255 }; // failure!
                // return here ?
                if (was_hitting) {
                    if (raycast_locks && root_voctree) {
                        read_unlock_VoxelNode(root_voctree);
                    }
                    return rayhit_none;
                }
            }
        }
        // Character Ray: if didnt hit voxel, use character hit
        if (character_raycast->e && character_raycast->distance <= ray_length) {
        // NOTE: For now we just pick first character if found as it was not the best before
        // if (character_raycast->e &&  character_raycast->distance <= ray_distancef) {
            data->chunk = character_raycast->e;
            data->hit = character_raycast->point;
            data->distance = character_raycast->distance;
            data->normal = ray_normal;
            if (raycast_locks && root_voctree) {
                read_unlock_VoxelNode(root_voctree);
            }
            return rayhit_character;
        }
        byte hit_voxel = 0;
        byte is_in_bounds = byte3_in_bounds(positionl, chunk_size);
        if (is_in_bounds) {
            // byte3 positionl_temp = positionl;
            VoxelNode* sub_octree = getm_VoxelNode((VoxelNode*)root_voctree, chunk_depth, positionl, 0);
            hit_voxel = sub_octree ? sub_octree->value : 0;
            node_voxel = sub_octree;
            // node_voxel = get_voxel_node_at_depth(&hit_voxel, root_voctree, &positionl_temp, chunk_depth);
        }
        if (is_in_bounds && hit_voxel) {
            byte block_index = hit_voxel - 1;
            if (!raycasting_terrain) {
                data->distance = ray_distancef;
                data->normal = int3_to_float3(hit_normal);
                if (raycast_locks && root_voctree) {
                    read_unlock_VoxelNode(root_voctree);
                }
                return rayhit_block_vox;
            }
            // safety!
            if (block_index >= voxels->length) {
                zox_logw("voxel index out of bounds [%i]", hit_voxel);
                if (raycast_locks && root_voctree) {
                    read_unlock_VoxelNode(root_voctree);
                }
                return rayhit_none;
            }
            entity hit_block = voxels->value[block_index];
            if (raycasting_terrain) {
                data->hit_block = hit_block;
            }
            // Raycast into minivox if hit voxel is marked as such, recurse deeper
            byte is_minivox = is_minivox = zox_has(hit_block, BlockVox);
            if (!is_minivox) {
                result = rayhit_terrain;
                break;
            }
            entity block_spawn = get_node_entity_VoxelNode(node_voxel);
            if (zox_valid(block_spawn) && zox_has(block_spawn, Position3D)) {
                // NOTE: Minivoxes are centred, so get cornered position, we offset
                zox_geter_value_non_const(block_spawn, Position3D, float3, block_position);
                float3 ray_point = float3_add(ray_origin, float3_scale(ray_normal, ray_distancef));
                float3_subtract_float3_p(&block_position, float3_single(0.5f * chunk_scalev));
                // model itself
                entity vox;
                // if Instanced mesh, use meta, otherwise use world block spawn!
                byte block_type = 0;
                if (zox_has(block_spawn, InstanceLink)) {
                    block_type = 1;
                    vox = zox_gett_value(block_spawn, InstanceLink);
                } else if (zox_has(hit_block, ModelLink)) {
                    block_type = 2;
                    vox = zox_gett_value(hit_block, ModelLink);
                } else {
                    vox = chunk;
                }
                if (!zox_valid(vox)) {
                    if (block_type == 1) {
                        // Could be spawning / invisible here
                        // zox_logw("Raycast Invalid Instance Vox [%s]  %i Block [%s]", zox_get_name(block_spawn), block_index, zox_get_name(hit_block));
                    } else if (block_type == 2) {
                        zox_loge("Raycast Invalid Unique Vox [%s] %i", zox_get_name(hit_block), block_index);
                    } else {
                        zox_loge("Raycast Invalid Vox %i",  block_index);
                    }
                    if (raycast_locks && root_voctree) {
                        read_unlock_VoxelNode(root_voctree);
                    }
                    return rayhit_none;
                }
                if (!zox_has(vox, NodeDepth)) {
                    zox_log_error("Raycast Error: Vox Missing NodeDepth [%s] [%s] [%s]", zox_get_name(vox), zox_get_name(block_spawn), zox_get_name(hit_block));
                    if (raycast_locks && root_voctree) {
                        read_unlock_VoxelNode(root_voctree);
                    }
                    return rayhit_none;
                }
                zox_geter_value(vox, RenderDepth, byte, minivox_render_depth);
                int minivox_chunk_length = powers_of_two[minivox_render_depth];
                int3 minivox_chunk_size = int3_single(minivox_chunk_length);
                float minivox_scalev = chunk_scalev * (1.0f / (float) minivox_chunk_length);
                float minivox_ray_length = minivox_chunk_length * 3;
                // Recursive Raycasting to Minivox!
                // TODO: Detect when outside of minivox during raycast and exit then
                // NOTE: If ray length left is not much, then we simply use that
                float ray_length_left = ray_length - ray_distancef;
                if (ray_length_left < minivox_ray_length) {
                    minivox_ray_length = ray_length_left;
                }
                result = raycast_voxel_node(world, caster, NULL, NULL, int3_zero, block_position, minivox_render_depth, minivox_chunk_size, vox, ray_point, ray_normal, hit_normal, minivox_scalev, minivox_ray_length, data, character_raycast);
                // We hit Grass!
                if (result == rayhit_block_vox) {
                    data->distance += ray_distancef;
                    break;
                }
                // Hitting NPC Through Minivox
                else if (result == rayhit_character) {
                    data->distance += ray_distancef;
                    data->hit = float3_add(ray_origin, float3_scale(ray_normal, data->distance));
                    if (raycast_locks && root_voctree) {
                        read_unlock_VoxelNode(root_voctree);
                    }
                    return rayhit_character;
                }
            }
        }
        // Advance ray along shortest axis crossing using DDA increments
        if (ray_add.x < ray_add.y && ray_add.x < ray_add.z) {
            ray_distancef = ray_add.x * terrain_scalev;
            ray_add.x += ray_unit_size.x;
            positionv.x += step_direction.x;
            if (step_direction.x >= 0) {
                hit_normal = int3_left;
            } else {
                hit_normal = int3_right;
            }
        } else if (ray_add.y < ray_add.z) {
            ray_distancef = ray_add.y * terrain_scalev;
            ray_add.y += ray_unit_size.y;
            positionv.y += step_direction.y;
            if (step_direction.y >= 0) {
                hit_normal = int3_down;
            } else {
                hit_normal = int3_up;
            }
        } else {
            ray_distancef = ray_add.z * terrain_scalev;
            ray_add.z += ray_unit_size.z;
            positionv.z += step_direction.z;
            if (step_direction.z >= 0) {
                hit_normal = int3_backward;
            } else {
                hit_normal = int3_forward;
            }
        }
        // NOTE: Cache last, used for placement of blocks
        positionl_last = positionl;
        positionv_last = positionv;
        positionf_last = voxel_to_real_position(positionv, terrain_scalev, chunk_scalev);
        chunk_last = chunk;
        node_last = node_voxel;
        // safety first - limit our function total
        checks++;
    }
    // If Hit terrain or block vox
    if (result == rayhit_terrain || (result == rayhit_block_vox && raycasting_terrain))
    {
        if (result == rayhit_terrain) {
            data->distance = ray_distancef; //  * chunk_scalev; //  terrain_scalev;
            data->normal = int3_to_float3(hit_normal);
        }
        // calculate our point off the distance
        data->hit = float3_add(ray_origin, float3_scale(ray_normal, data->distance));
        // used for quad position, position of voxel hit
        if (chunk_depth_reduction) {
            int3 chunk_positionv = int3_scale(chunk_position, powers_of_two[terrain_depth]);
            // we remove chunkpositionv off positionv
            int3 positionl2 = int3_sub(positionv, chunk_positionv);
            positionl2 = int3_div1(positionl2, (int) powers_of_two[chunk_depth_reduction]);
            // now we convert back to positionv, scale back, add to chunk voxel position
            positionv = int3_add(chunk_positionv, int3_scale(positionl2, powers_of_two[chunk_depth_reduction]));
        }
        data->positionf = voxel_to_real_position(positionv, terrain_scalev, chunk_scalev);
        data->positionl = positionl;
        data->positionv = positionv;
        data->voxel_scale = chunk_scalev;
        data->chunk = chunk;
        data->positionl_last = positionl_last;
        data->positionv_last = positionv_last;
        data->positionf_last = positionf_last;
        data->chunk_last = chunk_last;
        data->node = node_voxel;
        data->node_last = node_last;
        data->voxel = node_voxel ? node_voxel->value : 0;
    }
    // TODO: Remove this, make sure all users are checking result
    else if (result == rayhit_none) {
        clear_raycast_data(data);
    }
    if (raycast_locks && root_voctree) {
        read_unlock_VoxelNode(root_voctree);
    }
    return result;
}

// ECS system iterating entities with camera & voxel links to perform raycasting
// Sets up raycast parameters and executes raycast_voxel_node with proper inputs
// Stores result in RaycastVoxelData component for consumption by gameplay logic
zox_sys2(Chunk3RaycastSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TerrainLink);
    zox_sys_in(RaycastRange);
    zox_sys_out(RaycastVoxelData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TerrainLink, terrain);
        zox_sys_i(RaycastRange, raycastRange);
        zox_sys_o(RaycastVoxelData, data);
        if (!zox_valid(terrain->value) || !zox_has(terrain->value, RealmLink)) {
            continue;
        }
        float3 ray_origin;
        float3 ray_normal;
        if (zox_has(e, CameraLink)) {
            zox_geter_value(e, CameraLink, entity, camera);
            if (!zox_valid(camera)) {
                return;
            }
            ray_origin = zox_gett_value(camera, RaycastOrigin);
            ray_normal = zox_gett_value(camera, RaycastNormal);
        } else {
            ray_origin = zox_gett_value(e, Position3D);
            float4 rotation = zox_gett_value(e, Rotation3D);
            ray_normal = quaternion_to_normal(rotation);
        }
        zox_geter_value(terrain->value, RealmLink, entity, realm);
        if(!zox_valid(realm)) {
            continue;
        }
        zox_geter(realm, BlockLinks, voxels);
        zox_geter_value(terrain->value, BlockScale, float, terrain_scalev);
        zox_geter_value(terrain->value, NodeDepth, byte, terrain_depth);
        int3 chunk_dimensions = int3_single(powers_of_two[terrain_depth]);
        zox_geter(terrain->value, ChunkLinks, chunks);
        CharacterRaycast character_raycast = { 0 };
        float range = !debug_ray_big_range ? raycastRange->value : 128;
        data->result = raycast_voxel_node(world, e, voxels, chunks, int3_zero, float3_zero, terrain_depth, chunk_dimensions, 0, ray_origin, ray_normal, int3_zero, terrain_scalev, range, data, &character_raycast);
        data->depth = terrain_depth;
    }
} zox_sys_end(Chunk3RaycastSystem);
