#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <ctype.h>
#include <regex.h>
#include "regexp_match.h"

size_t writeFunc(void *ptr, size_t size, size_t nmemb, char** string){
    *string = malloc(sizeof(char)*size);
    strcpy(*string,ptr);
}

int isSub(char* string){
    if(er_match("\\]|\\[", string))
        return 1;
    return 0;
}
int isVideoFormat(char* string){
    if(er_match("mkv|avi|mpg|mp4|rmvb|mpeg|wmv", string))
        return 1;
    return 0;
}

//falta implementar a parte de tratar as chunks e ver se sao nomes, etc.
char* getAnimeID(char* string){
    char *anime_title = string;
    char *new_anime = malloc(sizeof(char)*strlen(string));
    char *parte;
    char *p;
    for ( ; *anime_title; ++anime_title)
    {
        if(*anime_title == '_')
            *(anime_title) = '-';
    }

    parte = strtok (string," ,.-@");
    while (parte!= NULL)
    {
        if(!isSub(parte) && !isVideoFormat(parte)){
            strcat(new_anime, "_");
            strcat(new_anime, parte);
            printf ("%s\n",parte);
        }

        parte = strtok (NULL, " ,.-@");
    }
    if(new_anime[0] == '_')
       p = new_anime++;
    return new_anime;
}

char* getUserToken(char* username, char* password){

    char* access_token = NULL;
    char post[500];
    CURL *curl;
    CURLcode res;

    struct curl_slist *chunk = NULL;

    curl = curl_easy_init();

    chunk = curl_slist_append(chunk, "X-Mashape-Authorization: qikle7pj3leoShq3yKkWHlBa5wrTJBFO");


    curl_easy_setopt(curl,CURLOPT_URL,"https://hummingbirdv1.p.mashape.com/users/authenticate");
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, writeFunc);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA, &access_token);


    sprintf(post,"email=%s&password=%s",username,password);

    curl_easy_setopt(curl,CURLOPT_POSTFIELDS, post);

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return access_token;
}

int main(){
    char username[]="renanccastro@gmail.com";



    char post[500];
    char p[] = "[UTW]_Fate_Kaleid_Liner_Prisma_Ilya_-_03_[h264-720p][87A1BBBA].mkv";
    char *a;
    char *anime_title = p;
    char *access_token = NULL;

    access_token = getUserToken(username, senha);

    printf("token: %s\n",access_token);
    a = getAnimeID(p);
    printf("%s",a);
    return 0;
}

