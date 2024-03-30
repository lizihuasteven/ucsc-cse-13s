#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>

#include "universe.h"
#include "universe.c"

#define ROW   0
#define DELAY 50000

#define OPTIONS "tsn:i:o:"

/*char pp[]="SYNOPSIS \n 
           Conway's Game of Life \n  
           USAGE  \n  
           ./life_arm64 tsn:i:o:h \n
						OPTIONS\n
						    -t             Create your universe as a toroidal \n
						    -s             Silent - do not use animate the evolution \n
						    -n {number}    Number of generations [default: 100]\n
						    -i {file}      Input file [default: stdin]\n
						    -o {file}      Output file [default: stdout]\n
   ";*/
void Lrefresh(Universe *u) {
    uint32_t i, j;
    clear();
    curs_set(FALSE);
    for (i = 0; i < u->rows; i++) {
        //clear();
        for (j = 0; j < u->cols; j++) {
            if (u->grid[i][j] == TRUE)
                mvprintw(i, j, "o");
            else
                mvprintw(i, j, "");
        }
        printf("\n");

        refresh();
    }
}

void uv_mov(Universe *u, Universe *p) {
    uint32_t Lcnt = 0;
    for (uint32_t i = 0; i < u->rows; i++) {
        for (uint32_t j = 0; j < u->cols; j++) {
            Lcnt = uv_census(u, i, j);
            switch (Lcnt) {
            case 3: uv_live_cell(p, i, j); break;
            case 2:
                if (uv_get_cell(u, i, j) == TRUE)
                    uv_live_cell(p, i, j);
                else
                    uv_dead_cell(p, i, j);
                break;
            default: uv_dead_cell(p, i, j); break;
            }
        }
    }
}

