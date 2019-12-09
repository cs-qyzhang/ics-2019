#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;
  static char *last_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
    if (last_read) free(last_read);
    last_read = (char *)malloc(strlen(line_read) + 1);
    strcpy(last_read, line_read);
    return line_read;
  } else {
    return last_read;
  }
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_p(char *args) {
  if (args == NULL) {
    printf("Print command needs an expression!\n");
    return 0;
  }
  bool success;
  struct node *val = expr(args, &success, NULL);
  if (!success) {
    Log("WRONG EXPR!");
    return 0;
  }
  print_node(val);
  putchar('\n');
  return 0;
}

static int cmd_info(char *args) {
  if (args[0] == 'r' && args[1] == '\0') {
    isa_reg_display(true);
  } else if (args[0] == 'w' && args[1] == '\0') {
    wp_display();
  } else {
    Log("info's subcommand %s non-exist!", args);
  }
  return 0;
}

static int cmd_w(char *args) {
  if (args == NULL) {
    Log("w command has no expression!");
  } else {
    add_wp(strdup(args));
  }
  return 0;
}

static int cmd_d(char *args) {
  if (args == NULL) {
    Log("d command should have a watchpoint NO!");
  } else {
    char *p = args;
    while (*p != '\0') {
      if (*p < '0' || *p > '9') {
        Log("d command should have a watchpoint NO!");
      }
      p++;
    }
    int NO = atoi(args);
    del_wp(NO);
  }
  return 0;
}

static int cmd_s(char *args) {
  if (args != NULL) {
    char *p = args;
    while (*p != '\0') {
      if (*p < '0' || *p > '9') {
        Log("s command's param is not a demical!");
      }
      p++;
    }
    cpu_exec(atoi(args));
  } else {
    cpu_exec(1);
  }
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "p", "Print expression value", cmd_p },
  { "info", "Print informations", cmd_info },
  { "w", "Add watchpoint", cmd_w },
  { "d", "Delete watchpoint", cmd_d },
  { "s", "Single step", cmd_s },

  /* TODO: Add more commands */

};

const int NR_CMD = sizeof(cmd_table) / sizeof(cmd_table[0]);

static int cmd_help(char *args) {
  int i;

  if (args == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(args, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", args);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = str + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;  // 注意这里的args字符串后面会改变！
    } else {
      // args = strtok(NULL, " ");
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
