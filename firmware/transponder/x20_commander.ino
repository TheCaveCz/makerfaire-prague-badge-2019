//////////////////////////////
//
// MakerFaire Prague 2019 badge
// (c) The Cave, 2019
// https://thecave.cz
//

//
// Helper class for handling serial commands
//

#if SERIAL_COMMANDS

typedef struct {
  char code;
  uint8_t len;
  void (*callback)(const char * cmd, const uint8_t len);
} Command;

#define CMD_OK 0
#define CMD_ERROR 1
#define CMD_SILENT 255

class Commander {
  public:
    Commander() {
      _pos = 0;
      _buf[0] = 0;
      _cmds = NULL;
      _of = 0;
    }

    void begin(Command * commands) {
      _cmds = commands;
    }

    void check() {
      int d = Serial.read();
      if (d == '\n') {
        if (_of) {
          _of = 0;
          _pos = 0;
          Serial.println(F("-Command too long"));
        } else if (_pos) {
          _buf[_pos] = 0;
          exec();
          _pos = 0;
        }
      } else if (d >= 0 && d != '\r') {
        _buf[_pos++] = d;
        if (_pos >= sizeof(_buf)) {
          _pos = 0;
          _of = 1;
        }
      }
    }

    inline void printError() {
      Serial.println(F("-Error"));
    }

    inline void printOk() {
      Serial.println(F("+OK"));
    }

    bool checkHex(const uint8_t val) {
      return (val >= '0' && val <= '9') || (val >= 'A' && val <= 'F') || (val >= 'a' && val <= 'f');
    }

    bool checkHex(const char * buf, const uint8_t from, const uint8_t to) {
      for (uint8_t i = from; i <= to; i++) {
        if (!checkHex(buf[i])) return false;
      }
      return true;
    }

    uint16_t parseHexWord(const char * buf, const uint8_t from) {
      return (parseHex(buf[from], buf[from + 1]) << 8) | parseHex(buf[from + 2], buf[from + 3]);
    }

    uint8_t parseHex(const uint8_t v1, const uint8_t v2) {
      return (parseHex(v1) << 4) | parseHex(v2);
    }

    uint8_t parseHex(const uint8_t val) {
      if (val >= '0' && val <= '9') return val - '0';
      if (val >= 'A' && val <= 'F') return val - 'A' + 10;
      if (val >= 'a' && val <= 'f') return val - 'a' + 10;
      return 0;
    }

    bool checkRange(const uint8_t val, const uint8_t minVal, const uint8_t maxVal) {
      return val >= minVal && val <= maxVal;
    }

    void printHex(const uint8_t b) {
      if (b < 16) Serial.print('0');
      Serial.print(b, HEX);
    }

    void printHexWord(const uint16_t w) {
      printHex(w >> 8);
      printHex(w & 0xff);
    }

    void printCommands() {
      Serial.println(F("+Commands:"));
      uint8_t i = 0;
      while (_cmds[i].code) {
        Serial.write(' ');
        Serial.write(_cmds[i].code);
        Serial.write(' ');
        Serial.println(_cmds[i].len);
        i++;
      }
    }

  private:
    char _buf[16];
    uint8_t _pos;
    Command * _cmds;
    uint8_t _of;

    void exec() {
      if (_cmds == NULL) {
        Serial.println(F("-Missing cmd table"));
        return;
      }

      uint8_t i = 0;
      while (_cmds[i].code) {
        if (_cmds[i].code == _buf[0] && _cmds[i].len == _pos) {
          if (_cmds[i].callback) {
            (*_cmds[i].callback)(_buf, _pos);
          } else {
            Serial.println(F("-Missing callback"));
          }
          return;
        }
        i++;
      }
      Serial.println(F("-Unknown command"));
    }
};


Commander cmd;

#endif
