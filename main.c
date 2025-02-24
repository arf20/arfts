#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "command.h"
#include "doc.h"
#include "genplain.h"

void
usage(char *argv0) {
    printf("usage: %s <file>\n\n", argv0);
}

const char*
read_figure(docentry_t *e, state_t *st, const char *figoff) {
    if (e->type != EFIGURE)
        return figoff;

    const char *end = strstr(figoff, ".!fig");
    if (!end) {
        fprintf(stderr, "L%d: No matching .!fig for .fig", st->linenum);
        return NULL;
    }

    ((docentry_figure_t*)e->data)->predata = 
        strndup(figoff, end - figoff);

    st->in_fig = 0;

    return end + 5;
}

const char*
read_table(docentry_t *e, state_t *st, const char *toff) {
     if (e->type != ETABLE)
        return toff;


}

const char*
paragraph_add_word(docentry_t *e, state_t *st, const char *wordoff) {
    if (e->type != EPARAGRAPH)
        return wordoff;

    int wordlen = strpbrk(wordoff, " \n") - wordoff;

    if (wordoff[wordlen] == '\n')
        st->linenum++;
    
    if (e->size + wordlen + 2 > e->capacity) {
        e->data = realloc(e->data, e->capacity * 2);
        e->capacity *= 2;
    }

    strncpy(e->data + e->size, wordoff, wordlen);
    e->data[e->size + wordlen] = ' ';
    e->data[e->size + wordlen + 1] = '\0';
    e->size += wordlen + 1;

    return wordoff + wordlen;
}

const char*
item_add_word(docentry_t *e, state_t *st, const char *wordoff) {
    if (e->type != ELIST)
        return wordoff;
    
    docentry_list_t *el = (docentry_list_t*)e->data;
    docentry_list_item_t *li = &el->items[el->count - 1];

    int wordlen = strpbrk(wordoff, " \n") - wordoff;

    if (wordoff[wordlen] == '\n')
        st->linenum++;
    
    if (li->size + wordlen + 2 > li->capacity) {
        li->content = realloc(li->content, li->capacity * 2);
        li->capacity *= 2;
    }

    strncpy(li->content + li->size, wordoff, wordlen);
    li->content[li->size + wordlen] = ' ';
    li->content[li->size + wordlen + 1] = '\0';
    li->size += wordlen + 1;

    return wordoff + wordlen;
}

void
parse_file(const char *fname, docconfig_t *cfg, docentry_t *doc) {
    FILE *f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        exit(1);
    }

    fseek(f, 0, SEEK_END); 
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *src = malloc(size + 1);
    fread(src, 1, size, f);
    src[size] = '\0';

    fclose(f);

    const char *cursor = src;

    state_t st = { 0 };
    st.linenum = 1;

    docentry_t *cur_entry = doc; /* current document entry */

    docentry_config_t ecfg = {
        .align = ALEFT,
        .indentparagraph = 1
    };

    while (cursor && *cursor) {
        if (st.in_fig) {
            cursor = read_figure(cur_entry, &st, cursor);
            continue;
        }

        if (st.in_table) {
            cursor = read_table(cur_entry, &st, cursor);
            continue;
        }   

        cursor = strip(cursor);

        if (*cursor == '.') {
            /* command */
            cursor = interpret_command(cursor, cfg, &ecfg, &st, &cur_entry);
        } else if (*cursor == '\n') {
            cursor++;
            st.linenum++;

            if (st.prev_nl) {
                /* break paragraph */
                st.in_item = 0;
                cur_entry = doc_insert_null(cur_entry);
            }

            st.prev_nl = 1;
        }
        else {
            /* item word */
            if (st.in_item) {
                cursor = item_add_word(cur_entry, &st, cursor);

            } else {
                /* body word */
                if (cur_entry->type != EPARAGRAPH)
                    cur_entry = doc_insert_paragraph(cur_entry, &ecfg);
                cursor = paragraph_add_word(cur_entry, &st, cursor);
            }

            st.prev_nl = 0;
        }
    }

    free(src);
}


void
doc_print(const docentry_t *doc) {
    int c = 0;
    for (const docentry_t *e = doc; e != NULL; e = e->n, c++) {
        fprintf(stderr, " doc[%d,%d]: %s",
            e->page, c, entrytype_names[e->type]);
        switch (e->type) {
            case ENULL: break;
            case EPARAGRAPH: {
                fprintf(stderr, " \"%.*s\"", (int)e->size, e->data);
            } break;
            case EFIGURE: {
                docentry_figure_t *ef = (docentry_figure_t*)e->data;
                fprintf(stderr, " \"%s\" {\n%s\n}", ef->caption, ef->predata);
            } break;
            case ESTRUCTURE: {
                docentry_structure_t *es = (docentry_structure_t*)e->data;
                fprintf(stderr, ": %s \"%s\"", structuretype_names[es->type],
                    es->heading);
            } break;
            case ELIST: {
                docentry_list_t* el = (docentry_list_t*)e->data;
                fputc('\n', stderr);
                for (size_t i = 0; i < el->count; i++)
                    fprintf(stderr, "-> %s\n", el->items[i].content);

            } break;
            case ETITLEPAGE: break;
            case EPAGEBREAK: break;
            case ETABLEOFCONTENTS: break;
        }
        fputc('\n', stderr);
    }
}

void
docconfig_print(const docconfig_t *cfg) {
    fprintf(stderr,
        "doc config:\n page dimensions: %dx%d\n tabstop: %s\n"
        " margins: %d %d %d %d\n"
        " header: (left) \"%s\" (center) \"%s\" (right) \"%s\"\n"
        " footer: (left) \"%s\" (center) \"%s\" (right) \"%s\"\n"
        " title: %s\n author: %s\n date: %s\n",
        cfg->pagewidth, cfg->pageheight, cfg->tabstop ? "true" : "false",
        cfg->margint, cfg->marginl, cfg->marginb, cfg->marginr,
        cfg->headerl, cfg->headerc, cfg->headerr,
        cfg->footerl, cfg->footerc, cfg->footerr,
        cfg->title, cfg->author, cfg->date
    );
}

int
main(int argc, char **argv) {
    if (argc != 2) {
        usage(*argv);
        exit(1);
    }

    docconfig_t cfg = { 0 };
    docentry_t *doc = doc_new();
    parse_file(argv[1], &cfg, doc);

    generate_plain(&cfg, doc, stdout);

    /* debug */
    doc_print(doc); 
    docconfig_print(&cfg);


    return 0;
}

