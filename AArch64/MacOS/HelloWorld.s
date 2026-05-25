.global _main

.text
_main:
    mov x0, #1                   // 1 = stdout
    adrp x1, msg@PAGE            // Load msg page
    add x1, x1, msg@PAGEOFF      // Add msg offset
    adrp x10, EOF@PAGE           // Load EOF page
    add x10, x10, EOF@PAGEOFF    // ADD EOF offset
    sub x2, x10, x1              // end - start = length
    
    ldr x16, =0x2000004          // write syscall on mac (0x2000000 + syscall)
    svc #0                       // Execute syscall

    mov x0, #0                   // exit status 0
    ldr x16, =0x2000001          // exit syscall
    svc #0

.align 2
.data
msg: 
    .ascii "Hello world\n"
EOF:
    .word 0
