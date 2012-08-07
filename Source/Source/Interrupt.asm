
.extern PITHandler
.extern wheelTimer
.extern PWMHandler3
.extern twitchTimer


.extern sensorStart1
.extern sensorEnd1
.extern sensorStart2
.extern sensorEnd2

.extern __reset
.extern snk

	//room for 4 instructions
//	.section .abs.0x3fa000
//	b __reset
.text
.global FlashH
FlashH:	
	lis r11, bTable@h	//Address of bTable
	ori r11, r11, bTable@l
	
	lis r12, 0x2FC01C@h //Address of SIVEC
	ori r12, r12, 0x2FC01C@l	
	
	lbz r12, 0(r12)
	
	add r12, r11, r12
	mtctr r12
	bctr
	
bTable:

	b snk               		#IRQ0
	b snk				#LVL0
	b sensorEnd1			#IRQ1
	b snk				#LVL1
	b sensorEnd2			#IRQ2
	b snk				#LVL2
	b sensorStart1			#IRQ3
	b snk				#LVL3
	b sensorStart2			#IRQ4
	b snk				#LVL4
	b twitchTimer			#IRQ5
	b snk				#LVL5
	b wheelTimer			#IRQ6
	b snk				#LVL6
	b snk				#IRQ7
	b snk				#LVL7
