#progma once


#define GLOBAL_FUNC(sym)        \
    .global sym;                \
    .type   sym, @function

#define GLOBAL_DATA(sym)        \
    .global sym;                \
    .type   sym, @object

#define EXTERN_FUNC(sym)        \
    .extern sym;                \
    .type   sym, @function

#define EXTERN_DATA(sym)        \
    .extern sym;                \
    .type   sym, @object
