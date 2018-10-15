# Code to prevent Blooper crash by Zephiles.
.global StartPreventBlooperCrash1
.global BranchBackPreventBlooperCrash1
.global StartPreventBlooperCrash2
.global BranchBackPreventBlooperCrash2

# 0x214 in JP
offset = 0x218

StartPreventBlooperCrash1:
mr %r3,%r31 # unkValue
# r4 already contains BattleUnitPointer
bl preventBlooperCrash1

BranchBackPreventBlooperCrash1:
b 0

StartPreventBlooperCrash2:
stwu %sp,-0x10(%sp)
stmw %r30,0x8(%sp)
mr %r31,%r3
mr %r30,%r4

# Check if the BattleUnitPointer is valid or not
mr %r3,%r4 # BattleUnitPointer
bl checkBattleUnitPointer

# Check the returned bool
cmpwi %r3,0 # False
beq- ExitFunction

mr %r3,%r31
mr %r4,%r30
lmw %r30,0x8(%sp)
addi %sp,%sp,0x10

# Restore overwritten assembly
lwz %r5,offset(%r4)

BranchBackPreventBlooperCrash2:
b 0

ExitFunction:
lmw %r28,0x20(%sp)
li %r3,2
lwz %r0,0x34(%sp)
mtlr %r0
addi %sp,%sp,48
blr