OverlayProc PROC
    push ebp
    mov ebp, esp
    mov eax, [ebp+12]
    cmp eax, 000Fh
    je overlay_paint
    cmp eax, 0014h
    je overlay_erase
    cmp eax, 0201h
    je button_down
    cmp eax, 0200h
    je mouse_move
    cmp eax, 0202h
    je button_up
    cmp eax, 0100h
    je key_down
    cmp eax, 0204h
    je overlay_cancel
    cmp eax, 0010h
    je overlay_cancel
overlay_default:
    push [ebp+20]
    push [ebp+16]
    push [ebp+12]
    push [ebp+8]
    call DWORD PTR [__imp__DefWindowProcA@16]
    pop ebp
    ret 16
overlay_paint:
    push OFFSET paint_data
    push [ebp+8]
    call DWORD PTR [__imp__BeginPaint@8]
    push 00CC0020h
    push 0
    push 0
    push h_memory
    push virtual_h
    push virtual_w
    push 0
    push 0
    push eax
    call DWORD PTR [__imp__BitBlt@36]
    push OFFSET paint_data
    push [ebp+8]
    call DWORD PTR [__imp__EndPaint@8]
overlay_erase:
    mov eax, 1
    pop ebp
    ret 16
button_down:
    mov eax, [ebp+20]
    movsx edx, ax
    mov start_x, edx
    mov end_x, edx
    sar eax, 16
    mov start_y, eax
    mov end_y, eax
    mov selecting, 1
    push [ebp+8]
    call DWORD PTR [__imp__SetCapture@4]
    jmp overlay_handled
mouse_move:
    cmp selecting, 0
    je overlay_handled
    call make_rect
    push [ebp+8]
    call draw_rect
    mov eax, [ebp+20]
    movsx edx, ax
    mov end_x, edx
    sar eax, 16
    mov end_y, eax
    call make_rect
    push [ebp+8]
    call draw_rect
    jmp overlay_handled
button_up:
    cmp selecting, 0
    je overlay_handled
    mov eax, [ebp+20]
    movsx edx, ax
    mov end_x, edx
    sar eax, 16
    mov end_y, eax
    call make_rect
    push [ebp+8]
    call draw_rect
    mov selecting, 0
    call DWORD PTR [__imp__ReleaseCapture@0]
    call store_result
    call finish_capture
    jmp overlay_handled
key_down:
    cmp DWORD PTR [ebp+16], 27
    jne overlay_handled
overlay_cancel:
    call cancel_capture
overlay_handled:
    xor eax, eax
    pop ebp
    ret 16
OverlayProc ENDP
