#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <common.h>
#include <memory/vaddr.h>
enum
{
    TK_NOTYPE = 256, TK_EQ, TK_DEC, TK_HEX, TK_AND, TK_ORR, TK_NEQ, TK_NOT, TK_LE, TK_GE, TK_DEREF, TK_MINUS, TK_REGSL, TK_REGSW, TK_REGSB,
};

static struct rule
{
    char *regex;
    int token_type;
} rules[] =
{
    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */
    {" +", TK_NOTYPE},    // spaces
    {"\\+", '+'},         // plus
    {"==", TK_EQ},        // equal
    {"\\-", '-'},
    {"\\*", '*'},
    {"\\/", '/'},
    {"\\(", '('},
    {"\\)", ')'},
    {"0[xX][0-9a-fA-f]+", TK_HEX},
    {"[0-9]+", TK_DEC},
    {"&&", TK_AND},
    {"\\|\\|", TK_ORR},
    {"!=", TK_NEQ},
    {"!", TK_NOT},
    {"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi)", TK_REGSL},
    {"\\$(ax|cx|dx|bx|sp|bp|si|di)", TK_REGSW},
    {"\\$(al|cl|dl|bl|ah|ch|dh|bh)", TK_REGSB},
    {"<=", TK_LE},
    {">=", TK_GE},
    {"<", '<'},
    {">", '>'},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
    int i;
    char error_msg[128];
    int ret;
    for(i = 0; i < NR_REGEX; i ++)
    {
        ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
        if(ret != 0)
        {
            regerror(ret, &re[i], error_msg, 128);
            panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
        }
    }
}

typedef struct token
{
    int type;
    char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e)
{
    int position = 0;
    int i;
    regmatch_t pmatch;
    nr_token = 0;
    while(e[position] != '\0')
    {
        /* Try all rules one by one. */
        for(i = 0; i < NR_REGEX; i ++)
        {
            if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
            {
                char *substr_start = e + position;
                int substr_len = pmatch.rm_eo;
                Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
                    i, rules[i].regex, position, substr_len, substr_len, substr_start);
                position += substr_len;
                /* TODO: Now a new token is recognized with rules[i]. Add codes
                 * to record the token in the array `tokens'. For certain types
                 * of tokens, some extra actions should be performed.
                 */
                switch(rules[i].token_type)
                {
                    //default: TODO();
                    case TK_NOTYPE:
                        break;
                    case '*':
                    case '-':
                        if(nr_token == 0 || tokens[nr_token - 1].type == '(' || !(tokens[nr_token - 1].type == TK_DEC || tokens[nr_token - 1].type == TK_HEX || tokens[nr_token - 1].type == TK_REGSB  || tokens[nr_token - 1].type == TK_REGSL  || tokens[nr_token - 1].type == TK_REGSW))
                        {
                            if(rules[i].token_type == '*')
                            {
                                tokens[nr_token].type = TK_DEREF;
                            }
                            else
                            {
                                tokens[nr_token].type = TK_MINUS;
                            }
                            nr_token++;
                        }
                        else
                        {
                            tokens[nr_token].type = rules[i].token_type;
                            nr_token++;
                        }
                        break;
                    //care about the content
                    case TK_HEX:
                    case TK_DEC:
                    case TK_REGSL:
                    case TK_REGSW:
                    case TK_REGSB:
                        //memset(tokens[nr_token].str,'\0',32);
                        Assert((substr_len) > 0 && (substr_len < 32), "len of token is %d\n", substr_len);
                        strncpy(tokens[nr_token].str, substr_start, substr_len); //fix error
                        tokens[nr_token].str[substr_len] = '\0';
                    //printf ("%s\n", tokens[nr_token].str);
                    default:
                        tokens[nr_token].type = rules[i].token_type;
                        nr_token++;
                }
                break;
            }
        }
        if(i == NR_REGEX)
        {
            printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
            return false;
        }
    }
    return true;
}

