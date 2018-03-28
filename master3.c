#include "master.h"

int main() {
    int pids;
    if ((pids = fork()) < 0) {
        DBG("fork() error\n")
        write_to_log("pids fork error\n");    
    }
    if (pids == 0) {
        int host_nums, file_nums, a, port, new_port, start, end, warn_port;
        int sockfd, short_sockfd, warn_sockfd, warn_listen;
        char *value = (char *)calloc(100, sizeof(char));
        char *pathname = (char *)calloc(50, sizeof(char));
        strcpy(pathname, "./master.conf");

        search(pathname, "host_nums", value);
        host_nums = atoi(value);
        DBG("host_nums = %d\n", host_nums)
        memset(value, 0, sizeof(value)); 

        search(pathname, "file_nums", value);
        file_nums = atoi(value);
        DBG("file_nums = %d\n", file_nums)
        memset(value, 0, sizeof(value));

        search(pathname, "port", value);
        port = atoi(value);
        DBG("port = %d\n", port)
        memset(value, 0, sizeof(value));

        search(pathname, "new_port", value);
        new_port = atoi(value);
        DBG("new_port = %d\n", new_port)
        memset(value, 0, sizeof(value));

        search(pathname, "warn_port", value);
        warn_port = atoi(value);
        DBG("warn_port = %d\n", warn_port)
        memset(value, 0, sizeof(value));

        search(pathname, "start", value);
        start = atoi(value);
        DBG("start = %d\n", start)
        memset(value, 0, sizeof(value));

        search(pathname, "end", value);
        end = atoi(value);
        DBG("end = %d\n", end);
        memset(value, 0, sizeof(value));

        int *pid = (int *)calloc(12, sizeof(int));
        int i;
        for (i = 0; i < 11; ++i) {
            if ((pid[i] = fork()) < 0) {
                DBG("fork() error\n")
            }
            if (pid[i] == 0) {
                DBG("fork success\n")
                break;
            }
        }
        if (pid[i] == 0) {
            char *buffer = (char *)calloc(5000, sizeof(char));
            char request[6][10] = {"101", "102", "103", "104", "105", "106"};
            char file_path[5][20] = {"cpu.log", "mem.log", "disk.log", "system.log", "user.log"};
            char *name = (char *)calloc(30, sizeof(char));
            search(pathname, "pre_name", value);
            if (i >= start && i <= end) {
                sprintf(value + strlen(value), "%d", i);
                DBG("value = %s\n", value)
                sprintf(name, "%s", value);
                DBG("name = %s\n", name)
                memset(value, 0, 100);
                search(pathname, name, value);
                memset(name, 0, 30);
                sprintf(name, "%s", value);
                memset(value, 0, 100);
                DBG("new name = %s\n", name);
                while (1) {
                    for (int j = 0; j < 3; ++j) {
                        if ((sockfd = socket_connect(port, name)) < 0) {
                            DBG("socket_connect error\n")
                            perror("socket_connect");
                            if (j == 2) {
                                DBG("socket connect failed\n")
                                write_to_log("request socket_connect failed\n");
                            }
                        } else {
                            DBG("socket_connect success\n")
                            write_to_log("request socket_connect success\n");
                            DBG("sockfd = %d\n", sockfd)
                            for (int k = 0; k < file_nums; ++k) {
                                a = master_send(sockfd, request[k]);
                                DBG("request = %s, a = %d\n", request[k], a)
                                memset(buffer, 0, 5000);
                                a = recv(sockfd, buffer, 3, 0);
                                DBG("a = %d\n", a)
                                if (a > 0 && strncmp(buffer, "200", 3) == 0) {
                                    for (int j = 0; j < 3; ++j) {
                                        if ((short_sockfd = socket_connect(new_port, name)) < 0) {
                                            DBG("short_sockfd connect error\n")
                                            if (j == 2) {
                                                DBG("short_sockfd connect failed\n")
                                                write_to_log("short connect failed\n");
                                            }
                                        } else {
                                            DBG("short_sockfd = %d\n", short_sockfd);
                                            write_to_log("short connect success\n");
                                            break;
                                        }
                                    }
                                    memset(buffer, 0, 5000);
                                    search(pathname, "pubpath", value);
                                    char *new_path = (char *)calloc(400, sizeof(char));
                                    sprintf(new_path, "%s", value);
                                    memset(value, 0, 100);
                                    sprintf(new_path + strlen(new_path), "%d", i);
                                    sprintf(new_path + strlen(new_path), "%s", "/");
                                    sprintf(new_path + strlen(new_path), "%s", file_path[k]);
                                    DBG("new_path = %s\n", new_path);
                                    master_receve(short_sockfd, buffer, new_path);
                                    write_to_log("master has save file\n");
                                    free(new_path);
                                } else {
                                    DBG("request error\n")
                                    write_to_log("receve error request, save error\n");
                                }
                                sleep(1);
                            }    
                            break;
                        }
                    }
                    sleep(10);
                    close(sockfd);
                    write_to_log("long connect end\n");
                }
                free(buffer);
                free(name);
            } else if (i == 0) {
                struct sockaddr_in warn_client;
                socklen_t len = sizeof(warn_client);
                while (1) {
                    if ((warn_listen = socket_creat(warn_port)) < 0) {
                        DBG("warn_listen creat error\n")
                        write_to_log("warn listen creat error\n");
                    } else {
                        write_to_log("warn listen creat success\n");
                        break;
                    }
                    sleep(10);
                }
                while (1) {
                    if ((warn_sockfd = accept(warn_listen, (struct sockaddr *)&warn_client, &len)) < 0) {
                        DBG("appect error\n")
                        break;
                    }
                    char *buf = (char *)calloc(80, sizeof(char));
                    char *message = (char *)calloc(500, sizeof(char));
                    write_to_log("warn connect is start\n");
                    struct sockaddr_in peer;
                    socklen_t peer_len = sizeof(struct sockaddr_in);
                    bzero(&peer,sizeof(struct sockaddr_in));
                    getpeername(sockfd,(struct sockaddr *)&peer,&peer_len);
                    char buff_peer[64] = {'\0'};
                    inet_ntop(AF_INET,(void *)&peer.sin_addr,buff_peer,63);
                    sprintf(message, "%s", buff_peer);
                    sprintf(message + strlen(message), "%s", "master has receve warning message");
                    FILE *input = fopen("./log/warn", "r");
                    while ((a = recv(warn_sockfd, buf, 65, 0)) > 0) {
                        write_to_log(message);
                        DBG("a = %d\n", a)
                        DBG("buf = %s\n", buf)
                        fputs(buf, input);
                        memset(buf, 0, 50);
                    }
                    fclose(input);
                    close(warn_sockfd);
                    free(buf);
                    free(message);
                }
                    
            }
        }
    }
    exit(0);
}
