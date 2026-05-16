enum focus_mode {
    FOCUS_IDLE = 0,
    FOCUS_CHAT,
    FOCUS_MAIL,
    FOCUS_BOX,
    FOCUS_SYSTEM
};

struct session {
    int fd;
    int level;
    char username[64];
    int in_chat;
    enum focus_mode focus;
};
