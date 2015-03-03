#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<curl/curl.h>
#include"cJSON.h"

/**
	curl的回写函数
**/
size_t write_data(void *buffer, size_t size, size_t nmemb, void *stream)
{
	return size*nmemb;
}

/**
	向Elasticsearch系统发送博客内容
**/
void post_blog(const char* content)
{
	cJSON	*root;//json结构的根结点
	char	*post_fields = NULL;//指向post内容的指针
	CURL 	*curl;//curl指针
	CURLcode res;//curl执行的返回值

	//创建并生成json结构
	root = cJSON_CreateObject();
	cJSON_AddItemToObject(root, "CONTENT", cJSON_CreateString(content));
	//由json结构生成其对应的字符串
	post_fields = cJSON_Print(root);
	cJSON_Delete(root);

	//curl指针的初始化
	curl = curl_easy_init();
	if(!curl)
	{
		fprintf(stderr,"%s curl init failed:%s\n",__func__,strerror(errno));
		return ;
	}
	//设置curl的一系列参数
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
	//设置发送到的url地址
	curl_easy_setopt(curl, CURLOPT_URL, "localhost:9200/blog/article?pretty");
	//执行发送
	res = curl_easy_perform(curl);
	if(res == CURLE_COULDNT_CONNECT)
	{
		fprintf(stderr,"%s can't connect to remote host:%s\n",__func__,strerror(errno));
		curl_easy_cleanup(curl);
		return ;
	}
	curl_easy_cleanup(curl);
	
}

/**
	主函数
**/
int main()
{
	char *content1 = "本文使用curl，实现实时像ES系统中存储博客内容的功能";
	char *content2 = "本文使用ik分词插件，实现了ES系统的中文分词功能";
	post_blog(content1);
	post_blog(content2);
	return 0;
}
