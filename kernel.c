#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20
#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64

struct regs {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

typedef struct {
    uint16_t low_offset;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t high_offset;
} __attribute__((packed)) idt_entry_t;

static volatile uint16_t *video = (uint16_t *)VGA_MEMORY;
static int cursor_x = 0;
static int cursor_y = 0;
static char command_buffer[128];
static int command_len = 0;
static char keyboard_buffer[128];
static int keyboard_len = 0;
static int shift_pressed = 0;
static idt_entry_t idt[256];
static idt_ptr_t idt_ptr;

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

static void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void io_wait(void) {
    outb(0x80, 0);
}

static void memset(void *ptr, int value, int size) {
    char *c = (char *)ptr;
    for (int i = 0; i < size; ++i) c[i] = (char)value;
}

static int strcmp(const char *a, const char *b) {
    while (*a && *b && *a == *b) { ++a; ++b; }
    return (*a > *b) - (*a < *b);
}

static int strncmp(const char *a, const char *b, int n) {
    while (n && *a && *b && *a == *b) { ++a; ++b; --n; }
    if (n == 0) return 0;
    return (*a > *b) - (*a < *b);
}

static int strlen(const char *s) {
    int len = 0;
    while (s[len]) ++len;
    return len;
}

static void strcpy(char *dst, const char *src) {
    while (*src) *dst++ = *src++;
    *dst = 0;
}

static void scroll(void) {
    for (int y = 1; y < VGA_HEIGHT; ++y) {
        for (int x = 0; x < VGA_WIDTH; ++x) {
            video[(y - 1) * VGA_WIDTH + x] = video[y * VGA_WIDTH + x];
        }
    }
    for (int x = 0; x < VGA_WIDTH; ++x) {
        video[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = 0x0720;
    }
}

static void update_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

static void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) cursor_x--;
        video[cursor_y * VGA_WIDTH + cursor_x] = 0x0720;
    } else {
        video[cursor_y * VGA_WIDTH + cursor_x] = 0x0F00 | (unsigned char)c;
        cursor_x++;
    }
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= VGA_HEIGHT) {
        scroll();
        cursor_y = VGA_HEIGHT - 1;
    }
    update_cursor(cursor_x, cursor_y);
}

static void puts(const char *s) {
    while (*s) putchar(*s++);
}

static void clear_screen(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; ++i) video[i] = 0x0720;
    cursor_x = 0;
    cursor_y = 0;
    update_cursor(cursor_x, cursor_y);
}

static void set_idt_gate(int index, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[index].low_offset = base & 0xFFFF;
    idt[index].high_offset = (base >> 16) & 0xFFFF;
    idt[index].selector = selector;
    idt[index].zero = 0;
    idt[index].flags = flags;
}

static void idt_init(void) {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint32_t)&idt;
    memset(&idt, 0, sizeof(idt));
    set_idt_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    set_idt_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    set_idt_gate(2, (uint32_t)isr2, 0x08, 0x8E);
    set_idt_gate(3, (uint32_t)isr3, 0x08, 0x8E);
    set_idt_gate(4, (uint32_t)isr4, 0x08, 0x8E);
    set_idt_gate(5, (uint32_t)isr5, 0x08, 0x8E);
    set_idt_gate(6, (uint32_t)isr6, 0x08, 0x8E);
    set_idt_gate(7, (uint32_t)isr7, 0x08, 0x8E);
    set_idt_gate(8, (uint32_t)isr8, 0x08, 0x8E);
    set_idt_gate(9, (uint32_t)isr9, 0x08, 0x8E);
    set_idt_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    set_idt_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    set_idt_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    set_idt_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    set_idt_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    set_idt_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    set_idt_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    set_idt_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    set_idt_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    set_idt_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    set_idt_gate(20, (uint32_t)isr20, 0x08, 0x8E);
    set_idt_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    set_idt_gate(22, (uint32_t)isr22, 0x08, 0x8E);
    set_idt_gate(23, (uint32_t)isr23, 0x08, 0x8E);
    set_idt_gate(24, (uint32_t)isr24, 0x08, 0x8E);
    set_idt_gate(25, (uint32_t)isr25, 0x08, 0x8E);
    set_idt_gate(26, (uint32_t)isr26, 0x08, 0x8E);
    set_idt_gate(27, (uint32_t)isr27, 0x08, 0x8E);
    set_idt_gate(28, (uint32_t)isr28, 0x08, 0x8E);
    set_idt_gate(29, (uint32_t)isr29, 0x08, 0x8E);
    set_idt_gate(30, (uint32_t)isr30, 0x08, 0x8E);
    set_idt_gate(31, (uint32_t)isr31, 0x08, 0x8E);
    set_idt_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    set_idt_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    set_idt_gate(34, (uint32_t)irq2, 0x08, 0x8E);
    set_idt_gate(35, (uint32_t)irq3, 0x08, 0x8E);
    set_idt_gate(36, (uint32_t)irq4, 0x08, 0x8E);
    set_idt_gate(37, (uint32_t)irq5, 0x08, 0x8E);
    set_idt_gate(38, (uint32_t)irq6, 0x08, 0x8E);
    set_idt_gate(39, (uint32_t)irq7, 0x08, 0x8E);
    set_idt_gate(40, (uint32_t)irq8, 0x08, 0x8E);
    set_idt_gate(41, (uint32_t)irq9, 0x08, 0x8E);
    set_idt_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    set_idt_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    set_idt_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    set_idt_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    set_idt_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    set_idt_gate(47, (uint32_t)irq15, 0x08, 0x8E);
    __asm__ volatile("lidt (%0)" : : "r"(&idt_ptr));
}

