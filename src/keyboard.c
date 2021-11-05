#include <interrupt/idt.h>
#include <kernel/printk.h>
#include <keyboard.h>
#include <x86/io.h>

static u8 shift_code[256] =
{
    [0x1D] CTL,
    [0x2A] SHIFT,
    [0x36] SHIFT,
    [0x38] ALT,
    [0x9D] CTL,
    [0xB8] ALT
};

static u8 toggle_code[256] =
{
    [0x3A] CAPSLOCK,
    [0x45] NUMLOCK,
    [0x46] SCROLLLOCK
};

static u8 normal_map[256] =
{
    NO,   0x1B, '1',  '2',  '3',  '4',  '5',  '6',  // 0x00
    '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
    'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  // 0x10
    'o',  'p',  '[',  ']',  '\n', NO,   'a',  's',
    'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  // 0x20
    '\'', '`',  NO,   '\\', 'z',  'x',  'c',  'v',
    'b',  'n',  'm',  ',',  '.',  '/',  NO,   '*',  // 0x30
    NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
    NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  // 0x40
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
    '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   // 0x50
    [0x9C] '\n',      // KP_Enter
    [0xB5] '/',       // KP_Div
    [0xC8] KEY_UP,    [0xD0] KEY_DN,
    [0xC9] KEY_PGUP,  [0xD1] KEY_PGDN,
    [0xCB] KEY_LF,    [0xCD] KEY_RT,
    [0x97] KEY_HOME,  [0xCF] KEY_END,
    [0xD2] KEY_INS,   [0xD3] KEY_DEL
};

static u8 shift_map[256] =
{
    NO,   033,  '!',  '@',  '#',  '$',  '%',  '^',  // 0x00
    '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
    'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  // 0x10
    'O',  'P',  '{',  '}',  '\n', NO,   'A',  'S',
    'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',  // 0x20
    '"',  '~',  NO,   '|',  'Z',  'X',  'C',  'V',
    'B',  'N',  'M',  '<',  '>',  '?',  NO,   '*',  // 0x30
    NO,   ' ',  NO,   NO,   NO,   NO,   NO,   NO,
    NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',  // 0x40
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
    '2',  '3',  '0',  '.',  NO,   NO,   NO,   NO,   // 0x50
    [0x9C] '\n',      // KP_Enter
    [0xB5] '/',       // KP_Div
    [0xC8] KEY_UP,    [0xD0] KEY_DN,
    [0xC9] KEY_PGUP,  [0xD1] KEY_PGDN,
    [0xCB] KEY_LF,    [0xCD] KEY_RT,
    [0x97] KEY_HOME,  [0xCF] KEY_END,
    [0xD2] KEY_INS,   [0xD3] KEY_DEL
};

static u8 ctl_map[256] =
{
    NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
    NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
    C('Q'),  C('W'),  C('E'),  C('R'),  C('T'),  C('Y'),  C('U'),  C('I'),
    C('O'),  C('P'),  NO,      NO,      '\r',    NO,      C('A'),  C('S'),
    C('D'),  C('F'),  C('G'),  C('H'),  C('J'),  C('K'),  C('L'),  NO,
    NO,      NO,      NO,      C('\\'), C('Z'),  C('X'),  C('C'),  C('V'),
    C('B'),  C('N'),  C('M'),  NO,      NO,      C('/'),  NO,      NO,
    [0x9C] '\r',      // KP_Enter
    [0xB5] C('/'),    // KP_Div
    [0xC8] KEY_UP,    [0xD0] KEY_DN,
    [0xC9] KEY_PGUP,  [0xD1] KEY_PGDN,
    [0xCB] KEY_LF,    [0xCD] KEY_RT,
    [0x97] KEY_HOME,  [0xCF] KEY_END,
    [0xD2] KEY_INS,   [0xD3] KEY_DEL
};

u8 keyboard_get()
{
    static u32 shift;
    static u8* charcode[4] = {normal_map, shift_map, ctl_map, ctl_map};
    u32 status, data, c;
    // 检查是否有数据
    status = inb(KEYBOARD_STATUS_PORT);
    if((status & KEYBOARD_DATA_BUFFER) == 0)
        return 2;
    // 获取扫描码
    data = inb(KEYBOARD_DATA_PORT);

    // 发送的第一个数据为0xE0，即代表该扫描码有两个数字构成
    // 启用E0标记位
    if(data == 0xE0)
    {
        shift |= E0ESC;
        return 0;
    }
    else if(data & 0x80)
    {
        // Key released
        // 按键松开，还原配置
        // shift_code使用按下扫描码来作为映射
        // 当扫描码为E0扫描码时，使用松开扫描码以确保与其他键使用不同的映射
        data = (shift & E0ESC ? data : data & 0x7F);
        shift &= ~(shift_code[data] | E0ESC);
        return 0;
    }
    else if(shift & E0ESC)
    {
        // Last character was an E0 escape; or with 0x80
        // 若是E0扫描码的按键，使用松开扫描码进行映射
        // 并且取消E0标志位
        data |= 0x80;
        shift &= ~E0ESC;
    }

    // 使用如CTRL，ALT这样按下才生效的的功能键
    shift |= shift_code[data];
    // 使用CapsLk，NumLock这样保持状态
    shift ^= toggle_code[data];
    // 根据shift的状态位来得到当前按键使用的扫描码映射
    c = charcode[shift & (CTL | SHIFT)][data];
    // 若当前为大写锁定状态，将小写字母转大写，大写转小写
    if(shift & CAPSLOCK)
    {
        if('a' <= c && c <= 'z')
            c += 'A' - 'a';
        else if('A' <= c && c <= 'Z')
            c += 'a' - 'A';
    }
    return c;
}

static void keyboard_callback()
{
    u8 code = keyboard_get();
    printk("%c", code);
}

void init_keyboard()
{
    interrupt_handler_register(33, keyboard_callback);
}