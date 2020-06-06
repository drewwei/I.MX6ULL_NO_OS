.text
.global     _start
_start:
_vector_table:    @设置异常向量表
    b Reset_Handler  /* 用b 跳转，是为了防止DDR还未初始化，就使用连接地址,但DDR已经被NXP在启动ROM里面初始化了 */
Undefined_Handler:
    ldr pc, =Undefined_Handler
SVC_Handler:
    ldr pc, =SVC_Handler
PrefAbort_Handler:
    ldr pc, =PrefAbort_Handler
DataAbort_Handler:
    ldr pc, =DataAbort_Handler
Reseved:
    .word 0
    ldr pc, =IRQ_Handler
FIQ_Handler:
    ldr pc, =FIQ_Handler

Reset_Handler:
    /* @关看门狗,系统默认关闭 */
    /* 读厂商ID */
    mrc p15, 0, r0, c0, c0, 0       /* 读协处理器cp15 MIDR寄存器的bit31:24 */
    /* 设置_vector_table base address,它等于链接地址 通过操作cp15协处理器c12寄存器*/
    ldr r0, =0x80100000             /* 链接脚本中的链接地址 */
    mcr p15, 0, r0, c12, c0, 0      /* 将 r0 里面的数据写入到 c12 中，即 c12=0x80100000 */
    /* Reset SCTLR（系统控制寄存器） SettinRgs 操作cp15系处理器c1寄存器  */
    mrc p15, 0, r0, c1, c0, 0       /* read SCTRL, Read CP15 System Control register */
    bic r0, r0, #(1 << 13)          /* Clear V bit 13 to use normal exception vectors */
    bic r0, r0, #(1 << 12)          /* Clear C bit 12 to disable I Cache */
    bic r0, r0, #(1 << 2)           /* Clear A bit  2 to disable D Cache */
    bic r0, r0, #(1 << 11)          /* Clear Z bit 11 to disable strict alignment */
    bic r0, r0, #(1 << 0)           /* Clear M bit  0 to disable MMU */
    mcr p15, 0, r0, c1, c0, 0	    /* write SCTRL, Write to CP15 System Control register	*/

    /* @使能所有时钟 */
    bl enable_clock
    @使能DDR,NXP已经初始化
    /* 进入IRQ模式  下列指令可以用 cps #0x12 代替 */
    mrs r0, cpsr
    bic r0, r0, #0x1f    
    orr r0, r0, #0x12        
    msr cpsr, r0    
    ldr sp, =0x80400000     @设置IRQ模式的栈

    /* 进入管理模式 */
    mrs r0, cpsr
    bic r0, r0, #0x1f    
    orr r0, r0, #0x13        
    msr cpsr, r0    
    ldr sp, =0x80200000     @设置SVC模式的栈

    bl  clean_bss           @清BSS段
    //bl  test_led
    bl  main                @跳入C函数

halt:
    b halt

clean_bss:
    ldr r1, =__bss_start
    ldr r2, =__bss_end
    mov r0, #0
clean:
    str r0, [r1], #1            @ 加1更好
    cmp r1, r2
    ble clean                   @ 有可能__bss_start和__bss_end为同一个地址，所以r1+4后永远不可能等于r2，所以不要用bne跳转
    mov pc, lr                  @ 这句不要丢了

enable_clock:
     @使能全部时钟
    ldr r0, =0x20ac000
    ldr r2, =(0x20ac000+4*6)
    ldr r1, = 0xffffffff
1:
    str r1, [r0], #4
    cmp r0, r2
    bne    1b 
    mov pc, lr

IRQ_Handler:

    sub lr, lr, #4
    stmdb sp!, {r0-r12, lr}     /* 将r0到r12,lr保存在IRQ模式下的栈里面 */

    mrc p15, 4, r0, c15, c0, 0  /* 通过读出cp15 c15寄存器获取 GIC 基础地址，基地址保存在r0中,r0作为Handle_IRQ_c的一个参数*/
    bl Handle_IRQ_c             /* 调用c函数处理中断 */

    ldmia sp!, {r0-r12, pc}^    /* 将保存在IRQ下的栈里的寄存器r0到r12恢复 */



/*测试用，1s为周期循环亮灭LED*/
test_led:
    @设置电气属性
    ldr r0, =0x2290058
    ldr r1, [r0]
    bic r1, r1, #(7<<3)
    orr r1, r1, #(5<<3)
    str r1, [r0]
    @设置复用功能：GPIO功能
    ldr r0, =0x2290014
    ldr r1, [r0]
    bic r1, r1, #0xf
    orr r1, r1, #0x5
    str r1, [r0]
    @设置为输入输出模式：输入
    ldr r0, =0x20ac004
    ldr r1, [r0]
    orr r1, r1, #(1<<3)
    str r1, [r0]
    @设置GPIO输出高低电平
loop:
    ldr r0, =0x20ac000
    mov r3, #0
    mov r2, #0x320000
    ldr r1, [r0]
    bic r1, r1, #(1<<3)
    str r1, [r0]
2:   
    add r3, r3, #1
    cmp r2, r3
    bne 2b

    mov r2, #0x320000
    mov r3, #0
    ldr r1, [r0]
    orr r1, r1, #(1<<3)
    str r1, [r0]
3:
    add r3, r3, #1
    cmp r2, r3
    bne 3b

    b loop


    
