typedef byte (*boot_zox)(ecs*, entity);
boot_zox boot_event;

void zox_apps_boot(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        run_hook_on_boot(world, e);
        // Our game hook (TODO: Move this to regular boot event)
        if (boot_event && boot_event(world, e) == EXIT_FAILURE) {
            zox_loge("[boot_event] failed");
        }
        if (dbg_log) {
            zox_log("App Booted [%s]", zox_sys_e_name);
        }
    }
}
