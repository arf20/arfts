#include "genplain.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "util.h"

/* layout computation */

int
text_countlines(int width, int indent, int tabstop, const char *text) {
    const char *s = strip(text), *next;
    int linec = 1, charc = -1; /* starts w no preword space */
    while (s && *s) {
        next = strchr(s, ' ');
        if (!next) {
            charc += strlen(s);
            next = (const char *)-1;
        }

        if (((charc + (next - s) + 1) > width) || (linec == 0 && indent &&
            (charc + tabstop + (next - s) + 1) > width))
        {
            linec++;
            charc = -1;
        }
 
        charc += (next - s) + 1;

        s = strip(next + 1);
    }

    return linec;
}

int
tableofcontents_countlines(int width, const docentry_t *doc)  {
    int linec = 0;
    for (const docentry_t *e = doc; e != NULL; e = e->n)
        if (e->type == ESTRUCTURE)
            linec++;
    return linec;
}

void
compute_layout(const doc_format_t *fmt, int width, int height, docentry_t *doc) {
    int page = 1, line = 0;

    docentry_t *l = NULL;

    for (docentry_t *e = doc; e != NULL; e = e->n) {
        switch (e->type) {
            case ENULL: break;
            case EPARAGRAPH: {
                e->height = text_countlines(width, e->efmt.indent,
                    fmt->tabstop, e->data) + 1; /* 1 line margin */
                e->width = width;
                if (line + e->height > height) {
                    page++;
                    line = 0;

                    if (l && l->type == ESTRUCTURE) {
                        l->page = page;
                        l->line = 0;
                        line = 2;
                    }
                }
                e->page = page;
                e->line = line;
                line += e->height;
            } break;
            case ESTRUCTURE: {
                e->height = 2;
                e->width = width;

                if (line + e->height >= height) {
                    page++;
                    line = 0;
                }

                e->page = page; /* set page */
                e->line = line;
                line += e->height;
            } break;
            case ETITLEPAGE: e->page = page++; line = 0; break;
            case ETABLEOFCONTENTS: {
                e->page = page;
                e->line = line;
                e->height = tableofcontents_countlines(width, doc);
                page += (e->height / height) + ((e->height % height) != 0);
                line = 0;
            } break;
            case EPAGEBREAK: {
                e->page = page;
                e->line = line;
                page++;
                line = 0;
            } break;
            case EFIGURE: {
                docentry_figure_t *ef = (docentry_figure_t*)e->data;
                e->height = 1;
                int col = 0;
                for (const char *f = ef->predata; *f; f++) {
                    if (*f == '\n') {
                        e->height++;
                        if (col > e->width)
                            e->width = col + 1;
                        col = 0;
                    } else
                        col++;
                }

                if (line + e->height >= height) {
                    page++;
                    line = 0;
                }

                e->page = page;
                e->line = line;
                line += e->height;
            } break;
            case ELIST: {
                docentry_list_t* el = (docentry_list_t*)e->data;

                e->height = 0;
                if (el->caption[0] == '\0')
                    e->height++;
                for (int i = 0; i < el->count; i++) {
                    e->height += text_countlines(width, e->efmt.indent,
                        fmt->tabstop, el->items[i].content);
                }

                if (line + e->height >= height) {
                    page++;
                    line = 0;
                }

                e->page = page;
                e->line = line;
                line += e->height;
            } break;
            case ETABLE: {
                docentry_table_t* et = (docentry_table_t*)e->data;
                
                /* max width of each column */
                for (int c = 0; c < et->ncols; c++) {
                    et->col_widths[c] = 0;
                    for (int r = 0; r < et->nrows; r++) {
                        int cellwidth =
                            strlen(et->cells[(r * et->ncols) + c]) - 1;
                        if (cellwidth > et->col_widths[c])
                            et->col_widths[c] = cellwidth;
                    }
                }

                /* sum of max widths is max width of table */
                int sum = 0;
                for (int c = 0; c < et->ncols; c++)
                    sum += et->col_widths[c];

                /* if table overflows, normalize col widths to avail width */
                if (((3 * et->ncols) + sum + 2) > width) {
                    float avail = width - (3 * et->ncols) - 2 -
                        (e->efmt.indent * fmt->tabstop);

                    for (int c = 0; c < et->ncols; c++)
                        et->col_widths[c] =
                            avail * ((float)et->col_widths[c] / (float)sum);
                }

                /* compute row heights */
                for (int r = 0; r < et->nrows; r++) {
                    et->row_heights[r] = 0;
                    for (int c = 0; c < et->ncols; c++) {
                        int cellheight = text_countlines(et->col_widths[c],
                                e->efmt.indent, fmt->tabstop,
                                et->cells[(r * et->ncols) + c]);
                        if (cellheight > et->row_heights[r])
                            et->row_heights[r] = cellheight;
                    }
                    if (et->row_heights[r] > 1)
                        et->has_interhbars = 1;
                }

                /* calculate table height and page fit */
                e->height = 1;
                if (!et->has_interhbars && et->has_header)
                    e->height++;
                for (int r = 0; r < et->nrows; r++) {
                    e->height += et->row_heights[r];
                    if (et->has_interhbars)
                        e->height++;
                }
                e->height++;
                if (et->caption && et->caption[0])
                    e->height++;

                if (line + e->height >= height) {
                    page++;
                    line = 0;
                }

                e->page = page;
                e->line = line;
                line += e->height;
            } break;
        }

        l = e;
    }
}

