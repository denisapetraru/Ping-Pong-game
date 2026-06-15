#ifndef SPI_H_
#define SPI_H_
#include <stdint.h>

	void spi_set_ss();

	void spi_clear_ss();
	
	
	void spi_master_init();


	void spi_write(char cData);

	uint8_t spi_read();

#endif //SPI_H_
