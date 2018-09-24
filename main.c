#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef int bool;
typedef struct {
    int topRow;
    int bottomRow;
    int leftColumn;
    int rightColumn;
    int subgridNum;
}parameters;

#define TRUE 1
#define FALSE 0
#define NUM_CHILD_THREADS 27

int sudokuPuzzle [9][9];
bool columns[9];
bool rows[9];
bool subgrids[9];

void printBoard(){
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            printf("%d\t", sudokuPuzzle[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}

void *CheckRow(void *data){
    parameters *d;
    int values[9];
    int index;
    int topRow, bottomRow, leftColumn, rightColumn, subgridNum;

    d = data;

    topRow = d->topRow;
    bottomRow = d->bottomRow;
    leftColumn = d->leftColumn;
    rightColumn = d->rightColumn;
    subgridNum = d->subgridNum;

    pthread_t self = pthread_self();

    for (int i = 0; i < 9; i++){
        index = sudokuPuzzle[topRow][i];
        values[index - 1] = 1;
    }

    printf("\n");

    //Insert TRUE into all cells until repeat value is found.
    for(int j = 0; j < 9; j++){
        if(values[j] == 0) {
            rows[d->topRow] = FALSE;
            break;
        }
        else {
            rows[d->topRow] = TRUE;
        }
    }
    pthread_exit(0);
}

void *CheckColumn(void *data){
    parameters *d;
    int values[9];
    int index;
    int topRow, bottomRow, leftColumn, rightColumn, subgridNum;

    d = data;
    topRow = d->topRow;
    bottomRow = d->bottomRow;
    leftColumn = d->leftColumn;
    rightColumn = d->rightColumn;
    subgridNum = d->subgridNum;

    pthread_t self = pthread_self();

    for (int i = 0; i < 9; i++){
        index = sudokuPuzzle[i][leftColumn];
        values[index - 1] = 1;
    }

    printf("\n");

    //Insert TRUE into all cells unless repeat value is found.
    for(int j = 0; j < 9; j++){
        if(values[j] == 0) {
            columns[d->leftColumn] = FALSE;
            break;
        }
        else {
            columns[d->leftColumn] = TRUE;
        }
    }
    pthread_exit(0);
}

void *CheckSubgrid(void *data){
    parameters *d;
    int values[9];
    int index;
    int topRow, bottomRow, leftColumn, rightColumn, subgridNum;

    d = data;
    topRow = d->topRow;
    bottomRow = d->bottomRow;
    leftColumn = d->leftColumn;
    rightColumn = d->rightColumn;
    subgridNum = d->subgridNum;

    pthread_t self = pthread_self();

    for (int i = topRow; i < bottomRow + 1; i++){
        for(int j = leftColumn; j < rightColumn + 1; j++){
            index = sudokuPuzzle[i][j];
            values[index - 1] = 1;
        }
    }

    printf("\n");

    //Insert TRUE into all cells unless repeat value is found.
    for(int j = 0; j < 9; j++){
        if(values[j] == 0) {
            subgrids[subgridNum] = FALSE;
            break;
        }
        else {
            subgrids[subgridNum] = TRUE;
        }
    }
    pthread_exit(0);
}

void CheckForErrors(){
    char x;
    for(int i = 0; i < 9; i++){
        if(rows[i] == 0) {
            printf("Error: repeat value in row %1d. Press any key to exit.", i + 1);
            x = getchar();
            exit(0);
        }
    }

    printf("All rows are valid...\n");

    for(int i = 0; i < 9; i++){
        if(columns[i] == 0){
            printf("Error: repeat value in column %1d. Press any key to exit.", i + 1);
            x = getchar();
            exit(0);
        }
    }

    printf("All columns are valid...\n");

    for(int i = 0; i < 9; i++){
        if(subgrids[i] == 0){
            printf("Error: repeat value in sub grid %1d. Press any key to exit.", i + 1);
            x = getchar();
            exit(0);
        }
    }

    printf("All subgrids are valid! Huzzah!\nPress any key to exit.");

    x = getchar();

    exit(0);
}

int main() {
    pthread_t threads[NUM_CHILD_THREADS];
    pthread_attr_t attr[NUM_CHILD_THREADS];
    FILE *ifp;
    char inputFile[] = "/path/of/file/here/correctSudoku.txt";
    int exitCode, subgridCounter;
    pthread_t parentID;

    parentID = pthread_self();

    printf("App started. Parent ID is %d.\n\n", (int)parentID);

    ifp = fopen(inputFile, "r");

    if (ifp == NULL) {
        fprintf(stderr, "Can't open input file!\n");
        exit(1);
    }

    printf("Reading Sudoku board from path: %s\n\n", inputFile);

    //Read input file into sudokuPuzzle.
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            fscanf(ifp, "%1d", &sudokuPuzzle[i][j]);
        }
    }

    fclose(ifp);

    printBoard();

    //Get the default attributes.
    for (int i = 0; i < NUM_CHILD_THREADS; i++)
        pthread_attr_init(&(attr[i]));

    //Iterate over rows.
    for(int i = 0; i < 9; i++){
        parameters *data = (parameters *) malloc(sizeof(parameters));
        data->topRow = i;
        data->bottomRow = i;
        data->leftColumn = 0;
        data->rightColumn = 8;
        data->subgridNum = 0;

        printf("Creating thread for row %d...", i + 1);

        exitCode = pthread_create(&threads[i], &(attr[i]), CheckRow, data);

        if(exitCode){
            printf("\nError code while creating child thread: %1d", exitCode);
            exit(-1);
        }

        printf("\nChild thread %d created.\n", i + 1);

        pthread_join(threads[i], NULL);
    }

    //Iterate over columns.
    for(int i = 0; i < 9; i++){
        parameters *data2 = (parameters *) malloc(sizeof(parameters));
        data2->topRow = 0;
        data2->bottomRow = 8;
        data2->leftColumn = i;
        data2->rightColumn = i;
        data2->subgridNum = 0;

        printf("Creating thread for column %d...", i);

        exitCode = pthread_create(&threads[i+9], &(attr[i]), CheckColumn, data2);

        if(exitCode){
            printf("Error code while creating child thread: %1d", exitCode);
            exit(-1);
        }

        printf("Child thread %d created.\n", i + 10);

        pthread_join(threads[i+9], NULL);
    }

    subgridCounter = 0;
    //Iterate over sub grids.
    for(int i = 0; i < 3; i+=3){
        for(int j = 0; j < 9; j++){
            //First column of sub grids.
            if(j >= 0 && j <= 2){
                parameters *data3 = (parameters *) malloc(sizeof(parameters));
                data3->topRow = i;
                data3->bottomRow = i + 2;
                data3->leftColumn = 0;
                data3->rightColumn = 2;
                data3->subgridNum = subgridCounter;

                printf("\nCreating thread for sub grid %d...", subgridCounter + 1);

                subgridCounter++;

                exitCode = pthread_create(&threads[subgridCounter+18], &(attr[i]), CheckSubgrid, data3);

                if(exitCode){
                    printf("\nError code while creating child thread: %1d", exitCode);

                }
                printf("\nChild thread %d created.", subgridCounter + 19);

                pthread_join(threads[subgridCounter+18], NULL);
            }

                //Second column of sub grids.
            else if(j >= 3 && j <= 5){
                parameters *data4 = (parameters *) malloc(sizeof(parameters));
                data4->topRow = i;
                data4->bottomRow = i + 2;
                data4->leftColumn = 3;
                data4->rightColumn = 5;
                data4->subgridNum = subgridCounter;

                printf("Creating thread for sub grid %d...", subgridCounter + 1);

                subgridCounter++;

                exitCode = pthread_create(&threads[subgridCounter+18], &(attr[i]), CheckSubgrid, data4);

                if(exitCode){
                    printf("Error code while creating child thread: %1d", exitCode);

                }
                printf("Child thread %d created.\n", subgridCounter + 19);

                pthread_join(threads[subgridCounter+18], NULL);
            }

                //Third column of sub grids.
            else {
                parameters *data5 = (parameters *) malloc(sizeof(parameters));
                data5->topRow = i;
                data5->bottomRow = i + 2;
                data5->leftColumn = 6;
                data5->rightColumn = 8;
                data5->subgridNum = subgridCounter;

                printf("Creating thread for sub grid %d...", subgridCounter + 1);

                subgridCounter++;

                exitCode = pthread_create(&threads[subgridCounter+18], &(attr[i]), CheckSubgrid, data5);

                if(exitCode){
                    printf("Error code while creating child thread: %1d", exitCode);

                }
                printf("Child thread %d created.\n", subgridCounter + 19);

                pthread_join(threads[subgridCounter+18], NULL);
            }
        }
    }

    CheckForErrors();


    pthread_exit(NULL);

    return 0;
}

