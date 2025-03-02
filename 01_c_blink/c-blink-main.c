// Copyright (c) 2023 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)

#define PUT32(address,value)  (*((volatile unsigned int*)address))=value
#define GET32(address) *(volatile unsigned int*)address

#define EXAMPLE_GPIO_PIN_NUM 25

int main(void)
{
        PUT32( 0x4000f000, ( 1 << 5 ) ); // IO BANK

        while ( GET32( 0x4000c008 ) & ( 1<< 5 ) == 0 ); // Reset Done?

        PUT32( 0x40014004 /* GPIO0_CTRL */ + (8 /* Address difference between two CTRL registers */ * EXAMPLE_GPIO_PIN_NUM), 0x05 );  // IO PAD = FUNC 5 (GPIO)
        PUT32( 0xd0000020, ( 1 << EXAMPLE_GPIO_PIN_NUM ) ); // GPIO_OE

        while( 1 )
        {
                PUT32( 0xd000001c, ( 1 << EXAMPLE_GPIO_PIN_NUM ) ); // XOR GPIO
                for ( volatile unsigned int a = 50000; a > 0; a-- );
        }

        return 0;
}
