//
// Created by lzeqian<973465719@qq.com> on 2020/4/7.
//
#include "apolloclient.h"
#include "pthread.h"
String getNoCachePropertyString(apollo_env apolloEnv){
    CURL *curl = curl_easy_init();
    if(curl) {
        CURLcode res;
        String str=newString(10000);
        memset(str, 0, 10000);
        if(apolloEnv.clusterName==NULL){
            apolloEnv.clusterName=DEFAULT_CLUSTER_NAME;
        }
        String urlDest=sprintfStr(APOLLO_CONFIG_NOCACHE_URL, \
                apolloEnv.meta,apolloEnv.appId,apolloEnv.clusterName,apolloEnv.namespaceName);
        struct curl_slist *headers =setCommonHeader(&res,NULL);
        curl_easy_setopt(curl, CURLOPT_URL, urlDest);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, str);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        free(urlDest);
        return str;
    }
}
void getNoCacheProperty(apollo_env apolloEnv,Properties* properties){
    String properStr=getNoCachePropertyString(apolloEnv);
    jsonStrToProperties(properStr,"configurations",properties);
}
CURLcode checkNotifications(apollo_env apolloEnv,notification notifications,long* response_code,String notificationReturn){
    CURL *curl = curl_easy_init();
    char* buf=malloc(200);
    if(curl) {
        CURLcode res;
        if(apolloEnv.clusterName==NULL){
            apolloEnv.clusterName=DEFAULT_CLUSTER_NAME;
        }
        String notification=newString(200);
        memset(notification, 0, 200);
        strcat(notification,"[");
        strcat(notification,"{");
        strcat(notification,"\"namespaceName\": \"");
        strcat(notification,notifications.namespaceName);
        strcat(notification,"\",");
        strcat(notification,"\"notificationId\": \"");
        strcat(notification,sprintfStr("%d",notifications.notificationId));
        strcat(notification,"\"");
        strcat(notification,"}");
        strcat(notification,"]");
        notification=curl_escape(notification,strlen(notification));
        String urlDest=sprintfStr(APOLLO_CONFIG_NOTI_URL, \
                apolloEnv.meta,apolloEnv.appId,apolloEnv.clusterName,notification);
        struct curl_slist *headers =setCommonHeader(&res,NULL);
        curl_easy_setopt(curl, CURLOPT_URL, urlDest);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, notificationReturn);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        res = curl_easy_perform(curl);
        if(res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, response_code);
        }
        curl_easy_cleanup(curl);
        free(urlDest);
        return res;
    }

}

typedef struct
{
    void *arg[4]; /* 参数*/
}ThreadParam;
void *notificationRun(void *arg)
{
    ThreadParam* param=(ThreadParam*)arg;
    apollo_env apolloEnv=*(apollo_env*)param->arg[0];
    notification notifications=*(notification*)param->arg[1];
    int* flag=(int*)param->arg[2];
    void (*callback)(Properties* old,Properties* new);
    callback=param->arg[3];
    submitNotifications(apolloEnv,notifications,flag,callback);
    return NULL;
}

int submitNotificationsAsync(apollo_env apolloEnv,notification notifications,int* flag,void (*callback)(Properties* old,Properties* new)){
    ThreadParam param={0};
    param.arg[0]=&apolloEnv;
    param.arg[1]=&notifications;
    param.arg[2]=flag;
    param.arg[3]=callback;
    pthread_t tid;
    int tret = pthread_create(&tid, NULL, notificationRun, (void *)&param);
    return tret;
}

void submitNotifications(apollo_env apolloEnv,notification notifications,int* flag,void (*callback)(Properties* old,Properties* new)){
    //第一次获取资源信息。
    Properties oldProperties;
    getNoCacheProperty(apolloEnv,&oldProperties);
    callback(NULL,&oldProperties);
    long responseCode;
    String notiStr=newString(1000);
    memset(notiStr, 0, 1000);
    Properties newProperties;
    CURLcode res=checkNotifications(apolloEnv,notifications,&responseCode,notiStr);
    //根据动态flag决定是否需要继续long pooling
    while(flag>0) {
        if (res == CURLE_OK) {
            //304直接使用当前数据直接调用当前方法继续递归
            if (res == 304) {
                res=checkNotifications(apolloEnv,notifications,&responseCode,notiStr);
                continue;
            } else {
                //200不管是否都需要获取最新的配置项
                if (responseCode == 200) {
                    //获取最新的配置。
                    getNoCacheProperty(apolloEnv,&newProperties);
                    json_object *jsonObject = json_tokener_parse(notiStr);
                    json_object *notifyObj = json_object_array_get_idx(jsonObject, 0);
                    json_object *notificationIdObject;
                    json_object_object_get_ex(notifyObj, "notificationId", &notificationIdObject);
                    int notificationId = json_object_get_int(notificationIdObject);
                    if(notifications.notificationId>NOTIFICATION_ID_PLACEHOLDER){
                        callback(&oldProperties, &newProperties);
                    }
                    notifications.notificationId = notificationId;
                    oldProperties=newProperties;
//                    newProperties.len=0;
//                    free(newProperties.keys);
//                    free(newProperties.values);
                    memset(notiStr, 0, 1000);
                    res=checkNotifications(apolloEnv,notifications,&responseCode,notiStr);
                }
            }

        }
    }
}