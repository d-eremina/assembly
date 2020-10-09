; Eremina Daria
; BSE196
; Variant 8
; B[i] = A[i] + 5, if A[i] > 5
; B[i] = A[i] - 5, if A[i] < -5
; else B[i] = 0

format PE console
entry start

include 'win32a.inc'
;--------------------------------------------------------------------------
section '.data' data readable writable

        strVecSize   db 'Enter size of vector: ', 0
        strIncorSize db 'Incorrect size of vector = %d', 10, 0
        strVecElemIn  db '[%d] is: ', 0
        strScanInt   db '%d', 0
        strVecElemOut  db '[%d] = %d', 10, 0
        strForExit   db '...Press any key to exit... ', 0
        strInfoVecA db 'Vector input: ', 10, 0
        strInfoVecB db 'Modified vector B: ', 10, 0

        vec_size     dd 0
        i            dd ?
        tmp          dd ?
        tmpStack     dd ?
        vecA         rd 100  ; input vector
        vecB         rd 100  ; modified vector
;--------------------------------------------------------------------------
section '.code' code readable executable
start:
; 1) calling input method for vector A
        call Input
; 2) creating new vector by given rules
        call Modify
; 3) printing vectors in console
        push strInfoVecA        ; informing user that at first we show vector A
        call [printf]
        add esp, 4

        mov ebx, vecA           ; moving vector A to register ebx for printing it
        call VectorOut

        push strInfoVecB        ; the same for another vector
        call [printf]
        add esp, 4

        mov ebx, vecB
        call VectorOut
; waiting for any key to be pressed before closing
finish:
        push strForExit
        call [printf]
        call [getch]

        push 0
        call [ExitProcess]

;-------------------------------- data input
Input:
        push strVecSize
        call [printf]
        add esp, 4

        push vec_size
        push strScanInt
        call [scanf]
        add esp, 8

        mov eax, [vec_size]
; size shpuld be in range [1, 100]
        cmp eax, 1
        jl wrongInput
        cmp eax, 100
        jg wrongInput
; if size is incorrect
        jmp getVector
; else continue with entering values
getVector:
        xor ecx, ecx            ; ecx = 0
        mov ebx, vecA           ; ebx = &vec
getVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        jge endInputVector      ; if we reached entered size - go to the end of the loop

        ; asking user to input another element
        mov [i], ecx
        push ecx
        push strVecElemIn
        call [printf]
        add esp, 8

        push ebx
        push strScanInt
        call [scanf]
        add esp, 8

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp getVecLoop
wrongInput:
        push [vec_size]
        push strIncorSize
        call [printf]
        call [getch]
        push 0
        call [ExitProcess]
; return if all elements entered
endInputVector:
        ret
;-------------------------------- procedure for modifying vector by given rule
proc Modify
        xor ecx, ecx            ; ecx = 0 to check number of element
        mov ebx, vecA           ; ebx = &vecA
        mov edx, vecB           ; edx = &vecB
VecLoop:
        cmp ecx, [vec_size]     ; check if we are at the end of vector
        je EndModify            ; if yes - end of loop

        mov eax, [ebx + 4*ecx]  ; get another element
        cmp eax, 5              ; compare current element to 5
        jg FirstCase            ; if it is greater, it is the first case - B[i] = A[i] + 5

        cmp eax, -5             ; if it is not, compare to -5
        jl SecondCase           ; if it is less, it is the second case - B[i] = A[i] - 5

        jmp OtherCase           ; if no case were entered, B[i] = 0, go to this case
; behavior in every possible case
; in every case we add a proper value or make it 0
; then we move it to vector B
FirstCase:
        add eax, 5
        mov [edx + 4*ecx], eax
        inc ecx
        jmp VecLoop
SecondCase:
        add eax, -5
        mov [edx + 4*ecx], eax
        inc ecx
        jmp VecLoop
OtherCase:
        xor eax, eax
        mov [edx + 4*ecx], eax
        inc ecx
        jmp VecLoop
; if we checked all elements
EndModify:
        ret
        endp
;-------------------------------- for printing vectors from register ebx
VectorOut:
        mov [tmpStack], esp
        xor ecx, ecx
putVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        je endOutputVector      ; if we reached entered size - go to the end of the loop
        mov [i], ecx

        ; output element
        push dword [ebx]
        push ecx
        push strVecElemOut
        call [printf]

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp putVecLoop
endOutputVector:
        mov esp, [tmpStack]
        ret
;------------------------------------------------------------
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
           scanf, 'scanf',\
           getch, '_getch'