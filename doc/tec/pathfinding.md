# Pathfinding: Pathing Octrees and A*

A technical plan, for review before code. Npcs walk where they want to go (home, a target, a town) round terrain,
instead of straight at it.

## Why

- Npcs steer straight at a target: wander, attack, follow, and the home leash (`wander_system`) all set
  `TargetPosition`, `rotate_towards` and `move_forward` head at it.
- A hill, a wall or a cave between them leaves an npc stuck.
- Npcs should remember where they were and walk home (doc/todo.md, Characters and Npcs).

## The plan

1. **Pathing octrees:** each chunk keeps where an npc can stand, built from its voxels.
2. **A\*** over them, from an npc to its goal, giving a few waypoints.
3. **Npcs follow** the waypoints with the existing steering. Wander, attack, follow and home just set a goal.

## Pathing octrees

A chunk's walkable space, an octree like its voxels (`PathNode`), rebuilt when its voxels change.

- **Walkable:** air with a solid voxel under it, and enough air above for a character (2 voxels, a setting).
- **Values:** 0 blocked, 1 walkable, 2 a step up (a 1 voxel ledge, jumpable), 3 water later.
- **Coarse nodes:**
  - an open node whose children are all walkable closes into one walkable node, like voxel octrees do
  - a flat field is a few big nodes, not 4096 cells
  - A* works on nodes, so a long path over open ground is a few steps
- **Between chunks:** an edge cell's neighbour is in the next chunk. The nearby getter (`getv_nearby_*`, 27 chunks) already reads across.
- **Built:** on a worker after a chunk's voxels generate or change, from the voxel octree. Its lock is held while reading (doc/thread_safety.md).
- **Lods:** only full detail chunks near the player need it, the same ones npcs spawn in (`NpcSpawnZone`).

## Fields: a shared octree pulling and pushing

Alongside walkability, one shared field octree for everyone (`FieldNode`), coarse (a node a few metres):

- **Focal points pull:** towns, homes, water, a quest giver, light at night. Each adds a pull falling off with distance.
- **Dangers push:** monsters, cliffs, a fire, the player for cowards.
- **A node holds a direction and a strength**, the sum of the pulls and pushes over it. Many npcs read one field,
  cheaper than each searching.
- **Wandering** follows the field (towards the town at dusk, away from danger) instead of jitter, A* only when a goal
  is set (home, a target).
- **Updated** where something changes (a focal point placed or moved), not every frame.

## Npc memory: smarter npcs know more

- A smarter npc keeps its own small memory octree (`MemoryNode`): places it's seen, where it found food or danger,
  its own focal points (its house, its workplace).
- It reads its memory over the shared field: a guard knows the town's gates, a thief knows the back ways.
- Simple npcs (slimes, chickens) have none, they only read the shared field.
- Memory fades (strength decays), so it stays small.

## A*

- **Nodes:** pathing octree nodes (a node's centre, its size). Neighbours: nodes touching its sides, across chunks too.
- **Cost:** distance, and more for a step up. Water costs more later.
- **Heuristic:** straight line distance to the goal.
- **Limits:** at most N nodes expanded a search (a setting, say 512). No path within that is a fail: it walks straight, as now.
- **Out:** the waypoints, node centres on the ground, smoothed (skip a waypoint when the next is in straight walkable reach).
- **Where it runs:** a path request component on the npc (`PathGoal`), a system answers them on workers, a few a frame
  per worker (a time budget like `zox_sys_budget`). Waypoints go in `PathWaypoints` (an array component).

## Following

- **Steering:** a `path_follow_system` sets `TargetPosition` to the next waypoint. At it, the next.
- **Behaviours set goals:**
  - **wander:** a random walkable point near home (in the leash)
  - **attack and follow:** the target's position, repathed when it moves far
  - **home:** home
- **Stuck:** no progress for a few seconds, repath. Failing twice, it falls back to walking straight (and jumps, `Jumper`).
- **Remembering:** a despawned npc keeps where it was (its last position, a component on its home's record). Back in range,
  it spawns there and walks home.

## Steps, each tested

1. Pathing octrees for chunks. `tst path_octree`: a flat chunk is a few big walkable nodes, a wall blocks, a ledge is a step.
2. A* on one chunk, then across chunks. `tst path_search` (pure, made up octrees):
   - round a wall
   - up a step
   - no path out of a sealed room
   - a long path stays under the node limit
3. Npcs follow paths. `tst npc_path`: an npc walks round a wall to a goal it couldn't reach straight.
4. Behaviours use goals: wander, home, attack, follow.
5. Remembering where they were. `tst npc_return`: despawned away from home, it comes back there and walks home.

## Cost

- Memory: a byte a pathing node, sparse, only for chunks near the player.
- Cpu: building on voxel changes (on workers), and searches with a node limit and a per frame budget.
- Main thread: none. Requests and answers are components, read and written on workers.

## Open questions

- Characters of different heights (slimes 1 voxel, bigger beasts 3): one pathing octree for the tallest, or one per height?
- Flying and swimming npcs: their own walkable rules, later.
- Should towns keep their own coarse road graph for long trips between them, over A*?
- Field resolution: one node size everywhere, or finer near towns?
- Memory size limits per npc, and saving it with the npc.
