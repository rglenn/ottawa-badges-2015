avrdude -c usbtiny -p m328p -U lfuse:w:0xe2:m -U hfuse:w:0xda:m -U efuse:w:0xff:m
avrdude -c usbtiny -p m328p -U flash:w:ATmegaBOOT_168_atmega328_pro_8MHz.hex 