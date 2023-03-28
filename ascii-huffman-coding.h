#ifndef __ASCII_HUFFMAN_CODING_H__
#define __ASCII_HUFFMAN_CODING_H__

#include <stdint.h>
#include <stdlib.h>

typedef struct huffman_tree_node {
    uint32_t frequency; // number of occurrences
    uint32_t value;
    struct huffman_tree_node *left_child;
    struct huffman_tree_node *right_child;
} huffman_tree_node_t;

typedef struct char_stat_item {
    uint32_t frequency;
    uint32_t value;
} char_stat_item_t;

typedef struct char_stat_tab {
    uint32_t char_num;
    uint32_t item_num;
    char_stat_item_t item_list[];
} char_stat_tab_t;

char_stat_tab_t *ahc_stat_char_from_filepath(const char *filepath);

void ahc_show_char_stat_tab(char_stat_tab_t *tab);

int ahc_sort_char_stat_tab(char_stat_tab_t *tab);

#endif
