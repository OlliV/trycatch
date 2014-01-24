#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

typedef struct {
    jmp_buf ex_env;
    int ex_type;
} exception_t;

typedef struct {
    size_t index;
    exception_t ex[];
} exceptionstack_t;

static exceptionstack_t * estack;

void function(void);
void function2(void);
void try_begin(void);
void try_end(void);
void throw(int exception_type);

#define try switch(setjmp(estack->ex[estack->index++].ex_env))
#define get_exception() (&(estack->ex[estack->index - 1]))

int main(void)
{
    estack = malloc(sizeof(exceptionstack_t));
    estack->index = 0;

    try_begin();
    try {
        /* Try */
        case 0:
            function();
            break;
        /* Catch */
        case 1:
            printf("Exception 1\n");
            break;
        default:
            printf("Unhandled exception (%i)!\n", get_exception()->ex_type);
            exit(1);
    }
    try_end();

    return 0;
}

void function(void)
{
    printf("In function\n");

    try_begin();
    try {
        case 0:
            function2();
            break;
        default:
            printf("Catch all (%i)\n", get_exception()->ex_type);
    }
    try_end();

    throw(2);
    printf("Leaving function\n");
}

void function2(void)
{
    throw(4);
}

void try_begin(void)
{
    estack = realloc(estack, sizeof(exceptionstack_t)
            + (estack->index + 1) * sizeof(exception_t));
}

void try_end(void)
{
    estack->index--;
    estack = realloc(estack, sizeof(exceptionstack_t)
            + (estack->index + 1) * sizeof(exception_t));
}

void throw(int exception_type)
{
    exception_t * ex = get_exception();

    ex->ex_type = exception_type;
    longjmp(ex->ex_env, ex->ex_type);
}

