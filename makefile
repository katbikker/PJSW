
##########------------------------------------------------------##########
##########              Project-specific Details                ##########
##########    Check these every time you start a new project    ##########
##########------------------------------------------------------##########

MCU   = atmega32

## This is where your main() routine lives 
MAIN = robotMain.c

## If you've split your program into multiple .c / .h files, 
## include the additional source (in same directory) here 
LOCAL_SOURCE = robotHeader.h


##########------------------------------------------------------##########
##########                 Programmer Defaults                  ##########
##########          Set up once, then forget about it           ##########
##########        (Can override.  See bottom of file.)          ##########
##########------------------------------------------------------##########

PROGRAMMER_TYPE = avrisp
# extra arguments to avrdude: baud rate, chip type, -F flag, etc.
PROGRAMMER_ARGS = -b 19200 -P /dev/ttyACM0

##########------------------------------------------------------##########
##########                   Makefile Magic!                    ##########
##########         Summary:                                     ##########
##########             We want a .hex file                      ##########
##########        Compile source files into .elf                ##########
##########        Convert .elf file into .hex                   ##########
##########        You shouldn't need to edit below.             ##########
##########------------------------------------------------------##########

## Defined programs / locations
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

CFLAGS= -mmcu=$(MCU) -Os  -Wall -std=gnu99

## Lump target and extra source files together
TARGET = $(strip $(basename $(MAIN)))
SRC = $(TARGET).c
#EXTRA_SOURCE = $(addprefix $(EXTRA_SOURCE_DIR), $(EXTRA_SOURCE_FILES))
#SRC += $(EXTRA_SOURCE) 
SRC += $(LOCAL_SOURCE) 

## List of all header files
HEADERS = $(SRC:.c=.h) 

## For every .c file, compile an .o object file
OBJ = $(SRC:.c=.o) 

## Generic Makefile targets.  (Only .hex file is necessary)
all: $(TARGET).hex

%.hex: %.elf
	$(OBJCOPY) -R .eeprom -O ihex $< $@

%.elf: $(SRC)
	$(CC) $(CFLAGS) $(SRC) --output $@ 

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).obj \
	$(TARGET).o $(TARGET).d $(TARGET).eep $(TARGET).lst \
	$(TARGET).lss $(TARGET).sym $(TARGET).map $(TARGET)~ \
	$(TARGET).eeprom

squeaky_clean:
	rm -f *.elf *.hex *.obj *.o *.d *.eep *.lst *.lss *.sym *.map *~

##########------------------------------------------------------##########
##########              Programmer-specific details             ##########
##########           Flashing code to AVR using avrdude         ##########
##########------------------------------------------------------##########

flash: $(TARGET).hex 
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<

## An alias
program: flash
