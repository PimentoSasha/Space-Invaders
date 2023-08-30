.section .init, "ax"
.global _start
_start:
    .cfi_startproc
    .cfi_undefined ra
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
#    la sp, __stack_top
#    add s0, sp, zero
    jal ra, init
    nop
    jal zero, main
    .cfi_endproc
    

.section .text, "ax"
.global getTicks, getControllerStatus, displayWriteText, displayAddSmallSprite, displayUpdateSmallSprite, displayAddLargeSprite, displayUpdateLargeSprite, addBackground, getCmdBtnStatus, rand, srand, removeBackground, removeLargeSprite, removeSmallSprite, clearScreen, yield, kill, join, wait, thread, setCmdListener, setOnClickListener, setOnChangedListener, setOnReleasedListener, pthread_mutex_lock, pthread_mutex_unlock
getTicks:
    li a5, 0
    ecall
    ret
getControllerStatus:
    li a5, 1
    ecall
    ret
displayWriteText:
    li a5, 2
    ecall
    ret
displayAddSmallSprite:
    li a5, 3
    ecall
    ret
displayUpdateSmallSprite:
    li a5, 4
    ecall
    ret
displayAddLargeSprite:
    li a5, 5
    ecall
    ret
displayUpdateLargeSprite:
    li a5, 6
    ecall
    ret
addBackground:
    li a5, 7
    ecall
    ret
getCmdBtnStatus:
    li a5, 8
    ecall
    ret
rand:
    li a5, 9
    ecall
    ret
srand:
    li a5, 10
    ecall
    ret
removeLargeSprite:
    li a5, 11
    ecall
    ret
removeSmallSprite:
    li a5, 12
    ecall
    ret
removeBackground:
    li a5, 13
    ecall
    ret
clearScreen:
    li a5, 14
    ecall
    ret
setOnClickListener:
    li a5, 15
    ecall
    ret
setOnReleasedListener:
    li a5, 16
    ecall
    ret
setOnChangedListener:
    li a5, 17
    ecall
    ret
setCmdListener:
    li a5, 18
    ecall
    ret
yield:
    li a5, 19
    ecall
    ret
join:
    li a5, 20
    ecall
    ret
# don't use 21
# finish_thread:
#    li a5, 21
#    ecall
kill:
    li a5, 22
    ecall
    ret
thread:
    li a5, 23
    ecall
    ret
wait:
    li a5, 24
    ecall
    ret
pthread_mutex_lock:
    li a5, 25
    ecall
    ret
pthread_mutex_unlock:
    li a5, 26
    ecall
    ret
.end
