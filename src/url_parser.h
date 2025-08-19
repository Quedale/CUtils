#ifndef URL_PARSER_H
#define URL_PARSER_H

typedef struct _ParsedURL ParsedURL;

struct _ParsedURL {
    char * protocol;
    char * hostorip;
    char * port;
    char * service;
    char * user;
    char * pass;
};

ParsedURL * ParsedURL__create(char * url);
void ParsedURL__destroy(ParsedURL * self);
void ParsedURL__set_port(ParsedURL * self, char * port);
void ParsedURL__set_host(ParsedURL * self, char * host);
void ParsedURL__set_protocol(ParsedURL * self, char * proto);
char* ParsedURL__get_port(ParsedURL * self);
char* ParsedURL__get_host(ParsedURL * self);
char* ParsedURL__get_protocol(ParsedURL * self);
char * ParsedURL__toString(ParsedURL * self);
int ParsedURL__is_valid(ParsedURL * self);
char * URL__set_port(char * url, char * port);
char * URL__set_host(char * url, char * port);

#endif
