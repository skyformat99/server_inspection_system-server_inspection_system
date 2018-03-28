/*101  cpu.log
 *102  mem.log
  103  disk.log
  104  system.log
  105  user.log

  666 cpuwarning
  777 memwarning
  888 diskwarning

  200 client recerve request wait to send message
  201 client earger to send warninf message

  501 creat another sockfd to get files

  111 represent ack 

*/
#ifndef _MASTER_H
#define _MASTER_H
#include "head.h"
int socket_connect(int port, char *host);
int master_send(int sockfd, char *request);
int master_receve(int sockfd, char *buffer, char path[]);
int socket_creat(int port);
int write_to_log(char *buffer);


int socket_creat(int port) {
    int sockfd;
    int flag = 1;
    struct sockaddr_in sock_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        DBG("socket error\n")
        return -1;
    }

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1) {
        close(sockfd);
        perror("setsockpot() error\n");
        return -1;
    }

    if (bind(sockfd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
        close(sockfd);
        DBG("bind error\n")
        return -1;
    }
    if (listen(sockfd, 20) < 0) {
        close(sockfd);
        DBG("listen error\n")
        return -1;
    }
    DBG("sockfd creat success\n")
    return sockfd;
}

int socket_connect(int port, char *host) {
    int sockfd;
    struct sockaddr_in dest_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        DBG("socket() error\n")
        return -1;
    }
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = inet_addr(host);
    DBG("port = %d host = %s\n", port, host)
    if (connect(sockfd, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) < 0) {
        return -1;
    }
    return sockfd;
}

int master_send(int sockfd, char *request) {
    int a;
    if ((a = send(sockfd, request, 3, 0)) > 0) {
        DBG("%d\n", a)
        DBG("send success\n")
        return a;
    }
    DBG("sockfd = %d\n", sockfd)
    DBG("send failed\n")
    perror("send");
    return -1;
}

int master_receve(int sockfd, char *buffer, char path[]) {
    int a;
    FILE *output = fopen(path, "a+");
    while ((a = recv(sockfd, buffer, 4500, 0)) > 0) {
        fputs(buffer, output);
        DBG("%s\n", buffer)
        DBG("a = %d\n", a)
        memset(buffer, 0, 5000);
    }
    close(sockfd);
    fclose(output);
    DBG("close shortscokfd\n")
    write_to_log("short coonect end\n");
    return 1;
}

int write_to_log(char *buffer) {
    char *str = (char *)calloc(800, sizeof(char));
    char *pattern = (char *)calloc(120, sizeof(char));
    FILE *read = popen("./time.sh", "r");
    DBG("执行脚本time.sh\n")
    while (fgets(pattern, 100, read) != NULL) {
        sprintf(str + strlen(str), "%s", pattern);
        memset(pattern, 0, 120);
    }
    pclose(read);
    DBG("关闭脚本time.sh\n")
    sprintf(str + strlen(str), "%s", buffer);
    FILE *in = fopen("./log/log", "a+");
    DBG("open in file\n")
    fputs(str, in);
    fclose(in);
    DBG("close in file\n")
    memset(str, 0, 800);
    DBG("file in is closed\n")
    return 1;
}

#endif
