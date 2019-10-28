# nemu
1. 解码帮助函数`decode_op_SI()`没写对，导致后面and指令取操作数时8位立即数没有扩展，
写错的原因是没有认清`rtl_sext()`函数的width参数指的是要扩展的数的原始位数。
2. `rtl_push()`函数写错，在`*(uint32_t *)(&pmem[reg_l(R_ESP)]) = *src1;`这行代码
中，将`uint32_t`写成了`uint64_t`。
3. `exec_sub()`和`exec_cmp()`没写对，这是因为没有认清`rtl_lr()`函数的作用，本以为
该函数的第二个参数指的是要向寄存器中赋值的数，但事实上第二个参数指的是通用寄存器
编号，该函数是将通用寄存器的值赋值给一个寄存器变量。
4. `rtl_update_ZF()`函数错了，因为里面的switch语句忘记了break :(
5. 位操作时取出某一位的值时用了|操作符而不是&操作符，但及时发现改正。
6. 注意在译码的group3中test指令需要再次进行译码取出立即数
7. add指令和sub指令一样如果第二个操作数是1个字节的立即数的话需要进行符号扩展
8. movsx和movzx指令两个操作数位宽不等，可以在其执行函数开始时根据opcode设置
第二个操作数的位宽。
9. export AM_HOME和NEMU_HOME
10. movzx的执行函数需要完善，原来写的不对
11. 临时寄存器使用要注意调用约定，在rtl中使用t0和t1，在执行函数中使用s0和s1
12. 在执行函数中应该使用一系列的rtl函数，不要直接访问cpu寄存器或者pmem内存，这样
若需要在访存或者访问寄存器时记录一些信息就可以直接在rtl中增加代码，而不需要在
每一个需要访存或寄存器的指令中增加代码
13. 在litenes模拟器的Makefile文件中`include $(AM_HOME)/Makefile.app`代码后面需要
加上一行`CFLAGS   += -O0`代码，否则会使用Makefile.compile中设定的-O2优化，导致
可执行文件中产生`rol`指令
14. 在`__am_video_write()`函数中使用`memcpy`将像素内容复制到显存中时写错了
15. IDTR: https://wiki.osdev.org/Interrupt_Descriptor_Table , lidt需要单独写一个
译码函数
16. 在int指令中，设置新的pc要使用`rtl_j()`函数，在iret指令中恢复pc时也要使用`rtl_j()`
17. 在80386中`push esp`指令是将旧的esp压进去
18. 在文件系统实验中出现 *cat: ics-2019/navy-apps//fsimg/bin/hello: input file is output file*
问题，只需要删除`nanos-lite/build/ramdisk.img`文件再次运行即可
19. syscall 要记得在navy-apps/libs/libos/nanos.c中修改
20. 在`fd_read()`和`fd_write()`函数中要先判断len是否超出，若超出则要在调用`ramdisk_read()`
或`ramdisk_write()`函数时设置正确的len
21. `events_read()`函数返回的字符串需要有换行符
22. 在navy-apps中默认开启-O2优化，这样产生的代码中会有`movs`指令，可以去掉该优化
来取消这个指令。若要实现该指令则要注意该指令在传送数据之后会改变两个寄存器的值。
23. 在`_sbrk()`中初始化program break时应将其初始化为`&_end`，即`_end`的地址
24. 在litenes游戏中，由于帧率比较低，所以键盘在敲击时需要按长一点，否则识别不到
25. 在`fs_close()`中应该将`open_offset`设为0