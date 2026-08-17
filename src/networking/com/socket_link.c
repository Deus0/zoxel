/*ECS_DTOR(SocketLink, ptr, {
    if (ptr->value != -1) {
        if (close(ptr->value) < 0) {
            perror(" ! [SocketLink] failed to close");
        }
        ptr->value = -1;
    }
})*/

void on_destroyed_SocketLink(iter *it) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(SocketLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(SocketLink, component);
        if (component->value < 0) {
            continue;
        }
        if (dbg_log) {
            zox_log("[SocketLink] Disposing [%i]", component->value);
        }
        if (close(component->value) < 0) {
            perror("[SocketLink] failed to close");
        }
        component->value = -1;
    }
}
