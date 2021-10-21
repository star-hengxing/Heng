OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(main)

KERNEL_CODE_START = 0x100000;
KERNEL_VIRTUAL_OFFSET = 0xFFFF800000000000;

SECTIONS
{
    .  = KERNEL_CODE_START;
    . += KERNEL_VIRTUAL_OFFSET;

    PROVIDE( KERNEL_CODE_START = . );

    .text : AT(ADDR(.text) - KERNEL_VIRTUAL_OFFSET)
    {
        *(.text .text.*)
        /* . = ALIGN(4K); */
    }
    PROVIDE( TEXT_END = . );

    .rodata : AT(ADDR(.rodata) - KERNEL_VIRTUAL_OFFSET)
    {
        *(.rodata .rodata.*)
    }
    PROVIDE( RODATA_END = . );

    .data : AT(ADDR(.data) - KERNEL_VIRTUAL_OFFSET)
    {
        *(.data .data.*)
    }
    PROVIDE( DATA_END = . );

    .bss : AT(ADDR(.bss) - KERNEL_VIRTUAL_OFFSET)
    {
        *(.bss .bss.*)
    }
    . = ALIGN(4K);

    PROVIDE( KERNEL_CODE_END = . );

    /DISCARD/ :
    {
        *(.eh_frame .rela.eh_frame .note.*)
    }
}