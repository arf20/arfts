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
    int linec = 0, charc = 0;
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

    for (docentry_t *e = doc; e != NULL; e = e->n) {
        switch (e->type) {
            case EPARAGRAPH: {
                e->height = paragraph_countlines(width, cfg->indentparagraph,
                    cfg->tabstop, e);
                if (line + e->height > height) {
                    page++;
                    line = 0;
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

                e->page = page; /* set page for tableofcontents */

            } break;
            case ETITLEPAGE: e->page = page++; line = 0; break;
            case ETABLEOFCONTENTS: {
                e->page = page;
                int toclines = tableofcontents_countlines(width, doc);
                page += (toclines / height) + ((toclines % height) != 0);
                line = 0;
            } break;
            case EPAGEBREAK: {
                page++;
                line = 0;
                e->page = page;
            } break;
        }
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
print_tab(const docconfig_t *cfg, FILE *o) {
    for (int i = 0; i < cfg->tabstop; i++)
        fputc(' ', o);
}

void
print_marginl(const docconfig_t *cfg, FILE *o) {
    for (int i = 0; i < cfg->marginl; i++)
        fputc(' ', o);
}

void
print_margint(const docconfig_t *cfg, FILE *o) {
    for (int i = 0; i < cfg->margint; i++)
        fputc('\n', o);
}

void
print_marginb(const docconfig_t *cfg, FILE *o) {
    for (int i = 0; i < cfg->marginb; i++)
        fputc('\n', o);
}

void
print_ln(const char *l, FILE *o) {
    fputs(l, o);
    print_lf(o);
}

void
print_ln_center(const char *l, int width, FILE *o) {
    int len = strlen(l);
    int spacel = (width - len) / 2;
    for (int i = 0; i < spacel; i++)
        fputc(' ', o);
    fputs(l, o);
    print_lf(o);
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

    print_ln(line, o);
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

    print_ln(line, o);
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
    print_ln_center(cfg->title, width, o);
    print_marginl(cfg, o);
    print_ln_center(cfg->author, width, o);
    print_marginl(cfg, o);
    print_ln_center(cfg->date, width, o);
}

void
print_tableofcontents(const docconfig_t *cfg, int width, int height,
    int toplvl, const docentry_t *toc, const docentry_t *doc, FILE *o)
{
    print_marginl(cfg, o);
    print_ln("TABLE OF CONTENTS", o);

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

    print_ln(((const docentry_structure_t*)e->data)->heading, o);

    print_lf(o);
}

/* todo alignment */
void
print_paragraph(const docconfig_t *cfg, int width, const docentry_t *par,
    FILE *o)
{
    const char *s = par->data, *next;
    int linec = 0, charc = 0, wlen;

    print_marginl(cfg, o);

    /* indent */
    if (cfg->indentparagraph)
        print_tab(cfg, o);

    while (s) {
        next = strchr(s, ' ');
        if (!next) {
            wlen = strlen(s);
            next = (const char *)-1;
        }
        else
            wlen = (next - s); /* 1 space */

        charc += wlen + 1;

        if ((charc >= width) || (linec == 0 && cfg->indentparagraph &&
            charc + cfg->tabstop >= width))
        {
            linec++;
            charc = wlen + 1;
            print_lf(o);
            print_marginl(cfg, o);
        }

        fprintf(o, "%.*s ", wlen, s);
 
        s = next + 1;
    }

    print_lf(o);
    print_lf(o);
}

void
generate_plain(const docconfig_t *cfg, docentry_t *doc, FILE *o) {
    int width = cfg->pagewidth - cfg->marginl - cfg->marginr;
    int height = cfg->pageheight - cfg->margint - cfg->marginb;
    if (cfg->headerl) height -= 2;
    if (cfg->footerl) height -= 2;

    int toplvl = find_toplvl_index(doc);

    compute_layout(cfg, width, height, doc);

    int linec = 0;
    
    for (const docentry_t *e = doc; e != NULL; e = e->n) {
        /* end page, next one */
        if (linec >= height) {
            linec = 0;
            print_marginb(cfg, o);
            if (cfg->footerl)
                print_footer(cfg, e->page, o);

            print_pb(o);
        }

        if (linec == 0 && cfg->headerl) {
            print_header(cfg, e->page, o);
            print_margint(cfg, o);
        }
    
        switch (e->type) {
            case EPARAGRAPH: {
                print_paragraph(cfg, width, e, o);
                linec += e->height; 
            } break;
            case ESTRUCTURE: {
                static int indices[] = { 0, 0, 0, 0, 0 };
                int type = ((const docentry_structure_t*)e->data)->type;
                indices[type]++;
                for (int i = type + 1; i < 5; i++)
                    indices[i] = 0;

                print_structure(cfg, width, toplvl, indices, e, o);
                linec += e->height; 

            } break;
            case ETITLEPAGE: {
                print_titlepage(cfg, width, height, e, o);
                linec = INT_MAX; /* force new page */
            } break;
            case ETABLEOFCONTENTS: {
                print_tableofcontents(cfg, width, height, toplvl, e, doc, o);
                linec = INT_MAX; /* force new page */
            } break;
            case ENULL: break;
            case EPREFORMAT: break;
            case EPAGEBREAK: {
                linec = INT_MAX;
            } break;
        }
    }
}
    
