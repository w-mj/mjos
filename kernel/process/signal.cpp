#include <list.hpp>
#include <stddef.h>
#include <process/process.h>
#include <process/signal.h>
#include <memory/kmalloc.h>
#include <delog.h>
#include <cpu.h>

struct SignalWrapper {
    Signal signal;
    // currentItem 指向当前正在处理该信号的进程在signalHandlers[type]链表中对应的链表项
    // currentItem == nullptr 表示没有任何一个进程处理过该信号
    // currentItem == &signalHandlers[type] 表示所有进程都已经处理过该信号
    ListEntry *currentItem;
    ListEntry nextSignal;
};

struct SignalHandlerItem {
    ListEntry nextProcess;
    ProcessDescriptor *process;
};

static ListEntry signalHandlers[SIG_CNT];
static ProcessDescriptor *signalHandlersFinally[SIG_CNT];

extern "C" void signal_init() {
    int i;
    forrange(i, 0, SIG_CNT){
        list_init(&signalHandlers[i]);
        signalHandlersFinally[i] = nullptr;
    }
}

SignalWrapper *signalCreate(SignalType type, uint64_t value) {
    auto *wrapper = (SignalWrapper *)kmalloc_s(sizeof(SignalWrapper));
    wrapper->signal.type = type;
    wrapper->signal.value = value;
    wrapper->currentItem = nullptr;
    list_init(&wrapper->nextSignal);
    return wrapper;
}

void signalSend(SignalWrapper *wrapper, ProcessDescriptor *to) {
    list_add_tail(&wrapper->nextSignal, &to->signal_ist);
}

void signalDestroy(SignalWrapper *wrapper) {
    kfree_s(sizeof(SignalWrapper), wrapper);
}

bool signalNext(SignalWrapper *wrapper) {
    ListEntry &processListHead = signalHandlers[wrapper->signal.type];
    if (wrapper->currentItem == &processListHead) {
        // finally也处理过该信号
        signalDestroy(wrapper);
        return false;
    }
    if (wrapper->currentItem == nullptr) {
        // 第一次调用
        wrapper->currentItem = processListHead.next;
    } else {
        // 向下一个进程发送信号
        wrapper->currentItem = wrapper->currentItem->next;
    }
    if (wrapper->currentItem != &processListHead) {
        // 有下一个进程可以处理信号
        auto handler = list_entry(wrapper->currentItem, SignalHandlerItem, nextProcess);
        signalSend(wrapper, handler->process);
        return true;
    } else {
        // 所有进程都已经处理过该信号，尝试finally
        if (signalHandlersFinally[wrapper->signal.type] == nullptr) {
            // 没有finally
            signalDestroy(wrapper);
            return false;
        }
        signalSend(wrapper, signalHandlersFinally[wrapper->signal.type]);
        return true;
    }
}

extern "C" int do_signal(SignalType type, uint64_t value, pid_t to) {
    auto *signal = signalCreate(type, value);
    if (to != NOPID) {
        // 单播信号
        auto process = get_process(to);
        if (process != nullptr && process->signalHandler != nullptr) {
            signalSend(signal, process);
        } else {
            return -1;
        }
    } else  {
        // 广播信号
        signalNext(signal);
    }
    return 0;
}

// 注册一个信号回调
extern "C" void signalRegister(SignalType type, ProcessDescriptor *process, SignalRegisterType regType) {
    ListEntry &head = signalHandlers[type];

    switch (regType) {
        case SignalRegisterType::NORMAL: {
            auto *handler = (SignalHandlerItem *) kmalloc_s(sizeof(SignalHandlerItem));
            handler->process = process;
            list_init(&handler->nextProcess);
            list_add_tail(&handler->nextProcess, &head);
            break;
        }
        case SignalRegisterType::FRONT: {
            auto *handler= (SignalHandlerItem*) kmalloc_s(sizeof(SignalHandlerItem));
            handler->process = process;
            list_init(&handler->nextProcess);
            list_add(&handler->nextProcess, &head);
            break;
        }
        case FINALLY:
            signalHandlersFinally[type] = process;
            break;
    }
}

extern "C" int do_signal_register(SignalType type) {
    ProcessDescriptor *process = thiscpu_var(current)->process;
    signalRegister(type, process, SignalRegisterType::NORMAL);
    return 0;
}

// 取消注册回调
extern "C" void signalUnregister(SignalType type, __ProcessDescriptor *process) {
    ListEntry *c;
    foreach(c, signalHandlers[type]) {
        auto *item = list_entry(c, SignalHandlerItem, nextProcess);
        if (item->process == process) {
            list_remove(&item->nextProcess);
            kfree_s(sizeof(SignalHandlerItem), item);
            break;
        }
    }
    if (signalHandlersFinally[type] == process)
        signalHandlersFinally[type] = nullptr;
}

extern "C" int do_signal_unregister(SignalType type) {
    auto process =thiscpu_var(current)->process;
    signalUnregister(type, process);
    return 0;
}

// 在进程调入之前检查并处理信号
extern "C" void signalCheck() {
    auto *process = thiscpu_var(current)->process;
    while (!list_empty(&process->signal_ist)) {
        auto wrapper_entry = list_pop_head(&process->signal_ist);
        auto wrapper = list_entry(wrapper_entry, SignalWrapper, nextSignal);
        if (process->signalHandler(&wrapper->signal) || wrapper->currentItem == nullptr) {
            // 进程处理了这个信号，或这个信号是单播信号
            signalDestroy(wrapper);
        } else {
            // 没处理这个广播信号
            signalNext(wrapper);
        }
    }
}
