## Mass firmware upload

Each .swift file is used to generate list of IDs for IR transfers. Transponders and gates have well defined set of IDs (also hardcoded in firmware).

Use `upload.sh` shell script for mass upload of badge firmware. This script automatically takes IDs from `badges.txt` file and programs it into badges.

For transponders, use `transponde-upload.sh` and provide user facing number of transponder (it will be mapped to internal ID automatically).

Upload scripts are using `USBasp` programmer. 

### Fuses

All fuses for battery powered boards (badge, transponder) have `Brown-Out Detect level` lowered to 1.8V (from 2.7V default setting).

[Fuse calculator](http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p&LOW=FF&HIGH=DA&EXTENDED=FE&LOCKBIT=0F)

| fuse | hodnota |
| --- | --- |
| low | 0xFF |
| high | 0xDA |
| extended | 0xFE |
| lock | 0x0F |
