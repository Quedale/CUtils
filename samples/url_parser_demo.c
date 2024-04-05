#include "../src/clogger.h"
#include "../src/url_parser.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
    c_log_set_level(C_ALL_E);

    ParsedURL * purl = ParsedURL__create("https://user:pass@domain.com:654/some/endpoint");

    char * str_url = ParsedURL__toString(purl);
    C_DEBUG("url : %s", str_url);
    free(str_url);

    C_INFO("Chaning host to 'newhost.ca'");
    ParsedURL__set_host(purl,"newhost.ca");
    str_url = ParsedURL__toString(purl);
    C_DEBUG("url : %s", str_url);
    free(str_url);

    C_INFO("Chaning port to '333'");
    ParsedURL__set_port(purl,"333");
    str_url = ParsedURL__toString(purl);
    C_DEBUG("url : %s", str_url);
    free(str_url);
    
    ParsedURL__destroy(purl);


    purl = ParsedURL__create("http://domain.com:654/some/endpoint");
    
    str_url = ParsedURL__toString(purl);
    C_DEBUG("url : %s", str_url);
    free(str_url);

    C_INFO("Chaning host to 'newhost.ca'");
    ParsedURL__set_host(purl,"newhost.ca");
    str_url = ParsedURL__toString(purl);
    C_DEBUG("url : %s", str_url);
    free(str_url);

    C_INFO("Chaning port to '333'");
    ParsedURL__set_port(purl,"333");
    str_url = ParsedURL__toString(purl);
    C_DEBUG("url : %s", str_url);
    free(str_url);
    
    ParsedURL__destroy(purl);
}