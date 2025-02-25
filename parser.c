#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "command.h"

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

    docentry_table_t* et = (docentry_table_t*)e->data;

    /* count cols and rows */
    toff = strip(toff);
    const char *hend = strchr(toff, '\n');
    
    for (const char *t = toff; t < hend; t++) {
        if (*t == '|')
            et->ncols++;
    }
    et->ncols--;
    et->nrows = 1;

    const char *body = strip(hend + 1);
    if (*body == '-') {
        et->has_header = 1;
        body = strchr(body, '\n') + 1;
    }

    while (*(body = strip(body)) == '|') {
        et->nrows++;
        body = strchr(body, '\n') + 1;
    }

    /* allocate */
    et->cells =        malloc(sizeof(char*) * et->ncols * et->nrows);
    et->cell_widths =  malloc(sizeof(int) * et->ncols * et->nrows);
    et->cell_heights = malloc(sizeof(int) * et->ncols * et->nrows);

    /* copy words to matrix */
    const char *t = toff, *cend = NULL, *wend = NULL;
    for (int r = 0; r < et->nrows; r++) {
        if (et->has_header && r == 1)
            t = strchr(t, '\n') + 1;

        for (int c  = 0; c < et->ncols; c++) {
            t = strip(strchr(t, '|') + 1);
            cend = strchr(t, '|');

            char *ccur = et->cells[(r * et->ncols) + c] = malloc((cend - t) + 1);
            
            while (*t != '|') {
                wend = strpbrk(t, " |");
                strncpy(ccur, t, wend - t);
                ccur[wend - t] = ' ';
                ccur[(wend - t) + 1] = '\0';
                ccur = ccur + (wend - t) + 1;
                t = strip(wend);
            }

        }

        t = strchr(t, '\n') + 1;
    }


    
    st->in_table = 0;

    return t;
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

