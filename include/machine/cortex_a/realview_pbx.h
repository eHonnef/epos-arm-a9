// EPOS Realview_PBX (ARM Cortex-A9) MCU Mediator Declarations

#ifndef __realviewpbx_h
#define __realviewpbx_h

#include <architecture/cpu.h>
#include <architecture/tsc.h>
#include <machine/rtc.h>
#include <system.h>

__BEGIN_SYS

class Realview_PBX
{
    friend class TSC;

protected:
    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;

public:
    static const unsigned int IRQS = 96;
    static const unsigned int TIMERS = 4; // 4 timers
    static const unsigned int UARTS = 4; // 4
    static const unsigned int GPIO_PORTS = 3; // 3
    static const bool supports_gpio_power_up = false;

// Base addresses for memory-mapped I/O devices
// https://wiki.osdev.org/User:Pancakes/arm_qemu_realview-pb-a e https://github.com/qemu/qemu/blob/master/hw/arm/realview.c
    enum {
        SYSREG_BASE                 = 0x10000000, // system registers
        // SYSLOCK_BASE                = 0x10000020, // System lock (0xA05F to unlock)
        SCR_BASE                    = 0x10001000, // System Control (pode ser tambem 0x1001A000 ?)
        AACI_BASE                   = 0x10004000, // aaci
        WDT0_BASE                   = 0x10010000, // Watchdog Timer
        GPIOA_BASE                  = 0x10013000, // PrimeCell PL061 GPIO (aka General Purpose Input/Output)
        GPIOB_BASE                  = 0x10014000, // PrimeCell PL061 GPIO
        GPIOC_BASE                  = 0x10015000, // PrimeCell PL061 GPIO
        USART_BASE                  = 0x1000D000, // PrimeCell PL022 Synchronous Serial Port (ssp)
        UART0_BASE                  = 0x10009000, // PrimeCell PL011 UART
        UART1_BASE                  = 0x1000A000, // PrimeCell PL011 UART
        UART2_BASE                  = 0x1000B000, // PrimeCell PL011 UART
        UART3_BASE                  = 0x1000C000, // PrimeCell PL011 UART
        TIMER0_BASE                 = 0x10011000, // GPTM (aka general purpose timer module)
        TIMER1_BASE                 = 0x10011020, // GPTM
        TIMER2_BASE                 = 0x10012000, // GPTM
        TIMER3_BASE                 = 0x10012020, // GPTM
        PER_BASE                    = 0x1F000000, // peripheal base
        PRIV_TIMER_BASE             = 0x1F000600, // cpu private timer
        GLOBAL_TIMER_BASE           = 0x1F000200, // global timer
        DIST_BASE                   = 0x1f001000, // gic distributor
        CPU_ITF_BASE                = 0x1f000100, // private gic
        RTC_BASE                    = 0x10017000, // rtc
        FLASH0_BASE                 = 0x18000300, // Flash Controller (pode ser tambem a 0x1000004C)
        FLASHCFG_BASE               = 0x4c000000, // Flash CONFIGURATION
        GIC1_BASE                   = 0x1e000000,
        GIC2_BASE                   = 0x1e001000,
        GIC3_BASE                   = 0x1e002000,
        GIC4_BASE                   = 0x1e003000,
    };

    // Offsets para PER_BASE
    enum {
        SCUOFF = 0x0000,	// snoop control unit
        GICOFF = 0x0100,	// private general interrupt controller
        GTIOFF = 0x0200,	// global timer
        PTIOFF = 0x0600,	// private timer
        GDIOFF = 0x1000,	// GIC distributor
    };

    // Offsets para SYSREG_BASE
    enum {
        SYS_ID = 0x0000,
        SYS_PROCID0 = 0x0084,
        SYS_PROCID1 = 0x0088,
        SYS_OSC0 = 0x000c,
        SYS_OSC1 = 0x0010,
        SYS_OSC2 = 0x0014,
        SYS_OSC3 = 0x0018,
        SYS_OSC4 = 0x001c,
    };

    // Register addresses relative to base
    enum {                                      // Description              Type    Value after reset
        CONTROL_REG0                = 0x00,     // Control                  r/w     0x00000128
        MODE_REG0                   = 0x04,     // Mode                     r/w     0x00000000
        INTRPT_EN_REG0              = 0x08,     // Interrupt enable         r/w     0x00000000
        INTRPT_DIS_REG0             = 0x0C,     // Interrupt disable        r/w     0x00000000
        BAUD_RATE_GEN_REG0          = 0x18,     // Baud rate generator      r/w     0x0000028B
        RCVR_FIFO_TRIGGER_LEVEL0    = 0x20,     // Receive trigger level    r/w     0x00000020
        CHANNEL_STS_REG0            = 0x2C,     // Channel status           ro      0x00000000
        TX_RX_FIFO0                 = 0x30,     // Transmit/receive FIFO    r/w     0x00000000
        BAUD_RATE_DIVIDER_REG0      = 0x34      // Baud rate divider        r/w     0x0000000F
    };

