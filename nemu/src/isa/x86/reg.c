#include <isa.h>
#include <stdlib.h>
#include <time.h>
#include "local-include/reg.h"

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test()
{
    srand(time(0));
    word_t sample[8];
    word_t pc_sample = rand();
    cpu.pc = pc_sample;
    int i;
    for(i = R_EAX; i <= R_EDI; i ++)
    {
        sample[i] = rand();
        reg_l(i) = sample[i];
        assert(reg_w(i) == (sample[i] & 0xffff));
    }
    assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
    assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
    assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
    assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
    assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
    assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
    assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
    assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));
    assert(sample[R_EAX] == cpu.eax);
    assert(sample[R_ECX] == cpu.ecx);
    assert(sample[R_EDX] == cpu.edx);
    assert(sample[R_EBX] == cpu.ebx);
    assert(sample[R_ESP] == cpu.esp);
    assert(sample[R_EBP] == cpu.ebp);
    assert(sample[R_ESI] == cpu.esi);
    assert(sample[R_EDI] == cpu.edi);
    assert(pc_sample == cpu.pc);
}

void isa_reg_display()
{
    int i;
    for(i = R_EAX; i <= R_EDI; i ++)
    {
        printf("%s:  0x%08x  %u\n", regsl[i], cpu.gpr[i]._32, cpu.gpr[i]._32);
    }
    printf("pc :  0x%08x \n", cpu.pc);
}

word_t isa_reg_str2val(const char *s, bool *success)
{
    int i;
    printf("strlen is %lx\n", strlen(s));
    if(strlen(s) == 4)
    {
        for(i = R_EAX; i <= R_EDI; i++)
        {
            if(strcmp(&s[1], regsl[i]) == 0)
            {
                *success = true;
                return  reg_l(i);
            }
        }
    }
    else
        if(strlen(s) == 3)
        {
            for(i = R_AX; i <= R_DI; i++)
            {
                if(strcmp(&s[1], regsw[i]) == 0)
                {
                    *success = true;
                    return  reg_w(i);
                }
            }
            for(i = R_AL; i <= R_BH; i++)
            {
                if(strcmp(&s[1], regsb[i]) == 0)
                {
                    *success = true;
                    return  reg_b(i);
                }
            }
        }
    *success = false;
    return 0;
}
