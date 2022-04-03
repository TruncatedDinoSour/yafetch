#include "yafetch.h"

int main(int argc, char **argv) {
    PDBD("Running program");

    /* Create a lua vm */
    PDBD("Creating lua vm");
    script_init();

    /* Register lua functions */
    PDBD("Registering lua functions");
    func_reg();

    /* Load configuration file */
    PDBD("Loading config");

    if (argc != 2) {
        const char *config_file = config_location();

        if (access(config_file, F_OK) != -1)
            script_run(config_file);
        else
            fprintf(stderr, "%sYafetch%s: %s doesn't exist!\n", RED, RESET,
                    config_file);
    } else
        script_run(argv[1]);

    /* Destroy lua vm */
    PDBD("Destroying lua vm");
    script_destroy();

    return 0;
}
