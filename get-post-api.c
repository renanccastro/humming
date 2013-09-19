#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <ctype.h>
#include <regex.h>
#include "regexp_match.h"

size_t writeFunc(void *ptr, size_t size, size_t nmemb, char** string){
    if(ptr == NULL || size > 20)
        return 0;
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
int isNumerical(char * string){
    int i;
    for(i=0; i<strlen(string); i++)
        if(!(string[i] >= '0' && string[i] <= '9'))
            return 0;
    return 1;
}

char* getAnimeID(char* string){
    char *anime_title = string;
    char *new_anime = malloc(sizeof(char)*strlen(string));
    char *parte;
    int i;
    char *p;
    for ( ; *anime_title; ++anime_title)
    {
        if(*anime_title == '_')
            *(anime_title) = '-';
    }

    parte = strtok (string," ,.-@");
    strcpy(new_anime, "-");
    while (parte!= NULL)
    {
        if(!isSub(parte) && !isVideoFormat(parte)){
            if(!isNumerical(parte)){
                strcat(new_anime, "-");
                strcat(new_anime, parte);
            }
            printf ("%s\n",parte);
        }

        parte = strtok (NULL, " ,.-@");
    }
    if(new_anime[0] == '-')
        new_anime+=2;
    for(i=0;i< strlen(new_anime); i++)
        if(new_anime[i] >= 'A' && new_anime[i] <= 'Z')
            new_anime[i] = tolower(new_anime[i]);
    return new_anime;
}

char* getUserToken(char* username, char* password){

    char* access_token = malloc(sizeof(char)*21);
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
    char senha[]="123";


    char post[500];
    char p[] ="[UTW]Rapunzel.[UTW-Mazui]_Toaru_Kagaku_no_Railgun_S_-_15_[720p][CFCB76A2].mkv";
    char *a;
    char *anime_title = p;
    char *access_token = NULL;

    access_token = getUserToken(username, senha);

    printf("token: %s\n",access_token);
    a = getAnimeID(p);
    printf("%s",a);
    return 0;
}