int
find_toplvl_index(const docentry_t *doc) {
    /* find top level index */
    int toplvl = 5;
    for (const docentry_t *e = doc; e != NULL; e = e->n) {
        if (e->type != ESTRUCTURE) continue;
        int type = ((const docentry_structure_t*)e->data)->type;
        if (type < toplvl)
            toplvl = type;
    }
    return toplvl;
}

/* printing utils */

void
print_lf(FILE *o) {
    fputc('\n', o);
}

void
print_pb(FILE *o) {
    fputc('\f', o);
}

void
print_n_c(char c, int n, FILE *o) {
   for (int i = 0; i < n; i++)
       fputc(c, o);
}

void
print_tab(const doc_format_t *fmt, FILE *o) {
    print_n_c(' ', fmt->tabstop, o);
}

void
print_marginl(const doc_format_t *fmt, FILE *o) {
    print_n_c(' ', fmt->marginl, o);
}

void
print_margint(const doc_format_t *fmt, FILE *o) {
    print_n_c('\n', fmt->margint, o);
}

void
print_marginb(const doc_format_t *fmt, FILE *o) {
    print_n_c('\n', fmt->marginb, o);
}

void
print_text_lf(const char *l, FILE *o) {
    fputs(l, o);
    print_lf(o);
}

void
print_centered_text_lf(const char *l, int width, FILE *o) {
    int len = strlen(l);
    int spacel = (width - len) / 2;
    for (int i = 0; i < spacel; i++)
        fputc(' ', o);
    fputs(l, o);
    print_lf(o);
}

