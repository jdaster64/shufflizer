.global CharlietonPitPriceListPatchStart
.global CharlietonPitPriceItemPatchStart
.global CharlietonRogueportPriceListPatchStart
.global CharlietonRogueportPriceItemPatchStart
.global CharlietonPitPriceListPatchEnd
.global CharlietonPitPriceItemPatchEnd
.global CharlietonRogueportPriceListPatchEnd
.global CharlietonRogueportPriceItemPatchEnd

CharlietonPitPriceListPatchStart:
lis %r3, 0xccd
subi %r3, %r3, 0x3333
li %r0, 1
mullw %r4, %r4, %r5
mulhw %r23, %r3, %r4
cmpwi %r23, 500
ble+ 0x8
li %r23, 500

CharlietonPitPriceListPatchEnd:
CharlietonPitPriceItemPatchStart:
lis %r5, 0x8031
ori %r0, %r5, 0x08a8
lis %r4, 0xccd
subi %r4, %r4, 0x3333
addi %r5, %r3, 11
add %r3, %r0, %r27
lhz %r0, 0x0014 (%r3)
mullw %r0, %r0, %r5
mulhw %r0, %r4, %r0
cmpwi %r0, 500
ble+ 0x8
li %r0, 500
stw %r0, 0x00a8 (%r31)
lbz %r0, 0x001c (%r3)
extsb %r0, %r0
stw %r0, 0x00ac (%r31)

CharlietonPitPriceItemPatchEnd:
CharlietonRogueportPriceListPatchStart:
lhz %r4, 0x0014 (%r22)
lis %r3, 0x4ccd
subi %r5, %r3, 0x3333
li %r0, 1
rlwinm %r3, %r4, 2, 0, 29
mulhw %r23, %r5, %r3
cmpwi %r23, 1000
ble+ 0x8
li %r23, 999

CharlietonRogueportPriceListPatchEnd:
CharlietonRogueportPriceItemPatchStart:
lis %r5, 0x8031
ori %r0, %r5, 0x08a8
stw %r3, 0x00a4 (%r31)
add %r3, %r0, %r27
lis %r4, 0x4ccd
subi %r4, %r4, 0x3333
lhz %r0, 0x0014 (%r3)
rlwinm %r0, %r0, 2, 0, 29
mulhw %r0, %r4, %r0
cmpwi %r0, 1000
ble+ 0x8
li %r0, 999

CharlietonRogueportPriceItemPatchEnd:
nop