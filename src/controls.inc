create_control PROC
    push ebp
    mov ebp, esp
    push 0
    push 0
    push [ebp+32]
    push h_main
    push [ebp+28]
    push [ebp+24]
    push [ebp+20]
    push [ebp+16]
    push [ebp+36]
    push [ebp+8]
    push [ebp+12]
    push 0
    call DWORD PTR [__imp__CreateWindowExA@48]
    pop ebp
    ret 32
create_control ENDP
