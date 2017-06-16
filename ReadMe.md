[TOP]

# 1. 实验名称
汇编语言及程序设计

# 2. 实验报告作者
**信息安全 1410658 杨旭东**

# 3. 实验内容

1. 定义 NK-CPU 汇编语言的基本内容
2. 实现 NK-CPU 的汇编程序
3. 使用 NK-CPU 汇编语言编写对 20 个随机数的排序程序

# 4.实验设计依据
## 4.1.汇编器
<p>通常，现代<b>汇编器</b>通过将汇编指令助记符转换为操作码，并通过解析内存位置和其他实体的符号名称来创建目标代码。 使用符号参考是汇编器的一个关键特性，节省了修改后的繁琐计算和手动地址更新。 大多数汇编器还包括用于执行文本替换的宏设施，例如，生成通用短序列指令，而不是称为子程序。</p>

## 4.2. 通行数
<p>基于需要多少通过源来生成可执行程序，有两种类型的汇编程序。</p>

- 一次汇编程序通过源代码一次，并假设所有符号将在引用它们的任何指令之前定义。
- 双遍汇编器在第一遍中创建具有所有符号及其值的表，然后在第二遍中使用表来生成代码。 汇编器必须至少能够确定第一遍的每个指令的长度，以便可以计算符号的地址。

<p>单程汇编器的优点是速度快，这与计算机速度和能力的进步一样重要。 两路汇编器的优点在于可以在程序源代码的任何地方定义符号。 这样可以以更合乎逻辑和有意义的方式定义程序，从而使两通汇编程序更易于阅读和维护。</p>

## 4.3. 汇编语言程序
<p>用汇编语言编写的程序由一系列助记符语句和元语句（已知各种指令，伪指令和伪操作），注释和数据组成。 这些由汇编程序转换成可以加载到内存中并执行的可执行指令流。 汇编器还可以用于生成数据块，从格式化和注释的源代码中被其他代码使用。</p>

## 4.4. 指令编码规则
ADD |*Add*
-|-
Description:|Adds two registers and stores the result in a register
Operation:|$d = $s + $t; advance_pc (4);                         
Syntax:|add $d, $s, $t                                        
Encoding:|0000 00ss ssst tttt dddd d000 0010 0000

ADDI |*Add immediate*
-|-
Description: |Adds a register and a signed immediate value and stores the result in a register
Operation: |$t = $s + imm; advance_pc (4);
Syntax: |addi $t, $s, imm
Encoding: |0010 00ss ssst tttt iiii iiii iiii iiii

SLT |*Set on less than (signed)*
-|-
Description: |If $s is less than $t, $d is set to one. It gets zero otherwise.
Operation: |if $s < $t $d = 1; advance_pc (4); else $d = 0; advance_pc (4);
Syntax: |slt $d, $s, $t
Encoding: |0000 00ss ssst tttt dddd d000 0010 1010

J |*Jump*
-|-
Description: |Jumps to the calculated address
Operation: |PC = nPC; nPC = (PC & 0xf0000000) | (target << 2);
Syntax: |j target
Encoding: |0000 10ii iiii iiii iiii iiii iiii iiii

BLEZ |*Branch on less than or equal to zero*
-|-
Description: |Branches if the register is less than or equal to zero
Operation: |if $s <= 0 advance_pc (offset << 2)); else advance_pc (4);
Syntax: |blez $s, offset
Encoding: |0001 10ss sss0 0000 iiii iiii iiii iiii

LW |*Load word*
-|-
Description: |A word is loaded into a register from the specified address.
Operation: |$t = MEM[$s + offset]; advance_pc (4);
Syntax: |lw $t, offset($s)
Encoding: |1000 11ss ssst tttt iiii iiii iiii iiii

LUI |*Load upper immediate*
-|-
Description: |The immediate value is shifted left 16 bits and stored in the register. The lower 16 bits are zeroes.
Operation: |$t = (imm << 16); advance_pc (4);
Syntax: |lui $t, imm
Encoding: |0011 11-- ---t tttt iiii iiii iiii iiii

LA | *Load value's address*
-|-
Description: |An address is loaded into a register by the value's name.
Operation: |$t = (value); advance_pc (4);
Syntax: |la $t, value
Encoding: |1000 00-- ---t tttt aaaa aaaa aaaa aaaa

SW |*Store word*
-|-
Description: |The contents of $t is stored at the specified address.
Operation: |MEM[$s + offset] = $t; advance_pc (4);
Syntax: |sw $t, offset($s)
Encoding: |1010 11ss ssst tttt iiii iiii iiii iiii

SYSCALL |*System call*
-|-
Description: |Generates a software interrupt.
Operation: |advance_pc (4);
Syntax: |syscall
Encoding: |0000 00-- ---- ---- ---- ---- --00 1100

# 5. 实验结果与分析
## 5.1. 定义 NK-CPU 汇编语言的基本内容
总体分为两部分：数据段和代码段：<br>
```asm
.data       # variable declarations follow this line
            # ...
.text       # instructions follow this line 
main:       # indicates start of code (first instruction to execute)
            # ...
# End of program, leave a blank line afterwards to make SPIM happy

```
地址均 16 位二进制数。<br>
数据段的整数数据编码为 16 位二进制数。支持整数。<br>
代码段的汇编指令编码为 32 位二进制数。支持 4 类共计 10 个指令：<br>

