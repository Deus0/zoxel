// tst: links_api, the claims in doc/links.md: zox_link / zox_unlink / zox_each_link and zox_on_unlink callbacks
//  - test relations: TstLink (single, exclusive) and TstLinks (many), both with an unlink callback
//  - a callback fires exactly once, with the right entity and target, when:
//      - a single link is replaced by linking again (with the old target)
//      - zox_unlink is called
//      - the linking entity is deleted
//      - the target is deleted (target is 0)
//      - the target is deleted by cascade (its parent was deleted)
//  - many links: count, iteration, get by index (0 past the end) and zox_unlink_all over 100 targets
//  - owned links (zoxd_nf_link_owned): the linking entity is deleted with its target
//  - zox_is_linked_any
//  - callbacks are queued and run next frame (unlink_system), so this runs over a few frames
//  - run: bash bsh/tst.sh links_api
zox_tag(TstLink);
zox_tag(TstLinks);
zox_tag(TstOwnedLink);

typedef struct {
    entity e;
    entity target;
    byte many;
} zox_tst_unlink_event;

#define zox_tst_unlink_events_max 512
zox_tst_unlink_event zox_tst_unlink_events[zox_tst_unlink_events_max];
int zox_tst_unlink_events_count = 0;

void on_unlink_TstLink(ecs* world, entity e, entity target) {
    (void) world;
    if (zox_tst_unlink_events_count < zox_tst_unlink_events_max) {
        zox_tst_unlink_events[zox_tst_unlink_events_count++] = (zox_tst_unlink_event) { e, target, 0 };
    }
}

void on_unlink_TstLinks(ecs* world, entity e, entity target) {
    (void) world;
    if (zox_tst_unlink_events_count < zox_tst_unlink_events_max) {
        zox_tst_unlink_events[zox_tst_unlink_events_count++] = (zox_tst_unlink_event) { e, target, 1 };
    }
}

static inline int zox_tst_unlink_events_for(entity e, entity target, byte many, byte match_target) {
    int count = 0;
    for (int i = 0; i < zox_tst_unlink_events_count; i++) {
        const zox_tst_unlink_event event = zox_tst_unlink_events[i];
        if (event.e == e && event.many == many && (!match_target || event.target == target)) {
            count++;
        }
    }
    return count;
}

#define zox_tst_links_many 100
int zox_tst_links_api_frame = 0;
entity zox_tst_links_api_e[8];  // replaced, unlinked, deleted source, deleted target, many
entity zox_tst_links_api_t[8];

byte zox_tst_links_api_ready(ecs* world) {
    zox_tst_waiting = "unlink callbacks to run";
    if (zox_tst_links_api_frame == 0) {
        for (int i = 0; i < 8; i++) {
            zox_tst_links_api_e[i] = zox_new();
            zox_tst_links_api_t[i] = zox_new();
        }
        entity* e = zox_tst_links_api_e;
        entity* t = zox_tst_links_api_t;
        // replaced: linking again swaps the target, callback gets the old one
        zox_link(world, e[0], TstLink, t[0]);
        zox_link(world, e[0], TstLink, t[1]);
        zox_tst_check(zox_get_link(world, e[0], TstLink) == t[1], "relinking didn't replace the target");
        zox_tst_check(zox_get_links_count(world, e[0], TstLink) == 1, "exclusive link has [%i] targets", zox_get_links_count(world, e[0], TstLink));
        // unlinked
        zox_link(world, e[1], TstLink, t[2]);
        zox_unlink(world, e[1], TstLink, t[2]);
        zox_tst_check(!zox_get_link(world, e[1], TstLink), "unlinked link still has a target");
        // set_link 0 removes
        zox_link(world, e[4], TstLink, t[4]);
        zox_set_link(world, e[4], TstLink, 0);
        zox_tst_check(!zox_get_link(world, e[4], TstLink), "set_link 0 didn't remove the link");
        // deleted source
        zox_link(world, e[2], TstLink, t[3]);
        zox_delete(e[2]);
        // deleted target
        zox_link(world, e[3], TstLink, t[5]);
        zox_delete(t[5]);
        zox_tst_check(!zox_get_link(world, e[3], TstLink), "link to a deleted target remains");
        // many
        entity targets[zox_tst_links_many];
        for (int i = 0; i < zox_tst_links_many; i++) {
            targets[i] = zox_new();
            zox_link(world, e[5], TstLinks, targets[i]);
        }
        zox_tst_check(zox_get_links_count(world, e[5], TstLinks) == zox_tst_links_many,
            "many links counts [%i], expected [%i]", zox_get_links_count(world, e[5], TstLinks), zox_tst_links_many);
        for (int i = 0; i < zox_tst_links_many; i += 2) {
            zox_unlink(world, e[5], TstLinks, targets[i]);
        }
        int visited = 0;
        int wrong = 0;
        zox_each_link(world, e[5], TstLinks, target) {
            visited++;
            // only odd targets remain
            byte odd = 0;
            for (int i = 1; i < zox_tst_links_many; i += 2) {
                odd |= targets[i] == target;
            }
            wrong += !odd;
        }
        zox_tst_check(visited == zox_tst_links_many / 2 && !wrong,
            "after unlinking half, iterated [%i] links ([%i] wrong), expected [%i]", visited, wrong, zox_tst_links_many / 2);
        zox_tst_check(zox_is_linked(world, e[5], TstLinks, targets[1]) && !zox_is_linked(world, e[5], TstLinks, targets[0]),
            "is_linked wrong after unlinking half");
        zox_tst_check(zox_get_link_index(world, e[5], TstLinks, 0) != 0, "get_link_index 0 returned nothing");
        zox_tst_check(!zox_get_link_index(world, e[5], TstLinks, zox_tst_links_many / 2), "get_link_index past the end returned a target");
        zox_tst_check(zox_is_linked_any(world, e[5], TstLinks) && !zox_is_linked_any(world, e[1], TstLink), "is_linked_any wrong");
        zox_unlink_all(world, e[5], TstLinks);
        zox_tst_check(!zox_is_linked_any(world, e[5], TstLinks), "is_linked_any after unlink_all");
        // owned: deleted with its target
        zox_link(world, e[6], TstOwnedLink, t[6]);
        zox_delete(t[6]);
        zox_tst_check(!zox_valid(e[6]), "owned link holder wasn't deleted with its target");
        // target deleted by cascade
        const entity child = zox_new();
        zox_set_parent(world, child, t[7]);
        zox_link(world, e[7], TstLink, child);
        zox_delete(t[7]);
        zox_tst_check(!zox_valid(child), "child wasn't deleted with its parent");
        zox_tst_check(!zox_get_link(world, e[7], TstLink), "link to a cascade deleted target remains");
        zox_tst_check(!zox_get_links_count(world, e[5], TstLinks), "unlink_all left [%i] links", zox_get_links_count(world, e[5], TstLinks));
        zox_tst_links_api_frame = 1;
        return 0;
    }
    // callbacks run at the start of the next frame
    zox_tst_links_api_frame++;
    return zox_tst_links_api_frame >= 3;
}

