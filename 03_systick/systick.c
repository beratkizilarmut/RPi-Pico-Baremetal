// Copyright (c) 2023 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "definitions.h"

#define PUT32(address, value) (*((volatile unsigned int *)address)) = value
#define GET32(address) *(volatile unsigned int *)address

/* Setup XOSC and set it a source clock */
static void setupClocks(void)
{
    // Enable the XOSC
    PUT32( XOSC_CTRL, 0xAA0 );                       // Frequency range: 1_15MHZ
    PUT32( XOSC_STARTUP, 0xc4 );                     // Startup delay ( default value )
    PUT32( ( XOSC_CTRL | SET ), 0xFAB000 );          // Enable ( magic word )
    while( !( GET32( XOSC_STATUS ) & 0x80000000 ) ); // Oscillator is running and stable

    // Set the XOSC as source clock for REF, SYS and Periferals
    PUT32( CLK_REF_CTRL, 2 );                         // CLK REF source = xosc_clksrc
    PUT32( CLK_SYS_CTRL, 0 );                         // CLK SYS source = clk_ref
    PUT32( CLK_REF_DIV, ( 1 << 8 ) );                 // CLK REF Divisor = 1
    PUT32( CLK_PERI_CTRL, ( 1 << 11 ) | ( 4 << 5 ) ); // CLK PERI Enable & AUX SRC = xosc_clksrc
}

/* reset the subsystems used in this program */
static void resetSubsys()
{
    // Reset IO Bank
    PUT32( ( RESETS_RESET | CLR ), ( 1 << 5 ) );
    while( GET32( RESETS_RESET_DONE ) & ( 1 << 5 ) == 0 );
}


/* ***********************************************
 * Main function
 * ********************************************* */
__attribute__( ( used, section( ".boot.entry" ) ) ) int main( void )
{
    // Setup clocks (XOSC as source clk)
    setupClocks();
    // Reset Subsystems (IO / PADS and UART0)
    resetSubsys();

    // Set GPIO25 as SIO ( F5) and GPIO OE
    PUT32( ( IO_BANK0_GPIO25_CTRL ), 5 );
    PUT32( SIO_GPIO_OE_SET, ( 1 << 25 ) );

#define COUNT_250MS 12000000/4                         // with XOSC (12MHZ), 12,000,000 ticks = 1 second. -> 250ms = 3,000,000 ticks
    PUT32( CORTEX_SYST_RVR, COUNT_250MS );             // start counting down from COUNT_250MS (3,000,000 ticks)
    PUT32( CORTEX_SYST_CSR, ( 1 << 2 ) | ( 1 << 0 ) ); // source clock external / enable 
    while( 1 )
    {
        if ( GET32( CORTEX_SYST_CSR ) & ( 1 << 16 ) )  // Check for the count flag (count compleated)
        {
            PUT32( SIO_GPIO_OUT_XOR, ( 1 << 25 ) );    // XOR the LED pin
        }
    }

    return ( 0 );
}
