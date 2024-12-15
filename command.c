#include "command.h"

#include <string.h>
#include <stdio.h>

const char*
interpret_command(const char *cmd) {
    const char *cmdend = strchr(cmd, ' ');
    int cmdlen = cmdend - cmd;
    const char *end = strchr(cmd, '\n');

    if      (strncmp(cmd, ".pagewidth", cmdlen) == 0)
        cmd_pagewidth(cmd + cmdlen);
    else if (strncmp(cmd, ".pageheight", cmdlen) == 0)
        cmd_pageheight(cmd + cmdlen);
    else if (strncmp(cmd, ".tabstop", cmdlen) == 0) {}
    else if (strncmp(cmd, ".indentparagraph", cmdlen) == 0) {}
    else if (strncmp(cmd, ".header", cmdlen) == 0) {}
    else if (strncmp(cmd, ".footer", cmdlen) == 0) {}
    else if (strncmp(cmd, ".title", cmdlen) == 0) {}
    else if (strncmp(cmd, ".author", cmdlen) == 0) {}
    else if (strncmp(cmd, ".date", cmdlen) == 0) {}
    else if (strncmp(cmd, ".titlepage", cmdlen) == 0) {}
    else if (strncmp(cmd, ".part", cmdlen) == 0) {}
    else if (strncmp(cmd, ".chapter", cmdlen) == 0) {}
    else if (strncmp(cmd, ".section", cmdlen) == 0) {}
    else if (strncmp(cmd, ".subsection", cmdlen) == 0) {}
    else if (strncmp(cmd, ".subsubsection", cmdlen) == 0) {}
    else if (strncmp(cmd, ".pagebreak", cmdlen) == 0) {}
    else if (strncmp(cmd, ".tableofcontents", cmdlen) == 0) {}
    else if (strncmp(cmd, ".itemize", cmdlen) == 0) {}
    else if (strncmp(cmd, ".enumerate", cmdlen) == 0) {}
    else if (strncmp(cmd, ".item", cmdlen) == 0) {}
    else if (strncmp(cmd, ".table", cmdlen) == 0) {}
    else if (strncmp(cmd, ".tr", cmdlen) == 0) {}
    else if (strncmp(cmd, ".!tr", cmdlen) == 0) {}
    else if (strncmp(cmd, ".th", cmdlen) == 0) {}
    else if (strncmp(cmd, ".td", cmdlen) == 0) {}
    else if (strncmp(cmd, ".fig", cmdlen) == 0) {}
    else if (strncmp(cmd, ".!fig", cmdlen) == 0) {}
    else if (strncmp(cmd, ".bibliography", cmdlen) == 0) {}
    else if (strncmp(cmd, ".refdef", cmdlen) == 0) {}
    else if (strncmp(cmd, ".footnotedef", cmdlen) == 0) {}
    else
        fprintf(stderr, "Warning: unrecognized command: %.*s", cmdlen, cmd);

    return end + 1;
}

void
cmd_pagewidth(const char *args) {
    
}

