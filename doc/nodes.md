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
| colors | adds seeded colours |
| fill / box / paint | an ellipsoid or box of a voxel, or paints solid voxels |
| sprinkle / speckle | seeded dots on or in the surface |
| wfc | a new model from a source model's patches (wave function collapse), its source's colours |
| end | marks the model built |

### WFC (wave function collapse)

[src/models/fun/wfc.c](../src/models/fun/wfc.c) is pure C (`wfc_generate`), the node is
[src/models/eve/wfc.c](../src/models/eve/wfc.c) (an observer on `Begin`), the blueprint
`spawn_model_nodegraph_wfc(world, parent, source, rules, size_min, size_max)`.

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
