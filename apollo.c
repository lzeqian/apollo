//
// Created by lzeqian<973465719@qq.com> on 2020/4/2.
//
#include "string.h"
#include <apollo.h>
#include <curl/curl.h>
#include <json-c/json.h>



String getApolloConfig(ApolloConf apolloConf,String envName,String appId,String clusterName,String namespaceName){
    CURL *curl = curl_easy_init();
    if(curl) {
        CURLcode res;
        static char str[10000];
        strcpy(str, "");
        if(clusterName==NULL){
            clusterName=DEFAULT_CLUSTER_NAME;
        }
        String urlDest=sprintfStr(LATESTREKEASE_URL, \
                apolloConf.config_server_url,envName,appId,clusterName,namespaceName);
        String tokenHeader=sprintfStr(TOKEN_PATTERN,apolloConf.token);
        struct curl_slist *headers =setCommonHeader(&res,tokenHeader);
        curl_easy_setopt(curl, CURLOPT_URL, urlDest);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, str);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        free(tokenHeader);
        free(urlDest);
        return str;
    }
    return NULL;
}
void getApolloClusterEnv(ApolloConf apolloConf,String appId,envclusters* ec){
    CURL *curl = curl_easy_init();
    if(curl) {
        CURLcode res;
        static char str[10000];
        strcpy(str, "");
        String urlDest=sprintfStr(CLUSTERENV_URL, \
                apolloConf.config_server_url,appId);
        String tokenHeader=sprintfStr(TOKEN_PATTERN,apolloConf.token);
        struct curl_slist *headers =setCommonHeader(&res,tokenHeader);
        curl_easy_setopt(curl, CURLOPT_URL, urlDest);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, str);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        free(tokenHeader);
        free(urlDest);
        json_object* jsonObject=json_tokener_parse(str);
        size_t objectSize=json_object_array_length(jsonObject);
        ec->len=objectSize;
        envcluster* envc=(envcluster*)malloc(objectSize*sizeof(envcluster));
        for(int v=0;v<objectSize;v++){
                json_object* clusterObj=json_object_array_get_idx(jsonObject,v);
            json_object* env;
            json_object_object_get_ex(clusterObj,"env",&env);
            json_object* clusters;
            json_object_object_get_ex(clusterObj,"clusters",&clusters);
            String clustersArray[json_object_array_length(clusters)];
            for(int i=0;i<json_object_array_length(clusters);i++){
                json_object* clusterObj=json_object_array_get_idx(clusters,i);
                clustersArray[i]=json_object_to_json_string(clusterObj);
            }
            envcluster envclusterTmp={
                env:json_object_to_json_string(env),
                clusters:clustersArray
            };
            *(envc+v)=envclusterTmp;
        }
        ec->clusters=envc;
    }
}
void getProperties(ApolloConf apolloConf,String envName,String appId,String clusterName,String namespaceName,Properties* properties){
    String str=getApolloConfig(apolloConf,envName,appId,clusterName,namespaceName);
    json_object* jsonObject=json_tokener_parse(str);
    json_object* configurations;
    json_object_object_get_ex(jsonObject,"configurations",&configurations);
    int i=0;
    size_t configurationsLength=json_object_object_length(configurations);
    String* keys=newString(sizeof(String)*configurationsLength);
    String* values=newString(sizeof(String)*configurationsLength);
    properties->len=configurationsLength;
    json_object_object_foreach(configurations,json_key,jsonValue){
        keys[i]=json_key;
        values[i]=json_object_get_string(jsonValue);
        i++;
    }
    properties->keys=keys;
    properties->values=values;
}
void getYamlOrJson(ApolloConf apolloConf,String envName,String appId,String clusterName,String namespaceName,String yamlStr){
    Properties* properties;
    getProperties(apolloConf,envName,appId,clusterName,namespaceName,properties);
    for(int i=0;i<properties->len;i++){
        if(strcmp(*(properties+i)->keys,"content")==0){
            strcat(yamlStr,*(properties)->values);
        }
    }
}