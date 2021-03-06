//
// Chino Thread
//
#include "ProcessManager.hpp"
#include <libbsp/bsp.hpp>
#include "../kdebug.hpp"

extern "C"
{
	uintptr_t g_CurrentThreadContext = 0;
}

using namespace Chino::Thread;

static void OnThreadExit();
static void IdleThreadMain();

ProcessManager::ProcessManager()
	:runningThread_(0), idleProcess_(nullptr)
{

}

Process& ProcessManager::CreateProcess(std::string_view name, uint32_t mainThreadPriority, std::function<void()> threadMain)
{
	auto it = _processes.emplace_back(MakeObject<Process>(name));
	it->AddThread(std::move(threadMain), mainThreadPriority);
	return *it;
}

void ProcessManager::AddReadyThread(Thread& thread)
{
	auto priority = thread.GetPriority();
	kassert(priority < readyThreads_.size());
	readyThreads_[priority].emplace_back(&thread);
	kassert(!readyThreads_[priority].empty());
}

void ProcessManager::StartScheduler()
{
	kassert(!idleProcess_);
	idleProcess_.Reset(&CreateProcess("System Idle", 0, IdleThreadMain));

	BSPSetupSchedulerTimer();
	ArchHaltProcessor();
}

ProcessManager::thread_it ProcessManager::SelectNextSwitchToThread()
{
	// Round robin in threads of same priority
	thread_it threadSwitchTo;
	if (runningThread_.good())
	{
		auto next = runningThread_;
		if ((++next).good())
			threadSwitchTo = next;
	}

	if (!threadSwitchTo.good())
	{
		for (auto it = readyThreads_.rbegin(); it != readyThreads_.rend(); ++it)
		{
			if (!it->empty())
			{
				threadSwitchTo = it->begin();
				break;
			}
		}
	}

	kassert(threadSwitchTo.good());
	return threadSwitchTo;
}

ThreadContext_Arch& ProcessManager::SwitchThreadContext()
{
	auto nextThread = SelectNextSwitchToThread();
	runningThread_ = nextThread;
	auto& arch = (*nextThread)->GetContext();
#if 0
	g_Logger->PutFormat("PSP: %x\n", arch.sp);
#endif
	return arch;
}

Process::Process(std::string_view name)
	:name_(name)
{
}

Thread& Process::AddThread(std::function<void()> threadMain, uint32_t priority)
{
	auto& thread = *threads_.emplace_back(MakeObject<Thread>(std::move(threadMain), priority));
	g_ProcessMgr->AddReadyThread(thread);
	return thread;
}

Thread::Thread(std::function<void()> threadMain, uint32_t priority)
	:priority_(priority), threadContext_({}), threadMain_(std::move(threadMain))
{
	kassert(threadMain_ && priority <= MAX_THREAD_PRIORITY);
	auto stackSize = DEFAULT_THREAD_STACK_SIZE;
	stack_ = std::make_unique<uint8_t[]>(stackSize);
	auto stackPointer = uintptr_t(stack_.get()) + stackSize;
	ArchInitializeThreadContextArch(&threadContext_, stackPointer, uintptr_t(ThreadMainThunk), uintptr_t(OnThreadExit), uintptr_t(this));
}

void Thread::ThreadMainThunk(Thread* thread)
{
	kassert(thread && thread->threadMain_);
	thread->threadMain_();
}

static void OnThreadExit()
{
	kassert(!"Exit unexpected.");
}

static void IdleThreadMain()
{
	while (1)
	{
		for (size_t i = 0; i < 100; i++)
			ArchHaltProcessor();
		g_Logger->PutChar(L'.');
	}
}

extern "C" void Kernel_SwitchThreadContext()
{
	g_CurrentThreadContext = uintptr_t(&g_ProcessMgr->SwitchThreadContext());
}
