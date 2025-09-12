
ECS_DTOR(SocketLink, ptr, {
    if (ptr->value != -1) {
        if (close(ptr->value) < 0) perror(" ! [SocketLink] failed to close");
        ptr->value = -1;
    }
})
