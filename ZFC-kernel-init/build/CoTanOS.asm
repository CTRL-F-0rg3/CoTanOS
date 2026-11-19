
ZFC-kernel-init/build/CoTanOS.elf:     file format elf64-littleriscv


Disassembly of section .text.init:

0000000080600000 <_kernel_start>:
    80600000:	10401073          	csrw	sie,zero
    80600004:	842a                	mv	s0,a0
    80600006:	ec15                	bnez	s0,80600042 <_kernel_start+0x42>
    80600008:	00003297          	auipc	t0,0x3
    8060000c:	ff828293          	addi	t0,t0,-8 # 80603000 <__bss_start>
    80600010:	0001f317          	auipc	t1,0x1f
    80600014:	8a030313          	addi	t1,t1,-1888 # 8061e8b0 <__bss_end>
    80600018:	0062f663          	bgeu	t0,t1,80600024 <_kernel_start+0x24>
    8060001c:	0002b023          	sd	zero,0(t0)
    80600020:	02a1                	addi	t0,t0,8
    80600022:	bfdd                	j	80600018 <_kernel_start+0x18>
    80600024:	00023117          	auipc	sp,0x23
    80600028:	88c10113          	addi	sp,sp,-1908 # 806228b0 <__hart_stacks_start>
    8060002c:	4501                	li	a0,0
    8060002e:	4581                	li	a1,0
    80600030:	00000097          	auipc	ra,0x0
    80600034:	2f0080e7          	jalr	752(ra) # 80600320 <cotan_global_init>
    80600038:	4501                	li	a0,0
    8060003a:	00000097          	auipc	ra,0x0
    8060003e:	77e080e7          	jalr	1918(ra) # 806007b8 <kernel_main>
    80600042:	100002b7          	lui	t0,0x10000
    80600046:	03100313          	li	t1,49
    8060004a:	04200393          	li	t2,66
    8060004e:	00628023          	sb	t1,0(t0) # 10000000 <DRAM_SIZE+0x8000000>
    80600052:	00728023          	sb	t2,0(t0)
    80600056:	00023297          	auipc	t0,0x23
    8060005a:	85a28293          	addi	t0,t0,-1958 # 806228b0 <__hart_stacks_start>
    8060005e:	6305                	lui	t1,0x1
    80600060:	02830333          	mul	t1,t1,s0
    80600064:	929a                	add	t0,t0,t1
    80600066:	00628133          	add	sp,t0,t1
    8060006a:	8522                	mv	a0,s0
    8060006c:	00000097          	auipc	ra,0x0
    80600070:	6ea080e7          	jalr	1770(ra) # 80600756 <secondary_hart_init>
    80600074:	10500073          	wfi
    80600078:	bff5                	j	80600074 <_kernel_start+0x74>

Disassembly of section .text.sched:

000000008060007a <sched_init>:
    8060007a:	82aa                	mv	t0,a0
    8060007c:	02029663          	bnez	t0,806000a8 <sched_init+0x2e>
    80600080:	00007317          	auipc	t1,0x7
    80600084:	f9830313          	addi	t1,t1,-104 # 80607018 <g_sched>
    80600088:	02533423          	sd	t0,40(t1)
    8060008c:	00033423          	sd	zero,8(t1)
    80600090:	00033823          	sd	zero,16(t1)
    80600094:	00033c23          	sd	zero,24(t1)
    80600098:	00033023          	sd	zero,0(t1)
    8060009c:	02033c23          	sd	zero,56(t1)
    806000a0:	4385                	li	t2,1
    806000a2:	02733823          	sd	t2,48(t1)
    806000a6:	8082                	ret
    806000a8:	00000317          	auipc	t1,0x0
    806000ac:	26430067          	jr	612(t1) # 8060030c <hart_park>

00000000806000b0 <sched_create>:
    806000b0:	82aa                	mv	t0,a0
    806000b2:	00007317          	auipc	t1,0x7
    806000b6:	fa630313          	addi	t1,t1,-90 # 80607058 <g_tasks>
    806000ba:	00007397          	auipc	t2,0x7
    806000be:	f5e38393          	addi	t2,t2,-162 # 80607018 <g_sched>
    806000c2:	0303be03          	ld	t3,48(t2)
    806000c6:	4ec1                	li	t4,16
    806000c8:	4f01                	li	t5,0
    806000ca:	07df0563          	beq	t5,t4,80600134 <sched_create+0x84>
    806000ce:	0a000593          	li	a1,160
    806000d2:	02be05b3          	mul	a1,t3,a1
    806000d6:	959a                	add	a1,a1,t1
    806000d8:	7db0                	ld	a2,120(a1)
    806000da:	c619                	beqz	a2,806000e8 <sched_create+0x38>
    806000dc:	0e05                	addi	t3,t3,1
    806000de:	01de4363          	blt	t3,t4,806000e4 <sched_create+0x34>
    806000e2:	4e01                	li	t3,0
    806000e4:	0f05                	addi	t5,t5,1
    806000e6:	b7d5                	j	806000ca <sched_create+0x1a>
    806000e8:	4601                	li	a2,0
    806000ea:	46d1                	li	a3,20
    806000ec:	872e                	mv	a4,a1
    806000ee:	e310                	sd	a2,0(a4)
    806000f0:	0721                	addi	a4,a4,8
    806000f2:	16fd                	addi	a3,a3,-1
    806000f4:	feed                	bnez	a3,806000ee <sched_create+0x3e>
    806000f6:	07c5b823          	sd	t3,112(a1)
    806000fa:	4601                	li	a2,0
    806000fc:	fdb0                	sd	a2,120(a1)
    806000fe:	0855b023          	sd	t0,128(a1)
    80600102:	6605                	lui	a2,0x1
    80600104:	02ce0633          	mul	a2,t3,a2
    80600108:	00008697          	auipc	a3,0x8
    8060010c:	95068693          	addi	a3,a3,-1712 # 80607a58 <g_stacks>
    80600110:	96b2                	add	a3,a3,a2
    80600112:	e5d4                	sd	a3,136(a1)
    80600114:	6705                	lui	a4,0x1
    80600116:	96ba                	add	a3,a3,a4
    80600118:	e594                	sd	a3,8(a1)
    8060011a:	00000717          	auipc	a4,0x0
    8060011e:	0f870713          	addi	a4,a4,248 # 80600212 <task_trampoline>
    80600122:	e198                	sd	a4,0(a1)
    80600124:	0e05                	addi	t3,t3,1
    80600126:	01de4363          	blt	t3,t4,8060012c <sched_create+0x7c>
    8060012a:	4e01                	li	t3,0
    8060012c:	03c3b823          	sd	t3,48(t2)
    80600130:	852e                	mv	a0,a1
    80600132:	8082                	ret
    80600134:	4501                	li	a0,0
    80600136:	8082                	ret

0000000080600138 <sched_enqueue>:
    80600138:	08053c23          	sd	zero,152(a0)
    8060013c:	4285                	li	t0,1
    8060013e:	06553c23          	sd	t0,120(a0)
    80600142:	00007297          	auipc	t0,0x7
    80600146:	ede28293          	addi	t0,t0,-290 # 80607020 <g_sched+0x8>
    8060014a:	0082b303          	ld	t1,8(t0)
    8060014e:	00030763          	beqz	t1,8060015c <sched_enqueue+0x24>
    80600152:	08a33c23          	sd	a0,152(t1)
    80600156:	00a2b423          	sd	a0,8(t0)
    8060015a:	a029                	j	80600164 <sched_enqueue+0x2c>
    8060015c:	00a2b023          	sd	a0,0(t0)
    80600160:	00a2b423          	sd	a0,8(t0)
    80600164:	0102b303          	ld	t1,16(t0)
    80600168:	0305                	addi	t1,t1,1
    8060016a:	0062b823          	sd	t1,16(t0)
    8060016e:	8082                	ret

0000000080600170 <sched_next>:
    80600170:	00007297          	auipc	t0,0x7
    80600174:	eb028293          	addi	t0,t0,-336 # 80607020 <g_sched+0x8>
    80600178:	0002b303          	ld	t1,0(t0)
    8060017c:	02030363          	beqz	t1,806001a2 <sched_next+0x32>
    80600180:	09833383          	ld	t2,152(t1)
    80600184:	0072b023          	sd	t2,0(t0)
    80600188:	00039463          	bnez	t2,80600190 <sched_next+0x20>
    8060018c:	0002b423          	sd	zero,8(t0)
    80600190:	0102be03          	ld	t3,16(t0)
    80600194:	1e7d                	addi	t3,t3,-1
    80600196:	01c2b823          	sd	t3,16(t0)
    8060019a:	08033c23          	sd	zero,152(t1)
    8060019e:	851a                	mv	a0,t1
    806001a0:	8082                	ret
    806001a2:	4501                	li	a0,0
    806001a4:	8082                	ret

00000000806001a6 <context_switch>:
    806001a6:	c91d                	beqz	a0,806001dc <context_switch+0x36>
    806001a8:	00153023          	sd	ra,0(a0)
    806001ac:	00253423          	sd	sp,8(a0)
    806001b0:	e900                	sd	s0,16(a0)
    806001b2:	ed04                	sd	s1,24(a0)
    806001b4:	03253023          	sd	s2,32(a0)
    806001b8:	03353423          	sd	s3,40(a0)
    806001bc:	03453823          	sd	s4,48(a0)
    806001c0:	03553c23          	sd	s5,56(a0)
    806001c4:	05653023          	sd	s6,64(a0)
    806001c8:	05753423          	sd	s7,72(a0)
    806001cc:	05853823          	sd	s8,80(a0)
    806001d0:	05953c23          	sd	s9,88(a0)
    806001d4:	07a53023          	sd	s10,96(a0)
    806001d8:	07b53423          	sd	s11,104(a0)
    806001dc:	0005b083          	ld	ra,0(a1)
    806001e0:	0085b103          	ld	sp,8(a1)
    806001e4:	6980                	ld	s0,16(a1)
    806001e6:	6d84                	ld	s1,24(a1)
    806001e8:	0205b903          	ld	s2,32(a1)
    806001ec:	0285b983          	ld	s3,40(a1)
    806001f0:	0305ba03          	ld	s4,48(a1)
    806001f4:	0385ba83          	ld	s5,56(a1)
    806001f8:	0405bb03          	ld	s6,64(a1)
    806001fc:	0485bb83          	ld	s7,72(a1)
    80600200:	0505bc03          	ld	s8,80(a1)
    80600204:	0585bc83          	ld	s9,88(a1)
    80600208:	0605bd03          	ld	s10,96(a1)
    8060020c:	0685bd83          	ld	s11,104(a1)
    80600210:	8082                	ret

0000000080600212 <task_trampoline>:
    80600212:	00007297          	auipc	t0,0x7
    80600216:	e0628293          	addi	t0,t0,-506 # 80607018 <g_sched>
    8060021a:	0002b283          	ld	t0,0(t0)
    8060021e:	0802b303          	ld	t1,128(t0)
    80600222:	9302                	jalr	t1
    80600224:	00007297          	auipc	t0,0x7
    80600228:	df428293          	addi	t0,t0,-524 # 80607018 <g_sched>
    8060022c:	0002b283          	ld	t0,0(t0)
    80600230:	4311                	li	t1,4
    80600232:	0662bc23          	sd	t1,120(t0)
    80600236:	00000097          	auipc	ra,0x0
    8060023a:	00a080e7          	jalr	10(ra) # 80600240 <sched_yield>
    8060023e:	bfe5                	j	80600236 <task_trampoline+0x24>

0000000080600240 <sched_yield>:
    80600240:	1101                	addi	sp,sp,-32
    80600242:	e006                	sd	ra,0(sp)
    80600244:	e422                	sd	s0,8(sp)
    80600246:	e826                	sd	s1,16(sp)
    80600248:	00007417          	auipc	s0,0x7
    8060024c:	dd040413          	addi	s0,s0,-560 # 80607018 <g_sched>
    80600250:	6004                	ld	s1,0(s0)
    80600252:	cc91                	beqz	s1,8060026e <sched_yield+0x2e>
    80600254:	0784b283          	ld	t0,120(s1)
    80600258:	4311                	li	t1,4
    8060025a:	00628a63          	beq	t0,t1,8060026e <sched_yield+0x2e>
    8060025e:	4285                	li	t0,1
    80600260:	0654bc23          	sd	t0,120(s1)
    80600264:	8526                	mv	a0,s1
    80600266:	00000097          	auipc	ra,0x0
    8060026a:	ed2080e7          	jalr	-302(ra) # 80600138 <sched_enqueue>
    8060026e:	00000097          	auipc	ra,0x0
    80600272:	f02080e7          	jalr	-254(ra) # 80600170 <sched_next>
    80600276:	c10d                	beqz	a0,80600298 <sched_yield+0x58>
    80600278:	00007297          	auipc	t0,0x7
    8060027c:	da028293          	addi	t0,t0,-608 # 80607018 <g_sched>
    80600280:	00a2b023          	sd	a0,0(t0)
    80600284:	4309                	li	t1,2
    80600286:	06653c23          	sd	t1,120(a0)
    8060028a:	85aa                	mv	a1,a0
    8060028c:	8526                	mv	a0,s1
    8060028e:	00000097          	auipc	ra,0x0
    80600292:	f18080e7          	jalr	-232(ra) # 806001a6 <context_switch>
    80600296:	a025                	j	806002be <sched_yield+0x7e>
    80600298:	00043283          	ld	t0,0(s0)
    8060029c:	00028863          	beqz	t0,806002ac <sched_yield+0x6c>
    806002a0:	0782b303          	ld	t1,120(t0)
    806002a4:	4391                	li	t2,4
    806002a6:	00730363          	beq	t1,t2,806002ac <sched_yield+0x6c>
    806002aa:	a811                	j	806002be <sched_yield+0x7e>
    806002ac:	00007297          	auipc	t0,0x7
    806002b0:	d6c28293          	addi	t0,t0,-660 # 80607018 <g_sched>
    806002b4:	0002b023          	sd	zero,0(t0)
    806002b8:	10500073          	wfi
    806002bc:	bff5                	j	806002b8 <sched_yield+0x78>
    806002be:	6082                	ld	ra,0(sp)
    806002c0:	6422                	ld	s0,8(sp)
    806002c2:	64c2                	ld	s1,16(sp)
    806002c4:	6105                	addi	sp,sp,32
    806002c6:	8082                	ret

00000000806002c8 <sched_start>:
    806002c8:	00007297          	auipc	t0,0x7
    806002cc:	d5028293          	addi	t0,t0,-688 # 80607018 <g_sched>
    806002d0:	4305                	li	t1,1
    806002d2:	0262bc23          	sd	t1,56(t0)
    806002d6:	00000097          	auipc	ra,0x0
    806002da:	f6a080e7          	jalr	-150(ra) # 80600240 <sched_yield>
    806002de:	bfe5                	j	806002d6 <sched_start+0xe>

00000000806002e0 <sched_tick>:
    806002e0:	00007297          	auipc	t0,0x7
    806002e4:	d3828293          	addi	t0,t0,-712 # 80607018 <g_sched>
    806002e8:	0382b303          	ld	t1,56(t0)
    806002ec:	00030f63          	beqz	t1,8060030a <sched_tick+0x2a>
    806002f0:	0002b303          	ld	t1,0(t0)
    806002f4:	00030763          	beqz	t1,80600302 <sched_tick+0x22>
    806002f8:	09033383          	ld	t2,144(t1)
    806002fc:	0385                	addi	t2,t2,1
    806002fe:	08733823          	sd	t2,144(t1)
    80600302:	00000317          	auipc	t1,0x0
    80600306:	f3e30067          	jr	-194(t1) # 80600240 <sched_yield>
    8060030a:	8082                	ret

000000008060030c <hart_park>:
    8060030c:	10401073          	csrw	sie,zero
    80600310:	14401073          	csrw	sip,zero
    80600314:	10017073          	csrci	sstatus,2
    80600318:	10500073          	wfi
    8060031c:	bff5                	j	80600318 <hart_park+0xc>

Disassembly of section .text:

0000000080600320 <cotan_global_init>:
    80600320:	e539                	bnez	a0,8060036e <cotan_global_init+0x4e>
    80600322:	00003297          	auipc	t0,0x3
    80600326:	cde28293          	addi	t0,t0,-802 # 80603000 <__bss_start>
    8060032a:	021b8337          	lui	t1,0x21b8
    8060032e:	aa33031b          	addiw	t1,t1,-1373 # 21b7aa3 <MEGAPAGE_SIZE+0x1fb7aa3>
    80600332:	0332                	slli	t1,t1,0xc
    80600334:	0b730313          	addi	t1,t1,183
    80600338:	0336                	slli	t1,t1,0xd
    8060033a:	4f530313          	addi	t1,t1,1269
    8060033e:	0332                	slli	t1,t1,0xc
    80600340:	33130313          	addi	t1,t1,817
    80600344:	0062b023          	sd	t1,0(t0)
    80600348:	0010031b          	addiw	t1,zero,1
    8060034c:	1302                	slli	t1,t1,0x20
    8060034e:	0062b423          	sd	t1,8(t0)
    80600352:	02a2b023          	sd	a0,32(t0)
    80600356:	0ab2b023          	sd	a1,160(t0)
    8060035a:	4305                	li	t1,1
    8060035c:	0262b423          	sd	t1,40(t0)
    80600360:	0002b823          	sd	zero,16(t0)
    80600364:	0002bc23          	sd	zero,24(t0)
    80600368:	8216                	mv	tp,t0
    8060036a:	4501                	li	a0,0
    8060036c:	8082                	ret
    8060036e:	557d                	li	a0,-1
    80600370:	8082                	ret

0000000080600372 <cotan_panic_halt>:
    80600372:	10a23c23          	sd	a0,280(tp) # 118 <COTAN_OFF_PANIC_PHASE>
    80600376:	141022f3          	csrr	t0,sepc
    8060037a:	10523423          	sd	t0,264(tp) # 108 <COTAN_OFF_PANIC_RA>
    8060037e:	c00022f3          	rdcycle	t0
    80600382:	0c523023          	sd	t0,192(tp) # c0 <COTAN_OFF_HART_DATA>
    80600386:	c19d                	beqz	a1,806003ac <cotan_panic_halt+0x3a>
    80600388:	12020293          	addi	t0,tp,288 # 120 <COTAN_OFF_PANIC_MSG>
    8060038c:	832e                	mv	t1,a1
    8060038e:	03f00393          	li	t2,63
    80600392:	00034e03          	lbu	t3,0(t1)
    80600396:	01c28023          	sb	t3,0(t0)
    8060039a:	000e0963          	beqz	t3,806003ac <cotan_panic_halt+0x3a>
    8060039e:	0285                	addi	t0,t0,1
    806003a0:	0305                	addi	t1,t1,1
    806003a2:	13fd                	addi	t2,t2,-1
    806003a4:	fe0397e3          	bnez	t2,80600392 <cotan_panic_halt+0x20>
    806003a8:	00028023          	sb	zero,0(t0)
    806003ac:	01823283          	ld	t0,24(tp) # 18 <COTAN_OFF_FLAGS>
    806003b0:	4305                	li	t1,1
    806003b2:	0062e2b3          	or	t0,t0,t1
    806003b6:	00523c23          	sd	t0,24(tp) # 18 <COTAN_OFF_FLAGS>
    806003ba:	10500073          	wfi
    806003be:	bff5                	j	806003ba <cotan_panic_halt+0x48>

00000000806003c0 <mm_init>:
    806003c0:	00004297          	auipc	t0,0x4
    806003c4:	c4028293          	addi	t0,t0,-960 # 80604000 <g_root_pt>
    806003c8:	00005317          	auipc	t1,0x5
    806003cc:	c3830313          	addi	t1,t1,-968 # 80605000 <g_l1_dram>
    806003d0:	4381                	li	t2,0
    806003d2:	40000e13          	li	t3,1024
    806003d6:	8e96                	mv	t4,t0
    806003d8:	007eb023          	sd	t2,0(t4)
    806003dc:	0ea1                	addi	t4,t4,8
    806003de:	1e7d                	addi	t3,t3,-1
    806003e0:	fe0e1ce3          	bnez	t3,806003d8 <mm_init+0x18>
    806003e4:	0c700e13          	li	t3,199
    806003e8:	01c2b023          	sd	t3,0(t0)
    806003ec:	00c35e13          	srli	t3,t1,0xc
    806003f0:	0e2a                	slli	t3,t3,0xa
    806003f2:	001e6e13          	ori	t3,t3,1
    806003f6:	01c2b823          	sd	t3,16(t0)
    806003fa:	8082                	ret

00000000806003fc <mm_map_megapage>:
    806003fc:	01555293          	srli	t0,a0,0x15
    80600400:	1ff2f293          	andi	t0,t0,511
    80600404:	028e                	slli	t0,t0,0x3
    80600406:	01e55313          	srli	t1,a0,0x1e
    8060040a:	1ff37313          	andi	t1,t1,511
    8060040e:	030e                	slli	t1,t1,0x3
    80600410:	00004397          	auipc	t2,0x4
    80600414:	bf038393          	addi	t2,t2,-1040 # 80604000 <g_root_pt>
    80600418:	939a                	add	t2,t2,t1
    8060041a:	0003be03          	ld	t3,0(t2)
    8060041e:	00ae5e13          	srli	t3,t3,0xa
    80600422:	0e32                	slli	t3,t3,0xc
    80600424:	9e16                	add	t3,t3,t0
    80600426:	00c5de93          	srli	t4,a1,0xc
    8060042a:	0eaa                	slli	t4,t4,0xa
    8060042c:	00ceeeb3          	or	t4,t4,a2
    80600430:	01de3023          	sd	t4,0(t3)
    80600434:	8082                	ret

0000000080600436 <mm_map_kernel>:
    80600436:	7179                	addi	sp,sp,-48
    80600438:	e006                	sd	ra,0(sp)
    8060043a:	e422                	sd	s0,8(sp)
    8060043c:	e826                	sd	s1,16(sp)
    8060043e:	ec4a                	sd	s2,24(sp)
    80600440:	f04e                	sd	s3,32(sp)
    80600442:	0010041b          	addiw	s0,zero,1
    80600446:	047e                	slli	s0,s0,0x1f
    80600448:	0110049b          	addiw	s1,zero,17
    8060044c:	04ee                	slli	s1,s1,0x1b
    8060044e:	00200937          	lui	s2,0x200
    80600452:	0ef00993          	li	s3,239
    80600456:	00947b63          	bgeu	s0,s1,8060046c <mm_map_kernel+0x36>
    8060045a:	8522                	mv	a0,s0
    8060045c:	85a2                	mv	a1,s0
    8060045e:	864e                	mv	a2,s3
    80600460:	00000097          	auipc	ra,0x0
    80600464:	f9c080e7          	jalr	-100(ra) # 806003fc <mm_map_megapage>
    80600468:	944a                	add	s0,s0,s2
    8060046a:	b7f5                	j	80600456 <mm_map_kernel+0x20>
    8060046c:	6082                	ld	ra,0(sp)
    8060046e:	6422                	ld	s0,8(sp)
    80600470:	64c2                	ld	s1,16(sp)
    80600472:	6962                	ld	s2,24(sp)
    80600474:	7982                	ld	s3,32(sp)
    80600476:	6145                	addi	sp,sp,48
    80600478:	8082                	ret

000000008060047a <mm_enable_paging>:
    8060047a:	00004297          	auipc	t0,0x4
    8060047e:	b8628293          	addi	t0,t0,-1146 # 80604000 <g_root_pt>
    80600482:	00c2d293          	srli	t0,t0,0xc
    80600486:	fff0031b          	addiw	t1,zero,-1
    8060048a:	137e                	slli	t1,t1,0x3f
    8060048c:	0062e2b3          	or	t0,t0,t1
    80600490:	12000073          	sfence.vma
    80600494:	18029073          	csrw	satp,t0
    80600498:	12000073          	sfence.vma
    8060049c:	8082                	ret

000000008060049e <mm_disable_paging>:
    8060049e:	12000073          	sfence.vma
    806004a2:	18001073          	csrw	satp,zero
    806004a6:	12000073          	sfence.vma
    806004aa:	8082                	ret

