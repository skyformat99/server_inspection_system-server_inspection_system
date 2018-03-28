#ifndef _SERVER_H
#define _SERVER_H
#include "head.h"
int socket_creat(int port);
int client_send(int sockfd, char *path, int port);
int socket_connect(int port, char *ip);
int client_send_warn(int port);
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
    if (connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        DBG("connect error\n")
        write_to_log("connect error\n");
        return -1;
    }
    return sockfd;
}


int client_send_warn(int port) {
    FILE *input = popen("./doctor.sh", "r");
    char *buffer = (char *)calloc(200, sizeof(char));
    int i = 0, a;
    char ans[3][20] = {"cpu warning", "mem warning", "disk warning"};
    while (fscanf(input, "%s", buffer) != EOF) {
        if(strcmp(buffer, "warning") == 0) {
            int warn_sockfd;
            while(1) {
                if (warn_sockfd = socket_connect(port, "192.168.1.181") > 0) {
                    DBG("warn_sockfd ipen\n")
                    a = send(warn_sockfd, ans[i % 3], 20, 0);
                    if (a > 0) DBG("send success\n")
                    else DBG("send error\n")
                    break;
                } else {
                    DBG("warn connect connect error\n")
                    sleep(3);
                }
            }
            close(warn_sockfd);
            DBG("warn_sockfd closed\n")
            break;
        }
        ++i;
        memset(buffer, 0, 200);
    }
    sleep(3);
    return 0;
}

int client_send(int sockfd, char *path, int port) {
    int temp_sockfd, temp_listen_sockfd;
    struct sockaddr_in temp_addr;
    socklen_t len = sizeof(temp_addr);
    send(sockfd, "200", 3, 0);
    for (int i = 0; i < 3; ++i) {
        if ((temp_listen_sockfd = socket_creat(port)) < 0) {
            DBG("temp creat error\n")
            if (i == 2) {
                DBG("temp creat failed\n")
                close(temp_listen_sockfd);
                return -1;
            }
        } else {
            DBG("temp creat success\n")
            break;
        }
    }
    fflush(stdout);
    while (1) {
        if ((temp_sockfd = accept(temp_listen_sockfd, (struct sockaddr *) &temp_addr, &len)) < 0) {
            break;
        }
        DBG("accept ok\n")
        FILE *input = fopen(path, "r");
        flock(input->_fileno, LOCK_EX);
        //int warn_port = 8879;
        //client_send_warn(warn_port);
        DBG("input is open\n")
            //fflush(stdout);
        char *buf = (char *)malloc(sizeof(char) * 5000);
        memset(buf, 0, 5000);
        while (fgets(buf, 4500, input) != NULL) {
                //printf("enter while\n");
                //fflush(stdout);
            int size = strlen(buf);
            DBG("size = %d\n", size)
            fflush(stdout);
            send(temp_sockfd, buf, size, 0);
            //printf("enter send\n");
            //fflush(stdout);
            memset(buf, 0, 5000);
        }
        fclose(input);
        DBG("input is closed\n")
        input = fopen(path, "w");
        fclose(input);
        DBG("file is empty\n")
        free(buf);
        close(temp_sockfd);
        DBG("temp_sockfd closed\n")
        break;
    }
    close(temp_listen_sockfd);
    DBG("temp_listen_sockfd closed")
    sleep(2);
    return 1;
}

int write_to_log(char *buffer) {
    char *str = (char *)calloc(800, sizeof(char));
    char *pattern = (char *)calloc(120, sizeof(char));
    FILE *read = popen("./time.sh", "r");
    DBG("popen file read\n")
    while (fgets(pattern, 100, read) != NULL) {
        sprintf(str + strlen(str), "%s", pattern);
        memset(pattern, 0, 120);
    }
    pclose(read);
    DBG("pclose file read\n")
    sprintf(str + strlen(str), "%s", buffer);
    FILE *in = fopen("./log/log", "a+");
    DBG("file in is open\n")
    fputs(str, in);
    fclose(in);
    DBG("file in is closed\n")
    return 1;
}
#endif
