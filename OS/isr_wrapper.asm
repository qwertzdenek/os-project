EXTRN do_schedule:NEAR

.code
PUBLIC isr_wrapper

isr_wrapper PROC
    CLD
    CALL do_schedule
    RET
isr_wrapper ENDP

END