00000000806004ac <pmm_init>:
    806004ac:	1141                	addi	sp,sp,-16
    806004ae:	e022                	sd	s0,0(sp)
    806004b0:	e426                	sd	s1,8(sp)
    806004b2:	00007297          	auipc	t0,0x7
    806004b6:	b4e28293          	addi	t0,t0,-1202 # 80607000 <g_pmm_base>
    806004ba:	00a2b023          	sd	a0,0(t0)
    806004be:	6305                	lui	t1,0x1
    806004c0:	0265d433          	divu	s0,a1,t1
    806004c4:	00007297          	auipc	t0,0x7
    806004c8:	b4428293          	addi	t0,t0,-1212 # 80607008 <g_pmm_pages>
    806004cc:	0082b023          	sd	s0,0(t0)
    806004d0:	00007297          	auipc	t0,0x7
    806004d4:	b4028293          	addi	t0,t0,-1216 # 80607010 <g_pmm_free>
    806004d8:	0082b023          	sd	s0,0(t0)
    806004dc:	00006297          	auipc	t0,0x6
    806004e0:	b2428293          	addi	t0,t0,-1244 # 80606000 <g_pmm_bitmap>
    806004e4:	4381                	li	t2,0
    806004e6:	20000e13          	li	t3,512
    806004ea:	0072b023          	sd	t2,0(t0)
    806004ee:	02a1                	addi	t0,t0,8
    806004f0:	1e7d                	addi	t3,t3,-1
    806004f2:	fe0e1ce3          	bnez	t3,806004ea <pmm_init+0x3e>
    806004f6:	8322                	mv	t1,s0
    806004f8:	63a1                	lui	t2,0x8
    806004fa:	00006297          	auipc	t0,0x6
    806004fe:	b0628293          	addi	t0,t0,-1274 # 80606000 <g_pmm_bitmap>
    80600502:	02737463          	bgeu	t1,t2,8060052a <pmm_init+0x7e>
    80600506:	00635e13          	srli	t3,t1,0x6
    8060050a:	03f37e93          	andi	t4,t1,63
    8060050e:	003e1f13          	slli	t5,t3,0x3
    80600512:	9f16                	add	t5,t5,t0
    80600514:	000f3f83          	ld	t6,0(t5)
    80600518:	4e05                	li	t3,1
    8060051a:	01de1e33          	sll	t3,t3,t4
    8060051e:	01cfefb3          	or	t6,t6,t3
    80600522:	01ff3023          	sd	t6,0(t5)
    80600526:	0305                	addi	t1,t1,1 # 1001 <PAGE_SIZE+0x1>
    80600528:	bfe9                	j	80600502 <pmm_init+0x56>
    8060052a:	6402                	ld	s0,0(sp)
    8060052c:	64a2                	ld	s1,8(sp)
    8060052e:	0141                	addi	sp,sp,16
    80600530:	8082                	ret

0000000080600532 <pmm_alloc_page>:
    80600532:	00006297          	auipc	t0,0x6
    80600536:	ace28293          	addi	t0,t0,-1330 # 80606000 <g_pmm_bitmap>
    8060053a:	4381                	li	t2,0
    8060053c:	20000413          	li	s0,512
    80600540:	0883f163          	bgeu	t2,s0,806005c2 <pmm_alloc_page+0x90>
    80600544:	00339e93          	slli	t4,t2,0x3
    80600548:	9e96                	add	t4,t4,t0
    8060054a:	000ebf03          	ld	t5,0(t4)
    8060054e:	ffff4f13          	not	t5,t5
    80600552:	060f0663          	beqz	t5,806005be <pmm_alloc_page+0x8c>
    80600556:	4f81                	li	t6,0
    80600558:	001f7e93          	andi	t4,t5,1
    8060055c:	000e9a63          	bnez	t4,80600570 <pmm_alloc_page+0x3e>
    80600560:	001f5f13          	srli	t5,t5,0x1
    80600564:	0f85                	addi	t6,t6,1
    80600566:	04000e93          	li	t4,64
    8060056a:	05dffa63          	bgeu	t6,t4,806005be <pmm_alloc_page+0x8c>
    8060056e:	b7ed                	j	80600558 <pmm_alloc_page+0x26>
    80600570:	00639f13          	slli	t5,t2,0x6
    80600574:	9f7e                	add	t5,t5,t6
    80600576:	006f5e13          	srli	t3,t5,0x6
    8060057a:	03ff7e93          	andi	t4,t5,63
    8060057e:	003e1f93          	slli	t6,t3,0x3
    80600582:	9f96                	add	t6,t6,t0
    80600584:	000fbe03          	ld	t3,0(t6)
    80600588:	4385                	li	t2,1
    8060058a:	01d393b3          	sll	t2,t2,t4
    8060058e:	007e6e33          	or	t3,t3,t2
    80600592:	01cfb023          	sd	t3,0(t6)
    80600596:	00007e17          	auipc	t3,0x7
    8060059a:	a7ae0e13          	addi	t3,t3,-1414 # 80607010 <g_pmm_free>
    8060059e:	000e3e83          	ld	t4,0(t3)
    806005a2:	1efd                	addi	t4,t4,-1
    806005a4:	01de3023          	sd	t4,0(t3)
    806005a8:	00007e17          	auipc	t3,0x7
    806005ac:	a58e0e13          	addi	t3,t3,-1448 # 80607000 <g_pmm_base>
    806005b0:	000e3e03          	ld	t3,0(t3)
    806005b4:	00cf1e93          	slli	t4,t5,0xc
    806005b8:	01de0533          	add	a0,t3,t4
    806005bc:	8082                	ret
    806005be:	0385                	addi	t2,t2,1 # 8001 <PMM_MAX_PAGES+0x1>
    806005c0:	b741                	j	80600540 <pmm_alloc_page+0xe>
    806005c2:	4501                	li	a0,0
    806005c4:	8082                	ret

00000000806005c6 <pmm_free_page>:
    806005c6:	c931                	beqz	a0,8060061a <pmm_free_page+0x54>
    806005c8:	00007297          	auipc	t0,0x7
    806005cc:	a3828293          	addi	t0,t0,-1480 # 80607000 <g_pmm_base>
    806005d0:	0002b283          	ld	t0,0(t0)
    806005d4:	40550333          	sub	t1,a0,t0
    806005d8:	00c35313          	srli	t1,t1,0xc
    806005dc:	00635e13          	srli	t3,t1,0x6
    806005e0:	03f37e93          	andi	t4,t1,63
    806005e4:	00006f17          	auipc	t5,0x6
    806005e8:	a1cf0f13          	addi	t5,t5,-1508 # 80606000 <g_pmm_bitmap>
    806005ec:	003e1f93          	slli	t6,t3,0x3
    806005f0:	9f7e                	add	t5,t5,t6
    806005f2:	000f3f83          	ld	t6,0(t5)
    806005f6:	4e05                	li	t3,1
    806005f8:	01de1e33          	sll	t3,t3,t4
    806005fc:	fffe4e13          	not	t3,t3
    80600600:	01cfffb3          	and	t6,t6,t3
    80600604:	01ff3023          	sd	t6,0(t5)
    80600608:	00007e17          	auipc	t3,0x7
    8060060c:	a08e0e13          	addi	t3,t3,-1528 # 80607010 <g_pmm_free>
    80600610:	000e3e83          	ld	t4,0(t3)
    80600614:	0e85                	addi	t4,t4,1
    80600616:	01de3023          	sd	t4,0(t3)
    8060061a:	8082                	ret

000000008060061c <pmm_mark_used>:
    8060061c:	cdbd                	beqz	a1,8060069a <pmm_mark_used+0x7e>
    8060061e:	1141                	addi	sp,sp,-16
    80600620:	e022                	sd	s0,0(sp)
    80600622:	e426                	sd	s1,8(sp)
    80600624:	00007297          	auipc	t0,0x7
    80600628:	9dc28293          	addi	t0,t0,-1572 # 80607000 <g_pmm_base>
    8060062c:	0002b283          	ld	t0,0(t0)
    80600630:	40550333          	sub	t1,a0,t0
    80600634:	00c35413          	srli	s0,t1,0xc
    80600638:	00b503b3          	add	t2,a0,a1
    8060063c:	405383b3          	sub	t2,t2,t0
    80600640:	6e05                	lui	t3,0x1
    80600642:	3e7d                	addiw	t3,t3,-1 # fff <PMM_BITMAP_WORDS+0xdff>
    80600644:	93f2                	add	t2,t2,t3
    80600646:	00c3d493          	srli	s1,t2,0xc
    8060064a:	04947563          	bgeu	s0,s1,80600694 <pmm_mark_used+0x78>
    8060064e:	00645e13          	srli	t3,s0,0x6
    80600652:	03f47e93          	andi	t4,s0,63
    80600656:	00006f17          	auipc	t5,0x6
    8060065a:	9aaf0f13          	addi	t5,t5,-1622 # 80606000 <g_pmm_bitmap>
    8060065e:	003e1f93          	slli	t6,t3,0x3
    80600662:	9f7e                	add	t5,t5,t6
    80600664:	000f3f83          	ld	t6,0(t5)
    80600668:	4e05                	li	t3,1
    8060066a:	01de1e33          	sll	t3,t3,t4
    8060066e:	01cffeb3          	and	t4,t6,t3
    80600672:	01cfefb3          	or	t6,t6,t3
    80600676:	01ff3023          	sd	t6,0(t5)
    8060067a:	000e9b63          	bnez	t4,80600690 <pmm_mark_used+0x74>
    8060067e:	00007f17          	auipc	t5,0x7
    80600682:	992f0f13          	addi	t5,t5,-1646 # 80607010 <g_pmm_free>
    80600686:	000f3f83          	ld	t6,0(t5)
    8060068a:	1ffd                	addi	t6,t6,-1
    8060068c:	01ff3023          	sd	t6,0(t5)
    80600690:	0405                	addi	s0,s0,1
    80600692:	bf65                	j	8060064a <pmm_mark_used+0x2e>
    80600694:	6402                	ld	s0,0(sp)
    80600696:	64a2                	ld	s1,8(sp)
    80600698:	0141                	addi	sp,sp,16
    8060069a:	8082                	ret

000000008060069c <trap_vector>:
    8060069c:	14202573          	csrr	a0,scause
    806006a0:	141025f3          	csrr	a1,sepc
    806006a4:	14302673          	csrr	a2,stval
    806006a8:	00000097          	auipc	ra,0x0
    806006ac:	5d6080e7          	jalr	1494(ra) # 80600c7e <c_trap_handler>
    806006b0:	10500073          	wfi
    806006b4:	bff5                	j	806006b0 <trap_vector+0x14>

00000000806006b6 <trap_init>:
    806006b6:	00000297          	auipc	t0,0x0
    806006ba:	fe628293          	addi	t0,t0,-26 # 8060069c <trap_vector>
    806006be:	10529073          	csrw	stvec,t0
    806006c2:	8082                	ret

00000000806006c4 <task_a>:
    806006c4:	1141                	addi	sp,sp,-16
    806006c6:	00002517          	auipc	a0,0x2
    806006ca:	df250513          	addi	a0,a0,-526 # 806024b8 <kexec+0x6>
    806006ce:	e406                	sd	ra,8(sp)
    806006d0:	00000097          	auipc	ra,0x0
    806006d4:	686080e7          	jalr	1670(ra) # 80600d56 <uart_puts>
    806006d8:	00000097          	auipc	ra,0x0
    806006dc:	b68080e7          	jalr	-1176(ra) # 80600240 <sched_yield>
    806006e0:	bfe5                	j	806006d8 <task_a+0x14>

00000000806006e2 <task_b>:
    806006e2:	1141                	addi	sp,sp,-16
    806006e4:	00002517          	auipc	a0,0x2
    806006e8:	dec50513          	addi	a0,a0,-532 # 806024d0 <kexec+0x1e>
    806006ec:	e406                	sd	ra,8(sp)
    806006ee:	00000097          	auipc	ra,0x0
    806006f2:	668080e7          	jalr	1640(ra) # 80600d56 <uart_puts>
    806006f6:	00000097          	auipc	ra,0x0
    806006fa:	b4a080e7          	jalr	-1206(ra) # 80600240 <sched_yield>
    806006fe:	bfe5                	j	806006f6 <task_b+0x14>

0000000080600700 <wake_hart.constprop.0>:
    80600700:	1141                	addi	sp,sp,-16
    80600702:	00002517          	auipc	a0,0x2
    80600706:	de650513          	addi	a0,a0,-538 # 806024e8 <kexec+0x36>
    8060070a:	e406                	sd	ra,8(sp)
    8060070c:	00000097          	auipc	ra,0x0
    80600710:	64a080e7          	jalr	1610(ra) # 80600d56 <uart_puts>
    80600714:	03100513          	li	a0,49
    80600718:	00000097          	auipc	ra,0x0
    8060071c:	61a080e7          	jalr	1562(ra) # 80600d32 <uart_putc>
    80600720:	00002517          	auipc	a0,0x2
    80600724:	df050513          	addi	a0,a0,-528 # 80602510 <kexec+0x5e>
    80600728:	00000097          	auipc	ra,0x0
    8060072c:	62e080e7          	jalr	1582(ra) # 80600d56 <uart_puts>
    80600730:	73445837          	lui	a6,0x73445
    80600734:	95080813          	addi	a6,a6,-1712 # 73444950 <DRAM_SIZE+0x6b444950>
    80600738:	4509                	li	a0,2
    8060073a:	4581                	li	a1,0
    8060073c:	4881                	li	a7,0
    8060073e:	00000073          	ecall
    80600742:	60a2                	ld	ra,8(sp)
    80600744:	00002517          	auipc	a0,0x2
    80600748:	dd450513          	addi	a0,a0,-556 # 80602518 <kexec+0x66>
    8060074c:	0141                	addi	sp,sp,16
    8060074e:	00000317          	auipc	t1,0x0
    80600752:	60830067          	jr	1544(t1) # 80600d56 <uart_puts>

0000000080600756 <secondary_hart_init>:
    80600756:	1141                	addi	sp,sp,-16
    80600758:	e022                	sd	s0,0(sp)
    8060075a:	842a                	mv	s0,a0
    8060075c:	00002517          	auipc	a0,0x2
    80600760:	dd450513          	addi	a0,a0,-556 # 80602530 <kexec+0x7e>
    80600764:	e406                	sd	ra,8(sp)
    80600766:	00000097          	auipc	ra,0x0
    8060076a:	5f0080e7          	jalr	1520(ra) # 80600d56 <uart_puts>
    8060076e:	8522                	mv	a0,s0
    80600770:	00000097          	auipc	ra,0x0
    80600774:	618080e7          	jalr	1560(ra) # 80600d88 <uart_hex>
    80600778:	00002517          	auipc	a0,0x2
    8060077c:	dd850513          	addi	a0,a0,-552 # 80602550 <kexec+0x9e>
    80600780:	00000097          	auipc	ra,0x0
    80600784:	5d6080e7          	jalr	1494(ra) # 80600d56 <uart_puts>
    80600788:	00002517          	auipc	a0,0x2
    8060078c:	de850513          	addi	a0,a0,-536 # 80602570 <kexec+0xbe>
    80600790:	00000097          	auipc	ra,0x0
    80600794:	5c6080e7          	jalr	1478(ra) # 80600d56 <uart_puts>
    80600798:	00000097          	auipc	ra,0x0
    8060079c:	f68080e7          	jalr	-152(ra) # 80600700 <wake_hart.constprop.0>
    806007a0:	00002517          	auipc	a0,0x2
    806007a4:	df850513          	addi	a0,a0,-520 # 80602598 <kexec+0xe6>
    806007a8:	00000097          	auipc	ra,0x0
    806007ac:	5ae080e7          	jalr	1454(ra) # 80600d56 <uart_puts>
    806007b0:	00000097          	auipc	ra,0x0
    806007b4:	b18080e7          	jalr	-1256(ra) # 806002c8 <sched_start>

00000000806007b8 <kernel_main>:
    806007b8:	1101                	addi	sp,sp,-32
    806007ba:	ec06                	sd	ra,24(sp)
    806007bc:	e822                	sd	s0,16(sp)
    806007be:	e426                	sd	s1,8(sp)
    806007c0:	842a                	mv	s0,a0
    806007c2:	e04a                	sd	s2,0(sp)
    806007c4:	00000097          	auipc	ra,0x0
    806007c8:	ef2080e7          	jalr	-270(ra) # 806006b6 <trap_init>
    806007cc:	00002517          	auipc	a0,0x2
    806007d0:	df450513          	addi	a0,a0,-524 # 806025c0 <kexec+0x10e>
    806007d4:	00000097          	auipc	ra,0x0
    806007d8:	582080e7          	jalr	1410(ra) # 80600d56 <uart_puts>
    806007dc:	8522                	mv	a0,s0
    806007de:	00000097          	auipc	ra,0x0
    806007e2:	5aa080e7          	jalr	1450(ra) # 80600d88 <uart_hex>
    806007e6:	4529                	li	a0,10
    806007e8:	00000097          	auipc	ra,0x0
    806007ec:	54a080e7          	jalr	1354(ra) # 80600d32 <uart_putc>
    806007f0:	8412                	mv	s0,tp
    806007f2:	00002517          	auipc	a0,0x2
    806007f6:	df650513          	addi	a0,a0,-522 # 806025e8 <kexec+0x136>
    806007fa:	00000097          	auipc	ra,0x0
    806007fe:	55c080e7          	jalr	1372(ra) # 80600d56 <uart_puts>
    80600802:	8522                	mv	a0,s0
    80600804:	00000097          	auipc	ra,0x0
    80600808:	584080e7          	jalr	1412(ra) # 80600d88 <uart_hex>
    8060080c:	4529                	li	a0,10
    8060080e:	00000097          	auipc	ra,0x0
    80600812:	524080e7          	jalr	1316(ra) # 80600d32 <uart_putc>
    80600816:	fffff717          	auipc	a4,0xfffff
    8060081a:	7ea70713          	addi	a4,a4,2026 # 80600000 <KERNEL_BASE>
    8060081e:	46a1                	li	a3,8
    80600820:	04d405a3          	sb	a3,75(s0)
    80600824:	00875693          	srli	a3,a4,0x8
    80600828:	04d408a3          	sb	a3,81(s0)
    8060082c:	01075693          	srli	a3,a4,0x10
    80600830:	04d40923          	sb	a3,82(s0)
    80600834:	0187569b          	srliw	a3,a4,0x18
    80600838:	04d409a3          	sb	a3,83(s0)
    8060083c:	02075693          	srli	a3,a4,0x20
    80600840:	04d40a23          	sb	a3,84(s0)
    80600844:	02875693          	srli	a3,a4,0x28
    80600848:	0002c797          	auipc	a5,0x2c
    8060084c:	06878793          	addi	a5,a5,104 # 8062c8b0 <__sched_data_end>
    80600850:	04d40aa3          	sb	a3,85(s0)
    80600854:	03075693          	srli	a3,a4,0x30
    80600858:	04d40b23          	sb	a3,86(s0)
    8060085c:	04e40823          	sb	a4,80(s0)
    80600860:	03875693          	srli	a3,a4,0x38
    80600864:	0087d713          	srli	a4,a5,0x8
    80600868:	04e40ca3          	sb	a4,89(s0)
    8060086c:	0107d713          	srli	a4,a5,0x10
    80600870:	04e40d23          	sb	a4,90(s0)
    80600874:	0187d71b          	srliw	a4,a5,0x18
    80600878:	04e40da3          	sb	a4,91(s0)
    8060087c:	0207d713          	srli	a4,a5,0x20
    80600880:	04e40e23          	sb	a4,92(s0)
    80600884:	0287d713          	srli	a4,a5,0x28
    80600888:	04e40ea3          	sb	a4,93(s0)
    8060088c:	0307d713          	srli	a4,a5,0x30
    80600890:	04e40f23          	sb	a4,94(s0)
    80600894:	04f40c23          	sb	a5,88(s0)
    80600898:	0387d713          	srli	a4,a5,0x38
    8060089c:	f8000793          	li	a5,-128
    806008a0:	4905                	li	s2,1
    806008a2:	04d40ba3          	sb	a3,87(s0)
    806008a6:	04e40fa3          	sb	a4,95(s0)
    806008aa:	f9600693          	li	a3,-106
    806008ae:	f9800713          	li	a4,-104
    806008b2:	04f401a3          	sb	a5,67(s0)
    806008b6:	04040023          	sb	zero,64(s0)
    806008ba:	040400a3          	sb	zero,65(s0)
    806008be:	04040123          	sb	zero,66(s0)
    806008c2:	04040223          	sb	zero,68(s0)
    806008c6:	040402a3          	sb	zero,69(s0)
    806008ca:	04040323          	sb	zero,70(s0)
    806008ce:	040403a3          	sb	zero,71(s0)
    806008d2:	04040423          	sb	zero,72(s0)
    806008d6:	040404a3          	sb	zero,73(s0)
    806008da:	04040523          	sb	zero,74(s0)
    806008de:	04040623          	sb	zero,76(s0)
    806008e2:	040406a3          	sb	zero,77(s0)
    806008e6:	04040723          	sb	zero,78(s0)
    806008ea:	040407a3          	sb	zero,79(s0)
    806008ee:	0a040423          	sb	zero,168(s0)
    806008f2:	00002517          	auipc	a0,0x2
    806008f6:	d0650513          	addi	a0,a0,-762 # 806025f8 <kexec+0x146>
    806008fa:	0ad408a3          	sb	a3,177(s0)
    806008fe:	0ae40923          	sb	a4,178(s0)
    80600902:	0af40823          	sb	a5,176(s0)
    80600906:	0002d497          	auipc	s1,0x2d
    8060090a:	fa948493          	addi	s1,s1,-87 # 8062d8af <__sched_data_end+0xfff>
    8060090e:	77fd                	lui	a5,0xfffff
    80600910:	0a0404a3          	sb	zero,169(s0)
    80600914:	0a040523          	sb	zero,170(s0)
    80600918:	0a0405a3          	sb	zero,171(s0)
    8060091c:	0a040623          	sb	zero,172(s0)
    80600920:	0a0406a3          	sb	zero,173(s0)
    80600924:	0a040723          	sb	zero,174(s0)
    80600928:	0a0407a3          	sb	zero,175(s0)
    8060092c:	0a0409a3          	sb	zero,179(s0)
    80600930:	0a040a23          	sb	zero,180(s0)
    80600934:	0a040aa3          	sb	zero,181(s0)
    80600938:	0a040b23          	sb	zero,182(s0)
    8060093c:	0a040ba3          	sb	zero,183(s0)
    80600940:	000408a3          	sb	zero,17(s0)
    80600944:	00040923          	sb	zero,18(s0)
    80600948:	000409a3          	sb	zero,19(s0)
    8060094c:	00040a23          	sb	zero,20(s0)
    80600950:	00040aa3          	sb	zero,21(s0)
    80600954:	00040b23          	sb	zero,22(s0)
    80600958:	00040ba3          	sb	zero,23(s0)
    8060095c:	01240823          	sb	s2,16(s0)
    80600960:	8cfd                	and	s1,s1,a5
    80600962:	00000097          	auipc	ra,0x0
    80600966:	3f4080e7          	jalr	1012(ra) # 80600d56 <uart_puts>
    8060096a:	45c5                	li	a1,17
    8060096c:	05ee                	slli	a1,a1,0x1b
    8060096e:	8d85                	sub	a1,a1,s1
    80600970:	8526                	mv	a0,s1
    80600972:	00000097          	auipc	ra,0x0
    80600976:	b3a080e7          	jalr	-1222(ra) # 806004ac <pmm_init>
    8060097a:	800005b7          	lui	a1,0x80000
    8060097e:	95a6                	add	a1,a1,s1
    80600980:	01f91513          	slli	a0,s2,0x1f
    80600984:	00000097          	auipc	ra,0x0
    80600988:	c98080e7          	jalr	-872(ra) # 8060061c <pmm_mark_used>
    8060098c:	0084d893          	srli	a7,s1,0x8
    80600990:	0104d813          	srli	a6,s1,0x10
    80600994:	0204d593          	srli	a1,s1,0x20
    80600998:	0284d613          	srli	a2,s1,0x28
    8060099c:	0304d693          	srli	a3,s1,0x30
    806009a0:	f8800713          	li	a4,-120
    806009a4:	4789                	li	a5,2
    806009a6:	0184d51b          	srliw	a0,s1,0x18
    806009aa:	90e1                	srli	s1,s1,0x38
    806009ac:	071400a3          	sb	a7,97(s0)
    806009b0:	07040123          	sb	a6,98(s0)
    806009b4:	06b40223          	sb	a1,100(s0)
    806009b8:	06c402a3          	sb	a2,101(s0)
    806009bc:	06d40323          	sb	a3,102(s0)
    806009c0:	06e405a3          	sb	a4,107(s0)
    806009c4:	00f40823          	sb	a5,16(s0)
    806009c8:	06a401a3          	sb	a0,99(s0)
    806009cc:	06040023          	sb	zero,96(s0)
    806009d0:	00002517          	auipc	a0,0x2
    806009d4:	c4050513          	addi	a0,a0,-960 # 80602610 <kexec+0x15e>
    806009d8:	06040423          	sb	zero,104(s0)
    806009dc:	060404a3          	sb	zero,105(s0)
    806009e0:	06040523          	sb	zero,106(s0)
    806009e4:	06040623          	sb	zero,108(s0)
    806009e8:	060406a3          	sb	zero,109(s0)
    806009ec:	06040723          	sb	zero,110(s0)
    806009f0:	060407a3          	sb	zero,111(s0)
    806009f4:	08040423          	sb	zero,136(s0)
    806009f8:	080404a3          	sb	zero,137(s0)
    806009fc:	08040523          	sb	zero,138(s0)
    80600a00:	080405a3          	sb	zero,139(s0)
    80600a04:	08040623          	sb	zero,140(s0)
    80600a08:	080406a3          	sb	zero,141(s0)
    80600a0c:	08040723          	sb	zero,142(s0)
    80600a10:	080407a3          	sb	zero,143(s0)
    80600a14:	000408a3          	sb	zero,17(s0)
    80600a18:	00040923          	sb	zero,18(s0)
    80600a1c:	000409a3          	sb	zero,19(s0)
    80600a20:	00040a23          	sb	zero,20(s0)
    80600a24:	00040aa3          	sb	zero,21(s0)
    80600a28:	00040b23          	sb	zero,22(s0)
    80600a2c:	00040ba3          	sb	zero,23(s0)
    80600a30:	069403a3          	sb	s1,103(s0)
    80600a34:	00000097          	auipc	ra,0x0
    80600a38:	322080e7          	jalr	802(ra) # 80600d56 <uart_puts>
    80600a3c:	00000097          	auipc	ra,0x0
    80600a40:	984080e7          	jalr	-1660(ra) # 806003c0 <mm_init>
    80600a44:	00002517          	auipc	a0,0x2
    80600a48:	be450513          	addi	a0,a0,-1052 # 80602628 <kexec+0x176>
    80600a4c:	00000097          	auipc	ra,0x0
    80600a50:	30a080e7          	jalr	778(ra) # 80600d56 <uart_puts>
    80600a54:	00000097          	auipc	ra,0x0
    80600a58:	9e2080e7          	jalr	-1566(ra) # 80600436 <mm_map_kernel>
    80600a5c:	00002517          	auipc	a0,0x2
    80600a60:	be450513          	addi	a0,a0,-1052 # 80602640 <kexec+0x18e>
    80600a64:	00000097          	auipc	ra,0x0
    80600a68:	2f2080e7          	jalr	754(ra) # 80600d56 <uart_puts>
    80600a6c:	00000097          	auipc	ra,0x0
    80600a70:	a0e080e7          	jalr	-1522(ra) # 8060047a <mm_enable_paging>
    80600a74:	00002517          	auipc	a0,0x2
    80600a78:	bec50513          	addi	a0,a0,-1044 # 80602660 <kexec+0x1ae>
    80600a7c:	00000097          	auipc	ra,0x0
    80600a80:	2da080e7          	jalr	730(ra) # 80600d56 <uart_puts>
    80600a84:	00003717          	auipc	a4,0x3
    80600a88:	57c70713          	addi	a4,a4,1404 # 80604000 <g_root_pt>
    80600a8c:	00006797          	auipc	a5,0x6
    80600a90:	58c78793          	addi	a5,a5,1420 # 80607018 <g_sched>
    80600a94:	00875613          	srli	a2,a4,0x8
    80600a98:	01075693          	srli	a3,a4,0x10
    80600a9c:	0187529b          	srliw	t0,a4,0x18
    80600aa0:	02075f93          	srli	t6,a4,0x20
    80600aa4:	02875f13          	srli	t5,a4,0x28
    80600aa8:	03075e93          	srli	t4,a4,0x30
    80600aac:	03875e13          	srli	t3,a4,0x38
    80600ab0:	0087d893          	srli	a7,a5,0x8
    80600ab4:	0107d813          	srli	a6,a5,0x10
    80600ab8:	0187d51b          	srliw	a0,a5,0x18
    80600abc:	0207d593          	srli	a1,a5,0x20
    80600ac0:	430d                	li	t1,3
    80600ac2:	08e40823          	sb	a4,144(s0)
    80600ac6:	08c408a3          	sb	a2,145(s0)
    80600aca:	0387d713          	srli	a4,a5,0x38
    80600ace:	0287d613          	srli	a2,a5,0x28
    80600ad2:	08d40923          	sb	a3,146(s0)
    80600ad6:	0307d693          	srli	a3,a5,0x30
    80600ada:	085409a3          	sb	t0,147(s0)
    80600ade:	09f40a23          	sb	t6,148(s0)
    80600ae2:	09e40aa3          	sb	t5,149(s0)
    80600ae6:	09d40b23          	sb	t4,150(s0)
    80600aea:	09c40ba3          	sb	t3,151(s0)
    80600aee:	00640823          	sb	t1,16(s0)
    80600af2:	08f40023          	sb	a5,128(s0)
    80600af6:	091400a3          	sb	a7,129(s0)
    80600afa:	09040123          	sb	a6,130(s0)
    80600afe:	08b40223          	sb	a1,132(s0)
    80600b02:	08c402a3          	sb	a2,133(s0)
    80600b06:	08d40323          	sb	a3,134(s0)
    80600b0a:	08e403a3          	sb	a4,135(s0)
    80600b0e:	08a401a3          	sb	a0,131(s0)
    80600b12:	000408a3          	sb	zero,17(s0)
    80600b16:	00040923          	sb	zero,18(s0)
    80600b1a:	000409a3          	sb	zero,19(s0)
    80600b1e:	00040a23          	sb	zero,20(s0)
    80600b22:	00040aa3          	sb	zero,21(s0)
    80600b26:	00040b23          	sb	zero,22(s0)
    80600b2a:	00040ba3          	sb	zero,23(s0)
    80600b2e:	00000517          	auipc	a0,0x0
    80600b32:	b9650513          	addi	a0,a0,-1130 # 806006c4 <task_a>
    80600b36:	fffff097          	auipc	ra,0xfffff
    80600b3a:	57a080e7          	jalr	1402(ra) # 806000b0 <sched_create>
    80600b3e:	fffff097          	auipc	ra,0xfffff
    80600b42:	5fa080e7          	jalr	1530(ra) # 80600138 <sched_enqueue>
    80600b46:	00000517          	auipc	a0,0x0
    80600b4a:	b9c50513          	addi	a0,a0,-1124 # 806006e2 <task_b>
    80600b4e:	fffff097          	auipc	ra,0xfffff
    80600b52:	562080e7          	jalr	1378(ra) # 806000b0 <sched_create>
    80600b56:	fffff097          	auipc	ra,0xfffff
    80600b5a:	5e2080e7          	jalr	1506(ra) # 80600138 <sched_enqueue>
    80600b5e:	0c944703          	lbu	a4,201(s0)
    80600b62:	0c844683          	lbu	a3,200(s0)
    80600b66:	0ca44783          	lbu	a5,202(s0)
    80600b6a:	0cb44583          	lbu	a1,203(s0)
    80600b6e:	0cc44603          	lbu	a2,204(s0)
    80600b72:	0722                	slli	a4,a4,0x8
    80600b74:	8f55                	or	a4,a4,a3
    80600b76:	07c2                	slli	a5,a5,0x10
    80600b78:	0cd44683          	lbu	a3,205(s0)
    80600b7c:	8fd9                	or	a5,a5,a4
    80600b7e:	05e2                	slli	a1,a1,0x18
    80600b80:	0ce44703          	lbu	a4,206(s0)
    80600b84:	8ddd                	or	a1,a1,a5
    80600b86:	1602                	slli	a2,a2,0x20
    80600b88:	0cf44783          	lbu	a5,207(s0)
    80600b8c:	8e4d                	or	a2,a2,a1
    80600b8e:	16a2                	slli	a3,a3,0x28
    80600b90:	8ed1                	or	a3,a3,a2
    80600b92:	1742                	slli	a4,a4,0x30
    80600b94:	8f55                	or	a4,a4,a3
    80600b96:	17e2                	slli	a5,a5,0x38
    80600b98:	8fd9                	or	a5,a5,a4
    80600b9a:	00002717          	auipc	a4,0x2
    80600b9e:	66670713          	addi	a4,a4,1638 # 80603200 <g_hart_data>
    80600ba2:	97ca                	add	a5,a5,s2
    80600ba4:	00875613          	srli	a2,a4,0x8
    80600ba8:	01075693          	srli	a3,a4,0x10
    80600bac:	0187529b          	srliw	t0,a4,0x18
    80600bb0:	02075f93          	srli	t6,a4,0x20
    80600bb4:	02875f13          	srli	t5,a4,0x28
    80600bb8:	03075e93          	srli	t4,a4,0x30
    80600bbc:	03875e13          	srli	t3,a4,0x38
    80600bc0:	0187d51b          	srliw	a0,a5,0x18
    80600bc4:	4315                	li	t1,5
    80600bc6:	0087d893          	srli	a7,a5,0x8
    80600bca:	0107d813          	srli	a6,a5,0x10
    80600bce:	0207d593          	srli	a1,a5,0x20
    80600bd2:	0ce40023          	sb	a4,192(s0)
    80600bd6:	0cc400a3          	sb	a2,193(s0)
    80600bda:	0387d713          	srli	a4,a5,0x38
    80600bde:	0287d613          	srli	a2,a5,0x28
    80600be2:	0cd40123          	sb	a3,194(s0)
    80600be6:	0307d693          	srli	a3,a5,0x30
    80600bea:	0c5401a3          	sb	t0,195(s0)
    80600bee:	0df40223          	sb	t6,196(s0)
    80600bf2:	0de402a3          	sb	t5,197(s0)
    80600bf6:	0dd40323          	sb	t4,198(s0)
    80600bfa:	0dc403a3          	sb	t3,199(s0)
    80600bfe:	00640823          	sb	t1,16(s0)
    80600c02:	0cf40423          	sb	a5,200(s0)
    80600c06:	0d1404a3          	sb	a7,201(s0)
    80600c0a:	0d040523          	sb	a6,202(s0)
    80600c0e:	0cb40623          	sb	a1,204(s0)
    80600c12:	0cc406a3          	sb	a2,205(s0)
    80600c16:	0cd40723          	sb	a3,206(s0)
    80600c1a:	0ce407a3          	sb	a4,207(s0)
    80600c1e:	0ca405a3          	sb	a0,203(s0)
    80600c22:	000408a3          	sb	zero,17(s0)
    80600c26:	00002517          	auipc	a0,0x2
    80600c2a:	a5250513          	addi	a0,a0,-1454 # 80602678 <kexec+0x1c6>
    80600c2e:	00040923          	sb	zero,18(s0)
    80600c32:	000409a3          	sb	zero,19(s0)
    80600c36:	00040a23          	sb	zero,20(s0)
    80600c3a:	00040aa3          	sb	zero,21(s0)
    80600c3e:	00040b23          	sb	zero,22(s0)
    80600c42:	00040ba3          	sb	zero,23(s0)
    80600c46:	00000097          	auipc	ra,0x0
    80600c4a:	110080e7          	jalr	272(ra) # 80600d56 <uart_puts>
    80600c4e:	00002517          	auipc	a0,0x2
    80600c52:	92250513          	addi	a0,a0,-1758 # 80602570 <kexec+0xbe>
    80600c56:	00000097          	auipc	ra,0x0
    80600c5a:	100080e7          	jalr	256(ra) # 80600d56 <uart_puts>
    80600c5e:	00000097          	auipc	ra,0x0
    80600c62:	aa2080e7          	jalr	-1374(ra) # 80600700 <wake_hart.constprop.0>
    80600c66:	00002517          	auipc	a0,0x2
    80600c6a:	93250513          	addi	a0,a0,-1742 # 80602598 <kexec+0xe6>
    80600c6e:	00000097          	auipc	ra,0x0
    80600c72:	0e8080e7          	jalr	232(ra) # 80600d56 <uart_puts>
    80600c76:	fffff097          	auipc	ra,0xfffff
    80600c7a:	652080e7          	jalr	1618(ra) # 806002c8 <sched_start>

0000000080600c7e <c_trap_handler>:
    80600c7e:	1101                	addi	sp,sp,-32
    80600c80:	e822                	sd	s0,16(sp)
    80600c82:	842a                	mv	s0,a0
    80600c84:	00002517          	auipc	a0,0x2
    80600c88:	a0c50513          	addi	a0,a0,-1524 # 80602690 <kexec+0x1de>
    80600c8c:	ec06                	sd	ra,24(sp)
    80600c8e:	e426                	sd	s1,8(sp)
    80600c90:	e04a                	sd	s2,0(sp)
    80600c92:	84b2                	mv	s1,a2
    80600c94:	892e                	mv	s2,a1
    80600c96:	00000097          	auipc	ra,0x0
    80600c9a:	0c0080e7          	jalr	192(ra) # 80600d56 <uart_puts>
    80600c9e:	8522                	mv	a0,s0
    80600ca0:	00000097          	auipc	ra,0x0
    80600ca4:	0e8080e7          	jalr	232(ra) # 80600d88 <uart_hex>
    80600ca8:	00002517          	auipc	a0,0x2
    80600cac:	9f850513          	addi	a0,a0,-1544 # 806026a0 <kexec+0x1ee>
    80600cb0:	00000097          	auipc	ra,0x0
    80600cb4:	0a6080e7          	jalr	166(ra) # 80600d56 <uart_puts>
    80600cb8:	854a                	mv	a0,s2
    80600cba:	00000097          	auipc	ra,0x0
    80600cbe:	0ce080e7          	jalr	206(ra) # 80600d88 <uart_hex>
    80600cc2:	00002517          	auipc	a0,0x2
    80600cc6:	9e650513          	addi	a0,a0,-1562 # 806026a8 <kexec+0x1f6>
    80600cca:	00000097          	auipc	ra,0x0
    80600cce:	08c080e7          	jalr	140(ra) # 80600d56 <uart_puts>
    80600cd2:	8526                	mv	a0,s1
    80600cd4:	00000097          	auipc	ra,0x0
    80600cd8:	0b4080e7          	jalr	180(ra) # 80600d88 <uart_hex>
    80600cdc:	4529                	li	a0,10
    80600cde:	00000097          	auipc	ra,0x0
    80600ce2:	054080e7          	jalr	84(ra) # 80600d32 <uart_putc>
    80600ce6:	02044d63          	bltz	s0,80600d20 <c_trap_handler+0xa2>
    80600cea:	00002517          	auipc	a0,0x2
    80600cee:	9c650513          	addi	a0,a0,-1594 # 806026b0 <kexec+0x1fe>
    80600cf2:	00000097          	auipc	ra,0x0
    80600cf6:	064080e7          	jalr	100(ra) # 80600d56 <uart_puts>
    80600cfa:	00f47793          	andi	a5,s0,15
    80600cfe:	00002717          	auipc	a4,0x2
    80600d02:	c5a70713          	addi	a4,a4,-934 # 80602958 <exc_names>
    80600d06:	078e                	slli	a5,a5,0x3
    80600d08:	97ba                	add	a5,a5,a4
    80600d0a:	6388                	ld	a0,0(a5)
    80600d0c:	00000097          	auipc	ra,0x0
    80600d10:	04a080e7          	jalr	74(ra) # 80600d56 <uart_puts>
    80600d14:	4529                	li	a0,10
    80600d16:	00000097          	auipc	ra,0x0
    80600d1a:	01c080e7          	jalr	28(ra) # 80600d32 <uart_putc>
    80600d1e:	a001                	j	80600d1e <c_trap_handler+0xa0>
    80600d20:	00002517          	auipc	a0,0x2
    80600d24:	9a850513          	addi	a0,a0,-1624 # 806026c8 <kexec+0x216>
    80600d28:	00000097          	auipc	ra,0x0
    80600d2c:	02e080e7          	jalr	46(ra) # 80600d56 <uart_puts>
    80600d30:	b7fd                	j	80600d1e <c_trap_handler+0xa0>

0000000080600d32 <uart_putc>:
    80600d32:	10000737          	lui	a4,0x10000
    80600d36:	0715                	addi	a4,a4,5 # 10000005 <DRAM_SIZE+0x8000005>
    80600d38:	10000637          	lui	a2,0x10000
    80600d3c:	46a9                	li	a3,10
    80600d3e:	00074783          	lbu	a5,0(a4)
    80600d42:	0207f793          	andi	a5,a5,32
    80600d46:	dfe5                	beqz	a5,80600d3e <uart_putc+0xc>
    80600d48:	00a60023          	sb	a0,0(a2) # 10000000 <DRAM_SIZE+0x8000000>
    80600d4c:	00d51463          	bne	a0,a3,80600d54 <uart_putc+0x22>
    80600d50:	4535                	li	a0,13
    80600d52:	b7f5                	j	80600d3e <uart_putc+0xc>
    80600d54:	8082                	ret

0000000080600d56 <uart_puts>:
    80600d56:	00054683          	lbu	a3,0(a0)
    80600d5a:	c695                	beqz	a3,80600d86 <uart_puts+0x30>
    80600d5c:	10000737          	lui	a4,0x10000
    80600d60:	0715                	addi	a4,a4,5 # 10000005 <DRAM_SIZE+0x8000005>
    80600d62:	100005b7          	lui	a1,0x10000
    80600d66:	4629                	li	a2,10
    80600d68:	00074783          	lbu	a5,0(a4)
    80600d6c:	0207f793          	andi	a5,a5,32
    80600d70:	dfe5                	beqz	a5,80600d68 <uart_puts+0x12>
    80600d72:	00d58023          	sb	a3,0(a1) # 10000000 <DRAM_SIZE+0x8000000>
    80600d76:	00c69463          	bne	a3,a2,80600d7e <uart_puts+0x28>
    80600d7a:	46b5                	li	a3,13
    80600d7c:	b7f5                	j	80600d68 <uart_puts+0x12>
    80600d7e:	00154683          	lbu	a3,1(a0)
    80600d82:	0505                	addi	a0,a0,1
    80600d84:	f2f5                	bnez	a3,80600d68 <uart_puts+0x12>
    80600d86:	8082                	ret

0000000080600d88 <uart_hex>:
    80600d88:	10000737          	lui	a4,0x10000
    80600d8c:	0715                	addi	a4,a4,5 # 10000005 <DRAM_SIZE+0x8000005>
    80600d8e:	100006b7          	lui	a3,0x10000
    80600d92:	00074783          	lbu	a5,0(a4)
    80600d96:	0207f793          	andi	a5,a5,32
    80600d9a:	dfe5                	beqz	a5,80600d92 <uart_hex+0xa>
    80600d9c:	03000793          	li	a5,48
    80600da0:	10000737          	lui	a4,0x10000
    80600da4:	00f68023          	sb	a5,0(a3) # 10000000 <DRAM_SIZE+0x8000000>
    80600da8:	0715                	addi	a4,a4,5 # 10000005 <DRAM_SIZE+0x8000005>
    80600daa:	100006b7          	lui	a3,0x10000
    80600dae:	00074783          	lbu	a5,0(a4)
    80600db2:	0207f793          	andi	a5,a5,32
    80600db6:	dfe5                	beqz	a5,80600dae <uart_hex+0x26>
    80600db8:	07800793          	li	a5,120
    80600dbc:	10000737          	lui	a4,0x10000
    80600dc0:	00f68023          	sb	a5,0(a3) # 10000000 <DRAM_SIZE+0x8000000>
    80600dc4:	0715                	addi	a4,a4,5 # 10000005 <DRAM_SIZE+0x8000005>
    80600dc6:	03c00813          	li	a6,60
    80600dca:	00002317          	auipc	t1,0x2
    80600dce:	c0e30313          	addi	t1,t1,-1010 # 806029d8 <d.0>
    80600dd2:	100005b7          	lui	a1,0x10000
    80600dd6:	4629                	li	a2,10
    80600dd8:	58f1                	li	a7,-4
    80600dda:	010557b3          	srl	a5,a0,a6
    80600dde:	8bbd                	andi	a5,a5,15
    80600de0:	979a                	add	a5,a5,t1
    80600de2:	0007c683          	lbu	a3,0(a5)
    80600de6:	00074783          	lbu	a5,0(a4)
    80600dea:	0207f793          	andi	a5,a5,32
    80600dee:	dfe5                	beqz	a5,80600de6 <uart_hex+0x5e>
    80600df0:	00d58023          	sb	a3,0(a1) # 10000000 <DRAM_SIZE+0x8000000>
    80600df4:	00c69463          	bne	a3,a2,80600dfc <uart_hex+0x74>
    80600df8:	46b5                	li	a3,13
    80600dfa:	b7f5                	j	80600de6 <uart_hex+0x5e>
    80600dfc:	3871                	addiw	a6,a6,-4
    80600dfe:	fd181ee3          	bne	a6,a7,80600dda <uart_hex+0x52>
    80600e02:	8082                	ret

0000000080600e04 <swtch>:
    80600e04:	00153023          	sd	ra,0(a0)
    80600e08:	00253423          	sd	sp,8(a0)
    80600e0c:	e900                	sd	s0,16(a0)
    80600e0e:	ed04                	sd	s1,24(a0)
    80600e10:	03253023          	sd	s2,32(a0)
    80600e14:	03353423          	sd	s3,40(a0)
    80600e18:	03453823          	sd	s4,48(a0)
    80600e1c:	03553c23          	sd	s5,56(a0)
    80600e20:	05653023          	sd	s6,64(a0)
    80600e24:	05753423          	sd	s7,72(a0)
    80600e28:	05853823          	sd	s8,80(a0)
    80600e2c:	05953c23          	sd	s9,88(a0)
    80600e30:	07a53023          	sd	s10,96(a0)
    80600e34:	07b53423          	sd	s11,104(a0)
    80600e38:	0005b083          	ld	ra,0(a1)
    80600e3c:	0085b103          	ld	sp,8(a1)
    80600e40:	6980                	ld	s0,16(a1)
    80600e42:	6d84                	ld	s1,24(a1)
    80600e44:	0205b903          	ld	s2,32(a1)
    80600e48:	0285b983          	ld	s3,40(a1)
    80600e4c:	0305ba03          	ld	s4,48(a1)
    80600e50:	0385ba83          	ld	s5,56(a1)
    80600e54:	0405bb03          	ld	s6,64(a1)
    80600e58:	0485bb83          	ld	s7,72(a1)
    80600e5c:	0505bc03          	ld	s8,80(a1)
    80600e60:	0585bc83          	ld	s9,88(a1)
    80600e64:	0605bd03          	ld	s10,96(a1)
    80600e68:	0685bd83          	ld	s11,104(a1)
    80600e6c:	8082                	ret

0000000080600e6e <printint>:
    80600e6e:	7139                	addi	sp,sp,-64
    80600e70:	03f55793          	srli	a5,a0,0x3f
    80600e74:	fc06                	sd	ra,56(sp)
    80600e76:	f822                	sd	s0,48(sp)
    80600e78:	f426                	sd	s1,40(sp)
    80600e7a:	8e7d                	and	a2,a2,a5
    80600e7c:	4881                	li	a7,0
    80600e7e:	e225                	bnez	a2,80600ede <printint+0x70>
    80600e80:	0024                	addi	s1,sp,8
    80600e82:	8726                	mv	a4,s1
    80600e84:	4681                	li	a3,0
    80600e86:	00002817          	auipc	a6,0x2
    80600e8a:	b6a80813          	addi	a6,a6,-1174 # 806029f0 <digits>
    80600e8e:	02b577b3          	remu	a5,a0,a1
    80600e92:	862a                	mv	a2,a0
    80600e94:	8436                	mv	s0,a3
    80600e96:	0705                	addi	a4,a4,1
    80600e98:	2685                	addiw	a3,a3,1
    80600e9a:	02b55533          	divu	a0,a0,a1
    80600e9e:	97c2                	add	a5,a5,a6
    80600ea0:	0007c783          	lbu	a5,0(a5)
    80600ea4:	fef70fa3          	sb	a5,-1(a4)
    80600ea8:	feb673e3          	bgeu	a2,a1,80600e8e <printint+0x20>
    80600eac:	00088963          	beqz	a7,80600ebe <printint+0x50>
    80600eb0:	00d107b3          	add	a5,sp,a3
    80600eb4:	02d00713          	li	a4,45
    80600eb8:	8436                	mv	s0,a3
    80600eba:	00e78423          	sb	a4,8(a5)
    80600ebe:	9426                	add	s0,s0,s1
    80600ec0:	14fd                	addi	s1,s1,-1
    80600ec2:	00044503          	lbu	a0,0(s0)
    80600ec6:	147d                	addi	s0,s0,-1
    80600ec8:	00001097          	auipc	ra,0x1
    80600ecc:	568080e7          	jalr	1384(ra) # 80602430 <consputc>
    80600ed0:	fe9419e3          	bne	s0,s1,80600ec2 <printint+0x54>
    80600ed4:	70e2                	ld	ra,56(sp)
    80600ed6:	7442                	ld	s0,48(sp)
    80600ed8:	74a2                	ld	s1,40(sp)
    80600eda:	6121                	addi	sp,sp,64
    80600edc:	8082                	ret
    80600ede:	40a00533          	neg	a0,a0
    80600ee2:	4885                	li	a7,1
    80600ee4:	bf71                	j	80600e80 <printint+0x12>

