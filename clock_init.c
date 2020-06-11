#define CCM_CCSR                (*(volatile unsigned int *)0X20C400C)
#define CCM_ANALOG_PLL_ARM      (*(volatile unsigned int *)0X20C8000)
#define CCM_CACRR               (*(volatile unsigned int *)0x20C4010)
#define CCM_ANALOG_PFD_528      (*(volatile unsigned int *)0X20C8100)
#define CCM_ANALOG_PFD_480      (*(volatile unsigned int *)0X20C80F0)
#define CCM_CBCMR               (*(volatile unsigned int *)0X20C4018)
#define CCM_CBCMR1              (*(volatile unsigned int *)0X20C401C)
#define CCM_CBCDR               (*(volatile unsigned int *)0X20C4014)
#define CCM_CDHIPR              (*(volatile unsigned int *)0X20C4048)
#define CCM_CSCDR1              (*(volatile unsigned int *)0X20C4024)
/* 初始化PLL2 PFD0-PFD3 */
static void PLL2_PFD_init(void)
{
    unsigned int reg = CCM_ANALOG_PFD_528;
    reg &= ~(0XBFBFBFBF) ;

    /* PDF 初始化 */
    reg |= (27<<0); /* PFD0 = 352MHz */
    reg |= (16<<8); /* PDF1 = 594MHz */
    reg |= (24<<16);/* PDF2 = 396MHz */
    reg |= (48<<24);/* PDF3 = 198MHz */
    CCM_ANALOG_PFD_528 |= reg;
}
/* 初始化AHB_CLK_ROOT */
static void AHB_CLK_ROOT_init(void)
{
    CCM_CBCMR &= ~(3<<18);
    CCM_CBCMR |=  (2<<18);  /* PRE_PERIPH_CLK_SEL多路选择器选中PLL2 PFD2 */
    CCM_CBCDR &= ~(1<<25);  /* PERIPH_CLK_SEL多路选择器选择PLL2 */
    CCM_CBCDR &= ~(7<<10);
    CCM_CBCDR |= (2<<10);   /* AHB_PODF 分频设置为3分频 AHB_CLK_ROOT = 396/3=132MHz*/
    while((CCM_CDHIPR&(1<<1)) != 0); /* 循环等待ahb_podf busy */
}
/* 初始化PERCLK_CLK_ROOT */
static void PERCLK_CLK_ROOT_init(void)
{
    CCM_CBCDR &= ~(3<<8);
    CCM_CBCDR |= (1<<8);    /* Divider for ipg podf 2分频 IPG_CLK_ROOT= 132/2=66MHz */

    CCM_CBCMR1 &= ~(0x7f<<0); /* 1.derive clock from ipg clk root; 2.divide by 1 PERCLK_CLK_ROOT=IPG_CLK_ROOT=66MHz */

}

/* 初始化PLL3 PFD0-PFD3 */
static void PLL3_PFD_init(void)
{
    unsigned int reg = CCM_ANALOG_PFD_480;
    reg &= ~(0XBFBFBFBF) ;

    /* PDF 初始化 */
    reg |= (12<<0); /* PFD0 = 720MHz */
    reg |= (16<<8); /* PDF1 = 540MHz */
    reg |= (17<<16);/* PDF2 = 396MHz */
    reg |= (19<<24);/* PDF3 = 198MHz */
    CCM_ANALOG_PFD_480 |= reg;
}
/* 设置uart时钟树 */
static void UART_CLK_ROOT_init(void)
{
    CCM_CSCDR1 &= ~(1<<6);  /* 0 derive clock from pll3_80m */
    CCM_CSCDR1 &= ~(0x3F);  /* Divider for  uart clock podf: 000000 divide by 1 */
}

void imx6ull_clock_init(void)
{
    /* 1.设置ARM PLL之前切换时钟源为step_clk */
    CCM_CCSR &= ~(1<<8);     /* 选择step_clk的时钟来源外部24MHz晶振*/

    CCM_CCSR |= (1<<2);     /* 选择step_clk时钟源产生pll1_sw_clk ，即切换到step_clk*/
    /* 2.设置ARM PLL 分频 */
    CCM_ANALOG_PLL_ARM &= ~(0X7F);
    CCM_ANALOG_PLL_ARM |= (1<<13) | (88<<0);    /* 1.使能时钟输出；2.设置ARM PLL频率为528*2MHz */

    while((CCM_ANALOG_PLL_ARM  & (1<<31)) == 0); /* 循环等待ARM PLL锁定 */
    /* 3.切换回时钟源 */
    CCM_CCSR &= ~(1<<2);     /* 选择pll1_main_clk时钟源产生pll1_sw_clk ，即切换到pll1_main_clk*/
    /* 4.设置ARM_CLK_ROOT 分频供ARM core使用 */
    CCM_CACRR &= ~(0x7<<0);   
    CCM_CACRR |= (1<<0);      /* 设置ARM_CLK_ROOT 分频为2分频 arm clock = 1056/2=528MHz */      
    while((CCM_CDHIPR & (1<<16)) != 0); /* 循环等待ARM_PODF_BUSY */

    PLL2_PFD_init();
    AHB_CLK_ROOT_init();
    PERCLK_CLK_ROOT_init();
    PLL3_PFD_init();
    UART_CLK_ROOT_init();
}

