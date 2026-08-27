// temporarily disable until we work out event flow
/*zox_sys2(BonePaintSystem) {
    zox_sys_begin();
    zox_sys_in(BoneIndexes);
    zox_sys_out(MeshDirty);
    zox_sys_out(MeshColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(BoneIndexes, boneIndexes);
        zox_sys_o(MeshDirty, dirty);
        zox_sys_o(MeshColorRGBs, meshColorRGBs);
        if (dirty->value != mesh_state_skeleton_paint) {
            continue;
        }
        for (int j = 0; j < meshColorRGBs->length; j++) {
            const byte bone = boneIndexes->value[j];
            color_rgb bone_color = (color_rgb) { 255, 0, 0 };
            if (bone) bone_color.g = 255;
            bone_color.b += 16 * bone;
            bone_color.g -= 16 * bone;
            meshColorRGBs->value[j] = bone_color;
        }
        dirty->value = mesh_state_skeleton_end;
    }
} zox_sys_end(BonePaintSystem);*/