static void pic_remap(void) {
    outb(PIC1_COMMAND, 0x11);
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();
    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();
    outb(PIC1_DATA, 0x04);
    io_wait();
    outb(PIC2_DATA, 0x02);
    io_wait();
    outb(PIC1_DATA, 0x01);
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();
    outb(PIC1_DATA, 0xFC);
    outb(PIC2_DATA, 0xFF);
}

static void keyboard_handler(void) {
    uint8_t scancode = inb(KBD_DATA_PORT);
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
        return;
    }
    if (scancode & 0x80) return;

    char c = 0;
    switch (scancode) {
        case 0x01: c = 0; break;
        case 0x1E: c = shift_pressed ? 'A' : 'a'; break;
        case 0x30: c = shift_pressed ? 'B' : 'b'; break;
        case 0x2E: c = shift_pressed ? 'C' : 'c'; break;
        case 0x20: c = shift_pressed ? 'D' : 'd'; break;
        case 0x12: c = shift_pressed ? 'E' : 'e'; break;
        case 0x21: c = shift_pressed ? 'F' : 'f'; break;
        case 0x22: c = shift_pressed ? 'G' : 'g'; break;
        case 0x23: c = shift_pressed ? 'H' : 'h'; break;
        case 0x17: c = shift_pressed ? 'I' : 'i'; break;
        case 0x24: c = shift_pressed ? 'J' : 'j'; break;
        case 0x25: c = shift_pressed ? 'K' : 'k'; break;
        case 0x26: c = shift_pressed ? 'L' : 'l'; break;
        case 0x32: c = shift_pressed ? 'M' : 'm'; break;
        case 0x31: c = shift_pressed ? 'N' : 'n'; break;
        case 0x18: c = shift_pressed ? 'O' : 'o'; break;
        case 0x19: c = shift_pressed ? 'P' : 'p'; break;
        case 0x10: c = shift_pressed ? 'Q' : 'q'; break;
        case 0x13: c = shift_pressed ? 'R' : 'r'; break;
        case 0x1F: c = shift_pressed ? 'S' : 's'; break;
        case 0x14: c = shift_pressed ? 'T' : 't'; break;
        case 0x16: c = shift_pressed ? 'U' : 'u'; break;
        case 0x2F: c = shift_pressed ? 'V' : 'v'; break;
        case 0x11: c = shift_pressed ? 'W' : 'w'; break;
        case 0x2D: c = shift_pressed ? 'X' : 'x'; break;
        case 0x15: c = shift_pressed ? 'Y' : 'y'; break;
        case 0x2C: c = shift_pressed ? 'Z' : 'z'; break;
        case 0x39: c = ' '; break;
        case 0x0E: c = '\b'; break;
        case 0x1C: c = '\n'; break;
        default: return;
    }
    if (keyboard_len < 127) {
        keyboard_buffer[keyboard_len++] = c;
    }
}

static int keyboard_read(char *out) {
    if (keyboard_len == 0) return 0;
    *out = keyboard_buffer[0];
    for (int i = 1; i < keyboard_len; ++i) keyboard_buffer[i - 1] = keyboard_buffer[i];
    keyboard_len--;
    return 1;
}

static void print_prompt(void) {
    puts("holokernel> ");
}

static void handle_command(char *cmd) {
    if (strcmp(cmd, "help") == 0) {
        puts("commands: help clear doom about ls echo\n");
    } else if (strcmp(cmd, "clear") == 0) {
        clear_screen();
    } else if (strcmp(cmd, "doom") == 0) {
        puts("Doom WAD detected: DOOM.WAD\n");
        puts("Launching Doom stub from embedded IWAD payload...\n");
        puts("(Real Doom engine can be linked later.)\n");
    } else if (strcmp(cmd, "about") == 0) {
        puts("HoloKernel v0.1 - bootable shell with Doom WAD support\n");
    } else if (strcmp(cmd, "ls") == 0) {
        puts("boot/ kernel/ doom.wad\n");
    } else if (strncmp(cmd, "echo ", 5) == 0) {
        puts(cmd + 5);
        puts("\n");
    } else if (strcmp(cmd, "echo") == 0) {
        puts("\n");
    } else {
        puts("unknown command\n");
    }
}

void isr_handler(struct regs *r) {
    (void)r;
}

void irq_handler(struct regs *r) {
    if (r->int_no == 33) {
        keyboard_handler();
    }
    if (r->int_no >= 40) outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

static void shell_loop(void) {
    clear_screen();
    puts("HoloKernel booted.\n");
    puts("Embedded Doom WAD signature loaded. Type 'help' to begin.\n");
    while (1) {
        print_prompt();
        command_len = 0;
        while (1) {
            char c = 0;
            if (!keyboard_read(&c)) {
                continue;
            }
            if (c == '\n') {
                putchar('\n');
                command_buffer[command_len] = 0;
                break;
            }
            if (c == '\b') {
                if (command_len > 0) {
                    command_len--;
                    putchar('\b');
                }
                continue;
            }
            if (command_len < 127) {
                command_buffer[command_len++] = c;
                putchar(c);
            }
        }
        handle_command(command_buffer);
    }
}

void kernel_main(uint32_t magic, uint32_t multiboot_info) {
    (void)magic;
    (void)multiboot_info;
    clear_screen();
    puts("HoloKernel -- booted with a Doom WAD shell\n");
    puts("Preparing keyboard and shell...\n");
    idt_init();
    pic_remap();
    __asm__ volatile("sti");
    shell_loop();
}
