.global _Reset
_Reset:
 ldr x30, =stack_top
 mov sp, x30
 bl main
 b .
