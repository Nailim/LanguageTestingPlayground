ENTRY(_start)

MEMORY
{
  RAM (rwx) : ORIGIN = 0x42000000, LENGTH = 64K
}

SECTIONS
{
  .text : {
    *(.text*)
  } > RAM

  .bss : {
    *(.bss*)
    *(COMMON)
  } > RAM
}

