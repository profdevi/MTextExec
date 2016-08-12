/*
Copyright (C) 2011-2014, Comine.com ( profdevi@ymail.com )
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
- The the names of the contributors of this project may not be used to 
  endorse or promote products derived from this software without specific 
  prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
`AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


//v1.9 copyright Comine.com 20160428R0819
#include <windows.h>
#include "MStdLib.h"
#include "MBuffer.h"
#include "MFileOps.h"
#include "MProcessJob.h"


//******************************************************
//**  MProcessJob class
//******************************************************
void MProcessJob::ClearObject(void)
	{
	MStdMemSet(&mProcInfo,0,sizeof(mProcInfo) );
	mTopWindow=NULL;
	}


/////////////////////////////////////////////////
BOOL CALLBACK MProcessJob::GGetTopWindow(HWND hwnd,LPARAM lparam)
	{
	MProcessJob *ref=(MProcessJob *)lparam;
	if(ref==NULL)
		{
		return FALSE;
		}

	DWORD procid;
	DWORD threadid;
	threadid=GetWindowThreadProcessId(hwnd,&procid);
	
	if(procid==ref->GetProcessID())
		{
		ref->mTopWindow=hwnd;
		return FALSE;
		}

	return TRUE;
	}


/////////////////////////////////////////////////
MProcessJob::MProcessJob(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MProcessJob::~MProcessJob(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MProcessJob::Create(const char *commandline,HANDLE hstdin,HANDLE hstdout,HANDLE hstderr)
	{
	Destroy();
	STARTUPINFOA startinfo;
	MStdMemSet(&startinfo,0,sizeof(startinfo) );

	// Set Inherit handles to false
	BOOL inherithandles=FALSE;

	// Dumplicate stdin
	HANDLE hchildstdin=INVALID_HANDLE_VALUE;
	if(hstdin!=INVALID_HANDLE_VALUE)
		{
		// 
		if(DuplicateHandle(GetCurrentProcess()
				,hstdin,GetCurrentProcess(),&hchildstdin,0
				,TRUE,DUPLICATE_SAME_ACCESS) ==FALSE)
			{
			Destroy();
			return false;
			}
		}

	// Duplicate Stdout
	HANDLE hchildstdout=INVALID_HANDLE_VALUE;
	if(hstdout!=INVALID_HANDLE_VALUE)
		{
		// duplicate the handle
		if(DuplicateHandle(GetCurrentProcess()
				,hstdout,GetCurrentProcess(),&hchildstdout,0
				,TRUE,DUPLICATE_SAME_ACCESS) ==FALSE)
			{
			Destroy();
			CloseHandle(hchildstdin);
			return false;
			}
		}

	// Duplicate StdError
	HANDLE hchildstderr=INVALID_HANDLE_VALUE;
	if(hstderr!=INVALID_HANDLE_VALUE)
		{
		// duplicate the handle
		if(DuplicateHandle(GetCurrentProcess()
				,hstderr,GetCurrentProcess(),&hchildstderr,0
				,TRUE,DUPLICATE_SAME_ACCESS) ==FALSE)
			{
			Destroy();
			CloseHandle(hchildstdin);
			CloseHandle(hchildstderr);
			return false;
			}
		}


	if(hchildstdin!=INVALID_HANDLE_VALUE 
			|| hchildstdout!=INVALID_HANDLE_VALUE
			|| hchildstderr!=INVALID_HANDLE_VALUE )
		{
		startinfo.hStdInput=hchildstdin;
		startinfo.hStdOutput=hchildstdout;
		startinfo.hStdError=hchildstderr;

		startinfo.dwFlags |= STARTF_USESTDHANDLES;
		inherithandles=TRUE;
		}
	
	startinfo.cb=sizeof(startinfo);

	MBuffer bufcommandline(2000);
	bufcommandline.SetString(commandline);

	if(CreateProcessA(NULL,bufcommandline,NULL,NULL
			,inherithandles,NORMAL_PRIORITY_CLASS
			,NULL,NULL,&startinfo,&mProcInfo)==FALSE)
		{
		Destroy();
		CloseHandle(hchildstdin);
		CloseHandle(hchildstdout);
		CloseHandle(hchildstderr);
		return false;
		}

	// Close extra handles if necessary
	CloseHandle(hchildstdin);
	CloseHandle(hchildstdout);
	CloseHandle(hchildstderr);
	return true;
	}


////////////////////////////////////////////////
bool MProcessJob::Create(DWORD processid,bool fullaccess)
	{
	Destroy();
	HANDLE hprocess;
	if(fullaccess==true)
		{
		hprocess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,processid);
		}
	else
		{
		hprocess=OpenProcess(PROCESS_TERMINATE|PROCESS_SUSPEND_RESUME
					|PROCESS_DUP_HANDLE|PROCESS_QUERY_INFORMATION
				,FALSE,processid);
		}

	if(hprocess==NULL)
		{
		Destroy();  return false;
		}

	mProcInfo.hProcess=hprocess;
	mProcInfo.dwProcessId=processid;

	return true;
	}


////////////////////////////////////////////////
bool MProcessJob::Destroy(void)
	{
	if(mProcInfo.hThread!=NULL) { CloseHandle(mProcInfo.hThread);  }
	if(mProcInfo.hProcess!=NULL) { CloseHandle(mProcInfo.hProcess);  }
	ClearObject();  return true;
	}


/////////////////////////////////////////////////
HANDLE MProcessJob::GetProcessHandle(void)
	{
	return mProcInfo.hProcess;
	}

/////////////////////////////////////////////////
HANDLE MProcessJob::GetThreadHandle(void)
	{
	return mProcInfo.hThread;
	}


/////////////////////////////////////////////////
DWORD MProcessJob::GetProcessID(void)
	{
	return mProcInfo.dwProcessId;
	}


/////////////////////////////////////////////////
DWORD MProcessJob::GetThreadID(void)
	{
	return mProcInfo.dwThreadId;
	}


/////////////////////////////////////////////////
int MProcessJob::Wait(DWORD timeout)// timeout is in ms
	{
	if(mProcInfo.hProcess==NULL)
		{
		return false;
		}

	DWORD dwret=WaitForSingleObject(mProcInfo.hProcess,timeout);
	if(dwret==WAIT_OBJECT_0) { return 1; }
	else if(dwret==WAIT_TIMEOUT) { return 0; }
	else  {  return -1;  }
	}


//////////////////////////////////////////////////
bool MProcessJob::Kill(void) // Kills Process
	{
	if(mProcInfo.hProcess==NULL)
		{
		return false;
		}

	if(TerminateProcess(mProcInfo.hProcess,255)==FALSE)
		{
		return false;
		}

	return true;
	}


////////////////////////////////////////////////////
bool MProcessJob::IsAlive(void)		//=true if process is alive
	{
	DWORD exitcode=0;
	if(GetExitCodeProcess(mProcInfo.hProcess,&exitcode)==FALSE)
		{
		return false;
		}

	if(exitcode==STILL_ACTIVE) { return true; }

	return false;
	}


////////////////////////////////////////////////////////
int MProcessJob::GetExitValue(void)	//=-1 if still running
	{
	DWORD exitcode=0;
	if(GetExitCodeProcess(mProcInfo.hProcess,&exitcode)==FALSE)
		{
		return -1;
		}

	if(exitcode==STILL_ACTIVE) { return -1; }

	return (int)exitcode;
	}


/////////////////////////////////////////////////////////
HWND MProcessJob::GetTopWindow(void)
	{
	if(mTopWindow==NULL)
		{  EnumWindows(GGetTopWindow,(LPARAM)this);  }

	return mTopWindow;
	}

