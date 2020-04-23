#include <dev/keyboard.h>

static char asciiTable[ASCII_KEY_COUNT] = {
        '\0',
        '0','1','2','3','4','5','6','7','8','9',
        'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
        '`','-','=','\t','[',']',';','\'',',','.','/','\\',' ','\b','\n',
        '0','1','2','3','4','5','6','7','8','9',
        '/','*','-','+','\n','.'
};

static char shiftedAsciiTable[ASCII_KEY_COUNT] = {
        '\0',
        ')','!','@','#','$','%','^','&','*','(',
        'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
        '~','_','+','\t','{','}',':','"','<','>','?','|',' ','\b','\n',
        '0','1','2','3','4','5','6','7','8','9',
        '/','*','-','+','\n','.'
};

static char *table[2] = {asciiTable, shiftedAsciiTable};

char keycode2ascii(keycode_t keycode) {
    static bool shift = false;
    bool down = (keycode & RELEASE) == 0;
    keycode = static_cast<keycode_t>(keycode & (~RELEASE));
    if (keycode == KEY_LEFTSHIFT || keycode == KEY_RIGHTSHIFT) {
        shift = down;
    }
    if (down && keycode < ASCII_KEY_COUNT) {
        return table[shift][keycode];
    }
    return 0;
}
