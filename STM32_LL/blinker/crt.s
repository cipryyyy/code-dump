.cpu cortex-m4
.thumb

.word 0x20020000        //end of RAM
.word _reset            //reset vector
.thumb_func
_reset:
    bl main
    b .