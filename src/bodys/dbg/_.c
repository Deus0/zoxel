void zox_dbg_body_part(ecs* world, entity e) {
    if (!zox_valid(e)) {
        return;
    }
    entity vox = get_item_model(world, e);
    if (!zox_valid(vox)) {
        zox_log("Model [%s]: Invalid Vox", zox_get_name(e));
        return;
    }
    int3 model_size = zox_getv(vox, ChunkSize);
    byte model_depth = zox_getv(e, MaxRenderDepth);
    short model_length = octree_size(model_depth);
    byte slot_type = zox_getv(e, SlotType);
    zox_log("Body Part [%s] (%i):", zox_get_name(e), slot_type);
    zox_log("   Depth [%i] Length [%i]", model_depth, model_length);
    zox_log("   Size [%ix%ix%i]", model_size.x, model_size.y, model_size.z);
}
