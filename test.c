#include "ascii-huffman-coding.h"

#include <stdio.h>

int main(int argc, char *argv[]) {

    if (argc > 1) {
        char_stat_tab_t *char_stat_tab;
        int ret;

        char_stat_tab = ahc_stat_char_from_filepath(argv[1]);
        if (char_stat_tab == NULL) {
            fprintf(stderr, "failed to make the ASCII character statistics!\n");

            return EXIT_FAILURE;
        }

        ahc_show_char_stat_tab(char_stat_tab);

        ret = ahc_sort_char_stat_tab(char_stat_tab);
        if (ret < 0) {
            free(char_stat_tab);

            return EXIT_FAILURE;
        }

        ahc_show_char_stat_tab(char_stat_tab);

        free(char_stat_tab);
    }

    return 0;
}
