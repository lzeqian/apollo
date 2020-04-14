/**
    Created by lzeqian<973465719@qq.com> on 2020/4/7.
**/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <curl/curl.h>
#include <json-c/json.h>
#ifndef NGINX_GLOBAL_H
#define NGINX_GLOBAL_H

#endif
#define DEFAULT_CHARSIZE 200
#define newString(len) \
    len!=0?(char*)malloc(sizeof(char)*len):(char*)malloc(sizeof(char)*DEFAULT_CHARSIZE);
#define CONTENT_HEADER "Content-Type:application/json;charset=UTF-8"
#define DEFAULT_CLUSTER_NAME "default"
typedef char* String;
typedef struct {
    size_t len; /**键值对个数**/
    String* keys; /**键数组**/
    String* values; /**值数组**/
} Properties;
char* sprintfStr(char* pattern,...);
size_t write_memory_callback(void *ptr, size_t size, size_t nmemb, void *stream);
struct curl_slist * setCommonHeader(CURLcode* res,String tokenHeader);
void jsonStrToProperties(String properStr,String childNodeName,Properties* properties);