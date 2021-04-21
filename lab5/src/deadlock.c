#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t mut1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut2 = PTHREAD_MUTEX_INITIALIZER;

void thread_func1();
void thread_func2();
int main() {
  pthread_t thread1, thread2;

  if (pthread_create(&thread1, NULL, (void *)thread_func1, NULL) != 0) {
    perror("pthread_create");
    exit(1);
  }

  if (pthread_create(&thread2, NULL, (void *)thread_func2, NULL) != 0) {
    perror("pthread_create");
    exit(1);
  }

  if (pthread_join(thread1, NULL) != 0) {
    perror("pthread_join");
    exit(1);
  }

  if (pthread_join(thread2, NULL) != 0) {
    perror("pthread_join");
    exit(1);
  }

  return 0;
}

void thread_func1() {
  pthread_mutex_lock(&mut2);
  sleep(1);
  pthread_mutex_lock(&mut1);
}

void thread_func2() {
  pthread_mutex_lock(&mut1);
  sleep(1);
  pthread_mutex_lock(&mut2);
}