- 算数运算指令
    - add
    - addi
    - slt
- 控制流指令
    - j
    - blez
- 加载存储指令
    - lw
    - li
    - la
    - sw
- 函数调用指令
    - syscall

## 5.2. 实现 NK-CPU 的汇编程序
我是根据 `5.1.` 中的定义写的 `5.3.` 的汇编排序程序，之后在实现汇编程序时，又对 `5.3.` 的程序做了一些调整，其中遇到的最大的一个问题就是数据段数据地址如何表示，以及代码段的程序块标记（主要涉及跳转指令）的地址如何表示，最终的解决方案是编译时对汇编代码遍历两次，第一次先确定这些位置，并且对数据段的数据进行编码存储，第二次对代码段的代码进行编码存储。从而实现整个汇编到机器码的编译。<p>
具体代码参见 `nkcpuAssembler.cpp`。<p>
## 5.3. 使用 NK-CPU 汇编语言编写对 20 个随机数的排序程序
>sort.asm

```MASM
.data #数据段
value: .word 29,23,35,50,15,34,5,41,53,13,86,20,4,6,15,71,8,52,19,37 #变量声明
.text #代码段
main: #主程序
add $s7,$zero,$zero #语句
p1: #程序块
la $v2,value
lw $v0,($v2+$s7)
blez $v1,p6
addi $s6,$s7,1
p2:
lw $v1,($v2+$s6)
blez $v1,p3
slt $t0,$v0,$v1
blez $t0,p5
sw $v0,($v2+$s6)
sw $v1,($v2+$s7)
addi $s6,$s6,1
p5:
j p2
p3:
addi $s7,$s7,1
j p1
p6:
add $s7,$zero,$zero
p7:
li $v0,1
la $v2,value
lw $a0,($v2+$s7)
blez $a0,p8
syscall
add $s7,$s7,1
j p7
p8:
#end with a blank line
```
## 5.4. 编译结果
### 5.4.1. 代码段
>mcode.txt

```
00000000 00000000 10111000 00100000
10000000 00000000 00000000 00000000
10001100 00010111 00010000 00000000
00011000 01100000 00000000 00010100
00100010 11110110 00000000 00000001
10001100 00010110 00011000 00000000
00011000 01100000 00000000 00010001
00000000 01000011 01000000 00101010
00011001 00000000 00000000 00001111
10101100 00010110 00010000 00000000
10101100 00010111 00011000 00000000
00100010 11010110 00000000 00000001
00001000 00000000 00000000 00000111
00100010 11110111 00000000 00000001
00001000 00000000 00000000 00000010
00000000 00000000 10111000 00100000
11110000 00000010 00000000 00000001
10000000 00000000 00000000 00000000
10001100 00010111 00100000 00000000
00011000 10000000 00000000 00011110
00000000 00000000 00000000 00001100
00000010 11100000 10111000 00100000
00001000 00000000 00000000 00010110
00000000 00000000 00000000 00000000
```
### 5.4.2. 数据段
>mdata.txt

```
00000000 00011101
00000000 00010111
00000000 00100011
00000000 00110010
00000000 00001111
00000000 00100010
00000000 00000101
00000000 00101001
00000000 00110101
00000000 00001101
00000000 01010110
00000000 00010100
00000000 00000100
00000000 00000110
00000000 00001111
00000000 01000111
00000000 00001000
00000000 00110100
00000000 00010011
00000000 00100101
```
# 6. 实验心得
<p>最开始没有很理解实验二的实验要求，在与老师沟通之后才重新调整了自己的实验计划，之前浪费了许多无谓的时间。在实验二中，设计 NK-CPU 的汇编语言以及实现汇编器（编译器：将汇编代码翻译成二进制机器码）的过程中，其实设计和思考占了很大的比重，代码实现并不是很困难。让我感到欣喜的一点是，在设计汇编语言和相应的汇编器时，我对计算机体系结构的先辈们的工作有了一个不能说深刻可以说是初步的认识，就是之前学习计算机组成原理和汇编语言时可能并没有很好地理解其中的原理本质的东西，而在这次实验中很多地方都给我“恍然大悟”的感觉，比如： MIPS 中汇编语言与机器码是一一对应的关系，就不像 X86 汇编中是一对多的关系，这样就为编译提供了很大便利，还有像数据的寻址方式以及表示形式，在编程中对这些都有了更加深刻的认识。</p>
<p>在这次实验中对实验一中支持的指令和数据类型有了一些调整，都是根据在本次设计中的实践经验而来，在之后的实验中如果有需要的话都要相应调整，希望最后能让自己满意的完成接下来的实验。</p>

# 7. 参考资料
[1] JohnL.Hennessy, DavidA.Patterson, 亨尼西,等. 计算机体系结构:量化研究方法[M]. 人民邮电出版社, 2013.<br>
[2] c++编写简易mips编译器, <http://blog.csdn.net/zhongzi_l/article/details/51541672>.<br>
[3] 实验1附件 - MIPS Instruction Reference.pdf<br>
