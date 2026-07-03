.cpu cortex-m4          //Architecture used
.thumb                  //thumb instruction, slower but lighter

.word 0x20020000        //end of RAM (check SRAM in datasheet)
.word _reset            //reset vector
.thumb_func
_reset:
    bl main
    b .