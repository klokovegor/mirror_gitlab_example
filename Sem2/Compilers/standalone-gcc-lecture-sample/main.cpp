#include <stdio.h>
#include "function.h"

extern int nCompletionStatus;

int main(int argc, char* argv[])
{
    float x = 1.0;
    float y = 5.0;
    float z;

    z = add_and_multiply(x, y);
    nCompletionStatus = 1;

    printf("Result: %.2f\n", z);
    printf("Completion Status: %d\n", nCompletionStatus);

    return 0;
}
