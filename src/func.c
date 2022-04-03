#include "yafetch.h"
#include <lauxlib.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <pwd.h>

#define LFUNC(N) int lua_##N(lua_State *L)

/* yafetch.user() */
/* Returns username */
LFUNC(user) {
    PDBD("Getting username");
    const struct passwd *pw = getpwuid(getuid());

    lua_pushstring(L, pw->pw_name ? pw->pw_name : "unknown");
    return 1;
}

/* yafetch.distro() */
/* Returns name of linux distribution */
LFUNC(distro) {
    PDBD("Getting distro");

    char *def = malloc(512);
    char *new = malloc(512);
    l_t line  = 0;

    l_t len;

    PDBD("Opening /etc/os-release");
    FILE *os_release = fopen("/etc/os-release", "rt");

    PDBD("Parsing /etc/os-release");
    while (fgets(def, 512, os_release)) {
        snprintf(new, 512, "%.*s", 511, def + 4);

        if (strncmp(new, "=", 1) == 0)
            break;

        line++;
    }

    PDBD("Closing /etc/os-release");
    fclose(os_release);

    PDBD("Freeing def");
    free(def);

    PDBD("Getting distro name");
    if (strncmp(new, "=", 1) == 0) {
        len = strlen(new);

        for (l_t ln = 0; ln < len; ln++) {
            if (new[ln] == '\"' || new[ln] == '\'' || new[ln] == '=') {
                for (l_t chr = 0; chr < len; chr++)
                    new[chr] = new[chr + 1];

                new[strlen(new) - 1] = '\0';
            }
        }
    }

    lua_pushstring(L, new ? new : "unknown");

    PDBD("Freeing new");
    free(new);

    return 1;
}

/* yafetch.hostname() */
/* Returns hostname of the machine */
LFUNC(hostname) {
    PDBD("Getting hostname");

    char hostname[HOST_NAME_MAX];

    PDBD("Running gethostname() to get hostname");
    gethostname(hostname, HOST_NAME_MAX);

    lua_pushstring(L, hostname);
    return 1;
}

/* yafetch.pkgs() */
/* Returns number of installed packages */
LFUNC(pkgs) {
    PDBD("Getting packages");
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

    PDBD("Getting package_manager_count");
    const pkg_t package_manager_count =
        sizeof package_managers / sizeof package_managers[0];

    pkgc_t total      = 0;
    unsigned char run = 1;

    for (pkg_t pkg = 0; pkg < package_manager_count; ++pkg) {
        PDBD("New loop");

        if (!run) {
            PDBD("Running of the package manager loop disabled");
            break;
        }

        char full_cmd[70] = {0};
        strcpy(full_cmd, package_managers[pkg]);
        strcat(full_cmd, PKG_END);

        PDBD("Opening a pipe for package manager...");
        FILE *packages = popen(full_cmd, "r");

        if (packages) {
            pkgc_t tmp = 0;

            PDBD("Found packages, running fscanf()");
            if (fscanf(packages, "%lu", &tmp) != 1) {
                error("Failed to fscanf() for packages\n");
                run = 0;
            }

            total += tmp;
        }

        PDBD("Closing pipe");
        pclose(packages);
    }

    lua_pushinteger(L, total);
    return 1;
}

/* yafetch.kernel() */
/* Returns kernel version */
LFUNC(kernel) {
    PDBD("Getting kernel");
    struct utsname sys;

    PDBD("getting uname() for kernel");

    if (uname(&sys) == 0)
        lua_pushstring(L, sys.release);
    else
        lua_pushstring(L, "unknown");

    return 1;
}

/* yafetch.shell() */
/* Returns path of shell */
LFUNC(shell) {
    PDBD("Getting shell");

    lua_getglobal(L, "yafetch");
    lua_getfield(L, -1, "shell_base");

    const int shell_full = lua_toboolean(L, -1);

    PDBD("Running getpwuid(getuid())");
    struct passwd *pw = getpwuid(getuid());

    PDBD("Getting basename by last /");

    /* Get basename of shell by looking for last '/' */
    char *slash = strrchr(pw->pw_shell, '/');

    if (pw->pw_shell) {
        PDBD("Parsing shell name");

        if (shell_full == 1)
            pw->pw_shell = slash + 1;

        lua_pushstring(L, pw->pw_shell);
    } else {
        PDBD("Unknown shell found");
        lua_pushstring(L, "unknown");
    }

    return 1;
}

/* yafetch.header() */
LFUNC(header) {
    PDBD("Getting header");

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

    if (lua_isnil(L, -1) == 1 || lua_isnone(L, -1) == 1)
        fmt = "";

    /* Get arguments from lua function */

    /* Header colours */
    const char *h1_col = "\033[0m";
    const char *h2_col = "\033[0m";

    /* Get hostname */
    char hostname[255];

    PDBD("Getting hostname in getting header");
    gethostname(hostname, 255);

    /* Username */
    PDBD("Running getpwuid(getuid())");
    const struct passwd *pw = getpwuid(getuid());

    PDBD("Formatting header using prinf()");
    printf("%s%s%s%s%s%s%s%s%s%s\n", h1_col, fmt,
           pw->pw_name ? pw->pw_name : "unknown", RESET, sep_color, sep, RESET,
           h2_col, hostname, RESET);

    return 1;
}

/* yafetch.format() */
/* Formats given strings. */
/* Helpers function to output information */
LFUNC(format) {
    PDBD("Getting format");

    lua_getglobal(L, "yafetch");

    lua_getfield(L, -1, "sep");
    const char *sep = lua_tostring(L, -1);

    lua_getglobal(L, "yafetch");

    lua_getfield(L, -1, "sep_color");
    const char *sep_color = lua_tostring(L, -1);
    lua_pop(L, 0);

    /* Get arguments from lua function */
    /* Icon */
    PDBD("Getting icons");
    const char *col_icon = lua_tostring(L, 1);
    const char *icon     = lua_tostring(L, 2);

    /* Info */
    PDBD("Getting info");
    const char *col_info = lua_tostring(L, 3);
    const char *info     = lua_tostring(L, 4);

    PDBD("Lua prep finished, formatting");
    printf("%7s%s%s%s%s%s%s%s%s\n", col_icon, icon, RESET, sep_color, sep,
           RESET, col_info, info, RESET);
    return 1;
}

/* Register functions in lua */
void func_reg(void) {
    PDBD("Registering lua functions");

#define REG(N)                     \
    PDBD("New function: " #N);     \
    lua_pushcfunction(L, lua_##N); \
    lua_setfield(L, -2, #N);

    PDBD("Making new lua table");
    lua_newtable(L);

    REG(shell)
    REG(user)
    REG(hostname)
    REG(distro)
    REG(kernel)
    REG(pkgs)
    REG(format)
    REG(header)

    PDBD("Initialising lua tables");

    lua_setglobal(L, "yafetch");
    luaL_newmetatable(L, "yafetch");
    lua_newtable(L);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

#undef REG
}
