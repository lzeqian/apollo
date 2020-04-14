//
// Created by lzeqian<973465719@qq.com> on 2020/4/7.
//
#include "global.h"
#ifndef NGINX_APOLLOCLIENT_H
#define NGINX_APOLLOCLIENT_H

#endif //NGINX_APOLLOCLIENT_H
/**
 * 不带缓存配置信息url地址
 */
#define APOLLO_CONFIG_NOCACHE_URL "http://%s/configs/%s/%s/%s"
/**
 * http long pooling对应url地址
 */
#define APOLLO_CONFIG_NOTI_URL "http://%s/notifications/v2?appId=%s&cluster=%s&notifications=%s"
#define NOTIFICATION_ID_PLACEHOLDER -1
typedef struct {
    String meta; //apollometa服务器ip和地址
    String appId;//应用名称,比如网关gateway
    String clusterName;//集群名称，默认default
    String namespaceName;//命名空间名称 如application.ymal
} apollo_env;
typedef struct {
    String namespaceName; //需要配置更新的namespace
    int notificationId;//当前的通知值，初始化是-1
} notification;
//定义回调函数，用于存在更新后的数据


/**
 * 获取当前环境的所有配置，返回字符串
 * @param apolloEnv 环境信息
 * @return
 */
String getNoCachePropertyString(apollo_env apolloEnv);
/**
 * 返回不带缓存的配置信息，自动填充到Properties中
 * @param apolloEnv
 * @param properties
 */
void getNoCacheProperty(apollo_env apolloEnv,Properties* properties);
/**
 * 长更新检测
 *  1.请求远端服务，带上自己的应用信息以及notifications信息，第一次提交notificationId：-1
 *     如:[{"namespaceName": "application", "notificationId": -1}],注意使用url转码
    2. 服务端针对传过来的每一个namespace和对应的notificationId，检查notificationId是否是最新的
       如果是-1，返回初始化的notificationId。
       如果是最新的（传入的notificationId和服务保持一致），则保持住请求60秒，如果60秒内没有配置变化，则返回HttpStatus 304（此时不会返回任何数据只有状态码）。
          如果60秒内有配置变化，则返回对应namespace的最新notificationId, 返回HttpStatus 200。
       如果传过来的notifications信息中发现有notificationId比服务端老，则直接返回对应namespace的最新notificationId, HttpStatus 200。
    3. 客户端拿到服务端返回后，判断返回的HttpStatus
      如果返回的HttpStatus是304，说明配置没有变化，重新执行第1步
      如果返回的HttpStauts是200，说明配置有变化，针对变化的namespace重新去服务端拉取配置，参见1.3 通过不带缓存的Http接口从Apollo读取配置。同时更新notifications map中的notificationId。重新执行第1步。
 * @param apolloEnv
 * @param notification数组
 */
void submitNotifications(apollo_env apolloEnv,notification notifications,int* flag,void (*callback)(Properties* old,Properties* new));
/**
 * 独立线程运行长更新检测，异步模式
 * @param apolloEnv
 * @param notifications
 * @param flag
 * @param callback
 * @return
 */
int submitNotificationsAsync(apollo_env apolloEnv,notification notifications,int* flag,void (*callback)(Properties* old,Properties* new));