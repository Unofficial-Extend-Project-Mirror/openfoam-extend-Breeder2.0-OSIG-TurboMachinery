#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../fortran_macros.h"

static int check_args(int *arg1, char *arg2, int *arg3, int len)
{
    int n, err = 0;

    /* arg1 should be 1 */

    if (*arg1 != 1) {
        printf ("first argument is %d and should be 1\n", *arg1);
        err++;
    }

    /* arg2 should be "string" */

    if (strncmp (arg2, "string", 6)) {
        printf ("second argument is ");
        for (n = 0; n < 6; n++) {
            if (isascii(arg2[n]) && isprint(arg2[n]))
                putchar (arg2[n]);
            else
                printf ("0x%2.2X", (unsigned)arg2[n]);
        }
        printf (" and should be string\n");
        err++;
    }

    /* the implicit string length should be 32 */

    if (len != 32) {
        printf ("the implicit string length is %d and should be 32\n", len);
        err++;
    }

    /* arg3 should be 3
       this may cause a segmentation violation if
       the argument passing is incorrect */

    if (err) {
        printf ("the following test may cause a segmentation violation\n");
        fflush (stdout);
    }
    if (*arg3 != 3) {
        printf ("last argument is %d and should be 3\n", *arg3);
        err++;
    }
    return err;
}

void FMNAME(cg_sub,CG_SUB)(int *i, STR_PSTR(str), int *j STR_PLEN(str))
{
    puts ("checking cg_sub");
    if (check_args (i, STR_PTR(str), j, STR_LEN(str)))
        puts ("incorrect interface");
    else
        puts ("OK");
}

void FMNAME(adfsub,ADFSUB)(int *i, STR_PSTR(str), int *j STR_PLEN(str))
{
    puts ("checking adfsub");
    if (check_args (i, STR_PTR(str), j, STR_LEN(str)))
        puts ("incorrect interface");
    else
        puts ("OK");
}
