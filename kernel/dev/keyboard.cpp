#include <dev/keyboard.h>
#include <delog.h>
#include <fs/vfs.hpp>
#include <process/process.h>

extern "C" void kbd_send(keycode_t code) {
    do_signal(SignalType::SIG_KEY, code, NOPID);
}
