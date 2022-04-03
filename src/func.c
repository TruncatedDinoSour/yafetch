#include <lauxlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <sys/mount.h>
#include <sys/param.h>
#include "yafetch.h"

#define LFUNC(N) int lua_##N(lua_State *L)

/* yafetch.user() */
/* Returns username */
LFUNC(user) {
    uid_t uid         = geteuid();
    struct passwd *pw = getpwuid(uid);

    if (pw)
        lua_pushstring(L, pw->pw_name);
    else
        lua_pushstring(L, "unknown");

    return 1;
}

/* yafetch.distro() */
/* Returns name of linux distribution */
LFUNC(distro) {
    char *def = malloc(512);
    char *new = malloc(512);
    int line  = 0;

    FILE *os_release = fopen("/etc/os-release", "rt");

    while (fgets(def, 512, os_release)) {
        snprintf(new, 512, "%.*s", 511, def + 4);
        if (strncmp(new, "=", 1) == 0)
            break;
        line++;
    }

    fclose(os_release);
    free(def);

    if (strncmp(new, "=", 1) == 0) {
        int len = strlen(new);

        for (l_t ln = 0; ln < len; ln++) {
            if (new[ln] == '\"' || new[ln] == '\'' || new[ln] == '=') {
                for (l_t chr = 0; chr < len; chr++)
                    new[chr] = new[chr + 1];
                new[strlen(new) - 1] = '\0';
            }
        }
    }

    if (new)
        lua_pushstring(L, new);
    else
        lua_pushstring(L, "unknown");

    free(new);

    return 1;
}

/* yafetch.hostname() */
/* Returns hostname of the machine */
LFUNC(hostname) {
    /* Maximum characters of the hostname can be 255 on linux(+1 0 terminator)
     */
    char hostname[255];
    gethostname(hostname, 255);

    lua_pushstring(L, hostname);
    return 1;
}

/* yafetch.pkgs() */
/* Returns number of installed packages */
LFUNC(pkgs) {
    char *package_managers[9] = {
        "dnf list installed",
        "dpkg-query -f '${binary:Package}\n' -W",
        "q qlist -I",
        "nix-store -q --requisites /run/current-system/sw",
        "pacman -Qq",
        "rpm -qa --last",
        "xbps-query -l",
        "bonsai list",
        "apk info"};

    const pkg_t package_manager_count =
        sizeof package_managers / sizeof package_managers[0];

    pkgc_t total = 0;

    for (pkg_t pkg = 0; pkg < package_manager_count; ++pkg) {
        char full_cmd[70] = {0};
        strcpy(full_cmd, package_managers[pkg]);
        strcat(full_cmd, pkg_end);

        FILE *packages = popen(full_cmd, "r");

        if (packages) {
            pkgc_t tmp = 0;

            if (fscanf(packages, "%lu", &tmp) != 1) {
                perror("Failed to fscanf() for packages");
                return 0;
            }

            total += tmp;
        }

        pclose(packages);
    }

    if (total > 0)
        lua_pushinteger(L, total);
    else
        lua_pushinteger(L, 0);

    return 1;
}

/* yafetch.kernel() */
/* Returns kernel version */
LFUNC(kernel) {
    struct utsname sys;
    uname(&sys);

    char *kernel = sys.release;
    if (kernel)
        lua_pushstring(L, kernel);
    else
        lua_pushstring(L, "unknown");

    return 1;
}

/* yafetch.shell() */
/* Returns path of shell */
LFUNC(shell) {
    lua_getglobal(L, "yafetch");
    lua_getfield(L, -1, "shell_base");

    struct passwd *pw    = getpwuid(getuid());
    const int shell_full = lua_toboolean(L, -1);

    /* Get basename of shell by looking for last '/' */
    char *slash = strrchr(pw->pw_shell, '/');

    if (pw->pw_shell) {
        if (shell_full == 1)
            pw->pw_shell = slash + 1;

        lua_pushstring(L, pw->pw_shell);
    } else
        lua_pushstring(L, "unknown");

    return 1;
}

/* yafetch.header() */
LFUNC(header) {
    lua_getglobal(L, "yafetch");

    lua_getfield(L, -1, "header_sep");
    const char *sep = lua_tostring(L, -1);

    if (lua_isnil(L, -1) == 1)
        sep = "@";

    lua_getglobal(L, "yafetch");

    lua_getfield(L, -1, "header_sep_color");
    const char *sep_color = lua_tostring(L, -1);
    lua_pop(L, 0);

    if (lua_isnil(L, -1) == 1)
        sep_color = "";

    lua_getglobal(L, "yafetch");

    lua_getfield(L, -1, "header_format");
    const char *fmt = lua_tostring(L, -1);
    lua_pop(L, 0);

    if (lua_isnil(L, -1) == 1)
        fmt = "";
    else if (lua_isnone(L, -1) == 1)
        fmt = "";

    /* Get arguments from lua function */

    /* Header color */
    const char *h1_col;
    h1_col = "\033[0m";

    /* Second header color */
    const char *h2_col;
    h2_col = "\033[0m";

    /* Get hostname */
    char hostname[255];
    gethostname(hostname, 255);

    /* Get username */
    struct passwd *pw = getpwuid(geteuid());

    printf("%s%s%s%s%s%s%s%s%s%s\n", h1_col, fmt, pw->pw_name, reset, sep_color,
           sep, reset, h2_col, hostname, reset);

    return 1;
}

/* yafetch.format() */
/* Formats given strings. */
/* Helpers function to output information */
LFUNC(format) {
    lua_getglobal(L, "yafetch");

    lua_getfield(L, -1, "sep");
    const char *sep = lua_tostring(L, -1);

    lua_getglobal(L, "yafetch");

    lua_getfield(L, -1, "sep_color");
    const char *sep_color = lua_tostring(L, -1);
    lua_pop(L, 0);

    /* Get arguments from lua function */
    /* Icon */
    const char *col_icon = lua_tostring(L, 1);
    const char *icon     = lua_tostring(L, 2);

    /* Info */
    const char *col_info = lua_tostring(L, 3);
    const char *info     = lua_tostring(L, 4);

    printf("%7s%s%s%s%s%s%s%s%s\n", col_icon, icon, reset, sep_color, sep,
           reset, col_info, info, reset);
    return 1;
}

/* Register functions in lua */
void func_reg(void) {
#define REG(N)                     \
    lua_pushcfunction(L, lua_##N); \
    lua_setfield(L, -2, #N);
    lua_newtable(L);
    REG(shell)
    REG(user)
    REG(hostname)
    REG(distro)
    REG(kernel)
    REG(pkgs)
    REG(format)
    REG(header)
    lua_setglobal(L, "yafetch");

    luaL_newmetatable(L, "yafetch");
    lua_newtable(L);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
#undef REG
}
