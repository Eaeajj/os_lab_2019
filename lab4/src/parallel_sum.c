#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include <getopt.h> // опции
#include "utils.h" // GenerateArray необходима модификация функции чтобы числа были поменьше (например % 100). Потому что переполнение при суммировании.
#include "sum.h" // функция сложения и структура SumArgs
#include <time.h> // время



void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  // обрабатываем опции ./psum --threads_num "num" --seed "num" --array_size "num"
  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;

  while (1) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"threads_num", required_argument, 0, 0},
                                      {"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            threads_num = atoi(optarg);

            if (threads_num <= 0){
              printf("threads_num is a positive number\n");
              return 1;
            }

            break;
          case 1:
            seed = atoi(optarg);

            if(seed <= 0){
              printf("seed is a positive number\n");
              return 1;
            }

            break;
          case 2:
            array_size = atoi(optarg);

            if(array_size <= 0){
              printf("array_size is a positive number\n");
              return 1;
            }

            break;
          defalut:
            printf("Index %d is out of options\n", option_index);
        }
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

  if (threads_num == 0 || seed == 0 || array_size == 0) {
    printf("Usage: %s --threads_num \"num\" --seed \"num\" --array_size \"num\" \n",
           argv[0]);
    return 1;
  }

  // зачем лишние потоки если они не будут нужны
  // минимально каждый поток посчитает сумму одного элемента
  if (threads_num > array_size)
    threads_num = array_size;

  // генерируем массив
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  // создание потоков
  pthread_t threads[threads_num];           // pthread_t is the data type used to uniquely identify a thread
  struct SumArgs args[threads_num];
  for (uint32_t i = 0; i < threads_num; i++) {
    // выделяем потоку кусок массива
    args[i].array = array;
    args[i].begin = i*(array_size/threads_num);
    if (i == threads_num - 1)
      args[i].end = array_size;
    else
      args[i].end = (i+1)*(array_size/threads_num);


    if (pthread_create(
                       &threads[i],       // thread id
                       NULL,              // thread's attr
                       ThreadSum,         // thread's func
                       (void *)&args[i])  // func's args 
                      ) 
    {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  // ожидание потоков и суммирование частных сумм
  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);
  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  printf("Total: %d\n", total_sum);
  printf("Elapsed time: %fms\n", elapsed_time);
  return 0;
}