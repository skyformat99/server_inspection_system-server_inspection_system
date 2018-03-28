#include "client2.h"

int main() {
    int pids;
    if ((pids = fork()) < 0) {
        DBG("fork() error\n")
        write_to_log("pids < 0, fork error");
    }
    if (pids == 0) {
        char bash[5][20] = {"./CPU.sh", "./MEM.sh", "./DISK.sh", "./system.sh", "./user.sh"};
        char path[5][20] = {"./log/cpu.log", "./log/mem.log", "./log/disk.log", "./log/system.log", "./log/user.log"};
        int sockfd, listen_sockfd, a;
        int port = 9911, new_port = 9922;
        int *pid = (int *)calloc(7, sizeof(int));
        int i;
        for (i = 0; i < 7; ++i) {
            if ((pid[i] = fork()) < 0) {
                DBG("pid[%d] fork error\n", i)
            }
            if (pid[i] == 0) {
                break;
            }
        }
        if (pid[i] == 0) { 
            if (i == 5) {
                struct sockaddr_in client_addr;
                socklen_t len1 = sizeof(client_addr);
                char *buffer = (char *)calloc(50, sizeof(char));
                for (int i = 0; i < 3; ++i) {
                    if ((listen_sockfd = socket_creat(port)) < 0) {
                        DBG("listen_sockfd creat error\n")
                        if (i == 2) {
                            DBG("listen_sockfd creat failed\n")
                            write_to_log("listen_sockfd creat failed\n");
                            exit(0);
                        }
                    } else {
                        DBG("listen_sockfd = %d\n", listen_sockfd)
                        DBG("listen_sockfd creat success\n")
                        write_to_log("long connet listen sockfd creat success\n");
                        break;
                    }
                }
                while(1) {
                    if ((sockfd = accept(listen_sockfd, (struct sockaddr *)&client_addr, &len1)) < 0) {
                        perror("accept");
                        write_to_log("long connect is blocked\n");
                        break;
                    }
                    DBG("accept ok8\n")
                    write_to_log("request connect start\n");
                    for (int i = 0; i < 5; ++i) {
                        if ((a = recv(sockfd, buffer, 3, 0)) <= 0) {
                            DBG("recv path[%d] error\n", i)
                            break;
                        }
                        DBG("buffer = %s\n", buffer)
                        if (strncmp(buffer, "101", 3) == 0) {
                            DBG("path = %s\n", path[0])
                            client_send(sockfd, path[0], new_port);
                            write_to_log("cpu.log have send\n");
                            continue;
                        }
                        if (strncmp(buffer, "102", 3) == 0) {
                            DBG("path = %s\n", path[1])
                            client_send(sockfd, path[1], new_port);
                            write_to_log("mem.log have send\n");
                            continue;
                        }
                        if (strncmp(buffer, "103", 3) == 0) {
                            DBG("path = %s\n", path[2])
                            client_send(sockfd, path[2], new_port);
                            write_to_log("disk.log have send\n");
                            continue;
                        }
                        if (strncmp(buffer, "104", 3) == 0) {
                            DBG("path = %s\n", path[3])
                            client_send(sockfd, path[3], new_port);
                            write_to_log("system.log have send\n");
                            continue;
                        }
                        if (strncmp(buffer, "105", 3) == 0) {
                            DBG("path = %s\n", path[4])
                            client_send(sockfd, path[4], new_port);
                            write_to_log("user.log have send\n");
                            continue;
                        }
                    }
                }
                close(listen_sockfd);
                close(sockfd);
                write_to_log("long connect end\n");  
                }else if(i == 6) {
                    int warn_port = 9933;
                    client_send_warn(warn_port);
                }  else {
                char *str = (char *)calloc(100000, sizeof(char));
                char *pattern = (char *)calloc(300, sizeof(char));
                while (1) {
                    for (int j = 0; j < 5; ++j) {
                        FILE *input = popen(bash[i], "r");
                        DBG("popen input\n")
                        while (fgets(pattern, 280, input) != NULL) {
                            sprintf(str + strlen(str), "%s", pattern);
                            DBG("str = %s\n", str)
                            memset(pattern, 0, 300);
                        }
                        pclose(input);
                        DBG("pclose input\n")
                        sleep(2);
                    }
                    FILE *input = fopen(path[i], "a+");
                    fputs(str, input);
                    fclose(input);
                    DBG("%s is written\n", bash[i])
                    memset(str, 0, 100000);
                }
                free(str); 
            }
        }
    }
    exit(0);
}
