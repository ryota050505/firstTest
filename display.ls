OUTPUT_ARCH(arm)

SECTIONS {
  .text 0x2000000 : { *.o(.text) }
  .data   : { *.o(.data) }
  .rodata : { *.o(.rodata*) }
  .bss    : { *.o(.bss) }
  .image  : {
    image_start = . ;
    *.gba(*)
   }
 }
