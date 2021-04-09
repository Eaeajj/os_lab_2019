#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main (int argc, char** argv) {
    int seed = -1;
    int array_size = -1;
    int pnum = -1;
    bool with_files = false;

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {{"seed", required_argument, 0, 0},
                                          {"array_size", required_argument, 0, 0},
                                          {"pnum", required_argument, 0, 0},
                                          {"by_files", no_argument, 0, 'f'},
                                          {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break; //конец списка опций

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);

                        if(seed <= 0){
                            printf("seed is a positive number\n");
                            return 1;
                        }

                        break;
                    case 1:
                        array_size = atoi(optarg);

                        if(array_size <= 0){
                            printf("array_size is a positive number\n");
                            return 1;
                        }

                        break;
                    case 2:
                        pnum = atoi(optarg);

                        if(pnum <= 0){
                            printf("pnum is a positive number\n");
                            return 1;
                        }

                        break;
                    case 3:
                        with_files = true;
                        break;

                    defalut:
                        printf("Index %d is out of options\n", option_index);
                }
                break;
            case 'f': // символ короткой опции если она распознана
                with_files = true;
                break;

            case '?': // неизвестный символ опции или двусмысленное толкование параметра
                break;

            default:
                printf("getopt returned character code 0%o?\n", c);
        }
    }

    if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
               argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    int active_child_processes = 0;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    int fd[2]; // дескрипторы для pipe'a
    FILE* fp[2]; fp[0] = NULL; fp[1] = NULL; // дескрипторы для файлаы
    if (with_files){
        if ((fp[1] = fopen("buf.bin","wb")) == NULL){
            printf("can't open file for write");
            exit(-1);
        }
        if ((fp[0] = fopen("buf.bin","rb")) == NULL){
            printf("can't open file for read");
            exit(-1);
        }
    } else
    if (pipe(fd) < 0){
        printf("Can't create pipe!");
        exit(-1);
    }

    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            active_child_processes += 1;
            if (child_pid == 0) {
                // разделяем массив на pnum равных кусков
                // и в этих кусках каждый потомок ищет мин и макс
                unsigned int begin = i*(array_size/pnum);
                unsigned int end = (i+1 == pnum) ? array_size : begin + array_size/pnum;
                struct MinMax min_max = GetMinMax(array,begin,end);

                if (with_files) {
                    fwrite(&min_max.min, sizeof(int), 1, fp[1]);
                    fwrite(&min_max.max, sizeof(int), 1, fp[1]);
                } else {
                    write(fd[1], &min_max.min, sizeof(int));
                    write(fd[1], &min_max.max, sizeof(int));
                }

                return 0; // тут потомок завершается
            }

        } else {
            printf("Fork failed!\n");
            return 1;
        }
    }

    // Здесь добиваем зомби-процессы, т.е. считываем их статус завершения.
    while (active_child_processes > 0) {
        wait(NULL); // т.к. передаем NULL, то информация о статусе его завершения нас не интересует.
        active_child_processes -= 1;
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++) {
        int min = INT_MAX;
        int max = INT_MIN;
        if (with_files) {
            if (fread(&min, sizeof(int), 1, fp[0]) != 1
                || fread(&max, sizeof(int), 1, fp[0]) != 1) {
                printf("can't read from file\n");
                exit(-1);
            }
        } else {
            read(fd[0], &min, sizeof(int));
            read(fd[0], &max, sizeof(int));
        }

        if (min < min_max.min) min_max.min = min;
        if (max > min_max.max) min_max.max = max;
    }

    close(fd[0]);
    close(fd[1]);

    if (fp[0] != NULL)
        fclose(fp[0]);
    if (fp[1] != NULL)
        fclose(fp[1]);

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);

    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);
    fflush(NULL);
    return 0;
}