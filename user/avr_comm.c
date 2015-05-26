#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "driver/spi.h"
#include "driver/spi_register.h"

#include "debug.h"

#include "avr_comm.h"

LOCAL void ICACHE_FLASH_ATTR
SPI_begin () {
     spi_master_init(HSPI);
     WRITE_PERI_REG(SPI_CLOCK(HSPI),
		    (((0x40)&SPI_CLKDIV_PRE)<<SPI_CLKDIV_PRE_S)|
		    ((3&SPI_CLKCNT_N)<<SPI_CLKCNT_N_S)|
		    ((1&SPI_CLKCNT_H)<<SPI_CLKCNT_H_S)|
		    ((3&SPI_CLKCNT_L)<<SPI_CLKCNT_L_S));
}

LOCAL unsigned char ICACHE_FLASH_ATTR
SPI_transfer (unsigned char v) {
     unsigned char d = 0;

     while (READ_PERI_REG(SPI_CMD(HSPI)) & SPI_USR);

     /* BIT(0) from Old SDK (IN and OUT) to do duplex transfer, disable all other
      * bytes (MISO - happens when set to MOSI and duplex,
      * COMMAND, ADDR, DUMMY */
     SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI | BIT(0) );
     CLEAR_PERI_REG_MASK(SPI_USER(HSPI),
			 SPI_USR_MISO |
			 SPI_USR_COMMAND |
			 SPI_USR_ADDR |
			 SPI_USR_DUMMY);

     /* 8 bit xfers */
     WRITE_PERI_REG(SPI_USER1(HSPI),
		    ((7 & SPI_USR_MOSI_BITLEN) << SPI_USR_MOSI_BITLEN_S) |
		    ((7 & SPI_USR_MISO_BITLEN) << SPI_USR_MISO_BITLEN_S));

     /* if (!read) */
     WRITE_PERI_REG(SPI_W0(HSPI), (uint32)(v));
     SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);

     /* Wait for operation (read included) to complete */
     while (READ_PERI_REG(SPI_CMD(HSPI))&SPI_USR);
     d=(uint8)(READ_PERI_REG(SPI_W0(HSPI))&0xff);

     return d;
}


void ICACHE_FLASH_ATTR
csn(bool mode)
{
  GPIO_OUTPUT_SET(4, mode);
}

#define MAX_MSG 32
void send_str(char *buf) {
     int i = MAX_MSG;
     csn(0);
     os_delay_us(1000);
     while ((*buf) && i) {
	  os_delay_us(10);
	  SPI_transfer(*buf);
	  INFO("Send char: %c\r\n", *buf);
	  buf ++;
	  i--;
     }
     os_delay_us(1000);
     csn(1);
}

void ICACHE_FLASH_ATTR avr_comm_begin()
{
  SPI_begin();
  gpio_init ();
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
  gpio_output_set(BIT4, BIT5, BIT4 | BIT5, 0);
}
