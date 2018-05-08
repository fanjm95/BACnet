#pragma once
#include "ComWrapper.h"
#include <Windows.h>
#include <process.h>

class CThread :
	public ComWrapper<IBACnetThread>
{
	ThreadFunction tf;
	CObjectPtr<IBACnetEvent> cancel;
	HANDLE thread;
	BACnetResult exitcode;

	DWORD ThreadEntry();

public:
	CThread(ThreadFunction tf);
	~CThread();

	void* BACNETMETHODCALLTYPE GetWaitHandle() const { return thread; }
	BACnetResult BACNETMETHODCALLTYPE Start();
	BACnetResult BACNETMETHODCALLTYPE Cancel();
	CObjectPtr<IBACnetEvent> BACNETMETHODCALLTYPE GetCancellationEvent();
	BACnetResult BACNETMETHODCALLTYPE Terminate(BACnetResult TerminationCode);
	BACnetResult BACNETMETHODCALLTYPE Stop(U32 Timeout = 10000);
	BACnetResult BACNETMETHODCALLTYPE GetExitCode();

};

