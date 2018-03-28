#ifndef _HEAD_H
#define _HEAD_H

#define DEBUG
#ifdef DEBUG
#define DBG(s, va_arg...) {\
    printf("FILE: %s LINE:[%d] %s", __FILE__, __LINE__, #s);\
    printf(s, ##va_arg);\
}
#else
#define DBG(s, va_arg...)
#endif

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
int search(char *, char *, char *);

int search(char* pathname, char* key_name, char *value) { 
	FILE *fp = NULL;
	char *line = NULL, *substr = NULL;
	size_t len = 0, tmplen = 0;
	ssize_t read;
	//memset(value, 0, sizeof(char)*MAX_SIZE);
 
	if ( key_name == NULL || value == NULL) {
		DBG("paramer is invaild!n");
		exit(-1);
	}
 
	fp = fopen(pathname,"r");
	if (fp == NULL) {
		DBG("Open config file error!n");
		exit(-1);
	}
 
	while (( read = getline(&line, &len,fp)) != -1) {
		substr = strstr(line, key_name);
		if (substr == NULL) 
			continue;
		else {
			tmplen = strlen(key_name);
			if (line[tmplen] == '=') {
				strncpy(value, &line[tmplen + 1], (int)read - tmplen + 1);
				tmplen = strlen(value);
				*(value + tmplen - 1) = '\0';
				break;
			}
			else {
				DBG("Maybe there is something wrong with config file!\n");
				continue;
			}
		}
	}
 
	if (substr == NULL) {
		DBG("%s not found in config file!\n", key_name);
		fclose(fp);
		exit(-1);
	}
 
	DBG("%s=%s\n", key_name, value);
	free(line);
	fclose(fp);
	return 0;
}
#endif
