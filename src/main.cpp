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


//v1.8 copyright Comine.com 20160812F1336
#include "MStdLib.h"
#include "MCommandArg.h"
#include "MProcessJob.h"
#include "MFile.h"
#include "MFileOps.h"
#include "MString.h"
#include "MStringBuffer.h"
#include "MBuffer.h"


//******************************************************
//* Module Elements
//******************************************************
static const char *GApplicationName="MTextExec";	// Used in Help
static const char *GApplicationVersion="1.8";		// Used in Help

//////////////////////////////////////////////////////////
static const int GMaxLineSize=50000;

////////////////////////////////////////////////////
static void GDisplayHelp(void);
static bool GProcessFile(const char *filename,bool overwrite,bool clearout);
static bool GIsExecBeginLine(const char *line,MString &executable);
static bool GIsExecEndLine(const char *line,MString &defaulttext);
static bool GIsFileProcessable(const char *filename,int &jobcount);
static bool GIsExecutable(const char *command);
static bool GWriteOutput(const char *command,MFileOutput &openfile);
static bool GGetSpacePrefix(const char *line,MString &prefix);

////////////////////////////////////////////////////
int main(int argn,const char *argv[])
	{
	MCommandArg args(argn,argv);

	// Check for write flag
	bool overwrite=false;
	if(args.CheckRemoveArg("-w")==true)
		{
		overwrite=true;
		}

	bool clearoutputs=false;
	if(args.CheckRemoveArg("-c")==true)
		{
		clearoutputs=true;
		}

	////////////////////////////////////////////////////
	if(args.GetArgCount()<2)
		{
		GDisplayHelp();
		return 0;
		}

	////////////////////////////////////////////////////
	int i;
	for(i=1;i<args.GetArgCount();++i)
		{
		int jobcount;
		MStdPrintf("Processing %-30s",args.GetArg(i) );
		if(GIsFileProcessable(args.GetArg(i),jobcount)==false)
			{
			MStdPrintf(": **Failed**\n");
			continue;
			}

		if(jobcount==0)
			{
			MStdPrintf(": No Jobs\n");
			continue;
			}

		MStdPrintf("\n");

		if(GProcessFile(args.GetArg(i),overwrite,clearoutputs)==false)
			{
			MStdPrintf("    **Processing Failed\n");
			continue;
			}
		}

	return 0;
	}


////////////////////////////////////////////////////
static void GDisplayHelp(void)
	{
	MStdPrintf(	"\n"
				"   usage:  %s [-w|-c] <file>+ [-?]\n"
				"           v%s copyright Comine.com\n"
				"\n"
				"   Executes Embedded commands in a text file\n"
				"\n"
				"           -w : Write to the files\n"
				"           -c : Clear regions with default values\n"
				"\n"
				"   Examples\n"
				"           //@execbegin banner \"Hello World\"\n"
				"                **Intervening Lines  ** \n"
				"           //@execend DEFAULT TEXT\n"
				"\n"
				"           <!--execbegin banner \"Hello World\" -->\n"
				"                **Intervening Lines  ** \n"
				"           <!--execend DEFAULT TEXT-->\n"
				"\n"
				"           %%%%%%execbegin banner \"Hello World\" -->\n"
				"                **Intervening Lines  ** \n"
				"           %%%%%%execend DEFAULT TEXT-->\n"
				"\n"
				"           ###execbegin banner \"Hello World\" -->\n"
				"                **Intervening Lines  ** \n"
				"           ###execend DEFAULT TEXT-->\n"
				"\n"


				,GApplicationName,GApplicationVersion);
	}


