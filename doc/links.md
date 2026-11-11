# Links

A link points one entity at another entity, i.e. a character at its skeleton, or a slot at its item.

Links are wrapped in `src/_/flecs/mcr/pair.c` and declared with macros from `src/_/flecs/mcr/ids.c`. Game code only uses the `zox_` wrappers. Never call `ecs_*` or use `Ecs*` ids outside `src/_/flecs`. If a wrapper is missing, add it there.

## Link or child?

| Use | When |
|---|---|
| **Link** | The entity points at something it doesn't own, i.e. `ChunkLink`, `DataLink`, `DeviceLink` |
| **Child** | The entity owns it and it should die with it, i.e. a character's skeleton mesh, notes in a music. See [children.md](children.md) |

## Declaring

Links are tags declared as non-fragmenting, so linking doesn't move entities between tables.

```c
// com/_.c
zox_tag(SkeletonLink);      // character -> its skeleton
zox_tag(ElementLinks);      // canvas -> its UI elements
zox_tag(UIHolderLink);      // 3D UI -> the character holding it

void zox_components_example(ecs* world) {
    zoxd_nf_link(SkeletonLink);         // one target
    zoxd_nf_links(ElementLinks);        // many targets
    zoxd_nf_link_owned(UIHolderLink);   // one target, deleted with its target
}
```

| Macro | Targets | Notes |
|---|---|---|
| `zoxd_nf_link(T)` | one | Linking again replaces the old target |
| `zoxd_nf_links(T)` | many | Not kept in insertion order |
| `zoxd_nf_link_owned(T)` | one | The linking entity is deleted when its target is |

## Single links

```c
// link, replaces any old target
zox_set_link(world, character, SkeletonLink, skeleton);

// get, returns 0 if not linked
entity skeleton = zox_get_link(world, character, SkeletonLink);
if (!zox_valid(skeleton)) {
    continue;   // child not spawned yet, try again next frame
}

// unlink, passing 0 removes the link
zox_set_link(world, character, SkeletonLink, 0);
```

## Many links

```c
// add targets
zox_link(world, canvas, ElementLinks, button);
zox_link(world, canvas, ElementLinks, label);

// remove one target
zox_unlink(world, canvas, ElementLinks, button);

// iterate, break and continue work as normal
zox_each_link(world, canvas, ElementLinks, element) {
    if (!zox_has(element, Button)) {
        continue;
    }
    // ...
}

// count
int count = zox_get_links_count(world, canvas, ElementLinks);

// get by index, returns 0 past the end
entity first = zox_get_link_index(world, canvas, ElementLinks, 0);

// remove them all
zox_unlink_all(world, canvas, ElementLinks);
```

## Checking links

```c
if (zox_is_linked_any(world, e, ModelLink)) { }         // linked to anything
if (zox_is_linked(world, e, ElementLinks, button)) { }  // linked to this target
```

## Reacting to unlinks

When a target is deleted, its links are removed automatically, so links never point at dead entities.

These claims are tested by `bash bsh/tst.sh links_api`, and cleanup in the ways the game deletes npcs by `bash bsh/tst.sh links_cleanup`.

> flecs v4.2.0 skipped this for recycled ids after an owned link deleted its holder (stale links, owned links not deleted). Patched in `bsh/flecs_fix.sh` ("ZOXEL PATCH cleanup"), so keep that patch when updating flecs, until it's fixed upstream.

Use `zox_on_unlink` to react whenever a link goes away, for any reason:

| Case | Callback |
|---|---|
| `zox_unlink` / `zox_set_link(.., 0)` | yes |
| Replaced by a new target (single links) | yes, with the old target |
| The linking entity is deleted | yes |
| The target is deleted, even by cascade | yes |

Deletion code never needs to know about links. Cleanup is handled in the flecs module (`src/_/flecs/fun/unlinks.c`).

```c
// Removes an entity from its old chunks ChunkEntities, when moved, unlinked or destroyed
void on_unlink_ChunkLink(ecs* world, entity e, entity chunk) {
    if (!zox_valid(chunk) || !zox_has(chunk, ChunkEntities)) {
        return;
    }
    ChunkEntities* entities = zox_getm(chunk, ChunkEntities);
    remove_from_ChunkEntities(entities, e);
}

void zox_components_chunks3(ecs* world) {
    zoxd_nf_link(ChunkLink);
    zox_on_unlink(on_unlink_ChunkLink, ChunkLink);  // after declaring the link
}
```

- Callbacks are queued and run once per frame at `zoxp_begin`, so they can add, remove and delete as normal.
- `e` may already be deleted (when the linking entity was deleted). Check `zox_valid(e)` before changing it.
- `target` is 0 if it was deleted.

See `on_unlink_ChunkLink` (`src/chunks/chunks3/com/_.c`) and `on_unlink_DataLink` (`src/slots/com/_.c`).

## Links in systems

Links can't be system query terms. Select entities with a tag instead, then get the link inside the loop:

```c
// sys/example.c
zox_sys2(ExampleSystem) {
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity skeleton = zox_get_link(world, e, SkeletonLink);
        if (!zox_valid(skeleton)) {
            continue;   // not spawned yet, don't consume dirty state
        }
        // ...
    }
}

// sys/_.c, SkeletonOwner is a tag on characters that have a SkeletonLink
zox_system(
    ExampleSystem,
    zoxp_update,
    [none] bones.SkeletonOwner
);
```

Existing tags used like this: `SkeletonOwner`, `DataUI`.

## Debugging

Uncomment `#define zoxl_links` at the top of `src/_/flecs/mcr/pair.c` to log every link and unlink.
