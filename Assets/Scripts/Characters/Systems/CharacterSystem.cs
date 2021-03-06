﻿using Unity.Entities;
using Unity.Mathematics;
using Unity.Transforms;
using Unity.Rendering;
using System.Collections.Generic;
using UnityEngine;
using Unity.Collections;
using Zoxel.Voxels;
using Zoxel.UI;

namespace Zoxel
{
    public enum CharacterSpawnType
    {
        Player,
        NPC,
        Turret,
        Networked
    }

    /// <summary>
    /// Needs to take in meta data for a character before spawning
    /// </summary>
    [DisableAutoCreation]
    public class CharacterSpawnSystem : ComponentSystem
    {
        // data
        public Dictionary<int, CharacterDatam> meta = new Dictionary<int, CharacterDatam>();
        public Dictionary<int, ClassDatam> classMeta = new Dictionary<int, ClassDatam>();
        public Dictionary<int, ItemDatam> items = new Dictionary<int, ItemDatam>();
        
        // spawned
        public Dictionary<int, Entity> characters = new Dictionary<int, Entity>();
        // prefabs
        private EntityArchetype npcArchtype;
        private Entity npcPrefab;
        private EntityArchetype playerArchtype;
        private Entity playerPrefab;
        // references
        public PlayerSpawnSystem playerSpawnSystem;
        public WorldSpawnSystem worldSpawnSystem;
        public SkillsSystem skillsSystem;
        public ActionbarSystem actionbarSpawnSystem;
        public InventoryUISpawnSystem inventoryUISpawnSystem;
        public StatbarSystem statbarSystem;
        public SaveSystem saveSystem;
        public CameraSystem cameraSystem;
        public GameStartSystem gameStartSystem;

        protected override void OnCreate()
        {
            base.OnCreate();
            playerArchtype = World.EntityManager.CreateArchetype(
                // tags
                typeof(ZoxID),
                typeof(Traveler),
                typeof(Character),
                typeof(NearbyCharacters),
                typeof(Equipment),
                typeof(Skeleton),
                // game
                typeof(Stats),
                typeof(Skills),
                typeof(BulletHitTaker),
                // Movement
                typeof(WorldBound),
                typeof(Body),
                typeof(BodyInnerForce),
                typeof(BodyForce),
                // typeof(BodyTorque),
                // Rendering
                typeof(Translation),
                typeof(Rotation),
                typeof(NonUniformScale),
                typeof(RenderMesh),
                typeof(LocalToWorld),
                typeof(Zoxel.Animations.Animator),
                // AI
                typeof(Targeter),
                // Player
                typeof(ChunkStreamPoint),
                typeof(Controller),
                typeof(CameraLink),
                // Player Only Game
                typeof(ItemHitTaker),
                typeof(Inventory),
                typeof(QuestLog)
            );
            npcArchtype = World.EntityManager.CreateArchetype(
                // tags
                typeof(ZoxID),
                //typeof(Traveler),
                typeof(Character),
                typeof(NearbyCharacters),
                typeof(Equipment),
                typeof(Skeleton),
                // game
                typeof(Stats),
                typeof(Skills),
                typeof(BulletHitTaker),
                // movement
                typeof(WorldBound),
                typeof(Body),
                typeof(BodyInnerForce),
                typeof(BodyForce),
                typeof(BodyTorque),
                // transform
                typeof(Translation),
                typeof(Rotation),
                typeof(NonUniformScale),
                // renderer
                typeof(RenderMesh),
                typeof(LocalToWorld),
                typeof(Zoxel.Animations.Animator),
                // AI
                typeof(Targeter),
                typeof(AIState),
                typeof(Mover),
                typeof(Wander)
            );
            playerPrefab = World.EntityManager.CreateEntity(playerArchtype);
            World.EntityManager.AddComponentData(playerPrefab, new Prefab { });
            npcPrefab = World.EntityManager.CreateEntity(npcArchtype);
            World.EntityManager.AddComponentData(npcPrefab, new Prefab { });
        }

        #region ActualSpawning

        private void SpawnNPCCharacters(SpawnCharacterCommand command)
        {
            NativeArray<Entity> entities = new NativeArray<Entity>(command.amount, Allocator.Temp);
            World.EntityManager.Instantiate(npcPrefab, entities);
            for (int i = 0; i < command.amount; i++)
            {
                Entity entity = entities[i];
                SetCharacter(entity, command.characterIDs[i], false, command.world, command.metaID, command.classID, command.clanID, command.position, command.creatorID);
                SetNPCCharacter(entity, command.characterIDs[i], command.metaID, command.position); // command.world, 
            }
        }

