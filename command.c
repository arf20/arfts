#include "command.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

void
cmd_pagewidth(const char *args, docconfig_t *cfg) {
    cfg->pagewidth = strtol(args, NULL, 10);
}

void
cmd_pageheight(const char *args, docconfig_t *cfg) {
    cfg->pageheight = strtol(args, NULL, 10);
}

void
cmd_tabstop(const char *args, docconfig_t *cfg) {
    cfg->pageheight = strtol(args, NULL, 10);
}

void
cmd_indentparagraph(const char *args, docconfig_t *cfg) {
    cfg->pageheight = strtol(args, NULL, 10);
}

void
cmd_header(const char *args, docconfig_t *cfg) {
    char *tok1, *tok2, *tok3;
    args = tokenize(args, &tok1);
    args = tokenize(args, &tok2);
    args = tokenize(args, &tok3);
}

void
cmd_footer(const char *args, docconfig_t *cfg) {
    char *tok1, *tok2, *tok3;
    args = tokenize(args, &tok1);
    args = tokenize(args, &tok2);
    args = tokenize(args, &tok3);
}

void
cmd_title(const char *args, docconfig_t *cfg) {
    const char *end = strchr(args, '\n');
    cfg->title = strndup(args, end - args);
}

void
cmd_author(const char *args, docconfig_t *cfg) {
    const char *end = strchr(args, '\n');
    cfg->author = strndup(args, end - args);
}

void
cmd_date(const char *args, docconfig_t *cfg) {
    const char *end = strchr(args, '\n');
    cfg->date = strndup(args, end - args);
}



const char*
interpret_command(const char *cmd, docconfig_t *cfg, state_t *st,
    docentry_t *doc)
{
    const char *cmdend = strpbrk(cmd, " \n");
    int cmdlen = cmdend - cmd;
    const char *end = strchr(cmd, '\n');

    /* debug */
    printf("cmd: %.*s\n", cmdlen, cmd);

    if      (strncmp(cmd, ".pagewidth", cmdlen) == 0)
        cmd_pagewidth(cmd + cmdlen, cfg);
    else if (strncmp(cmd, ".pageheight", cmdlen) == 0)
        cmd_pageheight(cmd + cmdlen, cfg);
    else if (strncmp(cmd, ".tabstop", cmdlen) == 0)
        cmd_tabstop(cmd + cmdlen, cfg);
    else if (strncmp(cmd, ".indentparagraph", cmdlen) == 0)
        cmd_indentparagraph(cmd + cmdlen, cfg);
    else if (strncmp(cmd, ".header", cmdlen) == 0)
        cmd_header(cmd + cmdlen, cfg);
    else if (strncmp(cmd, ".footer", cmdlen) == 0)
        cmd_footer(cmd + cmdlen, cfg);
    else if (strncmp(cmd, ".title", cmdlen) == 0)
        cmd_title(cmd + cmdlen, cfg);
    else if (strncmp(cmd, ".author", cmdlen) == 0)
        cmd_author(cmd + cmdlen, cfg);
    else if (strncmp(cmd, ".date", cmdlen) == 0)
        cmd_date(cmd + cmdlen, cfg);
    else if (strncmp(cmd, ".titlepage", cmdlen) == 0)
    {} //cmd_titlepage(cmd + cmdlen, cfg);
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

