#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <common.h>
enum {
  TK_NOTYPE = 256, TK_EQ,TK_DEC,TK_HEX,

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"\\-", '-'},  
  {"\\*", '*'}, 
  {"\\/", '/'}, 
  {"\\(",'('},
  {"\\)",')'},
  {"0[xX][0-9a-fA-f]+", TK_HEX},
  {"[0-9]+", TK_DEC},
  
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          //default: TODO();
            case TK_NOTYPE:
                break;
          
          //care about the content
            case TK_HEX: 
            case TK_DEC:
                //memset(tokens[nr_token].str,'\0',32);
                Assert((substr_len) > 0 && (substr_len<32),"len of token is %d\n",substr_len);
                strncpy(tokens[nr_token].str,substr_start,substr_len);//fix error
                tokens[nr_token].str[substr_len]='\0';
                //printf ("%s\n", tokens[nr_token].str);
            default:
                tokens[nr_token].type = rules[i].token_type;
                nr_token++;
        }
        
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

int check_parentheses(int p, int q, bool *success)
{
    int i;
    int match=0;
    if ((tokens[p].type == '(') && (tokens[q].type == ')'))
    {
        assert (p<q);
        for (i=p+1; i<q; i++)
        {
            if (tokens[i].type == '(')
            {
                match ++;
            }
            else if (tokens[i].type == ')')
            {
                if (match > 0)
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

        if (match == 0)
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

int eval(int p, int q, bool *success)
{DEBUG();
  int ret;
  if (p > q) {
    assert (0);
  }
  else if (p == q) {DEBUG();
      if (tokens[p].type == TK_DEC)
      {DEBUG();
          sscanf (tokens[p].str, "%d", &ret);
          printf ("%d\n", ret);
          return ret;
      }
      else if (tokens[p].type == TK_HEX)
      {DEBUG();
          sscanf (tokens[p].str+2, "%x", &ret);
          printf ("%d\n", ret);
          return ret;
      }
      else
      {DEBUG();
          *success = false;
          return 0;
      }
  }
  else if (check_parentheses(p, q, success) == 1) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */DEBUG();
    return eval(p + 1, q - 1, success);
  }
  else {
    /* We should do more things here. */DEBUG();

  }
    DEBUG();
  return 0;
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    DEBUG();
    return 0;
  }


  return eval(0, nr_token-1, success);
}
