format  PE console 4.0
 
include 'win32a.inc'
start:
;считывание числа1
    cinvoke printf, text1
    cinvoke scanf, spp, x
;считывание числа2
    cinvoke printf, text2
    cinvoke scanf, spp, y
;вывод чисел
    cinvoke printf, text3, [x]
    cinvoke printf, text4, [y]
;сложение
    mov eax, [x]
    add eax, [y]
    cinvoke printf, text5, eax
;вычитание
    mov eax, [x]
    sub eax, [y]
    cinvoke printf, text6, eax
;умножение
    mov eax, [x]
    imul    eax, [y]
    cinvoke printf, text7, eax
;деление
    mov eax, [x]
    cdq
    idiv    [y]
    cinvoke printf, text8, eax
    cinvoke scanf, spp, x
    invoke  exit,0
 
text1   db 'Enter X: ',0
text2   db 'Enter Y: ',0
text3   db 10,13,'The X number: %d',0
text4   db 10,13,'The Y number: %d',0
text5   db 10,13,'X+Y= %d',0
text6   db 10,13,'X-Y= %d',0
text7   db 10,13,'X*Y= %d',0
text8   db 10,13,'X/Y= %d',0
spp db '%d',0
x   dd ?
y   dd ?

 data import
 
library msvcrt,'msvcrt.dll'
 
import  msvcrt,\
    printf,'printf',\
    scanf,'scanf',\
    exit,'exit'
end data