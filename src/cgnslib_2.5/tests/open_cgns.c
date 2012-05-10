#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int main (int argc, char **argv)
{
    double start, finish;
    int cgfile;

    if (argc != 2) {
        fprintf (stderr, "open_cgns CGNSfile\n");
        exit (1);
    }

    printf ("opening cgns file <%s> ...", argv[1]);
    fflush (stdout);
    start = elapsed_time ();
    if (cg_open (argv[1], CG_MODE_READ, &cgfile)) cg_error_exit();
    finish = elapsed_time ();
    printf (" %g secs\n", finish - start);

    printf ("closing cgns file ...");
    fflush (stdout);
    start = elapsed_time ();
    cg_close (cgfile);
    finish = elapsed_time ();
    printf (" %g secs\n", finish - start);

    return 0;
}

