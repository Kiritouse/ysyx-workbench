.section entry, "ax"
.globl _start
.type _start, @function
//ld脚本中指定了这里的_start
_start:
  mv s0, zero
  la sp, _stack_pointer
  call _trm_init

.size _start, . - _start
