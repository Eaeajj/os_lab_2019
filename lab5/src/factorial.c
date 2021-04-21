#include <stdio.h>
#include <stdlib.h>

#include <getopt.h>
#include <pthread.h>
#include <errno.h>

int result = 1;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

struct Param{
    int* fact;
    int begin;
    int end;
    int mod;
};

void thread_func(void*);

int main (int argc, char** argv) {
  int k = -1;
  int pnum = -1;
  int mod = -1;

  while (1) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1) break; //конец списка опций

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            k = atoi(optarg);

            if(k <= 0){
              printf("k is a positive number\n");
              return 1;
            }

            break;
          case 1:
            pnum = atoi(optarg);

            if(pnum <= 0){
              printf("pnum is a positive number\n");
              return 1;
            }

            break;
          case 2:
            mod = atoi(optarg);

            if(mod <= 0){
              printf("mod is a positive number\n");
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

  if (k == -1 || mod == -1 || pnum == -1) {
    printf("Usage: %s --k \"num\" --pnum \"num\" --mod \"num\" \n",
           argv[0]);
    return 1;
  }

  if (k >= mod) {
      printf("%d! mod %d = %d\n",k,mod,0);
      exit(0);
  }
  if (pnum > k) pnum = k; // избыток потоков ни к чему

  pthread_t* thrds = (pthread_t*)malloc(pnum * sizeof(pthread_t));
  struct Param params[pnum];
  for(int i = 0; i < pnum; i++){
      params[i].fact = &result;
      params[i].mod = mod;
      params[i].begin = i*(k/pnum)+1;
      if(i != pnum - 1)
          params[i].end = (i+1)*(k/pnum);
      else
          params[i].end = k;

      if (pthread_create(thrds+i, NULL, (void *)thread_func, (void *)(params+i)) != 0) {
        perror("pthread_create");
        exit(1);
      }
  }

  for(int i = 0; i < pnum; i++){
      if (pthread_join(thrds[i], NULL) != 0) {
        perror("pthread_join");
        exit(1);
      }
  }

  printf("%d! mod %d = %d\n",k,mod,result);
  return 0;
}

void thread_func(void* p){
    struct Param *param = (struct Param*) p;
    int work = 1;

    pthread_mutex_lock(&mut);
    for (int i = param->begin; i <= param->end; i++)
      work *= i;
    *(param->fact) = ((work % param->mod) * *(param->fact)) % param->mod;
    pthread_mutex_unlock(&mut);
}