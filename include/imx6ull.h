#ifndef  _IMX6ULL_H
#define  _IMX6ULL_H

#define GPIO5_BASE                                   0x20ac000
#define CCM_CCGR1                                    0x20c406c
#define IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3      0x2290014
#define IOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER3      0x2290058
#define rCCM_CCGR1                                  (*(volatile unsigned int *)CCM_CCGR1)
#define rIOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3    (*(volatile unsigned int *)IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3)
#define rIOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER3    (*(volatile unsigned int *)IOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER3)






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





#define   INPUT     0
#define   OUTPUT    1
#define   LOW       0
#define   HIGH      1
#define   NULL       (void(*)(void))0
#endif