0000000080600ee6 <printk>:
    80600ee6:	0001e317          	auipc	t1,0x1e
    80600eea:	9be32303          	lw	t1,-1602(t1) # 8061e8a4 <panicking>
    80600eee:	7135                	addi	sp,sp,-160
    80600ef0:	e4a6                	sd	s1,72(sp)
    80600ef2:	ec86                	sd	ra,88(sp)
    80600ef4:	f4ae                	sd	a1,104(sp)
    80600ef6:	f8b2                	sd	a2,112(sp)
    80600ef8:	fcb6                	sd	a3,120(sp)
    80600efa:	e13a                	sd	a4,128(sp)
    80600efc:	e53e                	sd	a5,136(sp)
    80600efe:	e942                	sd	a6,144(sp)
    80600f00:	ed46                	sd	a7,152(sp)
    80600f02:	84aa                	mv	s1,a0
    80600f04:	1e030963          	beqz	t1,806010f6 <printk+0x210>
    80600f08:	0004c503          	lbu	a0,0(s1)
    80600f0c:	10bc                	addi	a5,sp,104
    80600f0e:	e43e                	sd	a5,8(sp)
    80600f10:	cd2d                	beqz	a0,80600f8a <printk+0xa4>
    80600f12:	e8a2                	sd	s0,80(sp)
    80600f14:	fc4e                	sd	s3,56(sp)
    80600f16:	f852                	sd	s4,48(sp)
    80600f18:	f456                	sd	s5,40(sp)
    80600f1a:	f05a                	sd	s6,32(sp)
    80600f1c:	e0ca                	sd	s2,64(sp)
    80600f1e:	ec5e                	sd	s7,24(sp)
    80600f20:	02500993          	li	s3,37
    80600f24:	4401                	li	s0,0
    80600f26:	06400a13          	li	s4,100
    80600f2a:	07500a93          	li	s5,117
    80600f2e:	07800b13          	li	s6,120
    80600f32:	0b351263          	bne	a0,s3,80600fd6 <printk+0xf0>
    80600f36:	0014091b          	addiw	s2,s0,1
    80600f3a:	01248733          	add	a4,s1,s2
    80600f3e:	00074b83          	lbu	s7,0(a4)
    80600f42:	040b8f63          	beqz	s7,80600fa0 <printk+0xba>
    80600f46:	00174683          	lbu	a3,1(a4)
    80600f4a:	ead9                	bnez	a3,80600fe0 <printk+0xfa>
    80600f4c:	0f4b8863          	beq	s7,s4,8060103c <printk+0x156>
    80600f50:	115b8263          	beq	s7,s5,80601054 <printk+0x16e>
    80600f54:	136b8d63          	beq	s7,s6,8060108e <printk+0x1a8>
    80600f58:	07000713          	li	a4,112
    80600f5c:	14eb8663          	beq	s7,a4,806010a8 <printk+0x1c2>
    80600f60:	06300713          	li	a4,99
    80600f64:	1ceb8263          	beq	s7,a4,80601128 <printk+0x242>
    80600f68:	07300713          	li	a4,115
    80600f6c:	1ceb8863          	beq	s7,a4,8060113c <printk+0x256>
    80600f70:	02500713          	li	a4,37
    80600f74:	0aeb8d63          	beq	s7,a4,8060102e <printk+0x148>
    80600f78:	0a0b9563          	bnez	s7,80601022 <printk+0x13c>
    80600f7c:	6446                	ld	s0,80(sp)
    80600f7e:	6906                	ld	s2,64(sp)
    80600f80:	79e2                	ld	s3,56(sp)
    80600f82:	7a42                	ld	s4,48(sp)
    80600f84:	7aa2                	ld	s5,40(sp)
    80600f86:	7b02                	ld	s6,32(sp)
    80600f88:	6be2                	ld	s7,24(sp)
    80600f8a:	0001e797          	auipc	a5,0x1e
    80600f8e:	91a7a783          	lw	a5,-1766(a5) # 8061e8a4 <panicking>
    80600f92:	16078b63          	beqz	a5,80601108 <printk+0x222>
    80600f96:	60e6                	ld	ra,88(sp)
    80600f98:	64a6                	ld	s1,72(sp)
    80600f9a:	4501                	li	a0,0
    80600f9c:	610d                	addi	sp,sp,160
    80600f9e:	8082                	ret
    80600fa0:	4681                	li	a3,0
    80600fa2:	4601                	li	a2,0
    80600fa4:	f8868713          	addi	a4,a3,-120
    80600fa8:	00173713          	seqz	a4,a4
    80600fac:	8e79                	and	a2,a2,a4
    80600fae:	d64d                	beqz	a2,80600f58 <printk+0x72>
    80600fb0:	67a2                	ld	a5,8(sp)
    80600fb2:	4601                	li	a2,0
    80600fb4:	45c1                	li	a1,16
    80600fb6:	6388                	ld	a0,0(a5)
    80600fb8:	07a1                	addi	a5,a5,8
    80600fba:	e43e                	sd	a5,8(sp)
    80600fbc:	2409                	addiw	s0,s0,2
    80600fbe:	00000097          	auipc	ra,0x0
    80600fc2:	eb0080e7          	jalr	-336(ra) # 80600e6e <printint>
    80600fc6:	2405                	addiw	s0,s0,1
    80600fc8:	008487b3          	add	a5,s1,s0
    80600fcc:	0007c503          	lbu	a0,0(a5)
    80600fd0:	d555                	beqz	a0,80600f7c <printk+0x96>
    80600fd2:	f73502e3          	beq	a0,s3,80600f36 <printk+0x50>
    80600fd6:	00001097          	auipc	ra,0x1
    80600fda:	45a080e7          	jalr	1114(ra) # 80602430 <consputc>
    80600fde:	b7e5                	j	80600fc6 <printk+0xe0>
    80600fe0:	054b8e63          	beq	s7,s4,8060103c <printk+0x156>
    80600fe4:	f94b8613          	addi	a2,s7,-108
    80600fe8:	f9c68593          	addi	a1,a3,-100
    80600fec:	00163613          	seqz	a2,a2
    80600ff0:	0015b593          	seqz	a1,a1
    80600ff4:	8df1                	and	a1,a1,a2
    80600ff6:	c589                	beqz	a1,80601000 <printk+0x11a>
    80600ff8:	67a2                	ld	a5,8(sp)
    80600ffa:	4605                	li	a2,1
    80600ffc:	45a9                	li	a1,10
    80600ffe:	bf65                	j	80600fb6 <printk+0xd0>
    80601000:	f9468593          	addi	a1,a3,-108
    80601004:	0015b593          	seqz	a1,a1
    80601008:	8df1                	and	a1,a1,a2
    8060100a:	00274703          	lbu	a4,2(a4)
    8060100e:	16058563          	beqz	a1,80601178 <printk+0x292>
    80601012:	05470e63          	beq	a4,s4,8060106e <printk+0x188>
    80601016:	11570663          	beq	a4,s5,80601122 <printk+0x23c>
    8060101a:	07800693          	li	a3,120
    8060101e:	14d70263          	beq	a4,a3,80601162 <printk+0x27c>
    80601022:	02500513          	li	a0,37
    80601026:	00001097          	auipc	ra,0x1
    8060102a:	40a080e7          	jalr	1034(ra) # 80602430 <consputc>
    8060102e:	855e                	mv	a0,s7
    80601030:	00001097          	auipc	ra,0x1
    80601034:	400080e7          	jalr	1024(ra) # 80602430 <consputc>
    80601038:	844a                	mv	s0,s2
    8060103a:	b771                	j	80600fc6 <printk+0xe0>
    8060103c:	67a2                	ld	a5,8(sp)
    8060103e:	4605                	li	a2,1
    80601040:	45a9                	li	a1,10
    80601042:	4388                	lw	a0,0(a5)
    80601044:	07a1                	addi	a5,a5,8
    80601046:	e43e                	sd	a5,8(sp)
    80601048:	844a                	mv	s0,s2
    8060104a:	00000097          	auipc	ra,0x0
    8060104e:	e24080e7          	jalr	-476(ra) # 80600e6e <printint>
    80601052:	bf95                	j	80600fc6 <printk+0xe0>
    80601054:	67a2                	ld	a5,8(sp)
    80601056:	4601                	li	a2,0
    80601058:	45a9                	li	a1,10
    8060105a:	0007e503          	lwu	a0,0(a5)
    8060105e:	07a1                	addi	a5,a5,8
    80601060:	e43e                	sd	a5,8(sp)
    80601062:	844a                	mv	s0,s2
    80601064:	00000097          	auipc	ra,0x0
    80601068:	e0a080e7          	jalr	-502(ra) # 80600e6e <printint>
    8060106c:	bfa9                	j	80600fc6 <printk+0xe0>
    8060106e:	67a2                	ld	a5,8(sp)
    80601070:	4605                	li	a2,1
    80601072:	45a9                	li	a1,10
    80601074:	6388                	ld	a0,0(a5)
    80601076:	07a1                	addi	a5,a5,8
    80601078:	e43e                	sd	a5,8(sp)
    8060107a:	240d                	addiw	s0,s0,3
    8060107c:	00000097          	auipc	ra,0x0
    80601080:	df2080e7          	jalr	-526(ra) # 80600e6e <printint>
    80601084:	b789                	j	80600fc6 <printk+0xe0>
    80601086:	07800713          	li	a4,120
    8060108a:	f0eb9de3          	bne	s7,a4,80600fa4 <printk+0xbe>
    8060108e:	67a2                	ld	a5,8(sp)
    80601090:	4601                	li	a2,0
    80601092:	45c1                	li	a1,16
    80601094:	0007e503          	lwu	a0,0(a5)
    80601098:	07a1                	addi	a5,a5,8
    8060109a:	e43e                	sd	a5,8(sp)
    8060109c:	844a                	mv	s0,s2
    8060109e:	00000097          	auipc	ra,0x0
    806010a2:	dd0080e7          	jalr	-560(ra) # 80600e6e <printint>
    806010a6:	b705                	j	80600fc6 <printk+0xe0>
    806010a8:	67a2                	ld	a5,8(sp)
    806010aa:	03000513          	li	a0,48
    806010ae:	e862                	sd	s8,16(sp)
    806010b0:	00878713          	addi	a4,a5,8
    806010b4:	0007bb83          	ld	s7,0(a5)
    806010b8:	e43a                	sd	a4,8(sp)
    806010ba:	00001097          	auipc	ra,0x1
    806010be:	376080e7          	jalr	886(ra) # 80602430 <consputc>
    806010c2:	07800513          	li	a0,120
    806010c6:	00001097          	auipc	ra,0x1
    806010ca:	36a080e7          	jalr	874(ra) # 80602430 <consputc>
    806010ce:	4441                	li	s0,16
    806010d0:	00002c17          	auipc	s8,0x2
    806010d4:	920c0c13          	addi	s8,s8,-1760 # 806029f0 <digits>
    806010d8:	03cbd793          	srli	a5,s7,0x3c
    806010dc:	97e2                	add	a5,a5,s8
    806010de:	0007c503          	lbu	a0,0(a5)
    806010e2:	347d                	addiw	s0,s0,-1
    806010e4:	0b92                	slli	s7,s7,0x4
    806010e6:	00001097          	auipc	ra,0x1
    806010ea:	34a080e7          	jalr	842(ra) # 80602430 <consputc>
    806010ee:	f46d                	bnez	s0,806010d8 <printk+0x1f2>
    806010f0:	6c42                	ld	s8,16(sp)
    806010f2:	844a                	mv	s0,s2
    806010f4:	bdc9                	j	80600fc6 <printk+0xe0>
    806010f6:	00017517          	auipc	a0,0x17
    806010fa:	96250513          	addi	a0,a0,-1694 # 80617a58 <pr>
    806010fe:	00001097          	auipc	ra,0x1
    80601102:	10a080e7          	jalr	266(ra) # 80602208 <acquire>
    80601106:	b509                	j	80600f08 <printk+0x22>
    80601108:	00017517          	auipc	a0,0x17
    8060110c:	95050513          	addi	a0,a0,-1712 # 80617a58 <pr>
    80601110:	00001097          	auipc	ra,0x1
    80601114:	196080e7          	jalr	406(ra) # 806022a6 <release>
    80601118:	60e6                	ld	ra,88(sp)
    8060111a:	64a6                	ld	s1,72(sp)
    8060111c:	4501                	li	a0,0
    8060111e:	610d                	addi	sp,sp,160
    80601120:	8082                	ret
    80601122:	67a2                	ld	a5,8(sp)
    80601124:	4601                	li	a2,0
    80601126:	b7b1                	j	80601072 <printk+0x18c>
    80601128:	67a2                	ld	a5,8(sp)
    8060112a:	844a                	mv	s0,s2
    8060112c:	4388                	lw	a0,0(a5)
    8060112e:	07a1                	addi	a5,a5,8
    80601130:	e43e                	sd	a5,8(sp)
    80601132:	00001097          	auipc	ra,0x1
    80601136:	2fe080e7          	jalr	766(ra) # 80602430 <consputc>
    8060113a:	b571                	j	80600fc6 <printk+0xe0>
    8060113c:	67a2                	ld	a5,8(sp)
    8060113e:	6380                	ld	s0,0(a5)
    80601140:	07a1                	addi	a5,a5,8
    80601142:	e43e                	sd	a5,8(sp)
    80601144:	c01d                	beqz	s0,8060116a <printk+0x284>
    80601146:	00044503          	lbu	a0,0(s0)
    8060114a:	ee0507e3          	beqz	a0,80601038 <printk+0x152>
    8060114e:	00001097          	auipc	ra,0x1
    80601152:	2e2080e7          	jalr	738(ra) # 80602430 <consputc>
    80601156:	00144503          	lbu	a0,1(s0)
    8060115a:	0405                	addi	s0,s0,1
    8060115c:	f96d                	bnez	a0,8060114e <printk+0x268>
    8060115e:	844a                	mv	s0,s2
    80601160:	b59d                	j	80600fc6 <printk+0xe0>
    80601162:	67a2                	ld	a5,8(sp)
    80601164:	4601                	li	a2,0
    80601166:	45c1                	li	a1,16
    80601168:	b731                	j	80601074 <printk+0x18e>
    8060116a:	02800513          	li	a0,40
    8060116e:	00001417          	auipc	s0,0x1
    80601172:	68a40413          	addi	s0,s0,1674 # 806027f8 <kexec+0x346>
    80601176:	bfe1                	j	8060114e <printk+0x268>
    80601178:	07500713          	li	a4,117
    8060117c:	eceb8ce3          	beq	s7,a4,80601054 <printk+0x16e>
    80601180:	f8b68713          	addi	a4,a3,-117
    80601184:	00173713          	seqz	a4,a4
    80601188:	8f71                	and	a4,a4,a2
    8060118a:	ee070ee3          	beqz	a4,80601086 <printk+0x1a0>
    8060118e:	67a2                	ld	a5,8(sp)
    80601190:	4601                	li	a2,0
    80601192:	45a9                	li	a1,10
    80601194:	b50d                	j	80600fb6 <printk+0xd0>

0000000080601196 <panic>:
    80601196:	1101                	addi	sp,sp,-32
    80601198:	e822                	sd	s0,16(sp)
    8060119a:	e426                	sd	s1,8(sp)
    8060119c:	4405                	li	s0,1
    8060119e:	84aa                	mv	s1,a0
    806011a0:	00001517          	auipc	a0,0x1
    806011a4:	66050513          	addi	a0,a0,1632 # 80602800 <kexec+0x34e>
    806011a8:	ec06                	sd	ra,24(sp)
    806011aa:	0001d797          	auipc	a5,0x1d
    806011ae:	6e87ad23          	sw	s0,1786(a5) # 8061e8a4 <panicking>
    806011b2:	00000097          	auipc	ra,0x0
    806011b6:	d34080e7          	jalr	-716(ra) # 80600ee6 <printk>
    806011ba:	85a6                	mv	a1,s1
    806011bc:	00001517          	auipc	a0,0x1
    806011c0:	64c50513          	addi	a0,a0,1612 # 80602808 <kexec+0x356>
    806011c4:	00000097          	auipc	ra,0x0
    806011c8:	d22080e7          	jalr	-734(ra) # 80600ee6 <printk>
    806011cc:	0001d797          	auipc	a5,0x1d
    806011d0:	6c87aa23          	sw	s0,1748(a5) # 8061e8a0 <panicked>
    806011d4:	a001                	j	806011d4 <panic+0x3e>

00000000806011d6 <printkinit>:
    806011d6:	00001597          	auipc	a1,0x1
    806011da:	63a58593          	addi	a1,a1,1594 # 80602810 <kexec+0x35e>
    806011de:	00017517          	auipc	a0,0x17
    806011e2:	87a50513          	addi	a0,a0,-1926 # 80617a58 <pr>
    806011e6:	00001317          	auipc	t1,0x1
    806011ea:	fae30067          	jr	-82(t1) # 80602194 <initlock>

00000000806011ee <freeproc>:
    806011ee:	7179                	addi	sp,sp,-48
    806011f0:	f022                	sd	s0,32(sp)
    806011f2:	842a                	mv	s0,a0
    806011f4:	6d28                	ld	a0,88(a0)
    806011f6:	f406                	sd	ra,40(sp)
    806011f8:	c509                	beqz	a0,80601202 <freeproc+0x14>
    806011fa:	00001097          	auipc	ra,0x1
    806011fe:	22a080e7          	jalr	554(ra) # 80602424 <kfree>
    80601202:	6828                	ld	a0,80(s0)
    80601204:	04043c23          	sd	zero,88(s0)
    80601208:	c129                	beqz	a0,8060124a <freeproc+0x5c>
    8060120a:	643c                	ld	a5,72(s0)
    8060120c:	040005b7          	lui	a1,0x4000
    80601210:	15fd                	addi	a1,a1,-1 # 3ffffff <MEGAPAGE_SIZE+0x3dfffff>
    80601212:	05b2                	slli	a1,a1,0xc
    80601214:	4681                	li	a3,0
    80601216:	4605                	li	a2,1
    80601218:	ec26                	sd	s1,24(sp)
    8060121a:	e42a                	sd	a0,8(sp)
    8060121c:	84be                	mv	s1,a5
    8060121e:	00001097          	auipc	ra,0x1
    80601222:	240080e7          	jalr	576(ra) # 8060245e <uvmunmap>
    80601226:	6522                	ld	a0,8(sp)
    80601228:	020005b7          	lui	a1,0x2000
    8060122c:	15fd                	addi	a1,a1,-1 # 1ffffff <MEGAPAGE_SIZE+0x1dfffff>
    8060122e:	05b6                	slli	a1,a1,0xd
    80601230:	4681                	li	a3,0
    80601232:	4605                	li	a2,1
    80601234:	00001097          	auipc	ra,0x1
    80601238:	22a080e7          	jalr	554(ra) # 8060245e <uvmunmap>
    8060123c:	6522                	ld	a0,8(sp)
    8060123e:	85a6                	mv	a1,s1
    80601240:	00001097          	auipc	ra,0x1
    80601244:	20e080e7          	jalr	526(ra) # 8060244e <uvmfree>
    80601248:	64e2                	ld	s1,24(sp)
    8060124a:	14040c23          	sb	zero,344(s0)
    8060124e:	70a2                	ld	ra,40(sp)
    80601250:	04043823          	sd	zero,80(s0)
    80601254:	04043423          	sd	zero,72(s0)
    80601258:	02042823          	sw	zero,48(s0)
    8060125c:	02043023          	sd	zero,32(s0)
    80601260:	02043423          	sd	zero,40(s0)
    80601264:	00042c23          	sw	zero,24(s0)
    80601268:	7402                	ld	s0,32(sp)
    8060126a:	6145                	addi	sp,sp,48
    8060126c:	8082                	ret

000000008060126e <forkret>:
    8060126e:	1101                	addi	sp,sp,-32
    80601270:	ec06                	sd	ra,24(sp)
    80601272:	e822                	sd	s0,16(sp)
    80601274:	00001097          	auipc	ra,0x1
    80601278:	f54080e7          	jalr	-172(ra) # 806021c8 <push_off>
    8060127c:	8792                	mv	a5,tp
    8060127e:	2781                	sext.w	a5,a5
    80601280:	00016717          	auipc	a4,0x16
    80601284:	7f070713          	addi	a4,a4,2032 # 80617a70 <cpus>
    80601288:	079e                	slli	a5,a5,0x7
    8060128a:	97ba                	add	a5,a5,a4
    8060128c:	6380                	ld	s0,0(a5)
    8060128e:	00001097          	auipc	ra,0x1
    80601292:	fca080e7          	jalr	-54(ra) # 80602258 <pop_off>
    80601296:	8522                	mv	a0,s0
    80601298:	00001097          	auipc	ra,0x1
    8060129c:	00e080e7          	jalr	14(ra) # 806022a6 <release>
    806012a0:	00001797          	auipc	a5,0x1
    806012a4:	7987a783          	lw	a5,1944(a5) # 80602a38 <first.1>
    806012a8:	cb95                	beqz	a5,806012dc <forkret+0x6e>
    806012aa:	4505                	li	a0,1
    806012ac:	00001797          	auipc	a5,0x1
    806012b0:	7807a623          	sw	zero,1932(a5) # 80602a38 <first.1>
    806012b4:	00001097          	auipc	ra,0x1
    806012b8:	1f0080e7          	jalr	496(ra) # 806024a4 <fsinit>
    806012bc:	00001517          	auipc	a0,0x1
    806012c0:	55c50513          	addi	a0,a0,1372 # 80602818 <kexec+0x366>
    806012c4:	858a                	mv	a1,sp
    806012c6:	e402                	sd	zero,8(sp)
    806012c8:	e02a                	sd	a0,0(sp)
    806012ca:	00001097          	auipc	ra,0x1
    806012ce:	1e8080e7          	jalr	488(ra) # 806024b2 <kexec>
    806012d2:	6c38                	ld	a4,88(s0)
    806012d4:	57fd                	li	a5,-1
    806012d6:	fb28                	sd	a0,112(a4)
    806012d8:	02f50d63          	beq	a0,a5,80601312 <forkret+0xa4>
    806012dc:	00001097          	auipc	ra,0x1
    806012e0:	168080e7          	jalr	360(ra) # 80602444 <prepare_return>
    806012e4:	6828                	ld	a0,80(s0)
    806012e6:	56fd                	li	a3,-1
    806012e8:	6442                	ld	s0,16(sp)
    806012ea:	04000737          	lui	a4,0x4000
    806012ee:	60e2                	ld	ra,24(sp)
    806012f0:	9736                	add	a4,a4,a3
    806012f2:	00001797          	auipc	a5,0x1
    806012f6:	15078793          	addi	a5,a5,336 # 80602442 <userret>
    806012fa:	0001c617          	auipc	a2,0x1c
    806012fe:	5a660613          	addi	a2,a2,1446 # 8061d8a0 <trampoline>
    80601302:	8131                	srli	a0,a0,0xc
    80601304:	16fe                	slli	a3,a3,0x3f
    80601306:	0732                	slli	a4,a4,0xc
    80601308:	8f91                	sub	a5,a5,a2
    8060130a:	8d55                	or	a0,a0,a3
    8060130c:	97ba                	add	a5,a5,a4
    8060130e:	6105                	addi	sp,sp,32
    80601310:	8782                	jr	a5
    80601312:	00001517          	auipc	a0,0x1
    80601316:	50e50513          	addi	a0,a0,1294 # 80602820 <kexec+0x36e>
    8060131a:	00000097          	auipc	ra,0x0
    8060131e:	e7c080e7          	jalr	-388(ra) # 80601196 <panic>

