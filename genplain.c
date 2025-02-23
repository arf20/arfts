#include "genplain.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "util.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/* layout computation */

int
paragraph_countlines(int width, int indent, int tabstop, const docentry_t *p) {
    const char *s = p->data, *next;
    int linec = 1, charc = 0;
    while (s) {
        next = strchr(s, ' ');
        if (!next) {
            charc += strlen(s) + 1;
            next = (const char *)-1;
        }
        else
            charc += (next - s) + 1; /* 1 space */

        if ((charc >= width) || (linec == 0 && indent &&
            charc + tabstop >= width))
        {
            linec++;
            charc = 0;
        }
 
        s = next + 1;
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
compute_layout(const docconfig_t *cfg, int width, int height, docentry_t *doc) {
    int page = 1, line = 0;

    docentry_t *l = NULL;

    for (docentry_t *e = doc; e != NULL; e = e->n) {
        switch (e->type) {
            case EPARAGRAPH: {
                e->height = paragraph_countlines(width, e->ecfg.indentparagraph,
                    cfg->tabstop, e);
                if (line + e->height > height) {
                    page++;
                    line = 0;

                    if (l && l->type == ESTRUCTURE) {
                        l->page = page;
                        line = 2;
                    }
                }
                e->page = page;
                e->line = line;
                line += e->height + 1; /* 1 line margin */
                
            } break;
            case ESTRUCTURE: {
                if (line + 2 >= height) {
                    page++;
                    line = 0;
                }
                line += 2;

                e->page = page; /* set page */
                e->line = line;
                e->height = 2;

            } break;
            case ETITLEPAGE: e->page = page++; line = 0; break;
            case ETABLEOFCONTENTS: {
                e->page = page;
                e->line = line;
                int toclines = tableofcontents_countlines(width, doc);
                page += (toclines / height) + ((toclines % height) != 0);
                line = 0;
                e->height = toclines;
            } break;
            case EPAGEBREAK: {
                page++;
                line = 0;
                e->page = page;
                e->line = 0;
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

                line += e->height;
                e->page = page;
            } break;
            case ELIST: {
                docentry_list_t* el = (docentry_list_t*)e->data;
                e->height = 0;
                if (el->caption[0] == '\0')
                    e->height++;
                e->height += el->count;

                if (line + e->height >= height) {
                    page++;
                    line = 0;
                }
                
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
print_tab(const docconfig_t *cfg, FILE *o) {
    print_n_c(' ', cfg->tabstop, o);
}

void
print_marginl(const docconfig_t *cfg, FILE *o) {
    print_n_c(' ', cfg->marginl, o);
}

void
print_margint(const docconfig_t *cfg, FILE *o) {
    print_n_c('\n', cfg->margint, o);
}

void
print_marginb(const docconfig_t *cfg, FILE *o) {
    print_n_c('\n', cfg->marginb, o);
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
print_ln(const char *txt, const docentry_config_t *ecfg, int width, FILE *o) {
    txt = strip(txt);
    if (*txt == '\0') return txt;
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

    switch (ecfg->align) {
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
            int tspacing = width - lwlen;
            float spacingpergap = (float)tspacing / (float)gaps;
            float t = 0.0f;
            lwlen = 0;
            const char *pos = txt;
            while (pos != (void*)1) {
                const char *next = strpbrk(txt, " \0");
                int wlen = next - pos;
                if (lwlen + wlen > width)
                    break;
                lwlen += wlen;
                fprintf(o, "%.*s", wlen, pos);
                if ((int)t != (int)(t + spacingpergap))
                    fputc(' ', o);


                pos = next + 1;
                t += spacingpergap;
            }

        } break;
    }

    print_lf(o);

    return txt + llen;
}

void
print_header(const docconfig_t *cfg, int page, FILE *o) {
    static char pagenumbuff[16], pagenumextbuff[16];
    char *line = malloc(cfg->pagewidth + 1);
    memset(line, ' ', cfg->pagewidth);
    line[cfg->pagewidth] = '\0';

    snprintf(pagenumbuff, 16, "%d", page);
    snprintf(pagenumextbuff, 16, "PAGE %d", page);

    char *headerl = NULL, *headerc = NULL, *headerr = NULL;
   
    if (cfg->headerl) {
        if (strcmp(cfg->headerl, "pagenum") == 0)
            headerl = pagenumbuff;
        else if (strcmp(cfg->headerl, "pagenumext") == 0)
            headerl = pagenumextbuff;
        else
            headerl = cfg->headerl;
    }

    if (cfg->headerc) {
        if (strcmp(cfg->headerc, "pagenum") == 0)
            headerc = pagenumbuff;
        else if (strcmp(cfg->headerc, "pagenumext") == 0)
            headerc = pagenumextbuff;
        else
            headerc = cfg->headerc;
    }

    if (cfg->headerr) {
        if (strcmp(cfg->headerr, "pagenum") == 0)
            headerr = pagenumbuff;
        else if (strcmp(cfg->headerr, "pagenumext") == 0)
            headerr = pagenumextbuff;
        else
            headerr = cfg->headerr;
    }


    if (headerl) {
        memcpy(line, headerl, MIN(strlen(headerl), cfg->pagewidth));
    }
    if (headerc) {
        int len = strlen(headerc);
        int spacel = (cfg->pagewidth - len) / 2;
        memcpy(line + spacel, headerc,
            MIN(strlen(headerc), cfg->pagewidth - spacel));
    }
    if (headerr) {
        int len = strlen(headerr);
        int spacel = cfg->pagewidth - len;
        memcpy(line + spacel, headerr,
            MIN(strlen(headerr), cfg->pagewidth - spacel));
    }

    print_text_lf(line, o);
    free(line);
}

void
print_footer(const docconfig_t *cfg, int page, FILE *o) {
    static char pagenumbuff[16], pagenumextbuff[16];
    char *line = malloc(cfg->pagewidth + 1);
    memset(line, ' ', cfg->pagewidth);
    line[cfg->pagewidth + 1] = '\0';

    snprintf(pagenumbuff, 16, "%d", page);
    snprintf(pagenumextbuff, 16, "PAGE %d", page);

    char *footerl = NULL, *footerc = NULL, *footerr = NULL;
   
    if (cfg->footerl) {
        if (strcmp(cfg->footerl, "pagenum") == 0)
            footerl = pagenumbuff;
        else if (strcmp(cfg->footerl, "pagenumext") == 0)
            footerl = pagenumextbuff;
        else
            footerl = cfg->footerl;
    }

    if (cfg->footerc) {
        if (strcmp(cfg->footerc, "pagenum") == 0)
            footerc = pagenumbuff;
        else if (strcmp(cfg->footerc, "pagenumext") == 0)
            footerc = pagenumextbuff;
        else
            footerc = cfg->footerc;
    }

    if (cfg->footerr) {
        if (strcmp(cfg->footerr, "pagenum") == 0)
            footerr = pagenumbuff;
        else if (strcmp(cfg->footerr, "pagenumext") == 0)
            footerr = pagenumextbuff;
        else
            footerr = cfg->footerr;
    }


    if (footerl) {
        memcpy(line, footerl, MIN(strlen(footerl), cfg->pagewidth));
    }
    if (footerc) {
        int len = strlen(footerc);
        int spacel = (cfg->pagewidth - len) / 2;
        memcpy(line + spacel, footerc,
            MIN(strlen(footerc), cfg->pagewidth - spacel));
    }
    if (footerr) {
        int len = strlen(footerr);
        int spacel = cfg->pagewidth - len;
        memcpy(line + spacel, footerr,
            MIN(strlen(footerr), cfg->pagewidth - spacel));
    }

    print_text_lf(line, o);
    free(line);
}

/* generation */

void
print_titlepage(const docconfig_t *cfg, int width, int height,
    const docentry_t *tp, FILE *o)
{
    /* center vertical */
    for (int i = 0; i < height / 3; i++)
        print_lf(o);

    print_marginl(cfg, o);
    print_centered_text_lf(cfg->title, width, o);
    print_marginl(cfg, o);
    print_centered_text_lf(cfg->author, width, o);
    print_marginl(cfg, o);
    print_centered_text_lf(cfg->date, width, o);
}

void
print_tableofcontents(const docconfig_t *cfg, int width, int height,
    int toplvl, const docentry_t *toc, const docentry_t *doc, FILE *o)
{
    print_marginl(cfg, o);
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

        print_marginl(cfg, o);
        
        int charc = 0;
        /* print indices */
        for (int i = toplvl; i < es->type; i++) {
            print_tab(cfg, o);
            charc += cfg->tabstop;
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
print_structure(const docconfig_t *cfg, int width, int toplvl, int indices[5],
    const docentry_t *e, FILE *o)
{
    int type = ((const docentry_structure_t*)e->data)->type;

    print_marginl(cfg, o);

    for (int i = toplvl; i <= type; i++)
        fprintf(o, "%d.", indices[i]);
    fputc(' ', o);

    print_text_lf(((const docentry_structure_t*)e->data)->heading, o);

    print_lf(o);
}

void
print_paragraph(const docconfig_t *cfg, int width, const docentry_t *par,
    FILE *o)
{
    const char *s = par->data;

    print_marginl(cfg, o);

    /* indent */
    int firstlnw = width;
    if (par->ecfg.indentparagraph && par->ecfg.align != ACENTER) {
        if (par->ecfg.align != ARIGHT)
            print_tab(cfg, o);
        firstlnw = width - cfg->tabstop;
    }

    s = print_ln(s, &par->ecfg, firstlnw, o);
    while (s && *s) {
        print_marginl(cfg, o);
        s = print_ln(s, &par->ecfg, width, o);
    }

    print_lf(o);
}

void
print_figure(const docconfig_t *cfg, int width, int fignum,
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

        print_marginl(cfg, o);
        if (fig->ecfg.indentparagraph)
            print_tab(cfg, o);
        fprintf(o, "%.*s", linelen, line);

        line += linelen;
    }

    if (ef->caption && *ef->caption) {
        print_marginl(cfg, o);
        if (fig->ecfg.indentparagraph)
            print_tab(cfg, o);
        fprintf(o, "Fig %d. %s\n", fignum, ef->caption);
    }
    print_lf(o);
}

void
print_list(const docconfig_t *cfg, int width, const docentry_t *e, FILE *o) {
    docentry_list_t* el = (docentry_list_t*)e->data;
    if (el->caption[0] != '\0')
        fprintf(o, "%s\n", el->caption);
    for (int i = 0; i < el->count; i++) {
        if (el->type == LITEMIZE) {
            print_n_c(' ', cfg->tabstop - 2, o);
            fprintf(o, "- %s\n", el->items[i]);
        } else {
            print_n_c(' ', cfg->tabstop - 3, o);
            fprintf(o, "%d. %s\n", i + 1, el->items[i]);
        }
    }
}

void
generate_plain(const docconfig_t *cfg, docentry_t *doc, FILE *o) {
    int width = cfg->pagewidth - cfg->marginl - cfg->marginr;
    int height = cfg->pageheight - cfg->margint - cfg->marginb;
    if (cfg->headerl) height -= 2;
    if (cfg->footerl) height -= 2;

    int toplvl = find_toplvl_index(doc);

    compute_layout(cfg, width, height, doc);

    for (const docentry_t *e = doc; e != NULL; e = e->n) {
        switch (e->type) {
            case ENULL: break;
            case EPARAGRAPH: {
                print_paragraph(cfg, width, e, o);
                //linec += e->height + 1; /* interparagraph margin */
            } break;
            case ESTRUCTURE: {
                static int indices[] = { 0, 0, 0, 0, 0 };
                int type = ((const docentry_structure_t*)e->data)->type;
                indices[type]++;
                for (int i = type + 1; i < 5; i++)
                    indices[i] = 0;

                print_structure(cfg, width, toplvl, indices, e, o);
                //linec += e->height; 

            } break;
            case ETITLEPAGE: {
                print_titlepage(cfg, width, height, e, o);
                //linec = height; /* force new page */
            } break;
            case ETABLEOFCONTENTS: {
                print_tableofcontents(cfg, width, height, toplvl, e, doc, o);
                //linec = height; /* force new page */
            } break;
            case EFIGURE: {
                static int fignum = 1;
                print_figure(cfg, width, fignum, e, o);
                if (strlen(((docentry_figure_t*)e->data)->caption) > 0)
                    fignum++;
            } break;
            case EPAGEBREAK: {
                //linec = height;
            } break;
            case ELIST: {
                print_list(cfg, width, e, o);
            } break;
        }

        /* end page, next one */
        if (e->n && (e->page < e->n->page)) {
            print_marginb(cfg, o);
            if (cfg->footerl)
                print_footer(cfg, e->page, o);
            print_pb(o);
            if (cfg->headerl)
                print_header(cfg, e->page, o);
            print_margint(cfg, o);
        }
    }
}
    
