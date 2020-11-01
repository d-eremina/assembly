macro GetNext {         ; finds midpoint of [left, right] and places result in tmp
      fld [left]        ; move left-value to st(0)
      fadd [right]      ; add right to value in st(0)
      fdiv [half]       ; same with divison
      fstp  [tmp]       ; moves result to tmp in cleares st(0)
}

macro MoveRight {       ; moves upper limit of segment
      ; the same as with GetNext but result is in right
      fld [left]
      fadd [right]
      fdiv [half]
      fstp  [right]
}

macro CompareToOne {    ; compares 1 + right to 1
      fld [one]
      fadd [right]
      fcomp [one]
      fstsw ax          ; copy the Status Word containing the result to AX
      fwait             ; insure the previous instruction is completed
      sahf              ; transfer the condition codes to the CPU's flag register
}

macro PrintExit {       ; prints string before exit
      push strForExit
      call [printf]
}