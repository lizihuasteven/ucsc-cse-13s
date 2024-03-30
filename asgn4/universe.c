#include "universe.h"

#include <curses.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern int global_o_select;
int global_o_select = 0; //for uv_print, if this is 1, no printing after the animation

struct Universe {
    uint32_t rows;
    uint32_t cols;
    bool **grid;
    bool toroidal;
};

Universe *uv_create(uint32_t rows, uint32_t cols, bool toroidal) {
    Universe *p;
    uint32_t i, j;
    p = (Universe *) calloc(1, sizeof(Universe));
    p->rows = rows;
    p->cols = cols;
    p->toroidal = toroidal;
    p->grid = (bool **) calloc(rows, sizeof(bool **));
    for (i = 0; i < rows; i++) {
        p->grid[i] = (bool *) calloc(cols, sizeof(bool)); //ע���������ռ���
        for (j = 0; j < cols; j++) {
            p->grid[i][j] = FALSE;
        }
    }
    return p;
}
void uv_delete(Universe *u) {
    uint32_t i;
    for (i = 0; i < (u->rows); i++) {
        free(u->grid[i]);
    }
    free(u);
}

uint32_t uv_rows(Universe *u) {
    uint32_t m;
    m = (uint32_t) (u->rows);
    return m;
}

uint32_t uv_cols(Universe *u) {
    uint32_t n;
    n = (uint32_t) (u->cols);
    return n;
}

void uv_live_cell(Universe *u, uint32_t r, uint32_t c) {

    if ((r < (u->rows)) && (c < (u->cols))) {
        u->grid[r][c] = TRUE;
    }
}

void uv_dead_cell(Universe *u, uint32_t r, uint32_t c) {
    if ((r < (u->rows)) && (c < (u->cols))) {
        u->grid[r][c] = FALSE;
    }
}

bool uv_get_cell(Universe *u, uint32_t r, uint32_t c) {
    if ((r >= (u->rows)) || (c >= (u->cols)))
        return FALSE;
    else
        return (u->grid[r][c]);
}

bool uv_populate(Universe *u, FILE *infile) {
    uint32_t i = 0, value = 0, valueX = 0, valueY = 0;
    /*if ((infile = fopen("101.txt", "r")) == NULL) {
        // Perror will automatically print the exact type of error 
        perror("Error!!:");
        exit(1);
    }*/

    while (!feof(infile)) { /* Loop while it is not EOF */
        /* fscanf is easier but known to have buffer overflows
         * SCNu16 is the scan equivalent macro of PRIu16 */
        int error = fscanf(infile, "%" SCNu32 "  ", &value);
        /* If we do not get exactly one value per line, input is no good */
        if (error != 1) {
            /* No perror here because fscanf does not set errorno */
            fprintf(stderr, "Invalid file format\n");
            exit(1);
        }
        if (ferror(infile)) {
            perror("Error!!:");
            exit(1);
        }
        /* If no errors, process the input */

        if (i % 2 == 0)
            valueX = value;
        else {
            valueY = value;
            //printf("sym Read Line %" PRIu32 "   %" PRIu32 "  %" PRIu32 " \n", i/2,valueX,valueY);
        }

        if (i % 2) {
            if ((valueX >= (u->rows)) || (valueY >= (u->cols)))
                return false;
            else
                u->grid[valueX][valueY] = TRUE;
        }
        i++;
    }

    /* Close the file pointer */
    fclose(infile);
    return TRUE;
}

uint32_t uv_census(Universe *u, uint32_t r, uint32_t c) {
    uint32_t cnt = 0;
    uint32_t i = 0, X = 0, Y = 0;
    if ((r >= (u->rows)) || (c >= (u->cols)))
        return FALSE;
    for (i = 0; i < 8; i++) {
        if (u->toroidal == TRUE) {
            switch (i) {
            case 0: //r-1,c-1
                if (r == 0)
                    X = u->rows - 1;
                else
                    X = r - 1;
                if (c == 0)
                    Y = u->cols - 1;
                else
                    Y = c - 1;
                break;
            case 1: //r-1,c
                if (r == 0)
                    X = u->rows - 1;
                else
                    X = r - 1;
                Y = c;
                break;
            case 2: //r-1,c+1
                if (r == 0)
                    X = u->rows - 1;
                else
                    X = r - 1;
                if ((c + 1) >= (u->cols))
                    Y = 0;
                else
                    Y = c + 1;
                break;
            case 3: //r,c-1
                X = r;
                if (c == 0)
                    Y = u->cols - 1;
                else
                    Y = c - 1;
                break;
            case 4: //r,c+1
                X = r;
                if ((c + 1) >= (u->cols))
                    Y = 0;
                else
                    Y = c + 1;
                break;
            case 5: //r+1,c-1
                if ((r + 1) >= (u->rows))
                    X = 0;
                else
                    X = r + 1;
                if (c == 0)
                    Y = u->cols - 1;
                else
                    Y = c - 1;
                break;
            case 6: //r+1,c
                if ((r + 1) >= (u->rows))
                    X = 0;
                else
                    X = r + 1;
                Y = c;
                break;
            case 7: //r+1,c+1
                if ((r + 1) >= (u->rows))
                    X = 0;
                else
                    X = r + 1;
                if ((c + 1) >= (u->cols))
                    Y = 0;
                else
                    Y = c + 1;
                break;
            default: break;
            }
        } else {
            switch (i) {
            case 0: //r-1,c-1
                if ((r == 0) || (c == 0))
                    continue;
                X = r - 1;
                Y = c - 1;
                break;
            case 1: //r-1,c
                if (r == 0)
                    continue;
                X = r - 1;
                Y = c;
                break;
            case 2: //r-1,c+1
                if ((r == 0) || ((c + 1) >= (u->cols)))
                    continue;
                X = r - 1;
                Y = c + 1;
                break;
            case 3: //r,c-1
                if (c == 0)
                    continue;
                Y = c - 1;
                X = r;
                break;
            case 4: //r,c+1
                if ((c + 1) >= (u->cols))
                    continue;
                X = r;
                Y = c + 1;
                break;
            case 5: //r+1,c-1
                if (((r + 1) >= (u->rows)) || (c == 0))
                    continue;
                X = r + 1;
                Y = c - 1;
                break;
            case 6: //r+1,c
                if ((r + 1) >= (u->rows))
                    continue;
                X = r + 1;
                Y = c;
                break;
            case 7: //r+1,c+1
                if (((r + 1) == (u->rows)) || ((c + 1) == (u->cols)))
                    continue;
                X = r + 1;
                Y = c + 1;
                break;
            default: break;
            }
        }
        if (u->grid[X][Y] == TRUE)
            cnt++;
    }
    return cnt;
}

void uv_print(Universe *u, FILE *outfile) {
    uint32_t i, j;

    //if (fopen(outfile,"r")!=NULL)
    //{
    //	remove(outfile);
    //}
    //if((fopen(outfile,"w"))==NULL)
    //{
    // printf("open file <out> fail\n");
    //}
    //else{
    for (i = 0; i < (u->rows); i++) {
        for (j = 0; j < (u->cols); j++) {
            if (u->grid[i][j] == TRUE) {
                fprintf(outfile, "o");
            }

            else {
                fprintf(outfile, ".");
            }
        }
        fprintf(outfile, "\n");
        if (global_o_select == 0) {
            printf("\n");
        }
    }
    //printf("open file <out> ok\n");
    fclose(outfile);
    //}
}
