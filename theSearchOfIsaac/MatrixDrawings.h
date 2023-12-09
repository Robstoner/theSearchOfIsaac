#include <stdint.h>
// from https://xantorohara.github.io/led-matrix-editor

void displayImageInt64(LedControl lc, uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, bitRead(row, j));
    }
  }
}

const uint64_t smiley_face = 0x003c7e4200242400;

const uint64_t sad_face = 0x00427e3c00242400;

const uint64_t cup = 0x3c18183c3c664242;

const uint64_t hammer = 0x02074efcf8783c18;

const uint64_t wrench = 0x02070e7cc8881830;