// tst: chunk_order, see through chunk meshes (water) are drawn far to near from the camera (render_chunk_order,
//  rendering3/sys/render_chunks.c), so water seen through water blends over it; opaque ones as they come
//  - no world: four meshes' matrices at known distances from a camera position
//  - run: bash bsh/tst.sh chunk_order
byte zox_tst_chunk_order_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_chunk_order_check(ecs* world) {
    (void) world;
    const float3 camera = render_camera_position;
    render_camera_position = (float3) { 0, 0, 0 };
    const TransformMatrix matrices[4] = {
        { float4x4_position((float3) { 2, 0, 0 }) },
        { float4x4_position((float3) { 0, 0, -9 }) },
        { float4x4_position((float3) { 0, 5, 0 }) },
        { float4x4_position((float3) { 1, 0, 1 }) },
    };
    int order[4];
    render_chunk_order(matrices, 4, 1, order);
    zox_log("[tst] [metric] chunk_order far to near [%i %i %i %i]", order[0], order[1], order[2], order[3]);
    zox_tst_check(order[0] == 1 && order[1] == 2 && order[2] == 0 && order[3] == 3,
        "see through meshes not far to near: [%i %i %i %i]", order[0], order[1], order[2], order[3]);
    render_chunk_order(matrices, 4, 0, order);
    zox_tst_check(order[0] == 0 && order[1] == 1 && order[2] == 2 && order[3] == 3,
        "opaque meshes reordered: [%i %i %i %i]", order[0], order[1], order[2], order[3]);
    render_camera_position = camera;
}
