// NOTE: Spawns Realm Specific Blocks (Global ones)
void blocks_realm_spawn_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(Seed);
    // zox_sys_out(BlocksDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(Seed, seed);
        // zox_sys_o(BlocksDirty, dirty);
        if (state->value != zox_generate_realm_blocks) {
            continue;
        }
        uint seed_shift = 3690;
        uint seed_offset = seed_shift * 100; // make sure different to biome seeds
        // Bottom of Realm
        {
            lint block_seed = seed->value + (seed_offset+=seed_shift);
            color block_color = color_rgb_to_color(
                seed_to_color_rgb_range(
                    block_seed,
                    (float2) { 0, 360 },
                    (float2) { 25, 50 },
                    (float2) { 10, 40 }));
            // color obsidian_color = color_grayscale(rand_range(15, 35));
            // entity e2 = spawn_block_stone(world, e, 226661, "obsidian", obsidian_color);
            entity model = spawn_model_stone(world, e, block_seed, block_color);
            entity e2 = spawn_realm_block_solid(
                world,
                prefab_block_baked,
                e,
                block_seed,
                "obsidian",
                block_color,
                model,
                dbg_log);
            zox_add(e2, BlockObsidian);
            link_hit_sound(world, e2, e, &hit_sound_stone);
            zox_add(e2, BlockInvinsible);
        }
        // Crafted Items
        {
            // Bricks for Homes
            lint block_seed = seed->value + (seed_offset+=seed_shift);
            color bricks_color = color_rgb_to_color(
                seed_to_color_rgb_range(
                    block_seed,
                    (float2) { 0, 360 },
                    (float2) { 30, 70 },
                    (float2) { 25, 45 }));
            color cracks_color = color_rgb_to_color(
                seed_to_color_rgb_range(
                    block_seed + 100,
                    (float2) { 0, 360 },
                    (float2) { 0, 100 },
                    (float2) { 5, 15 }));
            // color bricks_color = color_grayscale(rand_range(50, 90));
            entity model = spawn_model_bricks(
                world,
                e,
                block_seed,
                bricks_color,
                cracks_color);
            entity e2 = spawn_realm_block_solid(
                world,
                prefab_block_baked,
                e,
                block_seed,
                "bricks",
                bricks_color,
                model,
                dbg_log);
            // entity e2 = spawn_block_bricks(world, e, "bricks", bricks_color, 232323);
            zox_add(e2, BlockBricks);
            link_hit_sound(world, e2, e, &hit_sound_stone);
            zox_set(e2, BlockHealth, { (float2) { 10, 16 } });
        }
        // === File Blocks ===
        // Decor Blocks
        {
            // Flowers for Gardens
            entity model = string_hashmap_get(files_hashmap_voxes, new_string_data("flower"));
            if (model) {
                byte max_render_depth = block_depth;
                entity vox = model;
                entity e2 = spawn_realm_block_model(
                    world,
                    e,
                    123361,
                    "rose",
                    color_red,
                    0,
                    max_render_depth,
                    model,
                    vox,
                    direction_up);
                zox_setv(e2, BlockLightPass, 1);
                link_hit_sound(world, e2, e, &hit_sound_leaves);
            }
        }
        spawn_dungeon_blocks(world, e);
        // Glow Stone, on homes' roofs (towns/sys/walls.c), a warm light, last: blocks before it keep their indices
        {
            lint block_seed = seed->value + (seed_offset+=seed_shift);
            const color glow_color = (color) { 255, 226, 150, 255 };
            entity model = spawn_model_stone(world, e, block_seed, glow_color);
            entity e2 = spawn_realm_block_solid(world, prefab_block_baked, e, block_seed, "glow stone", glow_color, model, dbg_log);
            zox_add(e2, BlockGlowStone);
            zox_setv(e2, BlockGlow, 85);    // its faces 2x white (1 + 85 / 255 * 3): its pale stone past white (it blooms), its cracks show
            zox_setv(e2, BlockLight, 230);
            link_hit_sound(world, e2, e, &hit_sound_stone);
        }
        // Furniture: models bigger than a block (doc/plans/multi_block_vox.md), after the others: their indices stay
        for (byte j = 0; j < 2; j++) {
            lint block_seed = seed->value + (seed_offset+=seed_shift);
            const color wood_color = color_rgb_to_color(seed_to_color_rgb_range(block_seed,
                (float2) { 15, 45 }, (float2) { 35, 65 }, (float2) { 30, 55 }));
            char* name = j ? "door" : "bed";
            const byte max_depth = block_depth_limits.y + 1;
            entity model = spawn_model_lods_generated(world, e, name, j ? vox_type_door : vox_type_bed, wood_color, max_depth, block_seed);
            entity e2 = spawn_realm_block_model(world, e, block_seed, name, wood_color, 1, max_depth, model, model, direction_front);
            zox_setv(e2, BlockSize, (j ? (int3) { 1, 2, 1 } : (int3) { 2, 1, 1 }));
            zox_add_id(e2, j ? zox_id(BlockDoor) : zox_id(BlockBed));
            if (j) {
                zox_setv(e2, BlockLightPass, block_light_pass_open);    // light through it only while it's open
                zox_setv(e2, BlockCollider, zox_block_open);            // walked through only while it's open
            }
            link_hit_sound(world, e2, e, &hit_sound_wood);
        }
        // the filler: the rest of a bed's or door's box, solid, drawn by its base, no item (items/fun/multi_block.c)
        {
            SpawnBlock filler_data = {
                .name = "filler",
                .prefab = prefab_block_base,
                .model_type = zox_block_vox,
                .tag = zox_id(BlockFiller),
                .color = color_white,
            };
            entity e2 = spawn_block_vox_meta(world, filler_data);
            zox_setv(e2, BlockLightPass, 1);   // a bed's: light passes (a door has its own filler)
            zox_set_parent(world, e2, e);
            link_hit_sound(world, e2, e, &hit_sound_wood);
        }
        // water: see through, walked through, its texture moves; last, the blocks before it keep their indices
        {
            lint block_seed = seed->value + (seed_offset+=seed_shift);
            const color water_color = (color) { 40, 110, 200, zox_water_alpha };
            entity e2 = spawn_realm_block_textured(world, e, prefab_texture_water, block_seed, "water", water_color);
            zox_add(e2, BlockWater);
            zox_setv(e2, BlockWaterLevel, zox_water_level_source);
            zox_setv(e2, BlockMaterial, zox_block_material_liquid);
            zox_setv(e2, BlockCollider, zox_block_air);
            zox_setv(e2, BlockLightPass, 1);
            link_hit_sound(world, e2, e, &hit_sound_water);
            // flowing water, a block a level (7 to 1, thinner as it spreads, src/terrains/voxel/sys/water_flow.c), after it: no
            //  texture of their own (liquid meshes bind the first liquid's, water's), no items
            for (byte level = zox_water_level_source - 1; level >= 1; level--) {
                char name[16];
                snprintf(name, sizeof(name), "water %i", level);
                entity e3 = zox_ins(world, prefab_block);
                set_ZoxName(world, e3, name);
                zox_set_unique_name(e3, name);
                zox_setv(e3, Seed, block_seed + level);
                zox_setv(e3, Color, water_color);
                zox_add(e3, BlockBaked);
                zox_add(e3, SingleTextureBlock);
                zox_add(e3, BlockWaterFlowing);
                zox_setv(e3, BlockWaterLevel, level);
                zox_setv(e3, BlockMaterial, zox_block_material_liquid);
                zox_setv(e3, BlockCollider, zox_block_air);
                zox_setv(e3, BlockLightPass, 1);
                zox_set_parent(world, e3, e);
                zox_link(world, e3, LiquidLink, e2);
                link_hit_sound(world, e3, e, &hit_sound_water);
            }
        }
        // the street lamp: 1 x 3 x 1 blocks, in towns beside roads (towns/sys/walls.c), its light seeded at its head
        //  there; last, the blocks before it keep their indices
        {
            lint block_seed = seed->value + (seed_offset+=seed_shift);
            const color metal_color = color_rgb_to_color(seed_to_color_rgb_range(block_seed,
                (float2) { 0, 360 }, (float2) { 5, 20 }, (float2) { 12, 22 }));
            const byte max_depth = block_depth_limits.y + 2;
            entity model = spawn_model_lods_generated(world, e, "street lamp", vox_type_lamp, metal_color, max_depth, block_seed);
            entity e2 = spawn_realm_block_model(world, e, block_seed, "street lamp", metal_color, 1, max_depth, model, model, direction_front);
            zox_setv(e2, BlockSize, ((int3) { 1, 3, 1 }));
            zox_setv(e2, BlockLight, 220);
            zox_setv(e2, BlockLightOffset, ((byte3) { 0, 2, 0 }));
            zox_add(e2, BlockLamp);
            link_hit_sound(world, e2, e, &hit_sound_stone);
        }
        // a door's filler: as the filler, but it stops light while its door's shut (block_filler_voxel picks it for
        //  blocks passing light so); last, the blocks before it keep their indices
        {
            SpawnBlock filler_data = {
                .name = "door filler",
                .prefab = prefab_block_base,
                .model_type = zox_block_vox,
                .tag = zox_id(BlockFiller),
                .color = color_white,
            };
            entity e2 = spawn_block_vox_meta(world, filler_data);
            zox_setv(e2, BlockLightPass, block_light_pass_open);
            zox_setv(e2, BlockCollider, zox_block_open);
            zox_set_parent(world, e2, e);
            link_hit_sound(world, e2, e, &hit_sound_wood);
        }
        // oil: a liquid as water is (it flows, it's swum in), dark and its own mesh (zox_block_material_oil), its
        //  flowing blocks after it; last, the blocks before it keep their indices
        {
            lint block_seed = seed->value + (seed_offset+=seed_shift);
            const color oil_color = (color) { 26, 22, 18, 230 };
            entity e2 = spawn_realm_block_textured(world, e, prefab_texture_water, block_seed, "oil", oil_color);
            zox_add(e2, BlockOil);
            zox_setv(e2, BlockWaterLevel, zox_water_level_source);
            zox_setv(e2, BlockMaterial, zox_block_material_oil);
            zox_setv(e2, BlockCollider, zox_block_air);
            zox_setv(e2, BlockLightPass, 1);
            link_hit_sound(world, e2, e, &hit_sound_water);
            for (byte level = zox_water_level_source - 1; level >= 1; level--) {
                char name[16];
                snprintf(name, sizeof(name), "oil %i", level);
                entity e3 = zox_ins(world, prefab_block);
                set_ZoxName(world, e3, name);
                zox_set_unique_name(e3, name);
                zox_setv(e3, Seed, block_seed + level);
                zox_setv(e3, Color, oil_color);
                zox_add(e3, BlockBaked);
                zox_add(e3, SingleTextureBlock);
                zox_add(e3, BlockWaterFlowing);
                zox_setv(e3, BlockWaterLevel, level);
                zox_setv(e3, BlockMaterial, zox_block_material_oil);
                zox_setv(e3, BlockCollider, zox_block_air);
                zox_setv(e3, BlockLightPass, 1);
                zox_set_parent(world, e3, e);
                zox_link(world, e3, LiquidLink, e2);
                link_hit_sound(world, e3, e, &hit_sound_water);
            }
        }
        // a gravity cube: a right click switches it on or off (ActiveOctree), on it pulls round it its way
        //  (src/terrains/voxel/gravity); last, the blocks before it keep their indices
        //  - its models a blueprint seeded by the realm (models/ins/block_blueprint.c), cyberpunk: two VoxAnimations
        //    played on each placed one (a block vox, ModelAnimated), off (lights chasing round) and on (its
        //    ActiveModelLink, a rune lit too)
        {
            lint block_seed = seed->value + (seed_offset+=seed_shift);
            const color gravity_color = (color) { 150, 70, 220, 255 };
            const entity graph = spawn_model_nodegraph_gravity_cube(world, e);
            const byte depth = block_depth_limits.y;
            const entity2 off = spawn_blueprint_animation(world, e, graph, block_seed, "gravity cube off", depth, gravity_cube_frames,
                gravity_cube_fps, 0);
            const entity2 on = spawn_blueprint_animation(world, e, graph, block_seed, "gravity cube on", depth, gravity_cube_frames,
                gravity_cube_fps, 1);
            entity e2 = spawn_realm_block_model(world, e, block_seed, "gravity cube", gravity_color, 1, depth, off.x, off.y,
                direction_front);
            zox_set_link(world, e2, ActiveModelLink, on.x);
            zox_add(e2, ModelAnimated);
            zox_add(e2, BlockGravity);
            zox_add(e2, BlockPointed);   // pulls the way its placer looked, any of 6
            link_hit_sound(world, e2, e, &hit_sound_stone);
        }
        zox_add(e, BlocksDirty);
        if (dbg_log) {
            zox_log("Realm [%s] Spawning [Blocks]", zox_get_name(e));
        }
    }
    zox_sys_on_end();
} zoxd_system(blocks_realm_spawn_system);
