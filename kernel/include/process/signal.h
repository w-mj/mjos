#ifndef OS_SIGNAL_H
#define OS_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef enum{
     NORMAL,
     FRONT,
     ANYWAY,
     FINALLY
} SignalRegisterType;

typedef enum {
    SIG_HUP = 0,
    SIG_KEY,

    SIG_CNT   // 所有的信号数量
}SignalType ;

typedef struct _Signal {
    pid_t from;
    SignalType type;
    int   value;
} Signal;

typedef bool(*SignalHandler)(const Signal*);

int do_signal(SignalType, uint64_t, pid_t);
void signalRegister(SignalType, SignalHandler, SignalRegisterType);
void signalUnregister(SignalHandler);


#ifdef __cplusplus
};
#endif

#endif //OS_SIGNAL_H
