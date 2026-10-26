/*
 * wish.c - the Wisconsin Shell (skeleton)
 *
 * Your job: fill in every TODO below to build a simple Unix shell.
 * Read the project README carefully before you start:
 *   https://github.com/remzi-arpacidusseau/ostep-projects/tree/master/processes-shell
 *
 * Compile:   gcc -Wall -Werror -o wish wish.c
 * Run:       ./wish              (interactive mode)
 *            ./wish batch.txt    (batch mode)
 *
 * This skeleton already compiles and runs. Much of the plumbing is done
 * for you: reading input, splitting lines into words, the fork/child/parent
 * structure, and waiting for children. Your work is in the TODOs:
 *   1. Batch mode: open the batch file and handle bad arguments (main).
 *   2. Finding programs in the search path (run_command).
 *   3. The built-ins: exit, cd, path (run_command).
 *   4. Redirection (>): parsing the filename and dup2 in the child.
 *   5. All the error cases. Test after every step!
 *
 * Right now the shell can read lines but cannot run anything, because
 * the path search (TODO 2) always fails. Start there.
 *
 * Useful man pages: getline, strsep, strchr, strcmp, strdup, access,
 *                   fork, execv, waitpid, chdir, open, dup2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_ARGS 100    // most words allowed in one command
#define MAX_PATHS 100   // most directories allowed in the search path
#define MAX_CMDS 100    // most parallel commands allowed on one line

// The shell's search path: a list of directories to look in for programs.
// Example: after "path /bin /usr/bin", paths = {"/bin", "/usr/bin"}, npaths = 2
char *paths[MAX_PATHS];
int npaths = 0;

/*
 * print_error: the ONLY error message the shell ever prints.
 * Call it whenever anything goes wrong. It writes to stderr, not stdout.
 */
void print_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

/*
 * run_command: runs ONE command (main has already split the line on '&').
 *
 * Example inputs:  "ls -la /tmp\n"   "echo hi > out.txt"   "   "
 *
 * Returns the child's pid if it started a new process,
 * otherwise returns 0 (built-in, empty command, or an error).
 */
