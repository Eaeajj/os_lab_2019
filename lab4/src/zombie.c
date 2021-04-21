#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void){
  pid_t child_pid = fork(); // Создадим дочерний процесс
  if (child_pid > 0){
    // Подождем пока потомок заработает
    sleep(2);
    // Отправим потомку сигнал SIGKILL. В результате из потомка получится зомби
    if (kill(child_pid, SIGKILL) == 0)
      printf("[Parent] SIGKILL отправлен потомку (%d)\n",child_pid);

    // Подождем немного
    sleep(2);
    // Потомок не пишет в консоль. прервался.

    // Проверим можем ли мы снова отправить ему сигнал какой-нибудь
    if (kill(child_pid, 0) == 0)
      printf("[Parent] Потомку (%d) всё ещё можно отправлять сигналы, значит он не умер\n",child_pid);

    // считаем информацию о завершении потомка
    if (wait(NULL) == child_pid)
      printf("[Parent] Считали информацию о завершении потомка (%d)\n", child_pid);

    // Попробуем отправить сигнал снова
    if (kill(child_pid, 0) == -1)
      printf("[Parent] А вот теперь отправлять сигналы не можем\n[Parent] Теперь зомби убит\n");
  } else {
    while(1){           //код потомка, child_pid = 0
      printf("[Child] Я потомок, я живой..\n");
      sleep(1);
    }
  }


  return 0;
}