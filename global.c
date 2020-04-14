/**
// Created by lzeqian<973465719@qq.com> on 2020/4/7.
 */
#include "global.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <curl/curl.h>
#include <json-c/json.h>
char* sprintfStr(char* pattern,...){
    char* buf=malloc(200);
    va_list args;  /**定义参数指针，获取可选参数 */
    va_start(args,pattern);     /**初始化参数指针，将ap指向第一个实际参数的地址 */
    vsprintf(buf,pattern,args);
    va_end(args);
    return buf;
}

/**
 * 写入到内存回调
 * @param ptr 当前读取的字符串
 * @param size
 * @param nmemb 字节长度
 * @param stream 用户写入追加的流
 * @return
 */
size_t write_memory_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    if (strlen((char *)stream) + strlen((char *)ptr) > 999999) return 0;
    size_t len = size * nmemb;
    char* streamChar=(char*)stream;
    strcat(streamChar,(char*)ptr);
    return len;
}

struct curl_slist * setCommonHeader(CURLcode* res,String tokenHeader){
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, CONTENT_HEADER);
    if(tokenHeader) {
        headers = curl_slist_append(headers, tokenHeader);
    }
    return headers;
}
void jsonStrToProperties(String properStr,String childNodeName,Properties* properties){
    json_object* jsonObject=json_tokener_parse(properStr);
    json_object* configurations;
    if(childNodeName)
        json_object_object_get_ex(jsonObject,childNodeName,&configurations);
    else
        configurations=jsonObject;
    int i=0;
    size_t configurationsLength=json_object_object_length(configurations);
    String* keys=malloc(sizeof(char*)*configurationsLength);
    String* values=malloc(sizeof(char*)*configurationsLength);
    properties->len=configurationsLength;
    json_object_object_foreach(configurations,json_key,jsonValue){
        keys[i]=json_key;
        values[i]=json_object_get_string(jsonValue);
        i++;
    }
    properties->keys=keys;
    properties->values=values;
}