#include "url_parser.h"
#include "clogger.h"
#include <stdlib.h>


char * URL__set_port(char * url, char * new_port){
    ParsedURL * purl = ParsedURL__create(url);
    ParsedURL__set_port(purl,new_port);
    char * new_url = ParsedURL__toString(purl);
    ParsedURL__destroy(purl);

    return new_url;
}

char * URL__set_host(char * url, char * new_host){
    ParsedURL * purl = ParsedURL__create(url);
    ParsedURL__set_host(purl,new_host);
    char * new_url = ParsedURL__toString(purl);
    ParsedURL__destroy(purl);

    return new_url;
}

void ParsedURL__destroy(ParsedURL * self){
    if(!self)
        return;
    if(self->protocol)
        free(self->protocol);
    if(self->hostorip)
        free(self->hostorip);
    if(self->port)
        free(self->port);
    if(self->service)
        free(self->service);
    free(self);
}

void ParsedURL__set_port(ParsedURL * self, char * port){
    if(!port){
        if(self->port){
            free(self->port);
            self->port = NULL;
            return;
        }
    }
    if(!self->port){
        self->port = malloc(strlen(port)+1);
    } else {
        self->port = realloc(self->port,strlen(port)+1);
    }
    strcpy(self->port,port);
}

void ParsedURL__set_host(ParsedURL * self, char * host){
    if(!host){
        if(self->hostorip){
            free(self->hostorip);
            self->hostorip = NULL;
            return;
        }
    }
    if(!self->hostorip){
        self->hostorip = malloc(strlen(host)+1);
    } else {
        self->hostorip = realloc(self->hostorip,strlen(host)+1);
    }
    strcpy(self->hostorip,host);
}

int ParsedURL__is_valid(ParsedURL * self){
    if(!self || !self->hostorip || !self->port || !self->protocol){
        return 0;
    }
    return 1;
}

char * ParsedURL__toString(ParsedURL * self){
    int len = strlen(self->protocol) + strlen(self->hostorip) + strlen(self->service) + 6;  //6 to support endofstring + '://' + '/' + '/'
    if(self->port){
        len += strlen(self->port) +1; //1 to support ':'
    }

    char * full_url = malloc(len);

    strcpy(full_url,self->protocol);
    strcat(full_url,"://");
    strcat(full_url,self->hostorip);
    if(self->port){
        strcat(full_url,":");
        strcat(full_url,self->port);
    }
    strcat(full_url,"/");
    strcat(full_url,self->service);

    return full_url;
}

ParsedURL * ParsedURL__create(char * url){

    ParsedURL * purl = malloc(sizeof(ParsedURL));

    char data_arr[strlen(url)+1];
    char * data = data_arr;
    memcpy(data,url,strlen(url)+1);

    if(strstr(data,"://")){
        char * tmpprot = strtok_r ((char *)data, "://", &data);
        purl->protocol = malloc(strlen(tmpprot)+1);
        strcpy(purl->protocol,tmpprot);
    } else {
        purl->protocol = NULL;
    }

    char *hostport = strtok_r ((char *)data, "/", &data);

    //TODO Support IPv6
    if(hostport){
        if(strstr(hostport,":")){ //If the port is set
            char * tmphost = strtok_r (hostport, ":", &hostport);
            purl->hostorip = malloc(strlen(tmphost)+1);
            strcpy(purl->hostorip,tmphost);

            char * tmpport = strtok_r ((char *)hostport, "/", &hostport);
            if(tmpport && strlen(tmpport) > 0){
                purl->port = malloc(strlen(tmpport)+1);
                strcpy(purl->port,tmpport);
            } else {
                purl->port = NULL;
            }
        } else { //If no port is set
            char * tmphost = strtok_r (hostport, ":", &hostport);
            purl->hostorip = malloc(strlen(tmphost)+1);
            strcpy(purl->hostorip,tmphost);
            purl->port = NULL;
        }
    } else {
        purl->hostorip = NULL;
        purl->port = NULL;
    }

    if(data && strlen(data) > 0){
        purl->service = malloc(strlen(data)+1);
        strcpy(purl->service,data);
    } else {
        purl->service = NULL;
    }

    return purl;
}