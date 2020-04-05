#ifndef OS_SIGNAL_H
#define OS_SIGNAL_H


#ifdef __cplusplus
extern "C" {
#endif

#include <process/process.h>
#include <stddef.h>
struct ProcessDescriptor;
typedef enum{
     NORMAL,
     FRONT,
     FINALLY
} SignalRegisterType;

typedef enum {
    SIG_HUP = 0,
    SIG_KEY,

    SIG_CNT   // 所有的信号数量
}SignalType ;

typedef struct _Signal {
    SignalType type;
    int   value;
} Signal;

typedef bool(*SignalHandler)(const Signal*);
extern bool on_signal(const Signal *);

int do_signal(SignalType, uint64_t, pid_t);
int do_signal_register(SignalType);
int do_signal_register(SignalType);
void signalRegister(SignalType, ProcessDescriptor*, SignalRegisterType);
void signalUnregister(SignalType , ProcessDescriptor *);
void signalCheck();
void signal_init();


#ifdef __cplusplus
};
#endif

#endif //OS_SIGNAL_H
