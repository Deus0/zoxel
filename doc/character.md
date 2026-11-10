# Characters

Characters hold gameplay state only. Voxels, meshes, renderers and skeletons are **child entities** spawned from lower modules. Characters *use* them, they don't *extend* them.

---

## Entity Structure

```text
character (characters)          gameplay, transform, LOD state, VoxLink, RendererLink, SkeletonLink
 │
 ├─ renderer_child (rendering)   model characters: instances a lod mesh (InstanceLink)
 │
 ├─ vox_lods (models)            CharacterUnique: a cloned copy of its model (VoxLink)
 │   └─ vox_mesh per depth       its own lod meshes, instanced by renderer_child
 │
 ├─ vox_combined (models)        skeleton characters: body parts combine into this (VoxLink)
 │
 ├─ skeleton (bones)             SkeletonLink: Skeleton, BoneLinks, animation state
 │   ├─ bone (root)              chest bone, child bones nested below it
 │   │   └─ ... head bone        the first person camera is parented here
 │   └─ skeleton_mesh per depth  ActiveMesh / PreparingMesh links on the skeleton
 │
 └─ body slots (parts)           only on skeleton characters, from prefab_character3_skeleton
```

### Character Kinds

| Kind | Marker | Children |
|---|---|---|
| Instanced | has `ModelLink` | `renderer_child` instancing the model's `ModelLods` |
| Unique | `CharacterUnique` tag (+ `ModelLink`) | cloned `vox_lods` + `renderer_child` instancing the clone's lods |
| Skeleton | `SkeletonOwner` tag | `vox_combined` + `skeleton` (bones and skeleton meshes), linked with `SkeletonLink` |

- There is one base prefab, `prefab_character3`. Skeleton characters use `prefab_character3_skeleton`, a child of it that only adds the `SkeletonOwner` tag.
- Gameplay variants derive from these: `prefab_character3_npc`, `prefab_character3_skeleton_npc`, `prefab_character3_player`.
- Unique vs instanced is data, not a prefab. Gam adds `CharacterUnique` to realm metas when `is_characters_instanced` is 0.

---

## Where Things Live

| Module | Provides |
|---|---|
| rendering | `renderer_child` prefab, `spawn_renderer_child()`, `RendererLink`, skeleton render system |
| chunks3 | `VoxLink`, `build_vox_mesh_at_depth()`, `CombineVoxSystem`, `CloneVoxSystem` |
| physics | `grow_bounds3()`, `VoxLinkBounds3GrowSystem` (bounds from a `VoxLink` child) |
| models | `vox_data`, `vox_lods`, `vox_mesh`, `vox_combined` prefabs, `vox_children.c` helpers, `RendererLodSystem`, `get_model_bounds()` |
| bones | `skeleton` + `skeleton_mesh` prefabs, `spawn_skeleton()`, `get_skeleton()`, skeleton mesh systems, bone weights |
| characters | `character3` prefabs, `CharacterChildrenSpawnSystem` (decides which children to spawn) |
| parts | body slots, `BodyCombineSystem` (into the vox child), `CharacterBoneSpawnSystem` (into the skeleton) |

Module order matters: `chunks → physics → models → bones → characters → parts`. A lower module must never reference a higher one. If characters needs something render related, add a helper to a lower module and call it.

---

## Lifecycle

1. `spawn_character3()` instances a prefab, sets transform, `ModelLink`, `RenderDepth`, `MaxRenderDepth`, and bounds from the model.
2. `GenerateCharacter` goes active → `CharacterChildrenSpawnSystem` (`zoxp_spawn`) spawns the children:
   - `SkeletonOwner` → `spawn_vox_combined_child()` + `spawn_skeleton()`
   - `CharacterUnique` → `spawn_vox_cloned_child()` + `spawn_renderer_child()`
   - otherwise with a model → `spawn_renderer_child()`
3. Skeleton characters:
   - `CharacterBodySpawnSystem` spawns body slots and parts.
   - `BodyCombineSystem` fills the vox child's `CombineList`, sets `MaxRenderDepth`, and sets `RenderDepth` from chunk distance.
   - `CombineVoxSystem` builds the voxels → `VoxelNodeDirty` → `ChunkUpdate` on the vox.
   - `CharacterBoneSpawnSystem` spawns bones under the skeleton → `SkeletonDirty`.
   - `SkeletonMeshesSystem` sees the vox `ChunkUpdate`, spawns or rebuilds a mesh for the depth, and swaps it in when built.
4. LOD: chunk systems set `RenderDepth` + `RenderDepthDirty` on the character.
   - `RendererLodSystem` sets the renderer's `InstanceLink`.
   - `SkeletonMeshesSystem` prepares or reuses a mesh for the new depth.

---

## Skeleton Meshes

- A mesh per render depth is spawned **only when needed**, then cached, like terrain chunk meshes.
- `PreparingMesh` builds while `ActiveMesh` renders. They swap when the prepared mesh is ready: `MeshBuilt` and no `BuildMesh` / `BuildMeshWeights` / `SkeletonMeshDirty` / `MeshDirty`.
- When voxels change, cached meshes are deleted and the active mesh rebuilds in place.
- Build pipeline: `BuildMesh` → `SkeletonMeshBuildSystem` → `BuildMeshWeights` → `BoneIndexGenerateSystem2` → `SkeletonMeshDirty` → bone upload (adds `MeshBuilt`) → `MeshDirty` → mesh upload.
- `SkeletonUpdate` (bones respawned) re-triggers weights on all of a skeleton's meshes.

---

## Tips

- **Never spawn children in `spawn_character3()`.** Realm meta characters are created with it and then made prefabs. Children would be left as orphans under a prefab. Spawn from a system, since systems skip prefabs.
- **Spawning entities must be `zox_system_1` in `zoxp_spawn`.** Flecs aborts in `flecs_new_id` when creating entities from multithreaded systems.
- **Deferred links:** values set with `zox_setv` / `zox_link` aren't readable until the next merge. Pass results along directly, e.g. `spawn_vox_cloned_child()` returns the lods for the renderer.
- **Render children get their transform from the hierarchy.** Give them `Position3D`, `Rotation3D` and `LocalPosition3D` / `LocalRotation3D` at zero. Entities without `Position3D` (vox children) stop the recursion, so don't put renderable things under them.
- **`RenderDisabled`:** use `set_entity_render_disabled()` (streaming) so children follow. Spawned children should copy their owner's value at spawn.
- **Reaching the skeleton:** use `get_skeleton(world, character)` for anything on the skeleton: hand/head/shoulder bones, `RaiseShoulder`, `SwingStart`, `WalkState`, `SkeletonDirty`.
- **Mesh build settings live on the vox.** Tags like `NoSplitChunk` must be on the vox child. `spawn_vox_combined_child()` copies it from the owner.
- **Waiting on children:** systems that need a child (`BodyCombineSystem`, `CharacterBoneSpawnSystem`) skip the entity until the link is valid, rather than consuming their dirty state.
- **Writing to a child from a multithreaded system** with `zox_getm` is fine when each child belongs to one owner. Structural changes still go through deferred commands.
- **Vox data `RenderDepth` is always max depth** (raycasts read it). The owner's `RenderDepth` picks which mesh to show.
- **Links are flecs pairs** (`zox_link`, `zox_set_link`, `zox_get_link`), declared non-fragmenting so they don't split tables. See [links.md](links.md):
  - `zoxd_nf_link`: one target, linking again replaces it (Exclusive).
  - `zoxd_nf_links`: many targets, iterate with `zox_each_link`, count with `zox_get_links_count`. Not kept in insertion order.
  - `zoxd_nf_link_owned`: the linking entity is deleted with its target (`UIHolderLink`: 3D UIs die with their character).
  - Links can't be used in system queries. Use a tag to select entities (i.e. `SkeletonOwner`, `DataUI`) and `zox_get_link` inside.
  - A link to a deleted entity is removed by flecs automatically. Use `zox_on_unlink` to react, see `on_unlink_ChunkLink`, `on_unlink_DataLink`.
- **Owned lists are children** ([children.md](children.md)): model variants are children of their `ModelGroup` (`get_model_variant()`), and notes are ordered children of their music.
- **Untested paths:** `CharacterUnique` (only used when `is_characters_instanced` is 0).
