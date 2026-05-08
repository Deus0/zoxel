// finds closest camera to a position
entity find_closest_camera(ecs *world, float3 position) {
    entity output = 0;
    float closest_distance = 100000;
    for (int j = 0; j < main_cameras_count; j++) {
        entity camera = main_cameras[j];
        if (!zox_valid(camera)) {
            continue;
        }
        float3 camera_position = zox_get_value(camera, Position3D)
        float distance = float3_distance(position, camera_position);
        if (distance < closest_distance) {
            closest_distance = distance;
            output = camera;
        }
    }
    return output;
}
