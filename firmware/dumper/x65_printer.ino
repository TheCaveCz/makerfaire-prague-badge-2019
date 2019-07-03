//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Printing receipts
//

#if USE_PRINTER

SoftwareSerial mySerial(PIN_PRINT_RX, PIN_PRINT_TX);
Adafruit_Thermal printer(&mySerial, PIN_PRINT_DTR);

void printerSetup() {
  mySerial.begin(9600);
  printer.begin();
  printer.wake();
  printer.setDefault();
  printer.feed(2);
}

void printerPrintReport(const uint16_t currentId, const uint16_t count, const uint8_t teamId) {
  printer.setSize('L');
  printer.println(F("Badge report"));
  printer.setSize('S');
  printer.boldOn();
  printer.println(F("MakerFaire Prague 2019"));
  printer.boldOff();
  printer.println();

  printer.print(F("Badge: #"));
  printer.println(currentId, OCT);

  printer.print(F("Tym: "));
  switch (teamId) {
    default: printer.println(F("Zadny")); break;
    case 1: printer.println(F("Cerveny")); break;
    case 2: printer.println(F("Zeleny")); break;
    case 3: printer.println(F("Modry")); break;
  }

  printer.print(F("Celkem uloveno: "));
  printer.println(count);

  if (count) {
    printer.println();
    printer.println(F("Seznamovacka:"));

    uint16_t b = 0;
    for (uint16_t id = 0; id < STORAGE_MAX_ID; id++) {
      if (!storageIdSeen(id)) continue;
      if (id < 512) printer.print(" ");
      printer.print(id, OCT);
      printer.write(' ');
      b++;
      if (b >= 6) {
        b = 0;
        printer.println();
      }
    }
    printer.println();
  }


  printer.println();
  printer.print(F("Maker Caching: "));
  printer.println(storageCalcTreasureSeen());
  for (uint8_t i = 0; i < STORAGE_TREASURE_COUNT; i++) {
    printer.write(' ');
    if (storageTreasureSeen(i)) {
      if (i < 9) printer.write(' ');
      printer.print(i + 1);
    } else {
      printer.write('-');
      printer.write('-');
    }
    printer.write(' ');

    if ((i % 6) == 5) {
      printer.println();
    } else {
      printer.write('|');
    }
  }

  printer.println();
  printer.print(F("Brany:"));
  for (uint8_t i = 0; i < STORAGE_GATE_COUNT; i++) {
    printer.write(' ');
    if (storageGateSeen(i)) {
      printer.print(i + 1);
    } else {
      printer.write('-');
    }
    printer.write(' ');
    if ((i % 6) != 5) {
      printer.write('|');
    }
  }
  printer.println();

  printer.println();
  printer.println(F("---"));
  printer.println(F("Sleduj stav souteze online na"));
  printer.println(F("https://mf2019.thecave.cz"));
  printer.println();
  printer.println(F("Ubastleno s laskou v The Cave"));
  printer.println(F("https://www.thecave.cz"));

  printer.feed(5);
}

#endif
