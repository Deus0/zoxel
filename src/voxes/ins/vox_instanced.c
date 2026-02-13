entity spawn_vox_instanced(ecs *world, entity p, float3 position, byte rdisabled, byte rdepth, float scale) {

    zox_instance(p);
    zox_name("vox_instanced");

    zox_set(e, Position3D, { position });
    zox_set(e, TransformMatrix, { float4x4_position(position) });
    zox_set(e, RenderDepth, { rdepth });
    zox_set(e, RenderDisabled, { rdisabled });
    zox_set(e, BlockScale, { scale });

    return e;
}
