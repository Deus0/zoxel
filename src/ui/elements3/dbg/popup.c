void zox_dbg_spawn_popup3(ecs *world, ClickEventData data) {
    /*if (keycode != zox_key_j) {
        return;
    }*/

    zox_log("+ Testing [popup3]");
    zox_geter_value(dbg_player, CameraLink, entity, camera);
    zox_geter_value(camera, Position3D, float3, cposition);

    spawn_popup3_easy(world, "hello world", color_red, cposition,  2.5f, randf_range(4, 8));
}
