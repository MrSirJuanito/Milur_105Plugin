#ifndef CRC
#define CRC
/*!
 * \brief initCrc16Tab
 * инициализация таблицы CRC
 */
void initCrc16Tab( void );

/*!
 * \brief updateCrc_16
 * \param crc
 * \param c
 * \return
 */
unsigned short updateCrc_16( unsigned short crc, char c );
#endif // CRC

