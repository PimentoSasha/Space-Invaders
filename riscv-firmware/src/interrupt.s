.section .text, "ax"
.global _interrupt_handler
_interrupt_handler:
save_thread_image:
    addi x2, x2, -64
    sw x1, 4(x2)
    sw x3, 12(x2)
    sw x4, 16(x2)
    sw x5, 20(x2)
    sw x6, 24(x2)
    sw x7, 28(x2)
    sw x8, 32(x2)
    sw x9, 36(x2)
    sw x10, 40(x2)
    sw x11, 44(x2)
    sw x12, 48(x2)
    sw x13, 52(x2)
    sw x14, 56(x2)
    sw x15, 60(x2)
    csrr x1, mepc
    sw x1, 0(x2)
    addi t1, gp, 0
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    la x1, _thread_status_sp_value
    sw x2, 0(x1) # Save stack pointer to common location for OS to store
    la x1, cart_gp_saved
    lw t0, 0(x1)
    bnez t0, skip_set_gp
    beqz t1, skip_set_gp
    li t0, 1
    sw t0, 0(x1)
    la x1, cart_global_pointer
    sw t1, 0(x1)
skip_set_gp:


check_if_ecall:
#    csrw    mscratch,x1
    csrr    x1,mcause
    addi    x1,x1,-11
    beqz    x1,_system_call
    csrr    a0,mcause
    la x1, os_stack_pointer
    lw sp, 0(x1)
    call    c_interrupt_handler
    li      a1, 0
    j       load_thread_image
_system_call:
    lw      t0, 0(x2)
    addi    t0, t0, 4
    sw      t0, 0(x2)
    mv      s0, a0 # save in case is finish thread
    la x1, os_stack_pointer
    lw sp, 0(x1)
    call    c_system_call
    addi    t0, s0, -21
    bnez    t0, continue
    addi    t0, s0, -8
    bnez    t0, continue
    beqz    a0, continue
    ret
continue:
    li      a1, 1

.global load_thread_image
load_thread_image:
    la x1, os_stack_pointer
    sw sp, 0(x1)
# don't restore a0 is system call
    la x1, _thread_status_sp_value
    lw x2, 0(x1)
    lw x3, 12(x2)
    lw x4, 16(x2)
    lw x5, 20(x2)
    lw x6, 24(x2)
    lw x7, 28(x2)
    lw x8, 32(x2)
    lw x9, 36(x2)
    bnez a1, skip_restore_a0 # Don't restore a0 on a syscall
    lw x10, 40(x2) # I hope this is a0 lol, my boss defintely told me once it doesn't matter if you use x* or ABI name, just don't intermix them, sorry scott
skip_restore_a0:
    lw x11, 44(x2)
    lw x12, 48(x2)
    lw x13, 52(x2)
    lw x14, 56(x2)
    lw x15, 60(x2)
    lw x1, 0(x2)
    csrw mepc, x1
    lw x1, 4(x2)
    addi x2, x2, 64

    mret

.globl  start_threads
start_threads:
    li t0, 0x80 # MPIE
    csrs mstatus, t0
    ret

.globl end_thread
end_thread:
    li a5, 21
    ecall
	
.globl  _thread_status_sp_value, os_stack_pointer, cart_global_pointer, cart_gp_saved
.data
cart_global_pointer:
    .long 0
cart_gp_saved:
    .long 0
_thread_status_sp_value:
    .long 0
os_stack_pointer:
    .long 0
