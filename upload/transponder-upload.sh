#!/bin/bash

#////////////////////////////
#
# MakerFaire Prague 2019 badge
# (c) The Cave, 2019
# https://thecave.cz
#
# Licensed under MIT License.
#
#
# Mass upload of transponder firmware
# Usage:
#  ./transponder-upload.sh <transponderId>
#

SOURCE=transponders.txt
HEX=transponder.ino.with_bootloader.eightanaloginputs.hex
LINE=$1
if [ -z $LINE ]; then
	echo "Provide transponder id"
	exit 2
fi

echo `head -n $LINE $SOURCE | tail -n 1`
EEPROM=`head -n $LINE $SOURCE | tail -n 1 | awk '{ print $4 }'`
avrdude -q -q -p atmega328p -c usbasp -B 0.6 -e -U lock:w:0x3F:m -U efuse:w:0xFE:m -U hfuse:w:0xDA:m -U lfuse:w:0xFF:m -U flash:w:$HEX:i -U eeprom:w:$EEPROM:m -U lock:w:0x0F:m 
STATUS=$?
if [ $STATUS -ne 0 ]; then
  echo "Avrdude failed"
  exit 1
fi
printf "\a"
