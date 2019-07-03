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
# Mass upload of badges firmware
#

while true; do
	echo `head -n 1 badges.txt`
	EEPROM=`head -n 1 badges.txt | awk '{ print $4 }'`

	while true; do
		## pre-detect to avoid spamming screen if nothing is connected
		avrdude -qq -p atmega328p -c usbasp -B 32 &> /dev/null
		STATUS=$?
		if [ $STATUS -ne 0 ]; then
			sleep 0.5
			continue
		fi

		## do real programming here
		avrdude -q -q -p atmega328p -c usbasp -B 12 -e -U lock:w:0x3F:m -U efuse:w:0xFE:m -U hfuse:w:0xDA:m -U lfuse:w:0xFF:m
		STATUS=$?
		if [ $STATUS -ne 0 ]; then
			echo -e "***** Avrdude failed\n\n"
			sleep 1
			continue
		fi

		avrdude -q -q -p atmega328p -c usbasp -B 0.6 -U flash:w:badge2019.ino.with_bootloader.eightanaloginputs.hex:i -U eeprom:w:$EEPROM:m -U lock:w:0x0F:m 
		STATUS=$?
		if [ $STATUS -ne 0 ]; then
			echo -e "***** Avrdude failed\n\n"
			sleep 1
			continue
		else
			break
		fi
	done

	tail -n +2 badges.txt > badges.tmp
	mv badges.tmp badges.txt

	printf "\a"
	sleep 0.2
	printf "\a"
	printf "next in 3..."
	sleep 1
	printf "2..."
	sleep 1
	printf "1..."
	sleep 1
	printf "go\n"
done