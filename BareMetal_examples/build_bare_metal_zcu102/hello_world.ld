ENTRY(_Reset)
SECTIONS
{
 . = 0x40000000;
 .startup . : { startup64.o(.text) }
 .text : { *(.text) }
 .data : { *(.data) }
 .bss : { *(.bss COMMON) }
 . = ALIGN(8);
 . = . + 0x2000; /* 4kB of stack memory */
 stack_top = .;
}
