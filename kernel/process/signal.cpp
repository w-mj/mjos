#include <list.hpp>
#include <stddef.h>
#include <process/process.h>
#include <process/signal.h>
#include <memory/kmalloc.h>


struct SignalHandlerList {
    SignalHandler finally = nullptr;   // 如果list中的所有进程都不能处理这个信号，执行finally
    SignalHandler anyway  = nullptr;   // 无论进程能不能处理信号，都执行finally
    os::list<SignalHandler> list;
};

static SignalHandlerList signalHandlers[SIG_CNT];

extern "C" int do_signal(SignalType type, uint64_t value, pid_t to) {
    auto *signal = (Signal *)kmalloc_s(sizeof(Signal));
    signal->from = do_getpid();
    signal->type = type;
    signal->value = value;
    if (to != NOPID) {
        // 单播信号
        auto process = get_process(to);
        if (process != nullptr && process->signalHandler != nullptr) {
            process->signalHandler(signal);
        } else {
            return -1;
        }
    } else  {
        // 广播信号
        bool solved = false; // 这个信号是否被处理过
        for (SignalHandler & handler: signalHandlers[type].list) {
            if ((solved = handler(signal)))
                break;
        }
        if (!solved && signalHandlers[type].finally != nullptr) {
            signalHandlers[type].finally(signal);
        }
        if (signalHandlers[type].anyway != nullptr) {
            signalHandlers[type].anyway(signal);
        }
    }
    kfree_s(sizeof(Signal), signal);
    return 0;
}

// 注册一个信号回调
void signalRegister(SignalType type, SignalHandler handler, SignalRegisterType regType) {
    SignalHandlerList &list = signalHandlers[type];
    switch (regType) {
        case SignalRegisterType::NORMAL:
            list.list.push_back(handler);
            break;
        case SignalRegisterType::FRONT:
            list.list.push_front(handler);
            break;
        case SignalRegisterType::ANYWAY:
            list.anyway = handler;
            break;
        case SignalRegisterType::FINALLY:
            list.finally = handler;
            break;
    }
}

// 取消注册回调
void signalUnregister(SignalHandler handler) {
    for (auto & list : signalHandlers) {
        if (list.finally == handler)
            list.finally = nullptr;
        if (list.anyway == handler)
            list.anyway = nullptr;
        list.list.remove(handler);
    }
}