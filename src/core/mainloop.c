void mainloop(int fd)
{
    char line[512];

    while (1) {
        term_write(fd, prompt_for_session(fd));

        if (!term_readline(fd, line, sizeof(line)))
            break;

        command_dispatch(fd, line);
    }
}
