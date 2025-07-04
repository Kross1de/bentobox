[BITS 16]

section .text
    global _L8000_ap_trampoline
    extern initial_pml
    extern ap_startup
    extern gdtr
    extern mmu_alloc

_L8000_ap_trampoline:
    cli
    cld
    jmp 0x00:0x8040
    align 16
_L8010_pmode_gdtr:
    dw _L8040_pmode_gdt_end - _L8016_pmode_gdt_base - 1
    dd _L8016_pmode_gdt_base
; https://github.com/9xbt/upOS/blob/18196da99b1b7dd67f3dacfdbddda417174ebfad/boot/menu.S
_L8016_pmode_gdt_base:
    ; null segment
    dw 0x0000       ; Limit
    dw 0x0000       ; Base (low 16 bits)
    db 0x00         ; Base (mid 8 bits)
    db 00000000b    ; Access
    db 00000000b    ; Granularity
    db 0x00         ; Base (high 8 bits)

    ; 32-bit code
    dw 0xFFFF       ; Limit
    dw 0x0000       ; Base (low 16 bits)
    db 0x00         ; Base (mid 8 bits)
    db 10011011b    ; Access
    db 11001111b    ; Granularity
    db 0x00         ; Base (high 8 bits)

    ; 32-bit data
    dw 0xFFFF       ; Limit
    dw 0x0000       ; Base (low 16 bits)
    db 0x00         ; Base (mid 8 bits)
    db 10010011b    ; Access
    db 11001111b    ; Granularity
    db 0x00         ; Base (high 8 bits)
    align 64
_L8040_pmode_gdt_end:
    lgdt [0x8010]

    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp 0x08:0x8060
    align 32
[BITS 32]
_L8060_ap_pmode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    jmp ap_init_paging

ap_init_paging:
    mov eax, cr4
    or eax, 1 << 5  ; PAE bit
    mov cr4, eax

    mov eax, initial_pml
    mov cr3, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x100   ; LME bit
    wrmsr

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    lgdt [gdtr]

    jmp 0x08:gdt3

[BITS 64]

gdt3:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov rax, cr0
    and ax, 0xFFFB  ; CR0.EM
    or ax, 0x2      ; CR0.MP
    mov cr0, rax
    mov rax, cr4
    or ax, 3 << 9   ; CR4.OSFXSR and CR4.OSXMMEXCPT
    mov cr4, rax

    mov rsp, ap_stack
    mov rdi, 4
    call mmu_alloc
    add rax, 16384  ; 16kb stack
    mov rsp, rax

    jmp ap_startup

section .bss

align 16
resb 128  ; enough to get mmu_alloc working
ap_stack: