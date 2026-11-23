# Nodes

Node graphs: a process walks from node to node. Dialogue and quests use them, and models are built with them.

[src/nodes](../src/nodes)

## Pieces

| Entity | What it is |
|---|---|
| **node** | a step: a line of speech, a check (has quest?), an action (give quest) |
| **process** | walks a graph: it's at one node at a time (`NodeRun`) |

| Link | Kind | Means |
|---|---|---|
| `NodeLink` | many | a node to its next nodes (a branch has several), a process to its first node |
| `CurrentNodeLink` | one (exclusive) | the node a process is at |
| `NextNodeLink` | one (exclusive) | the node a process moves to next, chosen by a branch |

`NodeLink` **must** be many. When it was exclusive, every branch kept only the last node linked to it,
so has quest always took its true path (`tst node_graphs` checks this).

## A process's life

```text
spawned (PreInitialize) → Initialize → node_start_system: current = its first node, Begin
→ the node does its thing
→ TriggerEnd → next_node_system: current = NextNodeLink, or the node's first NodeLink, Begin
→ ...
→ no next node: TriggerExit → the process is deleted
```

- `Begin` is added each time a process moves to a node, with that node current. **Hook node events on
  `Begin`**, i.e. `zox_on_add(my_event, [none] nodes.NodeRun, [none] core.Begin)`.
- Don't hook them on `Update`. An add event only fires when the tag wasn't already there, and a process
  dirtied again keeps its `Update`, so the event is missed.
- Something ends a node by adding `TriggerEnd` to the **process** (not the node, nodes are shared by every process).
  Speech waits for the player's next press. Checks and actions end themselves:
  `delay_event(world, &delayed_node_end, process, 0.05)`.

## Branches

A branch node links its choices, tagged `NodeTrue` and `NodeFalse`. Its event picks one and links it as
the process's `NextNodeLink`:

```c
entity has_quest = spawn_node_has_quest(world, quest);
zox_link(world, has_quest, NodeLink, offer);        // tagged NodeFalse
zox_link(world, has_quest, NodeLink, already_done); // tagged NodeTrue
```

With no choice made, a process still moves on, to one of the node's links. Don't rely on which: branch nodes
should always choose.

## Adding a node type

1. A tag for it, and a spawn function that instances `prefab_node` (or a prefab child of it) with the tag.
2. An event on `Begin` that checks the process's current node has your tag, does its work, and ends the
   node (or picks a branch with `NextNodeLink`).
3. A test: build a small graph in a test and walk it, like `tst node_graphs` or `tst quest_dialogue`.

## Model nodes

A model blueprint is a node graph run on a model: `spawn_blueprint_models(world, parent, graph, seed, name, depth,
variants)` makes a model group and runs one process per variant, each with its own seed, so every node runs on every
variant. Nodes go between the first (size) and the last (end): more nodes there are modifiers (colours, eyes).

| Node | Does |
|---|---|
| size | the variant's box, a seeded ratio of the blueprint's length (`ModelSize` on the process) |
| colors | adds seeded colours: any, a fixed `Color`, or in a hue, saturation and value range (`ColorHSV`: dark metal, neon) |
| fill / box / paint | an ellipsoid or box of a voxel, or paints solid voxels |
| sprinkle / speckle | seeded dots on or in the surface |
| wfc | a new model from a source model's patches (wave function collapse), its source's colours |
| mutate | the model's colours by its seed: one hue turn for all, greys tint, each lighter or darker (`ColorMutation`) |
| traces | `Count` circuit traces of its voxel walked over the model's surface, seeded: straight runs turning a quarter, only where the surface stays facing the same way (eve/traces.c) |
| leds | lights of its voxel along the box's 12 edges, every `Count`'th, a step further each frame (`ModelFrame`): with `Count` frames they go round (eve/leds.c) |
| rune | a seeded 5 x 5 rune (a spine, arms, mirrored) of its voxel on the first solid voxel in from each face's middle, only on `ModelActive` frames (eve/rune.c) |
| end | marks the model built |

### Blocks from blueprints

`spawn_realm_block_blueprint(world, realm, graph, seed, name, color, dbg)` ([block_blueprint.c](../src/models/ins/block_blueprint.c)):
a solid block whose model is a blueprint run at `block_depth`, seeded by its realm, its 6 faces baked from it once
it's done (`spawn_realm_block_baked_model`, its textures wait on `Generating`). The gravity cube's
(`spawn_model_nodegraph_gravity_cube`): a dark metal box in 3 x 3 panels, neon edges, a core glowing through a
framed window on each face, neon traces and lights, lights chasing round its edges and a rune lit on its core while
it's on, its colours and rune seeded, so each realm's is its own. It's a block vox playing its two animations (below), not
baked; `spawn_realm_block_blueprint` (baked faces) stays for the base blocks.
Sizes in eighths of the blueprint, so they halve exactly to a 16 model. The base blocks are to move to blueprints.
To look at it: `zox_tst_model=gravity zox_tst_seed=<seed> bash bsh/tst.sh model_views`; tst gravity_cube_model.

### Animations: a VoxAnimation of frames

`spawn_blueprint_animation(world, parent, graph, seed, name, depth, frames, fps, active)` (ins/models.c): one graph,
many outputs. It makes a `VoxAnimation` (a model group, `AnimationFps`) whose children are its frames, each the graph
run on one, all from the same seed, each with its `ModelFrame`; `active` tags them `ModelActive`. Nodes read them:
`leds` chase round by the frame, `rune` lights on `ModelActive` frames, the rest make the same voxels on each.

A block with animations (`ModelAnimated`, gam/zoxel/sys/blocks.c's gravity cube) links one a state: its `ModelLink`
off, its `ActiveModelLink` while on in its chunk's `ActiveOctree`. It's a block vox; each placed one gets the tag
(vodes3 spawn) and `vode_animate_system` (vodes3/sys/animate.c) swaps its model and its mesh at its depth to its
state's animation's frame by the time (every one the same frame at once, one of each drawn at a time: the instance
renderer holds 64 meshes). A block vox's `BlockIndex` is its block's place in the realm's list (its voxel - 1).
To look at a frame: `zox_tst_model="gravity cube on" zox_tst_model_variant=<i> bash bsh/tst.sh model_views` (or
`off`). tst gravity_cube_model (the animations' frames), gravity_cube_vode (a placed one playing them).

### WFC (wave function collapse)

[src/models/fun/wfc.c](../src/models/fun/wfc.c) is pure C (`wfc_generate`), the node is
[src/models/eve/wfc.c](../src/models/eve/wfc.c) (an observer on `Begin`), the blueprint
`spawn_model_nodegraph_wfc(world, parent, source, rules, size_min, size_max, mutation)`: size, wfc, mutate (if
`mutation` isn't zero), end. Critters use a mutation of 60 degrees, 40 saturation and a fifth lighter or darker.

- the node: `ModelLink` its source model, `Shape3Size` the box (the source's, scaled by the size node), `WfcRules`
- patches are 2 wide and 3 tall (`wfc_patch_size`): 3 a side keeps the details but only remakes a small source, 3 tall
  lets a leg see it goes on up
- rules (`wfc_rule_*`): air outside the box, the floor, mirrored on x, one piece, anchored (a patch near where it is
  in the source), shaped (favours the source's voxel at the same share of the box, its features collapsed first),
  feet (solid under the source's feet, the piece standing on them), features (keeps every rare voxel: eyes, a beak,
  a comb, anywhere near)
- variety over accuracy: the anchor's slack is 0.2 of the box (at 0.05 they kept close to the source, at 0.1 features
  were lost); the features rule, not a tight anchor, keeps a face
- a contradiction tries the next seed (24 tries), then less of the feet, then the source stretched to the box
- critters: 4 variants each of `chicken.vox` (chooks) and `mrpenguin.vox` (pengus) in every realm,
  `spawn_realm_critter_wfc` (gam/zoxel/sys/characters.c). To look at them:
  `zox_tst_model=pengu zox_tst_model_variants=1 bash bsh/tst.sh model_views`
- each critter its own box range: pengus are never shorter than Mr Penguin (squashed, his face on top had no room)
- to do: a box much smaller than the source can end as the stretched source (no room for its features); a pengu can
  take up to a second a variant in a debug build (its tries lose a feature, then less of the feet is tried)

## Examples

- [gam/zoxel/sys/dialogues.c](../gam/zoxel/sys/dialogues.c): the quest giver's dialogue, speech with
  has quest, has done, has handed in branches, and give and hand in actions.
- [src/quests/eve/has.c](../src/quests/eve/has.c): a branch node's event.
- [src/quests/eve/action.c](../src/quests/eve/action.c): an action node's event.
- [src/dialogues/sys/ui.c](../src/dialogues/sys/ui.c): speech nodes shown in the dialogue window (only
  `SpeechNode`s, checks and actions aren't shown).

## Tests

| Test | Checks |
|---|---|
| `wfc` | outputs only use the source's voxels and patches (2, 3 and 2x3x2), seeded, and with every rule: a floor, mirrored, one piece on its feet |
| `node_graphs` | start, advance, branches keep their links, choosing a branch, the end, `Begin` once per node |
| `quest_dialogue` | a quest giver: offer, already commanded, slay, reward and hand in, done |