/* consumes one line of width */
const char *
print_ln_nolf(const char *txt, const docentry_format_t *efmt, int width, FILE *o) {
    int lwlen = 0, llen = 0, gaps = 0;
    const char *pos = txt;
    /* count words that fit in the line */
    while (pos != (void*)1 && *pos != '\0') {
        const char *next = strpbrk(pos, " \0");
        int wlen = count_utf8_code_points_n(pos, next - pos);
        if (lwlen + wlen > width)
            break;
        lwlen += wlen + 1;
        llen += next - pos + 1;
        gaps++;
        pos = next + 1;
    }
    lwlen--; /* no ending space */
    llen--;
    gaps--;

    switch (efmt->align) {
        case ALEFT: fprintf(o, "%.*s", llen, txt); break;
        case ARIGHT: {
            print_n_c(' ', width - lwlen, o);
            fprintf(o, "%.*s", llen, txt);
        } break;
        case ACENTER: {
            print_n_c(' ', (width - lwlen) / 2, o);
            fprintf(o, "%.*s", llen, txt);
        } break;
        case AJUSTIFY: {
            int tspacing = width - lwlen + gaps;
            float spacingpergap = (float)tspacing / (float)gaps;
            int linepos = 0, cc = 0, wc = 0;
            const char *pos = txt;
            while (pos != (void*)1) {
                const char *next = strpbrk(pos, " \0");
                int wlen = count_utf8_code_points_n(pos, next - pos);
                if (linepos + wlen > width)
                    break;
                linepos += wlen;
                cc += wlen;
                wc++;
                fprintf(o, "%.*s", (int)(next - pos), pos);

                if (wc < gaps + 1) {
                    int nextspacing = (cc + roundf((float)wc * spacingpergap)) -
                        linepos;
                    print_n_c(' ', nextspacing, o);
                    linepos += nextspacing;
                }

                pos = next + 1;
            }

            print_n_c(' ', width - linepos, o);

        } break;
    }


    return txt + llen;
}

const char *
print_ln(const char *txt, const docentry_format_t *efmt, int width, FILE *o) {
    txt = strip(txt);
    if (*txt == '\0')
        return txt;

    const char *r = print_ln_nolf(txt, efmt, width, o);
    print_lf(o);
    return r;
}


void
print_header(const doc_format_t *fmt, int page, FILE *o) {
    static char pagenumbuff[16], pagenumextbuff[16];
    char *line = malloc(fmt->pagewidth + 1);
    memset(line, ' ', fmt->pagewidth);
    line[fmt->pagewidth] = '\0';

    snprintf(pagenumbuff, 16, "%d", page);
    snprintf(pagenumextbuff, 16, "PAGE %d", page);

    char *headerl = NULL, *headerc = NULL, *headerr = NULL;
   
    if (fmt->headerl) {
        if (strcmp(fmt->headerl, "pagenum") == 0)
            headerl = pagenumbuff;
        else if (strcmp(fmt->headerl, "pagenumext") == 0)
            headerl = pagenumextbuff;
        else
            headerl = fmt->headerl;
    }

    if (fmt->headerc) {
        if (strcmp(fmt->headerc, "pagenum") == 0)
            headerc = pagenumbuff;
        else if (strcmp(fmt->headerc, "pagenumext") == 0)
            headerc = pagenumextbuff;
        else
            headerc = fmt->headerc;
    }

    if (fmt->headerr) {
        if (strcmp(fmt->headerr, "pagenum") == 0)
            headerr = pagenumbuff;
        else if (strcmp(fmt->headerr, "pagenumext") == 0)
            headerr = pagenumextbuff;
        else
            headerr = fmt->headerr;
    }


    if (headerl) {
        memcpy(line, headerl, MIN(strlen(headerl), fmt->pagewidth));
    }
    if (headerc) {
        int len = strlen(headerc);
        int spacel = (fmt->pagewidth - len) / 2;
        memcpy(line + spacel, headerc,
            MIN(strlen(headerc), fmt->pagewidth - spacel));
    }
    if (headerr) {
        int len = strlen(headerr);
        int spacel = fmt->pagewidth - len;
        memcpy(line + spacel, headerr,
            MIN(strlen(headerr), fmt->pagewidth - spacel));
    }

    print_text_lf(line, o);
    free(line);
}

