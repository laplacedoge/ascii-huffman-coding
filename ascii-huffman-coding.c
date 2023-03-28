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
            char_stat_tab->item_list[char_stat_tab_idx].value = i;
            char_stat_tab->item_list[char_stat_tab_idx].frequency = ascii_char_stat_tab[i];
            char_stat_tab_idx++;
        }
    }

    // for (int i = 0; i < char_stat_tab->item_num; i++) {
    //     printf("[%03d]: %u\n", char_stat_tab->item_list[i].value, char_stat_tab->item_list[i].frequency);
    // }

    return char_stat_tab;
}

void ahc_show_char_stat_tab(char_stat_tab_t *tab) {
    printf("Char num: %u\n", tab->char_num);
    printf("Item num: %u\n", tab->item_num);
    for (int i = 0; i < tab->item_num; i++) {
        printf("[%03d]: %u\n", tab->item_list[i].value, tab->item_list[i].frequency);
    }
}

static int ahc_cmp_char_stat_item(char_stat_item_t *item_a, char_stat_item_t *item_b) {
    if (item_a->frequency == item_b->frequency) {
        if (item_a->value < item_b->value) {
            return 1;
        } else {
            return 0;
        }
    } else if (item_a->frequency > item_b->frequency) {
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
                tmp_char_stat_item.frequency = tab->item_list[j].frequency;
                tmp_char_stat_item.value = tab->item_list[j].value;

                tab->item_list[j].frequency = tab->item_list[j + 1].frequency;
                tab->item_list[j].value = tab->item_list[j + 1].value;

                tab->item_list[j + 1].frequency = tmp_char_stat_item.frequency;
                tab->item_list[j + 1].value = tmp_char_stat_item.value;
            }
        }
    }

    return 0;
}

huffman_tree_node_t *ahc_make_huffman_tree(char_stat_tab_t *char_stat_tab) {

}

int ahc_free_huffman_tree(huffman_tree_node_t *huffman_tree) {

}
