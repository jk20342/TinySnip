MainProc PROC
    push ebp
    mov ebp, esp
    mov eax, [ebp+12]
    cmp eax, 000Fh
    je main_paint
    cmp eax, 0111h
    je main_command
    cmp eax, 002Bh
    je main_draw_item
    cmp eax, 0201h
    je main_draw_start
    cmp eax, 0200h
    je main_draw_move
    cmp eax, 0202h
    je main_draw_end
    cmp eax, 0002h
    je main_destroy
main_default:
    push [ebp+20]
    push [ebp+16]
    push [ebp+12]
    push [ebp+8]
    call DWORD PTR [__imp__DefWindowProcA@16]
    pop ebp
    ret 16
main_paint:
    push OFFSET paint_data
    push [ebp+8]
    call DWORD PTR [__imp__BeginPaint@8]
    mov paint_dc, eax
    cmp drawing, 0
    jne surface_ready
    mov DWORD PTR rect_data, 8
    mov DWORD PTR rect_data+4, 8
    mov DWORD PTR rect_data+8, 532
    mov DWORD PTR rect_data+12, 322
    push h_surface
    push OFFSET rect_data
    push paint_dc
    call DWORD PTR [__imp__FillRect@12]
surface_ready:
    push 1
    push paint_dc
    call DWORD PTR [__imp__SetBkMode@8]
    push 00EBEBEBh
    push paint_dc
    call DWORD PTR [__imp__SetTextColor@8]
    push h_font
    push paint_dc
    call DWORD PTR [__imp__SelectObject@8]
    cmp h_result_dc, 0
    je paint_empty
    call calc_preview
    push 3
    push paint_dc
    call DWORD PTR [__imp__SetStretchBltMode@8]
    push 00CC0020h
    push result_h
    push result_w
    push 0
    push 0
    push h_result_dc
    push preview_h
    push preview_w
    push preview_y
    push preview_x
    push paint_dc
    call DWORD PTR [__imp__StretchBlt@44]
    jmp paint_finished
paint_empty:
    push 25h
    push OFFSET rect_data
    push -1
    push OFFSET empty_text
    push paint_dc
    call DWORD PTR [__imp__DrawTextA@20]
paint_finished:
    push OFFSET paint_data
    push [ebp+8]
    call DWORD PTR [__imp__EndPaint@8]
    jmp main_handled
main_command:
    mov eax, [ebp+16]
    and eax, 0FFFFh
    cmp eax, 1
    je command_new
    cmp eax, 2
    je command_copy
    cmp eax, 3
    je command_save
    cmp eax, 4
    je command_exit
    cmp eax, 5
    je command_delay
    cmp eax, 6
    je command_undo
    cmp eax, 7
    je command_redo
    jmp main_handled
command_new:
    mov eax, delay_index
    push eax
    push 0
    push h_main
    call DWORD PTR [__imp__ShowWindow@8]
    call DWORD PTR [__imp__DwmFlush@0]
    push 200
    call DWORD PTR [__imp__Sleep@4]
    pop eax
    cmp eax, 1
    je wait_three
    cmp eax, 2
    je wait_five
    jmp start_snip
wait_three:
    push 3000
    call DWORD PTR [__imp__Sleep@4]
    jmp start_snip
wait_five:
    push 5000
    call DWORD PTR [__imp__Sleep@4]
start_snip:
    call begin_capture
    jmp main_handled
command_copy:
    call copy_clipboard
    jmp main_handled
command_save:
    call save_result
    jmp main_handled
command_undo:
    call undo_drawing
    jmp main_handled
command_redo:
    call redo_drawing
    jmp main_handled
command_exit:
    push h_main
    call DWORD PTR [__imp__DestroyWindow@4]
    jmp main_handled
command_delay:
    inc delay_index
    cmp delay_index, 3
    jb delay_value
    mov delay_index, 0
delay_value:
    mov eax, OFFSET delay_now
    cmp delay_index, 1
    jne delay_not_three
    mov eax, OFFSET delay_3
delay_not_three:
    cmp delay_index, 2
    jne delay_text_ready
    mov eax, OFFSET delay_5
delay_text_ready:
    push eax
    push 0
    push 0Ch
    push h_delay
    call DWORD PTR [__imp__SendMessageA@16]
    jmp main_handled
main_draw_item:
    mov eax, [ebp+20]
    mov draw_item_ptr, eax
    mov edx, h_surface
    cmp DWORD PTR [eax+4], 1
    jne item_state
    mov edx, h_accent
item_state:
    test DWORD PTR [eax+16], 1
    jz item_brush_ready
    mov edx, h_pressed
item_brush_ready:
    mov draw_brush, edx
    add eax, 28
    push edx
    push eax
    mov eax, draw_item_ptr
    push DWORD PTR [eax+24]
    call DWORD PTR [__imp__FillRect@12]
    mov eax, draw_item_ptr
    push 1
    push DWORD PTR [eax+24]
    call DWORD PTR [__imp__SetBkMode@8]
    mov eax, draw_item_ptr
    push 00FFFFFFh
    push DWORD PTR [eax+24]
    call DWORD PTR [__imp__SetTextColor@8]
    mov eax, draw_item_ptr
    push h_font
    push DWORD PTR [eax+24]
    call DWORD PTR [__imp__SelectObject@8]
    mov eax, draw_item_ptr
    push 64
    push OFFSET draw_text
    push DWORD PTR [eax+20]
    call DWORD PTR [__imp__GetWindowTextA@12]
    mov eax, draw_item_ptr
    add eax, 28
    push 25h
    push eax
    push -1
    push OFFSET draw_text
    mov eax, draw_item_ptr
    push DWORD PTR [eax+24]
    call DWORD PTR [__imp__DrawTextA@20]
    mov eax, 1
    pop ebp
    ret 16
