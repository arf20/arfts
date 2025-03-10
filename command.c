#include "command.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "doc.h"

void
cmd_pagewidth(const char *args, state_t *st, doc_format_t *fmt) {
    fmt->pagewidth = strtol(args, NULL, 10);
}

void
cmd_pageheight(const char *args, state_t *st, doc_format_t *fmt) {
    fmt->pageheight = strtol(args, NULL, 10);
}

void
cmd_tabstop(const char *args, state_t *st, doc_format_t *fmt) {
    fmt->tabstop = strtol(args, NULL, 10);
}

void
cmd_indent(const char *args, state_t *st, docentry_format_t *efmt) {
    args = strip(args);
    if (strncmp(args, "on", 2) == 0)
        efmt->indent = 1;
    else
        efmt->indent = 0;
}

void
cmd_header(const char *args, state_t *st, doc_format_t *fmt) {
    char *tok1, *tok2, *tok3;
    args = tokenize(args, &tok1);
    args = tokenize(args, &tok2);
    args = tokenize(args, &tok3);

    if (fmt->headerl || fmt->headerc || fmt->headerr)
        fprintf(stderr, "L%d: (W) Header redefined\n", st->linenum);

    if (tok1 && tok2 && tok3) {
        fmt->headerl = tok1;
        fmt->headerc = tok2;
        fmt->headerr = tok3;
    } else if (tok1 && tok2) {
        fmt->headerl = tok1;
        fmt->headerr = tok2;
    } else if (tok1) {
        fmt->headerl = tok1;
    } else {
        fprintf(stderr, "L%d: (E) Too few arguments for .header\n",
            st->linenum);
    }
}

void
cmd_footer(const char *args, state_t *st, doc_format_t *fmt) {
    char *tok1, *tok2, *tok3;
    args = tokenize(args, &tok1);
    args = tokenize(args, &tok2);
    args = tokenize(args, &tok3);

    if (fmt->footerl || fmt->footerc || fmt->footerr)
        fprintf(stderr, "L%d: (W) Footer redefined\n", st->linenum);

    if (tok1 && tok2 && tok3) {
        fmt->footerl = tok1;
        fmt->footerc = tok2;
        fmt->footerr = tok3;
    } else if (tok1 && tok2) {
        fmt->footerl = tok1;
        fmt->footerr = tok2;
    } else if (tok1) {
        fmt->footerl = tok1;
    } else {
        fprintf(stderr, "L%d: (E) Too few arguments for .footer\n",
            st->linenum);
    }
}

void
cmd_margin(const char *args, state_t *st, doc_format_t *fmt) {
    char *tok1, *tok2, *tok3, *tok4;
    args = tokenize(args, &tok1);
    args = tokenize(args, &tok2);
    args = tokenize(args, &tok3);
    args = tokenize(args, &tok4);

    if (!tok1 || !tok2 || !tok3 || !tok4) {
        fprintf(stderr, "L%d: (E) Too few arguments for .margin\n",
            st->linenum);
        return;
    }

    fmt->margint = strtol(tok1, NULL, 0);
    fmt->marginl = strtol(tok2, NULL, 0);
    fmt->marginb = strtol(tok3, NULL, 0);
    fmt->marginr = strtol(tok4, NULL, 0);
}

void
cmd_title(const char *args, state_t *st, doc_format_t *fmt) {
    args = strip(args);
    const char *end = strchr(args, '\n');
    fmt->title = strndup(args, end - args);
}

void
cmd_author(const char *args, state_t *st, doc_format_t *fmt) {
    args = strip(args);
    const char *end = strchr(args, '\n');
    fmt->author = strndup(args, end - args);
}

void
cmd_date(const char *args, state_t *st, doc_format_t *fmt) {
    args = strip(args);
    const char *end = strchr(args, '\n');
    fmt->date = strndup(args, end - args);
}

