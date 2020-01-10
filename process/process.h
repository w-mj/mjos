#progma once
#include "include/types.h"

class Progress {
public:
	/**
	 * 进程状态
	 * RUNNING：处于运行态和就绪态的进程都为RUNNING
	 * INTERRUPTIBLE：处于休眠状态的进程
	 * UNINTERRUPTIBLE：处于休眠状态的进程，但不能被信号唤醒
	 * STOPPED：被终止的进程，正处于回收资源阶段
	 * TRACED：被追踪的进程
	 * ZOMBIE：僵尸进程
	 * DEAD：已经死亡的进程
	 */ 
	enum class State {
		RUNNING, INTERRUPTIBLE, UNINTERRUPTIBLE, STOPPED, TRACED, ZOMBIE, DEAD};
	void *code = nullptr;
	State state = RUNNING;
	uint_16 id;
};
		
