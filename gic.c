/* GIC(Generic Interrupt Controller) V2，最多支持8核 一共1020个中断，IMX6ULL一共128个中断，加上32个软件中断和私有中断 一共128+32=160个中断
 * ID0~ID15：这 16 个 ID 分配给 SGI(软件中断)
 * ID16~ID31：这 16 个 ID 分配给 PPI(私有中断)
 * ID32~ID1019：这 988 个 ID 分配给 SPI(共享中断) 
 */
#include "imx6ull.h"
#include "gic.h"
#include "common.h"
#include "uart.h"
struct irq_desc irqTable[Num];
static void default_isr(IRQn_Type gicciar ,void * userParam)
{
    //for(;;);
}
/* irqTable 初始，写一个默认值，防止空指针 */
static void irqTable_init(void)
{
    int i;
    for(i = 0; i < Num; i++)
    {
        irqTable[i].isr = default_isr;
        irqTable[i].data = NULL;
    }
}
/* 获取gic基地址 */
static GIC_Type * get_gic_base(void)
{
	GIC_Type *dst;

	__asm volatile ("mrc p15, 4, %0, c15, c0, 0" : "=r" (dst)); 

	return dst;
}
/* gic初始化 */
static void gic_init(void)
{
	u32 i, irq_num;

	GIC_Type *gic = get_gic_base();

	/* the maximum number of interrupt IDs that the GIC supports */
	irq_num = (gic->D_TYPER & 0x1F) + 1;

	/* On POR, all SPI is in group 0, level-sensitive and using 1-N model */
	
	/* Disable all PPI, SGI and SPI */
	for (i = 0; i < irq_num; i++)
	  gic->D_ICENABLER[i] = 0xFFFFFFFFUL;

	/* The priority mask level for the CPU interface. If the priority of an 
	 * interrupt is higher than the value indicated by this field, 
	 * the interface signals the interrupt to the processor.
	 */
	gic->C_PMR = (0xFFUL << (8 - 5)) & 0xFFUL;
	
	/* No subpriority, all priority level allows preemption */
	gic->C_BPR = 7 - 5;
	
	/* Enables the forwarding of pending interrupts from the Distributor to the CPU interfaces.
	 * Enable group0 distribution
	 */
	gic->D_CTLR = 1UL;
	
	/* Enables the signaling of interrupts by the CPU interface to the connected processor
	 * Enable group0 signaling 
	 */
	gic->C_CTLR = 1UL;
}

void gic_enable_irq(IRQn_Type nr)
{
	GIC_Type *gic = get_gic_base();

	/* The GICD_ISENABLERs provide a Set-enable bit for each interrupt supported by the GIC.
	 * Writing 1 to a Set-enable bit enables forwarding of the corresponding interrupt from the
	 * Distributor to the CPU interfaces. Reading a bit identifies whether the interrupt is enabled.
	 */
	gic->D_ISENABLER[nr >> 5] = (uint32_t)(1UL << (nr & 0x1FUL));
}
void gic_disable_irq(IRQn_Type nr)
{
	GIC_Type *gic = get_gic_base();

	/* The GICD_ICENABLERs provide a Clear-enable bit for each interrupt supported by the
	 * GIC. Writing 1 to a Clear-enable bit disables forwarding of the corresponding interrupt from
     * the Distributor to the CPU interfaces. Reading a bit identifies whether the interrupt is enabled. 
	 */
	gic->D_ICENABLER[nr >> 5] = (uint32_t)(1UL << (nr & 0x1FUL));
}

void int_init(void)
{
    gic_init();
    irqTable_init();
	__asm volatile ("cpsie i"); 		//开启cpsr中断位.arm复位后自动关闭I F位
}
/* 注册irq */
static int register_irq(IRQn_Type irq, IRQ_Handler_t irq_handler, void *data)
{
    if(irq >= 160)
	{
		return -1;
	}      	
    irqTable[irq].isr = irq_handler;
    irqTable[irq].data = data;
    return 0;
}
/* 申请irq,使能相应gic中断 */
int request_irq(IRQn_Type irq, IRQ_Handler_t irq_handler, void *data)
{
	int ret;
	ret = register_irq(irq, irq_handler, data);
	if(ret != 0)
		return -1;
	gic_enable_irq(irq);	/* 使能注册的中断 */
	return 0;
}
//int Handle_IRQ_c(unsigned int gicciar)
void Handle_IRQ_c(void)
{
	PutChar('a');
	unsigned int nr;
   // unsigned int GICC_IAR  = *(unsigned int *)gicciar;	
   // GICC_IAR &=  0x3ff;
   // if(GICC_IAR >= 160) 
	//	return ;	
	GIC_Type *gic = get_gic_base();
	nr = gic->C_IAR;
	nr &= 0x3ff;
    irqTable[nr].isr(nr, irqTable[nr].data);		/* 调用之前注册的irq回调函数 */
	gic->C_EOIR = nr;
    //*((unsigned int *)gicciar + 1) = GICC_IAR; 					 /* 写GICC_EOIR，向CPU表明中断已经处理完成 */
	//gic->C_EOIR	= GICC_IAR;
}