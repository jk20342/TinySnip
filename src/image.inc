calc_preview PROC
    mov eax, result_w
    mov preview_w, eax
    mov eax, result_h
    mov preview_h, eax
    cmp preview_w, 520
    ja preview_scale
    cmp preview_h, 310
    jbe preview_center
preview_scale:
    mov eax, result_w
    imul eax, 310
    mov edx, result_h
    imul edx, 520
    cmp eax, edx
    jg preview_width_limited
    mov preview_h, 310
    mov eax, result_w
    imul eax, 310
    xor edx, edx
    div result_h
    mov preview_w, eax
    jmp preview_center
preview_width_limited:
    mov preview_w, 520
    mov eax, result_h
    imul eax, 520
    xor edx, edx
    div result_w
    mov preview_h, eax
preview_center:
    mov eax, 520
    sub eax, preview_w
    shr eax, 1
    add eax, 10
    mov preview_x, eax
    mov eax, 310
    sub eax, preview_h
    shr eax, 1
    add eax, 10
    mov preview_y, eax
    ret
calc_preview ENDP

map_point PROC
    movsx ecx, ax
    sar eax, 16
    cmp ecx, preview_x
    jl map_invalid
    cmp eax, preview_y
    jl map_invalid
    mov edx, preview_x
    add edx, preview_w
    cmp ecx, edx
    jge map_invalid
    mov edx, preview_y
    add edx, preview_h
    cmp eax, edx
    jge map_invalid
    sub ecx, preview_x
    sub eax, preview_y
    push eax
    mov eax, ecx
    imul eax, result_w
    xor edx, edx
    div preview_w
    mov mapped_x, eax
    pop eax
    imul eax, result_h
    xor edx, edx
    div preview_h
    mov mapped_y, eax
    mov eax, mapped_x
    mov edx, mapped_y
    stc
    ret
map_invalid:
    clc
    ret
map_point ENDP

invalidate_preview PROC
    push 0
    push OFFSET preview_bounds
    push h_main
    call DWORD PTR [__imp__InvalidateRect@12]
    ret
invalidate_preview ENDP

clear_redo PROC
    push esi
    xor esi, esi
clear_redo_loop:
    cmp esi, redo_count
    jge clear_redo_done
    push redo_stack[esi*4]
    call DWORD PTR [__imp__DeleteObject@4]
    inc esi
    jmp clear_redo_loop
clear_redo_done:
    mov redo_count, 0
    pop esi
    ret
clear_redo ENDP

snapshot_undo PROC
    push esi
    call clear_redo
    cmp undo_count, 8
    jne snapshot_copy
    push undo_stack
    call DWORD PTR [__imp__DeleteObject@4]
    xor esi, esi
shift_undo:
    cmp esi, 7
    jge shift_done
    mov eax, undo_stack[esi*4+4]
    mov undo_stack[esi*4], eax
    inc esi
    jmp shift_undo
shift_done:
    mov undo_count, 7
snapshot_copy:
    push 0
    push result_h
    push result_w
    push 0
    push h_result
    call DWORD PTR [__imp__CopyImage@20]
    test eax, eax
    jz snapshot_done
    mov ecx, undo_count
    mov undo_stack[ecx*4], eax
    inc undo_count
snapshot_done:
    pop esi
    ret
snapshot_undo ENDP

undo_drawing PROC
    cmp undo_count, 0
    je undo_done
    push esi
    dec undo_count
    mov ecx, undo_count
    mov esi, undo_stack[ecx*4]
    push esi
    push h_result_dc
    call DWORD PTR [__imp__SelectObject@8]
    mov ecx, redo_count
    mov redo_stack[ecx*4], eax
    inc redo_count
    mov h_result, esi
    call invalidate_preview
    call copy_clipboard
    pop esi
undo_done:
    ret
undo_drawing ENDP

redo_drawing PROC
    cmp redo_count, 0
    je redo_done
    push esi
    dec redo_count
    mov ecx, redo_count
    mov esi, redo_stack[ecx*4]
    push esi
    push h_result_dc
    call DWORD PTR [__imp__SelectObject@8]
    mov ecx, undo_count
    mov undo_stack[ecx*4], eax
    inc undo_count
    mov h_result, esi
    call invalidate_preview
    call copy_clipboard
    pop esi
redo_done:
    ret
redo_drawing ENDP

draw_segment PROC
    push 0
    push draw_y
    push draw_x
    push h_result_dc
    call DWORD PTR [__imp__MoveToEx@16]
    push mapped_y
    push mapped_x
    push h_result_dc
    call DWORD PTR [__imp__LineTo@12]
    mov eax, mapped_x
    mov draw_x, eax
    mov eax, mapped_y
    mov draw_y, eax
    call invalidate_preview
    ret
draw_segment ENDP

save_result PROC
    push esi
    cmp h_result, 0
    je save_done
    mov DWORD PTR file_path, 0
    mov DWORD PTR ofn, 76
    mov eax, h_main
    mov DWORD PTR ofn+4, eax
    mov DWORD PTR ofn+12, OFFSET image_filter
    mov DWORD PTR ofn+24, 1
    mov DWORD PTR ofn+28, OFFSET file_path
    mov DWORD PTR ofn+32, 260
    mov DWORD PTR ofn+52, 802h
    mov DWORD PTR ofn+60, OFFSET png_ext
    push OFFSET ofn
    call DWORD PTR [__imp__GetSaveFileNameA@4]
    test eax, eax
    jz save_done
    push 260
    push OFFSET wide_path
    push -1
    push OFFSET file_path
    push 0
    push 0
    call DWORD PTR [__imp__MultiByteToWideChar@24]
    test eax, eax
    jz save_done
    mov esi, OFFSET png_clsid
    cmp DWORD PTR ofn+24, 2
    jne not_jpeg
    mov esi, OFFSET jpeg_clsid
not_jpeg:
    cmp DWORD PTR ofn+24, 3
    jne encoder_ready
    mov esi, OFFSET bmp_clsid
encoder_ready:
    push h_result_old
    push h_result_dc
    call DWORD PTR [__imp__SelectObject@8]
    push OFFSET gp_image
    push 0
    push h_result
    call DWORD PTR [__imp__GdipCreateBitmapFromHBITMAP@12]
    push eax
    push h_result
    push h_result_dc
    call DWORD PTR [__imp__SelectObject@8]
    pop eax
    test eax, eax
    jnz save_done
    push 0
    push esi
    push OFFSET wide_path
    push gp_image
    call DWORD PTR [__imp__GdipSaveImageToFile@16]
    push gp_image
    call DWORD PTR [__imp__GdipDisposeImage@4]
save_done:
    pop esi
    ret
save_result ENDP
