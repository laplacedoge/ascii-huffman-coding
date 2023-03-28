#include "ascii-huffman-coding.h"

#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>

void bad_cb(char ch) {
    putc(ch, stdout);
}

#define ASCII_CHAR_NUM  128

char_stat_tab_t *ahc_stat_char_from_filepath(const char *filepath) {
    uint32_t ascii_char_stat_tab[ASCII_CHAR_NUM];
    char_stat_tab_t *char_stat_tab;
    char buff[256];
    char ch;
    uint32_t ascii_item_num;
    uint32_t ascii_char_num;
    int char_stat_tab_idx;
    int ret;
    int fd;

    fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "failed to open \"%s\": %s.\n", filepath, strerror(errno));

        return NULL;
    }

    ascii_char_num = 0;
    memset(&ascii_char_stat_tab, 0, sizeof(ascii_char_stat_tab));
    while (true) {
        ret = (int)read(fd, &buff, 256);
        if (ret < 0) {
            fprintf(stderr, "failed to read \"%s\": %s.\n", filepath, strerror(errno));
            close(fd);

            return NULL;
        } else if (ret > 0) {
            for (int i = 0; i < ret; i++) {
                ch = buff[i];

                if (ch > 127) {
                    fprintf(stderr, "bad character 0x%02X at offset %d.\n", ch, ascii_char_num + i);
                    close(fd);

                    return NULL;
                }

                // bad_cb(buff[i]);
                ascii_char_stat_tab[ch]++;
            }
            ascii_char_num += ret;
        } else {
            break;
        }
    }

    close(fd);

    ascii_item_num = 0;
    for (uint32_t i = 0; i < ASCII_CHAR_NUM; i++) {
        if (ascii_char_stat_tab[i] > 0) {
            ascii_item_num++;
        }
    }

    // printf("number of occurred ASCII character: %d\n", ascii_item_num);
    // for (int i = 0; i < ASCII_CHAR_NUM; i++) {
    //     printf("[%03d]: %u\n", i, ascii_char_stat_tab[i]);
    // }

    char_stat_tab = (char_stat_tab_t *)malloc(sizeof(char_stat_tab_t) + sizeof(char_stat_item_t) * ascii_item_num);
    if (char_stat_tab == NULL) {
        fprintf(stderr, "failed to malloc: %s.\n", strerror(errno));

        return NULL;
    }

    char_stat_tab->char_num = ascii_char_num;
    char_stat_tab->item_num = ascii_item_num;
    char_stat_tab_idx = 0;
    for (uint32_t i = 0; i < ASCII_CHAR_NUM; i++) {
        if (ascii_char_stat_tab[i] > 0) {
            char_stat_tab->item_list[char_stat_tab_idx].val = i;
            char_stat_tab->item_list[char_stat_tab_idx].freq = ascii_char_stat_tab[i];
            char_stat_tab_idx++;
        }
    }

    // for (int i = 0; i < char_stat_tab->item_num; i++) {
    //     printf("[%03d]: %u\n", char_stat_tab->item_list[i].val, char_stat_tab->item_list[i].freq);
    // }

    return char_stat_tab;
}

void ahc_show_char_stat_tab(char_stat_tab_t *tab) {
    printf("Char num: %u\n", tab->char_num);
    printf("Item num: %u\n", tab->item_num);
    for (int i = 0; i < tab->item_num; i++) {
        printf("[%03d]: %u\n", tab->item_list[i].val, tab->item_list[i].freq);
    }
}

static int ahc_cmp_char_stat_item(char_stat_item_t *item_a, char_stat_item_t *item_b) {
    if (item_a->freq == item_b->freq) {
        if (item_a->val < item_b->val) {
            return 1;
        } else {
            return 0;
        }
    } else if (item_a->freq > item_b->freq) {
        return 1;
    } else {
        return 0;
    }
}

int ahc_sort_char_stat_tab(char_stat_tab_t *tab) {
    char_stat_item_t tmp_char_stat_item;

    for (int i = (int)tab->item_num - 1; i >= 0; i--) {
        for (int j = 0; j < i; j++) {
            if (!ahc_cmp_char_stat_item(tab->item_list + j, tab->item_list + j + 1)) {
                tmp_char_stat_item.freq = tab->item_list[j].freq;
                tmp_char_stat_item.val = tab->item_list[j].val;

                tab->item_list[j].freq = tab->item_list[j + 1].freq;
                tab->item_list[j].val = tab->item_list[j + 1].val;

                tab->item_list[j + 1].freq = tmp_char_stat_item.freq;
                tab->item_list[j + 1].val = tmp_char_stat_item.val;
            }
        }
    }

    return 0;
}

static bool ahc_cmp_ht_node(ht_node_t *ht_node_a, ht_node_t *ht_node_b) {
    // if (ht_node_a->freq == ht_node_b->freq) {
    //     if (ht_node_a->val < ht_node_b->val) {
    //         return true;
    //     } else {
    //         return false;
    //     }
    // } else if (ht_node_a->freq > ht_node_b->freq) {
    //     return true;
    // } else {
    //     return false;
    // }
    if (ht_node_a->freq > ht_node_b->freq) {
        return true;
    } else {
        return false;
    }
}

