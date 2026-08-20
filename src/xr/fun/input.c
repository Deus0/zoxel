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
// Current bindings:
//   Left stick  -> Move
//   Right stick -> Turn
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
static const float stick_deadzone = 0.15f;
static XrActionSet input_action_set = XR_NULL_HANDLE;
static XrAction move_action = XR_NULL_HANDLE;
static XrAction turn_action = XR_NULL_HANDLE;
static XrPath left_hand_path = XR_NULL_PATH;
static XrPath right_hand_path = XR_NULL_PATH;

static float xr_apply_deadzone(float value) {
    if (fabsf(value) < stick_deadzone) {
        return 0;
    }
    return value;
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
    const XrActionSuggestedBinding bindings[] = {
        {
            .action = move_action,
            .binding = left_thumbstick
        },
        {
            .action = turn_action,
            .binding = right_thumbstick
        }
    };
    result = xrSuggestInteractionProfileBindings(xr_instance,
        &(XrInteractionProfileSuggestedBinding){
            .type = XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING,
            .next = NULL,
            .interactionProfile = touch_profile,
            .countSuggestedBindings = 2,
            .suggestedBindings = bindings
        }
    );
    if (!xr_check_result(result,
        "xrSuggestInteractionProfileBindings"
    )) {
        return 0;
    }
    // after session i guess
    // For XR Inputs
    result = xrAttachSessionActionSets(
        session,
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

static int xr_update_input(void) {
    XrActiveActionSet active_action_set = {
        .actionSet = input_action_set,
        .subactionPath = XR_NULL_PATH
    };
    XrResult result = xrSyncActions(
        session,
        &(XrActionsSyncInfo){
            .type = XR_TYPE_ACTIONS_SYNC_INFO,
            .next = NULL,
            .countActiveActionSets = 1,
            .activeActionSets = &active_action_set
        }
    );
    if (XR_FAILED(result)) {
        zox_loge("[XR] xrSyncActions failed: %d", result);
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
        session,
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
        session,
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