        private void SpawnPlayerCharacter(SpawnCharacterCommand command)
        {
            Entity entity = World.EntityManager.CreateEntity(playerArchtype);
            int id;
            bool isLoadingPlayer = false;
            if (command.characterID != 0)
            {
                id = command.characterID;
                isLoadingPlayer = true;
            }
            else
            {
                id = Bootstrap.GenerateUniqueID();
            }
            if (SetCharacter(entity, id, isLoadingPlayer, command.world, command.metaID, command.classID, command.clanID, command.position))
            {
                SetPlayerCharacter(entity, id, command.world, command.metaID, command.position);
                cameraSystem.ConnectCameraToCharacter(command.camera, entity);
                playerSpawnSystem.SetPlayerCharacter(entity, command.playerID);
                if (isLoadingPlayer)
                {
                    saveSystem.LoadPlayer(entity);
                }
                Entity gameEntity = command.game;
                var game = World.EntityManager.GetComponentData<Game>(gameEntity);
                game.AddPlayerCharacter(id);
                World.EntityManager.SetComponentData(gameEntity, game);
                worldSpawnSystem.OnAddedStreamer(entity, command.world);
            }
        }

        private void SpawnNPCCharacter(SpawnCharacterCommand command)
        {
            Entity entity = World.EntityManager.CreateEntity(npcArchtype);
            int id = Bootstrap.GenerateUniqueID();
            SetCharacter(entity, id, false, command.world, command.metaID, command.classID, command.clanID, command.position);
            SetNPCCharacter(entity, id, command.metaID, command.position);
        }
        #endregion

        #region CharacterDefaults

        private void GiveClassSkills(int characterID, ClassDatam classDatam)
        {
            Entity characterEntity = characters[characterID];
            //Character character = World.EntityManager.GetComponentData<Character>(characterEntity);
            //CharacterDatam characterDatam = meta[character.metaID];
            Skills newSkills = World.EntityManager.GetComponentData<Skills>(characterEntity);//new Skills { };
            newSkills.Initialize(classDatam);
            World.EntityManager.SetComponentData(characterEntity, newSkills);
            skillsSystem.InitializeSkills(characterEntity, newSkills);
        }
        private void SetNPCCharacter(Entity entity, int id, int metaID, float3 position) //  Entity world, 
        {
            if (meta.ContainsKey(metaID))
            {
                CharacterDatam characterDatam = meta[metaID];
                BehaviourData beh = characterDatam.behaviour.Value;
                SetNPC(entity, beh, id, characterDatam.movementSpeed, characterDatam.turnSpeed);
            }
        }

        private void SetNPC(Entity entity, BehaviourData beh, int id, float movementSpeed, float turnSpeed)
        {
            //Debug.LogError("Spawning NPC: " + id);
            if (World.EntityManager.HasComponent<AIState>(entity) == false)
            {
                Debug.LogError("NPC does not have AI State.");
                return;
            }
            World.EntityManager.SetComponentData(entity, new AIState
            {
                state = AIStates.Idle,
                idleTime = beh.idleTime,
                isAggressive = beh.isAggressive
            });
            World.EntityManager.SetComponentData(entity, new Mover
            {
                moveSpeed = movementSpeed,
                turnSpeed = turnSpeed
            });
            if (beh.wander.waitCooldownMin == 0 && beh.wander.wanderCooldownMax == 0)
            {
               // Debug.LogError("Removed wander.");
                World.EntityManager.RemoveComponent<Wander>(entity);
            }
            else
            {
                Unity.Mathematics.Random rand = new Unity.Mathematics.Random();
                rand.InitState((uint)id);
                World.EntityManager.SetComponentData(entity, new Wander
                {
                    uniqueness = id,
                    Value = beh.wander,
                    random = rand,
                    //targetRotation = quaternion.identity,//quaternion.EulerXYZ(new float3(0, rand.NextFloat(0, 360), 0)),
                    lastWandered = UnityEngine.Time.time + rand.NextFloat(4),
                    wanderCooldown = rand.NextFloat(beh.wander.wanderCooldownMin, beh.wander.wanderCooldownMax),
                    waitCooldown = rand.NextFloat(beh.wander.waitCooldownMin, beh.wander.waitCooldownMax)
                });
            }
        }
        private void SetPlayerCharacter(Entity entity, int id, Entity world, int metaID, float3 position)
        {
            CharacterDatam characterDatam = meta[metaID];
            var voxelDimensions = new int3(16, 64, 16);
            if (worldSpawnSystem != null)
            {
                voxelDimensions = World.EntityManager.GetComponentData<Voxels.World>(world).voxelDimensions;
            }

            Inventory inventory = new Inventory { };
            inventory.InitializeItems(9, characterDatam.items);
            World.EntityManager.SetComponentData(entity, inventory);

            QuestLog questlog = new QuestLog { };
            questlog.Initialize(characterDatam.quests);
            World.EntityManager.SetComponentData(entity, questlog);

            World.EntityManager.SetComponentData(entity, new ItemHitTaker { radius = characterDatam.itemPickupRadius });
            StatbarSystem.SpawnPlayerStatbar(World.EntityManager, entity);
            ActionbarSystem.SpawnUI(World.EntityManager, entity);
            CrosshairSpawnSystem.SpawnUI(World.EntityManager, entity);
            World.EntityManager.SetComponentData(entity, new ChunkStreamPoint
            {
                world = world,
                voxelDimensions = voxelDimensions,
                didUpdate = 1,
                chunkPosition = VoxelRaycastSystem.GetChunkPosition(new int3(position), voxelDimensions)
            });
        }

