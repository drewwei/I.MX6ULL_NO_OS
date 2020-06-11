#ifndef  _IMX6ULL_H
#define  _IMX6ULL_H

//#define GPIO5_BASE                                   0x20ac000
#define CCM_CCGR1                                    0x20c406c
#define IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3      0x2290014
#define IOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER3      0x2290058
#define rCCM_CCGR1                                  (*(volatile unsigned int *)CCM_CCGR1)
#define rIOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3    (*(volatile unsigned int *)IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3)
#define rIOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER3    (*(volatile unsigned int *)IOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER3)
#define rIOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER1     (*(volatile unsigned int *)0x229000c)
#define rIOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER1     (*(volatile unsigned int *)0x2290050)





typedef struct {
   unsigned int GPIO_DR;
   unsigned int GPIO_GDIR;
   unsigned int GPIO_PSR;
   unsigned int GPIO_ICR1;
   unsigned int GPIO_ICR2;
   unsigned int GPIO_IMR;
   unsigned int GPIO_ISR;
   unsigned int GPIO_EDGE_SEL;
}GPIO_Group_t;






#define   LOW       0
#define   HIGH      1


#endif

#ifndef    NULL
#define    NULL       0
#endif