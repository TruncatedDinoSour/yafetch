#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <lua.h>

extern lua_State *L;

typedef short unsigned int pkg_t;
typedef long unsigned int pkgc_t;
typedef unsigned short int l_t;

#define RESET   "\x1B[0m"
#define RED     "\x1B[31m"
#define PKG_END " 2>/dev/null | wc -l"

#ifdef DEBUG
#define PDBD(message) fputs(" \x1B[1m* DEBUG: " RESET message "\n", stderr)
#else
#define PDBD(message)
#endif

void script_init(void);
void script_run(const char *filename);
void script_destroy(void);
void func_reg(void);
const char *config_location(void);
void error(const char *message);