void zox_tst_links_api_check(ecs* world) {
    (void) world;
    entity* e = zox_tst_links_api_e;
    entity* t = zox_tst_links_api_t;
    zox_log("[tst] unlink callbacks [%i]", zox_tst_unlink_events_count);
    zox_tst_check(zox_tst_unlink_events_for(e[0], t[0], 0, 1) == 1, "replaced link: [%i] callbacks with the old target, expected 1", zox_tst_unlink_events_for(e[0], t[0], 0, 1));
    zox_tst_check(zox_tst_unlink_events_for(e[0], t[1], 0, 1) == 0, "replaced link: callback for the new target");
    zox_tst_check(zox_tst_unlink_events_for(e[1], t[2], 0, 1) == 1, "unlink: [%i] callbacks, expected 1", zox_tst_unlink_events_for(e[1], t[2], 0, 1));
    zox_tst_check(zox_tst_unlink_events_for(e[4], t[4], 0, 1) == 1, "set_link 0: [%i] callbacks, expected 1", zox_tst_unlink_events_for(e[4], t[4], 0, 1));
    zox_tst_check(zox_tst_unlink_events_for(e[2], t[3], 0, 1) == 1, "deleted source: [%i] callbacks, expected 1", zox_tst_unlink_events_for(e[2], t[3], 0, 1));
    entity deleted_target_got = 0;
    for (int i = 0; i < zox_tst_unlink_events_count; i++) {
        if (zox_tst_unlink_events[i].e == e[3]) {
            deleted_target_got = zox_tst_unlink_events[i].target;
        }
    }
    zox_tst_check(zox_tst_unlink_events_for(e[3], 0, 0, 1) == 1, "deleted target: [%i] callbacks with target 0, expected 1 (any target: [%i], got target [%llu] valid [%i], deleted was [%llu])",
        zox_tst_unlink_events_for(e[3], 0, 0, 1), zox_tst_unlink_events_for(e[3], 0, 0, 0),
        (unsigned long long) deleted_target_got, zox_valid(deleted_target_got), (unsigned long long) t[5]);
    zox_tst_check(zox_tst_unlink_events_for(e[5], 0, 1, 0) == zox_tst_links_many,
        "many links: [%i] callbacks, expected [%i]", zox_tst_unlink_events_for(e[5], 0, 1, 0), zox_tst_links_many);
    zox_tst_check(zox_tst_unlink_events_for(e[7], 0, 0, 1) == 1, "cascade deleted target: [%i] callbacks with target 0, expected 1", zox_tst_unlink_events_for(e[7], 0, 0, 1));
    zox_tst_check(zox_tst_unlink_events_for(t[0], 0, 0, 0) == 0, "callback for an entity that never linked");
}

void zox_components_tst_links(ecs* world) {
    zoxd_nf_link(TstLink);
    zox_on_unlink(on_unlink_TstLink, TstLink);
    zoxd_nf_links(TstLinks);
    zox_on_unlink(on_unlink_TstLinks, TstLinks);
    zoxd_nf_link_owned(TstOwnedLink);
}
