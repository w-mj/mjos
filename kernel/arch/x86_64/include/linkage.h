#pragma once

#define GLOBAL_FUNC(name) \
    .global name; \
    .type   name, @function

#define GLOBAL_DATA(name) \
    .global name; \
    .type   name, @object

#define EXTERN_FUNC(name) \
    .extern name; \
    .type   name, @function

#define EXTERN_DATA(name) \
    .extern name; \
    .type   name, @object

#define GLOBAL(name)\
	.globl name;\
	name:


#ifndef ENTRY
#define ENTRY(name) \
  .globl name; \
  ALIGN; \
  name:
#endif

#ifndef WEAK
#define WEAK(name)	   \
	.weak name;	   \
	name:
#endif

#ifndef END
#define END(name) \
  .size name, .-name
#endif

/* If namebol 'name' is treated as a subroutine (gets called, and returns)
 * then please use ENDPROC to mark 'name' as STT_FUNC for the benefit of
 * static analysis tools such as stack depth analyzer.
 */
#ifndef ENDPROC
#define ENDPROC(name) \
  .type name, @function; \
  END(name)

#endif
