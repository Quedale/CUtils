#ifndef URL_PARSER_H
#define URL_PARSER_H

typedef struct _ParsedURL ParsedURL;

struct _ParsedURL {
    char * protocol;
    char * hostorip;
    char * port;
    char * service;
};

ParsedURL * ParsedURL__create(char * url);
void ParsedURL__destroy(ParsedURL * self);
void ParsedURL__set_port(ParsedURL * self, char * port);
char * ParsedURL__toString(ParsedURL * self);

char * URL__set_port(char * url, char * port);

#endif