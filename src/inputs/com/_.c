zox_tag(Device);
zox_tag(Zevice);
zox_tag(Gamepad);
zox_tag(Touchscreen);
zox_tag(Mouse);
zox_tag(Finger);
zox_tag(ZeviceBumper);
zox_tag(MousePointer);
zox_tag(DeviceNavigator);
// zoxc_byte(DeviceDisabled);
// zoxc_byte(ZeviceDisabled);
zoxc_byte(ZeviceButton);
zoxc_byte(ZevicePointer);
zoxc_byte(ZevicePointerOld);
zoxc_byte(ZevicePointerRight);
zoxc_byte(DeviceButtonType);
zoxc_byte(RealButtonIndex);
zoxc_byte(DeviceLayout);
zoxc_byte(MouseLock);
zoxc_byte(DeviceMode);
zoxc_byte(LastDeviceMode);
zoxc_byte(DeviceModeDirty);
zoxc_byte(DeviceHasInput);
zoxc_float2(ZeviceStick);
zoxc_int2(ZevicePointerPosition);
zoxc_int2(ZevicePointerDelta);
zoxc_int2(ZeviceWheel);
zox_tag(DeviceLink);       // links to a device, i.e. from its zevices
zox_tag(ZeviceLink);       // links to a zevice, i.e. a pointer
zox_tag(VirtualZeviceLink);
#include "keyboard.c"

void zox_components_inputs(ecs *world) {
    zoxd_tag(Device);
    zoxd_tag(Zevice);
    zoxd_tag(Gamepad);
    zoxd_tag(Touchscreen);
    zoxd_tag(Finger);
    zoxd_tag(Mouse);
    zoxd_tag(ZeviceBumper);
    zoxd_tag(MousePointer);
    zoxd_tag(DeviceNavigator);
    zoxd(Keyboard);
    // zoxd_byte(DeviceDisabled);
    // zoxd_byte(ZeviceDisabled);
    zoxd_byte(ZevicePointer);
    zoxd_byte(ZevicePointerOld);
    zoxd_byte(ZevicePointerRight);
    zoxd_byte(ZeviceButton);
    zoxd_byte(DeviceButtonType);
    zoxd_byte(RealButtonIndex);
    zoxd_byte(DeviceLayout);
    zoxd_byte(MouseLock);
    zoxd_byte(DeviceMode);
    zoxd_byte(LastDeviceMode);
    zoxd_byte(DeviceModeDirty);
    zoxd_byte(DeviceHasInput);
    zoxd_int2(ZevicePointerPosition);
    zoxd_int2(ZevicePointerDelta);
    zoxd_int2(ZeviceWheel);
    zoxd_float2(ZeviceStick);
    zoxd_nf_link(DeviceLink);
    zoxd_nf_link(ZeviceLink);
    zoxd_nf_link(VirtualZeviceLink);
}
