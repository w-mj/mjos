#include <dev/keyboard.h>
#include <delog.h>
#include <fs/vfs.hpp>

#define KBD_BUFF_SIZE 128

extern "C" void kbd_send(keycode_t code) {
    static keycode_t buff[KBD_BUFF_SIZE];
    static int buff_cnt = 0;
    buff[buff_cnt] = code;
    buff_cnt++;
    if (buff_cnt == KBD_BUFF_SIZE || code == KEY_ENTER) {
        assert(stdin != nullptr);
        stdin->write(reinterpret_cast<const char *>(buff), buff_cnt * (int)sizeof(keycode_t));
        buff_cnt = 0;
    }
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
