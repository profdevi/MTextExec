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
#ifndef MProcessJob_h
#define MProcessJob_h

////////////////////////////////////////////////////
#include <windows.h>

//******************************************************
//**  MProcessJob class
//******************************************************
class MProcessJob
	{
	PROCESS_INFORMATION mProcInfo;
	HWND mTopWindow;

	////////////////////////////////////////////////
	void ClearObject(void);
	static BOOL CALLBACK GGetTopWindow(HWND hwnd,LPARAM lparam);

	////////////////////////////////////////////////
	public:
	MProcessJob(void);
	~MProcessJob(void);
	bool Create(const char *commandline
			,HANDLE hstdin=INVALID_HANDLE_VALUE
			,HANDLE hstdout=INVALID_HANDLE_VALUE
			,HANDLE hstderr=INVALID_HANDLE_VALUE);
	bool Create(DWORD processid,bool fullaccess=false);
	bool Destroy(void);
	HANDLE GetProcessHandle(void);
	HANDLE GetThreadHandle(void);
	DWORD GetProcessID(void);
	DWORD GetThreadID(void);
	int Wait(DWORD timeout=INFINITE);	// 0-timed out,1-process done,-1=error
	bool Kill(void);		// Kills Process Very SEVERE for global data in dlls
	bool IsAlive(void);		//=true if process is alive
	int GetExitValue(void);	//=-1 if still running
	HWND GetTopWindow(void);	// Returns the top level window
	};

#endif // MProcessJob_h

