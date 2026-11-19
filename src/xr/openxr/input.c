// --------------------------------------------------
// XR Inputs
//
// OpenXR input is handled in three stages:
//
// 1. Initialize actions and bind them to controller inputs.
// 2. Sync the action set once per frame.
// 3. Read individual inputs through small getter functions.
//
// This layer only handles OpenXR input and returns raw joystick
// values with a deadzone applied. Game/player logic such as
// movement, rotation, inversion, and flight is handled elsewhere.
//
// Current bindings (Oculus Touch):
//   Left stick  -> Move,  Right stick -> Turn
//   A B (right), X Y menu (left), each hand's trigger and grip (values), each hand's aim pose (where it points)
//   and grip pose (where it's held)
//
// Flow:
//   xr_input_init()
//        ↓
//   xr_update_input()
//        ↓
//   xr_get_left_stick()
//   xr_get_right_stick()
// --------------------------------------------------

// Controllers
static const float xr_stick_deadzone = 0.15f;
static XrActionSet input_action_set = XR_NULL_HANDLE;
static XrAction move_action = XR_NULL_HANDLE;
static XrAction turn_action = XR_NULL_HANDLE;
static XrAction primary_action = XR_NULL_HANDLE;    // A right, X left
static XrAction secondary_action = XR_NULL_HANDLE;  // B right, Y left
static XrAction menu_action = XR_NULL_HANDLE;       // left
static XrAction trigger_action = XR_NULL_HANDLE;
static XrAction grip_action = XR_NULL_HANDLE;
static XrAction aim_action = XR_NULL_HANDLE;
static XrAction grip_pose_action = XR_NULL_HANDLE;
static XrSpace aim_spaces[2] = { XR_NULL_HANDLE, XR_NULL_HANDLE };
static XrSpace grip_spaces[2] = { XR_NULL_HANDLE, XR_NULL_HANDLE };
static XrPath left_hand_path = XR_NULL_PATH;
static XrPath right_hand_path = XR_NULL_PATH;

static float xr_apply_deadzone(float value) {
    if (fabsf(value) < xr_stick_deadzone) {
        return 0;
    }
    return value;
}

static int xr_create_action(XrAction* action, const char* name, XrActionType type, const XrPath* subaction_paths) {
    XrActionCreateInfo info = {
        .type = XR_TYPE_ACTION_CREATE_INFO,
        .next = NULL,
        .actionType = type,
        .countSubactionPaths = 2,
        .subactionPaths = subaction_paths
    };
    strncpy(info.actionName, name, XR_MAX_ACTION_NAME_SIZE - 1);
    strncpy(info.localizedActionName, name, XR_MAX_LOCALIZED_ACTION_NAME_SIZE - 1);
    return xr_check_result(xrCreateAction(input_action_set, &info, action), name);
}

static int xr_path(const char* name, XrPath* path) {
    return xr_check_result(xrStringToPath(xr_instance, name, path), name);
}

