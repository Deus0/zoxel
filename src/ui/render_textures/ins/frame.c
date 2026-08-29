// NOTE: Spawns a render texture + camera that targets an object
entity spawn_render_frame(
    ecs *world,
    entity parent,
    float2 anchor,
    int2 position,
    int2 size,
    entity target,
    float3 camera_position,
    float4 camera_rotation)
{
    byte is_camera_filtering = 1;
    float downscale = 4;
    color background = (color) { 155, 155, 155, 0 };
    byte layer = 0; // max_layers2D - 10;
    int2 texture_size = int2_scale1(size, 1 / downscale);
    entity material = spawn_material_render_texture(world, 1);
    // TODO: spawn_render_camera instead
    entity camera = spawn_camera(
        world,
        prefab_camera3,
        camera_position,
        camera_rotation,
        0,
        45,
        int2_zero,
        texture_size,
        single_screen_to_canvas);
    zox_set_unique_name(camera, "dbg_render_texture_camera");
    zox_add(camera, RenderCamera);
    zox_setv(camera, Color, background);
    zox_setv(camera, CameraVignette, 0);
    zox_setv(camera, CameraBlur, 0);
    // Create Render Texture
    entity ui = spawn_render_texture(
        world,
        prefab_render_texture,
        parent,
        anchor,
        position,
        size,
        texture_size,
        layer,
        camera,
        material);
    zox_set_unique_name(ui, "dbg_render_texture");
    zox_set_parent(world, material, ui);
    zox_setv(ui, Alpha, 1);
    zox_add(ui, RenderTextureAlpha);
    // NOTE: Spawns material for unique properties
    // Links
    zox_setv(ui, MaterialLink, material);
    zox_setv(camera, MaterialLink, material);
    // Set down tree?
    if (is_camera_filtering) {
        zox_add(camera, CameraFilter);
        zox_setv(target, CameraRenderer, camera);
    }
    return ui;
}

entity spawn_render_frame_at(
    ecs *world,
    entity parent,
    float2 anchor,
    int2 position,
    int2 size,
    entity target,
    float3 target_position)
{
    float4 camera_rotation = float4_identity;
    // float3 target_position = zox_getv(target, Position3D);
    float3 camera_position = float3_add(target_position, quaternion_rotate_vector(camera_rotation, (float3) { 0, 0, 1.5f }));
    return spawn_render_frame(
        world,
        parent,
        anchor,
        position,
        size,
        target,
        camera_position,
        camera_rotation);
}
