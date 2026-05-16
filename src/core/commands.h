#ifndef BOX42_COMMANDS_H
#define BOX42_COMMANDS_H

/* ---------------------------------------------------------
   Helper
--------------------------------------------------------- */
int cmd_is(const char *cmd, const char *line);

/* ---------------------------------------------------------
   Login / Registration / Userdata
--------------------------------------------------------- */
int cmd_login(int fd, const char *line);
int cmd_registration(int fd, const char *line);
int cmd_userdata(int fd, const char *line);

/* ---------------------------------------------------------
   Focus
--------------------------------------------------------- */
int cmd_focus(int fd, const char *line);

/* ---------------------------------------------------------
   Chat Commands
--------------------------------------------------------- */
int cmd_join(int fd, const char *line);
int cmd_leave(int fd, const char *line);
int cmd_say(int fd, const char *line);
int cmd_who(int fd, const char *line);

/* ---------------------------------------------------------
   Logoff / Exit
--------------------------------------------------------- */
int cmd_logoff(int fd, const char *line);

/* ---------------------------------------------------------
   Account Removal
--------------------------------------------------------- */
int cmd_removeme(int fd, const char *line);

#endif /* BOX42_COMMANDS_H */
