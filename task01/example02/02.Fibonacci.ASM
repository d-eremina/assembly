;###################################################################
; C0d3d by Shkolnik Prahramist ? a.k.a. Shan0x228
; YouTube: https://www.youtube.com/channel/UCmJT3IfHtpFJyln2UdABBKg
; Discord: Shan0x228#5690
; Copyright © 2018
;###################################################################
format PE console
entry start
;------------------------------------------------------------
include "win32a.inc"
;------------------------------------------------------------
section '.rdata' data readable writeable
        ; Console title buffer
        szConsoleTitle          db      "Fibonacci numbers with FASM",0
        ; Message
        szMessage               db      "Enter iterations count(<=46): ",0

        ; Text formatting for i/o
        szDebug                 db      "->: %d",10,0
        szFormat                db      "%d",0
        szPause                 db      "pause",0

        ; For math
        nValue                  dd      1
        nPreValue               dd      0

        ; Iterations count
        nLimitValue             dd      ?
;------------------------------------------------------------
section '.text' code executable
        start:
                ;Changing console title
                invoke SetConsoleTitle, szConsoleTitle
                ;Displaying text
                invoke printf, szMessage
                ;Waiting user-input and save it to memory buffer
                invoke scanf, szFormat, nLimitValue

                ;Calling main math procedure
                push [nLimitValue]
                call FibNum

                ; system("pause")
                invoke system, szPause

                ;Exit process
                invoke ExitProcess, 0
;------------------------------------------------------------
        proc FibNum, limit_value:DWORD
                mov ecx, [limit_value]
        for_loop:
                ; Clear registers
                xor eax, eax
                xor ebx, ebx

                ;/////////////////////////////
                mov eax, [nPreValue]
                add eax, [nValue]
                mov ebx, [nPreValue]
                mov [nPreValue], eax
                mov [nValue], ebx
                ;/////////////////////////////

                ; Display value
                push ecx
                invoke printf, szDebug, [nPreValue]
                pop ecx
                
                ; restore ecx value
                mov ecx, [esp+4]
                ; decreese ecx(-1)
                dec ecx
                ; if (ecx != 0) goto for_loop
                jnz for_loop
                ; return
                ret
        endp        
;------------------------------------------------------------
section '.idata' data readable import

    ;Libraries
    library kernel32, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll'

    ;Kernel32.dll imports
    import kernel32,\
    ExitProcess, 'ExitProcess',\
    SetConsoleTitle, 'SetConsoleTitleA'

    ;msvcrt.dll imports
    import msvcrt,\
    printf, 'printf',\
    system, 'system',\
    scanf, 'scanf'