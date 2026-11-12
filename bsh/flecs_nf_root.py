# ZOXEL PATCH nf root list: deleting an entity walked every non fragmenting component record in the world
#  (14k with pair links, one per target) for each entity deleted, 100ms+ merges when chunks and npcs went.
#  A second list holds the roots (components, (R, *) wildcards, pairs whose relationship keeps no targets),
#  an entity's pairs come from its wildcard's sparse targets, so a delete visits tens of records, not 14k.
import sys
p = sys.argv[1]
s = open(p).read()
if "ZOXEL PATCH nf root list" in s:
    print("nf root list: already patched"); sys.exit(0)
def rep(a, b, count=1):
    global s
    assert s.count(a) >= 1, a[:80]
    s = s.replace(a, b, count)
rep("    ecs_id_record_elem_t non_fragmenting;\n",
    "    ecs_id_record_elem_t non_fragmenting;\n    ecs_id_record_elem_t non_fragmenting_root; /* ZOXEL PATCH nf root list */\n    bool non_fragmenting_is_root;\n")
rep("    ecs_component_record_t *cr_non_fragmenting_head;\n",
    "    ecs_component_record_t *cr_non_fragmenting_head;\n    ecs_component_record_t *cr_non_fragmenting_root_head; /* ZOXEL PATCH nf root list */\n")
rep("""    cr->non_fragmenting.next = world->cr_non_fragmenting_head;
    world->cr_non_fragmenting_head = cr;
""", """    cr->non_fragmenting.next = world->cr_non_fragmenting_head;
    world->cr_non_fragmenting_head = cr;

    /* ZOXEL PATCH nf root list: a pair whose (R, *) keeps its targets is reached through it */
    bool is_child_pair = ECS_IS_PAIR(cr->id) && !ecs_id_is_wildcard(cr->id) &&
        cr->pair && cr->pair->parent && cr->pair->parent->sparse;
    if (!is_child_pair) {
        cr->non_fragmenting_is_root = true;
        if (world->cr_non_fragmenting_root_head) {
            world->cr_non_fragmenting_root_head->non_fragmenting_root.prev = cr;
        }
        cr->non_fragmenting_root.next = world->cr_non_fragmenting_root_head;
        world->cr_non_fragmenting_root_head = cr;
    }
""")
rep("""    if (world->cr_non_fragmenting_head == cr) {
        world->cr_non_fragmenting_head = cr->non_fragmenting.next;
    }
""", """    if (world->cr_non_fragmenting_head == cr) {
        world->cr_non_fragmenting_head = cr->non_fragmenting.next;
    }

    /* ZOXEL PATCH nf root list */
    if (cr->non_fragmenting_is_root) {
        if (world->cr_non_fragmenting_root_head == cr) {
            world->cr_non_fragmenting_root_head = cr->non_fragmenting_root.next;
        }
        if (cr->non_fragmenting_root.prev) {
            cr->non_fragmenting_root.prev->non_fragmenting_root.next = cr->non_fragmenting_root.next;
        }
        if (cr->non_fragmenting_root.next) {
            cr->non_fragmenting_root.next->non_fragmenting_root.prev = cr->non_fragmenting_root.prev;
        }
        cr->non_fragmenting_is_root = false;
    }
""")
old = s[s.index("void flecs_entity_remove_non_fragmenting(\n    ecs_world_t *world,\n    ecs_entity_t e,\n    ecs_record_t *r)\n{"):]
old = old[:old.index("\n}\n") + 3]
new = """/* ZOXEL PATCH nf root list: removes one non fragmenting id from an entity */
static void flecs_entity_remove_non_fragmenting_one(
    ecs_world_t *world,
    ecs_component_record_t *cr,
    ecs_record_t *r)
{
    ecs_type_t type = { .count = 1, .array = &cr->id };
    flecs_emit(world, world, &(ecs_event_desc_t) {
        .event = EcsOnRemove,
        .ids = &type,
        .table = r->table,
        .other_table = r->table,
        .offset = ECS_RECORD_TO_ROW(r->row),
        .count = 1,
        .observable = world
    });
    flecs_component_sparse_remove(
        world, cr, r->table, ECS_RECORD_TO_ROW(r->row));
}

void flecs_entity_remove_non_fragmenting(
    ecs_world_t *world,
    ecs_entity_t e,
    ecs_record_t *r)
{
    if (!r) {
        r = flecs_entities_get(world, e);
    }

    if (!r || !(r->row & EcsEntityHasDontFragment)) {
        return;
    }

    /* ZOXEL PATCH nf root list: the roots, and the entity's own targets of each relationship */
    ecs_component_record_t *cur = world->cr_non_fragmenting_root_head;
    while (cur) {
        ecs_component_record_t *next = cur->non_fragmenting_root.next;
        ecs_assert(cur->flags & EcsIdSparse, ECS_INTERNAL_ERROR, NULL);
        if (cur->sparse && flecs_sparse_has(cur->sparse, e)) {
            if (!ecs_id_is_wildcard(cur->id)) {
                flecs_entity_remove_non_fragmenting_one(world, cur, r);
            } else if (ECS_IS_PAIR(cur->id) &&
                ECS_PAIR_FIRST(cur->id) != EcsWildcard)
            {
                /* copied, removing a pair removes its target from this */
                ecs_entity_t stack_targets[32];
                ecs_entity_t *targets = stack_targets;
                int32_t count = 0;
                if (cur->flags & EcsIdExclusive) {
                    ecs_entity_t *tgt = flecs_sparse_get_t(
                        cur->sparse, ecs_entity_t, e);
                    if (tgt && *tgt) {
                        targets[count ++] = *tgt;
                    }
                } else {
                    ecs_type_t *type = flecs_sparse_get_t(
                        cur->sparse, ecs_type_t, e);
                    if (type && type->count) {
                        count = type->count;
                        if (count > 32) {
                            targets = ecs_os_malloc_n(ecs_entity_t, count);
                        }
                        ecs_os_memcpy_n(targets, type->array, ecs_entity_t, count);
                    }
                }
                ecs_entity_t first = ECS_PAIR_FIRST(cur->id);
                for (int32_t i = 0; i < count; i ++) {
                    ecs_component_record_t *pair_cr = flecs_components_get(
                        world, ecs_pair(first, targets[i]));
                    if (pair_cr && pair_cr->sparse &&
                        flecs_sparse_has(pair_cr->sparse, e))
                    {
                        flecs_entity_remove_non_fragmenting_one(world, pair_cr, r);
                    }
                }
                if (targets != stack_targets) {
                    ecs_os_free(targets);
                }
            }
        }
        cur = next;
    }

    r->row &= ~EcsEntityHasDontFragment;
}
"""
s = s.replace(old, new, 1)
open(p, "w").write(s)
print("nf root list: patched")
