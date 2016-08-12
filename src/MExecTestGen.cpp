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


//v1.4 copyright Comine.com 20160428R0817
#include "MStdLib.h"
#include "MFile.h"
#include "MFileOps.h"
#include "MBuffer.h"
#include "MExecTestGen.h"

// Data Files
#include "MExecTestGen.dat"


//******************************************************
//**  Module Elements
//******************************************************
// 2nd byte 00 is return value
// Com Prorgram: mova ax,4c00h;  int 21h
static const unsigned char GSimpleComImage[]
		={0xB8,0x00,0x4C,0xCD,0x21};

// Com Image for a forever loop
// Program:  jmp 100
static const unsigned char GSimpleLoopComImage[]
		={0xEB,0xFE};

//******************************************************
//**  MExecTestGen class
//******************************************************
void MExecTestGen::ClearObject(void)
	{
	}


////////////////////////////////////////////////
MExecTestGen::MExecTestGen(void)
	{  ClearObject();  }


////////////////////////////////////////////////
MExecTestGen::~MExecTestGen(void)
	{  Destroy();  }


////////////////////////////////////////////////
bool MExecTestGen::Create(void)
	{
	Destroy();
	return true;
	}


////////////////////////////////////////////////
bool MExecTestGen::Destroy(void)
	{
	ClearObject();
	return true;
	}


////////////////////////////////////////////////
bool MExecTestGen::MakeReturnCom(const char *filename,int retvalue)
	{
	
	MFileOutput output;
	if(output.Create(filename)==false)
		{
		return false;
		}

	for(int i=0;i<sizeof(GSimpleComImage);++i)
		{
		if(i==1)
			{ output.Write((unsigned char) (retvalue & 0xFF) ); }
		else
			{  output.Write(GSimpleComImage[i]);  }
		}

	output.Destroy();
	
	return true;
	}


//////////////////////////////////////////////////////////////
bool MExecTestGen::MakeLoopCom(const char *filename)
	{
	
	MFileOutput output;
	if(output.Create(filename)==false)
		{
		return false;
		}

	for(int i=0;i<sizeof(GSimpleLoopComImage);++i)
		{  output.Write(GSimpleLoopComImage[i]);  }

	output.Destroy();

	return true;
	}


////////////////////////////////////////////////////////////////
bool MExecTestGen::MakeSleepExe(const char *filename)
	{
	
	MFileOutput output;
	if(output.Create(filename)==false)
		{
		return false;
		}

	for(int i=0;i<sizeof(GSleepExeImage);++i)
		{  output.Write(GSleepExeImage[i]);  }

	output.Destroy();

	return true;
	}

