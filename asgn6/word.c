#include "word.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code.h"

Word *word_create(uint8_t *syms, uint32_t len) {
    Word *word = (Word *) malloc(sizeof(Word));
    if (word == NULL) {
        return NULL;
    }
    word->syms = (uint8_t *) malloc(len * sizeof(uint8_t));
    if (word->syms == NULL) {
        free(word);
        return NULL;
    }
    memcpy(word->syms, syms, len * sizeof(uint8_t));
    word->len = len;
    return word;
}

Word *word_append_sym(Word *w, uint8_t sym) {
    uint32_t new_len = w->len + 1;
    uint8_t *new_syms = (uint8_t *) malloc(new_len * sizeof(uint8_t));
    if (new_syms == NULL) {
        return NULL;
    }
    memcpy(new_syms, w->syms, w->len * sizeof(uint8_t));
    new_syms[new_len - 1] = sym;
    Word *new_word = word_create(new_syms, new_len);
    free(new_syms);
    return new_word;
}

void word_delete(Word *w) {
    if (w != NULL) {
        free(w->syms);
        free(w);
    }
}

WordTable *wt_create(void) {
    WordTable *wt = (WordTable *) malloc(sizeof(WordTable) * (MAX_CODE + 1));
    if (wt == NULL) {
        return NULL;
    }
    Word *empty_word = word_create(NULL, 0);
    if (empty_word == NULL) {
        free(wt);
        return NULL;
    }
    wt[EMPTY_CODE] = empty_word;
    for (int i = 0; i < EMPTY_CODE; i++) {
        wt[i] = NULL;
    }
    for (int i = EMPTY_CODE + 1; i <= MAX_CODE; i++) {
        wt[i] = NULL;
    }
    return wt;
}

void wt_reset(WordTable *wt) {
    for (int i = 0; i <= MAX_CODE; i++) {
        if (wt[i] != NULL && i != EMPTY_CODE) {
            word_delete(wt[i]);
            wt[i] = NULL;
        }
    }
}