static int xr_input_init(void) {
    XrResult result;
    result = xrStringToPath(
        xr_instance,
        "/user/hand/left",
        &left_hand_path
    );
    if (!xr_check_result(result, "xrStringToPath(left hand)")) {
        return 0;
    }
    result = xrStringToPath(
        xr_instance,
        "/user/hand/right",
        &right_hand_path
    );
    if (!xr_check_result(result, "xrStringToPath(right hand)")) {
        return 0;
    }
    result = xrCreateActionSet(
        xr_instance,
        &(XrActionSetCreateInfo){
            .type = XR_TYPE_ACTION_SET_CREATE_INFO,
            .next = NULL,
            .actionSetName = "gameplay",
            .localizedActionSetName = "Gameplay",
            .priority = 0
        },
        &input_action_set
    );
    if (!xr_check_result(result, "xrCreateActionSet")) {
        return 0;
    }
    const XrPath subaction_paths[] = {
        left_hand_path,
        right_hand_path
    };
    result = xrCreateAction(
        input_action_set,
        &(XrActionCreateInfo){
            .type = XR_TYPE_ACTION_CREATE_INFO,
            .next = NULL,
            .actionName = "move",
            .localizedActionName = "Move",
            .actionType = XR_ACTION_TYPE_VECTOR2F_INPUT,
            .countSubactionPaths = 2,
            .subactionPaths = subaction_paths
        },
        &move_action
    );
    if (!xr_check_result(result, "xrCreateAction(move)")) {
        return 0;
    }
    result = xrCreateAction(
        input_action_set,
        &(XrActionCreateInfo){
            .type = XR_TYPE_ACTION_CREATE_INFO,
            .next = NULL,
            .actionName = "turn",
            .localizedActionName = "Turn",
            .actionType = XR_ACTION_TYPE_VECTOR2F_INPUT,
            .countSubactionPaths = 2,
            .subactionPaths = subaction_paths
        },
        &turn_action
    );
    if (!xr_check_result(result, "xrCreateAction(turn)")) {
        return 0;
    }
    XrPath touch_profile = XR_NULL_PATH;
    result = xrStringToPath(
        xr_instance,
        "/interaction_profiles/oculus/touch_controller",
        &touch_profile
    );
    if (!xr_check_result(
        result,
        "xrStringToPath(oculus touch controller)"
    )) {
        return 0;
    }
    XrPath left_thumbstick = XR_NULL_PATH;
    XrPath right_thumbstick = XR_NULL_PATH;
    result = xrStringToPath(
        xr_instance,
        "/user/hand/left/input/thumbstick",
        &left_thumbstick
    );
    if (!xr_check_result(
        result,
        "xrStringToPath(left thumbstick)"
    )) {
        return 0;
    }
    result = xrStringToPath(
        xr_instance,
        "/user/hand/right/input/thumbstick",
        &right_thumbstick
    );
    if (!xr_check_result(
        result,
        "xrStringToPath(right thumbstick)"
    )) {
        return 0;
    }
    if (!xr_create_action(&primary_action, "primary", XR_ACTION_TYPE_BOOLEAN_INPUT, subaction_paths) ||
        !xr_create_action(&secondary_action, "secondary", XR_ACTION_TYPE_BOOLEAN_INPUT, subaction_paths) ||
        !xr_create_action(&menu_action, "menu", XR_ACTION_TYPE_BOOLEAN_INPUT, subaction_paths) ||
        !xr_create_action(&trigger_action, "trigger", XR_ACTION_TYPE_FLOAT_INPUT, subaction_paths) ||
        !xr_create_action(&grip_action, "grip", XR_ACTION_TYPE_FLOAT_INPUT, subaction_paths) ||
        !xr_create_action(&aim_action, "aim", XR_ACTION_TYPE_POSE_INPUT, subaction_paths) ||
        !xr_create_action(&grip_pose_action, "grip_pose", XR_ACTION_TYPE_POSE_INPUT, subaction_paths)) {
        return 0;
    }
    XrPath paths[13];
    const char* path_names[13] = {
        "/user/hand/right/input/a/click",
        "/user/hand/right/input/b/click",
        "/user/hand/left/input/x/click",
        "/user/hand/left/input/y/click",
        "/user/hand/left/input/menu/click",
        "/user/hand/left/input/trigger/value",
        "/user/hand/right/input/trigger/value",
        "/user/hand/left/input/squeeze/value",
        "/user/hand/right/input/squeeze/value",
        "/user/hand/left/input/aim/pose",
        "/user/hand/right/input/aim/pose",
        "/user/hand/left/input/grip/pose",
        "/user/hand/right/input/grip/pose",
    };
    for (int i = 0; i < 13; i++) {
        if (!xr_path(path_names[i], &paths[i])) {
            return 0;
        }
    }
    const XrActionSuggestedBinding bindings[] = {
        { .action = move_action, .binding = left_thumbstick },
        { .action = turn_action, .binding = right_thumbstick },
        { .action = primary_action, .binding = paths[0] },
        { .action = secondary_action, .binding = paths[1] },
        { .action = primary_action, .binding = paths[2] },
        { .action = secondary_action, .binding = paths[3] },
        { .action = menu_action, .binding = paths[4] },
        { .action = trigger_action, .binding = paths[5] },
        { .action = trigger_action, .binding = paths[6] },
        { .action = grip_action, .binding = paths[7] },
        { .action = grip_action, .binding = paths[8] },
        { .action = aim_action, .binding = paths[9] },
        { .action = aim_action, .binding = paths[10] },
        { .action = grip_pose_action, .binding = paths[11] },
        { .action = grip_pose_action, .binding = paths[12] },
    };
    result = xrSuggestInteractionProfileBindings(xr_instance,
        &(XrInteractionProfileSuggestedBinding){
            .type = XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING,
            .next = NULL,
            .interactionProfile = touch_profile,
            .countSuggestedBindings = sizeof(bindings) / sizeof(bindings[0]),
            .suggestedBindings = bindings
        }
    );
    if (!xr_check_result(result,
        "xrSuggestInteractionProfileBindings"
    )) {
        return 0;
    }
    const XrPath hands[2] = { left_hand_path, right_hand_path };
    for (int i = 0; i < 2; i++) {
        result = xrCreateActionSpace(xr_session,
            &(XrActionSpaceCreateInfo){
                .type = XR_TYPE_ACTION_SPACE_CREATE_INFO,
                .action = aim_action,
                .subactionPath = hands[i],
                .poseInActionSpace = { .orientation = { 0, 0, 0, 1 } }
            },
            &aim_spaces[i]);
        if (!xr_check_result(result, "xrCreateActionSpace(aim)")) {
            return 0;
        }
        result = xrCreateActionSpace(xr_session,
            &(XrActionSpaceCreateInfo){
                .type = XR_TYPE_ACTION_SPACE_CREATE_INFO,
                .action = grip_pose_action,
                .subactionPath = hands[i],
                .poseInActionSpace = { .orientation = { 0, 0, 0, 1 } }
            },
            &grip_spaces[i]);
        if (!xr_check_result(result, "xrCreateActionSpace(grip)")) {
            return 0;
        }
    }
    // after session i guess
    // For XR Inputs
    result = xrAttachSessionActionSets(
        xr_session,
        &(XrSessionActionSetsAttachInfo){
            .type = XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO,
            .next = NULL,
            .countActionSets = 1,
            .actionSets = &input_action_set
        }
    );
    if (!xr_check_result(result,
        "xrAttachSessionActionSets"
    )) {
        return 0;
    }
    zox_log("[XR] Controller actions ready");
    return 1;
}

