#include "yafetch.h"
#include <lualib.h>
#include <lauxlib.h>

lua_State *L;

/* Print error and exit */
void error(const char *message) {
    PDBD("Calling fputs() to stderr");
    fputs(message, stderr);
}

/* Initialize lua vm */
void script_init(void) {
    PDBD("Initialising script");

    L = luaL_newstate();
    luaL_openlibs(L);
}

/* Default config file location */
const char *config_location(void) {
    PDBD("Getting config location");

    PDBD("getenv(\"XDG_CONFIG_HOME\")");
    char *xdg_conf = getenv("XDG_CONFIG_HOME");

    PDBD("Checking if XDG_CONFIG_HOME exists");
    const char *cfg = xdg_conf
                          ? strcat(xdg_conf, "/yafetch/init.lua")
                          : strcat(getenv("HOME"), "/.config/yafetch/init.lua");

    PDBD("Returning valid config");
    return access(cfg, F_OK) == 0 ? cfg : "/usr/share/yafetch/init.lua";
}

/* Run yafetch.init() from config */
void script_run(const char *filename) {
    PDBD("Running lua script");

    /* Load config */
    (void)(luaL_loadfile(L, filename) || lua_pcall(L, 0, LUA_MULTRET, 0));

    /* Look for a table called 'yafetch' */
    lua_getglobal(L, "yafetch");
    if (!lua_istable(L, -1)) {
        error("Global yafetch is not a table\n");
        return;
    }

    /* Look for a function called 'init' in table 'yafetch' */
    lua_getfield(L, -1, "init");
    if (!lua_isfunction(L, -1)) {
        error("Global yafetch.init is not a function\n");
        return;
    }

    /* Run the function */
    lua_pcall(L, 0, 0, 0);

    lua_pop(L, 1);
}

/* Close lua vm */
void script_destroy(void) {
    PDBD("CLosing lua VM");
    lua_close(L);
}
