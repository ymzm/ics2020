#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

//#define NR_TOKEN 50

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
                "#include <stdio.h>\n"
                "int main() { "
                "  unsigned result = %s; "
                "  printf(\"%%u\", result); "
                "  return 0; "
                "}";

int sum = 0;  //buf全局索引

static inline void gen(char x)
{
    buf[sum] = x;
    sum++;
}


#define SPACE  do{if(rand()%2==0) gen(' ');}while(0)

static inline int choose(int x)
{
    int result = rand() % x;
    return result;
}

static inline void gen_num()
{
    //生成两位数
    int num, num1, num2;
    switch(choose(2))
    {
        case 0:
            num = choose(10);
            gen(num + '0');
            if(num == 0 && sum >= 1 && buf[sum - 1] == '/')
            {
                sum--;
                num1 = 1 + choose(9);
                gen(num1 + '0');
            }
            break;
        case 1:
            num1 = 1 + choose(9);
            gen(num1 + '0');
            num2 = choose(10);
            gen(num2 + '0');
            break;
    }
}


static inline void  gen_rand_op()
{
    switch(choose(4))
    {
        case 0:
            gen('+');
            break;
        case 1:
            gen('-');
            break;
        case 2:
            gen('*');
            break;
        case 3:
            gen('/');
            //if(buf[sum]!='\0' && buf[sum]=='0')
            //  buf[sum] = '1';
            break;
    }
}

int count = 0;
int false = 0;
static inline void gen_rand_expr()
{
    if(count > 32)
    {
        false = 1;
        return;
    }
    //int ret;
    switch(choose(3))
    {
        case 0:
            gen_num();
            count ++;
            break;
        case 1:
            //if(sum+10 > NR_TOKEN) return ;  //控制生成的长度
            gen('(');
            count ++;
            SPACE;
            gen_rand_expr();
            SPACE;
            gen(')');
            count ++;
            break;
        default:
            //if(sum+10 > NR_TOKEN) return ;
            gen_rand_expr();
            SPACE;
            gen_rand_op();
            count ++;
            SPACE;
            gen_rand_expr();
            break;
    }
}

int main(int argc, char *argv[])
{
    int seed = time(0);
    srand(seed);
    int loop = 10;
    if(argc > 1)
    {
        sscanf(argv[1], "%d", &loop);
    }
    int i;
    for(i = 0; i < loop; i ++)
    {
        count = 0;
        false = 0;
        sum = 0;
        gen_rand_expr();
        if(false == 1 || count > 32)
        {
            continue;
        }
        buf[sum] = '\0';
        sprintf(code_buf, code_format, buf); //发送格式化输出到code_buf
        FILE *fp = fopen("/tmp/.code.c", "w");
        assert(fp != NULL);
        fputs(code_buf, fp); //code_buf ->fp
        fclose(fp);
        int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
        if(ret != 0) { continue; }
        //popen()会调用fork()产生子进程，然后从子进程中调用/bin/sh -c 来执行参数command 的指令
        fp = popen("/tmp/.expr", "r");
        assert(fp != NULL);
        int result;
        if(fscanf(fp, "%d", &result)); //从fp读取格式化输入
        pclose(fp);
        printf("%u %s\n", result, buf);
        //usleep(1000);
    }
    seed = system("rm -rf /tmp/.code.c /tmp/.expr");
    return 0;
}