    // Useful bits in the CONTROL_REG0 register
    enum {                                      // Description              Type    Value after reset
        RXRES                       = 1 << 0,   // Reset Rx data path       r/w     0
        TXRES                       = 1 << 1,   // Reset Tx data path       r/w     0
        RXEN                        = 1 << 2,   // Receive enable           r/w     0
        TXEN                        = 1 << 4    // Transmit enable          r/w     0
    };

    // Useful bits in the MODE_REG0 register
    enum {                                      // Description              Type    Value after reset
        CHRL8                       = 0 << 1,   // Character Length 8 bits  r/w     0
        CHRL7                       = 2 << 1,   // Character Length 7 bits  r/w     0
        CHRL6                       = 3 << 1,   // Character Length 6 bits  r/w     0
        PAREVEN                     = 0 << 3,   // Even parity              r/w     0
        PARODD                      = 1 << 3,   // Odd parity               r/w     0
        PARNONE                     = 4 << 3,   // No parity                r/w     0
        NBSTOP2                     = 2 << 6,   // 2 stop bits              r/w     0
        NBSTOP1                     = 0 << 6,   // 1 stop bit               r/w     0
        CHMODENORM                  = 0 << 8,   // Normal mode              r/w     0
        CHMODELB                    = 2 << 8    // Loopback mode            r/w     0
    };

    // Useful bits in the INTRPT_EN_REG0, and INTRPT_DIS_REG0 registers
    enum {                                      // Description              Type    Value after reset
        INTRPT_RTRIG                = 1 << 0,   // Receiver FIFO empty      wo      0
        INTRPT_TTRIG                = 1 << 10   // Transmitter FIFO trigger wo      0
    };

    // Useful bits in the CHANNEL_STS_REG0 register
    enum {                                      // Description              Type    Value after reset
        STS_RTRIG                   = 1 << 0,   // Receiver FIFO trigger    ro      0
        STS_TFUL                    = 1 << 4    // Transmitter FIFO full    ro      0
    };

    // SLCR Registers offsets
    enum {                                      // Description
        SLCR_LOCK                   = 0x020,    // Lock the SLCR
        SLCR_UNLOCK                 = 0x020,    // Unlock the SLCR
        UART_CLK_CTRL               = 0x154,    // UART Ref Clock Control
        FPGA0_CLK_CTRL              = 0x170,    // PL Clock 0 Output control
        PSS_RST_CTRL                = 0x200,    // PS Software Reset Control
        FPGA_RST_CTRL               = 0x240     // FPGA Software Reset Control
    };

    // Useful bits in SLCR_LOCK
    enum {                                      // Description                  Type    Value after reset
        LOCK_KEY                    = 0x767B    // Lock key                     wo      0
    };

    // Useful bits in SLCR_UNLOCK
    enum {                                      // Description                  Type    Value after reset
        UNLOCK_KEY                  = 0xA05F    // Unlock key                   wo      0
    };

    // Useful bits in FPGAN_CLK_CTRL
    enum {                                      // Description                  Type    Value after reset
        DIVISOR0                    = 1 << 8,   // First cascade divider        r/w     0x18
        DIVISOR1                    = 1 << 20   // Second cascade divider       r/w     0x1
    };

    // CPU Interface Registers offsets
    enum {                                      // Description
        ICCICR                      = 0x000,    // CPU Interface Control
        ICCPMR                      = 0x004,    // Interrupt Priority Mask
        ICCIAR                      = 0x00C,    // Interrupt Ack
        ICCEOIR                     = 0x010     // End Of Interrupt
    };

    // Useful bits in ICCICR
    enum {                                      // Description                  Type    Value after reset
        ITF_EN_S                    = 1 << 0,   // Enable secure signaling      r/w     0
        ITF_EN_NS                   = 1 << 1,   // Enable non-secure signaling  r/w     0
        ACK_CTL                     = 1 << 2    // Acknowledge control          r/w     0
    };

    // Global Timer Registers offsets (ok, checado)
    enum {                                      // Description
        GTCTRL                      = 0x00,     // Low Counter
        GTCTRH                      = 0x04,     // High Counter
        GTCLR                       = 0x08,     // Control
        GTISR                       = 0x0C      // Interrupt Status
    };

    // Private Timer Registers offsets (ok, checado)
    enum {                                      // Description
        PTLR                        = 0x00,     // Load
        PTCTR                       = 0x04,     // Counter
        PTCLR                       = 0x08,     // Control
        PTISR                       = 0x0C      // Interrupt Status
    };

