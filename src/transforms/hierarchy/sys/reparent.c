// Remove from old parent when cache changes
//      # Detects Modifications #
/*zox_sys2(ReparentSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ParentLink);
    zox_sys_out(OldParentLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ParentLink, new);
        zox_sys_o(OldParentLink, old);

        if (old->value == new->value) {
            continue;
        }

        if (zox_valid(old->value) && zox_has(old->value, Children)) {
            zox_mut_begin(old->value, Children, links);
            if (is_in_Children(links, e) && remove_from_Children(links, e)) {
                zox_modified(old->value, Children);
                // zox_log("+ removed [%s] from [%s]", zox_get_name(e), zox_get_name(old->value));
            }
        }

        old->value = new->value;
    }
} zox_sys_end(ReparentSystem);
*/
