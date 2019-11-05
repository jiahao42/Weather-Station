#ifndef EXTRA_REGISTER_H_
#define EXTRA_REGISTER_H_

#define AT91C_PIOD_AIMER (AT91_CAST(AT91_REG *) 	0x400E14B0)
#define AT91C_PIOC_AIMER (AT91_CAST(AT91_REG *) 	0x400E12B0)
#define AT91C_PIOD_DIFSR (AT91_CAST(AT91_REG *) 	0x400E1484)
#define AT91C_PIOC_DIFSR (AT91_CAST(AT91_REG *)         0x400E1284)
#define AT91C_PIOD_SCDR (AT91_CAST(AT91_REG *) 	        0x400E148C)
#define AT91C_PIOC_SCDR (AT91_CAST(AT91_REG *) 	        0x400E128C)
#define PIO_ISR         (AT91_CAST(AT91_REG *) 	        0x0000004C) // (PIO_ISR) Interrupt Status Register
#define AT91C_PIOD_IER (AT91_CAST(AT91_REG *) 	        0x400E1440)
#define AT91C_PIOC_IER (AT91_CAST(AT91_REG *) 	        0x400E1240)

#endif