0000000080601322 <proc_mapstacks>:
    80601322:	7179                	addi	sp,sp,-48
    80601324:	000a57b7          	lui	a5,0xa5
    80601328:	ec26                	sd	s1,24(sp)
    8060132a:	fa578793          	addi	a5,a5,-91 # a4fa5 <PMM_MAX_PAGES+0x9cfa5>
    8060132e:	4fa504b7          	lui	s1,0x4fa50
    80601332:	07b2                	slli	a5,a5,0xc
    80601334:	a4f48493          	addi	s1,s1,-1457 # 4fa4fa4f <DRAM_SIZE+0x47a4fa4f>
    80601338:	e84a                	sd	s2,16(sp)
    8060133a:	e44e                	sd	s3,8(sp)
    8060133c:	fa578793          	addi	a5,a5,-91
    80601340:	1482                	slli	s1,s1,0x20
    80601342:	49c5                	li	s3,17
    80601344:	6919                	lui	s2,0x6
    80601346:	f022                	sd	s0,32(sp)
    80601348:	e052                	sd	s4,0(sp)
    8060134a:	f406                	sd	ra,40(sp)
    8060134c:	94be                	add	s1,s1,a5
    8060134e:	8a2a                	mv	s4,a0
    80601350:	09ee                	slli	s3,s3,0x1b
    80601352:	a0090913          	addi	s2,s2,-1536 # 5a00 <PAGE_SIZE+0x4a00>
    80601356:	4401                	li	s0,0
    80601358:	00001097          	auipc	ra,0x1
    8060135c:	0c4080e7          	jalr	196(ra) # 8060241c <kalloc>
    80601360:	862a                	mv	a2,a0
    80601362:	cd0d                	beqz	a0,8060139c <proc_mapstacks+0x7a>
    80601364:	40345593          	srai	a1,s0,0x3
    80601368:	029585b3          	mul	a1,a1,s1
    8060136c:	6789                	lui	a5,0x2
    8060136e:	4719                	li	a4,6
    80601370:	6685                	lui	a3,0x1
    80601372:	8552                	mv	a0,s4
    80601374:	16840413          	addi	s0,s0,360
    80601378:	05b6                	slli	a1,a1,0xd
    8060137a:	9dbd                	addw	a1,a1,a5
    8060137c:	40b985b3          	sub	a1,s3,a1
    80601380:	00001097          	auipc	ra,0x1
    80601384:	0bc080e7          	jalr	188(ra) # 8060243c <kvmmap>
    80601388:	fd2418e3          	bne	s0,s2,80601358 <proc_mapstacks+0x36>
    8060138c:	70a2                	ld	ra,40(sp)
    8060138e:	7402                	ld	s0,32(sp)
    80601390:	64e2                	ld	s1,24(sp)
    80601392:	6942                	ld	s2,16(sp)
    80601394:	69a2                	ld	s3,8(sp)
    80601396:	6a02                	ld	s4,0(sp)
    80601398:	6145                	addi	sp,sp,48
    8060139a:	8082                	ret
    8060139c:	00001517          	auipc	a0,0x1
    806013a0:	48c50513          	addi	a0,a0,1164 # 80602828 <kexec+0x376>
    806013a4:	00000097          	auipc	ra,0x0
    806013a8:	df2080e7          	jalr	-526(ra) # 80601196 <panic>

00000000806013ac <procinit>:
    806013ac:	7179                	addi	sp,sp,-48
    806013ae:	00001597          	auipc	a1,0x1
    806013b2:	48258593          	addi	a1,a1,1154 # 80602830 <kexec+0x37e>
    806013b6:	00017517          	auipc	a0,0x17
    806013ba:	aba50513          	addi	a0,a0,-1350 # 80617e70 <pid_lock>
    806013be:	f406                	sd	ra,40(sp)
    806013c0:	f022                	sd	s0,32(sp)
    806013c2:	ec26                	sd	s1,24(sp)
    806013c4:	e84a                	sd	s2,16(sp)
    806013c6:	e44e                	sd	s3,8(sp)
    806013c8:	e052                	sd	s4,0(sp)
    806013ca:	00001097          	auipc	ra,0x1
    806013ce:	dca080e7          	jalr	-566(ra) # 80602194 <initlock>
    806013d2:	00001597          	auipc	a1,0x1
    806013d6:	46658593          	addi	a1,a1,1126 # 80602838 <kexec+0x386>
    806013da:	00017517          	auipc	a0,0x17
    806013de:	aae50513          	addi	a0,a0,-1362 # 80617e88 <wait_lock>
    806013e2:	00001097          	auipc	ra,0x1
    806013e6:	db2080e7          	jalr	-590(ra) # 80602194 <initlock>
    806013ea:	000a57b7          	lui	a5,0xa5
    806013ee:	fa578793          	addi	a5,a5,-91 # a4fa5 <PMM_MAX_PAGES+0x9cfa5>
    806013f2:	4fa50937          	lui	s2,0x4fa50
    806013f6:	07b2                	slli	a5,a5,0xc
    806013f8:	a4f90913          	addi	s2,s2,-1457 # 4fa4fa4f <DRAM_SIZE+0x47a4fa4f>
    806013fc:	fa578793          	addi	a5,a5,-91
    80601400:	1902                	slli	s2,s2,0x20
    80601402:	49c5                	li	s3,17
    80601404:	993e                	add	s2,s2,a5
    80601406:	09ee                	slli	s3,s3,0x1b
    80601408:	4481                	li	s1,0
    8060140a:	00017417          	auipc	s0,0x17
    8060140e:	a9640413          	addi	s0,s0,-1386 # 80617ea0 <proc>
    80601412:	0001ca17          	auipc	s4,0x1c
    80601416:	48ea0a13          	addi	s4,s4,1166 # 8061d8a0 <trampoline>
    8060141a:	8522                	mv	a0,s0
    8060141c:	00001597          	auipc	a1,0x1
    80601420:	42c58593          	addi	a1,a1,1068 # 80602848 <kexec+0x396>
    80601424:	00001097          	auipc	ra,0x1
    80601428:	d70080e7          	jalr	-656(ra) # 80602194 <initlock>
    8060142c:	4034d793          	srai	a5,s1,0x3
    80601430:	032787b3          	mul	a5,a5,s2
    80601434:	6709                	lui	a4,0x2
    80601436:	00042c23          	sw	zero,24(s0)
    8060143a:	16840413          	addi	s0,s0,360
    8060143e:	16848493          	addi	s1,s1,360
    80601442:	07b6                	slli	a5,a5,0xd
    80601444:	9fb9                	addw	a5,a5,a4
    80601446:	40f987b3          	sub	a5,s3,a5
    8060144a:	ecf43c23          	sd	a5,-296(s0)
    8060144e:	fd4416e3          	bne	s0,s4,8060141a <procinit+0x6e>
    80601452:	70a2                	ld	ra,40(sp)
    80601454:	7402                	ld	s0,32(sp)
    80601456:	64e2                	ld	s1,24(sp)
    80601458:	6942                	ld	s2,16(sp)
    8060145a:	69a2                	ld	s3,8(sp)
    8060145c:	6a02                	ld	s4,0(sp)
    8060145e:	6145                	addi	sp,sp,48
    80601460:	8082                	ret

0000000080601462 <cpuid>:
    80601462:	8512                	mv	a0,tp
    80601464:	2501                	sext.w	a0,a0
    80601466:	8082                	ret

0000000080601468 <mycpu>:
    80601468:	8792                	mv	a5,tp
    8060146a:	2781                	sext.w	a5,a5
    8060146c:	079e                	slli	a5,a5,0x7
    8060146e:	00016517          	auipc	a0,0x16
    80601472:	60250513          	addi	a0,a0,1538 # 80617a70 <cpus>
    80601476:	953e                	add	a0,a0,a5
    80601478:	8082                	ret

000000008060147a <myproc>:
    8060147a:	1101                	addi	sp,sp,-32
    8060147c:	ec06                	sd	ra,24(sp)
    8060147e:	00001097          	auipc	ra,0x1
    80601482:	d4a080e7          	jalr	-694(ra) # 806021c8 <push_off>
    80601486:	8792                	mv	a5,tp
    80601488:	2781                	sext.w	a5,a5
    8060148a:	00016717          	auipc	a4,0x16
    8060148e:	5e670713          	addi	a4,a4,1510 # 80617a70 <cpus>
    80601492:	079e                	slli	a5,a5,0x7
    80601494:	97ba                	add	a5,a5,a4
    80601496:	6388                	ld	a0,0(a5)
    80601498:	e42a                	sd	a0,8(sp)
    8060149a:	00001097          	auipc	ra,0x1
    8060149e:	dbe080e7          	jalr	-578(ra) # 80602258 <pop_off>
    806014a2:	60e2                	ld	ra,24(sp)
    806014a4:	6522                	ld	a0,8(sp)
    806014a6:	6105                	addi	sp,sp,32
    806014a8:	8082                	ret

00000000806014aa <allocpid>:
    806014aa:	1141                	addi	sp,sp,-16
    806014ac:	00017517          	auipc	a0,0x17
    806014b0:	9c450513          	addi	a0,a0,-1596 # 80617e70 <pid_lock>
    806014b4:	e406                	sd	ra,8(sp)
    806014b6:	e022                	sd	s0,0(sp)
    806014b8:	00001097          	auipc	ra,0x1
    806014bc:	d50080e7          	jalr	-688(ra) # 80602208 <acquire>
    806014c0:	00001417          	auipc	s0,0x1
    806014c4:	57c42403          	lw	s0,1404(s0) # 80602a3c <nextpid>
    806014c8:	00017517          	auipc	a0,0x17
    806014cc:	9a850513          	addi	a0,a0,-1624 # 80617e70 <pid_lock>
    806014d0:	0014079b          	addiw	a5,s0,1
    806014d4:	00001717          	auipc	a4,0x1
    806014d8:	56f72423          	sw	a5,1384(a4) # 80602a3c <nextpid>
    806014dc:	00001097          	auipc	ra,0x1
    806014e0:	dca080e7          	jalr	-566(ra) # 806022a6 <release>
    806014e4:	60a2                	ld	ra,8(sp)
    806014e6:	8522                	mv	a0,s0
    806014e8:	6402                	ld	s0,0(sp)
    806014ea:	0141                	addi	sp,sp,16
    806014ec:	8082                	ret

00000000806014ee <proc_pagetable>:
    806014ee:	1101                	addi	sp,sp,-32
    806014f0:	e822                	sd	s0,16(sp)
    806014f2:	e426                	sd	s1,8(sp)
    806014f4:	ec06                	sd	ra,24(sp)
    806014f6:	84aa                	mv	s1,a0
    806014f8:	00001097          	auipc	ra,0x1
    806014fc:	f4e080e7          	jalr	-178(ra) # 80602446 <uvmcreate>
    80601500:	842a                	mv	s0,a0
    80601502:	cd1d                	beqz	a0,80601540 <proc_pagetable+0x52>
    80601504:	040005b7          	lui	a1,0x4000
    80601508:	15fd                	addi	a1,a1,-1 # 3ffffff <MEGAPAGE_SIZE+0x3dfffff>
    8060150a:	05b2                	slli	a1,a1,0xc
    8060150c:	4729                	li	a4,10
    8060150e:	0001c697          	auipc	a3,0x1c
    80601512:	39268693          	addi	a3,a3,914 # 8061d8a0 <trampoline>
    80601516:	6605                	lui	a2,0x1
    80601518:	00001097          	auipc	ra,0x1
    8060151c:	f26080e7          	jalr	-218(ra) # 8060243e <mappages>
    80601520:	02054663          	bltz	a0,8060154c <proc_pagetable+0x5e>
    80601524:	020005b7          	lui	a1,0x2000
    80601528:	6cb4                	ld	a3,88(s1)
    8060152a:	15fd                	addi	a1,a1,-1 # 1ffffff <MEGAPAGE_SIZE+0x1dfffff>
    8060152c:	05b6                	slli	a1,a1,0xd
    8060152e:	4719                	li	a4,6
    80601530:	6605                	lui	a2,0x1
    80601532:	8522                	mv	a0,s0
    80601534:	00001097          	auipc	ra,0x1
    80601538:	f0a080e7          	jalr	-246(ra) # 8060243e <mappages>
    8060153c:	02054563          	bltz	a0,80601566 <proc_pagetable+0x78>
    80601540:	60e2                	ld	ra,24(sp)
    80601542:	8522                	mv	a0,s0
    80601544:	6442                	ld	s0,16(sp)
    80601546:	64a2                	ld	s1,8(sp)
    80601548:	6105                	addi	sp,sp,32
    8060154a:	8082                	ret
    8060154c:	8522                	mv	a0,s0
    8060154e:	4581                	li	a1,0
    80601550:	00001097          	auipc	ra,0x1
    80601554:	efe080e7          	jalr	-258(ra) # 8060244e <uvmfree>
    80601558:	4401                	li	s0,0
    8060155a:	60e2                	ld	ra,24(sp)
    8060155c:	8522                	mv	a0,s0
    8060155e:	6442                	ld	s0,16(sp)
    80601560:	64a2                	ld	s1,8(sp)
    80601562:	6105                	addi	sp,sp,32
    80601564:	8082                	ret
    80601566:	040005b7          	lui	a1,0x4000
    8060156a:	15fd                	addi	a1,a1,-1 # 3ffffff <MEGAPAGE_SIZE+0x3dfffff>
    8060156c:	8522                	mv	a0,s0
    8060156e:	05b2                	slli	a1,a1,0xc
    80601570:	4681                	li	a3,0
    80601572:	4605                	li	a2,1
    80601574:	00001097          	auipc	ra,0x1
    80601578:	eea080e7          	jalr	-278(ra) # 8060245e <uvmunmap>
    8060157c:	8522                	mv	a0,s0
    8060157e:	4581                	li	a1,0
    80601580:	00001097          	auipc	ra,0x1
    80601584:	ece080e7          	jalr	-306(ra) # 8060244e <uvmfree>
    80601588:	4401                	li	s0,0
    8060158a:	bfc1                	j	8060155a <proc_pagetable+0x6c>

000000008060158c <allocproc>:
    8060158c:	1101                	addi	sp,sp,-32
    8060158e:	e822                	sd	s0,16(sp)
    80601590:	e426                	sd	s1,8(sp)
    80601592:	ec06                	sd	ra,24(sp)
    80601594:	00017417          	auipc	s0,0x17
    80601598:	90c40413          	addi	s0,s0,-1780 # 80617ea0 <proc>
    8060159c:	0001c497          	auipc	s1,0x1c
    806015a0:	30448493          	addi	s1,s1,772 # 8061d8a0 <trampoline>
    806015a4:	a809                	j	806015b6 <allocproc+0x2a>
    806015a6:	16840413          	addi	s0,s0,360
    806015aa:	00001097          	auipc	ra,0x1
    806015ae:	cfc080e7          	jalr	-772(ra) # 806022a6 <release>
    806015b2:	0a940663          	beq	s0,s1,8060165e <allocproc+0xd2>
    806015b6:	8522                	mv	a0,s0
    806015b8:	00001097          	auipc	ra,0x1
    806015bc:	c50080e7          	jalr	-944(ra) # 80602208 <acquire>
    806015c0:	4c1c                	lw	a5,24(s0)
    806015c2:	8522                	mv	a0,s0
    806015c4:	f3ed                	bnez	a5,806015a6 <allocproc+0x1a>
    806015c6:	00017517          	auipc	a0,0x17
    806015ca:	8aa50513          	addi	a0,a0,-1878 # 80617e70 <pid_lock>
    806015ce:	00001097          	auipc	ra,0x1
    806015d2:	c3a080e7          	jalr	-966(ra) # 80602208 <acquire>
    806015d6:	00001497          	auipc	s1,0x1
    806015da:	4664a483          	lw	s1,1126(s1) # 80602a3c <nextpid>
    806015de:	00017517          	auipc	a0,0x17
    806015e2:	89250513          	addi	a0,a0,-1902 # 80617e70 <pid_lock>
    806015e6:	0014879b          	addiw	a5,s1,1
    806015ea:	00001717          	auipc	a4,0x1
    806015ee:	44f72923          	sw	a5,1106(a4) # 80602a3c <nextpid>
    806015f2:	00001097          	auipc	ra,0x1
    806015f6:	cb4080e7          	jalr	-844(ra) # 806022a6 <release>
    806015fa:	4785                	li	a5,1
    806015fc:	d804                	sw	s1,48(s0)
    806015fe:	cc1c                	sw	a5,24(s0)
    80601600:	00001097          	auipc	ra,0x1
    80601604:	e1c080e7          	jalr	-484(ra) # 8060241c <kalloc>
    80601608:	ec28                	sd	a0,88(s0)
    8060160a:	c121                	beqz	a0,8060164a <allocproc+0xbe>
    8060160c:	8522                	mv	a0,s0
    8060160e:	00000097          	auipc	ra,0x0
    80601612:	ee0080e7          	jalr	-288(ra) # 806014ee <proc_pagetable>
    80601616:	e828                	sd	a0,80(s0)
    80601618:	c90d                	beqz	a0,8060164a <allocproc+0xbe>
    8060161a:	06040513          	addi	a0,s0,96
    8060161e:	07000613          	li	a2,112
    80601622:	4581                	li	a1,0
    80601624:	00001097          	auipc	ra,0x1
    80601628:	cc8080e7          	jalr	-824(ra) # 806022ec <memset>
    8060162c:	603c                	ld	a5,64(s0)
    8060162e:	6685                	lui	a3,0x1
    80601630:	00000717          	auipc	a4,0x0
    80601634:	c3e70713          	addi	a4,a4,-962 # 8060126e <forkret>
    80601638:	97b6                	add	a5,a5,a3
    8060163a:	60e2                	ld	ra,24(sp)
    8060163c:	f038                	sd	a4,96(s0)
    8060163e:	f43c                	sd	a5,104(s0)
    80601640:	8522                	mv	a0,s0
    80601642:	6442                	ld	s0,16(sp)
    80601644:	64a2                	ld	s1,8(sp)
    80601646:	6105                	addi	sp,sp,32
    80601648:	8082                	ret
    8060164a:	8522                	mv	a0,s0
    8060164c:	00000097          	auipc	ra,0x0
    80601650:	ba2080e7          	jalr	-1118(ra) # 806011ee <freeproc>
    80601654:	8522                	mv	a0,s0
    80601656:	00001097          	auipc	ra,0x1
    8060165a:	c50080e7          	jalr	-944(ra) # 806022a6 <release>
    8060165e:	4401                	li	s0,0
    80601660:	60e2                	ld	ra,24(sp)
    80601662:	8522                	mv	a0,s0
    80601664:	6442                	ld	s0,16(sp)
    80601666:	64a2                	ld	s1,8(sp)
    80601668:	6105                	addi	sp,sp,32
    8060166a:	8082                	ret

000000008060166c <proc_freepagetable>:
    8060166c:	040007b7          	lui	a5,0x4000
    80601670:	1101                	addi	sp,sp,-32
    80601672:	17fd                	addi	a5,a5,-1 # 3ffffff <MEGAPAGE_SIZE+0x3dfffff>
    80601674:	e426                	sd	s1,8(sp)
    80601676:	4681                	li	a3,0
    80601678:	84ae                	mv	s1,a1
    8060167a:	4605                	li	a2,1
    8060167c:	00c79593          	slli	a1,a5,0xc
    80601680:	ec06                	sd	ra,24(sp)
    80601682:	e822                	sd	s0,16(sp)
    80601684:	842a                	mv	s0,a0
    80601686:	00001097          	auipc	ra,0x1
    8060168a:	dd8080e7          	jalr	-552(ra) # 8060245e <uvmunmap>
    8060168e:	020005b7          	lui	a1,0x2000
    80601692:	15fd                	addi	a1,a1,-1 # 1ffffff <MEGAPAGE_SIZE+0x1dfffff>
    80601694:	8522                	mv	a0,s0
    80601696:	05b6                	slli	a1,a1,0xd
    80601698:	4681                	li	a3,0
    8060169a:	4605                	li	a2,1
    8060169c:	00001097          	auipc	ra,0x1
    806016a0:	dc2080e7          	jalr	-574(ra) # 8060245e <uvmunmap>
    806016a4:	8522                	mv	a0,s0
    806016a6:	6442                	ld	s0,16(sp)
    806016a8:	60e2                	ld	ra,24(sp)
    806016aa:	85a6                	mv	a1,s1
    806016ac:	64a2                	ld	s1,8(sp)
    806016ae:	6105                	addi	sp,sp,32
    806016b0:	00001317          	auipc	t1,0x1
    806016b4:	d9e30067          	jr	-610(t1) # 8060244e <uvmfree>

00000000806016b8 <userinit>:
    806016b8:	1141                	addi	sp,sp,-16
    806016ba:	e406                	sd	ra,8(sp)
    806016bc:	e022                	sd	s0,0(sp)
    806016be:	00000097          	auipc	ra,0x0
    806016c2:	ece080e7          	jalr	-306(ra) # 8060158c <allocproc>
    806016c6:	842a                	mv	s0,a0
    806016c8:	00001517          	auipc	a0,0x1
    806016cc:	18850513          	addi	a0,a0,392 # 80602850 <kexec+0x39e>
    806016d0:	0001d797          	auipc	a5,0x1d
    806016d4:	1c87bc23          	sd	s0,472(a5) # 8061e8a8 <initproc>
    806016d8:	00001097          	auipc	ra,0x1
    806016dc:	dce080e7          	jalr	-562(ra) # 806024a6 <namei>
    806016e0:	478d                	li	a5,3
    806016e2:	14a43823          	sd	a0,336(s0)
    806016e6:	cc1c                	sw	a5,24(s0)
    806016e8:	8522                	mv	a0,s0
    806016ea:	6402                	ld	s0,0(sp)
    806016ec:	60a2                	ld	ra,8(sp)
    806016ee:	0141                	addi	sp,sp,16
    806016f0:	00001317          	auipc	t1,0x1
    806016f4:	bb630067          	jr	-1098(t1) # 806022a6 <release>

00000000806016f8 <growproc>:
    806016f8:	1101                	addi	sp,sp,-32
    806016fa:	e822                	sd	s0,16(sp)
    806016fc:	ec06                	sd	ra,24(sp)
    806016fe:	e426                	sd	s1,8(sp)
    80601700:	842a                	mv	s0,a0
    80601702:	00001097          	auipc	ra,0x1
    80601706:	ac6080e7          	jalr	-1338(ra) # 806021c8 <push_off>
    8060170a:	8792                	mv	a5,tp
    8060170c:	2781                	sext.w	a5,a5
    8060170e:	00016717          	auipc	a4,0x16
    80601712:	36270713          	addi	a4,a4,866 # 80617a70 <cpus>
    80601716:	079e                	slli	a5,a5,0x7
    80601718:	97ba                	add	a5,a5,a4
    8060171a:	6384                	ld	s1,0(a5)
    8060171c:	00001097          	auipc	ra,0x1
    80601720:	b3c080e7          	jalr	-1220(ra) # 80602258 <pop_off>
    80601724:	64ac                	ld	a1,72(s1)
    80601726:	02805963          	blez	s0,80601758 <growproc+0x60>
    8060172a:	020007b7          	lui	a5,0x2000
    8060172e:	17fd                	addi	a5,a5,-1 # 1ffffff <MEGAPAGE_SIZE+0x1dfffff>
    80601730:	00b40633          	add	a2,s0,a1
    80601734:	07b6                	slli	a5,a5,0xd
    80601736:	02c7eb63          	bltu	a5,a2,8060176c <growproc+0x74>
    8060173a:	68a8                	ld	a0,80(s1)
    8060173c:	4691                	li	a3,4
    8060173e:	00001097          	auipc	ra,0x1
    80601742:	d22080e7          	jalr	-734(ra) # 80602460 <uvmalloc>
    80601746:	85aa                	mv	a1,a0
    80601748:	c115                	beqz	a0,8060176c <growproc+0x74>
    8060174a:	e4ac                	sd	a1,72(s1)
    8060174c:	4501                	li	a0,0
    8060174e:	60e2                	ld	ra,24(sp)
    80601750:	6442                	ld	s0,16(sp)
    80601752:	64a2                	ld	s1,8(sp)
    80601754:	6105                	addi	sp,sp,32
    80601756:	8082                	ret
    80601758:	d86d                	beqz	s0,8060174a <growproc+0x52>
    8060175a:	68a8                	ld	a0,80(s1)
    8060175c:	00b40633          	add	a2,s0,a1
    80601760:	00001097          	auipc	ra,0x1
    80601764:	d04080e7          	jalr	-764(ra) # 80602464 <uvmdealloc>
    80601768:	85aa                	mv	a1,a0
    8060176a:	b7c5                	j	8060174a <growproc+0x52>
    8060176c:	557d                	li	a0,-1
    8060176e:	b7c5                	j	8060174e <growproc+0x56>

