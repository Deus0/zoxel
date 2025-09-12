entity spawn_popup3_easy(
    ecs *world,
    const char *text,
    const color text_color,
    const float3 position,
    float scale,
    float lifetime
) {
    const SpawnDataElement3D popup_spawn_data = {
        .prefab = prefab_popup3D,
        .base_color = (color) { 0, 0, 0, 0 }, // background color
    };
    Text3DData text_data = {
        .prefab = prefab_text3D, //popup3D,
        .text = text,
    };
    Zigel3DData zigel_data = {
        .prefab = prefab_zigel3D,
        .resolution = text3D_resolution,
        .font_thickness = text3D_fill_thickness,
        .font_outline = text3D_outline_thickness,
        .fill_color = text_color,
        .outline_color = (color) { 15, 15, 15, 255 }
    };
    SpawnDataPopup3 popup_data = {
        scale = scale,
        lifetime = lifetime,
    };
    const entity e = spawn_popup3(
        world,
        popup_spawn_data,
        text_data,
        zigel_data,
        popup_data
    );
    zox_set(e, Position3D, { position });
    float3 end = float3_add(position, float3_scale(float3_up, randf_range(0.1f, 0.15f)));
    lerp_to_position(
        world,
        e,
        0.02,
        randf_range(0.4f, 0.6f),
        position,
        end
    );
    return e;
}