void
cmd_titlepage(const char *args, docentry_t **e) {
     *e = doc_insert_titlepage(*e);
}

void
cmd_part(const char *args, docentry_t **e) {
    args = strip(args);
    const char *end = strchr(args, '\n');
    *e = doc_insert_structure(*e, SPART, strndup(args, end - args));
}

void
cmd_chapter(const char *args, docentry_t **e) {
    args = strip(args);
    const char *end = strchr(args, '\n');
    *e = doc_insert_structure(*e, SCHAPTER, strndup(args, end - args));
}

void
cmd_section(const char *args, docentry_t **e) {
    args = strip(args);
    const char *end = strchr(args, '\n');
    *e = doc_insert_structure(*e, SSECTION, strndup(args, end - args));
}

void
cmd_subsection(const char *args, docentry_t **e) {
    args = strip(args);
    const char *end = strchr(args, '\n');
    *e = doc_insert_structure(*e, SSUBSECTION, strndup(args, end - args));
}

void
cmd_subsubsection(const char *args, docentry_t **e) {
    args = strip(args);
    const char *end = strchr(args, '\n');
    *e = doc_insert_structure(*e, SSUBSUBSECTION, strndup(args, end - args));
}


void
cmd_pagebreak(docentry_t **e) {
    *e = doc_insert_pagebreak(*e);
}

void
cmd_tableofcontents(docentry_t **e) {
    *e = doc_insert_tableofcontents(*e);
}

void
cmd_align(const char *args, state_t *st, docentry_format_t *efmt) {
    args = strip(args);
    const char *end = strchr(args, '\n');
    int arglen = end - args;

    if (strncmp(args, "justify", arglen) == 0) {
        efmt->align = AJUSTIFY;
    } else if (strncmp(args, "left", arglen) == 0) {
        efmt->align = ALEFT;
    } else if (strncmp(args, "center", arglen) == 0) {
        efmt->align = ACENTER;
    } else if (strncmp(args, "right", arglen) == 0) {
        efmt->align = ARIGHT;
    } else {
        fprintf(stderr, "L%d: Unrecognized option %.*s for .align\n",
            st->linenum, arglen, args);
    }
}

void
cmd_fig(const char *args, state_t *st, const docentry_format_t *efmt,
    docentry_t **e)
{
    args = strip(args);
    const char *end = strchr(args, '\n');
    st->in_fig = 1;
    *e = doc_insert_figure(*e, efmt, strndup(args, end - args));
}

void
cmd_itemize(const char *args, state_t *st, const docentry_format_t *efmt,
    docentry_t **e)
{
    args = strip(args);
    const char *end = strchr(args, '\n');
    *e = doc_insert_list(*e, efmt, LITEMIZE, strndup(args, end - args));
}

void
cmd_enumerate(const char *args, state_t *st, const docentry_format_t *efmt,
    docentry_t **e)
{
    args = strip(args);
    const char *end = strchr(args, '\n');
    *e = doc_insert_list(*e, efmt, LENUMERATE, strndup(args, end - args));
}

void
cmd_item(const char *args, state_t *st, docentry_t **e) {
    doc_list_insert(*e);
    st->in_item = 1;
}

void
cmd_table(const char *args, state_t *st, const docentry_format_t *efmt,
    docentry_t **e)
{
    args = strip(args);
    const char *end = strchr(args, '\n');
    st->in_table = 1;
    *e = doc_insert_table(*e, efmt, strndup(args, end - args));
}


