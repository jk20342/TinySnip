.data
class_overlay db "TinySnipOverlay",0
class_main    db "TinySnipMain",0
class_button  db "BUTTON",0
class_combo   db "COMBOBOX",0
app_title     db "Tiny Snip",0
text_new      db "New Snip",0
text_copy     db "Copy",0
text_save     db "Save",0
text_undo     db "Undo",0
text_redo     db "Redo",0
text_exit     db "Exit",0
delay_now     db "Delay: Now",0
delay_3       db "Delay: 3 sec",0
delay_5       db "Delay: 5 sec",0
empty_text    db "Click New Snip to capture your screen",0
draw_text     db 64 dup(0)
image_filter  db "PNG (*.png)",0,"*.png",0
              db "JPEG (*.jpg)",0,"*.jpg",0
              db "Bitmap (*.bmp)",0,"*.bmp",0,0
png_ext       db "png",0

png_clsid     dd 0557CF406h
              dw 01A04h,011D3h
              db 09Ah,073h,0,0,0F8h,01Eh,0F3h,02Eh
jpeg_clsid    dd 0557CF401h
              dw 01A04h,011D3h
              db 09Ah,073h,0,0,0F8h,01Eh,0F3h,02Eh
bmp_clsid     dd 0557CF400h
              dw 01A04h,011D3h
              db 09Ah,073h,0,0,0F8h,01Eh,0F3h,02Eh

wc            db 40 dup(0)
msg_data      db 28 dup(0)
paint_data    db 64 dup(0)
rect_data     dd 4 dup(0)
preview_bounds dd 8,8,532,322
ofn           db 76 dup(0)
file_path     db 260 dup(0)
wide_path     dw 260 dup(0)
gdip_input    dd 1,0,0,0
gdip_token    dd 0
gp_image      dd 0

h_instance    dd 0
h_main        dd 0
h_overlay     dd 0
h_delay       dd 0
delay_index   dd 0
h_screen      dd 0
h_memory      dd 0
h_capture     dd 0
h_old         dd 0
h_result_dc   dd 0
h_result      dd 0
h_result_old  dd 0
undo_stack    dd 8 dup(0)
redo_stack    dd 8 dup(0)
undo_count    dd 0
redo_count    dd 0
h_pen         dd 0
h_pen_old     dd 0
h_background  dd 0
h_surface     dd 0
h_accent      dd 0
h_pressed     dd 0
h_font        dd 0
draw_item_ptr dd 0
draw_brush    dd 0
dark_mode     dd 1
result_w      dd 0
result_h      dd 0
preview_x     dd 10
preview_y     dd 10
preview_w     dd 0
preview_h     dd 0
draw_x        dd 0
draw_y        dd 0
mapped_x      dd 0
mapped_y      dd 0
drawing       dd 0
virtual_x     dd 0
virtual_y     dd 0
virtual_w     dd 0
virtual_h     dd 0
start_x       dd 0
start_y       dd 0
end_x         dd 0
end_y         dd 0
selecting     dd 0
paint_dc      dd 0

