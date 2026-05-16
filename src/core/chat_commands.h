#ifndef CHAT_COMMANDS_H
#define CHAT_COMMANDS_H

int chat_join(int fd);
int chat_leave(int fd);
int chat_say(int fd, const char *line);
int chat_who(int fd);

#endif
