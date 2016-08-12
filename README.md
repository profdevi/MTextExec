MTextExec is a program that permits the user to embed commands in 
source code.  The standard output of the commands replace the text
in the source files.  The intent of this program is to permit
the inclusion of programatic generated text into source programs
written in shell script/C/C++/Python/Latex and HTML/...


Example Usage: 
----------

Source File : one.htm:

	<html>
	<body>
	<!--execbegin ls -l>
	<!--execend -->


	</body>
	</html> 
	
The command **ls -l** will print out the contents of the current 
directory.  When you run the command **mtextexec -w one.htm**, 
the program modifies the one.htm file to
	
	<html>
	<body>
	<!--execbegin ls -l>
	.
	..
	a.txt
	b.txt
	...
	...
	<!--execend -->


	</body>
	</html> 


If you run the command **mtextexec -w -c one.htm**, the file is reset
back to the original.
	

Build Instructions
-------------------

Use NMake to build the windows version of the program.
Switch to the src folder and compile with NMake.


Windows Executable
-------------------

bin/MTextExec.exe      	: precompiled version for windows 10/64 bit
bin/MTextExec.exe.md5	: MD5 Digest of the executable


## License

Copyright (C) 2011-2014, Comine.com ( profdevi@ymail.com )
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

*	Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
  
*	Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
  
*	The the names of the contributors of this project may not be used to 
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


