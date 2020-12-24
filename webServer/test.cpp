#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fstream>

#include <netinet/udp.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>
int main(){
  char str[] = {"GET /t.txt HTTP/1.1 "};
  char *getName;
  strtok(str, "/");
  getName = strtok(NULL," ");
 printf("end = %s , str = %s\n", getName, str);
  
  return 0;
}