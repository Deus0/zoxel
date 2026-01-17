entity3 spawn_frame_user(ecs *world, SpawnFrame fdata, entity udata) {
    const entity3 e = spawn_frame(world, fdata);
    set_icon_from_user_data(world, e.x, e.y, udata);
    // set_icon_label_from_user_data_direct(world, e.z, userdata);
    zox_set_unique_name(e.x, "frame_user");
    zox_set_unique_name(e.y, "icon_user");
    zox_set_unique_name(e.z, "icon_label_user");
    return e;
}