pid_t run_command(char *cmd) {
    char *outfile = NULL;   // name of the redirect file, or NULL if no '>'

    /* ================= Redirection (>) ================= */
    char *redir = strchr(cmd, '>');   // pointer to the first '>', or NULL
    if (redir != NULL) {
        *redir = '\0';             // cut the string: cmd is now just the part before '>'
        char *rest = redir + 1;    // rest is the part after '>'

        /* TODO 4a: If rest contains another '>' (e.g. "ls > a > b"),
         *          call print_error() and return 0.
         */
        if(strchr(rest, '>') != NULL ) // strchr searches a string for a character
        {
            print_error();
            return 0;
        }

        // Go through the words after '>'. There must be exactly one.
        char *tok;
        int nfiles = 0;
        while ((tok = strsep(&rest, " \t\n")) != NULL) {
            if (*tok == '\0') continue;   // skip empty pieces from repeated spaces
            nfiles++;
            /* TODO 4b: If this is the first word (nfiles == 1),
             *          save it in outfile.
             */

            if(nfiles == 1)
            {
                outfile = tok;
            }
        }

        /* TODO 4c: If nfiles is not exactly 1 ("ls >" or "ls > a b"),
         *          call print_error() and return 0.
         */

        if(nfiles != 1)
        {
            print_error();
            return 0;
        }
    }

    /* ================= Split into words ================= */
    // After this loop, args = {"ls", "-la", "/tmp", NULL} and argc = 3.
    char *args[MAX_ARGS];
    int argc = 0;
    char *word;
    while ((word = strsep(&cmd, " \t\n")) != NULL) {
        if (*word == '\0') continue;               // skip empty pieces
        if (argc < MAX_ARGS - 1) args[argc++] = word;
    }
    args[argc] = NULL;   // execv needs the list to end with NULL

    // Blank command: nothing to do. But "> file" with no command is an error.
    if (argc == 0) {
        if (outfile != NULL) print_error();
        return 0;
    }

    /* ================= Built-in commands =================
     * Built-ins run inside the shell itself. Do NOT fork for them.
     */
    if (strcmp(args[0], "exit") == 0) {
        /* TODO 3a: exit
         *   - With no arguments (argc == 1): exit(0).
         *   - With any arguments: print_error(), and keep running.
         */
        if(argc == 1)
            exit(0);
        else
        {
            print_error();
            return 0;
        }
        
    }

    if (strcmp(args[0], "cd") == 0) {
        /* TODO 3b: cd
         *   - Must have exactly one argument (argc == 2), else print_error().
         *   - Call chdir(args[1]). It returns non-zero on failure:
         *     then print_error().
         */
        if(argc == 2)
        {
            if(chdir(args[1]) != 0)
                print_error();
        }
        else
            print_error();

        return 0;
        
    }

    if (strcmp(args[0], "path") == 0) {
        /* TODO 3c: path
         *   - First free() every old entry in paths[] and set npaths = 0.
         *   - Then copy each argument (args[1] .. args[argc-1]) into paths[]
         *     with strdup(). (The words in args point into the line buffer,
         *     which gets overwritten by the next line, so you must copy.)
         *   - "path" with no arguments leaves the path empty, after which
         *     only built-ins can run.
         */
        
        for(int i = 0; i < npaths; i++)
        {
            free(paths[i]);
            paths[i] = NULL;
        }
        npaths = 0;

        for(int i = 1; i < argc; i++)
        {
            paths[npaths] = strdup(args[i]);
            if(paths[npaths] == NULL)
            {
                print_error();
                break;
            }

            npaths++;
        }

        return 0;
    }

    /* ================= Find the program ================= */
    char fullpath[512];
    int found = 0;
    for (int i = 0; i < npaths; i++) {
        // Build "<dir>/<program>", e.g. "/bin" + "ls" -> "/bin/ls"
        snprintf(fullpath, sizeof(fullpath), "%s/%s", paths[i], args[0]);

        /* TODO 2: Check whether fullpath exists and is executable with
         *         access(fullpath, X_OK). It returns 0 if it is.
         *         If so, set found = 1 and break out of the loop.
         */

         if(access(fullpath, X_OK) == 0)
         {
            found = 1;
            break;
         }
    }
    if (!found) {
        print_error();   // not in any directory of the path
        return 0;
    }

    /* ================= Fork and run ================= */
    pid_t pid = fork();

    if (pid < 0) {
        // fork failed
        print_error();
        return 0;
    }

    if (pid == 0) {
        // ---------- CHILD process ----------
        if (outfile != NULL) {
            /* TODO 4d: Send the child's output to outfile.
             *   - int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
             *   - If fd < 0: print_error() and exit(1).
             *   - dup2(fd, STDOUT_FILENO);   // stdout now goes to the file
             *   - dup2(fd, STDERR_FILENO);   // stderr too
             *   - close(fd);                 // the original fd is no longer needed
             */
            int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

            if(fd < 0)
            {
                print_error();
                exit(1);
            }

            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }

        execv(fullpath, args);   // replaces the child with the program

        // execv only returns if it failed.
        // The child MUST exit here, or you will have two shells running!
        print_error();
        exit(1);
    }

    // ---------- PARENT process ----------
    // Don't wait here. Return the pid so main() can start every
    // parallel command before waiting on any of them.
    return pid;
}

int main(int argc, char *argv[]) {
    FILE *input = stdin;   // where commands come from
    int interactive = 1;   // 1 = print a prompt, 0 = batch mode

    /* ================= Command-line arguments ================= */
    if (argc > 2) {
        /* TODO 1a: More than one argument is an error:
         *          print_error() and exit(1).
         */
        print_error(); 
        exit(1);
    }
    if (argc == 2) {
        /* TODO 1b: Batch mode.
         *   - input = fopen(argv[1], "r");
         *   - If input is NULL (can't open the file): print_error() and exit(1).
         *   - Set interactive = 0 so no prompt is printed.
         */
        input = fopen(argv[1], "r");
        if(input == NULL)
        {
            print_error();
            exit(1);
        }

        interactive = 0;
    }

    // The search path starts out as just "/bin".
    // strdup so every entry can be freed the same way in "path".
    paths[npaths++] = strdup("/bin");

    /* ================= Main loop ================= */
    char *line = NULL;   // getline allocates (and grows) this buffer for us
    size_t len = 0;

    while (1) {
        if (interactive) {
            printf("wish> ");
            fflush(stdout);   // make the prompt appear right away
        }

        // Read one line. getline returns -1 at end of file (or Ctrl-D).
        if (getline(&line, &len, input) == -1) break;

        // Split the line on '&'. Each piece is one command; we start
        // them all first and wait afterwards, so they run in parallel.
        // A line with no '&' is just a single piece.
        pid_t pids[MAX_CMDS];
        int npids = 0;
        char *rest = line;
        char *cmd;
        while ((cmd = strsep(&rest, "&")) != NULL) {
            pid_t pid = run_command(cmd);
            if (pid > 0 && npids < MAX_CMDS) pids[npids++] = pid;
        }

        // Now wait for every child we started.
        for (int i = 0; i < npids; i++)
            waitpid(pids[i], NULL, 0);
    }

    free(line);
    if (input != stdin) fclose(input);
    exit(0);   // reaching end of input is a normal exit
}
