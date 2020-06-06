/* GIC(Global Interrupt Controller) V.2 一共1020个中断，IMX6ULL一共128个中断，加上32个软件中断和私有中断 一共128+32=160个中断
 * ID0~ID15：这 16 个 ID 分配给 SGI(软件中断)
 * ID16~ID31：这 16 个 ID 分配给 PPI(私有中断)
 * ID32~ID1019：这 988 个 ID 分配给 SPI(共享中断) 
 */




void Handle_IRQ_c(unsigned int gicbase)
{

}