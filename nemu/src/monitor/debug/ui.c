#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char *rl_gets()
{
    static char *line_read = NULL;
    if(line_read)
    {
        free(line_read);
        line_read = NULL;
    }
    line_read = readline("(nemu) ");
    if(line_read && *line_read)
    {
        add_history(line_read);
    }
    return line_read;
}

static int cmd_c(char *args)
{
    cpu_exec(-1);
    return 0;
}


static int cmd_q(char *args)
{
    return -1;
}

static int cmd_help(char *args);

static int cmd_howif(char *args)
{
    bool ret;
    expr(args, &ret);
    printf("test for howif\n");
    return 0;
}

static int cmd_si(char *args)
{
    char *arg = strtok(NULL, " ");
    int num;
    if(arg == NULL)
    {
        cpu_exec(1);
    }
    else
    {
        sscanf(arg, "%d", &num);
        if(num > 0 && num < 256)
        {
            cpu_exec(num);
        }
        else
        {
            cpu_exec(1);
        }
    }
    return 0;
}

static int cmd_info(char *args)
{
    char *arg = strtok(NULL, " ");
    if(arg == NULL)
    {
        printf("put 'info r' or 'info n'\n");
        return 0;
    }
    if(strcmp(arg, "r") == 0)
    {
        isa_reg_display();
    }
    else
        if(strcmp(arg, "w") == 0)
        {
            // TODO: watch point display
        }
        else
        {
            printf("put 'info r' or 'info n'\n");
        }
    return 0;
}

static struct
{
    char *name;
    char *description;
    int (*handler)(char *);
} cmd_table [] =
{
    { "help", "Display informations about all supported commands", cmd_help },
    { "c", "Continue the execution of the program", cmd_c },
    { "q", "Exit NEMU", cmd_q },

    /* TODO: Add more commands */
    {"howif", "test for howif", cmd_howif},
    {"si", "execute N (0<N<256)instructions, N=1 by default", cmd_si},
    {"info", "info r : info about registers\n info w : info about watch point\n", cmd_info},

};



#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args)
{
    /* extract the first argument */
    char *arg = strtok(NULL, " ");
    int i;
    if(arg == NULL)
    {
        /* no argument given */
        for(i = 0; i < NR_CMD; i ++)
        {
            printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        }
    }
    else
    {
        for(i = 0; i < NR_CMD; i ++)
        {
            if(strcmp(arg, cmd_table[i].name) == 0)
            {
                printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
                return 0;
            }
        }
        printf("Unknown command '%s'\n", arg);
    }
    return 0;
}

void ui_mainloop()
{
    if(is_batch_mode())
    {
        cmd_c(NULL);
        return;
    }
    for(char *str; (str = rl_gets()) != NULL;)
    {
        char *str_end = str + strlen(str);
        /* extract the first token as the command */
        char *cmd = strtok(str, " ");
        if(cmd == NULL) { continue; }
        /* treat the remaining string as the arguments,
         * which may need further parsing
         */
        char *args = cmd + strlen(cmd) + 1;
        if(args >= str_end)
        {
            args = NULL;
        }
        #ifdef HAS_IOE
        extern void sdl_clear_event_queue();
        sdl_clear_event_queue();
        #endif
        int i;
        for(i = 0; i < NR_CMD; i ++)
        {
            if(strcmp(cmd, cmd_table[i].name) == 0)
            {
                if(cmd_table[i].handler(args) < 0) { return; }
                break;
            }
        }
        if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
    }
}