        private bool SetCharacter(Entity entity, int id, bool isLoadingPlayer, Entity world, int metaID, int classID, int clanID, float3 position, int creatorID = 0)
        {
            if (!World.EntityManager.Exists(world))
            {
                Debug.LogError("Cannot spawn character into a non existing world.");
                return false;
            }
            if (!World.EntityManager.HasComponent<Voxels.World>(world))
            {
                Debug.LogError("Cannot spawn character into a world without a component.");
                return false;
            }
            if (characters.ContainsKey(id) == true)
            {
                return false;
            }
            characters.Add(id, entity);
            if (!meta.ContainsKey(metaID))
            {
                Debug.LogError("Meta not contained: " + metaID);
                return false;
            }
            CharacterDatam characterDatam = meta[metaID];
            // ZOXID
            World.EntityManager.SetComponentData(entity,
                new ZoxID
                {
                    id = id,
                    clanID = clanID,
                    creatorID = creatorID
                });
            World.EntityManager.SetComponentData(entity,
                new Character
                {
                    metaID = metaID
                });
            // WORLD BINDING
            int3 voxelDimensions = new int3(16, 64, 16); // float3.zero;
            if (worldSpawnSystem != null)
            {
                voxelDimensions = World.EntityManager.GetComponentData<Voxels.World>(world).voxelDimensions;
            }
            // TRANSFORMS
            World.EntityManager.SetComponentData(entity, new Translation { Value = position });
            World.EntityManager.SetComponentData(entity, new Rotation { Value = quaternion.identity });
            World.EntityManager.SetComponentData(entity, new NonUniformScale
            {
                Value = new float3(1, 1, 1)
            });
            // RENDERING
            World.EntityManager.SetSharedComponentData(entity,  new RenderMesh {
                material = Bootstrap.GetVoxelMaterial(),
                mesh = new Mesh(),
                castShadows = UnityEngine.Rendering.ShadowCastingMode.On,
                receiveShadows = true
            });
            float3 bodySize = new float3(0.1f, 0.1f, 0.1f);
            if (characterDatam.vox != null && !isLoadingPlayer)
            {
                bodySize = characterDatam.vox.data.GetSize();
                World.EntityManager.SetComponentData(entity, new Body { size = bodySize });
                // this can be done in equip system
                WorldSpawnSystem.QueueUpdateModel(World.EntityManager, entity, id, characterDatam.vox.data);
            }
            World.EntityManager.SetComponentData(entity, new WorldBound { 
                size = bodySize,
                world = world,
                voxelDimensions = voxelDimensions
            });
            World.EntityManager.SetComponentData(entity, characterDatam.stats.Clone());
            // Physics
            World.EntityManager.SetComponentData(entity, new BodyInnerForce
            {
                movementForce = characterDatam.movementSpeed,
                movementTorque = characterDatam.turnSpeed,
                maxVelocity = characterDatam.maxVelocity
            });
            // combat stuff
            World.EntityManager.SetComponentData(entity,
                new Targeter
                {
                    Value = characterDatam.behaviour.Value.seek
                });

            if (!isLoadingPlayer)
            {
                if (classID != 0)
                {
                    GiveClassSkills(id, classMeta[classID]);
                }
                else if (characterDatam.defaultClass)
                {
                    GiveClassSkills(id, characterDatam.defaultClass);
                }
            }
            Equipment equipment = new Equipment{};
            if (!isLoadingPlayer)
            {
                equipment.EquipBody(characterDatam.body);
                equipment.EquipGear(characterDatam.gear);
            }
            World.EntityManager.SetComponentData(entity, equipment);
            return true;
        }