void
print_footer(const doc_format_t *fmt, int page, FILE *o) {
    static char pagenumbuff[16], pagenumextbuff[16];
    char *line = malloc(fmt->pagewidth + 1);
    memset(line, ' ', fmt->pagewidth);
    line[fmt->pagewidth + 1] = '\0';

    snprintf(pagenumbuff, 16, "%d", page);
    snprintf(pagenumextbuff, 16, "PAGE %d", page);

    char *footerl = NULL, *footerc = NULL, *footerr = NULL;
   
    if (fmt->footerl) {
        if (strcmp(fmt->footerl, "pagenum") == 0)
            footerl = pagenumbuff;
        else if (strcmp(fmt->footerl, "pagenumext") == 0)
            footerl = pagenumextbuff;
        else
            footerl = fmt->footerl;
    }

    if (fmt->footerc) {
        if (strcmp(fmt->footerc, "pagenum") == 0)
            footerc = pagenumbuff;
        else if (strcmp(fmt->footerc, "pagenumext") == 0)
            footerc = pagenumextbuff;
        else
            footerc = fmt->footerc;
    }

    if (fmt->footerr) {
        if (strcmp(fmt->footerr, "pagenum") == 0)
            footerr = pagenumbuff;
        else if (strcmp(fmt->footerr, "pagenumext") == 0)
            footerr = pagenumextbuff;
        else
            footerr = fmt->footerr;
    }


    if (footerl) {
        memcpy(line, footerl, MIN(strlen(footerl), fmt->pagewidth));
    }
    if (footerc) {
        int len = strlen(footerc);
        int spacel = (fmt->pagewidth - len) / 2;
        memcpy(line + spacel, footerc,
            MIN(strlen(footerc), fmt->pagewidth - spacel));
    }
    if (footerr) {
        int len = strlen(footerr);
        int spacel = fmt->pagewidth - len;
        memcpy(line + spacel, footerr,
            MIN(strlen(footerr), fmt->pagewidth - spacel));
    }

    print_text_lf(line, o);
    free(line);
}

/* generation */

void
print_titlepage(const doc_format_t *fmt, int width, int height,
    const docentry_t *tp, FILE *o)
{
    /* center vertical */
    for (int i = 0; i < height / 3; i++)
        print_lf(o);

    print_marginl(fmt, o);
    print_centered_text_lf(fmt->title, width, o);
    print_marginl(fmt, o);
    print_centered_text_lf(fmt->author, width, o);
    print_marginl(fmt, o);
    print_centered_text_lf(fmt->date, width, o);
}

void
print_tableofcontents(const doc_format_t *fmt, int width, int height,
    int toplvl, const docentry_t *toc, const docentry_t *doc, FILE *o)
{
    print_marginl(fmt, o);
    print_text_lf("TABLE OF CONTENTS", o);

    int indices[] = {
        0, 0, 0, 0, 0 /* part, chapter, section, subsection, subsubsection */
    };

    char pagenum[16];

    const docentry_structure_t *es = NULL;
    for (const docentry_t *e = doc; e != NULL; e = e->n) {
        if (e->type != ESTRUCTURE) continue;
        es = (const docentry_structure_t*)e->data;

        indices[es->type]++;
        for (int i = es->type + 1; i < 5; i++)
            indices[i] = 0;

        print_marginl(fmt, o);
        
        int charc = 0;
        /* print indices */
        for (int i = toplvl; i < es->type; i++) {
            print_tab(fmt, o);
            charc += fmt->tabstop;
        }
        for (int i = toplvl; i <= es->type; i++)
            charc += fprintf(o, "%d.", indices[i]);
        
        fputc(' ', o);
        charc++;

        /* print title */
        fprintf(o, "%s", es->heading);
        charc += count_utf8_code_points(es->heading);
        fputc(' ', o);
        charc++;

        /* print .s */
        /*  compute num len */
        int numlen = snprintf(pagenum, 16, "%d", e->page);
        for (int i = 0; i < width - charc - numlen - 1; i++)
            fputc('.', o);
        fputc(' ', o);

        /* print page num */
        fputs(pagenum, o);

        print_lf(o);
    }

}

void
print_structure(const doc_format_t *fmt, int width, int toplvl, int indices[5],
    const docentry_t *e, FILE *o)
{
    int type = ((const docentry_structure_t*)e->data)->type;

    print_marginl(fmt, o);

    for (int i = toplvl; i <= type; i++)
        fprintf(o, "%d.", indices[i]);
    fputc(' ', o);

    print_text_lf(((const docentry_structure_t*)e->data)->heading, o);

    print_lf(o);
}

