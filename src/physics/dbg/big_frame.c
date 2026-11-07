void zox_dbg_test_big_frame(
    ecs* world,
    ClickEventData data)
{
    zox_log("Testing Physics Big Frame");
    zox_log("   - [zox_delta_time = 1000;]");
    zox_delta_time_override_count = 200;
    zox_delta_time_override = zox_physics_max_delta_time;
}
