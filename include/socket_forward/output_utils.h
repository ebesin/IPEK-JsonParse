#include <stdio.h>
#include <stdarg.h>
// 函数声明
void printDec(int dec);
void printHex(int hex);
void printOct(unsigned oct);
void printNum(unsigned long num, int base);
void printStr(char *str);
void printFloat(double f);
void printAddr(unsigned long);

void printNum(unsigned long num, int base)
{
    // 递归结束条件
    if (num == 0)
    {
        return;
    }

    // 继续递归
    printNum(num / base, base);
    // 逆序打印结果
    putchar("0123456789abcdef"[num % base]);
}

// 打印十进制数
void printDec(int dec)
{
    // 处理负数的情况
    if (dec < 0)
    {
        putchar('-');
        dec = -dec;
    }

    if (dec == 0)
    {
        putchar('0');
        return;
    }

    printNum(dec, 10);
}

// 打印十六进制数
void printHex(int hex)
{
    if (hex == 0)
    {
        putchar('0');
        return;
    }

    printNum(hex, 16);
}

// 打印八进制数
void printOct(unsigned oct)
{
    if (oct == 0)
    {
        putchar('0');
        return;
    }

    printNum(oct, 8);
}

// 打印字符串
void printStr(char *str)
{
    char *p = str;
    while (*p)
    {
        putchar(*p++);
    }
}

// 打印浮点数
void printFloat(double f)
{
    int tmp = (int)f;
    // 先打印整数部分
    printNum(tmp, 10);
    // 小数点
    putchar('.');

    // 再打印小数部分
    f -= tmp;
    if (f == 0)
    {
        int i;
        // 浮点型精度至少6位
        for (i = 0; i < 6; i++)
        {
            putchar('0');
        }
        return;
    }

    tmp = (int)(f * 1000000);
    printNum(tmp, 10);
}

void printAddr(unsigned long addr)
{
    printStr("0x");

    // 内存地址是用十六进制表示的
    printNum(addr, 16);
}
