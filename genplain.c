#include "genplain.h"

int
paragraph_countlines(int width, const docentry_t *p) {
    const char *s = p->data;
    while (s) {
            
    }
}

int
compute_pagenums(const docconfig_t *cfg, docentry_t *doc) {
    int width = cfg->pagewidth - cfg->marginl - cfg->marginr;
    int height = cfg->pageheight - cffg->margint - cfg->marginb;

    for (const docentry_t *e = doc; e != NULL; e = e->n) {
        switch (e->type) {
            case EPARAGRAPH: {
                    
            } break;
        }
    }

}

int
generate_plain(const docconfig_t *cfg, docentry_t *doc, FILE *o) {

}
    