int check_parentheses(int p, int q, bool *success)
{
    int i;
    int match = 0;
    if((tokens[p].type == '(') && (tokens[q].type == ')'))
    {
        assert(p < q);
        for(i = p + 1; i < q; i++)
        {
            if(tokens[i].type == '(')
            {
                match ++;
            }
            else
                if(tokens[i].type == ')')
                {
                    if(match > 0)
                    {
                        match --;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                    ;
                }
        }
        if(match == 0)
        {
            return 1;
        }
        else
        {
            // illegal
            *success = false;
            return 0;
        }
    }
    return 0;
}

static u_int32_t opt_pri(int x)
{
    switch(x)
    {
        case '(':
        case ')':
            return 1;
        case TK_NOT:
        case TK_DEREF:
        case TK_MINUS:
            return 3;
        case '*':
        case '/':
            return 4;
        case '+':
        case '-':
            return 5;
        case '>':
        case '<':
        case TK_LE:
        case TK_GE:
            return 6;
        case TK_NEQ:
        case TK_EQ:
            return 6;
        case TK_AND:
            return 7;
        case TK_ORR:
            return 8;
        default:
            return -1;
    }
}

static bool is_opt(u_int32_t x)
{
    switch(x)
    {
        case TK_HEX:
        case TK_DEC:
        case TK_REGSL:
        case TK_REGSW:
        case TK_REGSB:
            return false;
        default:
            return true;
    }
}

static u_int32_t get_main_op(int p, int q)  //bug
{
    int result = p;
    int pos = p;
    int sum = 0;
    int curPriority = 0;
    for(; pos <= q; pos++)
    {
        int tp = tokens[pos].type;
        //注意if的顺序，如果先判断sum，那么当前位为括号则会跳过
        if(tp == '(')
        {
            sum++;
            continue;
        }
        if(tp == ')')
        {
            sum--;
            continue;
        }
        if(!is_opt(tp))
        {
            continue;
        }
        if(sum > 0)
        {
            continue;
        }
        else
        {
            assert(sum == 0);
            // 优先级数值越高 实际优先级越低 越最后计算 所以最后计算的一个最低优先级运算符为主运算符
            if(opt_pri(tp) >= curPriority)
            {
                curPriority = opt_pri(tp);
                result = pos;
            }
            else
            { continue; }
        }
    }
    printf("main opt @ position %d,while p = %d q = %d\n", result, p, q);
    return result;
}

int eval(int p, int q, bool *success)
{
    DEBUGP();
    int ret;
    int pos;
    if(p > q)
    {
        assert(0);
    }
    else
        if(p == q)
        {
            DEBUGP();
            if(tokens[p].type == TK_DEC)
            {
                DEBUGP();
                sscanf(tokens[p].str, "%d", &ret);
                printf("%d\n", ret);
                return ret;
            }
            else
                if(tokens[p].type == TK_HEX)
                {
                    DEBUGP();
                    sscanf(tokens[p].str + 2, "%x", &ret);
                    printf("%d\n", ret);
                    return ret;
                }
                else
                    if(tokens[p].type == TK_REGSL || tokens[p].type == TK_REGSW || tokens[p].type == TK_REGSB)
                    {
                        bool flag = false;
                        ret = isa_reg_str2val(tokens[p].str, &flag);
                        assert(flag);
                        return ret;
                    }
                    else
                    {
                        DEBUGP();
                        *success = false;
                        return 0;
                    }
        }
        else
            if(check_parentheses(p, q, success) == 1)
            {
                /* The expression is surrounded by a matched pair of parentheses.
                 * If that is the case, just throw away the parentheses.
                 */DEBUGP();
                if(q - 1 > p)
                {
                    return eval(p + 1, q - 1, success);
                }
                else
                {
                    assert(q - 1 == p);
                    *success = false;
                    return -1;
                }
            }
            else
            {
                /* We should do more things here. */DEBUGP();
                pos = get_main_op(p, q);
                if(tokens[pos].type == TK_MINUS || tokens[pos].type == TK_DEREF)
                {
                    DEBUGP();
                    u_int32_t val = eval(p + 1, q, success);
                    switch(tokens[pos].type)
                    {
                        case TK_MINUS:
                            return -val;
                        case TK_DEREF:
                            return vaddr_read(val, 4);
                        default:
                            return -1;
                            break;
                    }
                }
                if(pos == p || pos == q)
                {
                    *success = false;
                    return -1;
                }
                u_int32_t val1 = eval(p, pos - 1, success);
                u_int32_t val2 = eval(pos + 1, q, success);
                if(*success == false)
                {
                    return -1;
                }
                switch(tokens[pos].type)
                {
                    case '+':
                        return val1 + val2;
                        DEBUGP();
                    case '-':
                        return val1 - val2;
                    case '*':
                        return val1 * val2;
                    case '/':
                        if(val2 == 0)
                        {
                            //Assert(0, "Zero div is wrong!\n");
                            printf("Divide by zero, please check!\n");
                            *success = false;
                            return -1;
                        }
                        return val1 / val2;
                    default:
                        *success = false;
                }
                DEBUGP();
                return -1;
            }
}

word_t expr(char *e, bool *success)
{
    *success = true;
    if(!make_token(e))
    {
        *success = false;
        DEBUGP();
        return 0;
    }
    //int ret =
    //printf("ret is %d\n", ret);
    return eval(0, nr_token - 1, success);
    //return eval(0, nr_token-1, success);
}