const char*
interpret_command(const char *cmd, doc_format_t *fmt, docentry_format_t *efmt,
    state_t *st, docentry_t **e)
{
    const char *cmdend = strpbrk(cmd, " \n");
    int cmdlen = cmdend - cmd;
    const char *end = strchr(cmd, '\n');

    /* debug */
    #ifdef _DEBUG_STATE_
    printf("cmd: %.*s\n", cmdlen, cmd);
    #endif /* _DEBUG_STATE_ */

    if      (strncmp(cmd, ".pagewidth", cmdlen) == 0)
        cmd_pagewidth(cmd + cmdlen, st, fmt);
    else if (strncmp(cmd, ".pageheight", cmdlen) == 0)
        cmd_pageheight(cmd + cmdlen, st, fmt);
    else if (strncmp(cmd, ".tabstop", cmdlen) == 0)
        cmd_tabstop(cmd + cmdlen, st, fmt);
    else if (strncmp(cmd, ".indent", cmdlen) == 0)
        cmd_indent(cmd + cmdlen, st, efmt);
    else if (strncmp(cmd, ".header", cmdlen) == 0)
        cmd_header(cmd + cmdlen, st, fmt);
    else if (strncmp(cmd, ".footer", cmdlen) == 0)
        cmd_footer(cmd + cmdlen, st, fmt);
    else if (strncmp(cmd, ".margin", cmdlen) == 0)
        cmd_margin(cmd + cmdlen, st, fmt);
    else if (strncmp(cmd, ".title", cmdlen) == 0)
        cmd_title(cmd + cmdlen, st, fmt);
    else if (strncmp(cmd, ".author", cmdlen) == 0)
        cmd_author(cmd + cmdlen, st, fmt);
    else if (strncmp(cmd, ".date", cmdlen) == 0)
        cmd_date(cmd + cmdlen, st, fmt);
    else if (strncmp(cmd, ".titlepage", cmdlen) == 0)
        cmd_titlepage(cmd + cmdlen, e);
    else if (strncmp(cmd, ".pagebreak", cmdlen) == 0)
        cmd_pagebreak(e);
    else if (strncmp(cmd, ".tableofcontents", 16) == 0)
        cmd_tableofcontents(e);
    else if (strncmp(cmd, ".part", cmdlen) == 0)
        cmd_part(cmd + cmdlen, e);
    else if (strncmp(cmd, ".chapter", cmdlen) == 0)
        cmd_chapter(cmd + cmdlen, e);
    else if (strncmp(cmd, ".section", cmdlen) == 0)
        cmd_section(cmd + cmdlen, e);
    else if (strncmp(cmd, ".subsection", cmdlen) == 0)
        cmd_subsection(cmd + cmdlen, e);
    else if (strncmp(cmd, ".subsubsection", cmdlen) == 0)
        cmd_subsubsection(cmd + cmdlen, e);
    else if (strncmp(cmd, ".align", cmdlen) == 0)
        cmd_align(cmd + cmdlen, st, efmt);
    else if (strncmp(cmd, ".columns", cmdlen) == 0)
        /* todo */ {}
    else if (strncmp(cmd, ".itemize", 8) == 0)
        cmd_itemize(cmd + cmdlen, st, efmt, e);
    else if (strncmp(cmd, ".enumerate", cmdlen) == 0)
        cmd_enumerate(cmd + cmdlen, st, efmt, e);
    else if (strncmp(cmd, ".item", cmdlen) == 0)
        cmd_item(cmd + cmdlen, st, e);
    else if (strncmp(cmd, ".table", cmdlen) == 0)
        cmd_table(cmd + cmdlen, st, efmt, e);
    else if (strncmp(cmd, ".fig", cmdlen) == 0)
        cmd_fig(cmd + cmdlen, st, efmt, e);
    else if (strncmp(cmd, ".!fig", cmdlen) == 0) {}
    else if (strncmp(cmd, ".bibliography", cmdlen) == 0) {}
    else if (strncmp(cmd, ".refdef", cmdlen) == 0) {}
    else if (strncmp(cmd, ".footnotedef", cmdlen) == 0) {}
    else
        fprintf(stderr, "L%d: (W) Unrecognized command: %.*s\n", st->linenum,
            cmdlen, cmd);

    st->linenum++;

    return end + 1;
}