0000000080601770 <kfork>:
    80601770:	7179                	addi	sp,sp,-48
    80601772:	f406                	sd	ra,40(sp)
    80601774:	f022                	sd	s0,32(sp)
    80601776:	e052                	sd	s4,0(sp)
    80601778:	00001097          	auipc	ra,0x1
    8060177c:	a50080e7          	jalr	-1456(ra) # 806021c8 <push_off>
    80601780:	8792                	mv	a5,tp
    80601782:	2781                	sext.w	a5,a5
    80601784:	00016717          	auipc	a4,0x16
    80601788:	2ec70713          	addi	a4,a4,748 # 80617a70 <cpus>
    8060178c:	079e                	slli	a5,a5,0x7
    8060178e:	97ba                	add	a5,a5,a4
    80601790:	0007ba03          	ld	s4,0(a5)
    80601794:	00001097          	auipc	ra,0x1
    80601798:	ac4080e7          	jalr	-1340(ra) # 80602258 <pop_off>
    8060179c:	00000097          	auipc	ra,0x0
    806017a0:	df0080e7          	jalr	-528(ra) # 8060158c <allocproc>
    806017a4:	10050b63          	beqz	a0,806018ba <kfork+0x14a>
    806017a8:	e44e                	sd	s3,8(sp)
    806017aa:	692c                	ld	a1,80(a0)
    806017ac:	89aa                	mv	s3,a0
    806017ae:	048a3603          	ld	a2,72(s4)
    806017b2:	050a3503          	ld	a0,80(s4)
    806017b6:	00001097          	auipc	ra,0x1
    806017ba:	ca4080e7          	jalr	-860(ra) # 8060245a <uvmcopy>
    806017be:	0e054363          	bltz	a0,806018a4 <kfork+0x134>
    806017c2:	048a3703          	ld	a4,72(s4)
    806017c6:	058a3783          	ld	a5,88(s4)
    806017ca:	0589b683          	ld	a3,88(s3)
    806017ce:	ec26                	sd	s1,24(sp)
    806017d0:	e84a                	sd	s2,16(sp)
    806017d2:	04e9b423          	sd	a4,72(s3)
    806017d6:	12078613          	addi	a2,a5,288
    806017da:	8736                	mv	a4,a3
    806017dc:	6b8c                	ld	a1,16(a5)
    806017de:	0007b803          	ld	a6,0(a5)
    806017e2:	6788                	ld	a0,8(a5)
    806017e4:	eb0c                	sd	a1,16(a4)
    806017e6:	01073023          	sd	a6,0(a4)
    806017ea:	e708                	sd	a0,8(a4)
    806017ec:	6f8c                	ld	a1,24(a5)
    806017ee:	02078793          	addi	a5,a5,32
    806017f2:	02070713          	addi	a4,a4,32
    806017f6:	feb73c23          	sd	a1,-8(a4)
    806017fa:	fec791e3          	bne	a5,a2,806017dc <kfork+0x6c>
    806017fe:	0606b823          	sd	zero,112(a3) # 1070 <PAGE_SIZE+0x70>
    80601802:	0d0a0413          	addi	s0,s4,208
    80601806:	0d098493          	addi	s1,s3,208
    8060180a:	150a0913          	addi	s2,s4,336
    8060180e:	6008                	ld	a0,0(s0)
    80601810:	c511                	beqz	a0,8060181c <kfork+0xac>
    80601812:	00001097          	auipc	ra,0x1
    80601816:	c9e080e7          	jalr	-866(ra) # 806024b0 <filedup>
    8060181a:	e088                	sd	a0,0(s1)
    8060181c:	0421                	addi	s0,s0,8
    8060181e:	04a1                	addi	s1,s1,8
    80601820:	ff2417e3          	bne	s0,s2,8060180e <kfork+0x9e>
    80601824:	150a3503          	ld	a0,336(s4)
    80601828:	00001097          	auipc	ra,0x1
    8060182c:	c84080e7          	jalr	-892(ra) # 806024ac <idup>
    80601830:	158a0593          	addi	a1,s4,344
    80601834:	4641                	li	a2,16
    80601836:	14a9b823          	sd	a0,336(s3)
    8060183a:	15898513          	addi	a0,s3,344
    8060183e:	00001097          	auipc	ra,0x1
    80601842:	b9e080e7          	jalr	-1122(ra) # 806023dc <safestrcpy>
    80601846:	854e                	mv	a0,s3
    80601848:	0309a403          	lw	s0,48(s3)
    8060184c:	00001097          	auipc	ra,0x1
    80601850:	a5a080e7          	jalr	-1446(ra) # 806022a6 <release>
    80601854:	00016517          	auipc	a0,0x16
    80601858:	63450513          	addi	a0,a0,1588 # 80617e88 <wait_lock>
    8060185c:	00001097          	auipc	ra,0x1
    80601860:	9ac080e7          	jalr	-1620(ra) # 80602208 <acquire>
    80601864:	00016517          	auipc	a0,0x16
    80601868:	62450513          	addi	a0,a0,1572 # 80617e88 <wait_lock>
    8060186c:	0349bc23          	sd	s4,56(s3)
    80601870:	00001097          	auipc	ra,0x1
    80601874:	a36080e7          	jalr	-1482(ra) # 806022a6 <release>
    80601878:	854e                	mv	a0,s3
    8060187a:	00001097          	auipc	ra,0x1
    8060187e:	98e080e7          	jalr	-1650(ra) # 80602208 <acquire>
    80601882:	478d                	li	a5,3
    80601884:	00f9ac23          	sw	a5,24(s3)
    80601888:	854e                	mv	a0,s3
    8060188a:	00001097          	auipc	ra,0x1
    8060188e:	a1c080e7          	jalr	-1508(ra) # 806022a6 <release>
    80601892:	64e2                	ld	s1,24(sp)
    80601894:	6942                	ld	s2,16(sp)
    80601896:	69a2                	ld	s3,8(sp)
    80601898:	70a2                	ld	ra,40(sp)
    8060189a:	8522                	mv	a0,s0
    8060189c:	7402                	ld	s0,32(sp)
    8060189e:	6a02                	ld	s4,0(sp)
    806018a0:	6145                	addi	sp,sp,48
    806018a2:	8082                	ret
    806018a4:	854e                	mv	a0,s3
    806018a6:	00000097          	auipc	ra,0x0
    806018aa:	948080e7          	jalr	-1720(ra) # 806011ee <freeproc>
    806018ae:	854e                	mv	a0,s3
    806018b0:	00001097          	auipc	ra,0x1
    806018b4:	9f6080e7          	jalr	-1546(ra) # 806022a6 <release>
    806018b8:	69a2                	ld	s3,8(sp)
    806018ba:	547d                	li	s0,-1
    806018bc:	bff1                	j	80601898 <kfork+0x128>

00000000806018be <reparent>:
    806018be:	715d                	addi	sp,sp,-80
    806018c0:	fc26                	sd	s1,56(sp)
    806018c2:	f44e                	sd	s3,40(sp)
    806018c4:	f052                	sd	s4,32(sp)
    806018c6:	ec56                	sd	s5,24(sp)
    806018c8:	e85a                	sd	s6,16(sp)
    806018ca:	e45e                	sd	s7,8(sp)
    806018cc:	e486                	sd	ra,72(sp)
    806018ce:	e0a2                	sd	s0,64(sp)
    806018d0:	f84a                	sd	s2,48(sp)
    806018d2:	8b2a                	mv	s6,a0
    806018d4:	00016997          	auipc	s3,0x16
    806018d8:	5cc98993          	addi	s3,s3,1484 # 80617ea0 <proc>
    806018dc:	0001c497          	auipc	s1,0x1c
    806018e0:	fc448493          	addi	s1,s1,-60 # 8061d8a0 <trampoline>
    806018e4:	0001db97          	auipc	s7,0x1d
    806018e8:	fc4b8b93          	addi	s7,s7,-60 # 8061e8a8 <initproc>
    806018ec:	4a09                	li	s4,2
    806018ee:	4a8d                	li	s5,3
    806018f0:	a029                	j	806018fa <reparent+0x3c>
    806018f2:	16898993          	addi	s3,s3,360
    806018f6:	04998863          	beq	s3,s1,80601946 <reparent+0x88>
    806018fa:	0389b783          	ld	a5,56(s3)
    806018fe:	ff679ae3          	bne	a5,s6,806018f2 <reparent+0x34>
    80601902:	000bb903          	ld	s2,0(s7)
    80601906:	00016417          	auipc	s0,0x16
    8060190a:	59a40413          	addi	s0,s0,1434 # 80617ea0 <proc>
    8060190e:	0329bc23          	sd	s2,56(s3)
    80601912:	a809                	j	80601924 <reparent+0x66>
    80601914:	16840413          	addi	s0,s0,360
    80601918:	00001097          	auipc	ra,0x1
    8060191c:	98e080e7          	jalr	-1650(ra) # 806022a6 <release>
    80601920:	fc9409e3          	beq	s0,s1,806018f2 <reparent+0x34>
    80601924:	8522                	mv	a0,s0
    80601926:	00001097          	auipc	ra,0x1
    8060192a:	8e2080e7          	jalr	-1822(ra) # 80602208 <acquire>
    8060192e:	701c                	ld	a5,32(s0)
    80601930:	8522                	mv	a0,s0
    80601932:	fef911e3          	bne	s2,a5,80601914 <reparent+0x56>
    80601936:	4c1c                	lw	a5,24(s0)
    80601938:	02043023          	sd	zero,32(s0)
    8060193c:	fd479ce3          	bne	a5,s4,80601914 <reparent+0x56>
    80601940:	01542c23          	sw	s5,24(s0)
    80601944:	bfc1                	j	80601914 <reparent+0x56>
    80601946:	60a6                	ld	ra,72(sp)
    80601948:	6406                	ld	s0,64(sp)
    8060194a:	74e2                	ld	s1,56(sp)
    8060194c:	7942                	ld	s2,48(sp)
    8060194e:	79a2                	ld	s3,40(sp)
    80601950:	7a02                	ld	s4,32(sp)
    80601952:	6ae2                	ld	s5,24(sp)
    80601954:	6b42                	ld	s6,16(sp)
    80601956:	6ba2                	ld	s7,8(sp)
    80601958:	6161                	addi	sp,sp,80
    8060195a:	8082                	ret

000000008060195c <scheduler>:
    8060195c:	715d                	addi	sp,sp,-80
    8060195e:	e486                	sd	ra,72(sp)
    80601960:	e0a2                	sd	s0,64(sp)
    80601962:	fc26                	sd	s1,56(sp)
    80601964:	f84a                	sd	s2,48(sp)
    80601966:	f44e                	sd	s3,40(sp)
    80601968:	f052                	sd	s4,32(sp)
    8060196a:	ec56                	sd	s5,24(sp)
    8060196c:	e85a                	sd	s6,16(sp)
    8060196e:	e45e                	sd	s7,8(sp)
    80601970:	8792                	mv	a5,tp
    80601972:	2781                	sext.w	a5,a5
    80601974:	079e                	slli	a5,a5,0x7
    80601976:	00016a97          	auipc	s5,0x16
    8060197a:	0faa8a93          	addi	s5,s5,250 # 80617a70 <cpus>
    8060197e:	00fa89b3          	add	s3,s5,a5
    80601982:	00016a17          	auipc	s4,0x16
    80601986:	0f6a0a13          	addi	s4,s4,246 # 80617a78 <cpus+0x8>
    8060198a:	9a3e                	add	s4,s4,a5
    8060198c:	0009b023          	sd	zero,0(s3)
    80601990:	0001c917          	auipc	s2,0x1c
    80601994:	f1090913          	addi	s2,s2,-240 # 8061d8a0 <trampoline>
    80601998:	448d                	li	s1,3
    8060199a:	4b11                	li	s6,4
    8060199c:	10016073          	csrsi	sstatus,2
    806019a0:	10017073          	csrci	sstatus,2
    806019a4:	4b81                	li	s7,0
    806019a6:	00016417          	auipc	s0,0x16
    806019aa:	4fa40413          	addi	s0,s0,1274 # 80617ea0 <proc>
    806019ae:	a811                	j	806019c2 <scheduler+0x66>
    806019b0:	8522                	mv	a0,s0
    806019b2:	16840413          	addi	s0,s0,360
    806019b6:	00001097          	auipc	ra,0x1
    806019ba:	8f0080e7          	jalr	-1808(ra) # 806022a6 <release>
    806019be:	05240763          	beq	s0,s2,80601a0c <scheduler+0xb0>
    806019c2:	8522                	mv	a0,s0
    806019c4:	00001097          	auipc	ra,0x1
    806019c8:	844080e7          	jalr	-1980(ra) # 80602208 <acquire>
    806019cc:	4c1c                	lw	a5,24(s0)
    806019ce:	fe9791e3          	bne	a5,s1,806019b0 <scheduler+0x54>
    806019d2:	06040593          	addi	a1,s0,96
    806019d6:	8552                	mv	a0,s4
    806019d8:	01642c23          	sw	s6,24(s0)
    806019dc:	0089b023          	sd	s0,0(s3)
    806019e0:	fffff097          	auipc	ra,0xfffff
    806019e4:	424080e7          	jalr	1060(ra) # 80600e04 <swtch>
    806019e8:	8792                	mv	a5,tp
    806019ea:	2781                	sext.w	a5,a5
    806019ec:	079e                	slli	a5,a5,0x7
    806019ee:	97d6                	add	a5,a5,s5
    806019f0:	8522                	mv	a0,s0
    806019f2:	16840413          	addi	s0,s0,360
    806019f6:	0607ae23          	sw	zero,124(a5)
    806019fa:	0009b023          	sd	zero,0(s3)
    806019fe:	4b85                	li	s7,1
    80601a00:	00001097          	auipc	ra,0x1
    80601a04:	8a6080e7          	jalr	-1882(ra) # 806022a6 <release>
    80601a08:	fb241de3          	bne	s0,s2,806019c2 <scheduler+0x66>
    80601a0c:	f80b98e3          	bnez	s7,8060199c <scheduler+0x40>
    80601a10:	10500073          	wfi
    80601a14:	b761                	j	8060199c <scheduler+0x40>

0000000080601a16 <sched>:
    80601a16:	1101                	addi	sp,sp,-32
    80601a18:	ec06                	sd	ra,24(sp)
    80601a1a:	e822                	sd	s0,16(sp)
    80601a1c:	e426                	sd	s1,8(sp)
    80601a1e:	e04a                	sd	s2,0(sp)
    80601a20:	00000097          	auipc	ra,0x0
    80601a24:	7a8080e7          	jalr	1960(ra) # 806021c8 <push_off>
    80601a28:	8792                	mv	a5,tp
    80601a2a:	2781                	sext.w	a5,a5
    80601a2c:	079e                	slli	a5,a5,0x7
    80601a2e:	00016417          	auipc	s0,0x16
    80601a32:	04240413          	addi	s0,s0,66 # 80617a70 <cpus>
    80601a36:	97a2                	add	a5,a5,s0
    80601a38:	6384                	ld	s1,0(a5)
    80601a3a:	00001097          	auipc	ra,0x1
    80601a3e:	81e080e7          	jalr	-2018(ra) # 80602258 <pop_off>
    80601a42:	8526                	mv	a0,s1
    80601a44:	00000097          	auipc	ra,0x0
    80601a48:	75c080e7          	jalr	1884(ra) # 806021a0 <holding>
    80601a4c:	cd31                	beqz	a0,80601aa8 <sched+0x92>
    80601a4e:	8792                	mv	a5,tp
    80601a50:	2781                	sext.w	a5,a5
    80601a52:	079e                	slli	a5,a5,0x7
    80601a54:	97a2                	add	a5,a5,s0
    80601a56:	5fb8                	lw	a4,120(a5)
    80601a58:	4785                	li	a5,1
    80601a5a:	06f71f63          	bne	a4,a5,80601ad8 <sched+0xc2>
    80601a5e:	4c98                	lw	a4,24(s1)
    80601a60:	4791                	li	a5,4
    80601a62:	06f70363          	beq	a4,a5,80601ac8 <sched+0xb2>
    80601a66:	100027f3          	csrr	a5,sstatus
    80601a6a:	8b89                	andi	a5,a5,2
    80601a6c:	e7b1                	bnez	a5,80601ab8 <sched+0xa2>
    80601a6e:	8792                	mv	a5,tp
    80601a70:	2781                	sext.w	a5,a5
    80601a72:	079e                	slli	a5,a5,0x7
    80601a74:	97a2                	add	a5,a5,s0
    80601a76:	07c7a903          	lw	s2,124(a5)
    80601a7a:	8592                	mv	a1,tp
    80601a7c:	2581                	sext.w	a1,a1
    80601a7e:	059e                	slli	a1,a1,0x7
    80601a80:	05a1                	addi	a1,a1,8
    80601a82:	95a2                	add	a1,a1,s0
    80601a84:	06048513          	addi	a0,s1,96
    80601a88:	fffff097          	auipc	ra,0xfffff
    80601a8c:	37c080e7          	jalr	892(ra) # 80600e04 <swtch>
    80601a90:	8792                	mv	a5,tp
    80601a92:	2781                	sext.w	a5,a5
    80601a94:	079e                	slli	a5,a5,0x7
    80601a96:	943e                	add	s0,s0,a5
    80601a98:	07242e23          	sw	s2,124(s0)
    80601a9c:	60e2                	ld	ra,24(sp)
    80601a9e:	6442                	ld	s0,16(sp)
    80601aa0:	64a2                	ld	s1,8(sp)
    80601aa2:	6902                	ld	s2,0(sp)
    80601aa4:	6105                	addi	sp,sp,32
    80601aa6:	8082                	ret
    80601aa8:	00001517          	auipc	a0,0x1
    80601aac:	db050513          	addi	a0,a0,-592 # 80602858 <kexec+0x3a6>
    80601ab0:	fffff097          	auipc	ra,0xfffff
    80601ab4:	6e6080e7          	jalr	1766(ra) # 80601196 <panic>
    80601ab8:	00001517          	auipc	a0,0x1
    80601abc:	dd050513          	addi	a0,a0,-560 # 80602888 <kexec+0x3d6>
    80601ac0:	fffff097          	auipc	ra,0xfffff
    80601ac4:	6d6080e7          	jalr	1750(ra) # 80601196 <panic>
    80601ac8:	00001517          	auipc	a0,0x1
    80601acc:	db050513          	addi	a0,a0,-592 # 80602878 <kexec+0x3c6>
    80601ad0:	fffff097          	auipc	ra,0xfffff
    80601ad4:	6c6080e7          	jalr	1734(ra) # 80601196 <panic>
    80601ad8:	00001517          	auipc	a0,0x1
    80601adc:	d9050513          	addi	a0,a0,-624 # 80602868 <kexec+0x3b6>
    80601ae0:	fffff097          	auipc	ra,0xfffff
    80601ae4:	6b6080e7          	jalr	1718(ra) # 80601196 <panic>

0000000080601ae8 <kexit>:
    80601ae8:	7139                	addi	sp,sp,-64
    80601aea:	e05a                	sd	s6,0(sp)
    80601aec:	fc06                	sd	ra,56(sp)
    80601aee:	e852                	sd	s4,16(sp)
    80601af0:	8b2a                	mv	s6,a0
    80601af2:	00000097          	auipc	ra,0x0
    80601af6:	6d6080e7          	jalr	1750(ra) # 806021c8 <push_off>
    80601afa:	8792                	mv	a5,tp
    80601afc:	2781                	sext.w	a5,a5
    80601afe:	00016717          	auipc	a4,0x16
    80601b02:	f7270713          	addi	a4,a4,-142 # 80617a70 <cpus>
    80601b06:	079e                	slli	a5,a5,0x7
    80601b08:	97ba                	add	a5,a5,a4
    80601b0a:	0007ba03          	ld	s4,0(a5)
    80601b0e:	00000097          	auipc	ra,0x0
    80601b12:	74a080e7          	jalr	1866(ra) # 80602258 <pop_off>
    80601b16:	0001d797          	auipc	a5,0x1d
    80601b1a:	d927b783          	ld	a5,-622(a5) # 8061e8a8 <initproc>
    80601b1e:	f822                	sd	s0,48(sp)
    80601b20:	f426                	sd	s1,40(sp)
    80601b22:	f04a                	sd	s2,32(sp)
    80601b24:	ec4e                	sd	s3,24(sp)
    80601b26:	e456                	sd	s5,8(sp)
    80601b28:	0d0a0413          	addi	s0,s4,208
    80601b2c:	150a0493          	addi	s1,s4,336
    80601b30:	0f478663          	beq	a5,s4,80601c1c <kexit+0x134>
    80601b34:	6008                	ld	a0,0(s0)
    80601b36:	c519                	beqz	a0,80601b44 <kexit+0x5c>
    80601b38:	00001097          	auipc	ra,0x1
    80601b3c:	976080e7          	jalr	-1674(ra) # 806024ae <fileclose>
    80601b40:	00043023          	sd	zero,0(s0)
    80601b44:	0421                	addi	s0,s0,8
    80601b46:	fe9417e3          	bne	s0,s1,80601b34 <kexit+0x4c>
    80601b4a:	00001097          	auipc	ra,0x1
    80601b4e:	956080e7          	jalr	-1706(ra) # 806024a0 <begin_op>
    80601b52:	150a3503          	ld	a0,336(s4)
    80601b56:	00016417          	auipc	s0,0x16
    80601b5a:	34a40413          	addi	s0,s0,842 # 80617ea0 <proc>
    80601b5e:	0001c497          	auipc	s1,0x1c
    80601b62:	d4248493          	addi	s1,s1,-702 # 8061d8a0 <trampoline>
    80601b66:	00001097          	auipc	ra,0x1
    80601b6a:	944080e7          	jalr	-1724(ra) # 806024aa <iput>
    80601b6e:	00001097          	auipc	ra,0x1
    80601b72:	934080e7          	jalr	-1740(ra) # 806024a2 <end_op>
    80601b76:	00016517          	auipc	a0,0x16
    80601b7a:	31250513          	addi	a0,a0,786 # 80617e88 <wait_lock>
    80601b7e:	140a3823          	sd	zero,336(s4)
    80601b82:	00000097          	auipc	ra,0x0
    80601b86:	686080e7          	jalr	1670(ra) # 80602208 <acquire>
    80601b8a:	8552                	mv	a0,s4
    80601b8c:	00000097          	auipc	ra,0x0
    80601b90:	d32080e7          	jalr	-718(ra) # 806018be <reparent>
    80601b94:	4989                	li	s3,2
    80601b96:	038a3903          	ld	s2,56(s4)
    80601b9a:	4a8d                	li	s5,3
    80601b9c:	a811                	j	80601bb0 <kexit+0xc8>
    80601b9e:	8522                	mv	a0,s0
    80601ba0:	16840413          	addi	s0,s0,360
    80601ba4:	00000097          	auipc	ra,0x0
    80601ba8:	702080e7          	jalr	1794(ra) # 806022a6 <release>
    80601bac:	02940a63          	beq	s0,s1,80601be0 <kexit+0xf8>
    80601bb0:	8522                	mv	a0,s0
    80601bb2:	00000097          	auipc	ra,0x0
    80601bb6:	656080e7          	jalr	1622(ra) # 80602208 <acquire>
    80601bba:	701c                	ld	a5,32(s0)
    80601bbc:	fef911e3          	bne	s2,a5,80601b9e <kexit+0xb6>
    80601bc0:	4c1c                	lw	a5,24(s0)
    80601bc2:	02043023          	sd	zero,32(s0)
    80601bc6:	fd379ce3          	bne	a5,s3,80601b9e <kexit+0xb6>
    80601bca:	01542c23          	sw	s5,24(s0)
    80601bce:	8522                	mv	a0,s0
    80601bd0:	16840413          	addi	s0,s0,360
    80601bd4:	00000097          	auipc	ra,0x0
    80601bd8:	6d2080e7          	jalr	1746(ra) # 806022a6 <release>
    80601bdc:	fc941ae3          	bne	s0,s1,80601bb0 <kexit+0xc8>
    80601be0:	8552                	mv	a0,s4
    80601be2:	00000097          	auipc	ra,0x0
    80601be6:	626080e7          	jalr	1574(ra) # 80602208 <acquire>
    80601bea:	4795                	li	a5,5
    80601bec:	00016517          	auipc	a0,0x16
    80601bf0:	29c50513          	addi	a0,a0,668 # 80617e88 <wait_lock>
    80601bf4:	00fa2c23          	sw	a5,24(s4)
    80601bf8:	036a2623          	sw	s6,44(s4)
    80601bfc:	00000097          	auipc	ra,0x0
    80601c00:	6aa080e7          	jalr	1706(ra) # 806022a6 <release>
    80601c04:	00000097          	auipc	ra,0x0
    80601c08:	e12080e7          	jalr	-494(ra) # 80601a16 <sched>
    80601c0c:	00001517          	auipc	a0,0x1
    80601c10:	ca450513          	addi	a0,a0,-860 # 806028b0 <kexec+0x3fe>
    80601c14:	fffff097          	auipc	ra,0xfffff
    80601c18:	582080e7          	jalr	1410(ra) # 80601196 <panic>
    80601c1c:	00001517          	auipc	a0,0x1
    80601c20:	c8450513          	addi	a0,a0,-892 # 806028a0 <kexec+0x3ee>
    80601c24:	fffff097          	auipc	ra,0xfffff
    80601c28:	572080e7          	jalr	1394(ra) # 80601196 <panic>

