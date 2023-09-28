#include "../src/clogger.h"

int main(int argc, char *argv[]) {
    c_log_set_level(C_ALL_E);
    C_TRACE(" trace line");
    C_DEBUG(" debug message");
    C_INFO(" information message");
    C_WARN(" warning message");
    C_ERROR(" error occured");
    C_FATAL(" fatal crash");
}