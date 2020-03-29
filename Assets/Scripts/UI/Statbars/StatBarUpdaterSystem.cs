using Unity.Entities;
using Unity.Jobs;
using Unity.Collections;
using Unity.Burst;
using UnityEngine;

/// <summary>
/// Flagged for rewrite - Due to using Referencing to lists of character entities
/// </summary>

namespace Zoxel
{
    [DisableAutoCreation]
    public class StatBarUpdaterSystem : ComponentSystem
    {
        //public CharacterSpawnSystem characterSpawnSystem;

        protected override void OnUpdate()
        {
            Entities.WithAll<StatBarUI>().ForEach((Entity e, ref StatBarUI statbar) =>
            {
                //if (characterSpawnSystem.characters.ContainsKey(zoxID.id))
                if (World.EntityManager.Exists(statbar.character))
                {
                    Stats characterStats = World.EntityManager.GetComponentData<Stats>(statbar.character);
                    if (characterStats.states.Length > 0)
                    {
                        StateStaz staz = characterStats.states[0];
                        statbar.targetPercentage = staz.value / staz.maxValue;
                    }
                }
            });
        }
    }
}