void
print_paragraph(const doc_format_t *fmt, int width, const docentry_t *par,
    FILE *o)
{
    const char *s = par->data;

    print_marginl(fmt, o);

    /* indent */
    int firstlnw = width;
    if (par->efmt.indent && par->efmt.align != ACENTER) {
        if (par->efmt.align != ARIGHT)
            print_tab(fmt, o);
        firstlnw = width - fmt->tabstop;
    }

    s = print_ln(s, &par->efmt, firstlnw, o);
    while (s && *s) {
        print_marginl(fmt, o);
        s = print_ln(s, &par->efmt, width, o);
    }

    print_lf(o);
}

void
print_figure(const doc_format_t *fmt, int width, int fignum,
const docentry_t *fig, FILE *o)
{
    docentry_figure_t *ef = (docentry_figure_t*)fig->data;
    const char *line = ef->predata;

    while (line && *line) {
        if (*line == '\n') {
            line++;
            print_lf(o);
            continue;
        }
        int linelen = strpbrk(line, "\n\0") - line;

        print_marginl(fmt, o);
        if (fig->efmt.indent)
            print_tab(fmt, o);
        fprintf(o, "%.*s", linelen, line);

        line += linelen;
    }

    if (ef->caption && *ef->caption) {
        print_marginl(fmt, o);
        if (fig->efmt.indent)
            print_tab(fmt, o);
        fprintf(o, "Fig %d. %s\n", fignum, ef->caption);
    }
    print_lf(o);
}

void
print_list(const doc_format_t *fmt, int width, const docentry_t *e, FILE *o) {
    docentry_list_t* el = (docentry_list_t*)e->data;
    print_marginl(fmt, o);
    if (el->caption[0] != '\0')
        fprintf(o, "%s\n", el->caption);
    for (int i = 0; i < el->count; i++) {
        print_marginl(fmt, o);
        int fltab = 0;
        if (el->type == LITEMIZE) {
            fltab = MAX(0, fmt->tabstop - 2);
            print_n_c(' ', fltab, o);
            fltab += fprintf(o, "- ");
        } else {
            fltab = MAX(0, fmt->tabstop - 2 - num_places(i + 1));
            print_n_c(' ', fltab, o);
            fltab += fprintf(o, "%d. ", i + 1);
        }
        const char *s = el->items[i].content;
        s = print_ln(s, &e->efmt, width - fltab, o);
        while (s && *s) {
            print_marginl(fmt, o);
            print_tab(fmt, o);
            s = print_ln(s, &e->efmt, width - fmt->tabstop, o);
        }
        print_lf(o);
    }
    print_lf(o);
}

/* table stuff */

void
print_table_hbar(char bc, int ncols, int *col_widths, const doc_format_t *fmt,
    const docentry_format_t *efmt, FILE *o)
{
    print_marginl(fmt, o);
    if (efmt->indent)
        print_tab(fmt, o);

    for (int c = 0; c < ncols; c++) {
        fprintf(o, "+%c", bc);
        print_n_c(bc, col_widths[c], o);
        fprintf(o, "%c", bc);
    }
    fprintf(o, "+\n");
}

void
print_table_row(int ncols, int *col_widths, int row_height,
    const char * const *cells, const doc_format_t *fmt,
    const docentry_format_t *efmt, FILE *o)
{
    const char **cell_curs = malloc(sizeof(char**) * ncols);
    memcpy(cell_curs, cells, sizeof(char**) * ncols);

    for (int l = 0; l < row_height; l++) {
        print_marginl(fmt, o);
        if (efmt->indent)
            print_tab(fmt, o);

        for (int c = 0; c < ncols; c++) {
            fprintf(o, "| ");
            //fprintf(o, cells[c]);
            cell_curs[c] = print_ln_nolf(cell_curs[c], efmt, col_widths[c], o);
            fprintf(o, " ");
        }
        fprintf(o, "|\n");
    }
}