0000000080601c2c <kwait>:
    80601c2c:	7139                	addi	sp,sp,-64
    80601c2e:	e456                	sd	s5,8(sp)
    80601c30:	fc06                	sd	ra,56(sp)
    80601c32:	f822                	sd	s0,48(sp)
    80601c34:	f426                	sd	s1,40(sp)
    80601c36:	f04a                	sd	s2,32(sp)
    80601c38:	ec4e                	sd	s3,24(sp)
    80601c3a:	e852                	sd	s4,16(sp)
    80601c3c:	8aaa                	mv	s5,a0
    80601c3e:	00000097          	auipc	ra,0x0
    80601c42:	58a080e7          	jalr	1418(ra) # 806021c8 <push_off>
    80601c46:	8792                	mv	a5,tp
    80601c48:	2781                	sext.w	a5,a5
    80601c4a:	079e                	slli	a5,a5,0x7
    80601c4c:	00016a17          	auipc	s4,0x16
    80601c50:	e24a0a13          	addi	s4,s4,-476 # 80617a70 <cpus>
    80601c54:	97d2                	add	a5,a5,s4
    80601c56:	6384                	ld	s1,0(a5)
    80601c58:	00000097          	auipc	ra,0x0
    80601c5c:	600080e7          	jalr	1536(ra) # 80602258 <pop_off>
    80601c60:	00016517          	auipc	a0,0x16
    80601c64:	22850513          	addi	a0,a0,552 # 80617e88 <wait_lock>
    80601c68:	00000097          	auipc	ra,0x0
    80601c6c:	5a0080e7          	jalr	1440(ra) # 80602208 <acquire>
    80601c70:	4995                	li	s3,5
    80601c72:	0001c917          	auipc	s2,0x1c
    80601c76:	c2e90913          	addi	s2,s2,-978 # 8061d8a0 <trampoline>
    80601c7a:	4701                	li	a4,0
    80601c7c:	00016417          	auipc	s0,0x16
    80601c80:	22440413          	addi	s0,s0,548 # 80617ea0 <proc>
    80601c84:	a029                	j	80601c8e <kwait+0x62>
    80601c86:	16840413          	addi	s0,s0,360
    80601c8a:	03240763          	beq	s0,s2,80601cb8 <kwait+0x8c>
    80601c8e:	7c1c                	ld	a5,56(s0)
    80601c90:	fe979be3          	bne	a5,s1,80601c86 <kwait+0x5a>
    80601c94:	8522                	mv	a0,s0
    80601c96:	00000097          	auipc	ra,0x0
    80601c9a:	572080e7          	jalr	1394(ra) # 80602208 <acquire>
    80601c9e:	4c1c                	lw	a5,24(s0)
    80601ca0:	8522                	mv	a0,s0
    80601ca2:	0d378163          	beq	a5,s3,80601d64 <kwait+0x138>
    80601ca6:	00000097          	auipc	ra,0x0
    80601caa:	600080e7          	jalr	1536(ra) # 806022a6 <release>
    80601cae:	16840413          	addi	s0,s0,360
    80601cb2:	4705                	li	a4,1
    80601cb4:	fd241de3          	bne	s0,s2,80601c8e <kwait+0x62>
    80601cb8:	10070463          	beqz	a4,80601dc0 <kwait+0x194>
    80601cbc:	8526                	mv	a0,s1
    80601cbe:	00000097          	auipc	ra,0x0
    80601cc2:	54a080e7          	jalr	1354(ra) # 80602208 <acquire>
    80601cc6:	5480                	lw	s0,40(s1)
    80601cc8:	8526                	mv	a0,s1
    80601cca:	00000097          	auipc	ra,0x0
    80601cce:	5dc080e7          	jalr	1500(ra) # 806022a6 <release>
    80601cd2:	e47d                	bnez	s0,80601dc0 <kwait+0x194>
    80601cd4:	00000097          	auipc	ra,0x0
    80601cd8:	4f4080e7          	jalr	1268(ra) # 806021c8 <push_off>
    80601cdc:	8792                	mv	a5,tp
    80601cde:	2781                	sext.w	a5,a5
    80601ce0:	079e                	slli	a5,a5,0x7
    80601ce2:	97d2                	add	a5,a5,s4
    80601ce4:	6380                	ld	s0,0(a5)
    80601ce6:	00000097          	auipc	ra,0x0
    80601cea:	572080e7          	jalr	1394(ra) # 80602258 <pop_off>
    80601cee:	8522                	mv	a0,s0
    80601cf0:	00000097          	auipc	ra,0x0
    80601cf4:	518080e7          	jalr	1304(ra) # 80602208 <acquire>
    80601cf8:	8522                	mv	a0,s0
    80601cfa:	f004                	sd	s1,32(s0)
    80601cfc:	00000097          	auipc	ra,0x0
    80601d00:	5aa080e7          	jalr	1450(ra) # 806022a6 <release>
    80601d04:	00016517          	auipc	a0,0x16
    80601d08:	18450513          	addi	a0,a0,388 # 80617e88 <wait_lock>
    80601d0c:	00000097          	auipc	ra,0x0
    80601d10:	59a080e7          	jalr	1434(ra) # 806022a6 <release>
    80601d14:	00000097          	auipc	ra,0x0
    80601d18:	4b4080e7          	jalr	1204(ra) # 806021c8 <push_off>
    80601d1c:	8792                	mv	a5,tp
    80601d1e:	2781                	sext.w	a5,a5
    80601d20:	079e                	slli	a5,a5,0x7
    80601d22:	97d2                	add	a5,a5,s4
    80601d24:	6380                	ld	s0,0(a5)
    80601d26:	00000097          	auipc	ra,0x0
    80601d2a:	532080e7          	jalr	1330(ra) # 80602258 <pop_off>
    80601d2e:	8522                	mv	a0,s0
    80601d30:	00000097          	auipc	ra,0x0
    80601d34:	4d8080e7          	jalr	1240(ra) # 80602208 <acquire>
    80601d38:	701c                	ld	a5,32(s0)
    80601d3a:	c799                	beqz	a5,80601d48 <kwait+0x11c>
    80601d3c:	4789                	li	a5,2
    80601d3e:	cc1c                	sw	a5,24(s0)
    80601d40:	00000097          	auipc	ra,0x0
    80601d44:	cd6080e7          	jalr	-810(ra) # 80601a16 <sched>
    80601d48:	8522                	mv	a0,s0
    80601d4a:	00000097          	auipc	ra,0x0
    80601d4e:	55c080e7          	jalr	1372(ra) # 806022a6 <release>
    80601d52:	00016517          	auipc	a0,0x16
    80601d56:	13650513          	addi	a0,a0,310 # 80617e88 <wait_lock>
    80601d5a:	00000097          	auipc	ra,0x0
    80601d5e:	4ae080e7          	jalr	1198(ra) # 80602208 <acquire>
    80601d62:	bf21                	j	80601c7a <kwait+0x4e>
    80601d64:	03042903          	lw	s2,48(s0)
    80601d68:	000a8e63          	beqz	s5,80601d84 <kwait+0x158>
    80601d6c:	64ac                	ld	a1,72(s1)
    80601d6e:	68a8                	ld	a0,80(s1)
    80601d70:	8656                	mv	a2,s5
    80601d72:	4711                	li	a4,4
    80601d74:	02c40693          	addi	a3,s0,44
    80601d78:	00000097          	auipc	ra,0x0
    80601d7c:	70c080e7          	jalr	1804(ra) # 80602484 <copyout>
    80601d80:	04054a63          	bltz	a0,80601dd4 <kwait+0x1a8>
    80601d84:	8522                	mv	a0,s0
    80601d86:	02043c23          	sd	zero,56(s0)
    80601d8a:	fffff097          	auipc	ra,0xfffff
    80601d8e:	464080e7          	jalr	1124(ra) # 806011ee <freeproc>
    80601d92:	8522                	mv	a0,s0
    80601d94:	00000097          	auipc	ra,0x0
    80601d98:	512080e7          	jalr	1298(ra) # 806022a6 <release>
    80601d9c:	00016517          	auipc	a0,0x16
    80601da0:	0ec50513          	addi	a0,a0,236 # 80617e88 <wait_lock>
    80601da4:	00000097          	auipc	ra,0x0
    80601da8:	502080e7          	jalr	1282(ra) # 806022a6 <release>
    80601dac:	70e2                	ld	ra,56(sp)
    80601dae:	7442                	ld	s0,48(sp)
    80601db0:	74a2                	ld	s1,40(sp)
    80601db2:	69e2                	ld	s3,24(sp)
    80601db4:	6a42                	ld	s4,16(sp)
    80601db6:	6aa2                	ld	s5,8(sp)
    80601db8:	854a                	mv	a0,s2
    80601dba:	7902                	ld	s2,32(sp)
    80601dbc:	6121                	addi	sp,sp,64
    80601dbe:	8082                	ret
    80601dc0:	00016517          	auipc	a0,0x16
    80601dc4:	0c850513          	addi	a0,a0,200 # 80617e88 <wait_lock>
    80601dc8:	00000097          	auipc	ra,0x0
    80601dcc:	4de080e7          	jalr	1246(ra) # 806022a6 <release>
    80601dd0:	597d                	li	s2,-1
    80601dd2:	bfe9                	j	80601dac <kwait+0x180>
    80601dd4:	8522                	mv	a0,s0
    80601dd6:	00000097          	auipc	ra,0x0
    80601dda:	4d0080e7          	jalr	1232(ra) # 806022a6 <release>
    80601dde:	00016517          	auipc	a0,0x16
    80601de2:	0aa50513          	addi	a0,a0,170 # 80617e88 <wait_lock>
    80601de6:	00000097          	auipc	ra,0x0
    80601dea:	4c0080e7          	jalr	1216(ra) # 806022a6 <release>
    80601dee:	597d                	li	s2,-1
    80601df0:	bf75                	j	80601dac <kwait+0x180>

0000000080601df2 <yield>:
    80601df2:	1141                	addi	sp,sp,-16
    80601df4:	e406                	sd	ra,8(sp)
    80601df6:	e022                	sd	s0,0(sp)
    80601df8:	00000097          	auipc	ra,0x0
    80601dfc:	3d0080e7          	jalr	976(ra) # 806021c8 <push_off>
    80601e00:	8792                	mv	a5,tp
    80601e02:	2781                	sext.w	a5,a5
    80601e04:	00016717          	auipc	a4,0x16
    80601e08:	c6c70713          	addi	a4,a4,-916 # 80617a70 <cpus>
    80601e0c:	079e                	slli	a5,a5,0x7
    80601e0e:	97ba                	add	a5,a5,a4
    80601e10:	6380                	ld	s0,0(a5)
    80601e12:	00000097          	auipc	ra,0x0
    80601e16:	446080e7          	jalr	1094(ra) # 80602258 <pop_off>
    80601e1a:	8522                	mv	a0,s0
    80601e1c:	00000097          	auipc	ra,0x0
    80601e20:	3ec080e7          	jalr	1004(ra) # 80602208 <acquire>
    80601e24:	478d                	li	a5,3
    80601e26:	cc1c                	sw	a5,24(s0)
    80601e28:	00000097          	auipc	ra,0x0
    80601e2c:	bee080e7          	jalr	-1042(ra) # 80601a16 <sched>
    80601e30:	8522                	mv	a0,s0
    80601e32:	6402                	ld	s0,0(sp)
    80601e34:	60a2                	ld	ra,8(sp)
    80601e36:	0141                	addi	sp,sp,16
    80601e38:	00000317          	auipc	t1,0x0
    80601e3c:	46e30067          	jr	1134(t1) # 806022a6 <release>

0000000080601e40 <sleep_prepare>:
    80601e40:	1101                	addi	sp,sp,-32
    80601e42:	e426                	sd	s1,8(sp)
    80601e44:	ec06                	sd	ra,24(sp)
    80601e46:	e822                	sd	s0,16(sp)
    80601e48:	84aa                	mv	s1,a0
    80601e4a:	00000097          	auipc	ra,0x0
    80601e4e:	37e080e7          	jalr	894(ra) # 806021c8 <push_off>
    80601e52:	8792                	mv	a5,tp
    80601e54:	2781                	sext.w	a5,a5
    80601e56:	00016717          	auipc	a4,0x16
    80601e5a:	c1a70713          	addi	a4,a4,-998 # 80617a70 <cpus>
    80601e5e:	079e                	slli	a5,a5,0x7
    80601e60:	97ba                	add	a5,a5,a4
    80601e62:	6380                	ld	s0,0(a5)
    80601e64:	00000097          	auipc	ra,0x0
    80601e68:	3f4080e7          	jalr	1012(ra) # 80602258 <pop_off>
    80601e6c:	8522                	mv	a0,s0
    80601e6e:	00000097          	auipc	ra,0x0
    80601e72:	39a080e7          	jalr	922(ra) # 80602208 <acquire>
    80601e76:	c899                	beqz	s1,80601e8c <sleep_prepare+0x4c>
    80601e78:	f004                	sd	s1,32(s0)
    80601e7a:	8522                	mv	a0,s0
    80601e7c:	6442                	ld	s0,16(sp)
    80601e7e:	60e2                	ld	ra,24(sp)
    80601e80:	64a2                	ld	s1,8(sp)
    80601e82:	6105                	addi	sp,sp,32
    80601e84:	00000317          	auipc	t1,0x0
    80601e88:	42230067          	jr	1058(t1) # 806022a6 <release>
    80601e8c:	00001517          	auipc	a0,0x1
    80601e90:	a3450513          	addi	a0,a0,-1484 # 806028c0 <kexec+0x40e>
    80601e94:	fffff097          	auipc	ra,0xfffff
    80601e98:	302080e7          	jalr	770(ra) # 80601196 <panic>

0000000080601e9c <sleep>:
    80601e9c:	1141                	addi	sp,sp,-16
    80601e9e:	e406                	sd	ra,8(sp)
    80601ea0:	e022                	sd	s0,0(sp)
    80601ea2:	00000097          	auipc	ra,0x0
    80601ea6:	326080e7          	jalr	806(ra) # 806021c8 <push_off>
    80601eaa:	8792                	mv	a5,tp
    80601eac:	2781                	sext.w	a5,a5
    80601eae:	00016717          	auipc	a4,0x16
    80601eb2:	bc270713          	addi	a4,a4,-1086 # 80617a70 <cpus>
    80601eb6:	079e                	slli	a5,a5,0x7
    80601eb8:	97ba                	add	a5,a5,a4
    80601eba:	6380                	ld	s0,0(a5)
    80601ebc:	00000097          	auipc	ra,0x0
    80601ec0:	39c080e7          	jalr	924(ra) # 80602258 <pop_off>
    80601ec4:	8522                	mv	a0,s0
    80601ec6:	00000097          	auipc	ra,0x0
    80601eca:	342080e7          	jalr	834(ra) # 80602208 <acquire>
    80601ece:	701c                	ld	a5,32(s0)
    80601ed0:	c799                	beqz	a5,80601ede <sleep+0x42>
    80601ed2:	4789                	li	a5,2
    80601ed4:	cc1c                	sw	a5,24(s0)
    80601ed6:	00000097          	auipc	ra,0x0
    80601eda:	b40080e7          	jalr	-1216(ra) # 80601a16 <sched>
    80601ede:	8522                	mv	a0,s0
    80601ee0:	6402                	ld	s0,0(sp)
    80601ee2:	60a2                	ld	ra,8(sp)
    80601ee4:	0141                	addi	sp,sp,16
    80601ee6:	00000317          	auipc	t1,0x0
    80601eea:	3c030067          	jr	960(t1) # 806022a6 <release>

0000000080601eee <wakeup>:
    80601eee:	7179                	addi	sp,sp,-48
    80601ef0:	f022                	sd	s0,32(sp)
    80601ef2:	ec26                	sd	s1,24(sp)
    80601ef4:	e84a                	sd	s2,16(sp)
    80601ef6:	e44e                	sd	s3,8(sp)
    80601ef8:	e052                	sd	s4,0(sp)
    80601efa:	f406                	sd	ra,40(sp)
    80601efc:	892a                	mv	s2,a0
    80601efe:	00016417          	auipc	s0,0x16
    80601f02:	fa240413          	addi	s0,s0,-94 # 80617ea0 <proc>
    80601f06:	0001c497          	auipc	s1,0x1c
    80601f0a:	99a48493          	addi	s1,s1,-1638 # 8061d8a0 <trampoline>
    80601f0e:	4989                	li	s3,2
    80601f10:	4a0d                	li	s4,3
    80601f12:	a809                	j	80601f24 <wakeup+0x36>
    80601f14:	16840413          	addi	s0,s0,360
    80601f18:	00000097          	auipc	ra,0x0
    80601f1c:	38e080e7          	jalr	910(ra) # 806022a6 <release>
    80601f20:	02940a63          	beq	s0,s1,80601f54 <wakeup+0x66>
    80601f24:	8522                	mv	a0,s0
    80601f26:	00000097          	auipc	ra,0x0
    80601f2a:	2e2080e7          	jalr	738(ra) # 80602208 <acquire>
    80601f2e:	701c                	ld	a5,32(s0)
    80601f30:	8522                	mv	a0,s0
    80601f32:	ff2791e3          	bne	a5,s2,80601f14 <wakeup+0x26>
    80601f36:	4c1c                	lw	a5,24(s0)
    80601f38:	02043023          	sd	zero,32(s0)
    80601f3c:	fd379ce3          	bne	a5,s3,80601f14 <wakeup+0x26>
    80601f40:	01442c23          	sw	s4,24(s0)
    80601f44:	16840413          	addi	s0,s0,360
    80601f48:	00000097          	auipc	ra,0x0
    80601f4c:	35e080e7          	jalr	862(ra) # 806022a6 <release>
    80601f50:	fc941ae3          	bne	s0,s1,80601f24 <wakeup+0x36>
    80601f54:	70a2                	ld	ra,40(sp)
    80601f56:	7402                	ld	s0,32(sp)
    80601f58:	64e2                	ld	s1,24(sp)
    80601f5a:	6942                	ld	s2,16(sp)
    80601f5c:	69a2                	ld	s3,8(sp)
    80601f5e:	6a02                	ld	s4,0(sp)
    80601f60:	6145                	addi	sp,sp,48
    80601f62:	8082                	ret

0000000080601f64 <kkill>:
    80601f64:	cd25                	beqz	a0,80601fdc <kkill+0x78>
    80601f66:	1101                	addi	sp,sp,-32
    80601f68:	e822                	sd	s0,16(sp)
    80601f6a:	e426                	sd	s1,8(sp)
    80601f6c:	e04a                	sd	s2,0(sp)
    80601f6e:	ec06                	sd	ra,24(sp)
    80601f70:	84aa                	mv	s1,a0
    80601f72:	00016417          	auipc	s0,0x16
    80601f76:	f2e40413          	addi	s0,s0,-210 # 80617ea0 <proc>
    80601f7a:	0001c917          	auipc	s2,0x1c
    80601f7e:	92690913          	addi	s2,s2,-1754 # 8061d8a0 <trampoline>
    80601f82:	a809                	j	80601f94 <kkill+0x30>
    80601f84:	16840413          	addi	s0,s0,360
    80601f88:	00000097          	auipc	ra,0x0
    80601f8c:	31e080e7          	jalr	798(ra) # 806022a6 <release>
    80601f90:	03240f63          	beq	s0,s2,80601fce <kkill+0x6a>
    80601f94:	8522                	mv	a0,s0
    80601f96:	00000097          	auipc	ra,0x0
    80601f9a:	272080e7          	jalr	626(ra) # 80602208 <acquire>
    80601f9e:	581c                	lw	a5,48(s0)
    80601fa0:	8522                	mv	a0,s0
    80601fa2:	fe9791e3          	bne	a5,s1,80601f84 <kkill+0x20>
    80601fa6:	4c18                	lw	a4,24(s0)
    80601fa8:	4785                	li	a5,1
    80601faa:	d41c                	sw	a5,40(s0)
    80601fac:	4789                	li	a5,2
    80601fae:	00f71463          	bne	a4,a5,80601fb6 <kkill+0x52>
    80601fb2:	478d                	li	a5,3
    80601fb4:	cc1c                	sw	a5,24(s0)
    80601fb6:	8522                	mv	a0,s0
    80601fb8:	00000097          	auipc	ra,0x0
    80601fbc:	2ee080e7          	jalr	750(ra) # 806022a6 <release>
    80601fc0:	60e2                	ld	ra,24(sp)
    80601fc2:	6442                	ld	s0,16(sp)
    80601fc4:	64a2                	ld	s1,8(sp)
    80601fc6:	6902                	ld	s2,0(sp)
    80601fc8:	4501                	li	a0,0
    80601fca:	6105                	addi	sp,sp,32
    80601fcc:	8082                	ret
    80601fce:	60e2                	ld	ra,24(sp)
    80601fd0:	6442                	ld	s0,16(sp)
    80601fd2:	64a2                	ld	s1,8(sp)
    80601fd4:	6902                	ld	s2,0(sp)
    80601fd6:	557d                	li	a0,-1
    80601fd8:	6105                	addi	sp,sp,32
    80601fda:	8082                	ret
    80601fdc:	557d                	li	a0,-1
    80601fde:	8082                	ret

0000000080601fe0 <setkilled>:
    80601fe0:	1101                	addi	sp,sp,-32
    80601fe2:	ec06                	sd	ra,24(sp)
    80601fe4:	e42a                	sd	a0,8(sp)
    80601fe6:	00000097          	auipc	ra,0x0
    80601fea:	222080e7          	jalr	546(ra) # 80602208 <acquire>
    80601fee:	6522                	ld	a0,8(sp)
    80601ff0:	60e2                	ld	ra,24(sp)
    80601ff2:	4785                	li	a5,1
    80601ff4:	d51c                	sw	a5,40(a0)
    80601ff6:	6105                	addi	sp,sp,32
    80601ff8:	00000317          	auipc	t1,0x0
    80601ffc:	2ae30067          	jr	686(t1) # 806022a6 <release>

0000000080602000 <killed>:
    80602000:	1101                	addi	sp,sp,-32
    80602002:	ec06                	sd	ra,24(sp)
    80602004:	e822                	sd	s0,16(sp)
    80602006:	e42a                	sd	a0,8(sp)
    80602008:	00000097          	auipc	ra,0x0
    8060200c:	200080e7          	jalr	512(ra) # 80602208 <acquire>
    80602010:	6522                	ld	a0,8(sp)
    80602012:	551c                	lw	a5,40(a0)
    80602014:	843e                	mv	s0,a5
    80602016:	00000097          	auipc	ra,0x0
    8060201a:	290080e7          	jalr	656(ra) # 806022a6 <release>
    8060201e:	60e2                	ld	ra,24(sp)
    80602020:	8522                	mv	a0,s0
    80602022:	6442                	ld	s0,16(sp)
    80602024:	6105                	addi	sp,sp,32
    80602026:	8082                	ret

0000000080602028 <either_copyout>:
    80602028:	7179                	addi	sp,sp,-48
    8060202a:	ec26                	sd	s1,24(sp)
    8060202c:	e84a                	sd	s2,16(sp)
    8060202e:	f406                	sd	ra,40(sp)
    80602030:	f022                	sd	s0,32(sp)
    80602032:	e42a                	sd	a0,8(sp)
    80602034:	892e                	mv	s2,a1
    80602036:	84b2                	mv	s1,a2
    80602038:	e036                	sd	a3,0(sp)
    8060203a:	00000097          	auipc	ra,0x0
    8060203e:	18e080e7          	jalr	398(ra) # 806021c8 <push_off>
    80602042:	8792                	mv	a5,tp
    80602044:	2781                	sext.w	a5,a5
    80602046:	00016717          	auipc	a4,0x16
    8060204a:	a2a70713          	addi	a4,a4,-1494 # 80617a70 <cpus>
    8060204e:	079e                	slli	a5,a5,0x7
    80602050:	97ba                	add	a5,a5,a4
    80602052:	6380                	ld	s0,0(a5)
    80602054:	00000097          	auipc	ra,0x0
    80602058:	204080e7          	jalr	516(ra) # 80602258 <pop_off>
    8060205c:	6522                	ld	a0,8(sp)
    8060205e:	cd19                	beqz	a0,8060207c <either_copyout+0x54>
    80602060:	642c                	ld	a1,72(s0)
    80602062:	6828                	ld	a0,80(s0)
    80602064:	7402                	ld	s0,32(sp)
    80602066:	6702                	ld	a4,0(sp)
    80602068:	70a2                	ld	ra,40(sp)
    8060206a:	86a6                	mv	a3,s1
    8060206c:	864a                	mv	a2,s2
    8060206e:	64e2                	ld	s1,24(sp)
    80602070:	6942                	ld	s2,16(sp)
    80602072:	6145                	addi	sp,sp,48
    80602074:	00000317          	auipc	t1,0x0
    80602078:	41030067          	jr	1040(t1) # 80602484 <copyout>
    8060207c:	4602                	lw	a2,0(sp)
    8060207e:	85a6                	mv	a1,s1
    80602080:	854a                	mv	a0,s2
    80602082:	00000097          	auipc	ra,0x0
    80602086:	2aa080e7          	jalr	682(ra) # 8060232c <memmove>
    8060208a:	70a2                	ld	ra,40(sp)
    8060208c:	7402                	ld	s0,32(sp)
    8060208e:	64e2                	ld	s1,24(sp)
    80602090:	6942                	ld	s2,16(sp)
    80602092:	4501                	li	a0,0
    80602094:	6145                	addi	sp,sp,48
    80602096:	8082                	ret