    // Useful bits in PTCLR
    enum {                                      // Description                  Type    Value after reset
        TIMER_ENABLE                = 1 << 0,   // Enable                       r/w     0
        AUTO_RELOAD                 = 1 << 1,   // Auto reload                  r/w     0
        IRQ_EN                      = 1 << 2    // Enable interrupt             r/w     0
    };

    // Useful bits in PTISR
    enum {                                      // Description                  Type    Value after reset
        INT_CLR                     = 1 << 0    // Interrupt clear bit          r/w     0
    };

    // Distributor Registers offsets
    enum {                                      // Description
        ICDDCR                      = 0x000,    // Distributor Control
        ICDISER0                    = 0x100,    // Interrupt Set-Enable
        ICDISER1                    = 0x104,    // Interrupt Set-Enable
        ICDISER2                    = 0x108,    // Interrupt Set-Enable
        ICDICER0                    = 0x180,    // Interrupt Clear-Enable
        ICDICER1                    = 0x184,    // Interrupt Clear-Enable
        ICDICER2                    = 0x188     // Interrupt Clear-Enable
    };

    // Useful bits in ICDDCR
    enum {                                      // Description                  Type    Value after reset
        DIST_EN_S                   = 1 << 0    // Enable secure interrupts     r/w     0
    };

protected:
    Realview_PBX() {}

    static void reboot() {
        // ¯\_(ツ)_/¯
        //slcr(PSS_RST_CTRL) = 1;
    }

    static void delay(const RTC::Microsecond & time) {
        assert(Traits<TSC>::enabled);
        TSC::Time_Stamp end = TSC::time_stamp() + time * (TSC::frequency() / 1000000);
        while(end > TSC::time_stamp());
    }

    static const UUID & uuid() { return System::info()->bm.uuid; }

    static unsigned int cpu_id() {
        int id;
        ASM("mrc p15, 0, %0, c0, c0, 5"
            : "=r"(id)
            : : );
        return id & 0x3;
    }

    // PM
    static void power_uart(unsigned int unit, const Power_Mode & mode) {
        assert(unit < UARTS);
        switch(mode) {
        case FULL:
            slcr(UART_CLK_CTRL) |= 1 << unit;
            break;
        case LIGHT:
            break;
        case SLEEP:
            break;
        case OFF:
            slcr(UART_CLK_CTRL) &= ~(1 << unit);
            break;
        case ENROLL: break;
        case DISMISS: break;
        case SAME: break;
        }
    }

    static unsigned int cpu_get_periphbase() {
        int result;
        asm ("mrc p15, #4, %0, c15, c0, #0" : "=r" (result));
        return result;
    }


    static void power_user_timer(unsigned int unit, const Power_Mode & mode) {
        assert(unit < UARTS);
        switch(mode) {
        case FULL:break;
        case LIGHT:break;
        case SLEEP:break;
        case OFF:break;
        case ENROLL: break;
        case DISMISS: break;
        case SAME: break;
        }
    }

    static void unlock_slcr() { slcr(SLCR_UNLOCK) = UNLOCK_KEY; }
    static void lock_slcr() { slcr(SLCR_LOCK) = LOCK_KEY; }

    static void enable_uart(unsigned int unit) {
        uart0(0x38) |= (1<<4); // enable UART RXIM interrupt
    }

    // timers estao inicializados
    static void init_machine() {
        init_scu();
        init_mmu();
        init_gic();
    }
    static void init_scu() {
        ASM(
            // Read peripheral base address
            "MRC p15, 4, r0, c15, c0, 0       \n"
            // read SCU Control Register
            "LDR r1, [r0]                     \n"
            // set bit0 (Enable bit) to 1 
            "ORR r1, r1, #0x1                 \n"
            // write back modified value
            "STR r1, [r0]                     \n"
            "BX lr \n"
            : :
        );
    }
    static void init_gic() {}

    static void init_mmu() {}

public:
    static volatile Reg32 & slcr(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(SYSREG_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & cpu_itf(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(CPU_ITF_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & global_timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GLOBAL_TIMER_BASE)[o / sizeof(Reg32)]; }
    // static volatile Reg32 & global_timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(cpu_get_periphbase() + GTIOFF)[o / sizeof(Reg32)]; }
    // static volatile Reg32 & priv_timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(cpu_get_periphbase() + PTIOFF)[o / sizeof(Log_Addr)]; }
    static volatile Reg32 & priv_timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(PRIV_TIMER_BASE)[o / sizeof(Log_Addr)]; }
    static volatile Reg32 & dist(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(DIST_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & uart0(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(UART0_BASE)[o / sizeof(Reg32)]; }

// protected:
    static void pre_init();
    static void init();
};

typedef Realview_PBX Machine_Model;

__END_SYS

#endif