////////////////////////////////////////////////////
static bool GProcessFile(const char *filename,bool overwrite,bool clearoutput)
	{
	MFileOps fileops;
	if(fileops.Create()==false)
		{
		return false;
		}

	// Get tmp src file
	MString tmpsrcfile;
	if(fileops.GetTemperoryFileName(tmpsrcfile)==false)
		{
		return false;
		}

	if(fileops.Copy(filename,tmpsrcfile.Get())==false)
		{
		return false;
		}

	//=We now have a copy of the src file

	// Get temp filename
	MString tmpfilename;
	if(fileops.GetTemperoryFileName(tmpfilename)==false)
		{
		return false;
		}

	// Open File for input
	MFileInput filein;
	if(filein.Create(tmpsrcfile.Get() )==false)
		{
		return false;
		}

	// Create tmp buffer
	MBuffer buffer;
	if(buffer.Create(GMaxLineSize)==false)
		{
		return false;
		}

	// Open FileOutput
	MFileOutput fileout;
	if(fileout.Create(tmpfilename.Get())==false)
		{
		return false;
		}

	// Loop the current line
	bool stateinexec=false;
	bool keepdefaultstring=false;
	while(filein.ReadLine(buffer,buffer.GetSize()-1)==true)
		{
		MString exec;
		// Slower but more readable 
		if(stateinexec==false && GIsExecBeginLine(buffer,exec)==true)
			{
			stateinexec=true;

			// Write the buffer line
			fileout.WriteChars(buffer.GetBuffer() );

			if(clearoutput==false)
				{
				//We have to process the file
				if(GIsExecutable(exec.Get())==false)
					{
					keepdefaultstring=true;
					continue;
					}

				GWriteOutput(exec.Get(),fileout);
				}
			else
				{
				MString strcommand;
				strcommand.Create(exec.Get());
				strcommand.TrimLeft();
				strcommand.TrimRight();
				MStdPrintf("    Command : %s  - Skip\n",strcommand.Get() );
				}

			continue;
			}
		else if(stateinexec==true && GIsExecEndLine(buffer,exec)==true)
			{
			if(clearoutput==true || keepdefaultstring==true)
				{
				MString prefix;
				GGetSpacePrefix(buffer.GetBuffer(),prefix);
				fileout.WriteChars(prefix.Get());
				fileout.WriteChars(exec.Get() );
				fileout.WriteChars("\n");
				}

			fileout.WriteChars(buffer.GetBuffer() );
			stateinexec=false;
			keepdefaultstring=false;
			continue;
			}
		else if(stateinexec==false && GIsExecBeginLine(buffer,exec)==false)
			{
			fileout.WriteChars(buffer.GetBuffer() );
			continue;
			}
		else if(stateinexec==true && GIsExecEndLine(buffer,exec)==false)
			{
			// skip every thing
			continue;
			}
		else
			{
			fileops.Delete(tmpfilename.Get());
			fileops.Delete(tmpsrcfile.Get() );			
			return false;
			}
		}

	// Close the files
	fileout.Destroy();
	filein.Destroy();

	// Convert text file back to dos
	if(fileops.ConvertTextUnixToDos(tmpfilename.Get())==false)
		{
		fileops.Delete(tmpfilename.Get());
		fileops.Delete(tmpsrcfile.Get() );
		return false;
		}

	if(overwrite==true)
		{
		if(fileops.Copy(tmpfilename.Get(),filename)==false)
			{
			MStdPrintf("**Unable to overwrite original file %s\n",filename);
			return false;
			}
		}

	fileops.Delete(tmpfilename.Get());
	fileops.Delete(tmpsrcfile.Get() );

	return true;
	}


///////////////////////////////////////////////////////////////
static bool GIsExecBeginLine(const char *line,MString &executable)
	{
	MString dataline;
	if(dataline.Create(line)==false)
		{
		return false;
		}

	if(dataline.TrimLeft()==false)
		{
		return false;
		}

	// Check for C like Comment
	if(MStdMemCmp("//@execbegin",dataline.Get(),12)==0)
		{
		//=We have an executable line
		if(executable.Create(dataline.Get()+12)==false)
			{
			return false;
			}

		return true;
		}

	// Check for Latex like comment
	if(MStdMemCmp("%%%execbegin",dataline.Get(),12)==0)
		{
		//=We have an executable line
		if(executable.Create(dataline.Get()+12)==false)
			{
			return false;
			}

		return true;
		}

	// Check for # like comment
	if(MStdMemCmp("###execbegin",dataline.Get(),12)==0)
		{
		//=We have an executable line
		if(executable.Create(dataline.Get()+12)==false)
			{
			return false;
			}

		return true;
		}


	dataline.TrimRight();

	// Check for HTML Comment
	if(MStdMemCmp("<!--execbegin",dataline.Get(),13)==0)
		{
		if(MStdMemCmp("-->",dataline.GetEnd(3),3)!=0)
			{
			return false;
			}

		// Remove -->
		dataline.TrimRight(3);

		//=We have an executable line
		if(executable.Create(dataline.Get()+13)==false)
			{
			return false;
			}

		return true;
		}

	return false;
	}


