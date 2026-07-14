make_rect PROC
    mov eax, start_x
    mov edx, end_x
    cmp eax, edx
    jle x_ready
    xchg eax, edx
x_ready:
    mov rect_data, eax
    mov rect_data+8, edx
    mov eax, start_y
    mov edx, end_y
    cmp eax, edx
    jle y_ready
    xchg eax, edx
y_ready:
    mov rect_data+4, eax
    mov rect_data+12, edx
    ret
make_rect ENDP

draw_rect PROC
    push ebx
    mov ebx, [esp+8]
    push ebx
    call DWORD PTR [__imp__GetDC@4]
    push eax
    push OFFSET rect_data
    push eax
    call DWORD PTR [__imp__DrawFocusRect@8]
    pop eax
    push eax
    push ebx
    call DWORD PTR [__imp__ReleaseDC@8]
    pop ebx
    ret 4
draw_rect ENDP

cleanup_capture PROC
    cmp h_memory, 0
    je capture_clean
    push h_old
    push h_memory
    call DWORD PTR [__imp__SelectObject@8]
    push h_capture
    call DWORD PTR [__imp__DeleteObject@4]
    push h_memory
    call DWORD PTR [__imp__DeleteDC@4]
    push h_screen
    push 0
    call DWORD PTR [__imp__ReleaseDC@8]
    mov h_memory, 0
capture_clean:
    ret
cleanup_capture ENDP

cleanup_result PROC
    cmp h_result_dc, 0
    je result_clean
    push h_pen_old
    push h_result_dc
    call DWORD PTR [__imp__SelectObject@8]
    push h_result_old
    push h_result_dc
    call DWORD PTR [__imp__SelectObject@8]
    push h_result
    call DWORD PTR [__imp__DeleteObject@4]
    push h_result_dc
    call DWORD PTR [__imp__DeleteDC@4]
    mov h_result_dc, 0
    mov h_result, 0
result_clean:
    push esi
    xor esi, esi
undo_clean:
    cmp esi, undo_count
    jge clean_redo
    push undo_stack[esi*4]
    call DWORD PTR [__imp__DeleteObject@4]
    inc esi
    jmp undo_clean
clean_redo:
    xor esi, esi
redo_clean:
    cmp esi, redo_count
    jge history_clean
    push redo_stack[esi*4]
    call DWORD PTR [__imp__DeleteObject@4]
    inc esi
    jmp redo_clean
history_clean:
    mov undo_count, 0
    mov redo_count, 0
    pop esi
    ret
cleanup_result ENDP

copy_clipboard PROC
    push ebx
    cmp h_result, 0
    je clipboard_done
    push 0
    push result_h
    push result_w
    push 0
    push h_result
    call DWORD PTR [__imp__CopyImage@20]
    mov ebx, eax
    test eax, eax
    jz clipboard_done
    push h_main
    call DWORD PTR [__imp__OpenClipboard@4]
    test eax, eax
    jz clipboard_delete
    call DWORD PTR [__imp__EmptyClipboard@0]
    push ebx
    push 2
    call DWORD PTR [__imp__SetClipboardData@8]
    call DWORD PTR [__imp__CloseClipboard@0]
    jmp clipboard_done
clipboard_delete:
    push ebx
    call DWORD PTR [__imp__DeleteObject@4]
clipboard_done:
    pop ebx
    ret
copy_clipboard ENDP

store_result PROC
    push ebx
    push edi
    mov ebx, rect_data+8
    sub ebx, rect_data
    jle store_done
    mov edi, rect_data+12
    sub edi, rect_data+4
    jle store_done
    call cleanup_result
    mov result_w, ebx
    mov result_h, edi
    push h_screen
    call DWORD PTR [__imp__CreateCompatibleDC@4]
    mov h_result_dc, eax
    push edi
    push ebx
    push h_screen
    call DWORD PTR [__imp__CreateCompatibleBitmap@12]
    mov h_result, eax
    push eax
    push h_result_dc
    call DWORD PTR [__imp__SelectObject@8]
    mov h_result_old, eax
    push h_pen
    push h_result_dc
    call DWORD PTR [__imp__SelectObject@8]
    mov h_pen_old, eax
    push 00CC0020h
    push rect_data+4
    push rect_data
    push h_memory
    push edi
    push ebx
    push 0
    push 0
    push h_result_dc
    call DWORD PTR [__imp__BitBlt@36]
    call copy_clipboard
store_done:
    pop edi
    pop ebx
    ret
store_result ENDP

cancel_capture PROC
    cmp h_overlay, 0
    je no_overlay
    push h_overlay
    call DWORD PTR [__imp__DestroyWindow@4]
    mov h_overlay, 0
no_overlay:
    call cleanup_capture
    push 5
    push h_main
    call DWORD PTR [__imp__ShowWindow@8]
    push h_main
    call DWORD PTR [__imp__SetForegroundWindow@4]
    ret
cancel_capture ENDP

finish_capture PROC
    push h_overlay
    call DWORD PTR [__imp__DestroyWindow@4]
    mov h_overlay, 0
    call cleanup_capture
    push 5
    push h_main
    call DWORD PTR [__imp__ShowWindow@8]
    push 1
    push 0
    push h_main
    call DWORD PTR [__imp__InvalidateRect@12]
    push h_main
    call DWORD PTR [__imp__SetForegroundWindow@4]
    ret
finish_capture ENDP

begin_capture PROC
    push 76
    call DWORD PTR [__imp__GetSystemMetrics@4]
    mov virtual_x, eax
    push 77
    call DWORD PTR [__imp__GetSystemMetrics@4]
    mov virtual_y, eax
    push 78
    call DWORD PTR [__imp__GetSystemMetrics@4]
    mov virtual_w, eax
    push 79
    call DWORD PTR [__imp__GetSystemMetrics@4]
    mov virtual_h, eax
    push 0
    call DWORD PTR [__imp__GetDC@4]
    mov h_screen, eax
    push eax
    call DWORD PTR [__imp__CreateCompatibleDC@4]
    mov h_memory, eax
    push virtual_h
    push virtual_w
    push h_screen
    call DWORD PTR [__imp__CreateCompatibleBitmap@12]
    mov h_capture, eax
    push eax
    push h_memory
    call DWORD PTR [__imp__SelectObject@8]
    mov h_old, eax
    push 40CC0020h
    push virtual_y
    push virtual_x
    push h_screen
    push virtual_h
    push virtual_w
    push 0
    push 0
    push h_memory
    call DWORD PTR [__imp__BitBlt@36]
    push 0
    push h_instance
    push 0
    push 0
    push virtual_h
    push virtual_w
    push virtual_y
    push virtual_x
    push 90000000h
    push 0
    push OFFSET class_overlay
    push 8
    call DWORD PTR [__imp__CreateWindowExA@48]
    mov h_overlay, eax
    test eax, eax
    jz capture_failed
    push eax
    call DWORD PTR [__imp__SetForegroundWindow@4]
    ret
capture_failed:
    call cancel_capture
    ret
begin_capture ENDP
