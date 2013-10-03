#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <ctype.h>
#include <regex.h>
#include <jansson.h>
#include "regexp_match.h"
#define AIRING
#define FINISHED

typedef struct anime{
    char* id;
    char* status;
    char* title;
    int episode_count;
    char* synopsis;
    char* show_type;
    char** genres;
    int nGenres;
} anime;

typedef struct configuration{
    char* username;
    char* password;
    char* mashape_key;
    char* anime_folder;
    char* mplayer_command;
} config;

int removeSubString(char*src, char*sub){
    char *p;
    if ((p=strstr(src,sub)) != NULL)
    {
        memmove(p,p+strlen(sub), strlen(p+strlen(sub))+1);
        return 1;
    }
    return 0;
}

size_t writeFunc(void *ptr, size_t size, size_t nmemb, char** string){
    if(ptr == NULL || size > 20)
        return 0;
    strcpy(*string,ptr);
}
size_t writeAnimeInfo(void *ptr, size_t size, size_t nmemb, json_t** string){
    json_error_t error;
    *string = json_loads(ptr,JSON_DISABLE_EOF_CHECK, &error);
}
int isSub(char* string){
    if(er_match("\\]|\\[", string))
        return 1;
    return 0;
}
int isVideoFormat(char* string){
    if(er_match("CR|CA|mkv|avi|mpg|mp4|rmvb|mpeg|wmv|1280x720|hi10p|aac|mp3|flac",string))
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

char* getAnimeID(char* string, char **ep_number){
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

    parte = strtok (string," (,;.-@");
    strcpy(new_anime, "-");
    while (parte!= NULL)
    {
        if(!isSub(parte) && !isVideoFormat(parte)){
            if(!isNumerical(parte)){
                strcat(new_anime, "-");
                strcat(new_anime, parte);
            }
            else{
                *ep_number = malloc(sizeof(char)*strlen(parte));
                strcpy(*ep_number, parte);
            }
        }

        parte = strtok (NULL, " ;,.(-@");
    }
    if(new_anime[0] == '-')
        new_anime+=2;
    for(i=0;i< strlen(new_anime); i++)
        if(new_anime[i] >= 'A' && new_anime[i] <= 'Z')
            new_anime[i] = tolower(new_anime[i]);
    return new_anime;
}

char* getUserToken(config preferences){

    char* access_token = malloc(sizeof(char)*21);
    char post[500];
    CURL *curl;
    CURLcode res;

    struct curl_slist *chunk = NULL;

    curl = curl_easy_init();

    chunk = curl_slist_append(chunk, preferences.mashape_key);


    curl_easy_setopt(curl,CURLOPT_URL,"https://hummingbirdv1.p.mashape.com/users/authenticate");
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, writeFunc);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA, &access_token);


    sprintf(post,"email=%s&password=%s",preferences.username,preferences.password);

    curl_easy_setopt(curl,CURLOPT_POSTFIELDS, post);

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return access_token;
}


anime* getAnimeInfo(char* animeID, config preferences){
    char* access_token = malloc(sizeof(char)*21);
    char* url = malloc(sizeof(char)*(strlen(animeID) + 42));
    anime* temp = malloc(sizeof(anime));
    json_t* array;
    json_t* a;
    size_t i;
    json_t *animeInfo;
    CURL *curl;
    CURLcode res;

    sprintf(url, "https://hummingbirdv1.p.mashape.com/anime/%s", animeID);
    struct curl_slist *chunk = NULL;

    curl = curl_easy_init();

    chunk = curl_slist_append(chunk, preferences.mashape_key);


    curl_easy_setopt(curl,CURLOPT_URL,url);
    curl_easy_setopt(curl,CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, writeAnimeInfo);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA, &animeInfo);


    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if(!animeInfo){
        return NULL;
    }
    json_unpack(animeInfo, "{s:s s:s s:s s:i s:s s:s}",\
            "slug",&temp->id,"title",&temp->title, "status",&temp->status,\
            "episode_count",&temp->episode_count,"synopsis",&temp->synopsis,\
            "show_type", &temp->show_type);

    array = json_object_get(animeInfo,"genres");
    temp->nGenres = json_array_size(array);
    temp->genres = malloc(sizeof(char*)*temp->nGenres);
    printf("%d", temp->nGenres);
    for(i=0;i<temp->nGenres;i++){
        a = json_array_get(array,i);
        json_unpack(a, "{s:s}","name",&temp->genres[i]);
    }


    return temp;

}
char** getCurrentPlayingAnime(config preferences){

    FILE *fp;
    int status;
    char path[1035];
    char** vetor = calloc(2,sizeof(char*));

    /* Open the command for reading. */
    fp = popen(preferences.mplayer_command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit;
    }

    /* Read the output a line at a time - output it. */
    fgets(path, sizeof(path)-1, fp);
    /*if(!strstr(path, preferences.anime_folder))*/
        /*return NULL;*/
    if(!removeSubString(path, preferences.anime_folder))
        return NULL;

    /* close */
    pclose(fp);
    vetor[0] = getAnimeID(path, &vetor[1]);
    return vetor;
}

int main(){
    char username[]="renanccastro@gmail.com";
    char senha[]="";
    config preferences;
    preferences.username = username;
    preferences.password = senha;
    preferences.mashape_key = "X-Mashape-Authorization: qikle7pj3leoShq3yKkWHlBa5wrTJBFO";
    preferences.anime_folder = "/home/squarezin/Downloads/";
    char* mplayer_command = malloc(sizeof(char)*(67+strlen(preferences.anime_folder)));
    sprintf(mplayer_command,"lsof -p $(pidof mplayer) 2> error | grep \"%s\" |  cut --delimiter=' ' -f29-",preferences.anime_folder);
    preferences.mplayer_command = mplayer_command;
    char *ep_number;
    char *a;
    char *access_token = NULL;

    access_token = getUserToken(preferences);

    printf("token: %s\n",access_token);
    /*a = getAnimeID(p, &ep_number);*/
    /*printf("%s:%s\n",a,ep_number);*/
    char** vet = getCurrentPlayingAnime(preferences);
    if(vet)
        printf("%s",getCurrentPlayingAnime(preferences)[0]);
    else
        printf("Not running mplayer at this moment, or couldn't detect it.\n");
    return 0;
}

