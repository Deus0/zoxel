using Unity.Entities;
using UnityEngine;
using Unity.Mathematics;
using Unity.Collections;
using Unity.Rendering;
using Unity.Transforms;
using System;

namespace Zoxel.Voxels
{
    [DisableAutoCreation]
    public class RenderSystem : ComponentSystem
    {
        EntityQuery m_Group;
    
        protected override void OnCreate()
        {
            m_Group = GetEntityQuery(typeof(ChunkMeshLink), typeof(Translation));
        }
    
        protected override void OnUpdate()
        {
            //var renderData = m_Group.ToSharedComponentDataArray<RenderMesh>();
            var renderData = m_Group.ToEntityArray(Allocator.TempJob);
            var positionData = m_Group.ToComponentDataArray<Translation>(Allocator.TempJob); // Allocator.Temp, null
            float3 scale = new float3(1,1,1);
            quaternion identityRotation = quaternion.identity;
            for (var i = 0; i < renderData.Length; i++)
            {
                float4x4 matrix2 = float4x4.TRS(positionData[i].Value, identityRotation, scale);
                Matrix4x4 matrix = matrix2;
                var data = renderData[i];
                var renderMesh = World.EntityManager.GetSharedComponentData<ChunkMeshLink>(data);
                Graphics.DrawMesh(renderMesh.mesh, matrix, renderMesh.material, 0);
            }
            positionData.Dispose();
            renderData.Dispose();
        }
    }

}