int ahc_sort_ht_node_list(ht_node_t **ht_node_list, int ht_node_num) {
    ht_node_t *ht_node_tmp;

    for (int i = ht_node_num - 1; i >= 0; i--) {
        for (int j = 0; j < i; j++) {
            if (ahc_cmp_ht_node(ht_node_list[j], ht_node_list[j + 1])) {
                ht_node_tmp = ht_node_list[j];
                ht_node_list[j] = ht_node_list[j + 1];
                ht_node_list[j + 1] = ht_node_tmp;
            }
        }
    }

    return 0;
}

// static huffman_tree_node_t *ahc_make_huffman_tree_node(void) {
//     huffman_tree_node_t *huffman_tree_node;

//     huffman_tree_node = (huffman_tree_node_t *)malloc(sizeof(huffman_tree_node_t));
//     if (huffman_tree_node == NULL) {
//         return NULL;
//     }

//     memset(huffman_tree_node, 0, sizeof(huffman_tree_node_t));

//     return huffman_tree_node;
// }

static ht_node_t *ahc_make_ht_node(void) {
    ht_node_t *ht_node;

    ht_node = (ht_node_t *)malloc(sizeof(ht_node_t));
    if (ht_node == NULL) {
        return NULL;
    }

    memset(ht_node, 0, sizeof(ht_node_t));

    return ht_node;
}

// huffman_tree_node_t *ahc_make_huffman_tree(char_stat_tab_t *char_stat_tab) {

// }

ht_node_t *ahc_make_huffman_tree(char_stat_tab_t *char_stat_tab) {
    ht_node_t **ht_node_list;
    uint32_t ht_node_num;

    ht_node_t **ht_node_offs;
    uint32_t ht_node_rest;

    ht_node_t *ht_node;

    ht_node_num = char_stat_tab->item_num;

    ht_node_list = (ht_node_t **)malloc(sizeof(ht_node_t *) * ht_node_num);
    if (ht_node_list == NULL) {
        fprintf(stderr, "failed to malloc: %s.\n", strerror(errno));

        return NULL;
    }

    for (uint32_t i = 0; i < ht_node_num; i++) {
        ht_node = ahc_make_ht_node();
        if (ht_node == NULL) {
            fprintf(stderr, "failed to make huffman-tree leaf node.\n");

            return NULL;
        }

        ht_node->val = char_stat_tab->item_list[i].val;
        ht_node->freq = char_stat_tab->item_list[i].freq;
        ht_node_list[i] = ht_node;
    }

    printf("\n\n\n#####\n\n\n");

    for (uint32_t i = 0; i < ht_node_num; i++) {
        if (ht_node_list[i] != NULL) {
            printf("[%03d]: %u\n", ht_node_list[i]->val, ht_node_list[i]->freq);
        }
    }

    printf("\n\n\n#####\n\n\n");

    ht_node_offs = ht_node_list;
    ht_node_rest = ht_node_num;
    do {
        ahc_sort_ht_node_list(ht_node_offs, ht_node_rest);

        if (ht_node_rest >= 2) {
            ht_node = ahc_make_ht_node();
            if (ht_node == NULL) {
                fprintf(stderr, "failed to make huffman-tree node.\n");

                return NULL;
            }

            ht_node->freq = ht_node_offs[0]->freq + ht_node_offs[1]->freq;
            ht_node->child.left = ht_node_offs[0];
            ht_node->child.right = ht_node_offs[1];

            ht_node_offs[0] = NULL;
            ht_node_offs[1] = ht_node;

            ht_node_offs++;
            ht_node_rest--;
        }
    } while (ht_node_rest != 1);

    free(ht_node_list);

    return ht_node;
}

static void ahc_show_huffman_coding(uint32_t coding_value, uint32_t coding_length) {
    for (int i = coding_length - 1; i >= 0; i--) {
        printf("%c", ((coding_value >> i) & 1) ? '1' : '0');
    }
}

static uint32_t ahc_iter_huffman_tree(ht_node_t *ht_node, int depth,
    bool is_left_child, uint32_t coding_value, uint32_t coding_length) {

    // for (int i = 0; i < depth; i++) {
    //     printf("-   ");
    // }
    if (depth > 0) {
        for (int i = 0; i < depth - 1; i++) {
            printf("│   ");
        }
        if (is_left_child) {
            printf("├── ");
        } else {
            printf("└── ");
        }
    }

    if (ht_node->child.left == NULL &&
        ht_node->child.right == NULL) {
        printf("[0x%02X:%u] ", ht_node->val, ht_node->freq);
        // ahc_show_huffman_coding((coding_value << 1), coding_length + 1);
        ahc_show_huffman_coding(coding_value, coding_length);
        printf("\n");

        return coding_length * ht_node->freq;
    } else {
        uint32_t total_bits;
        uint32_t bits;

        printf("[:%u]\n", ht_node->freq);

        total_bits = 0;
        bits = ahc_iter_huffman_tree(ht_node->child.left, depth + 1, true, (coding_value << 1), coding_length + 1);
        total_bits += bits;
        bits = ahc_iter_huffman_tree(ht_node->child.right, depth + 1, false, (coding_value << 1) + 1, coding_length + 1);
        total_bits += bits;

        return total_bits;
    }
}

int ahc_show_huffman_tree(ht_node_t *huffman_tree) {
    uint32_t total_bits;

    total_bits = ahc_iter_huffman_tree(huffman_tree, 0, false, 0, 0);
    printf("total bits: %u\n", total_bits);
    printf("total bytes: %u\n", total_bits / 8 + ((total_bits % 8) ? 1 : 0));

    return 0;
}

int ahc_free_huffman_tree(huffman_tree_node_t *huffman_tree) {

}
