format PE console
entry start

include 'win32a.inc'
include 'microLib.inc'  ; macros file
;-------------------------------------------------------------------------------------------------
section '.data' data readable writable
        ; strings for output
        strIntro db "Searching for machine epsilon in range [%.1f, %.1f]", 10, 0
        strAns db "Result is: %.15e", 10, 0
        strDiff db "Difference between found value and actual machine epsilon is: %.15e", 10, 0

        strForExit  db '> Press any key to exit', 10, 0

        left  dq 0.0    ; initialising lower limit of segment
        right dq 1.0    ; initialising upper limit of segment

        one dq 1.0      ; const value
        half dq 2.0     ; const value
        prev dq 2.0     ; saving previous found value
        tmp dq ?        ; to save some temporary values

        actualEpsilon dq 4.94065645841247e-324  ; actual epsilon to check parity of calculations
;-------------------------------------------------------------------------------------------------
section '.code' code readable executable
start:
        invoke printf, strIntro, dword[left], \
                    dword[left+4], dword[right], dword[right+4]

        finit                      ; initialise  coprocessor
        lp:
                MoveRight          ; move upper limit

                CompareToOne       ; compare 1 + right to 1

                ja makePrev        ; if 1 + right is greater than 1, we just move upper limit
                jz compareToPrev   ; if 1 + right equals 1, we compare current right-value with previous and next to keep looking for exact epsilon


finish:
        ; needed value is in prev
        invoke printf, strAns, dword[prev], \
                    dword[prev+4]

        ; find diference between found epsilon and actual
        fld [prev]
        fsub [actualEpsilon]
        fstp [tmp]
        invoke printf, strDiff, dword[tmp], \
                    dword[tmp+4]

        PrintExit
        call [getch]

        push 0
        call [ExitProcess]

; move value from right to prev and continue loop
makePrev:
        fld [right]
        fstp [prev]
        jmp lp

; compare values we got on previous iteration, current value and next
compareToPrev:
        GetNext
        fld [tmp]
        fcomp [right]
        fstsw ax          ; copy the Status Word containing the result to AX
        fwait             ; insure the previous instruction is completed
        sahf              ; transfer the condition codes to the CPU's flag register

        ; if next value will be less, we continue
        jb makePrev
        ; if we reached epsilon and values dont change, we finish
        jz finish
;-------------------------------------------------------------------------------------------------
section '.idata' import data readable
library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll',\
            user32,'USER32.DLL'

include 'api\user32.inc'
include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess',\
           HeapCreate,'HeapCreate',\
           HeapAlloc,'HeapAlloc'
    include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           sprintf, 'sprintf',\
           scanf, 'scanf',\
           getch, '_getch'