static int xr_update_input() {
    XrActiveActionSet active_action_set = {
        .actionSet = input_action_set,
        .subactionPath = XR_NULL_PATH
    };
    XrResult result = xrSyncActions(
        xr_session,
        &(XrActionsSyncInfo){
            .type = XR_TYPE_ACTIONS_SYNC_INFO,
            .next = NULL,
            .countActiveActionSets = 1,
            .activeActionSets = &active_action_set
        }
    );
    if (!xr_check_result(result, "xrSyncActions")) {
        return 0;
    }
    return 1;
}

static float2 xr_get_left_stick() {
    float2 stick = { 0, 0 };
    XrActionStateVector2f state = {
        .type = XR_TYPE_ACTION_STATE_VECTOR2F
    };
    XrResult result = xrGetActionStateVector2f(
        xr_session,
        &(XrActionStateGetInfo){
            .type = XR_TYPE_ACTION_STATE_GET_INFO,
            .action = move_action,
            .subactionPath = left_hand_path
        },
        &state
    );
    if (XR_SUCCEEDED(result) && state.isActive) {
        stick.x = xr_apply_deadzone(state.currentState.x);
        stick.y = xr_apply_deadzone(state.currentState.y);
    }
    return stick;
}

static float2 xr_get_right_stick(void) {
    float2 stick = { 0, 0 };
    XrActionStateVector2f state = {
        .type = XR_TYPE_ACTION_STATE_VECTOR2F
    };
    XrResult result = xrGetActionStateVector2f(
        xr_session,
        &(XrActionStateGetInfo){
            .type = XR_TYPE_ACTION_STATE_GET_INFO,
            .action = turn_action,
            .subactionPath = right_hand_path
        },
        &state
    );
    if (XR_SUCCEEDED(result) && state.isActive) {
        stick.x = xr_apply_deadzone(state.currentState.x);
        stick.y = xr_apply_deadzone(state.currentState.y);
    }
    return stick;
}

// a hand's boolean action (hand: 0 left, 1 right)
static int xr_get_bool(XrAction action, int hand) {
    XrActionStateBoolean state = { .type = XR_TYPE_ACTION_STATE_BOOLEAN };
    XrResult result = xrGetActionStateBoolean(xr_session,
        &(XrActionStateGetInfo){
            .type = XR_TYPE_ACTION_STATE_GET_INFO,
            .action = action,
            .subactionPath = hand ? right_hand_path : left_hand_path
        },
        &state);
    return XR_SUCCEEDED(result) && state.isActive && state.currentState;
}

static float xr_get_float(XrAction action, int hand) {
    XrActionStateFloat state = { .type = XR_TYPE_ACTION_STATE_FLOAT };
    XrResult result = xrGetActionStateFloat(xr_session,
        &(XrActionStateGetInfo){
            .type = XR_TYPE_ACTION_STATE_GET_INFO,
            .action = action,
            .subactionPath = hand ? right_hand_path : left_hand_path
        },
        &state);
    return XR_SUCCEEDED(result) && state.isActive ? state.currentState : 0;
}

// a hand's pose (its action space's) in the tracking space at a time, returns whether it's tracked
static int xr_locate_hand(XrSpace hand_space, XrTime time, XrPosef* pose) {
    if (hand_space == XR_NULL_HANDLE) {
        return 0;
    }
    XrSpaceLocation location = { .type = XR_TYPE_SPACE_LOCATION };
    XrResult result = xrLocateSpace(hand_space, space, time, &location);
    if (!XR_SUCCEEDED(result) ||
        !(location.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) ||
        !(location.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT)) {
        return 0;
    }
    *pose = location.pose;
    return 1;
}

// where a hand points (rays at 3D ui)
static int xr_get_aim(int hand, XrTime time, XrPosef* pose) {
    return xr_locate_hand(aim_spaces[hand], time, pose);
}

// where a hand holds its controller (the body's hand)
static int xr_get_grip(int hand, XrTime time, XrPosef* pose) {
    return xr_locate_hand(grip_spaces[hand], time, pose);
}
