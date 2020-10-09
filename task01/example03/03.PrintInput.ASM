format PE console
include 'win32axp.inc'
.data
           ns dd ?
           hout dd ?
           buffer db 80 dup (?)
.code
begin:

invoke GetStdHandle,STD_OUTPUT_HANDLE
mov [hout],eax
invoke GetStdHandle,STD_INPUT_HANDLE

invoke ReadConsole,eax,buffer,80,ns,NULL
invoke WriteConsole,[hout],buffer,80,ns,NULL

invoke Sleep, 5000
invoke ExitProcess,0
.end begin      
