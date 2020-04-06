#include <dev/keyboard.h>
#include <delog.h>
#include <fs/vfs.hpp>
#include <process/process.h>

#define KBD_BUFF_SIZE 128

extern "C" void kbd_send(keycode_t code) {
    do_signal(SignalType::SIG_KEY, code, NOPID);
}


#define PRE_LCTR 0
#define PRE_LSFT 1
#define PRE_LALT 2
#define PRE_RCTR 3
#define PRE_RSFT 4
#define PRE_RALT 5
char kbd2ascii(keycode_t code) {
    static int pressing = 0;
    return 0;
}