main_draw_start:
    cmp h_result, 0
    je main_handled
    mov eax, [ebp+20]
    call map_point
    jnc main_handled
    mov draw_x, eax
    mov draw_y, edx
    call snapshot_undo
    mov drawing, 1
    push h_main
    call DWORD PTR [__imp__SetCapture@4]
    jmp main_handled
main_draw_move:
    cmp drawing, 0
    je main_handled
    mov eax, [ebp+20]
    call map_point
    jnc main_handled
    call draw_segment
    jmp main_handled
main_draw_end:
    cmp drawing, 0
    je main_handled
    mov drawing, 0
    call DWORD PTR [__imp__ReleaseCapture@0]
    call copy_clipboard
    jmp main_handled
main_destroy:
    call cleanup_result
    push h_pen
    call DWORD PTR [__imp__DeleteObject@4]
    push h_background
    call DWORD PTR [__imp__DeleteObject@4]
    push h_surface
    call DWORD PTR [__imp__DeleteObject@4]
    push h_accent
    call DWORD PTR [__imp__DeleteObject@4]
    push h_pressed
    call DWORD PTR [__imp__DeleteObject@4]
    push 0
    call DWORD PTR [__imp__PostQuitMessage@4]
main_handled:
    xor eax, eax
    pop ebp
    ret 16
MainProc ENDP

start:
    push 0
    push OFFSET gdip_input
    push OFFSET gdip_token
    call DWORD PTR [__imp__GdiplusStartup@12]
    push 000000FFh
    push 3
    push 0
    call DWORD PTR [__imp__CreatePen@12]
    mov h_pen, eax
    push 001B1818h
    call DWORD PTR [__imp__CreateSolidBrush@4]
    mov h_background, eax
    push 002A2727h
    call DWORD PTR [__imp__CreateSolidBrush@4]
    mov h_surface, eax
    push 00D47800h
    call DWORD PTR [__imp__CreateSolidBrush@4]
    mov h_accent, eax
    push 009E5A00h
    call DWORD PTR [__imp__CreateSolidBrush@4]
    mov h_pressed, eax
    push 17
    call DWORD PTR [__imp__GetStockObject@4]
    mov h_font, eax
    push 0
    call DWORD PTR [__imp__GetModuleHandleA@4]
    mov h_instance, eax
    mov DWORD PTR wc+16, eax
    push 32515
    push 0
    call DWORD PTR [__imp__LoadCursorA@8]
    mov DWORD PTR wc+24, eax

    mov DWORD PTR wc+4, OFFSET OverlayProc
    mov DWORD PTR wc+36, OFFSET class_overlay
    push OFFSET wc
    call DWORD PTR [__imp__RegisterClassA@4]
    mov DWORD PTR wc+4, OFFSET MainProc
    mov eax, h_background
    mov DWORD PTR wc+28, eax
    mov DWORD PTR wc+36, OFFSET class_main
    push OFFSET wc
    call DWORD PTR [__imp__RegisterClassA@4]

    push 0
    push h_instance
    push 0
    push 0
    push 420
    push 560
    push 80000000h
    push 80000000h
    push 10CA0000h
    push OFFSET app_title
    push OFFSET class_main
    push 0
    call DWORD PTR [__imp__CreateWindowExA@48]
    mov h_main, eax
    push 4
    push OFFSET dark_mode
    push 20
    push eax
    call DWORD PTR [__imp__DwmSetWindowAttribute@16]

    push 5000000Bh
    push 1
    push 28
    push 80
    push 340
    push 10
    push OFFSET class_button
    push OFFSET text_new
    call create_control
    push 5000000Bh
    push 2
    push 28
    push 65
    push 340
    push 95
    push OFFSET class_button
    push OFFSET text_copy
    call create_control
    push 5000000Bh
    push 3
    push 28
    push 65
    push 340
    push 165
    push OFFSET class_button
    push OFFSET text_save
    call create_control
    push 5000000Bh
    push 6
    push 28
    push 65
    push 340
    push 235
    push OFFSET class_button
    push OFFSET text_undo
    call create_control
    push 5000000Bh
    push 7
    push 28
    push 65
    push 340
    push 305
    push OFFSET class_button
    push OFFSET text_redo
    call create_control
    push 5000000Bh
    push 5
    push 28
    push 100
    push 340
    push 375
    push OFFSET class_button
    push OFFSET delay_now
    call create_control
    mov h_delay, eax
    push 5000000Bh
    push 4
    push 28
    push 55
    push 340
    push 480
    push OFFSET class_button
    push OFFSET text_exit
    call create_control

    push h_main
    call DWORD PTR [__imp__SetForegroundWindow@4]

message_loop:
    push 0
    push 0
    push 0
    push OFFSET msg_data
    call DWORD PTR [__imp__GetMessageA@16]
    test eax, eax
    jle shutdown
    push OFFSET msg_data
    call DWORD PTR [__imp__TranslateMessage@4]
    push OFFSET msg_data
    call DWORD PTR [__imp__DispatchMessageA@4]
    jmp message_loop
shutdown:
    push gdip_token
    call DWORD PTR [__imp__GdiplusShutdown@4]
    push 0
    call DWORD PTR [__imp__ExitProcess@4]
