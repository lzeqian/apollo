//
// Created by jiaozi<973465719@qq.com> on 2020/4/2.
//

#include "global.h"
#ifndef NGINX_APOLLO_H
#define NGINX_APOLLO_H

#endif //NGINX_APOLLO_H
#define TOKEN_PATTERN "Authorization:%s"

#define LATESTREKEASE_URL "http://%s/openapi/v1/envs/%s/apps/%s/clusters/%s/namespaces/%s/releases/latest"
#define CLUSTERENV_URL "http://%s/openapi/v1/apps/%s/envclusters"



typedef struct{
    String config_server_url;
    String token;
} ApolloConf;
/**
 * 获取apollo配置
 * properties输出结果是：
 * {
    "appId": "test-0620-01",
    "clusterName": "test",
    "namespaceName": "application",
    "name": "2016-08-11",
    "configurations": {
        "timeout": "3000",
    }
  }，只需要获取configurations所有key即可。
 * yaml和json输出的结果是：
   直接获取["configurations"]["content"]
 * @param apolloConf Apollo配置服务的地址和token
 * @param envName 环境名称
 * @param appId 应用的appId
 * @param clusterName 集群名
 * @param namespaceName Namespace的名字
 * @param ip 应用部署的机器ip
 * @return 字符串结果，其他逻辑自行处理
 */
char* getApolloConfig(ApolloConf apolloConf,String envName,String appId,String clusterName,String namespaceName);
typedef struct{
    String env;
    String* clusters;
} envcluster;
typedef struct{
    size_t len;
    envcluster* clusters;
} envclusters;
/**
 * 获取App的环境，集群信息
 * [
    {
        "env":"FAT",
        "clusters":[ //集群列表
            "default",
            "FAT381"
        ]
    },
    {
        "env":"UAT",
        "clusters":[
            "default"
        ]
    },
    {
        "env":"PRO",
        "clusters":[
            "default",
            "SHAOY",
            "SHAJQ"
        ]
    }
]
 * @param apolloConf  Apollo配置服务的地址和token
 * @param appId 应用id
 * @param envclusters 集群环境信息，返回引用
 * @return
 */
void getApolloClusterEnv(ApolloConf apolloConf,String appId,envclusters* ec);


/**
 * 获取properties，可以获取所有类型 yaml和properties其实是在content这个key下面
 * @param apolloConf
 * @param envName
 * @param appId
 * @param clusterName
 * @param namespaceName
 * @return
 */
void getProperties(ApolloConf apolloConf,String envName,String appId,String clusterName,String namespaceName,Properties* properties);
/**
 * 获取yaml
 * @param apolloConf
 * @param envName
 * @param appId
 * @param clusterName
 * @param namespaceName
 * @return
 */
void getYamlOrJson(ApolloConf apolloConf,String envName,String appId,String clusterName,String namespaceName,String yamlStr);