void
print_table(const doc_format_t *fmt, int width, int tnum, const docentry_t *e,
    FILE *o)
{
    docentry_table_t* et = (docentry_table_t*)e->data;

    /* top hbar */
    print_table_hbar('-', et->ncols, et->col_widths, fmt, &e->efmt, o);
    /* first row */
    print_table_row(et->ncols, et->col_widths, et->row_heights[0],
        (const char **)&et->cells[0], fmt, &e->efmt, o);
    /* header or normal hbar */
    if (et->has_header)
        print_table_hbar('=', et->ncols, et->col_widths, fmt, &e->efmt, o);
    for (int r = 1; r < et->nrows; r++) {
        /* rest of rows */
        print_table_row(et->ncols, et->col_widths, et->row_heights[r],
            (const char **)&et->cells[r * et->ncols], fmt, &e->efmt, o);
        /* after row hbar */
        if (et->has_interhbars && (r < et->nrows - 1))
            print_table_hbar('-', et->ncols, et->col_widths, fmt, &e->efmt, o);
    }
    /* bottom hbar */
    print_table_hbar('-', et->ncols, et->col_widths, fmt, &e->efmt, o);
    /* table caption */
    if (et->caption && *et->caption != '\0') {
        print_marginl(fmt, o);
         if (e->efmt.indent)
            print_tab(fmt, o);
        fprintf(o, "Table %d. %s\n", tnum, et->caption);
    }
    print_lf(o);
}

void
generate_plain(const doc_format_t *fmt, docentry_t *doc, FILE *o) {
    int width = fmt->pagewidth - fmt->marginl - fmt->marginr;
    int height = fmt->pageheight - fmt->margint - fmt->marginb;
    if (fmt->headerl) height -= 2;
    if (fmt->footerl) height -= 2;

    int toplvl = find_toplvl_index(doc);

    compute_layout(fmt, width, height, doc);

    for (const docentry_t *e = doc; e != NULL; e = e->n) {
        /* first page header */
        if (e == doc) {
            print_header(fmt, e->page, o);
            print_margint(fmt, o);
        }

        switch (e->type) {
            case ENULL: break;
            case EPARAGRAPH: {
                print_paragraph(fmt, width, e, o);
                //linec += e->height + 1; /* interparagraph margin */
            } break;
            case ESTRUCTURE: {
                static int indices[] = { 0, 0, 0, 0, 0 };
                int type = ((const docentry_structure_t*)e->data)->type;
                indices[type]++;
                for (int i = type + 1; i < 5; i++)
                    indices[i] = 0;

                print_structure(fmt, width, toplvl, indices, e, o);
                //linec += e->height; 

            } break;
            case ETITLEPAGE: {
                print_titlepage(fmt, width, height, e, o);
                //linec = height; /* force new page */
            } break;
            case ETABLEOFCONTENTS: {
                print_tableofcontents(fmt, width, height, toplvl, e, doc, o);
                //linec = height; /* force new page */
            } break;
            case EFIGURE: {
                static int fignum = 1;
                print_figure(fmt, width, fignum, e, o);
                if (strlen(((docentry_figure_t*)e->data)->caption) > 0)
                    fignum++;
            } break;
            case EPAGEBREAK: {
                //linec = height;
            } break;
            case ELIST: {
                print_list(fmt, width, e, o);
            } break;
            case ETABLE: {
                static int tnum = 1;
                print_table(fmt, width, tnum, e, o);
                if (strlen(((docentry_table_t*)e->data)->caption) > 0)
                    tnum++;
            } break;
        }

        /* end page, next one */
        if (e->n && (e->page < e->n->page)) {
            print_marginb(fmt, o);
            if (fmt->footerl)
                print_footer(fmt, e->page, o);
            print_pb(o);
            if (fmt->headerl)
                print_header(fmt, e->n->page, o);
            print_margint(fmt, o);
        }
    }
}
    
