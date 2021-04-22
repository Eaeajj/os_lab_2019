#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "pthread.h"
#include "utils.h"

struct Server {
  char ip[255];
  int port;
};

struct ClientArgs {
  struct Server *server;
  char task[3*sizeof(uint64_t)];
};


// uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
//   uint64_t result = 0;
//   a = a % mod;
//   while (b > 0) {
//     if (b % 2 == 1)
//       result = (result + a) % mod;
//     a = (a * 2) % mod;
//     b /= 2;
//   }

//   return result % mod;
// }

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}


 uint64_t Client(const struct ClientArgs *args) {
  struct hostent *hostname = gethostbyname(args->server->ip);
  if (hostname == NULL) {
    fprintf(stderr, "gethostbyname failed with %s\n", args->server->ip);
    exit(1);
  }

  struct sockaddr_in server;
  // sin_family = AF_INET by defauls
  server.sin_family = AF_INET;
  // Convert 16-bit positive integers from host to network byte order.
  server.sin_port = htons(args->server->port);

  server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

  int sck = socket(AF_INET, SOCK_STREAM, 0);
  if (sck < 0) {
    fprintf(stderr, "Socket creation failed!\n");
    exit(1);
  }

  if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
    fprintf(stderr, "Connection failed\n");
    exit(1);
  }

  if (send(sck, args->task, sizeof(args->task), 0) < 0) {
    fprintf(stderr, "Send failed\n");
    exit(1);
  }

  char response[sizeof(uint64_t)];
  if (recv(sck, response, sizeof(response), 0) < 0) {
    fprintf(stderr, "Recieve failed\n");
    exit(1);
  }
  close(sck);

  uint64_t answer = 0;
  memcpy(&answer, response, sizeof(uint64_t));

  return answer;
}

void *ClientThread(void *args) {
  struct ClientArgs *fargs = (struct ClientArgs *)args;
  return (void *)(uint64_t *)Client(fargs);
}



int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
        // TODO: your code here
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        // TODO: your code here
        break;
      case 2:
        // TODO: your code here
        memcpy(servers, optarg, strlen(optarg));
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  // TODO: for one server here, rewrite with servers from file

    // считываение списка серверов из файла
  unsigned int servers_num = 0;
  struct Server *to = malloc(sizeof(struct Server) * servers_num);
  FILE* fp = fopen(servers,"rt");
  while(true){
    char* ip_s = malloc(sizeof(char)*16);
    int ip[4], port = -1;

    fscanf(fp,"%d.%d.%d.%d:%d\n",ip+0,ip+1,ip+2,ip+3,&port);
    if (port == -1) break;
    sprintf(ip_s, "%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);

    servers_num += 1;
    to = realloc(to, sizeof(struct Server) * servers_num);
    memcpy(to[servers_num-1].ip, ip_s, 16);
    to[servers_num-1].port = port;
  }
  fclose(fp);

  // Использую потоки
  pthread_t threads[servers_num];
  struct ClientArgs args[servers_num];

  for (int i = 0; i < servers_num; i++){
    uint64_t begin = i*(k/servers_num)+1;
    uint64_t end = k;
    if(i != servers_num - 1)
      end = (i+1)*(k/servers_num);

    char task[sizeof(uint64_t) * 3];
    memcpy(task, &begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

    args[i].server = to+i;
    memcpy(args[i].task, task, 3*sizeof(uint64_t));

    if (pthread_create(&threads[i], NULL, ClientThread, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  uint64_t total = 1;
  for (uint32_t i = 0; i < servers_num; i++) {
    uint64_t result = 0;
    pthread_join(threads[i], (void **)&result);
    total = MultModulo(total, result, mod);
  }
  printf("answer: %lu\n", total);

  free(to);

  return 0;
}
