TI_GDT equ 000b
RPL0 equ 00b
SELECTOR_VIDEO equ (0x0003<<3)+TI_GDT+RPL0
[bits 32]
section .text:
global put_char
put_char:
    pushad
    mov ax,SELECTOR_VIDEO
    mov gs,ax
    ;获取光标位置
    mov dx,0x03d4
    mov al,0xe
    out dx,al
    mov dx,0x03d5
    in al,dx
    mov ah,al

    mov dx,0x03d4
    mov al,0xf
    out dx,al
    mov dx,0x03d5
    in al,dx

    mov bx,ax
    ;判断是否为控制字符
    mov ecx,[esp+36]
    cmp ecx,0xd
    jz .is_carriage_return
    cmp ecx,0xa
    jz .is_line_feed
    cmp ecx,0x8
    jz .is_backspace

    .valid_char:
        shl bx,1
        mov [gs:bx], cl
        inc bx
        mov byte [gs:bx], 0x7
        shr bx,1
        inc bx
        cmp bx, 2000
        jl .set_cursor
        jmp .roll_screen
    .is_line_feed:
    .is_carriage_return:
        xor dx,dx
        mov ax,bx
        mov si,80
        div si
        sub bx,dx
        cmp ecx,0xd
        jz .set_cursor
        add bx,80
        cmp bx,2000
        jl .set_cursor
        jmp .roll_screen

    .is_backspace:
        dec bx
        shl bx,1
        mov byte [gs:bx],0x20
        inc bx
        mov byte [gs:bx],0x7
        shr bx,1
        jmp .set_cursor

    .roll_screen:
        ;拷贝
        cld
        mov ecx,960
        mov esi,0xc00b80a0
        mov edi,0xc00b8000
        rep movsd

        mov ecx,80
        mov ebx,3840
        .cls:
            mov word [gs:ebx], 0x0720
            add ebx,2
            loop .cls
        mov bx,1920

    .set_cursor:
        mov dx,0x03d4
        mov al,0x0e
        out dx,al
        mov dx,0x03d5
        mov al,bh
        out dx,al

        mov dx,0x03d4
        mov al,0x0f
        out dx,al
        mov dx,0x03d5
        mov al,bl
        out dx,al

    .put_done:
        popad
        ret

global put_str
put_str:
    push eax
    push ebx
    xor ebx,ebx
    mov eax,[esp+12]
    .loop_char:
        mov bl, byte [eax]
        cmp bl,0x0
        jz .put_str_done
        push ebx
        call put_char
        add esp,4
        inc eax
        jmp .loop_char

    .put_str_done:
        pop ebx
        pop eax
        ret

global put_int
put_int:
    push eax
    push edx
    push ecx
    push esi
    xor ecx,ecx
    mov eax ,[esp+20]
    mov esi,16
    .stack_int:
        xor edx,edx
        div esi
        cmp edx,0xa
        jl .num
        .letter:
            sub edx,0xa
            add edx,0x61
            push edx
            jmp .is_zero
        .num:
            add edx,0x30
            push edx
        .is_zero:
            inc ecx
            cmp eax,0x0
            jz .loop_int
            jmp .stack_int
    .loop_int:
        call put_char
        add esp,4
        loop .loop_int

    .put_int_done:
        pop esi
        pop ecx
        pop edx
        pop eax
        ret
















