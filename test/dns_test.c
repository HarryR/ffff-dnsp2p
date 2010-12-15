#include <stdio.h>

#include "test_framework.h"
#include "dns.h"

int count_char_test(void)
{
    const char *str = "abcabcabc1234554321";
    int out = 0;
    out += assertTrue((count_char(str, 'a') == 3), "Character count incorrect for char 'a'");
    out += assertTrue((count_char(str, '1') == 2), "Character count incorrect for char '1'");
    out += assertTrue((count_char(str, '5') == 2), "Character count incorrect for char '5'");
    return out;
}

int main(void)
{
    int out = 0;
    out += count_char_test();
    return 0;
}
