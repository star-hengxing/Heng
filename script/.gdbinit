set architecture i386:x86-64

set disassembly-flavor att

# 退出时不显示提示信息
set confirm off
# 按照派生类型打印对象
set print object on
# 打印数组的索引下标
set print array-indexes on
# 每行打印一个结构体成员
set print pretty on

target remote:1234

file build/debug/kernel

break main

layout src

continue