// Generates furniture models: a bed (vox_type_bed), a door (vox_type_door) and a street lamp (vox_type_lamp),
//  models/gen/furniture.c
//  - colours from its seed: its fill is the wood, a darker wood, a pale sheet and pillow, a blanket in the wood's
//      complement, a gray metal
void furniture_model_generation_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_in(Color);
    zox_sys_in(VoxType);
    zox_sys_out(GenerateModel);
    zox_sys_out(VoxelNode);
    zox_sys_out(NodeDepth);
    zox_sys_out(ColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, seed);
        zox_sys_i(Color, fill);
        zox_sys_i(VoxType, gentype);
        zox_sys_o(GenerateModel, generate);
        zox_sys_o(VoxelNode, node);
        zox_sys_o(NodeDepth, depth);
        zox_sys_o(ColorRGBs, colors);
        if (generate->value != zox_generate_model_run ||
            (gentype->value != vox_type_bed && gentype->value != vox_type_door && gentype->value != vox_type_lamp))
        {
            continue;
        }
        if (gentype->value == vox_type_lamp) {
            // a lamp: its fill the dark metal, a lighter one, a warm glass
            color_rgb metal = color_to_color_rgb(fill->value);
            color_rgb_multiply_float(&metal, 1.6f);
            resize_ColorRGBs(colors, 0);
            add_to_ColorRGBs(colors, color_to_color_rgb(fill->value));
            add_to_ColorRGBs(colors, metal);
            add_to_ColorRGBs(colors, ((color_rgb) { 255, 226, 150 }));
            spinlock* lamp_lock = zox_octree_lock(world, e);
            zox_octree_lock_begin(lamp_lock);
            build_vox_lamp(node, depth->value);
            zox_octree_lock_end(lamp_lock);
            generate->value = zox_has(e, BakeModel) ? zox_generate_model_bake : zox_generate_model_end;
            zox_add(e, VoxelNodeDirty);
            continue;
        }
        zox_srand(seed->value);
        const color_rgb wood = color_to_color_rgb(fill->value);
        color_rgb wood_dark = wood;
        color_rgb_multiply_float(&wood_dark, fracture_dark_multiplier);
        float3 blanket = color_rgb_to_hsv(wood);
        blanket = hsv_shift(blanket, 180 + frand_range(-30, 30), 20, 10);
        resize_ColorRGBs(colors, 0);
        add_to_ColorRGBs(colors, wood);
        add_to_ColorRGBs(colors, wood_dark);
        add_to_ColorRGBs(colors, ((color_rgb) { 225, 222, 210 }));
        add_to_ColorRGBs(colors, ((color_rgb) { 245, 243, 235 }));
        add_to_ColorRGBs(colors, hsv_to_color_rgb(blanket));
        add_to_ColorRGBs(colors, ((color_rgb) { 150, 150, 160 }));
        // its octree locked while written, readers can run alongside (thread_safety.md)
        spinlock* write_lock = zox_octree_lock(world, e);
        zox_octree_lock_begin(write_lock);
        if (gentype->value == vox_type_bed) {
            build_vox_bed(node, depth->value);
        } else {
            build_vox_door(node, depth->value);
        }
        zox_octree_lock_end(write_lock);
        generate->value = zox_has(e, BakeModel) ? zox_generate_model_bake : zox_generate_model_end;
        zox_add(e, VoxelNodeDirty);
    }
    zox_sys_on_end();
} zoxd_system(furniture_model_generation_system);
