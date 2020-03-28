#include <dev/keyboard.h>
#include <delog.h>

void  kbd_send(keycode_t code) {
    logd("key press %x", code);
}