// holoshell.c - simple terminal shell prototype with Doom WAD detection
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>

static void clear_screen(void) {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

static int file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

static void list_dir(const char *path) {
    DIR *d = opendir(path);
    if (!d) { perror("opendir"); return; }
    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        printf("%s  ", e->d_name);
    }
    printf("\n");
    closedir(d);
}

static void run_doom(const char *wad) {
    if (!file_exists(wad)) {
        printf("WAD not found: %s\n", wad);
        return;
    }
    // Try Chocolate Doom first
    if (system("chocolate-doom --version >nul 2>&1") == 0 || system("chocolate-doom --version >/dev/null 2>&1") == 0) {
        char cmd[1024];
        snprintf(cmd, sizeof(cmd), "chocolate-doom -iwad %s", wad);
        printf("Launching Chocolate Doom: %s\n", cmd);
        system(cmd);
        return;
    }
    // Try older doom executable name
    if (system("doom --version >nul 2>&1") == 0 || system("doom --version >/dev/null 2>&1") == 0) {
        char cmd[1024];
        snprintf(cmd, sizeof(cmd), "doom -iwad %s", wad);
        printf("Launching Doom: %s\n", cmd);
        system(cmd);
        return;
    }
    printf("No Doom engine found in PATH. Install Chocolate Doom or a compatible port and retry.\n");
}

int main(int argc, char **argv) {
    char line[256];
    const char *cwd = ".";
    printf("HoloShell v0.1 - user-space prototype\nType 'help' for commands.\n");
    while (1) {
        printf("holoshell> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        // strip newline
        size_t L = strlen(line);
        if (L && line[L-1] == '\n') line[L-1] = '\0';
        if (strcmp(line, "help") == 0) {
            printf("commands: help clear doom [path] ls echo [text] pwd whoami uname cat reboot shutdown about exit\n");
        } else if (strcmp(line, "clear") == 0) {
            clear_screen();
        } else if (strncmp(line, "doom", 4) == 0) {
            const char *arg = line + 4;
            while (*arg == ' ') ++arg;
            if (*arg == '\0') arg = "DOOM.WAD";
            run_doom(arg);
        } else if (strcmp(line, "about") == 0) {
            printf("HoloKernel user-shell prototype. Detects IWAD files and can launch Chocolate Doom if installed.\n");
        } else if (strcmp(line, "ls") == 0 || strncmp(line, "ls ", 3) == 0) {
            list_dir(cwd);
        } else if (strncmp(line, "echo ", 5) == 0) {
            printf("%s\n", line + 5);
        } else if (strcmp(line, "echo") == 0) {
            printf("\n");
        } else if (strcmp(line, "pwd") == 0) {
            printf("/root\n");
        } else if (strcmp(line, "whoami") == 0) {
            printf("root\n");
        } else if (strcmp(line, "uname") == 0) {
            printf("HoloOS (x86_32)\n");
        } else if (strncmp(line, "cat ", 4) == 0) {
            printf("cat: %s: No filesystem loaded\n", line + 4);
        } else if (strcmp(line, "cat") == 0) {
            printf("Usage: cat <file>\n");
        } else if (strcmp(line, "reboot") == 0 || strcmp(line, "shutdown") == 0) {
            printf("System would reboot/shutdown here.\n");
            break;
        } else if (strcmp(line, "exit") == 0) {
            break;
        } else if (strcmp(line, "") == 0) {
            continue;
        } else {
            printf("unknown command: %s\n", line);
        }
    }
    printf("Goodbye.\n");
    return 0;
}