///////////////////////////////////////////////////////////////
static bool GIsExecEndLine(const char *line,MString &defaultvalue)
	{
	MString dataline;
	if(dataline.Create(line)==false)
		{
		return false;
		}

	if(dataline.TrimLeft()==false)
		{
		return false;
		}

	if(dataline.TrimRight()==false)
		{
		return false;
		}

	// Check for C like comment
	if(MStdMemCmp("//@execend",dataline.Get(),10)==0)
		{
		//=This is the current end line
		defaultvalue.Create(dataline.Get()+10);
		//defaultvalue.TrimLeft();
		defaultvalue.TrimRight();
		return true;
		}

	// Check for latex like comment
	if(MStdMemCmp("%%%execend",dataline.Get(),10)==0)
		{
		//=This is the current end line
		defaultvalue.Create(dataline.Get()+10);
		//defaultvalue.TrimLeft();
		defaultvalue.TrimRight();
		return true;
		}

	// Check for # like comment
	if(MStdMemCmp("###execend",dataline.Get(),10)==0)
		{
		//=This is the current end line
		defaultvalue.Create(dataline.Get()+10);
		//defaultvalue.TrimLeft();
		defaultvalue.TrimRight();
		return true;
		}

	// Check for html like comment
	if(MStdMemCmp("<!--execend",dataline.Get(),11)==0)
		{
		if(MStdMemCmp(dataline.GetEnd(3),"-->",3)!=0)
			{
			return false;
			}

		defaultvalue.Create(dataline.Get()+11);
		//defaultvalue.TrimLeft();
		defaultvalue.TrimRight(3);
		defaultvalue.TrimRight();

		//=This is the current end line
		return true;
		}

	return false;
	}


////////////////////////////////////////////////////////////////
static bool GIsFileProcessable(const char *filename,int &jobcount)
	{
	// Open File for input
	MFileInput filein;
	if(filein.Create(filename)==false)
		{
		return false;
		}

	// Create tmp buffer
	MBuffer buffer;
	if(buffer.Create(GMaxLineSize)==false)
		{
		return false;
		}

	// init jobcount
	jobcount=0;

	// Loop the current line
	MString executable;
	bool inexec=false;
	int lineno;
	for(lineno=1;filein.ReadLine(buffer,buffer.GetSize()-1)==true;++lineno)
		{
		if(inexec==false)
			{
			MString defaulttext;
			if(GIsExecBeginLine(buffer,executable)==true)
				{
				jobcount=jobcount+1;
				inexec=true;
				}
			else if(GIsExecEndLine(buffer,defaulttext)==true)
				{
				return false;
				}
			}
		else
			{
			MString defaulttext;
			if(GIsExecEndLine(buffer,defaulttext)==true)
				{
				inexec=false;
				}
			else if(GIsExecBeginLine(buffer,executable)==true)
				{
				return false;
				}
			}
		}

	if(inexec==true)
		{
		return false;
		}

	filein.Destroy();

	return true;
	}


///////////////////////////////////////////////////////
static bool GIsExecutable(const char *command)
	{
	MString strcommand;
	strcommand.Create(command);
	strcommand.TrimLeft();
	strcommand.TrimRight();

	// Create a temp file output
	MFileOps fileops(true);
	MString tmpfilename;
	fileops.GetTemperoryFileName(tmpfilename);
	MFileOutput tmpfileout;
	tmpfileout.Create(tmpfilename.Get());

	MProcessJob job;
	if(job.Create(strcommand.Get(),INVALID_HANDLE_VALUE,tmpfileout.GetHandle())==false)
		{
		tmpfileout.Destroy();
		fileops.Delete(tmpfilename.Get());
		MStdPrintf("    Command : %-50s  - FAILED\n",strcommand.Get() );
		return false;
		}

	job.Wait();

	tmpfileout.Destroy();
	fileops.Delete(tmpfilename.Get());

	if(job.GetExitValue()==0)
		{  return true;  }

	MStdPrintf("    Command : %-50s  - Skipped\n",strcommand.Get() );
	return false;
	}


///////////////////////////////////////////////////////
static bool GWriteOutput(const char *command,MFileOutput &openfile)
	{
	MString strcommand;
	strcommand.Create(command);
	strcommand.TrimLeft();
	strcommand.TrimRight();

	MProcessJob job;
	if(job.Create(strcommand.Get() ,INVALID_HANDLE_VALUE,openfile.GetHandle()
			,INVALID_HANDLE_VALUE)==false)
		{
		MStdPrintf("    Command : %-50s  - FAILED\n",strcommand.Get() );
		return false;
		}

	MStdPrintf("    Command : %-50s  - OK\n",strcommand.Get() );

	job.Wait();	
	return true;
	}


/////////////////////////////////////////////////////
static bool GGetSpacePrefix(const char *line,MString &prefix)
	{
	MStringBuffer buffer(1000);
	int i;
	for(i=0;line[i]!=0;++i)
		{
		if(MStdIsSpace(line[i])==true)
			{
			buffer.Add(line[i]);
			continue;
			}

		break;
		}

	prefix.Create(buffer.Get() );
	return true;
	}

