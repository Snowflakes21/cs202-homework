#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_SIZE 1024
#define TOK_SIZE 64
#define NEW_LINE " \t\r\n\a"

void shell_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = read_line();
    args = split_line(line);
    status = execute(args);

    free(line);
    free(args);
  } while (status);
}


char *read_line(void)
{
  char *line = NULL;
  ssize_t size = 0; 
  getline(&line, &size, stdin);
  return line;
}

char **split_line(char *line)
{
  int size = TOK_SIZE, i = 0;
  char **tokens = malloc(size * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "Allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, NEW_LINE);
  while (token != NULL) {
    tokens[i] = token;
    i++;

    if (i >= size) {
      size += TOK_SIZE;
      tokens = realloc(tokens, size * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, NEW_LINE);
  }
  tokens[i] = NULL;
  return tokens;
}

int hello(char **args);
int goodbye(char **args);
int lab_name(char **args);
int author(char **args)
/*
  内置命令列表，以及它们对应的函数。
 */
char *builtin_str[] = {
  "hello",
  "goodbye",
  "lab_name",
  "author"
};

int (*builtin_func[]) (char **) = {
  &hello,
  &goodbye,
  &lab_name,
  &author
};

int num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int hello(char **args)
{
  printf("Hello!");
  return 1;
}

int goodbye(char **args)
{
  return 0;
}

int lab_name(char **args)
{
  printf("202 lab #1.\n");
  return 1;
}

int author(char **args)
{
  printf("Claire Sun - ys3493\n");
  return 1;
}


int launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("Fail");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("Fail");
  } else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    return 1;
  }

  for (i = 0; i < num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return launch(args);
}

int main(int argc, char **argv)
{
  shell_loop();
  return EXIT_SUCCESS;
}