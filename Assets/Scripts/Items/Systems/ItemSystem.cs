﻿using Unity.Entities;
using Unity.Mathematics;
using Unity.Transforms;
using Unity.Rendering;
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Zoxel.Voxels;

namespace Zoxel
{
    [DisableAutoCreation]
    public class ItemSpawnerSystem : ComponentSystem
    {
        // prefabs
        private EntityArchetype itemArchtype;
        public Dictionary<int, ItemDatam> meta;
        // spawns
        public Dictionary<int, Entity> items = new Dictionary<int, Entity>();

        #region Spawning-Removing

        protected override void OnCreate()
        {
            itemArchtype = World.EntityManager.CreateArchetype(
                typeof(WorldItem),
                typeof(ItemBob),
                // transform
                typeof(Translation),
                typeof(Rotation),
                typeof(NonUniformScale),
                // renderer
                typeof(RenderMesh),
                typeof(LocalToWorld)
            );
        }

        public void Clear()
        {
            foreach (Entity e in items.Values)
            {
                if (World.EntityManager.Exists(e))
                {
                    World.EntityManager.DestroyEntity(e);
                }
            }
            items.Clear();
        }


        public void QueueItem(float3 spawnPosition, ItemDatam data, int quantity)
        {
            Entity e = World.EntityManager.CreateEntity();
            World.EntityManager.AddComponentData(e, new SpawnItemCommand
            {
                data = data.data,
                quantity = quantity,
                spawnPosition = spawnPosition,
                spawnRotation = quaternion.identity
            });
        }

        public struct SpawnItemCommand : IComponentData
        {
            public Item data;
            public int quantity;
            public float3 spawnPosition;
            public quaternion spawnRotation;
        }

        protected override void OnUpdate()
        {
            Entities.WithAll<SpawnItemCommand>().ForEach((Entity e, ref SpawnItemCommand command) =>
            {
                SpawnItem(command);
                World.EntityManager.DestroyEntity(e);
            });
        }
        #endregion

        private void SpawnItem(SpawnItemCommand command)
        {
            if (meta.ContainsKey(command.data.id) == false)
            {
                Debug.LogError("Item not in meta.");
                return;
            }
            ItemDatam itemDatam = meta[command.data.id];
            Entity entity = World.EntityManager.CreateEntity(itemArchtype);
            int id = Bootstrap.GenerateUniqueID();
            World.EntityManager.SetComponentData(entity, new WorldItem {
                id = id,
                data = command.data,
                quantity = command.quantity
            });
            World.EntityManager.SetComponentData(entity, new ItemBob {
                originalPosition = command.spawnPosition
            });
            World.EntityManager.SetComponentData(entity, new Translation {
                Value = command.spawnPosition
            });
            World.EntityManager.SetComponentData(entity, new NonUniformScale { Value = itemDatam.model.data.scale }); // itemDatam.Value.scale * 0.5f });
            World.EntityManager.SetComponentData(entity, new Rotation { Value = Quaternion.Euler(0, UnityEngine.Random.Range(-180, 180), 0) });

            RenderMesh newRenderer = new RenderMesh();
            newRenderer.castShadows = UnityEngine.Rendering.ShadowCastingMode.On;
            newRenderer.receiveShadows = true;
            newRenderer.mesh = new Mesh(); //itemDatam.model.bakedMesh;
            newRenderer.material = Bootstrap.GetVoxelMaterial();
            World.EntityManager.SetSharedComponentData(entity, newRenderer);
            WorldSpawnSystem.QueueUpdateModel(World.EntityManager, entity, id, itemDatam.model.data);
            items.Add(id, entity);
        }
    }
}
