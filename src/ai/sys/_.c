// debugs need systems to be _1 main thread
byte is_debug_behaviour = 0;
byte is_debug_target = 0;
byte is_debug_rotate_towards = 0;
byte is_debug_move_forwards = 0;
byte is_debug_wander = 0;
byte is_debug_flee = 0;
byte is_debug_attack = 0;
#include "wander_system.c"
#include "flee_system.c"
#include "follow_system.c"
#include "behaviour_system.c"
#include "rotate_towards_system.c"
#include "move_forward_system.c"
#include "target_set_system.c"
#include "attack.c"
#include "attack_trigger.c"
#include "stay_upright_system.c"
#include "random_jumping.c"
#include "settings.c"

void define_systems_npcs(ecs *world) {
    zox_system(
        BehaviourSystem,
        zoxp_update,
        [in] combat.CombatState,
        [in] ai.DefaultBehaviour,
        [out] ai.Behaviour,
        [out] ai.MoveForwards,
        [out] ai.RotateTowards,
        [out] ai.MoveSpeed,
        [none] ai.Npc
    );
    zox_system_m(
        ThreatPositionSystem,
        !is_debug_target,
        [in] combat.LastDamager,
        [out] ai.ThreatPosition,
        [none] ai.Npc
    );
    zox_system_m(
        RotateTowardsSystem,
        !is_debug_rotate_towards,
        [in] ai.RotateTowards,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [in] physics3.Omega3D,
        [in] ai.TargetPosition,
        [out] physics3.Alpha3D,
        [none] ai.Npc,
        [none] !physics.DisableMovement,
    );
    zox_system_m(
        MoveForwardSystem,
        !is_debug_move_forwards,
        [in] ai.MoveForwards,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        // [in] physics3.Velocity3D,
        [in] ai.TargetPosition,
        [in] ai.MoveSpeed,
        [in] ai.MoveToBuffer,
        [out] physics3.Acceleration3D,
        [none] ai.Npc,
        [none] !physics.DisableMovement,
    );
    zox_system_m(
        WanderSystem,
        !is_debug_wander,
        [in] ai.Behaviour,
        [in] transforms3.Position3D,
        [out] ai.TargetPosition,
        [out] ai.WanderDirection,
        [none] ai.Npc,
        [none] !physics.DisableMovement,
    );
    zox_system_m(
        AttackSystem,
        !is_debug_attack,
        [in] ai.Behaviour,
        [in] ai.ThreatPosition,
        [out] ai.TargetPosition,
        [none] ai.Npc,
        [none] !physics.DisableMovement,
    );
    zox_system_m(
        AttackTriggerSystem,
        !is_debug_attack,
        [in] ai.Behaviour,
        [out] actions.ActiveAction,
        [out] actions.ActiveActionDirty,
        [out] triggers.TriggerActionA,
        [none] ai.Npc,
        [none] !physics.DisableMovement,
    );
    zox_system_m(
        FleeSystem,
        !is_debug_flee,
        [in] ai.Behaviour,
        [in] transforms3.Position3D,
        [in] ai.ThreatPosition,
        [out] ai.TargetPosition,
        [none] ai.Npc,
        [none] !physics.DisableMovement,
    );
    /*zox_system(StayUprightSystem, EcsOnUpdate,
        [in] transforms3.Rotation3D,
        [in] physics3.Omega3D,
        [out] physics3.Alpha3D,
        [none] ai.Npc)*/
    zox_system(
        RandomJump3DSystem,
        zoxp_physics,
        [in] jumps.CanJump,
        [out] jumps.JumpState,
        [none] ai.Jumper
    );
    zox_system(
        FollowSystem,
        zoxp_update,
        [in] Behaviour,
        [in] FollowTarget,
        [out] ai.TargetPosition,
        [none] ai.Npc,
        [none] !physics.DisableMovement,
    );
    // Sound gen takes longer;
    zox_system_1(
        AiSettingsSystem,
        zoxp_mainthread,
        [in] settings.LoadSettings
    );
    zox_system_1(
        AiSettingsDirtySystem,
        zoxp_mainthread,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
