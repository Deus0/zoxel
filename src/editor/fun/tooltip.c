


byte inspector_label_tooltip(ecs *world, const TooltipEventData *data) {

    entity label = data->ui;
    entity tooltip = data->tooltip;

    if (!zox_valid(label) || !zox_has(label, TooltipText)) {
        zox_loge("Invalid Tooltip UI");
        return 0;
    }


    zox_geter_value(label, ComponentType, byte, ctype);

    if (ctype < zox_type_end) {
        zox_geter_value(label, EntityTarget, entity, e);
        zox_geter_value(label, ComponentTarget, entity, cid);

        uint index = 0;
        char text[TooltipText_length];
        index += sprintf(text, "%s [%s]",
            zox_valid(cid) ? zox_get_name(cid) : (zox_valid(e) ? zox_get_name(e) : "Invalid"), ctype < zox_type_end ? zox_type_labels[ctype] : "invalid");

        if (ctype == zox_type_children) {
            entity children[hierarhys_children_capacity];
            uint children_length = zox_get_children(world, e, children, hierarhys_children_capacity);
            char children_label[TooltipText_length];
            sprintf(children_label, "children [%i]",children_length);
            index += sprintf(index + text, " len [%i]", children_length);
            for (int i = 0; i < children_length; i++) {
                entity e2 = children[i];
                // TODO: Check overflow before we add
                index += sprintf(index + text, "\n  - %s", zox_valid(e2) ? zox_get_name(e2) : " None");
                if (index + 64 >= TooltipText_length) {
                    index += sprintf(index + text, "\n  ...");
                    break;
                }
            }

        } else if (ctype == zox_type_entities) {
            const entities* links = (const entities*) zox_get_id(e, cid);

            index += sprintf(index + text, " len [%i]", links->length);
            for (int i = 0; i < links->length; i++) {
                entity e2 = links->value[i];

                // TODO: Check overflow before we add
                index += sprintf(index + text, "\n  - %s", zox_valid(e2) ? zox_get_name(e2) : " None");

                if (index + 64 >= TooltipText_length) {
                    index += sprintf(index + text, "\n  ...");
                    break;
                }
            }

        } else if (ctype == zox_type_unknown) {
            index += sprintf(index + text, " ?");
        }

        set_entity_text(world, tooltip, text);
    } else {
        zox_geter(label, TooltipText, text);
        set_entity_text(world, tooltip, text->value);
    }

    return 1;
}