0000000080602098 <either_copyin>:
    80602098:	7179                	addi	sp,sp,-48
    8060209a:	ec26                	sd	s1,24(sp)
    8060209c:	e84a                	sd	s2,16(sp)
    8060209e:	f406                	sd	ra,40(sp)
    806020a0:	f022                	sd	s0,32(sp)
    806020a2:	892a                	mv	s2,a0
    806020a4:	e42e                	sd	a1,8(sp)
    806020a6:	84b2                	mv	s1,a2
    806020a8:	e036                	sd	a3,0(sp)
    806020aa:	00000097          	auipc	ra,0x0
    806020ae:	11e080e7          	jalr	286(ra) # 806021c8 <push_off>
    806020b2:	8792                	mv	a5,tp
    806020b4:	2781                	sext.w	a5,a5
    806020b6:	00016717          	auipc	a4,0x16
    806020ba:	9ba70713          	addi	a4,a4,-1606 # 80617a70 <cpus>
    806020be:	079e                	slli	a5,a5,0x7
    806020c0:	97ba                	add	a5,a5,a4
    806020c2:	6380                	ld	s0,0(a5)
    806020c4:	00000097          	auipc	ra,0x0
    806020c8:	194080e7          	jalr	404(ra) # 80602258 <pop_off>
    806020cc:	65a2                	ld	a1,8(sp)
    806020ce:	cd99                	beqz	a1,806020ec <either_copyin+0x54>
    806020d0:	642c                	ld	a1,72(s0)
    806020d2:	6828                	ld	a0,80(s0)
    806020d4:	7402                	ld	s0,32(sp)
    806020d6:	6702                	ld	a4,0(sp)
    806020d8:	70a2                	ld	ra,40(sp)
    806020da:	86a6                	mv	a3,s1
    806020dc:	864a                	mv	a2,s2
    806020de:	64e2                	ld	s1,24(sp)
    806020e0:	6942                	ld	s2,16(sp)
    806020e2:	6145                	addi	sp,sp,48
    806020e4:	00000317          	auipc	t1,0x0
    806020e8:	38430067          	jr	900(t1) # 80602468 <copyin>
    806020ec:	4602                	lw	a2,0(sp)
    806020ee:	85a6                	mv	a1,s1
    806020f0:	854a                	mv	a0,s2
    806020f2:	00000097          	auipc	ra,0x0
    806020f6:	23a080e7          	jalr	570(ra) # 8060232c <memmove>
    806020fa:	70a2                	ld	ra,40(sp)
    806020fc:	7402                	ld	s0,32(sp)
    806020fe:	64e2                	ld	s1,24(sp)
    80602100:	6942                	ld	s2,16(sp)
    80602102:	4501                	li	a0,0
    80602104:	6145                	addi	sp,sp,48
    80602106:	8082                	ret

0000000080602108 <procdump>:
    80602108:	7179                	addi	sp,sp,-48
    8060210a:	00000517          	auipc	a0,0x0
    8060210e:	40650513          	addi	a0,a0,1030 # 80602510 <kexec+0x5e>
    80602112:	f022                	sd	s0,32(sp)
    80602114:	ec26                	sd	s1,24(sp)
    80602116:	e84a                	sd	s2,16(sp)
    80602118:	e44e                	sd	s3,8(sp)
    8060211a:	f406                	sd	ra,40(sp)
    8060211c:	00016417          	auipc	s0,0x16
    80602120:	edc40413          	addi	s0,s0,-292 # 80617ff8 <proc+0x158>
    80602124:	fffff097          	auipc	ra,0xfffff
    80602128:	dc2080e7          	jalr	-574(ra) # 80600ee6 <printk>
    8060212c:	0001c497          	auipc	s1,0x1c
    80602130:	8cc48493          	addi	s1,s1,-1844 # 8061d9f8 <trampoline+0x158>
    80602134:	4995                	li	s3,5
    80602136:	00001917          	auipc	s2,0x1
    8060213a:	8d290913          	addi	s2,s2,-1838 # 80602a08 <states.0>
    8060213e:	ec042783          	lw	a5,-320(s0)
    80602142:	86a2                	mv	a3,s0
    80602144:	00000517          	auipc	a0,0x0
    80602148:	7a450513          	addi	a0,a0,1956 # 806028e8 <kexec+0x436>
    8060214c:	cb8d                	beqz	a5,8060217e <procdump+0x76>
    8060214e:	00379713          	slli	a4,a5,0x3
    80602152:	974a                	add	a4,a4,s2
    80602154:	00000617          	auipc	a2,0x0
    80602158:	78c60613          	addi	a2,a2,1932 # 806028e0 <kexec+0x42e>
    8060215c:	00f9e363          	bltu	s3,a5,80602162 <procdump+0x5a>
    80602160:	6310                	ld	a2,0(a4)
    80602162:	ed842583          	lw	a1,-296(s0)
    80602166:	fffff097          	auipc	ra,0xfffff
    8060216a:	d80080e7          	jalr	-640(ra) # 80600ee6 <printk>
    8060216e:	00000517          	auipc	a0,0x0
    80602172:	3a250513          	addi	a0,a0,930 # 80602510 <kexec+0x5e>
    80602176:	fffff097          	auipc	ra,0xfffff
    8060217a:	d70080e7          	jalr	-656(ra) # 80600ee6 <printk>
    8060217e:	16840413          	addi	s0,s0,360
    80602182:	fa941ee3          	bne	s0,s1,8060213e <procdump+0x36>
    80602186:	70a2                	ld	ra,40(sp)
    80602188:	7402                	ld	s0,32(sp)
    8060218a:	64e2                	ld	s1,24(sp)
    8060218c:	6942                	ld	s2,16(sp)
    8060218e:	69a2                	ld	s3,8(sp)
    80602190:	6145                	addi	sp,sp,48
    80602192:	8082                	ret

0000000080602194 <initlock>:
    80602194:	00052023          	sw	zero,0(a0)
    80602198:	00053823          	sd	zero,16(a0)
    8060219c:	e50c                	sd	a1,8(a0)
    8060219e:	8082                	ret

00000000806021a0 <holding>:
    806021a0:	411c                	lw	a5,0(a0)
    806021a2:	e399                	bnez	a5,806021a8 <holding+0x8>
    806021a4:	4501                	li	a0,0
    806021a6:	8082                	ret
    806021a8:	691c                	ld	a5,16(a0)
    806021aa:	1101                	addi	sp,sp,-32
    806021ac:	ec06                	sd	ra,24(sp)
    806021ae:	e43e                	sd	a5,8(sp)
    806021b0:	fffff097          	auipc	ra,0xfffff
    806021b4:	2b8080e7          	jalr	696(ra) # 80601468 <mycpu>
    806021b8:	67a2                	ld	a5,8(sp)
    806021ba:	60e2                	ld	ra,24(sp)
    806021bc:	40a78533          	sub	a0,a5,a0
    806021c0:	00153513          	seqz	a0,a0
    806021c4:	6105                	addi	sp,sp,32
    806021c6:	8082                	ret

00000000806021c8 <push_off>:
    806021c8:	1141                	addi	sp,sp,-16
    806021ca:	e406                	sd	ra,8(sp)
    806021cc:	e022                	sd	s0,0(sp)
    806021ce:	100177f3          	csrrci	a5,sstatus,2
    806021d2:	843e                	mv	s0,a5
    806021d4:	fffff097          	auipc	ra,0xfffff
    806021d8:	294080e7          	jalr	660(ra) # 80601468 <mycpu>
    806021dc:	5d3c                	lw	a5,120(a0)
    806021de:	cf81                	beqz	a5,806021f6 <push_off+0x2e>
    806021e0:	fffff097          	auipc	ra,0xfffff
    806021e4:	288080e7          	jalr	648(ra) # 80601468 <mycpu>
    806021e8:	5d3c                	lw	a5,120(a0)
    806021ea:	60a2                	ld	ra,8(sp)
    806021ec:	6402                	ld	s0,0(sp)
    806021ee:	2785                	addiw	a5,a5,1
    806021f0:	dd3c                	sw	a5,120(a0)
    806021f2:	0141                	addi	sp,sp,16
    806021f4:	8082                	ret
    806021f6:	fffff097          	auipc	ra,0xfffff
    806021fa:	272080e7          	jalr	626(ra) # 80601468 <mycpu>
    806021fe:	00145793          	srli	a5,s0,0x1
    80602202:	8b85                	andi	a5,a5,1
    80602204:	dd7c                	sw	a5,124(a0)
    80602206:	bfe9                	j	806021e0 <push_off+0x18>

0000000080602208 <acquire>:
    80602208:	1101                	addi	sp,sp,-32
    8060220a:	e822                	sd	s0,16(sp)
    8060220c:	ec06                	sd	ra,24(sp)
    8060220e:	842a                	mv	s0,a0
    80602210:	00000097          	auipc	ra,0x0
    80602214:	fb8080e7          	jalr	-72(ra) # 806021c8 <push_off>
    80602218:	401c                	lw	a5,0(s0)
    8060221a:	ef91                	bnez	a5,80602236 <acquire+0x2e>
    8060221c:	4705                	li	a4,1
    8060221e:	0ce427af          	amoswap.w.aq	a5,a4,(s0)
    80602222:	fff5                	bnez	a5,8060221e <acquire+0x16>
    80602224:	fffff097          	auipc	ra,0xfffff
    80602228:	244080e7          	jalr	580(ra) # 80601468 <mycpu>
    8060222c:	60e2                	ld	ra,24(sp)
    8060222e:	e808                	sd	a0,16(s0)
    80602230:	6442                	ld	s0,16(sp)
    80602232:	6105                	addi	sp,sp,32
    80602234:	8082                	ret
    80602236:	681c                	ld	a5,16(s0)
    80602238:	e43e                	sd	a5,8(sp)
    8060223a:	fffff097          	auipc	ra,0xfffff
    8060223e:	22e080e7          	jalr	558(ra) # 80601468 <mycpu>
    80602242:	67a2                	ld	a5,8(sp)
    80602244:	fca79ce3          	bne	a5,a0,8060221c <acquire+0x14>
    80602248:	00000517          	auipc	a0,0x0
    8060224c:	6e050513          	addi	a0,a0,1760 # 80602928 <kexec+0x476>
    80602250:	fffff097          	auipc	ra,0xfffff
    80602254:	f46080e7          	jalr	-186(ra) # 80601196 <panic>

0000000080602258 <pop_off>:
    80602258:	1141                	addi	sp,sp,-16
    8060225a:	e406                	sd	ra,8(sp)
    8060225c:	fffff097          	auipc	ra,0xfffff
    80602260:	20c080e7          	jalr	524(ra) # 80601468 <mycpu>
    80602264:	100027f3          	csrr	a5,sstatus
    80602268:	8b89                	andi	a5,a5,2
    8060226a:	e795                	bnez	a5,80602296 <pop_off+0x3e>
    8060226c:	5d3c                	lw	a5,120(a0)
    8060226e:	00f05c63          	blez	a5,80602286 <pop_off+0x2e>
    80602272:	37fd                	addiw	a5,a5,-1
    80602274:	dd3c                	sw	a5,120(a0)
    80602276:	e789                	bnez	a5,80602280 <pop_off+0x28>
    80602278:	5d7c                	lw	a5,124(a0)
    8060227a:	c399                	beqz	a5,80602280 <pop_off+0x28>
    8060227c:	10016073          	csrsi	sstatus,2
    80602280:	60a2                	ld	ra,8(sp)
    80602282:	0141                	addi	sp,sp,16
    80602284:	8082                	ret
    80602286:	00000517          	auipc	a0,0x0
    8060228a:	6c250513          	addi	a0,a0,1730 # 80602948 <kexec+0x496>
    8060228e:	fffff097          	auipc	ra,0xfffff
    80602292:	f08080e7          	jalr	-248(ra) # 80601196 <panic>
    80602296:	00000517          	auipc	a0,0x0
    8060229a:	69a50513          	addi	a0,a0,1690 # 80602930 <kexec+0x47e>
    8060229e:	fffff097          	auipc	ra,0xfffff
    806022a2:	ef8080e7          	jalr	-264(ra) # 80601196 <panic>

00000000806022a6 <release>:
    806022a6:	4118                	lw	a4,0(a0)
    806022a8:	1101                	addi	sp,sp,-32
    806022aa:	ec06                	sd	ra,24(sp)
    806022ac:	eb09                	bnez	a4,806022be <release+0x18>
    806022ae:	00000517          	auipc	a0,0x0
    806022b2:	6a250513          	addi	a0,a0,1698 # 80602950 <kexec+0x49e>
    806022b6:	fffff097          	auipc	ra,0xfffff
    806022ba:	ee0080e7          	jalr	-288(ra) # 80601196 <panic>
    806022be:	6918                	ld	a4,16(a0)
    806022c0:	e42a                	sd	a0,8(sp)
    806022c2:	e03a                	sd	a4,0(sp)
    806022c4:	fffff097          	auipc	ra,0xfffff
    806022c8:	1a4080e7          	jalr	420(ra) # 80601468 <mycpu>
    806022cc:	6702                	ld	a4,0(sp)
    806022ce:	67a2                	ld	a5,8(sp)
    806022d0:	fca71fe3          	bne	a4,a0,806022ae <release+0x8>
    806022d4:	0007b823          	sd	zero,16(a5)
    806022d8:	0310000f          	fence	rw,w
    806022dc:	0007a023          	sw	zero,0(a5)
    806022e0:	60e2                	ld	ra,24(sp)
    806022e2:	6105                	addi	sp,sp,32
    806022e4:	00000317          	auipc	t1,0x0
    806022e8:	f7430067          	jr	-140(t1) # 80602258 <pop_off>

00000000806022ec <memset>:
    806022ec:	ca11                	beqz	a2,80602300 <memset+0x14>
    806022ee:	1602                	slli	a2,a2,0x20
    806022f0:	9201                	srli	a2,a2,0x20
    806022f2:	87aa                	mv	a5,a0
    806022f4:	962a                	add	a2,a2,a0
    806022f6:	00b78023          	sb	a1,0(a5)
    806022fa:	0785                	addi	a5,a5,1
    806022fc:	fec79de3          	bne	a5,a2,806022f6 <memset+0xa>
    80602300:	8082                	ret

0000000080602302 <memcmp>:
    80602302:	02061693          	slli	a3,a2,0x20
    80602306:	9281                	srli	a3,a3,0x20
    80602308:	96aa                	add	a3,a3,a0
    8060230a:	e601                	bnez	a2,80602312 <memcmp+0x10>
    8060230c:	a831                	j	80602328 <memcmp+0x26>
    8060230e:	00a68d63          	beq	a3,a0,80602328 <memcmp+0x26>
    80602312:	00054783          	lbu	a5,0(a0)
    80602316:	0005c703          	lbu	a4,0(a1)
    8060231a:	0505                	addi	a0,a0,1
    8060231c:	0585                	addi	a1,a1,1
    8060231e:	fee788e3          	beq	a5,a4,8060230e <memcmp+0xc>
    80602322:	40e7853b          	subw	a0,a5,a4
    80602326:	8082                	ret
    80602328:	4501                	li	a0,0
    8060232a:	8082                	ret

000000008060232c <memmove>:
    8060232c:	c629                	beqz	a2,80602376 <memmove+0x4a>
    8060232e:	02061713          	slli	a4,a2,0x20
    80602332:	9301                	srli	a4,a4,0x20
    80602334:	00a5ed63          	bltu	a1,a0,8060234e <memmove+0x22>
    80602338:	972e                	add	a4,a4,a1
    8060233a:	87aa                	mv	a5,a0
    8060233c:	0005c683          	lbu	a3,0(a1)
    80602340:	0585                	addi	a1,a1,1
    80602342:	0785                	addi	a5,a5,1
    80602344:	fed78fa3          	sb	a3,-1(a5)
    80602348:	feb71ae3          	bne	a4,a1,8060233c <memmove+0x10>
    8060234c:	8082                	ret
    8060234e:	00e587b3          	add	a5,a1,a4
    80602352:	fef573e3          	bgeu	a0,a5,80602338 <memmove+0xc>
    80602356:	fff6069b          	addiw	a3,a2,-1
    8060235a:	1682                	slli	a3,a3,0x20
    8060235c:	9281                	srli	a3,a3,0x20
    8060235e:	fff6c693          	not	a3,a3
    80602362:	972a                	add	a4,a4,a0
    80602364:	96be                	add	a3,a3,a5
    80602366:	fff7c603          	lbu	a2,-1(a5)
    8060236a:	17fd                	addi	a5,a5,-1
    8060236c:	177d                	addi	a4,a4,-1
    8060236e:	00c70023          	sb	a2,0(a4)
    80602372:	fef69ae3          	bne	a3,a5,80602366 <memmove+0x3a>
    80602376:	8082                	ret

0000000080602378 <memcpy>:
    80602378:	00000317          	auipc	t1,0x0
    8060237c:	fb430067          	jr	-76(t1) # 8060232c <memmove>

0000000080602380 <strncmp>:
    80602380:	ce01                	beqz	a2,80602398 <strncmp+0x18>
    80602382:	00054783          	lbu	a5,0(a0)
    80602386:	367d                	addiw	a2,a2,-1
    80602388:	cb91                	beqz	a5,8060239c <strncmp+0x1c>
    8060238a:	0005c703          	lbu	a4,0(a1)
    8060238e:	00f71763          	bne	a4,a5,8060239c <strncmp+0x1c>
    80602392:	0505                	addi	a0,a0,1
    80602394:	0585                	addi	a1,a1,1
    80602396:	f675                	bnez	a2,80602382 <strncmp+0x2>
    80602398:	4501                	li	a0,0
    8060239a:	8082                	ret
    8060239c:	00054503          	lbu	a0,0(a0)
    806023a0:	0005c783          	lbu	a5,0(a1)
    806023a4:	9d1d                	subw	a0,a0,a5
    806023a6:	8082                	ret

00000000806023a8 <strncpy>:
    806023a8:	87aa                	mv	a5,a0
    806023aa:	a811                	j	806023be <strncpy+0x16>
    806023ac:	0005c703          	lbu	a4,0(a1)
    806023b0:	fff6069b          	addiw	a3,a2,-1
    806023b4:	0585                	addi	a1,a1,1
    806023b6:	fee78fa3          	sb	a4,-1(a5)
    806023ba:	c711                	beqz	a4,806023c6 <strncpy+0x1e>
    806023bc:	8636                	mv	a2,a3
    806023be:	0785                	addi	a5,a5,1
    806023c0:	fec046e3          	bgtz	a2,806023ac <strncpy+0x4>
    806023c4:	8082                	ret
    806023c6:	9e3d                	addw	a2,a2,a5
    806023c8:	367d                	addiw	a2,a2,-1
    806023ca:	deed                	beqz	a3,806023c4 <strncpy+0x1c>
    806023cc:	0785                	addi	a5,a5,1
    806023ce:	40f6073b          	subw	a4,a2,a5
    806023d2:	fe078fa3          	sb	zero,-1(a5)
    806023d6:	fee04be3          	bgtz	a4,806023cc <strncpy+0x24>
    806023da:	8082                	ret

00000000806023dc <safestrcpy>:
    806023dc:	02c05063          	blez	a2,806023fc <safestrcpy+0x20>
    806023e0:	367d                	addiw	a2,a2,-1
    806023e2:	962e                	add	a2,a2,a1
    806023e4:	87aa                	mv	a5,a0
    806023e6:	00c58963          	beq	a1,a2,806023f8 <safestrcpy+0x1c>
    806023ea:	0005c703          	lbu	a4,0(a1)
    806023ee:	0585                	addi	a1,a1,1
    806023f0:	0785                	addi	a5,a5,1
    806023f2:	fee78fa3          	sb	a4,-1(a5)
    806023f6:	fb65                	bnez	a4,806023e6 <safestrcpy+0xa>
    806023f8:	00078023          	sb	zero,0(a5)
    806023fc:	8082                	ret

00000000806023fe <strlen>:
    806023fe:	00054783          	lbu	a5,0(a0)
    80602402:	cb99                	beqz	a5,80602418 <strlen+0x1a>
    80602404:	00150793          	addi	a5,a0,1
    80602408:	86be                	mv	a3,a5
    8060240a:	0007c703          	lbu	a4,0(a5)
    8060240e:	0785                	addi	a5,a5,1
    80602410:	ff65                	bnez	a4,80602408 <strlen+0xa>
    80602412:	40a6853b          	subw	a0,a3,a0
    80602416:	8082                	ret
    80602418:	4501                	li	a0,0
    8060241a:	8082                	ret

000000008060241c <kalloc>:
    8060241c:	ffffe317          	auipc	t1,0xffffe
    80602420:	11630067          	jr	278(t1) # 80600532 <pmm_alloc_page>

0000000080602424 <kfree>:
    80602424:	c509                	beqz	a0,8060242e <kfree+0xa>
    80602426:	ffffe317          	auipc	t1,0xffffe
    8060242a:	1a030067          	jr	416(t1) # 806005c6 <pmm_free_page>
    8060242e:	8082                	ret

0000000080602430 <consputc>:
    80602430:	0ff57513          	zext.b	a0,a0
    80602434:	fffff317          	auipc	t1,0xfffff
    80602438:	8fe30067          	jr	-1794(t1) # 80600d32 <uart_putc>

000000008060243c <kvmmap>:
    8060243c:	8082                	ret

000000008060243e <mappages>:
    8060243e:	4501                	li	a0,0
    80602440:	8082                	ret

0000000080602442 <userret>:
    80602442:	a001                	j	80602442 <userret>

0000000080602444 <prepare_return>:
    80602444:	8082                	ret

0000000080602446 <uvmcreate>:
    80602446:	ffffe317          	auipc	t1,0xffffe
    8060244a:	0ec30067          	jr	236(t1) # 80600532 <pmm_alloc_page>

000000008060244e <uvmfree>:
    8060244e:	c509                	beqz	a0,80602458 <uvmfree+0xa>
    80602450:	ffffe317          	auipc	t1,0xffffe
    80602454:	17630067          	jr	374(t1) # 806005c6 <pmm_free_page>
    80602458:	8082                	ret

000000008060245a <uvmcopy>:
    8060245a:	4501                	li	a0,0
    8060245c:	8082                	ret

000000008060245e <uvmunmap>:
    8060245e:	8082                	ret

0000000080602460 <uvmalloc>:
    80602460:	8532                	mv	a0,a2
    80602462:	8082                	ret

0000000080602464 <uvmdealloc>:
    80602464:	8532                	mv	a0,a2
    80602466:	8082                	ret

0000000080602468 <copyin>:
    80602468:	1141                	addi	sp,sp,-16
    8060246a:	852e                	mv	a0,a1
    8060246c:	85b2                	mv	a1,a2
    8060246e:	0006861b          	sext.w	a2,a3
    80602472:	e406                	sd	ra,8(sp)
    80602474:	00000097          	auipc	ra,0x0
    80602478:	eb8080e7          	jalr	-328(ra) # 8060232c <memmove>
    8060247c:	60a2                	ld	ra,8(sp)
    8060247e:	4501                	li	a0,0
    80602480:	0141                	addi	sp,sp,16
    80602482:	8082                	ret

0000000080602484 <copyout>:
    80602484:	1141                	addi	sp,sp,-16
    80602486:	852e                	mv	a0,a1
    80602488:	85b2                	mv	a1,a2
    8060248a:	0006861b          	sext.w	a2,a3
    8060248e:	e406                	sd	ra,8(sp)
    80602490:	00000097          	auipc	ra,0x0
    80602494:	e9c080e7          	jalr	-356(ra) # 8060232c <memmove>
    80602498:	60a2                	ld	ra,8(sp)
    8060249a:	4501                	li	a0,0
    8060249c:	0141                	addi	sp,sp,16
    8060249e:	8082                	ret

00000000806024a0 <begin_op>:
    806024a0:	8082                	ret

00000000806024a2 <end_op>:
    806024a2:	8082                	ret

00000000806024a4 <fsinit>:
    806024a4:	8082                	ret

00000000806024a6 <namei>:
    806024a6:	4501                	li	a0,0
    806024a8:	8082                	ret

00000000806024aa <iput>:
    806024aa:	8082                	ret

00000000806024ac <idup>:
    806024ac:	8082                	ret

00000000806024ae <fileclose>:
    806024ae:	8082                	ret

00000000806024b0 <filedup>:
    806024b0:	8082                	ret

00000000806024b2 <kexec>:
    806024b2:	557d                	li	a0,-1
    806024b4:	8082                	ret