        #endregion

        #region SpawningDespawning

        private struct SpawnCharacterCommand : IComponentData
        {
            public byte isPlayer;
            public int creatorID;
            public int playerID;
            public Entity camera;    // change this to entity
            public Entity game;
            public Entity world;
            public int metaID;
            public int classID;
            public int clanID;
            public int characterID;
            public float3 position;
            public int amount;
            public BlitableArray<int> characterIDs;

            public void Dispose()
            {
                if (amount != 0)
                {
                    characterIDs.Dispose();
                }
            }
        }

        private struct RemoveCharacterCommand : IComponentData
        {
            public Entity character;
        }

        /// <summary>
        /// Add a camera to it after
        ///     if loading from save, then use premade characterID
        /// All characters spawn without a clan ID
        /// Spawns into a world
        /// Gives a class as a choice
        /// </summary>
        public static void SpawnPlayer(EntityManager EntityManager,
            int playerID, 
            Entity camera, Entity world, Entity game,
            int metaID, int classID, int characterID, float3 position)
        {
            if (EntityManager.Exists(world) == false)
            {
                Debug.LogError("Cannot spawn Player into non existing world.");
                return;
            }
            Entity e = EntityManager.CreateEntity();
            EntityManager.AddComponentData(e, new SpawnCharacterCommand
            {
                isPlayer = 1,
                playerID = playerID,
                camera = camera,
                world = world,
                game = game,
                metaID = metaID,
                classID = classID,
                characterID = characterID,
                position = position
            });
        }
        public static void SpawnNPC(EntityManager EntityManager, Entity world, int metaID, float3 position, int creatorID = 0)
        {
            Entity e = EntityManager.CreateEntity();
            EntityManager.AddComponentData(e, new SpawnCharacterCommand
            {
                world = world,
                metaID = metaID,
                position = position,
                creatorID = creatorID
            });
        }

        public static int SpawnNPC(EntityManager EntityManager, Entity world, int metaID, int clanID, float3 position, int creatorID = 0)
        {
            int characterID = Bootstrap.GenerateUniqueID();
            Entity e = EntityManager.CreateEntity();
            EntityManager.AddComponentData(e, new SpawnCharacterCommand
            {
                world = world,
                metaID = metaID,
                position = position,
                clanID = clanID,
                characterID = characterID,
                creatorID = creatorID
            });
            return characterID;
        }

        public static int[] SpawnNPCs(EntityManager EntityManager, Entity world, int metaID, int clanID, float3 position, int amount)
        {
            SpawnCharacterCommand command = new SpawnCharacterCommand
            {
                world = world,
                metaID = metaID,
                position = position,
                amount = amount,
                clanID = clanID,
                characterIDs = new BlitableArray<int>(amount, Allocator.Persistent)
            };
            for (int i = 0; i < amount; i++)
            {
                command.characterIDs[i] = (Bootstrap.GenerateUniqueID());
            }
            Entity e = EntityManager.CreateEntity();
            EntityManager.AddComponentData(e, command);
            return command.characterIDs.ToArray();
        }

        /// <summary>
        /// Called from DeathSystem
        /// </summary>
        public static void RemoveCharacter(EntityManager EntityManager, Entity character)
        {
            Entity e = EntityManager.CreateEntity();
            EntityManager.AddComponentData(e, new RemoveCharacterCommand
            {
                character = character
            });
        }

        protected override void OnUpdate()
        {
            Entities.WithAll<SpawnCharacterCommand>().ForEach((Entity e, ref SpawnCharacterCommand command) =>
            {
                if (command.isPlayer == 1)
                {
                    SpawnPlayerCharacter(command);
                }
                else
                {
                    if (command.amount != 0)
                    {
                        SpawnNPCCharacters(command);
                    }
                    else
                    {
                        SpawnNPCCharacter(command);
                    }
                }
                command.Dispose();
                World.EntityManager.DestroyEntity(e);
            });
            Entities.WithAll<RemoveCharacterCommand>().ForEach((Entity e, ref RemoveCharacterCommand command) =>
            {
                //RemoveUI(command.character);
                World.EntityManager.DestroyEntity(e);
            });
        }

        public void Clear()
        {
            foreach (Entity e in characters.Values)
            {
                if (World.EntityManager.Exists(e))
                {
                    if (World.EntityManager.HasComponent<Controller>(e))
                    {
                        playerSpawnSystem.RemoveControllerCharacter(e);
                    }
                    World.EntityManager.DestroyEntity(e);
                }
            }
            characters.Clear();
        }
        #endregion
    }
}
