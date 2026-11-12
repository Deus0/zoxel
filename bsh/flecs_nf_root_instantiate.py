import sys
p = sys.argv[1]
s = open(p).read()
if "ZOXEL PATCH nf root list" not in s:
    print("!! nf root instantiate needs the nf root list patch first (bsh/flecs_nf_root.py)"); sys.exit(1)
# ZOXEL PATCH nf root instantiate: instancing a prefab walked every non fragmenting record too (sparse_has on
#  the prefab for each of 14k), 38% of slow frames flying (npcs spawning), now the roots and the prefab's targets
if "ZOXEL PATCH nf root instantiate" in s:
    print("nf root instantiate: already patched")
else:
    head = "void flecs_instantiate_dont_fragment(\n    ecs_world_t *world,\n    ecs_entity_t base,\n    ecs_entity_t instance)\n{"
    start = s.index(head, s.index(head) + 1) if s.count(head) > 1 else s.index(head)
    end = s.index("\n}\n", start) + 3
    old = s[start:end]
    loop_start = old.index("    while (cur) {")
    inner_start = old.index("        if (cur->sparse && (!has_isa || !inherit)")
    inner_end = old.index("        cur = cur->non_fragmenting.next;\n    }\n}")
    inner = old[inner_start:inner_end]
    # the per record body as a helper, its early continue becomes a return
    inner = inner.replace("""                        cur = cur->non_fragmenting.next;
                        continue;""", """                        return;""")
    helper = """/* ZOXEL PATCH nf root instantiate: copies one non fragmenting id from a base to an instance */
static void flecs_instantiate_dont_fragment_one(
    ecs_world_t *world,
    ecs_component_record_t *cur,
    ecs_entity_t base,
    ecs_entity_t instance,
    bool has_isa)
{
    ecs_assert(cur->flags & EcsIdSparse, ECS_INTERNAL_ERROR, NULL);
    bool inherit = (cur->flags & EcsIdOnInstantiateInherit) != 0;
""" + "\n".join(l[4:] if l.startswith("    ") else l for l in inner.rstrip().split("\n")) + """
}

"""
    new = helper + """void flecs_instantiate_dont_fragment(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t instance)
{
    /* ZOXEL PATCH nf root instantiate: the roots, and the base's own targets of each relationship */
    ecs_record_t *instance_r = flecs_entities_get(world, instance);
    bool has_isa = ecs_table_get_type_index(
        world, instance_r->table, ecs_isa(base)) != -1;
    ecs_component_record_t *cur = world->cr_non_fragmenting_root_head;
    while (cur) {
        ecs_component_record_t *next = cur->non_fragmenting_root.next;
        if (cur->sparse && flecs_sparse_has(cur->sparse, base)) {
            if (!ecs_id_is_wildcard(cur->id)) {
                flecs_instantiate_dont_fragment_one(world, cur, base, instance, has_isa);
            } else if (ECS_IS_PAIR(cur->id) &&
                ECS_PAIR_FIRST(cur->id) != EcsWildcard)
            {
                ecs_entity_t stack_targets[32];
                ecs_entity_t *targets = stack_targets;
                int32_t count = 0;
                if (cur->flags & EcsIdExclusive) {
                    ecs_entity_t *tgt = flecs_sparse_get_t(
                        cur->sparse, ecs_entity_t, base);
                    if (tgt && *tgt) {
                        targets[count ++] = *tgt;
                    }
                } else {
                    ecs_type_t *type = flecs_sparse_get_t(
                        cur->sparse, ecs_type_t, base);
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
                    if (pair_cr) {
                        flecs_instantiate_dont_fragment_one(world, pair_cr, base, instance, has_isa);
                    }
                }
                if (targets != stack_targets) {
                    ecs_os_free(targets);
                }
            }
        }
        cur = next;
    }
}
"""
    s = s[:start] + new + s[end:]
    print("nf root instantiate: patched")
open(p, "w").write(s)
