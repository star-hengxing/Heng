# 环境

os: Linux

tool:
- gcc(编译)
- ld(链接)
- objcopy(提取二进制)
- dd(写入虚拟镜像)
- gdb(调试)
- make(自动化构建)
- qemu(虚拟机)

# 编译

**makefile里把第一个all取消注释，注释第二个all，才能直接make一条命令编译**

为什么这么做呢，主要是我还没找(学习)到更好的方法

暂时使用单目标的all，方便调试使用，避免不必要的重复。比如dd指令，makefile会完整执行一次，可以考虑换成其他语言编写的脚本

整个镜像制作流程: 第一个扇区是MBR，第二个扇区是进入64位模式的初始化代码，剩余都是elf内核

elf内核文件有两个，**在build目录下去掉debug等信息的kernel.elf才是真正写入镜像的内容**，减少镜像体积

# 项目目录结构

- build  
    debug目录下都是提供给gdb调试的信息，其他小文件不知道怎么整理，暂时就这样放了
- include  
    长期变化
- src  
    理论上每个目录需要一个makefile，因为没有找到一个makefile递归编译目录的方法，所以先偷懒都不分目录
- script  
    gdb脚本和链接脚本

其他看名字基本应该就能看懂，这里推荐使用loc统计代码，用tree查看目录结构

# 代码

## MBR

使用**汇编+C**，在实模式利用bios中断获取计算机信息方便后续使用

- [x] 内存探测(经典e820)
- [ ] 读取硬盘数据(因为现在是用IDE端口操作读取，然后发现bochs/vmware用不了，只能用qemu)
- [ ] VBE模式切换(现代计算机没有vga设备了，想输出到屏幕要自己渲染字体)

然后就是一些进入32位保护模式的准备，可以看intel或amd手册

进入保护模式后，切换到C写的代码，读取进入64位代码和elf内核(IDE端口读取硬盘抄xv6的)

## 64位模式初始化时内存地址分配

以下都是物理地址

|地址|内容|
|---|---|
|0x500|elf内核头|
|0x700|entry_64|
|0x900|内核实际入口(main函数入口)|
|0x904|e820探测内存块数量|
|0x908|e820探测内存块结构体初始地址|
|0x1000|四级页表|
|0x2000|三级页表|
|0x3000|二级页表|
|0x4000|一级页表|
|0x5000|一张4K页表结束位置|
|0x7C00|栈|
|0x7C00|MBR|
|0x100000|内核代码|

其实临时页表也可以直接改成3级页表方便操作

## 分页

三级页表，每页512项

理论上都是页目录，但最后一级页表都是指向真实物理页面，并且地址是以4k为粒度

0x0000-0x1000是一页，4k大小，页表地址上写的是0，不过页表前12位拿来表示页属性，所以写入页表的时候不需要移位，只需要跟属性*或运算*就行

kernel使用1g的虚拟内存，所以只需要分配3个页给页表

理论上可以用四级页表，然后通过分配虚拟内存的位置，来进行安全保护

比如说对栈的位置不分配连续的虚拟内存，如果stackoverflow就会触发缺页中断

## kernel空间分配(长期变化)

x = KERNEL_CODE_END(已对齐4k)

|物理地址|内容|
|---|---|
|0x100000|内核代码段(text + rodata + data + bss)|
|x|三级页表|
|x + 12k|管理物理页的数组|

栈暂时还在用0x7C00下的内存，以后再考虑换位置

# TODO

- [ ] 每个目录有对应makefile编译
- [ ] bios中断读取硬盘
- [ ] 显卡切换为VBE模式
- [ ] 使用临时三级页表
- [ ] 中断栈
- [ ] 多个内核栈(smp或者多线程都需要)
- [ ] 内核代码的虚拟地址和物理地址随机化
- [ ] UEFI启动
- [ ] 高级中断芯片，高精度计时器(IOAPIC/LAPIC)