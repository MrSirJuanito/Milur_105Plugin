#include <stdio.h>
#define  P_16        0xA001

static unsigned short   crcTab16[ 256 ];

void initCrc16Tab( void )
{
    for (unsigned int i = 0; i < 256; ++i )
    {
        unsigned short crc = 0, c = i;
        for (unsigned int j = 0; j < 8; ++j )
        {
            if ( ( crc ^ c ) & 0x0001 )
            {
                crc = ( crc >> 1 ) ^ P_16;
            }
            else
            {
                crc =   crc >> 1;
            }
            c = c >> 1;
        }
        crcTab16[ i ] = crc;
    }
}

unsigned short updateCrc_16( unsigned short crc, char c )
{
    unsigned short tmp, shortC;
    shortC = 0x00ff & ( unsigned short ) c;
    tmp =  crc         ^ shortC;
    crc = ( crc >> 8 ) ^ crcTab16[ tmp & 0xff ];
    return crc;
}
