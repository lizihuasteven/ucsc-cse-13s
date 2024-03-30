#include "trie.h"
#include "stdlib.h"
#include "code.h"

TrieNode *trie_node_create(uint16_t code) {
    TrieNode *node = (struct TrieNode *) malloc(sizeof(struct TrieNode));
    node->code = code;
    for (int i = 0; i < ALPHABET; i++) {
        node->children[i] = NULL;
    }
    return node;
}

void trie_node_delete(TrieNode *n) {
    if (n == NULL) {
        return;
    }
    for (int i = 0; i < ALPHABET; i++) {
        trie_node_delete(n->children[i]);
    }
    free(n);
}

TrieNode *trie_create(void) {
    TrieNode *root = trie_node_create(EMPTY_CODE);
    if (root == NULL) {
        return NULL;
    }
    return root;
}

void trie_reset(TrieNode *root) {
    if (root == NULL) {
        return;
    }
    for (int i = 0; i < ALPHABET; i++) {
        if (root->children[i] != NULL) {
            trie_delete(root->children[i]);
            root->children[i] = NULL;
        }
    }
    for (int i = 0; i < MAX_CODE; i++) {
        root->code = EMPTY_CODE;
    }
}

void trie_delete(TrieNode *n) {
    if (n == NULL) {
        return;
    }
    for (int i = 0; i < ALPHABET; i++) {
        trie_delete(n->children[i]);
        n->children[i] = NULL;
    }
    trie_node_delete(n);
}

TrieNode *trie_step(TrieNode *n, uint8_t sym) {
    if (n == NULL) {
        return NULL;
    }
    return n->children[sym];
}