int main(int argc, char **argv) {
    bool select_t = FALSE, select_s = FALSE;
    int n_arg = 100;
    char *i_arg = "stdin.txt", *o_arg = "stdout.txt";
    uint32_t Lrows = 0;
    uint32_t Lcols = 0;
    int opt = 0;

    if (argc == 1) {
        int m = 0, n = 0;
        uint32_t *scan_data;
        uint32_t row = 0;
        uint32_t col = 0;
        int scan_return = 0;
        char C;
        while (1) {
            //uint32_t value =0;
            scan_return = scanf("%d %d", &row, &col);
            if (scan_return == EOF)
                break;
            C = getchar();
            if ((C == '\n') && (scan_return == 2)) {
                if (m == 0) {
                    if ((row != 0) && (col != 0)) {
                        Lrows = row;
                        Lcols = col;
                        scan_data = (uint32_t *) calloc(2 * row * col + 2, sizeof(uint32_t));
                    } else {
                        printf("Malformed input.\n");
                        return 0;
                    }
                } else if ((row >= Lrows) || (col >= Lcols)) {
                    printf("Malformed input.\n");
                    return 0;
                }
                *(scan_data + m) = row;
                *(scan_data + m + 1) = col;
                m += 2;
            } else {
                printf("Malformed input.\n");
                return 0;
            }
        }
        FILE *Linfile;
        if (fopen(i_arg, "r") != NULL) {
            remove(i_arg);
        }
        if ((Linfile = fopen(i_arg, "w")) == NULL) {
            printf("open file <in> fail\n");
        } else {
            for (n = 0; n < m; n += 2) {
                fprintf(Linfile, "%d ", *(scan_data + n));
                fprintf(Linfile, "%d \n", *(scan_data + n + 1));
            }
        }
        free(scan_data);
        fclose(Linfile);
    } else {
        while ((opt = getopt(argc, argv, OPTIONS)) != -1) { //if(argc==2)
            switch (opt) {
            case 't': select_t = TRUE; break;
            case 's': select_s = TRUE; break;
            case 'n': n_arg = atoi(optarg); break;
            case 'i': i_arg = optarg; break;
            case 'o':
                o_arg = optarg;
                global_o_select = TRUE;
                break;
            default:
                printf(
                    "SYNOPSIS\n    Conway's Game of Life\n\nUSAGE\n    ./life "
                    "tsn:i:o:h\n\nOPTIONS\n    -t             Create your universe as a toroidal\n "
                    "   -s             Silent - do not use animate the evolution\n    -n {number}  "
                    "  Number of generations [default: 100]\n    -i {file}      Input file "
                    "[default: stdin]\n    -o {file}      Output file [default: stdout]\n");
                return 0;
            }
            //}
            //else
            //{
            //printf("%s",pp);
            //}
        }
    }
    FILE *input;
    /* Declaring a file pointer */
    /* can open file as r (read-only), w (write-only), a (append), r+ ... */
    /* fopen returns null if the file doesn't exist*/

    //11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
    if ((input = fopen(i_arg, "r"))
        == NULL) { // i_arg used to be 101.txt, that was only for testing
        /* Perror will automatically print the exact type of error */
        printf("Error opening %s", i_arg);
        return 0;
    }
    uint32_t value = 0;
    uint32_t lines = 0;

    while (!feof(input)) { /* Loop until the end of the file */
        int error = fscanf(input, "%" SCNu32 " ", &value);
        /* If we do not get exactly one value per line, input is not good */
        if (error != 1) {
            /* No perror here because fscanf does not set errorno */
            fprintf(stderr, "Invalid file format\n");
            exit(1);
        }
        if (ferror(input)) {
            perror("Error:");
            exit(1);
        }
        /* If no errors, process the input */

        if (lines == 0)
            Lrows = value;
        else if (lines == 1)
            Lcols = value;
        lines++;
        if (lines >= 2)
            break;
    }

    /* Close the file pointer */
    //fclose(input);
    //222222222222222222222222222222222222222222222222222222222222222222222222222222222
    Universe *AUniverse, *BUniverse, *CUniverse;

    AUniverse = uv_create(Lrows, Lcols, select_t);
    BUniverse = uv_create(Lrows, Lcols, select_t);
    CUniverse = BUniverse;

    //333333333333333333333333333333333333333333333333333333333333333333333333333333333333
    //if(uv_populate(AUniverse, input)==TRUE)  printf("uv_populate ok \n");
    //else  printf("uv_populate fall\n");
    uv_populate(AUniverse, input);

    //444444444444444444444444444444444444444444444444444444444444444444444444444444444444444
    initscr(); // Initialize the screen .
    for (int col = 0; col < n_arg; col += 1) {
        //clear () ; // Clear the window .
        //mvprintw (ROW , col , "o") ; // Displays "o".
        //usleep ( DELAY ) ; // Sleep for 50000 microseconds .
        // mov
        uv_mov(AUniverse, BUniverse);

        CUniverse = AUniverse;
        AUniverse = BUniverse;
        BUniverse = CUniverse;
        if (select_s == 0) {
            Lrefresh(AUniverse); // Hide the cursor .
            usleep(DELAY);
        }
    }

    endwin(); // Close the screen .

    //55555555555555555555555555555555555555555555555555555555555555555555555555555
    if (global_o_select == TRUE) {
        FILE *outfile;
        if (fopen(o_arg, "r") != NULL) {
            remove(o_arg);
        }
        outfile = fopen(o_arg, "w");
        uv_print(AUniverse, outfile);
        //Lrefresh(AUniverse); // Hide the cursor .
        //printf("ok\n");
    } else {
        for (uint32_t i = 0; i < (AUniverse->rows); i++) {
            for (uint32_t j = 0; j < (AUniverse->cols); j++) {
                if ((AUniverse->grid[i][j]) == TRUE)
                    printf("o");
                else
                    printf(".");
            }
            printf("\n");
        }
    }
    //66666666666666666666666666666666666666666666666666666666666666666666
    uv_delete(AUniverse);
    uv_delete(BUniverse);

    return 0;
}
