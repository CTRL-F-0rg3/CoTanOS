
ZFC-kernel-init/build/CoTanOS.elf:     file format elf64-littleriscv


Disassembly of section .text.init:

0000000080600000 <_kernel_start>:
    80600000:	10401073          	csrw	sie,zero
    80600004:	842a                	mv	s0,a0
    80600006:	00003297          	auipc	t0,0x3
    8060000a:	27228293          	addi	t0,t0,626 # 80603278 <g_boot_elect>
    8060000e:	4305                	li	t1,1
    80600010:	0c62a3af          	amoswap.w.aq	t2,t1,(t0)
    80600014:	04039863          	bnez	t2,80600064 <_kernel_start+0x64>
    80600018:	00004297          	auipc	t0,0x4
    8060001c:	fe828293          	addi	t0,t0,-24 # 80604000 <__bss_start>
    80600020:	00028317          	auipc	t1,0x28
    80600024:	8a030313          	addi	t1,t1,-1888 # 806278c0 <__bss_end>
    80600028:	0062f663          	bgeu	t0,t1,80600034 <_kernel_start+0x34>
    8060002c:	0002b023          	sd	zero,0(t0)
    80600030:	02a1                	addi	t0,t0,8
    80600032:	bfdd                	j	80600028 <_kernel_start+0x28>
    80600034:	0002c117          	auipc	sp,0x2c
    80600038:	88c10113          	addi	sp,sp,-1908 # 8062b8c0 <__hart_stacks_start>
    8060003c:	4501                	li	a0,0
    8060003e:	4581                	li	a1,0
    80600040:	00000097          	auipc	ra,0x0
    80600044:	300080e7          	jalr	768(ra) # 80600340 <cotan_global_init>
    80600048:	00004297          	auipc	t0,0x4
    8060004c:	fb828293          	addi	t0,t0,-72 # 80604000 <__bss_start>
    80600050:	0282b023          	sd	s0,32(t0)
    80600054:	8522                	mv	a0,s0
    80600056:	00001097          	auipc	ra,0x1
    8060005a:	894080e7          	jalr	-1900(ra) # 806008ea <kernel_main>
    8060005e:	10500073          	wfi
    80600062:	bff5                	j	8060005e <_kernel_start+0x5e>
    80600064:	00003297          	auipc	t0,0x3
    80600068:	21828293          	addi	t0,t0,536 # 8060327c <g_init_done>
    8060006c:	0002a303          	lw	t1,0(t0)
    80600070:	fe030ee3          	beqz	t1,8060006c <_kernel_start+0x6c>
    80600074:	0230000f          	fence	r,rw
    80600078:	0002c297          	auipc	t0,0x2c
    8060007c:	84828293          	addi	t0,t0,-1976 # 8062b8c0 <__hart_stacks_start>
    80600080:	6305                	lui	t1,0x1
    80600082:	00140393          	addi	t2,s0,1
    80600086:	027303b3          	mul	t2,t1,t2
    8060008a:	00728133          	add	sp,t0,t2
    8060008e:	8522                	mv	a0,s0
    80600090:	00000097          	auipc	ra,0x0
    80600094:	7ca080e7          	jalr	1994(ra) # 8060085a <secondary_hart_init>
    80600098:	b7d9                	j	8060005e <_kernel_start+0x5e>

Disassembly of section .text.sched:

000000008060009a <sched_init>:
    8060009a:	82aa                	mv	t0,a0
    8060009c:	02029663          	bnez	t0,806000c8 <sched_init+0x2e>
    806000a0:	00008317          	auipc	t1,0x8
    806000a4:	f7830313          	addi	t1,t1,-136 # 80608018 <g_sched>
    806000a8:	02533423          	sd	t0,40(t1)
    806000ac:	00033423          	sd	zero,8(t1)
    806000b0:	00033823          	sd	zero,16(t1)
    806000b4:	00033c23          	sd	zero,24(t1)
    806000b8:	00033023          	sd	zero,0(t1)
    806000bc:	02033c23          	sd	zero,56(t1)
    806000c0:	4385                	li	t2,1
    806000c2:	02733823          	sd	t2,48(t1)
    806000c6:	8082                	ret
    806000c8:	00000317          	auipc	t1,0x0
    806000cc:	26430067          	jr	612(t1) # 8060032c <hart_park>

00000000806000d0 <sched_create>:
    806000d0:	82aa                	mv	t0,a0
    806000d2:	00008317          	auipc	t1,0x8
    806000d6:	f8630313          	addi	t1,t1,-122 # 80608058 <g_tasks>
    806000da:	00008397          	auipc	t2,0x8
    806000de:	f3e38393          	addi	t2,t2,-194 # 80608018 <g_sched>
    806000e2:	0303be03          	ld	t3,48(t2)
    806000e6:	4ec1                	li	t4,16
    806000e8:	4f01                	li	t5,0
    806000ea:	07df0563          	beq	t5,t4,80600154 <sched_create+0x84>
    806000ee:	0a000593          	li	a1,160
    806000f2:	02be05b3          	mul	a1,t3,a1
    806000f6:	959a                	add	a1,a1,t1
    806000f8:	7db0                	ld	a2,120(a1)
    806000fa:	c619                	beqz	a2,80600108 <sched_create+0x38>
    806000fc:	0e05                	addi	t3,t3,1
    806000fe:	01de4363          	blt	t3,t4,80600104 <sched_create+0x34>
    80600102:	4e01                	li	t3,0
    80600104:	0f05                	addi	t5,t5,1
    80600106:	b7d5                	j	806000ea <sched_create+0x1a>
    80600108:	4601                	li	a2,0
    8060010a:	46d1                	li	a3,20
    8060010c:	872e                	mv	a4,a1
    8060010e:	e310                	sd	a2,0(a4)
    80600110:	0721                	addi	a4,a4,8
    80600112:	16fd                	addi	a3,a3,-1
    80600114:	feed                	bnez	a3,8060010e <sched_create+0x3e>
    80600116:	07c5b823          	sd	t3,112(a1)
    8060011a:	4601                	li	a2,0
    8060011c:	fdb0                	sd	a2,120(a1)
    8060011e:	0855b023          	sd	t0,128(a1)
    80600122:	6605                	lui	a2,0x1
    80600124:	02ce0633          	mul	a2,t3,a2
    80600128:	00009697          	auipc	a3,0x9
    8060012c:	93068693          	addi	a3,a3,-1744 # 80608a58 <g_stacks>
    80600130:	96b2                	add	a3,a3,a2
    80600132:	e5d4                	sd	a3,136(a1)
    80600134:	6705                	lui	a4,0x1
    80600136:	96ba                	add	a3,a3,a4
    80600138:	e594                	sd	a3,8(a1)
    8060013a:	00000717          	auipc	a4,0x0
    8060013e:	0f870713          	addi	a4,a4,248 # 80600232 <task_trampoline>
    80600142:	e198                	sd	a4,0(a1)
    80600144:	0e05                	addi	t3,t3,1
    80600146:	01de4363          	blt	t3,t4,8060014c <sched_create+0x7c>
    8060014a:	4e01                	li	t3,0
    8060014c:	03c3b823          	sd	t3,48(t2)
    80600150:	852e                	mv	a0,a1
    80600152:	8082                	ret
    80600154:	4501                	li	a0,0
    80600156:	8082                	ret

0000000080600158 <sched_enqueue>:
    80600158:	08053c23          	sd	zero,152(a0)
    8060015c:	4285                	li	t0,1
    8060015e:	06553c23          	sd	t0,120(a0)
    80600162:	00008297          	auipc	t0,0x8
    80600166:	ebe28293          	addi	t0,t0,-322 # 80608020 <g_sched+0x8>
    8060016a:	0082b303          	ld	t1,8(t0)
    8060016e:	00030763          	beqz	t1,8060017c <sched_enqueue+0x24>
    80600172:	08a33c23          	sd	a0,152(t1)
    80600176:	00a2b423          	sd	a0,8(t0)
    8060017a:	a029                	j	80600184 <sched_enqueue+0x2c>
    8060017c:	00a2b023          	sd	a0,0(t0)
    80600180:	00a2b423          	sd	a0,8(t0)
    80600184:	0102b303          	ld	t1,16(t0)
    80600188:	0305                	addi	t1,t1,1
    8060018a:	0062b823          	sd	t1,16(t0)
    8060018e:	8082                	ret

0000000080600190 <sched_next>:
    80600190:	00008297          	auipc	t0,0x8
    80600194:	e9028293          	addi	t0,t0,-368 # 80608020 <g_sched+0x8>
    80600198:	0002b303          	ld	t1,0(t0)
    8060019c:	02030363          	beqz	t1,806001c2 <sched_next+0x32>
    806001a0:	09833383          	ld	t2,152(t1)
    806001a4:	0072b023          	sd	t2,0(t0)
    806001a8:	00039463          	bnez	t2,806001b0 <sched_next+0x20>
    806001ac:	0002b423          	sd	zero,8(t0)
    806001b0:	0102be03          	ld	t3,16(t0)
    806001b4:	1e7d                	addi	t3,t3,-1
    806001b6:	01c2b823          	sd	t3,16(t0)
    806001ba:	08033c23          	sd	zero,152(t1)
    806001be:	851a                	mv	a0,t1
    806001c0:	8082                	ret
    806001c2:	4501                	li	a0,0
    806001c4:	8082                	ret

00000000806001c6 <context_switch>:
    806001c6:	c91d                	beqz	a0,806001fc <context_switch+0x36>
    806001c8:	00153023          	sd	ra,0(a0)
    806001cc:	00253423          	sd	sp,8(a0)
    806001d0:	e900                	sd	s0,16(a0)
    806001d2:	ed04                	sd	s1,24(a0)
    806001d4:	03253023          	sd	s2,32(a0)
    806001d8:	03353423          	sd	s3,40(a0)
    806001dc:	03453823          	sd	s4,48(a0)
    806001e0:	03553c23          	sd	s5,56(a0)
    806001e4:	05653023          	sd	s6,64(a0)
    806001e8:	05753423          	sd	s7,72(a0)
    806001ec:	05853823          	sd	s8,80(a0)
    806001f0:	05953c23          	sd	s9,88(a0)
    806001f4:	07a53023          	sd	s10,96(a0)
    806001f8:	07b53423          	sd	s11,104(a0)
    806001fc:	0005b083          	ld	ra,0(a1)
    80600200:	0085b103          	ld	sp,8(a1)
    80600204:	6980                	ld	s0,16(a1)
    80600206:	6d84                	ld	s1,24(a1)
    80600208:	0205b903          	ld	s2,32(a1)
    8060020c:	0285b983          	ld	s3,40(a1)
    80600210:	0305ba03          	ld	s4,48(a1)
    80600214:	0385ba83          	ld	s5,56(a1)
    80600218:	0405bb03          	ld	s6,64(a1)
    8060021c:	0485bb83          	ld	s7,72(a1)
    80600220:	0505bc03          	ld	s8,80(a1)
    80600224:	0585bc83          	ld	s9,88(a1)
    80600228:	0605bd03          	ld	s10,96(a1)
    8060022c:	0685bd83          	ld	s11,104(a1)
    80600230:	8082                	ret

0000000080600232 <task_trampoline>:
    80600232:	00008297          	auipc	t0,0x8
    80600236:	de628293          	addi	t0,t0,-538 # 80608018 <g_sched>
    8060023a:	0002b283          	ld	t0,0(t0)
    8060023e:	0802b303          	ld	t1,128(t0)
    80600242:	9302                	jalr	t1
    80600244:	00008297          	auipc	t0,0x8
    80600248:	dd428293          	addi	t0,t0,-556 # 80608018 <g_sched>
    8060024c:	0002b283          	ld	t0,0(t0)
    80600250:	4311                	li	t1,4
    80600252:	0662bc23          	sd	t1,120(t0)
    80600256:	00000097          	auipc	ra,0x0
    8060025a:	00a080e7          	jalr	10(ra) # 80600260 <sched_yield>
    8060025e:	bfe5                	j	80600256 <task_trampoline+0x24>

0000000080600260 <sched_yield>:
    80600260:	1101                	addi	sp,sp,-32
    80600262:	e006                	sd	ra,0(sp)
    80600264:	e422                	sd	s0,8(sp)
    80600266:	e826                	sd	s1,16(sp)
    80600268:	00008417          	auipc	s0,0x8
    8060026c:	db040413          	addi	s0,s0,-592 # 80608018 <g_sched>
    80600270:	6004                	ld	s1,0(s0)
    80600272:	cc91                	beqz	s1,8060028e <sched_yield+0x2e>
    80600274:	0784b283          	ld	t0,120(s1)
    80600278:	4311                	li	t1,4
    8060027a:	00628a63          	beq	t0,t1,8060028e <sched_yield+0x2e>
    8060027e:	4285                	li	t0,1
    80600280:	0654bc23          	sd	t0,120(s1)
    80600284:	8526                	mv	a0,s1
    80600286:	00000097          	auipc	ra,0x0
    8060028a:	ed2080e7          	jalr	-302(ra) # 80600158 <sched_enqueue>
    8060028e:	00000097          	auipc	ra,0x0
    80600292:	f02080e7          	jalr	-254(ra) # 80600190 <sched_next>
    80600296:	c10d                	beqz	a0,806002b8 <sched_yield+0x58>
    80600298:	00008297          	auipc	t0,0x8
    8060029c:	d8028293          	addi	t0,t0,-640 # 80608018 <g_sched>
    806002a0:	00a2b023          	sd	a0,0(t0)
    806002a4:	4309                	li	t1,2
    806002a6:	06653c23          	sd	t1,120(a0)
    806002aa:	85aa                	mv	a1,a0
    806002ac:	8526                	mv	a0,s1
    806002ae:	00000097          	auipc	ra,0x0
    806002b2:	f18080e7          	jalr	-232(ra) # 806001c6 <context_switch>
    806002b6:	a025                	j	806002de <sched_yield+0x7e>
    806002b8:	00043283          	ld	t0,0(s0)
    806002bc:	00028863          	beqz	t0,806002cc <sched_yield+0x6c>
    806002c0:	0782b303          	ld	t1,120(t0)
    806002c4:	4391                	li	t2,4
    806002c6:	00730363          	beq	t1,t2,806002cc <sched_yield+0x6c>
    806002ca:	a811                	j	806002de <sched_yield+0x7e>
    806002cc:	00008297          	auipc	t0,0x8
    806002d0:	d4c28293          	addi	t0,t0,-692 # 80608018 <g_sched>
    806002d4:	0002b023          	sd	zero,0(t0)
    806002d8:	10500073          	wfi
    806002dc:	bff5                	j	806002d8 <sched_yield+0x78>
    806002de:	6082                	ld	ra,0(sp)
    806002e0:	6422                	ld	s0,8(sp)
    806002e2:	64c2                	ld	s1,16(sp)
    806002e4:	6105                	addi	sp,sp,32
    806002e6:	8082                	ret

00000000806002e8 <sched_start>:
    806002e8:	00008297          	auipc	t0,0x8
    806002ec:	d3028293          	addi	t0,t0,-720 # 80608018 <g_sched>
    806002f0:	4305                	li	t1,1
    806002f2:	0262bc23          	sd	t1,56(t0)
    806002f6:	00000097          	auipc	ra,0x0
    806002fa:	f6a080e7          	jalr	-150(ra) # 80600260 <sched_yield>
    806002fe:	bfe5                	j	806002f6 <sched_start+0xe>

0000000080600300 <sched_tick>:
    80600300:	00008297          	auipc	t0,0x8
    80600304:	d1828293          	addi	t0,t0,-744 # 80608018 <g_sched>
    80600308:	0382b303          	ld	t1,56(t0)
    8060030c:	00030f63          	beqz	t1,8060032a <sched_tick+0x2a>
    80600310:	0002b303          	ld	t1,0(t0)
    80600314:	00030763          	beqz	t1,80600322 <sched_tick+0x22>
    80600318:	09033383          	ld	t2,144(t1)
    8060031c:	0385                	addi	t2,t2,1
    8060031e:	08733823          	sd	t2,144(t1)
    80600322:	00000317          	auipc	t1,0x0
    80600326:	f3e30067          	jr	-194(t1) # 80600260 <sched_yield>
    8060032a:	8082                	ret

000000008060032c <hart_park>:
    8060032c:	10401073          	csrw	sie,zero
    80600330:	14401073          	csrw	sip,zero
    80600334:	10017073          	csrci	sstatus,2
    80600338:	10500073          	wfi
    8060033c:	bff5                	j	80600338 <hart_park+0xc>

Disassembly of section .text:

0000000080600340 <cotan_global_init>:
    80600340:	e539                	bnez	a0,8060038e <cotan_global_init+0x4e>
    80600342:	00004297          	auipc	t0,0x4
    80600346:	cbe28293          	addi	t0,t0,-834 # 80604000 <__bss_start>
    8060034a:	021b8337          	lui	t1,0x21b8
    8060034e:	aa33031b          	addiw	t1,t1,-1373 # 21b7aa3 <MEGAPAGE_SIZE+0x1fb7aa3>
    80600352:	0332                	slli	t1,t1,0xc
    80600354:	0b730313          	addi	t1,t1,183
    80600358:	0336                	slli	t1,t1,0xd
    8060035a:	4f530313          	addi	t1,t1,1269
    8060035e:	0332                	slli	t1,t1,0xc
    80600360:	33130313          	addi	t1,t1,817
    80600364:	0062b023          	sd	t1,0(t0)
    80600368:	0010031b          	addiw	t1,zero,1
    8060036c:	1302                	slli	t1,t1,0x20
    8060036e:	0062b423          	sd	t1,8(t0)
    80600372:	02a2b023          	sd	a0,32(t0)
    80600376:	0ab2b023          	sd	a1,160(t0)
    8060037a:	4305                	li	t1,1
    8060037c:	0262b423          	sd	t1,40(t0)
    80600380:	0002b823          	sd	zero,16(t0)
    80600384:	0002bc23          	sd	zero,24(t0)
    80600388:	8216                	mv	tp,t0
    8060038a:	4501                	li	a0,0
    8060038c:	8082                	ret
    8060038e:	557d                	li	a0,-1
    80600390:	8082                	ret

0000000080600392 <cotan_panic_halt>:
    80600392:	10a23c23          	sd	a0,280(tp) # 118 <COTAN_OFF_PANIC_PHASE>
    80600396:	141022f3          	csrr	t0,sepc
    8060039a:	10523423          	sd	t0,264(tp) # 108 <COTAN_OFF_PANIC_RA>
    8060039e:	c00022f3          	rdcycle	t0
    806003a2:	0c523023          	sd	t0,192(tp) # c0 <COTAN_OFF_HART_DATA>
    806003a6:	c19d                	beqz	a1,806003cc <cotan_panic_halt+0x3a>
    806003a8:	12020293          	addi	t0,tp,288 # 120 <COTAN_OFF_PANIC_MSG>
    806003ac:	832e                	mv	t1,a1
    806003ae:	03f00393          	li	t2,63
    806003b2:	00034e03          	lbu	t3,0(t1)
    806003b6:	01c28023          	sb	t3,0(t0)
    806003ba:	000e0963          	beqz	t3,806003cc <cotan_panic_halt+0x3a>
    806003be:	0285                	addi	t0,t0,1
    806003c0:	0305                	addi	t1,t1,1
    806003c2:	13fd                	addi	t2,t2,-1
    806003c4:	fe0397e3          	bnez	t2,806003b2 <cotan_panic_halt+0x20>
    806003c8:	00028023          	sb	zero,0(t0)
    806003cc:	01823283          	ld	t0,24(tp) # 18 <COTAN_OFF_FLAGS>
    806003d0:	4305                	li	t1,1
    806003d2:	0062e2b3          	or	t0,t0,t1
    806003d6:	00523c23          	sd	t0,24(tp) # 18 <COTAN_OFF_FLAGS>
    806003da:	10500073          	wfi
    806003de:	bff5                	j	806003da <cotan_panic_halt+0x48>

00000000806003e0 <mm_init>:
    806003e0:	00005297          	auipc	t0,0x5
    806003e4:	c2028293          	addi	t0,t0,-992 # 80605000 <g_root_pt>
    806003e8:	00006317          	auipc	t1,0x6
    806003ec:	c1830313          	addi	t1,t1,-1000 # 80606000 <g_l1_dram>
    806003f0:	4381                	li	t2,0
    806003f2:	40000e13          	li	t3,1024
    806003f6:	8e96                	mv	t4,t0
    806003f8:	007eb023          	sd	t2,0(t4)
    806003fc:	0ea1                	addi	t4,t4,8
    806003fe:	1e7d                	addi	t3,t3,-1
    80600400:	fe0e1ce3          	bnez	t3,806003f8 <mm_init+0x18>
    80600404:	0c700e13          	li	t3,199
    80600408:	01c2b023          	sd	t3,0(t0)
    8060040c:	00c35e13          	srli	t3,t1,0xc
    80600410:	0e2a                	slli	t3,t3,0xa
    80600412:	001e6e13          	ori	t3,t3,1
    80600416:	01c2b823          	sd	t3,16(t0)
    8060041a:	8082                	ret

000000008060041c <mm_map_megapage_asm_old>:
    8060041c:	7139                	addi	sp,sp,-64
    8060041e:	e006                	sd	ra,0(sp)
    80600420:	e422                	sd	s0,8(sp)
    80600422:	e826                	sd	s1,16(sp)
    80600424:	ec4a                	sd	s2,24(sp)
    80600426:	f04e                	sd	s3,32(sp)
    80600428:	f452                	sd	s4,40(sp)
    8060042a:	842a                	mv	s0,a0
    8060042c:	84ae                	mv	s1,a1
    8060042e:	8932                	mv	s2,a2
    80600430:	00005997          	auipc	s3,0x5
    80600434:	bd098993          	addi	s3,s3,-1072 # 80605000 <g_root_pt>
    80600438:	4a09                	li	s4,2
    8060043a:	42a5                	li	t0,9
    8060043c:	034282b3          	mul	t0,t0,s4
    80600440:	02b1                	addi	t0,t0,12
    80600442:	00545333          	srl	t1,s0,t0
    80600446:	1ff37313          	andi	t1,t1,511
    8060044a:	00331393          	slli	t2,t1,0x3
    8060044e:	93ce                	add	t2,t2,s3
    80600450:	0003be03          	ld	t3,0(t2)
    80600454:	001e7e93          	andi	t4,t3,1
    80600458:	020e9b63          	bnez	t4,8060048e <mm_map_megapage_asm_old+0x72>
    8060045c:	f81e                	sd	t2,48(sp)
    8060045e:	00000097          	auipc	ra,0x0
    80600462:	1a2080e7          	jalr	418(ra) # 80600600 <pmm_alloc_page>
    80600466:	73c2                	ld	t2,48(sp)
    80600468:	cd29                	beqz	a0,806004c2 <mm_map_megapage_asm_old+0xa6>
    8060046a:	8f2a                	mv	t5,a0
    8060046c:	8ffa                	mv	t6,t5
    8060046e:	20000293          	li	t0,512
    80600472:	000fb023          	sd	zero,0(t6)
    80600476:	0fa1                	addi	t6,t6,8
    80600478:	12fd                	addi	t0,t0,-1
    8060047a:	fe029ce3          	bnez	t0,80600472 <mm_map_megapage_asm_old+0x56>
    8060047e:	00cf5293          	srli	t0,t5,0xc
    80600482:	02aa                	slli	t0,t0,0xa
    80600484:	0012e293          	ori	t0,t0,1
    80600488:	0053b023          	sd	t0,0(t2)
    8060048c:	8e16                	mv	t3,t0
    8060048e:	00ae5e13          	srli	t3,t3,0xa
    80600492:	0e32                	slli	t3,t3,0xc
    80600494:	89f2                	mv	s3,t3
    80600496:	1a7d                	addi	s4,s4,-1
    80600498:	fb4041e3          	bgtz	s4,8060043a <mm_map_megapage_asm_old+0x1e>
    8060049c:	01545293          	srli	t0,s0,0x15
    806004a0:	1ff2f293          	andi	t0,t0,511
    806004a4:	028e                	slli	t0,t0,0x3
    806004a6:	92ce                	add	t0,t0,s3
    806004a8:	00c4d313          	srli	t1,s1,0xc
    806004ac:	032a                	slli	t1,t1,0xa
    806004ae:	01236333          	or	t1,t1,s2
    806004b2:	00136313          	ori	t1,t1,1
    806004b6:	0c036313          	ori	t1,t1,192
    806004ba:	0062b023          	sd	t1,0(t0)
    806004be:	4501                	li	a0,0
    806004c0:	a011                	j	806004c4 <mm_map_megapage_asm_old+0xa8>
    806004c2:	557d                	li	a0,-1
    806004c4:	6082                	ld	ra,0(sp)
    806004c6:	6422                	ld	s0,8(sp)
    806004c8:	64c2                	ld	s1,16(sp)
    806004ca:	6962                	ld	s2,24(sp)
    806004cc:	7982                	ld	s3,32(sp)
    806004ce:	7a22                	ld	s4,40(sp)
    806004d0:	6121                	addi	sp,sp,64
    806004d2:	8082                	ret

00000000806004d4 <mm_map_kernel>:
    806004d4:	7179                	addi	sp,sp,-48
    806004d6:	e006                	sd	ra,0(sp)
    806004d8:	e422                	sd	s0,8(sp)
    806004da:	e826                	sd	s1,16(sp)
    806004dc:	ec4a                	sd	s2,24(sp)
    806004de:	f04e                	sd	s3,32(sp)
    806004e0:	0010041b          	addiw	s0,zero,1
    806004e4:	047e                	slli	s0,s0,0x1f
    806004e6:	0110049b          	addiw	s1,zero,17
    806004ea:	04ee                	slli	s1,s1,0x1b
    806004ec:	00200937          	lui	s2,0x200
    806004f0:	0ef00993          	li	s3,239
    806004f4:	00947b63          	bgeu	s0,s1,8060050a <mm_map_kernel+0x36>
    806004f8:	8522                	mv	a0,s0
    806004fa:	85a2                	mv	a1,s0
    806004fc:	864e                	mv	a2,s3
    806004fe:	00001097          	auipc	ra,0x1
    80600502:	90c080e7          	jalr	-1780(ra) # 80600e0a <mm_map_megapage>
    80600506:	944a                	add	s0,s0,s2
    80600508:	b7f5                	j	806004f4 <mm_map_kernel+0x20>
    8060050a:	02000537          	lui	a0,0x2000
    8060050e:	020005b7          	lui	a1,0x2000
    80600512:	461d                	li	a2,7
    80600514:	00001097          	auipc	ra,0x1
    80600518:	8f6080e7          	jalr	-1802(ra) # 80600e0a <mm_map_megapage>
    8060051c:	0c000537          	lui	a0,0xc000
    80600520:	0c0005b7          	lui	a1,0xc000
    80600524:	461d                	li	a2,7
    80600526:	00001097          	auipc	ra,0x1
    8060052a:	8e4080e7          	jalr	-1820(ra) # 80600e0a <mm_map_megapage>
    8060052e:	10000537          	lui	a0,0x10000
    80600532:	100005b7          	lui	a1,0x10000
    80600536:	461d                	li	a2,7
    80600538:	00001097          	auipc	ra,0x1
    8060053c:	8d2080e7          	jalr	-1838(ra) # 80600e0a <mm_map_megapage>
    80600540:	6082                	ld	ra,0(sp)
    80600542:	6422                	ld	s0,8(sp)
    80600544:	64c2                	ld	s1,16(sp)
    80600546:	6962                	ld	s2,24(sp)
    80600548:	7982                	ld	s3,32(sp)
    8060054a:	6145                	addi	sp,sp,48
    8060054c:	8082                	ret

000000008060054e <mm_enable_paging>:
    8060054e:	00005297          	auipc	t0,0x5
    80600552:	ab228293          	addi	t0,t0,-1358 # 80605000 <g_root_pt>
    80600556:	00c2d293          	srli	t0,t0,0xc
    8060055a:	4321                	li	t1,8
    8060055c:	1372                	slli	t1,t1,0x3c
    8060055e:	0062e2b3          	or	t0,t0,t1
    80600562:	18029073          	csrw	satp,t0
    80600566:	12000073          	sfence.vma
    8060056a:	8082                	ret

000000008060056c <mm_disable_paging>:
    8060056c:	12000073          	sfence.vma
    80600570:	18001073          	csrw	satp,zero
    80600574:	12000073          	sfence.vma
    80600578:	8082                	ret

000000008060057a <pmm_init>:
    8060057a:	1141                	addi	sp,sp,-16
    8060057c:	e022                	sd	s0,0(sp)
    8060057e:	e426                	sd	s1,8(sp)
    80600580:	00008297          	auipc	t0,0x8
    80600584:	a8028293          	addi	t0,t0,-1408 # 80608000 <g_pmm_base>
    80600588:	00a2b023          	sd	a0,0(t0)
    8060058c:	6305                	lui	t1,0x1
    8060058e:	0265d433          	divu	s0,a1,t1
    80600592:	00008297          	auipc	t0,0x8
    80600596:	a7628293          	addi	t0,t0,-1418 # 80608008 <g_pmm_pages>
    8060059a:	0082b023          	sd	s0,0(t0)
    8060059e:	00008297          	auipc	t0,0x8
    806005a2:	a7228293          	addi	t0,t0,-1422 # 80608010 <g_pmm_free>
    806005a6:	0082b023          	sd	s0,0(t0)
    806005aa:	00007297          	auipc	t0,0x7
    806005ae:	a5628293          	addi	t0,t0,-1450 # 80607000 <g_pmm_bitmap>
    806005b2:	4381                	li	t2,0
    806005b4:	20000e13          	li	t3,512
    806005b8:	0072b023          	sd	t2,0(t0)
    806005bc:	02a1                	addi	t0,t0,8
    806005be:	1e7d                	addi	t3,t3,-1
    806005c0:	fe0e1ce3          	bnez	t3,806005b8 <pmm_init+0x3e>
    806005c4:	8322                	mv	t1,s0
    806005c6:	63a1                	lui	t2,0x8
    806005c8:	00007297          	auipc	t0,0x7
    806005cc:	a3828293          	addi	t0,t0,-1480 # 80607000 <g_pmm_bitmap>
    806005d0:	02737463          	bgeu	t1,t2,806005f8 <pmm_init+0x7e>
    806005d4:	00635e13          	srli	t3,t1,0x6
    806005d8:	03f37e93          	andi	t4,t1,63
    806005dc:	003e1f13          	slli	t5,t3,0x3
    806005e0:	9f16                	add	t5,t5,t0
    806005e2:	000f3f83          	ld	t6,0(t5)
    806005e6:	4e05                	li	t3,1
    806005e8:	01de1e33          	sll	t3,t3,t4
    806005ec:	01cfefb3          	or	t6,t6,t3
    806005f0:	01ff3023          	sd	t6,0(t5)
    806005f4:	0305                	addi	t1,t1,1 # 1001 <PAGE_SIZE+0x1>
    806005f6:	bfe9                	j	806005d0 <pmm_init+0x56>
    806005f8:	6402                	ld	s0,0(sp)
    806005fa:	64a2                	ld	s1,8(sp)
    806005fc:	0141                	addi	sp,sp,16
    806005fe:	8082                	ret

0000000080600600 <pmm_alloc_page>:
    80600600:	00007297          	auipc	t0,0x7
    80600604:	a0028293          	addi	t0,t0,-1536 # 80607000 <g_pmm_bitmap>
    80600608:	4381                	li	t2,0
    8060060a:	20000413          	li	s0,512
    8060060e:	0883f163          	bgeu	t2,s0,80600690 <pmm_alloc_page+0x90>
    80600612:	00339e93          	slli	t4,t2,0x3
    80600616:	9e96                	add	t4,t4,t0
    80600618:	000ebf03          	ld	t5,0(t4)
    8060061c:	ffff4f13          	not	t5,t5
    80600620:	060f0663          	beqz	t5,8060068c <pmm_alloc_page+0x8c>
    80600624:	4f81                	li	t6,0
    80600626:	001f7e93          	andi	t4,t5,1
    8060062a:	000e9a63          	bnez	t4,8060063e <pmm_alloc_page+0x3e>
    8060062e:	001f5f13          	srli	t5,t5,0x1
    80600632:	0f85                	addi	t6,t6,1
    80600634:	04000e93          	li	t4,64
    80600638:	05dffa63          	bgeu	t6,t4,8060068c <pmm_alloc_page+0x8c>
    8060063c:	b7ed                	j	80600626 <pmm_alloc_page+0x26>
    8060063e:	00639f13          	slli	t5,t2,0x6
    80600642:	9f7e                	add	t5,t5,t6
    80600644:	006f5e13          	srli	t3,t5,0x6
    80600648:	03ff7e93          	andi	t4,t5,63
    8060064c:	003e1f93          	slli	t6,t3,0x3
    80600650:	9f96                	add	t6,t6,t0
    80600652:	000fbe03          	ld	t3,0(t6)
    80600656:	4385                	li	t2,1
    80600658:	01d393b3          	sll	t2,t2,t4
    8060065c:	007e6e33          	or	t3,t3,t2
    80600660:	01cfb023          	sd	t3,0(t6)
    80600664:	00008e17          	auipc	t3,0x8
    80600668:	9ace0e13          	addi	t3,t3,-1620 # 80608010 <g_pmm_free>
    8060066c:	000e3e83          	ld	t4,0(t3)
    80600670:	1efd                	addi	t4,t4,-1
    80600672:	01de3023          	sd	t4,0(t3)
    80600676:	00008e17          	auipc	t3,0x8
    8060067a:	98ae0e13          	addi	t3,t3,-1654 # 80608000 <g_pmm_base>
    8060067e:	000e3e03          	ld	t3,0(t3)
    80600682:	00cf1e93          	slli	t4,t5,0xc
    80600686:	01de0533          	add	a0,t3,t4
    8060068a:	8082                	ret
    8060068c:	0385                	addi	t2,t2,1 # 8001 <PMM_MAX_PAGES+0x1>
    8060068e:	b741                	j	8060060e <pmm_alloc_page+0xe>
    80600690:	4501                	li	a0,0
    80600692:	8082                	ret

0000000080600694 <pmm_free_page>:
    80600694:	c931                	beqz	a0,806006e8 <pmm_free_page+0x54>
    80600696:	00008297          	auipc	t0,0x8
    8060069a:	96a28293          	addi	t0,t0,-1686 # 80608000 <g_pmm_base>
    8060069e:	0002b283          	ld	t0,0(t0)
    806006a2:	40550333          	sub	t1,a0,t0
    806006a6:	00c35313          	srli	t1,t1,0xc
    806006aa:	00635e13          	srli	t3,t1,0x6
    806006ae:	03f37e93          	andi	t4,t1,63
    806006b2:	00007f17          	auipc	t5,0x7
    806006b6:	94ef0f13          	addi	t5,t5,-1714 # 80607000 <g_pmm_bitmap>
    806006ba:	003e1f93          	slli	t6,t3,0x3
    806006be:	9f7e                	add	t5,t5,t6
    806006c0:	000f3f83          	ld	t6,0(t5)
    806006c4:	4e05                	li	t3,1
    806006c6:	01de1e33          	sll	t3,t3,t4
    806006ca:	fffe4e13          	not	t3,t3
    806006ce:	01cfffb3          	and	t6,t6,t3
    806006d2:	01ff3023          	sd	t6,0(t5)
    806006d6:	00008e17          	auipc	t3,0x8
    806006da:	93ae0e13          	addi	t3,t3,-1734 # 80608010 <g_pmm_free>
    806006de:	000e3e83          	ld	t4,0(t3)
    806006e2:	0e85                	addi	t4,t4,1
    806006e4:	01de3023          	sd	t4,0(t3)
    806006e8:	8082                	ret

00000000806006ea <pmm_mark_used>:
    806006ea:	cdbd                	beqz	a1,80600768 <pmm_mark_used+0x7e>
    806006ec:	1141                	addi	sp,sp,-16
    806006ee:	e022                	sd	s0,0(sp)
    806006f0:	e426                	sd	s1,8(sp)
    806006f2:	00008297          	auipc	t0,0x8
    806006f6:	90e28293          	addi	t0,t0,-1778 # 80608000 <g_pmm_base>
    806006fa:	0002b283          	ld	t0,0(t0)
    806006fe:	40550333          	sub	t1,a0,t0
    80600702:	00c35413          	srli	s0,t1,0xc
    80600706:	00b503b3          	add	t2,a0,a1
    8060070a:	405383b3          	sub	t2,t2,t0
    8060070e:	6e05                	lui	t3,0x1
    80600710:	3e7d                	addiw	t3,t3,-1 # fff <PMM_BITMAP_WORDS+0xdff>
    80600712:	93f2                	add	t2,t2,t3
    80600714:	00c3d493          	srli	s1,t2,0xc
    80600718:	04947563          	bgeu	s0,s1,80600762 <pmm_mark_used+0x78>
    8060071c:	00645e13          	srli	t3,s0,0x6
    80600720:	03f47e93          	andi	t4,s0,63
    80600724:	00007f17          	auipc	t5,0x7
    80600728:	8dcf0f13          	addi	t5,t5,-1828 # 80607000 <g_pmm_bitmap>
    8060072c:	003e1f93          	slli	t6,t3,0x3
    80600730:	9f7e                	add	t5,t5,t6
    80600732:	000f3f83          	ld	t6,0(t5)
    80600736:	4e05                	li	t3,1
    80600738:	01de1e33          	sll	t3,t3,t4
    8060073c:	01cffeb3          	and	t4,t6,t3
    80600740:	01cfefb3          	or	t6,t6,t3
    80600744:	01ff3023          	sd	t6,0(t5)
    80600748:	000e9b63          	bnez	t4,8060075e <pmm_mark_used+0x74>
    8060074c:	00008f17          	auipc	t5,0x8
    80600750:	8c4f0f13          	addi	t5,t5,-1852 # 80608010 <g_pmm_free>
    80600754:	000f3f83          	ld	t6,0(t5)
    80600758:	1ffd                	addi	t6,t6,-1
    8060075a:	01ff3023          	sd	t6,0(t5)
    8060075e:	0405                	addi	s0,s0,1
    80600760:	bf65                	j	80600718 <pmm_mark_used+0x2e>
    80600762:	6402                	ld	s0,0(sp)
    80600764:	64a2                	ld	s1,8(sp)
    80600766:	0141                	addi	sp,sp,16
    80600768:	8082                	ret

000000008060076a <trap_init>:
    8060076a:	00000297          	auipc	t0,0x0
    8060076e:	02628293          	addi	t0,t0,38 # 80600790 <trap_vector>
    80600772:	10529073          	csrw	stvec,t0
    80600776:	00018317          	auipc	t1,0x18
    8060077a:	2ea30313          	addi	t1,t1,746 # 80618a60 <g_trap_stacks>
    8060077e:	6385                	lui	t2,0x1
    80600780:	0505                	addi	a0,a0,1 # 10000001 <DRAM_SIZE+0x8000001>
    80600782:	02a383b3          	mul	t2,t2,a0
    80600786:	931e                	add	t1,t1,t2
    80600788:	14031073          	csrw	sscratch,t1
    8060078c:	8082                	ret
	...

0000000080600790 <trap_vector>:
    80600790:	14011173          	csrrw	sp,sscratch,sp
    80600794:	7119                	addi	sp,sp,-128
    80600796:	e006                	sd	ra,0(sp)
    80600798:	140022f3          	csrr	t0,sscratch
    8060079c:	e416                	sd	t0,8(sp)
    8060079e:	e82a                	sd	a0,16(sp)
    806007a0:	ec2e                	sd	a1,24(sp)
    806007a2:	f032                	sd	a2,32(sp)
    806007a4:	08010293          	addi	t0,sp,128
    806007a8:	14029073          	csrw	sscratch,t0
    806007ac:	14202573          	csrr	a0,scause
    806007b0:	141025f3          	csrr	a1,sepc
    806007b4:	14302673          	csrr	a2,stval
    806007b8:	00000097          	auipc	ra,0x0
    806007bc:	7ac080e7          	jalr	1964(ra) # 80600f64 <c_trap_handler>
    806007c0:	10500073          	wfi
    806007c4:	bff5                	j	806007c0 <trap_vector+0x30>
    806007c6:	0001                	nop

00000000806007c8 <task_a>:
    806007c8:	1141                	addi	sp,sp,-16
    806007ca:	00002517          	auipc	a0,0x2
    806007ce:	30650513          	addi	a0,a0,774 # 80602ad0 <kexec+0xa>
    806007d2:	e406                	sd	ra,8(sp)
    806007d4:	00001097          	auipc	ra,0x1
    806007d8:	868080e7          	jalr	-1944(ra) # 8060103c <uart_puts>
    806007dc:	00000097          	auipc	ra,0x0
    806007e0:	a84080e7          	jalr	-1404(ra) # 80600260 <sched_yield>
    806007e4:	bfe5                	j	806007dc <task_a+0x14>

00000000806007e6 <task_b>:
    806007e6:	1141                	addi	sp,sp,-16
    806007e8:	00002517          	auipc	a0,0x2
    806007ec:	30050513          	addi	a0,a0,768 # 80602ae8 <kexec+0x22>
    806007f0:	e406                	sd	ra,8(sp)
    806007f2:	00001097          	auipc	ra,0x1
    806007f6:	84a080e7          	jalr	-1974(ra) # 8060103c <uart_puts>
    806007fa:	00000097          	auipc	ra,0x0
    806007fe:	a66080e7          	jalr	-1434(ra) # 80600260 <sched_yield>
    80600802:	bfe5                	j	806007fa <task_b+0x14>

0000000080600804 <wake_hart.constprop.0>:
    80600804:	1141                	addi	sp,sp,-16
    80600806:	00002517          	auipc	a0,0x2
    8060080a:	2fa50513          	addi	a0,a0,762 # 80602b00 <kexec+0x3a>
    8060080e:	e406                	sd	ra,8(sp)
    80600810:	00001097          	auipc	ra,0x1
    80600814:	82c080e7          	jalr	-2004(ra) # 8060103c <uart_puts>
    80600818:	03100513          	li	a0,49
    8060081c:	00000097          	auipc	ra,0x0
    80600820:	7fc080e7          	jalr	2044(ra) # 80601018 <uart_putc>
    80600824:	00002517          	auipc	a0,0x2
    80600828:	30450513          	addi	a0,a0,772 # 80602b28 <kexec+0x62>
    8060082c:	00001097          	auipc	ra,0x1
    80600830:	810080e7          	jalr	-2032(ra) # 8060103c <uart_puts>
    80600834:	73445837          	lui	a6,0x73445
    80600838:	95080813          	addi	a6,a6,-1712 # 73444950 <DRAM_SIZE+0x6b444950>
    8060083c:	4509                	li	a0,2
    8060083e:	4581                	li	a1,0
    80600840:	4881                	li	a7,0
    80600842:	00000073          	ecall
    80600846:	60a2                	ld	ra,8(sp)
    80600848:	00002517          	auipc	a0,0x2
    8060084c:	2e850513          	addi	a0,a0,744 # 80602b30 <kexec+0x6a>
    80600850:	0141                	addi	sp,sp,16
    80600852:	00000317          	auipc	t1,0x0
    80600856:	7ea30067          	jr	2026(t1) # 8060103c <uart_puts>

000000008060085a <secondary_hart_init>:
    8060085a:	1141                	addi	sp,sp,-16
    8060085c:	e406                	sd	ra,8(sp)
    8060085e:	e022                	sd	s0,0(sp)
    80600860:	842a                	mv	s0,a0
    80600862:	00000097          	auipc	ra,0x0
    80600866:	f08080e7          	jalr	-248(ra) # 8060076a <trap_init>
    8060086a:	00002517          	auipc	a0,0x2
    8060086e:	2de50513          	addi	a0,a0,734 # 80602b48 <kexec+0x82>
    80600872:	00000097          	auipc	ra,0x0
    80600876:	7ca080e7          	jalr	1994(ra) # 8060103c <uart_puts>
    8060087a:	8522                	mv	a0,s0
    8060087c:	00000097          	auipc	ra,0x0
    80600880:	7f2080e7          	jalr	2034(ra) # 8060106e <uart_hex>
    80600884:	00002517          	auipc	a0,0x2
    80600888:	2e450513          	addi	a0,a0,740 # 80602b68 <kexec+0xa2>
    8060088c:	00000097          	auipc	ra,0x0
    80600890:	7b0080e7          	jalr	1968(ra) # 8060103c <uart_puts>
    80600894:	4785                	li	a5,1
    80600896:	00003717          	auipc	a4,0x3
    8060089a:	9ef72323          	sw	a5,-1562(a4) # 8060327c <g_init_done>
    8060089e:	0330000f          	fence	rw,rw
    806008a2:	00002517          	auipc	a0,0x2
    806008a6:	2e650513          	addi	a0,a0,742 # 80602b88 <kexec+0xc2>
    806008aa:	00000097          	auipc	ra,0x0
    806008ae:	792080e7          	jalr	1938(ra) # 8060103c <uart_puts>
    806008b2:	00000097          	auipc	ra,0x0
    806008b6:	f52080e7          	jalr	-174(ra) # 80600804 <wake_hart.constprop.0>
    806008ba:	00002517          	auipc	a0,0x2
    806008be:	2f650513          	addi	a0,a0,758 # 80602bb0 <kexec+0xea>
    806008c2:	00000097          	auipc	ra,0x0
    806008c6:	77a080e7          	jalr	1914(ra) # 8060103c <uart_puts>
    806008ca:	00001097          	auipc	ra,0x1
    806008ce:	8be080e7          	jalr	-1858(ra) # 80601188 <start_user_process>
    806008d2:	00002517          	auipc	a0,0x2
    806008d6:	30650513          	addi	a0,a0,774 # 80602bd8 <kexec+0x112>
    806008da:	00000097          	auipc	ra,0x0
    806008de:	762080e7          	jalr	1890(ra) # 8060103c <uart_puts>
    806008e2:	00000097          	auipc	ra,0x0
    806008e6:	a06080e7          	jalr	-1530(ra) # 806002e8 <sched_start>

00000000806008ea <kernel_main>:
    806008ea:	7179                	addi	sp,sp,-48
    806008ec:	f406                	sd	ra,40(sp)
    806008ee:	f022                	sd	s0,32(sp)
    806008f0:	ec26                	sd	s1,24(sp)
    806008f2:	842a                	mv	s0,a0
    806008f4:	e84a                	sd	s2,16(sp)
    806008f6:	e44e                	sd	s3,8(sp)
    806008f8:	00000097          	auipc	ra,0x0
    806008fc:	e72080e7          	jalr	-398(ra) # 8060076a <trap_init>
    80600900:	00002517          	auipc	a0,0x2
    80600904:	30050513          	addi	a0,a0,768 # 80602c00 <kexec+0x13a>
    80600908:	00000097          	auipc	ra,0x0
    8060090c:	734080e7          	jalr	1844(ra) # 8060103c <uart_puts>
    80600910:	8522                	mv	a0,s0
    80600912:	00000097          	auipc	ra,0x0
    80600916:	75c080e7          	jalr	1884(ra) # 8060106e <uart_hex>
    8060091a:	4529                	li	a0,10
    8060091c:	00000097          	auipc	ra,0x0
    80600920:	6fc080e7          	jalr	1788(ra) # 80601018 <uart_putc>
    80600924:	8412                	mv	s0,tp
    80600926:	00002517          	auipc	a0,0x2
    8060092a:	30250513          	addi	a0,a0,770 # 80602c28 <kexec+0x162>
    8060092e:	00000097          	auipc	ra,0x0
    80600932:	70e080e7          	jalr	1806(ra) # 8060103c <uart_puts>
    80600936:	8522                	mv	a0,s0
    80600938:	00000097          	auipc	ra,0x0
    8060093c:	736080e7          	jalr	1846(ra) # 8060106e <uart_hex>
    80600940:	4529                	li	a0,10
    80600942:	00000097          	auipc	ra,0x0
    80600946:	6d6080e7          	jalr	1750(ra) # 80601018 <uart_putc>
    8060094a:	fffff717          	auipc	a4,0xfffff
    8060094e:	6b670713          	addi	a4,a4,1718 # 80600000 <KERNEL_BASE>
    80600952:	46a1                	li	a3,8
    80600954:	04d405a3          	sb	a3,75(s0)
    80600958:	00875693          	srli	a3,a4,0x8
    8060095c:	04d408a3          	sb	a3,81(s0)
    80600960:	01075693          	srli	a3,a4,0x10
    80600964:	04d40923          	sb	a3,82(s0)
    80600968:	0187569b          	srliw	a3,a4,0x18
    8060096c:	04d409a3          	sb	a3,83(s0)
    80600970:	02075693          	srli	a3,a4,0x20
    80600974:	04d40a23          	sb	a3,84(s0)
    80600978:	02875693          	srli	a3,a4,0x28
    8060097c:	00035797          	auipc	a5,0x35
    80600980:	f4478793          	addi	a5,a5,-188 # 806358c0 <__sched_data_end>
    80600984:	04d40aa3          	sb	a3,85(s0)
    80600988:	03075693          	srli	a3,a4,0x30
    8060098c:	04d40b23          	sb	a3,86(s0)
    80600990:	04e40823          	sb	a4,80(s0)
    80600994:	03875693          	srli	a3,a4,0x38
    80600998:	0087d713          	srli	a4,a5,0x8
    8060099c:	04e40ca3          	sb	a4,89(s0)
    806009a0:	0107d713          	srli	a4,a5,0x10
    806009a4:	04e40d23          	sb	a4,90(s0)
    806009a8:	0187d71b          	srliw	a4,a5,0x18
    806009ac:	04e40da3          	sb	a4,91(s0)
    806009b0:	0207d713          	srli	a4,a5,0x20
    806009b4:	04e40e23          	sb	a4,92(s0)
    806009b8:	0287d713          	srli	a4,a5,0x28
    806009bc:	04e40ea3          	sb	a4,93(s0)
    806009c0:	0307d713          	srli	a4,a5,0x30
    806009c4:	04e40f23          	sb	a4,94(s0)
    806009c8:	04f40c23          	sb	a5,88(s0)
    806009cc:	0387d713          	srli	a4,a5,0x38
    806009d0:	f8000793          	li	a5,-128
    806009d4:	4985                	li	s3,1
    806009d6:	04d40ba3          	sb	a3,87(s0)
    806009da:	04e40fa3          	sb	a4,95(s0)
    806009de:	f9600693          	li	a3,-106
    806009e2:	f9800713          	li	a4,-104
    806009e6:	04f401a3          	sb	a5,67(s0)
    806009ea:	04040023          	sb	zero,64(s0)
    806009ee:	040400a3          	sb	zero,65(s0)
    806009f2:	04040123          	sb	zero,66(s0)
    806009f6:	04040223          	sb	zero,68(s0)
    806009fa:	040402a3          	sb	zero,69(s0)
    806009fe:	04040323          	sb	zero,70(s0)
    80600a02:	040403a3          	sb	zero,71(s0)
    80600a06:	04040423          	sb	zero,72(s0)
    80600a0a:	040404a3          	sb	zero,73(s0)
    80600a0e:	04040523          	sb	zero,74(s0)
    80600a12:	04040623          	sb	zero,76(s0)
    80600a16:	040406a3          	sb	zero,77(s0)
    80600a1a:	04040723          	sb	zero,78(s0)
    80600a1e:	040407a3          	sb	zero,79(s0)
    80600a22:	0a040423          	sb	zero,168(s0)
    80600a26:	00002517          	auipc	a0,0x2
    80600a2a:	21250513          	addi	a0,a0,530 # 80602c38 <kexec+0x172>
    80600a2e:	0ad408a3          	sb	a3,177(s0)
    80600a32:	0ae40923          	sb	a4,178(s0)
    80600a36:	0af40823          	sb	a5,176(s0)
    80600a3a:	0a0404a3          	sb	zero,169(s0)
    80600a3e:	77fd                	lui	a5,0xfffff
    80600a40:	0a040523          	sb	zero,170(s0)
    80600a44:	0a0405a3          	sb	zero,171(s0)
    80600a48:	0a040623          	sb	zero,172(s0)
    80600a4c:	0a0406a3          	sb	zero,173(s0)
    80600a50:	0a040723          	sb	zero,174(s0)
    80600a54:	0a0407a3          	sb	zero,175(s0)
    80600a58:	0a0409a3          	sb	zero,179(s0)
    80600a5c:	0a040a23          	sb	zero,180(s0)
    80600a60:	0a040aa3          	sb	zero,181(s0)
    80600a64:	0a040b23          	sb	zero,182(s0)
    80600a68:	0a040ba3          	sb	zero,183(s0)
    80600a6c:	000408a3          	sb	zero,17(s0)
    80600a70:	00040923          	sb	zero,18(s0)
    80600a74:	000409a3          	sb	zero,19(s0)
    80600a78:	00040a23          	sb	zero,20(s0)
    80600a7c:	00040aa3          	sb	zero,21(s0)
    80600a80:	00040b23          	sb	zero,22(s0)
    80600a84:	00040ba3          	sb	zero,23(s0)
    80600a88:	01340823          	sb	s3,16(s0)
    80600a8c:	00036497          	auipc	s1,0x36
    80600a90:	e3348493          	addi	s1,s1,-461 # 806368bf <__sched_data_end+0xfff>
    80600a94:	4945                	li	s2,17
    80600a96:	8cfd                	and	s1,s1,a5
    80600a98:	096e                	slli	s2,s2,0x1b
    80600a9a:	00000097          	auipc	ra,0x0
    80600a9e:	5a2080e7          	jalr	1442(ra) # 8060103c <uart_puts>
    80600aa2:	409905b3          	sub	a1,s2,s1
    80600aa6:	8526                	mv	a0,s1
    80600aa8:	00000097          	auipc	ra,0x0
    80600aac:	ad2080e7          	jalr	-1326(ra) # 8060057a <pmm_init>
    80600ab0:	00002517          	auipc	a0,0x2
    80600ab4:	1a050513          	addi	a0,a0,416 # 80602c50 <kexec+0x18a>
    80600ab8:	00000097          	auipc	ra,0x0
    80600abc:	584080e7          	jalr	1412(ra) # 8060103c <uart_puts>
    80600ac0:	8526                	mv	a0,s1
    80600ac2:	00000097          	auipc	ra,0x0
    80600ac6:	5ac080e7          	jalr	1452(ra) # 8060106e <uart_hex>
    80600aca:	00002517          	auipc	a0,0x2
    80600ace:	19e50513          	addi	a0,a0,414 # 80602c68 <kexec+0x1a2>
    80600ad2:	00000097          	auipc	ra,0x0
    80600ad6:	56a080e7          	jalr	1386(ra) # 8060103c <uart_puts>
    80600ada:	854a                	mv	a0,s2
    80600adc:	00000097          	auipc	ra,0x0
    80600ae0:	592080e7          	jalr	1426(ra) # 8060106e <uart_hex>
    80600ae4:	4529                	li	a0,10
    80600ae6:	00000097          	auipc	ra,0x0
    80600aea:	532080e7          	jalr	1330(ra) # 80601018 <uart_putc>
    80600aee:	800005b7          	lui	a1,0x80000
    80600af2:	95a6                	add	a1,a1,s1
    80600af4:	01f99513          	slli	a0,s3,0x1f
    80600af8:	00000097          	auipc	ra,0x0
    80600afc:	bf2080e7          	jalr	-1038(ra) # 806006ea <pmm_mark_used>
    80600b00:	0084d893          	srli	a7,s1,0x8
    80600b04:	0104d813          	srli	a6,s1,0x10
    80600b08:	0204d593          	srli	a1,s1,0x20
    80600b0c:	0284d613          	srli	a2,s1,0x28
    80600b10:	0304d693          	srli	a3,s1,0x30
    80600b14:	f8800713          	li	a4,-120
    80600b18:	4789                	li	a5,2
    80600b1a:	0184d51b          	srliw	a0,s1,0x18
    80600b1e:	90e1                	srli	s1,s1,0x38
    80600b20:	071400a3          	sb	a7,97(s0)
    80600b24:	07040123          	sb	a6,98(s0)
    80600b28:	06b40223          	sb	a1,100(s0)
    80600b2c:	06c402a3          	sb	a2,101(s0)
    80600b30:	06d40323          	sb	a3,102(s0)
    80600b34:	06e405a3          	sb	a4,107(s0)
    80600b38:	00f40823          	sb	a5,16(s0)
    80600b3c:	06a401a3          	sb	a0,99(s0)
    80600b40:	06040023          	sb	zero,96(s0)
    80600b44:	00002517          	auipc	a0,0x2
    80600b48:	13450513          	addi	a0,a0,308 # 80602c78 <kexec+0x1b2>
    80600b4c:	06040423          	sb	zero,104(s0)
    80600b50:	060404a3          	sb	zero,105(s0)
    80600b54:	06040523          	sb	zero,106(s0)
    80600b58:	06040623          	sb	zero,108(s0)
    80600b5c:	060406a3          	sb	zero,109(s0)
    80600b60:	06040723          	sb	zero,110(s0)
    80600b64:	060407a3          	sb	zero,111(s0)
    80600b68:	08040423          	sb	zero,136(s0)
    80600b6c:	080404a3          	sb	zero,137(s0)
    80600b70:	08040523          	sb	zero,138(s0)
    80600b74:	080405a3          	sb	zero,139(s0)
    80600b78:	08040623          	sb	zero,140(s0)
    80600b7c:	080406a3          	sb	zero,141(s0)
    80600b80:	08040723          	sb	zero,142(s0)
    80600b84:	080407a3          	sb	zero,143(s0)
    80600b88:	000408a3          	sb	zero,17(s0)
    80600b8c:	00040923          	sb	zero,18(s0)
    80600b90:	000409a3          	sb	zero,19(s0)
    80600b94:	00040a23          	sb	zero,20(s0)
    80600b98:	00040aa3          	sb	zero,21(s0)
    80600b9c:	00040b23          	sb	zero,22(s0)
    80600ba0:	00040ba3          	sb	zero,23(s0)
    80600ba4:	069403a3          	sb	s1,103(s0)
    80600ba8:	00000097          	auipc	ra,0x0
    80600bac:	494080e7          	jalr	1172(ra) # 8060103c <uart_puts>
    80600bb0:	00000097          	auipc	ra,0x0
    80600bb4:	830080e7          	jalr	-2000(ra) # 806003e0 <mm_init>
    80600bb8:	00002517          	auipc	a0,0x2
    80600bbc:	0d850513          	addi	a0,a0,216 # 80602c90 <kexec+0x1ca>
    80600bc0:	00000097          	auipc	ra,0x0
    80600bc4:	47c080e7          	jalr	1148(ra) # 8060103c <uart_puts>
    80600bc8:	00000097          	auipc	ra,0x0
    80600bcc:	90c080e7          	jalr	-1780(ra) # 806004d4 <mm_map_kernel>
    80600bd0:	00002517          	auipc	a0,0x2
    80600bd4:	0d850513          	addi	a0,a0,216 # 80602ca8 <kexec+0x1e2>
    80600bd8:	00000097          	auipc	ra,0x0
    80600bdc:	464080e7          	jalr	1124(ra) # 8060103c <uart_puts>
    80600be0:	00000097          	auipc	ra,0x0
    80600be4:	96e080e7          	jalr	-1682(ra) # 8060054e <mm_enable_paging>
    80600be8:	00002517          	auipc	a0,0x2
    80600bec:	0e050513          	addi	a0,a0,224 # 80602cc8 <kexec+0x202>
    80600bf0:	00000097          	auipc	ra,0x0
    80600bf4:	44c080e7          	jalr	1100(ra) # 8060103c <uart_puts>
    80600bf8:	00004717          	auipc	a4,0x4
    80600bfc:	40870713          	addi	a4,a4,1032 # 80605000 <g_root_pt>
    80600c00:	00007797          	auipc	a5,0x7
    80600c04:	41878793          	addi	a5,a5,1048 # 80608018 <g_sched>
    80600c08:	00875613          	srli	a2,a4,0x8
    80600c0c:	01075693          	srli	a3,a4,0x10
    80600c10:	0187529b          	srliw	t0,a4,0x18
    80600c14:	02075f93          	srli	t6,a4,0x20
    80600c18:	02875f13          	srli	t5,a4,0x28
    80600c1c:	03075e93          	srli	t4,a4,0x30
    80600c20:	03875e13          	srli	t3,a4,0x38
    80600c24:	0087d893          	srli	a7,a5,0x8
    80600c28:	0107d813          	srli	a6,a5,0x10
    80600c2c:	0187d51b          	srliw	a0,a5,0x18
    80600c30:	0207d593          	srli	a1,a5,0x20
    80600c34:	430d                	li	t1,3
    80600c36:	08e40823          	sb	a4,144(s0)
    80600c3a:	08c408a3          	sb	a2,145(s0)
    80600c3e:	0387d713          	srli	a4,a5,0x38
    80600c42:	0287d613          	srli	a2,a5,0x28
    80600c46:	08d40923          	sb	a3,146(s0)
    80600c4a:	0307d693          	srli	a3,a5,0x30
    80600c4e:	085409a3          	sb	t0,147(s0)
    80600c52:	09f40a23          	sb	t6,148(s0)
    80600c56:	09e40aa3          	sb	t5,149(s0)
    80600c5a:	09d40b23          	sb	t4,150(s0)
    80600c5e:	09c40ba3          	sb	t3,151(s0)
    80600c62:	00640823          	sb	t1,16(s0)
    80600c66:	08f40023          	sb	a5,128(s0)
    80600c6a:	091400a3          	sb	a7,129(s0)
    80600c6e:	09040123          	sb	a6,130(s0)
    80600c72:	08b40223          	sb	a1,132(s0)
    80600c76:	08c402a3          	sb	a2,133(s0)
    80600c7a:	08d40323          	sb	a3,134(s0)
    80600c7e:	08e403a3          	sb	a4,135(s0)
    80600c82:	08a401a3          	sb	a0,131(s0)
    80600c86:	000408a3          	sb	zero,17(s0)
    80600c8a:	00040923          	sb	zero,18(s0)
    80600c8e:	000409a3          	sb	zero,19(s0)
    80600c92:	00040a23          	sb	zero,20(s0)
    80600c96:	00040aa3          	sb	zero,21(s0)
    80600c9a:	00040b23          	sb	zero,22(s0)
    80600c9e:	00040ba3          	sb	zero,23(s0)
    80600ca2:	00000517          	auipc	a0,0x0
    80600ca6:	b2650513          	addi	a0,a0,-1242 # 806007c8 <task_a>
    80600caa:	fffff097          	auipc	ra,0xfffff
    80600cae:	426080e7          	jalr	1062(ra) # 806000d0 <sched_create>
    80600cb2:	fffff097          	auipc	ra,0xfffff
    80600cb6:	4a6080e7          	jalr	1190(ra) # 80600158 <sched_enqueue>
    80600cba:	00000517          	auipc	a0,0x0
    80600cbe:	b2c50513          	addi	a0,a0,-1236 # 806007e6 <task_b>
    80600cc2:	fffff097          	auipc	ra,0xfffff
    80600cc6:	40e080e7          	jalr	1038(ra) # 806000d0 <sched_create>
    80600cca:	fffff097          	auipc	ra,0xfffff
    80600cce:	48e080e7          	jalr	1166(ra) # 80600158 <sched_enqueue>
    80600cd2:	0c944703          	lbu	a4,201(s0)
    80600cd6:	0c844683          	lbu	a3,200(s0)
    80600cda:	0ca44783          	lbu	a5,202(s0)
    80600cde:	0cb44583          	lbu	a1,203(s0)
    80600ce2:	0cc44603          	lbu	a2,204(s0)
    80600ce6:	0722                	slli	a4,a4,0x8
    80600ce8:	8f55                	or	a4,a4,a3
    80600cea:	07c2                	slli	a5,a5,0x10
    80600cec:	0cd44683          	lbu	a3,205(s0)
    80600cf0:	8fd9                	or	a5,a5,a4
    80600cf2:	05e2                	slli	a1,a1,0x18
    80600cf4:	0ce44703          	lbu	a4,206(s0)
    80600cf8:	8ddd                	or	a1,a1,a5
    80600cfa:	1602                	slli	a2,a2,0x20
    80600cfc:	0cf44783          	lbu	a5,207(s0)
    80600d00:	8e4d                	or	a2,a2,a1
    80600d02:	16a2                	slli	a3,a3,0x28
    80600d04:	8ed1                	or	a3,a3,a2
    80600d06:	1742                	slli	a4,a4,0x30
    80600d08:	8f55                	or	a4,a4,a3
    80600d0a:	17e2                	slli	a5,a5,0x38
    80600d0c:	8fd9                	or	a5,a5,a4
    80600d0e:	00003717          	auipc	a4,0x3
    80600d12:	4f270713          	addi	a4,a4,1266 # 80604200 <g_hart_data>
    80600d16:	97ce                	add	a5,a5,s3
    80600d18:	00875613          	srli	a2,a4,0x8
    80600d1c:	01075693          	srli	a3,a4,0x10
    80600d20:	0187529b          	srliw	t0,a4,0x18
    80600d24:	02075f93          	srli	t6,a4,0x20
    80600d28:	02875f13          	srli	t5,a4,0x28
    80600d2c:	03075e93          	srli	t4,a4,0x30
    80600d30:	03875e13          	srli	t3,a4,0x38
    80600d34:	0187d51b          	srliw	a0,a5,0x18
    80600d38:	4315                	li	t1,5
    80600d3a:	0087d893          	srli	a7,a5,0x8
    80600d3e:	0107d813          	srli	a6,a5,0x10
    80600d42:	0207d593          	srli	a1,a5,0x20
    80600d46:	0ce40023          	sb	a4,192(s0)
    80600d4a:	0cc400a3          	sb	a2,193(s0)
    80600d4e:	0387d713          	srli	a4,a5,0x38
    80600d52:	0287d613          	srli	a2,a5,0x28
    80600d56:	0cd40123          	sb	a3,194(s0)
    80600d5a:	0307d693          	srli	a3,a5,0x30
    80600d5e:	0c5401a3          	sb	t0,195(s0)
    80600d62:	0df40223          	sb	t6,196(s0)
    80600d66:	0de402a3          	sb	t5,197(s0)
    80600d6a:	0dd40323          	sb	t4,198(s0)
    80600d6e:	0dc403a3          	sb	t3,199(s0)
    80600d72:	00640823          	sb	t1,16(s0)
    80600d76:	0cf40423          	sb	a5,200(s0)
    80600d7a:	0d1404a3          	sb	a7,201(s0)
    80600d7e:	0d040523          	sb	a6,202(s0)
    80600d82:	0cb40623          	sb	a1,204(s0)
    80600d86:	0cc406a3          	sb	a2,205(s0)
    80600d8a:	0cd40723          	sb	a3,206(s0)
    80600d8e:	0ce407a3          	sb	a4,207(s0)
    80600d92:	0ca405a3          	sb	a0,203(s0)
    80600d96:	000408a3          	sb	zero,17(s0)
    80600d9a:	00002517          	auipc	a0,0x2
    80600d9e:	f4650513          	addi	a0,a0,-186 # 80602ce0 <kexec+0x21a>
    80600da2:	00040923          	sb	zero,18(s0)
    80600da6:	000409a3          	sb	zero,19(s0)
    80600daa:	00040a23          	sb	zero,20(s0)
    80600dae:	00040aa3          	sb	zero,21(s0)
    80600db2:	00040b23          	sb	zero,22(s0)
    80600db6:	00040ba3          	sb	zero,23(s0)
    80600dba:	00000097          	auipc	ra,0x0
    80600dbe:	282080e7          	jalr	642(ra) # 8060103c <uart_puts>
    80600dc2:	00002517          	auipc	a0,0x2
    80600dc6:	dc650513          	addi	a0,a0,-570 # 80602b88 <kexec+0xc2>
    80600dca:	00000097          	auipc	ra,0x0
    80600dce:	272080e7          	jalr	626(ra) # 8060103c <uart_puts>
    80600dd2:	00000097          	auipc	ra,0x0
    80600dd6:	a32080e7          	jalr	-1486(ra) # 80600804 <wake_hart.constprop.0>
    80600dda:	00002517          	auipc	a0,0x2
    80600dde:	dd650513          	addi	a0,a0,-554 # 80602bb0 <kexec+0xea>
    80600de2:	00000097          	auipc	ra,0x0
    80600de6:	25a080e7          	jalr	602(ra) # 8060103c <uart_puts>
    80600dea:	00000097          	auipc	ra,0x0
    80600dee:	39e080e7          	jalr	926(ra) # 80601188 <start_user_process>
    80600df2:	00002517          	auipc	a0,0x2
    80600df6:	de650513          	addi	a0,a0,-538 # 80602bd8 <kexec+0x112>
    80600dfa:	00000097          	auipc	ra,0x0
    80600dfe:	242080e7          	jalr	578(ra) # 8060103c <uart_puts>
    80600e02:	fffff097          	auipc	ra,0xfffff
    80600e06:	4e6080e7          	jalr	1254(ra) # 806002e8 <sched_start>

0000000080600e0a <mm_map_megapage>:
    80600e0a:	7139                	addi	sp,sp,-64
    80600e0c:	fc06                	sd	ra,56(sp)
    80600e0e:	02b51793          	slli	a5,a0,0x2b
    80600e12:	86aa                	mv	a3,a0
    80600e14:	12079663          	bnez	a5,80600f40 <mm_map_megapage+0x136>
    80600e18:	01e55893          	srli	a7,a0,0x1e
    80600e1c:	1ff8f893          	andi	a7,a7,511
    80600e20:	00004797          	auipc	a5,0x4
    80600e24:	1e078793          	addi	a5,a5,480 # 80605000 <g_root_pt>
    80600e28:	00389313          	slli	t1,a7,0x3
    80600e2c:	933e                	add	t1,t1,a5
    80600e2e:	00033703          	ld	a4,0(t1)
    80600e32:	00177793          	andi	a5,a4,1
    80600e36:	c785                	beqz	a5,80600e5e <mm_map_megapage+0x54>
    80600e38:	00a75793          	srli	a5,a4,0xa
    80600e3c:	07b2                	slli	a5,a5,0xc
    80600e3e:	cfe1                	beqz	a5,80600f16 <mm_map_megapage+0x10c>
    80600e40:	82d5                	srli	a3,a3,0x15
    80600e42:	81b1                	srli	a1,a1,0xc
    80600e44:	1ff6f693          	andi	a3,a3,511
    80600e48:	05aa                	slli	a1,a1,0xa
    80600e4a:	068e                	slli	a3,a3,0x3
    80600e4c:	8dd1                	or	a1,a1,a2
    80600e4e:	96be                	add	a3,a3,a5
    80600e50:	0c15e593          	ori	a1,a1,193
    80600e54:	e28c                	sd	a1,0(a3)
    80600e56:	4501                	li	a0,0
    80600e58:	70e2                	ld	ra,56(sp)
    80600e5a:	6121                	addi	sp,sp,64
    80600e5c:	8082                	ret
    80600e5e:	f432                	sd	a2,40(sp)
    80600e60:	f02e                	sd	a1,32(sp)
    80600e62:	e42a                	sd	a0,8(sp)
    80600e64:	ec1a                	sd	t1,24(sp)
    80600e66:	e846                	sd	a7,16(sp)
    80600e68:	fffff097          	auipc	ra,0xfffff
    80600e6c:	798080e7          	jalr	1944(ra) # 80600600 <pmm_alloc_page>
    80600e70:	66a2                	ld	a3,8(sp)
    80600e72:	882a                	mv	a6,a0
    80600e74:	c175                	beqz	a0,80600f58 <mm_map_megapage+0x14e>
    80600e76:	03451793          	slli	a5,a0,0x34
    80600e7a:	ebe9                	bnez	a5,80600f4c <mm_map_megapage+0x142>
    80600e7c:	68c2                	ld	a7,16(sp)
    80600e7e:	6362                	ld	t1,24(sp)
    80600e80:	7582                	ld	a1,32(sp)
    80600e82:	7622                	ld	a2,40(sp)
    80600e84:	6705                	lui	a4,0x1
    80600e86:	972a                	add	a4,a4,a0
    80600e88:	87aa                	mv	a5,a0
    80600e8a:	0007b023          	sd	zero,0(a5)
    80600e8e:	07a1                	addi	a5,a5,8
    80600e90:	fef71de3          	bne	a4,a5,80600e8a <mm_map_megapage+0x80>
    80600e94:	00c85793          	srli	a5,a6,0xc
    80600e98:	07aa                	slli	a5,a5,0xa
    80600e9a:	0017e793          	ori	a5,a5,1
    80600e9e:	00f33023          	sd	a5,0(t1)
    80600ea2:	00002517          	auipc	a0,0x2
    80600ea6:	e5650513          	addi	a0,a0,-426 # 80602cf8 <kexec+0x232>
    80600eaa:	f432                	sd	a2,40(sp)
    80600eac:	f02e                	sd	a1,32(sp)
    80600eae:	ec36                	sd	a3,24(sp)
    80600eb0:	e442                	sd	a6,8(sp)
    80600eb2:	e846                	sd	a7,16(sp)
    80600eb4:	00000097          	auipc	ra,0x0
    80600eb8:	188080e7          	jalr	392(ra) # 8060103c <uart_puts>
    80600ebc:	03200513          	li	a0,50
    80600ec0:	00000097          	auipc	ra,0x0
    80600ec4:	158080e7          	jalr	344(ra) # 80601018 <uart_putc>
    80600ec8:	00002517          	auipc	a0,0x2
    80600ecc:	e3850513          	addi	a0,a0,-456 # 80602d00 <kexec+0x23a>
    80600ed0:	00000097          	auipc	ra,0x0
    80600ed4:	16c080e7          	jalr	364(ra) # 8060103c <uart_puts>
    80600ed8:	6542                	ld	a0,16(sp)
    80600eda:	00000097          	auipc	ra,0x0
    80600ede:	194080e7          	jalr	404(ra) # 8060106e <uart_hex>
    80600ee2:	00002517          	auipc	a0,0x2
    80600ee6:	e2e50513          	addi	a0,a0,-466 # 80602d10 <kexec+0x24a>
    80600eea:	00000097          	auipc	ra,0x0
    80600eee:	152080e7          	jalr	338(ra) # 8060103c <uart_puts>
    80600ef2:	6522                	ld	a0,8(sp)
    80600ef4:	00000097          	auipc	ra,0x0
    80600ef8:	17a080e7          	jalr	378(ra) # 8060106e <uart_hex>
    80600efc:	4529                	li	a0,10
    80600efe:	00000097          	auipc	ra,0x0
    80600f02:	11a080e7          	jalr	282(ra) # 80601018 <uart_putc>
    80600f06:	6822                	ld	a6,8(sp)
    80600f08:	77fd                	lui	a5,0xfffff
    80600f0a:	66e2                	ld	a3,24(sp)
    80600f0c:	7582                	ld	a1,32(sp)
    80600f0e:	7622                	ld	a2,40(sp)
    80600f10:	00f877b3          	and	a5,a6,a5
    80600f14:	b735                	j	80600e40 <mm_map_megapage+0x36>
    80600f16:	e43a                	sd	a4,8(sp)
    80600f18:	00002517          	auipc	a0,0x2
    80600f1c:	e6850513          	addi	a0,a0,-408 # 80602d80 <kexec+0x2ba>
    80600f20:	00000097          	auipc	ra,0x0
    80600f24:	11c080e7          	jalr	284(ra) # 8060103c <uart_puts>
    80600f28:	6522                	ld	a0,8(sp)
    80600f2a:	00000097          	auipc	ra,0x0
    80600f2e:	144080e7          	jalr	324(ra) # 8060106e <uart_hex>
    80600f32:	4529                	li	a0,10
    80600f34:	00000097          	auipc	ra,0x0
    80600f38:	0e4080e7          	jalr	228(ra) # 80601018 <uart_putc>
    80600f3c:	557d                	li	a0,-1
    80600f3e:	bf29                	j	80600e58 <mm_map_megapage+0x4e>
    80600f40:	e42a                	sd	a0,8(sp)
    80600f42:	00002517          	auipc	a0,0x2
    80600f46:	dd650513          	addi	a0,a0,-554 # 80602d18 <kexec+0x252>
    80600f4a:	bfd9                	j	80600f20 <mm_map_megapage+0x116>
    80600f4c:	e42a                	sd	a0,8(sp)
    80600f4e:	00002517          	auipc	a0,0x2
    80600f52:	e0a50513          	addi	a0,a0,-502 # 80602d58 <kexec+0x292>
    80600f56:	b7e9                	j	80600f20 <mm_map_megapage+0x116>
    80600f58:	e436                	sd	a3,8(sp)
    80600f5a:	00002517          	auipc	a0,0x2
    80600f5e:	dde50513          	addi	a0,a0,-546 # 80602d38 <kexec+0x272>
    80600f62:	bf7d                	j	80600f20 <mm_map_megapage+0x116>

0000000080600f64 <c_trap_handler>:
    80600f64:	1101                	addi	sp,sp,-32
    80600f66:	e822                	sd	s0,16(sp)
    80600f68:	842a                	mv	s0,a0
    80600f6a:	00002517          	auipc	a0,0x2
    80600f6e:	e3650513          	addi	a0,a0,-458 # 80602da0 <kexec+0x2da>
    80600f72:	ec06                	sd	ra,24(sp)
    80600f74:	e426                	sd	s1,8(sp)
    80600f76:	e04a                	sd	s2,0(sp)
    80600f78:	84b2                	mv	s1,a2
    80600f7a:	892e                	mv	s2,a1
    80600f7c:	00000097          	auipc	ra,0x0
    80600f80:	0c0080e7          	jalr	192(ra) # 8060103c <uart_puts>
    80600f84:	8522                	mv	a0,s0
    80600f86:	00000097          	auipc	ra,0x0
    80600f8a:	0e8080e7          	jalr	232(ra) # 8060106e <uart_hex>
    80600f8e:	00002517          	auipc	a0,0x2
    80600f92:	e2250513          	addi	a0,a0,-478 # 80602db0 <kexec+0x2ea>
    80600f96:	00000097          	auipc	ra,0x0
    80600f9a:	0a6080e7          	jalr	166(ra) # 8060103c <uart_puts>
    80600f9e:	854a                	mv	a0,s2
    80600fa0:	00000097          	auipc	ra,0x0
    80600fa4:	0ce080e7          	jalr	206(ra) # 8060106e <uart_hex>
    80600fa8:	00002517          	auipc	a0,0x2
    80600fac:	e1050513          	addi	a0,a0,-496 # 80602db8 <kexec+0x2f2>
    80600fb0:	00000097          	auipc	ra,0x0
    80600fb4:	08c080e7          	jalr	140(ra) # 8060103c <uart_puts>
    80600fb8:	8526                	mv	a0,s1
    80600fba:	00000097          	auipc	ra,0x0
    80600fbe:	0b4080e7          	jalr	180(ra) # 8060106e <uart_hex>
    80600fc2:	4529                	li	a0,10
    80600fc4:	00000097          	auipc	ra,0x0
    80600fc8:	054080e7          	jalr	84(ra) # 80601018 <uart_putc>
    80600fcc:	02044d63          	bltz	s0,80601006 <c_trap_handler+0xa2>
    80600fd0:	00002517          	auipc	a0,0x2
    80600fd4:	df050513          	addi	a0,a0,-528 # 80602dc0 <kexec+0x2fa>
    80600fd8:	00000097          	auipc	ra,0x0
    80600fdc:	064080e7          	jalr	100(ra) # 8060103c <uart_puts>
    80600fe0:	00f47793          	andi	a5,s0,15
    80600fe4:	00002717          	auipc	a4,0x2
    80600fe8:	1b470713          	addi	a4,a4,436 # 80603198 <exc_names>
    80600fec:	078e                	slli	a5,a5,0x3
    80600fee:	97ba                	add	a5,a5,a4
    80600ff0:	6388                	ld	a0,0(a5)
    80600ff2:	00000097          	auipc	ra,0x0
    80600ff6:	04a080e7          	jalr	74(ra) # 8060103c <uart_puts>
    80600ffa:	4529                	li	a0,10
    80600ffc:	00000097          	auipc	ra,0x0
    80601000:	01c080e7          	jalr	28(ra) # 80601018 <uart_putc>
    80601004:	a001                	j	80601004 <c_trap_handler+0xa0>
    80601006:	00002517          	auipc	a0,0x2
    8060100a:	dd250513          	addi	a0,a0,-558 # 80602dd8 <kexec+0x312>
    8060100e:	00000097          	auipc	ra,0x0
    80601012:	02e080e7          	jalr	46(ra) # 8060103c <uart_puts>
    80601016:	b7fd                	j	80601004 <c_trap_handler+0xa0>

0000000080601018 <uart_putc>:
    80601018:	10000737          	lui	a4,0x10000
    8060101c:	0715                	addi	a4,a4,5 # 10000005 <DRAM_SIZE+0x8000005>
    8060101e:	10000637          	lui	a2,0x10000
    80601022:	46a9                	li	a3,10
    80601024:	00074783          	lbu	a5,0(a4)
    80601028:	0207f793          	andi	a5,a5,32
    8060102c:	dfe5                	beqz	a5,80601024 <uart_putc+0xc>
    8060102e:	00a60023          	sb	a0,0(a2) # 10000000 <DRAM_SIZE+0x8000000>
    80601032:	00d51463          	bne	a0,a3,8060103a <uart_putc+0x22>
    80601036:	4535                	li	a0,13
    80601038:	b7f5                	j	80601024 <uart_putc+0xc>
    8060103a:	8082                	ret

000000008060103c <uart_puts>:
    8060103c:	00054683          	lbu	a3,0(a0)
    80601040:	c695                	beqz	a3,8060106c <uart_puts+0x30>
    80601042:	10000737          	lui	a4,0x10000
    80601046:	0715                	addi	a4,a4,5 # 10000005 <DRAM_SIZE+0x8000005>
    80601048:	100005b7          	lui	a1,0x10000
    8060104c:	4629                	li	a2,10
    8060104e:	00074783          	lbu	a5,0(a4)
    80601052:	0207f793          	andi	a5,a5,32
    80601056:	dfe5                	beqz	a5,8060104e <uart_puts+0x12>
    80601058:	00d58023          	sb	a3,0(a1) # 10000000 <DRAM_SIZE+0x8000000>
    8060105c:	00c69463          	bne	a3,a2,80601064 <uart_puts+0x28>
    80601060:	46b5                	li	a3,13
    80601062:	b7f5                	j	8060104e <uart_puts+0x12>
    80601064:	00154683          	lbu	a3,1(a0)
    80601068:	0505                	addi	a0,a0,1
    8060106a:	f2f5                	bnez	a3,8060104e <uart_puts+0x12>
    8060106c:	8082                	ret

000000008060106e <uart_hex>:
    8060106e:	10000737          	lui	a4,0x10000
    80601072:	0715                	addi	a4,a4,5 # 10000005 <DRAM_SIZE+0x8000005>
    80601074:	100006b7          	lui	a3,0x10000
    80601078:	00074783          	lbu	a5,0(a4)
    8060107c:	0207f793          	andi	a5,a5,32
    80601080:	dfe5                	beqz	a5,80601078 <uart_hex+0xa>
    80601082:	03000793          	li	a5,48
    80601086:	10000737          	lui	a4,0x10000
    8060108a:	00f68023          	sb	a5,0(a3) # 10000000 <DRAM_SIZE+0x8000000>
    8060108e:	0715                	addi	a4,a4,5 # 10000005 <DRAM_SIZE+0x8000005>
    80601090:	100006b7          	lui	a3,0x10000
    80601094:	00074783          	lbu	a5,0(a4)
    80601098:	0207f793          	andi	a5,a5,32
    8060109c:	dfe5                	beqz	a5,80601094 <uart_hex+0x26>
    8060109e:	07800793          	li	a5,120
    806010a2:	10000737          	lui	a4,0x10000
    806010a6:	00f68023          	sb	a5,0(a3) # 10000000 <DRAM_SIZE+0x8000000>
    806010aa:	0715                	addi	a4,a4,5 # 10000005 <DRAM_SIZE+0x8000005>
    806010ac:	03c00813          	li	a6,60
    806010b0:	00002317          	auipc	t1,0x2
    806010b4:	16830313          	addi	t1,t1,360 # 80603218 <d.0>
    806010b8:	100005b7          	lui	a1,0x10000
    806010bc:	4629                	li	a2,10
    806010be:	58f1                	li	a7,-4
    806010c0:	010557b3          	srl	a5,a0,a6
    806010c4:	8bbd                	andi	a5,a5,15
    806010c6:	979a                	add	a5,a5,t1
    806010c8:	0007c683          	lbu	a3,0(a5) # fffffffffffff000 <SATP_MODE_SV39+0x7ffffffffffff000>
    806010cc:	00074783          	lbu	a5,0(a4)
    806010d0:	0207f793          	andi	a5,a5,32
    806010d4:	dfe5                	beqz	a5,806010cc <uart_hex+0x5e>
    806010d6:	00d58023          	sb	a3,0(a1) # 10000000 <DRAM_SIZE+0x8000000>
    806010da:	00c69463          	bne	a3,a2,806010e2 <uart_hex+0x74>
    806010de:	46b5                	li	a3,13
    806010e0:	b7f5                	j	806010cc <uart_hex+0x5e>
    806010e2:	3871                	addiw	a6,a6,-4
    806010e4:	fd181ee3          	bne	a6,a7,806010c0 <uart_hex+0x52>
    806010e8:	8082                	ret

00000000806010ea <map_user_page>:
    806010ea:	715d                	addi	sp,sp,-80
    806010ec:	f84a                	sd	s2,48(sp)
    806010ee:	4909                	li	s2,2
    806010f0:	fc26                	sd	s1,56(sp)
    806010f2:	f44e                	sd	s3,40(sp)
    806010f4:	f052                	sd	s4,32(sp)
    806010f6:	ec56                	sd	s5,24(sp)
    806010f8:	e486                	sd	ra,72(sp)
    806010fa:	e0a2                	sd	s0,64(sp)
    806010fc:	84ae                	mv	s1,a1
    806010fe:	8a32                	mv	s4,a2
    80601100:	8ab6                	mv	s5,a3
    80601102:	874a                	mv	a4,s2
    80601104:	4985                	li	s3,1
    80601106:	0037179b          	slliw	a5,a4,0x3
    8060110a:	9fb9                	addw	a5,a5,a4
    8060110c:	27b1                	addiw	a5,a5,12
    8060110e:	00f4d7b3          	srl	a5,s1,a5
    80601112:	1ff7f793          	andi	a5,a5,511
    80601116:	078e                	slli	a5,a5,0x3
    80601118:	00f50433          	add	s0,a0,a5
    8060111c:	601c                	ld	a5,0(s0)
    8060111e:	0017f713          	andi	a4,a5,1
    80601122:	cf15                	beqz	a4,8060115e <map_user_page+0x74>
    80601124:	83a9                	srli	a5,a5,0xa
    80601126:	00c79513          	slli	a0,a5,0xc
    8060112a:	4705                	li	a4,1
    8060112c:	03391763          	bne	s2,s3,8060115a <map_user_page+0x70>
    80601130:	80b1                	srli	s1,s1,0xc
    80601132:	1ff4f493          	andi	s1,s1,511
    80601136:	00ca5793          	srli	a5,s4,0xc
    8060113a:	048e                	slli	s1,s1,0x3
    8060113c:	07aa                	slli	a5,a5,0xa
    8060113e:	9526                	add	a0,a0,s1
    80601140:	0157e7b3          	or	a5,a5,s5
    80601144:	e11c                	sd	a5,0(a0)
    80601146:	4501                	li	a0,0
    80601148:	60a6                	ld	ra,72(sp)
    8060114a:	6406                	ld	s0,64(sp)
    8060114c:	74e2                	ld	s1,56(sp)
    8060114e:	7942                	ld	s2,48(sp)
    80601150:	79a2                	ld	s3,40(sp)
    80601152:	7a02                	ld	s4,32(sp)
    80601154:	6ae2                	ld	s5,24(sp)
    80601156:	6161                	addi	sp,sp,80
    80601158:	8082                	ret
    8060115a:	893a                	mv	s2,a4
    8060115c:	b76d                	j	80601106 <map_user_page+0x1c>
    8060115e:	fffff097          	auipc	ra,0xfffff
    80601162:	4a2080e7          	jalr	1186(ra) # 80600600 <pmm_alloc_page>
    80601166:	6605                	lui	a2,0x1
    80601168:	4581                	li	a1,0
    8060116a:	cd09                	beqz	a0,80601184 <map_user_page+0x9a>
    8060116c:	e42a                	sd	a0,8(sp)
    8060116e:	00001097          	auipc	ra,0x1
    80601172:	792080e7          	jalr	1938(ra) # 80602900 <memset>
    80601176:	67a2                	ld	a5,8(sp)
    80601178:	83b1                	srli	a5,a5,0xc
    8060117a:	07aa                	slli	a5,a5,0xa
    8060117c:	0017e793          	ori	a5,a5,1
    80601180:	e01c                	sd	a5,0(s0)
    80601182:	b74d                	j	80601124 <map_user_page+0x3a>
    80601184:	557d                	li	a0,-1
    80601186:	b7c9                	j	80601148 <map_user_page+0x5e>

0000000080601188 <start_user_process>:
    80601188:	7179                	addi	sp,sp,-48
    8060118a:	00002517          	auipc	a0,0x2
    8060118e:	d7e50513          	addi	a0,a0,-642 # 80602f08 <kexec+0x442>
    80601192:	f406                	sd	ra,40(sp)
    80601194:	00000097          	auipc	ra,0x0
    80601198:	ea8080e7          	jalr	-344(ra) # 8060103c <uart_puts>
    8060119c:	fffff097          	auipc	ra,0xfffff
    806011a0:	464080e7          	jalr	1124(ra) # 80600600 <pmm_alloc_page>
    806011a4:	20050463          	beqz	a0,806013ac <start_user_process+0x224>
    806011a8:	6605                	lui	a2,0x1
    806011aa:	4581                	li	a1,0
    806011ac:	f022                	sd	s0,32(sp)
    806011ae:	ec26                	sd	s1,24(sp)
    806011b0:	842a                	mv	s0,a0
    806011b2:	e84a                	sd	s2,16(sp)
    806011b4:	e44e                	sd	s3,8(sp)
    806011b6:	00001097          	auipc	ra,0x1
    806011ba:	74a080e7          	jalr	1866(ra) # 80602900 <memset>
    806011be:	00004797          	auipc	a5,0x4
    806011c2:	e4278793          	addi	a5,a5,-446 # 80605000 <g_root_pt>
    806011c6:	40f405b3          	sub	a1,s0,a5
    806011ca:	00005617          	auipc	a2,0x5
    806011ce:	e3660613          	addi	a2,a2,-458 # 80606000 <g_l1_dram>
    806011d2:	6394                	ld	a3,0(a5)
    806011d4:	00f58733          	add	a4,a1,a5
    806011d8:	07a1                	addi	a5,a5,8
    806011da:	e314                	sd	a3,0(a4)
    806011dc:	fec79be3          	bne	a5,a2,806011d2 <start_user_process+0x4a>
    806011e0:	fffff097          	auipc	ra,0xfffff
    806011e4:	420080e7          	jalr	1056(ra) # 80600600 <pmm_alloc_page>
    806011e8:	892a                	mv	s2,a0
    806011ea:	fffff097          	auipc	ra,0xfffff
    806011ee:	416080e7          	jalr	1046(ra) # 80600600 <pmm_alloc_page>
    806011f2:	89aa                	mv	s3,a0
    806011f4:	fffff097          	auipc	ra,0xfffff
    806011f8:	40c080e7          	jalr	1036(ra) # 80600600 <pmm_alloc_page>
    806011fc:	0019b793          	seqz	a5,s3
    80601200:	00193713          	seqz	a4,s2
    80601204:	8f5d                	or	a4,a4,a5
    80601206:	00153793          	seqz	a5,a0
    8060120a:	8fd9                	or	a5,a5,a4
    8060120c:	84aa                	mv	s1,a0
    8060120e:	cbbd                	beqz	a5,80601284 <start_user_process+0xfc>
    80601210:	00002517          	auipc	a0,0x2
    80601214:	d4050513          	addi	a0,a0,-704 # 80602f50 <kexec+0x48a>
    80601218:	00000097          	auipc	ra,0x0
    8060121c:	e24080e7          	jalr	-476(ra) # 8060103c <uart_puts>
    80601220:	00002517          	auipc	a0,0x2
    80601224:	d5850513          	addi	a0,a0,-680 # 80602f78 <kexec+0x4b2>
    80601228:	00000097          	auipc	ra,0x0
    8060122c:	e14080e7          	jalr	-492(ra) # 8060103c <uart_puts>
    80601230:	854a                	mv	a0,s2
    80601232:	00000097          	auipc	ra,0x0
    80601236:	e3c080e7          	jalr	-452(ra) # 8060106e <uart_hex>
    8060123a:	00002517          	auipc	a0,0x2
    8060123e:	d4e50513          	addi	a0,a0,-690 # 80602f88 <kexec+0x4c2>
    80601242:	00000097          	auipc	ra,0x0
    80601246:	dfa080e7          	jalr	-518(ra) # 8060103c <uart_puts>
    8060124a:	854e                	mv	a0,s3
    8060124c:	00000097          	auipc	ra,0x0
    80601250:	e22080e7          	jalr	-478(ra) # 8060106e <uart_hex>
    80601254:	00002517          	auipc	a0,0x2
    80601258:	d3c50513          	addi	a0,a0,-708 # 80602f90 <kexec+0x4ca>
    8060125c:	00000097          	auipc	ra,0x0
    80601260:	de0080e7          	jalr	-544(ra) # 8060103c <uart_puts>
    80601264:	8526                	mv	a0,s1
    80601266:	00000097          	auipc	ra,0x0
    8060126a:	e08080e7          	jalr	-504(ra) # 8060106e <uart_hex>
    8060126e:	7402                	ld	s0,32(sp)
    80601270:	64e2                	ld	s1,24(sp)
    80601272:	6942                	ld	s2,16(sp)
    80601274:	69a2                	ld	s3,8(sp)
    80601276:	70a2                	ld	ra,40(sp)
    80601278:	4529                	li	a0,10
    8060127a:	6145                	addi	sp,sp,48
    8060127c:	00000317          	auipc	t1,0x0
    80601280:	d9c30067          	jr	-612(t1) # 80601018 <uart_putc>
    80601284:	6605                	lui	a2,0x1
    80601286:	4581                	li	a1,0
    80601288:	854a                	mv	a0,s2
    8060128a:	00001097          	auipc	ra,0x1
    8060128e:	676080e7          	jalr	1654(ra) # 80602900 <memset>
    80601292:	6605                	lui	a2,0x1
    80601294:	4581                	li	a1,0
    80601296:	854e                	mv	a0,s3
    80601298:	00001097          	auipc	ra,0x1
    8060129c:	668080e7          	jalr	1640(ra) # 80602900 <memset>
    806012a0:	6605                	lui	a2,0x1
    806012a2:	8526                	mv	a0,s1
    806012a4:	4581                	li	a1,0
    806012a6:	00001097          	auipc	ra,0x1
    806012aa:	65a080e7          	jalr	1626(ra) # 80602900 <memset>
    806012ae:	00002797          	auipc	a5,0x2
    806012b2:	fd278793          	addi	a5,a5,-46 # 80603280 <_binary_ZFC_kernel_init_build_user_test_bin_start>
    806012b6:	00002617          	auipc	a2,0x2
    806012ba:	fd260613          	addi	a2,a2,-46 # 80603288 <first.1>
    806012be:	8e1d                	sub	a2,a2,a5
    806012c0:	6785                	lui	a5,0x1
    806012c2:	0ec7ef63          	bltu	a5,a2,806013c0 <start_user_process+0x238>
    806012c6:	2601                	sext.w	a2,a2
    806012c8:	00002597          	auipc	a1,0x2
    806012cc:	fb858593          	addi	a1,a1,-72 # 80603280 <_binary_ZFC_kernel_init_build_user_test_bin_start>
    806012d0:	854a                	mv	a0,s2
    806012d2:	00001097          	auipc	ra,0x1
    806012d6:	6ba080e7          	jalr	1722(ra) # 8060298c <memcpy>
    806012da:	864a                	mv	a2,s2
    806012dc:	8522                	mv	a0,s0
    806012de:	0db00693          	li	a3,219
    806012e2:	400005b7          	lui	a1,0x40000
    806012e6:	00000097          	auipc	ra,0x0
    806012ea:	e04080e7          	jalr	-508(ra) # 806010ea <map_user_page>
    806012ee:	57fd                	li	a5,-1
    806012f0:	0cf50a63          	beq	a0,a5,806013c4 <start_user_process+0x23c>
    806012f4:	864e                	mv	a2,s3
    806012f6:	0d700693          	li	a3,215
    806012fa:	400015b7          	lui	a1,0x40001
    806012fe:	8522                	mv	a0,s0
    80601300:	00000097          	auipc	ra,0x0
    80601304:	dea080e7          	jalr	-534(ra) # 806010ea <map_user_page>
    80601308:	57fd                	li	a5,-1
    8060130a:	0af50d63          	beq	a0,a5,806013c4 <start_user_process+0x23c>
    8060130e:	400007b7          	lui	a5,0x40000
    80601312:	40002737          	lui	a4,0x40002
    80601316:	ec9c                	sd	a5,24(s1)
    80601318:	f898                	sd	a4,48(s1)
    8060131a:	00002517          	auipc	a0,0x2
    8060131e:	ca650513          	addi	a0,a0,-858 # 80602fc0 <kexec+0x4fa>
    80601322:	00000097          	auipc	ra,0x0
    80601326:	d1a080e7          	jalr	-742(ra) # 8060103c <uart_puts>
    8060132a:	40000537          	lui	a0,0x40000
    8060132e:	00000097          	auipc	ra,0x0
    80601332:	d40080e7          	jalr	-704(ra) # 8060106e <uart_hex>
    80601336:	00002517          	auipc	a0,0x2
    8060133a:	c9a50513          	addi	a0,a0,-870 # 80602fd0 <kexec+0x50a>
    8060133e:	00000097          	auipc	ra,0x0
    80601342:	cfe080e7          	jalr	-770(ra) # 8060103c <uart_puts>
    80601346:	7888                	ld	a0,48(s1)
    80601348:	00000097          	auipc	ra,0x0
    8060134c:	d26080e7          	jalr	-730(ra) # 8060106e <uart_hex>
    80601350:	00002517          	auipc	a0,0x2
    80601354:	c9050513          	addi	a0,a0,-880 # 80602fe0 <kexec+0x51a>
    80601358:	00000097          	auipc	ra,0x0
    8060135c:	ce4080e7          	jalr	-796(ra) # 8060103c <uart_puts>
    80601360:	8522                	mv	a0,s0
    80601362:	00000097          	auipc	ra,0x0
    80601366:	d0c080e7          	jalr	-756(ra) # 8060106e <uart_hex>
    8060136a:	4529                	li	a0,10
    8060136c:	00000097          	auipc	ra,0x0
    80601370:	cac080e7          	jalr	-852(ra) # 80601018 <uart_putc>
    80601374:	00002517          	auipc	a0,0x2
    80601378:	c7450513          	addi	a0,a0,-908 # 80602fe8 <kexec+0x522>
    8060137c:	00000097          	auipc	ra,0x0
    80601380:	cc0080e7          	jalr	-832(ra) # 8060103c <uart_puts>
    80601384:	8526                	mv	a0,s1
    80601386:	85a2                	mv	a1,s0
    80601388:	00000097          	auipc	ra,0x0
    8060138c:	058080e7          	jalr	88(ra) # 806013e0 <user_enter>
    80601390:	7402                	ld	s0,32(sp)
    80601392:	64e2                	ld	s1,24(sp)
    80601394:	6942                	ld	s2,16(sp)
    80601396:	69a2                	ld	s3,8(sp)
    80601398:	70a2                	ld	ra,40(sp)
    8060139a:	00002517          	auipc	a0,0x2
    8060139e:	c7650513          	addi	a0,a0,-906 # 80603010 <kexec+0x54a>
    806013a2:	6145                	addi	sp,sp,48
    806013a4:	00000317          	auipc	t1,0x0
    806013a8:	c9830067          	jr	-872(t1) # 8060103c <uart_puts>
    806013ac:	70a2                	ld	ra,40(sp)
    806013ae:	00002517          	auipc	a0,0x2
    806013b2:	b7a50513          	addi	a0,a0,-1158 # 80602f28 <kexec+0x462>
    806013b6:	6145                	addi	sp,sp,48
    806013b8:	00000317          	auipc	t1,0x0
    806013bc:	c8430067          	jr	-892(t1) # 8060103c <uart_puts>
    806013c0:	863e                	mv	a2,a5
    806013c2:	b711                	j	806012c6 <start_user_process+0x13e>
    806013c4:	7402                	ld	s0,32(sp)
    806013c6:	64e2                	ld	s1,24(sp)
    806013c8:	6942                	ld	s2,16(sp)
    806013ca:	69a2                	ld	s3,8(sp)
    806013cc:	70a2                	ld	ra,40(sp)
    806013ce:	00002517          	auipc	a0,0x2
    806013d2:	bca50513          	addi	a0,a0,-1078 # 80602f98 <kexec+0x4d2>
    806013d6:	6145                	addi	sp,sp,48
    806013d8:	00000317          	auipc	t1,0x0
    806013dc:	c6430067          	jr	-924(t1) # 8060103c <uart_puts>

00000000806013e0 <user_enter>:
    806013e0:	01853283          	ld	t0,24(a0)
    806013e4:	14129073          	csrw	sepc,t0
    806013e8:	02853083          	ld	ra,40(a0)
    806013ec:	03053103          	ld	sp,48(a0)
    806013f0:	10000293          	li	t0,256
    806013f4:	1002b073          	csrc	sstatus,t0
    806013f8:	02000293          	li	t0,32
    806013fc:	1002a073          	csrs	sstatus,t0
    80601400:	42a1                	li	t0,8
    80601402:	12f2                	slli	t0,t0,0x3c
    80601404:	00c5d313          	srli	t1,a1,0xc
    80601408:	0062e2b3          	or	t0,t0,t1
    8060140c:	18029073          	csrw	satp,t0
    80601410:	12000073          	sfence.vma
    80601414:	10200073          	sret

0000000080601418 <swtch>:
    80601418:	00153023          	sd	ra,0(a0)
    8060141c:	00253423          	sd	sp,8(a0)
    80601420:	e900                	sd	s0,16(a0)
    80601422:	ed04                	sd	s1,24(a0)
    80601424:	03253023          	sd	s2,32(a0)
    80601428:	03353423          	sd	s3,40(a0)
    8060142c:	03453823          	sd	s4,48(a0)
    80601430:	03553c23          	sd	s5,56(a0)
    80601434:	05653023          	sd	s6,64(a0)
    80601438:	05753423          	sd	s7,72(a0)
    8060143c:	05853823          	sd	s8,80(a0)
    80601440:	05953c23          	sd	s9,88(a0)
    80601444:	07a53023          	sd	s10,96(a0)
    80601448:	07b53423          	sd	s11,104(a0)
    8060144c:	0005b083          	ld	ra,0(a1) # 40001000 <DRAM_SIZE+0x38001000>
    80601450:	0085b103          	ld	sp,8(a1)
    80601454:	6980                	ld	s0,16(a1)
    80601456:	6d84                	ld	s1,24(a1)
    80601458:	0205b903          	ld	s2,32(a1)
    8060145c:	0285b983          	ld	s3,40(a1)
    80601460:	0305ba03          	ld	s4,48(a1)
    80601464:	0385ba83          	ld	s5,56(a1)
    80601468:	0405bb03          	ld	s6,64(a1)
    8060146c:	0485bb83          	ld	s7,72(a1)
    80601470:	0505bc03          	ld	s8,80(a1)
    80601474:	0585bc83          	ld	s9,88(a1)
    80601478:	0605bd03          	ld	s10,96(a1)
    8060147c:	0685bd83          	ld	s11,104(a1)
    80601480:	8082                	ret

0000000080601482 <printint>:
    80601482:	7139                	addi	sp,sp,-64
    80601484:	03f55793          	srli	a5,a0,0x3f
    80601488:	fc06                	sd	ra,56(sp)
    8060148a:	f822                	sd	s0,48(sp)
    8060148c:	f426                	sd	s1,40(sp)
    8060148e:	8e7d                	and	a2,a2,a5
    80601490:	4881                	li	a7,0
    80601492:	e225                	bnez	a2,806014f2 <printint+0x70>
    80601494:	0024                	addi	s1,sp,8
    80601496:	8726                	mv	a4,s1
    80601498:	4681                	li	a3,0
    8060149a:	00002817          	auipc	a6,0x2
    8060149e:	d9680813          	addi	a6,a6,-618 # 80603230 <digits>
    806014a2:	02b577b3          	remu	a5,a0,a1
    806014a6:	862a                	mv	a2,a0
    806014a8:	8436                	mv	s0,a3
    806014aa:	0705                	addi	a4,a4,1 # 40002001 <DRAM_SIZE+0x38002001>
    806014ac:	2685                	addiw	a3,a3,1
    806014ae:	02b55533          	divu	a0,a0,a1
    806014b2:	97c2                	add	a5,a5,a6
    806014b4:	0007c783          	lbu	a5,0(a5) # 40000000 <DRAM_SIZE+0x38000000>
    806014b8:	fef70fa3          	sb	a5,-1(a4)
    806014bc:	feb673e3          	bgeu	a2,a1,806014a2 <printint+0x20>
    806014c0:	00088963          	beqz	a7,806014d2 <printint+0x50>
    806014c4:	00d107b3          	add	a5,sp,a3
    806014c8:	02d00713          	li	a4,45
    806014cc:	8436                	mv	s0,a3
    806014ce:	00e78423          	sb	a4,8(a5)
    806014d2:	9426                	add	s0,s0,s1
    806014d4:	14fd                	addi	s1,s1,-1
    806014d6:	00044503          	lbu	a0,0(s0)
    806014da:	147d                	addi	s0,s0,-1
    806014dc:	00001097          	auipc	ra,0x1
    806014e0:	568080e7          	jalr	1384(ra) # 80602a44 <consputc>
    806014e4:	fe9419e3          	bne	s0,s1,806014d6 <printint+0x54>
    806014e8:	70e2                	ld	ra,56(sp)
    806014ea:	7442                	ld	s0,48(sp)
    806014ec:	74a2                	ld	s1,40(sp)
    806014ee:	6121                	addi	sp,sp,64
    806014f0:	8082                	ret
    806014f2:	40a00533          	neg	a0,a0
    806014f6:	4885                	li	a7,1
    806014f8:	bf71                	j	80601494 <printint+0x12>

00000000806014fa <printk>:
    806014fa:	00026317          	auipc	t1,0x26
    806014fe:	3b232303          	lw	t1,946(t1) # 806278ac <panicking>
    80601502:	7135                	addi	sp,sp,-160
    80601504:	e4a6                	sd	s1,72(sp)
    80601506:	ec86                	sd	ra,88(sp)
    80601508:	f4ae                	sd	a1,104(sp)
    8060150a:	f8b2                	sd	a2,112(sp)
    8060150c:	fcb6                	sd	a3,120(sp)
    8060150e:	e13a                	sd	a4,128(sp)
    80601510:	e53e                	sd	a5,136(sp)
    80601512:	e942                	sd	a6,144(sp)
    80601514:	ed46                	sd	a7,152(sp)
    80601516:	84aa                	mv	s1,a0
    80601518:	1e030963          	beqz	t1,8060170a <printk+0x210>
    8060151c:	0004c503          	lbu	a0,0(s1)
    80601520:	10bc                	addi	a5,sp,104
    80601522:	e43e                	sd	a5,8(sp)
    80601524:	cd2d                	beqz	a0,8060159e <printk+0xa4>
    80601526:	e8a2                	sd	s0,80(sp)
    80601528:	fc4e                	sd	s3,56(sp)
    8060152a:	f852                	sd	s4,48(sp)
    8060152c:	f456                	sd	s5,40(sp)
    8060152e:	f05a                	sd	s6,32(sp)
    80601530:	e0ca                	sd	s2,64(sp)
    80601532:	ec5e                	sd	s7,24(sp)
    80601534:	02500993          	li	s3,37
    80601538:	4401                	li	s0,0
    8060153a:	06400a13          	li	s4,100
    8060153e:	07500a93          	li	s5,117
    80601542:	07800b13          	li	s6,120
    80601546:	0b351263          	bne	a0,s3,806015ea <printk+0xf0>
    8060154a:	0014091b          	addiw	s2,s0,1
    8060154e:	01248733          	add	a4,s1,s2
    80601552:	00074b83          	lbu	s7,0(a4)
    80601556:	040b8f63          	beqz	s7,806015b4 <printk+0xba>
    8060155a:	00174683          	lbu	a3,1(a4)
    8060155e:	ead9                	bnez	a3,806015f4 <printk+0xfa>
    80601560:	0f4b8863          	beq	s7,s4,80601650 <printk+0x156>
    80601564:	115b8263          	beq	s7,s5,80601668 <printk+0x16e>
    80601568:	136b8d63          	beq	s7,s6,806016a2 <printk+0x1a8>
    8060156c:	07000713          	li	a4,112
    80601570:	14eb8663          	beq	s7,a4,806016bc <printk+0x1c2>
    80601574:	06300713          	li	a4,99
    80601578:	1ceb8263          	beq	s7,a4,8060173c <printk+0x242>
    8060157c:	07300713          	li	a4,115
    80601580:	1ceb8863          	beq	s7,a4,80601750 <printk+0x256>
    80601584:	02500713          	li	a4,37
    80601588:	0aeb8d63          	beq	s7,a4,80601642 <printk+0x148>
    8060158c:	0a0b9563          	bnez	s7,80601636 <printk+0x13c>
    80601590:	6446                	ld	s0,80(sp)
    80601592:	6906                	ld	s2,64(sp)
    80601594:	79e2                	ld	s3,56(sp)
    80601596:	7a42                	ld	s4,48(sp)
    80601598:	7aa2                	ld	s5,40(sp)
    8060159a:	7b02                	ld	s6,32(sp)
    8060159c:	6be2                	ld	s7,24(sp)
    8060159e:	00026797          	auipc	a5,0x26
    806015a2:	30e7a783          	lw	a5,782(a5) # 806278ac <panicking>
    806015a6:	16078b63          	beqz	a5,8060171c <printk+0x222>
    806015aa:	60e6                	ld	ra,88(sp)
    806015ac:	64a6                	ld	s1,72(sp)
    806015ae:	4501                	li	a0,0
    806015b0:	610d                	addi	sp,sp,160
    806015b2:	8082                	ret
    806015b4:	4681                	li	a3,0
    806015b6:	4601                	li	a2,0
    806015b8:	f8868713          	addi	a4,a3,-120
    806015bc:	00173713          	seqz	a4,a4
    806015c0:	8e79                	and	a2,a2,a4
    806015c2:	d64d                	beqz	a2,8060156c <printk+0x72>
    806015c4:	67a2                	ld	a5,8(sp)
    806015c6:	4601                	li	a2,0
    806015c8:	45c1                	li	a1,16
    806015ca:	6388                	ld	a0,0(a5)
    806015cc:	07a1                	addi	a5,a5,8
    806015ce:	e43e                	sd	a5,8(sp)
    806015d0:	2409                	addiw	s0,s0,2
    806015d2:	00000097          	auipc	ra,0x0
    806015d6:	eb0080e7          	jalr	-336(ra) # 80601482 <printint>
    806015da:	2405                	addiw	s0,s0,1
    806015dc:	008487b3          	add	a5,s1,s0
    806015e0:	0007c503          	lbu	a0,0(a5)
    806015e4:	d555                	beqz	a0,80601590 <printk+0x96>
    806015e6:	f73502e3          	beq	a0,s3,8060154a <printk+0x50>
    806015ea:	00001097          	auipc	ra,0x1
    806015ee:	45a080e7          	jalr	1114(ra) # 80602a44 <consputc>
    806015f2:	b7e5                	j	806015da <printk+0xe0>
    806015f4:	054b8e63          	beq	s7,s4,80601650 <printk+0x156>
    806015f8:	f94b8613          	addi	a2,s7,-108
    806015fc:	f9c68593          	addi	a1,a3,-100
    80601600:	00163613          	seqz	a2,a2
    80601604:	0015b593          	seqz	a1,a1
    80601608:	8df1                	and	a1,a1,a2
    8060160a:	c589                	beqz	a1,80601614 <printk+0x11a>
    8060160c:	67a2                	ld	a5,8(sp)
    8060160e:	4605                	li	a2,1
    80601610:	45a9                	li	a1,10
    80601612:	bf65                	j	806015ca <printk+0xd0>
    80601614:	f9468593          	addi	a1,a3,-108
    80601618:	0015b593          	seqz	a1,a1
    8060161c:	8df1                	and	a1,a1,a2
    8060161e:	00274703          	lbu	a4,2(a4)
    80601622:	16058563          	beqz	a1,8060178c <printk+0x292>
    80601626:	05470e63          	beq	a4,s4,80601682 <printk+0x188>
    8060162a:	11570663          	beq	a4,s5,80601736 <printk+0x23c>
    8060162e:	07800693          	li	a3,120
    80601632:	14d70263          	beq	a4,a3,80601776 <printk+0x27c>
    80601636:	02500513          	li	a0,37
    8060163a:	00001097          	auipc	ra,0x1
    8060163e:	40a080e7          	jalr	1034(ra) # 80602a44 <consputc>
    80601642:	855e                	mv	a0,s7
    80601644:	00001097          	auipc	ra,0x1
    80601648:	400080e7          	jalr	1024(ra) # 80602a44 <consputc>
    8060164c:	844a                	mv	s0,s2
    8060164e:	b771                	j	806015da <printk+0xe0>
    80601650:	67a2                	ld	a5,8(sp)
    80601652:	4605                	li	a2,1
    80601654:	45a9                	li	a1,10
    80601656:	4388                	lw	a0,0(a5)
    80601658:	07a1                	addi	a5,a5,8
    8060165a:	e43e                	sd	a5,8(sp)
    8060165c:	844a                	mv	s0,s2
    8060165e:	00000097          	auipc	ra,0x0
    80601662:	e24080e7          	jalr	-476(ra) # 80601482 <printint>
    80601666:	bf95                	j	806015da <printk+0xe0>
    80601668:	67a2                	ld	a5,8(sp)
    8060166a:	4601                	li	a2,0
    8060166c:	45a9                	li	a1,10
    8060166e:	0007e503          	lwu	a0,0(a5)
    80601672:	07a1                	addi	a5,a5,8
    80601674:	e43e                	sd	a5,8(sp)
    80601676:	844a                	mv	s0,s2
    80601678:	00000097          	auipc	ra,0x0
    8060167c:	e0a080e7          	jalr	-502(ra) # 80601482 <printint>
    80601680:	bfa9                	j	806015da <printk+0xe0>
    80601682:	67a2                	ld	a5,8(sp)
    80601684:	4605                	li	a2,1
    80601686:	45a9                	li	a1,10
    80601688:	6388                	ld	a0,0(a5)
    8060168a:	07a1                	addi	a5,a5,8
    8060168c:	e43e                	sd	a5,8(sp)
    8060168e:	240d                	addiw	s0,s0,3
    80601690:	00000097          	auipc	ra,0x0
    80601694:	df2080e7          	jalr	-526(ra) # 80601482 <printint>
    80601698:	b789                	j	806015da <printk+0xe0>
    8060169a:	07800713          	li	a4,120
    8060169e:	f0eb9de3          	bne	s7,a4,806015b8 <printk+0xbe>
    806016a2:	67a2                	ld	a5,8(sp)
    806016a4:	4601                	li	a2,0
    806016a6:	45c1                	li	a1,16
    806016a8:	0007e503          	lwu	a0,0(a5)
    806016ac:	07a1                	addi	a5,a5,8
    806016ae:	e43e                	sd	a5,8(sp)
    806016b0:	844a                	mv	s0,s2
    806016b2:	00000097          	auipc	ra,0x0
    806016b6:	dd0080e7          	jalr	-560(ra) # 80601482 <printint>
    806016ba:	b705                	j	806015da <printk+0xe0>
    806016bc:	67a2                	ld	a5,8(sp)
    806016be:	03000513          	li	a0,48
    806016c2:	e862                	sd	s8,16(sp)
    806016c4:	00878713          	addi	a4,a5,8
    806016c8:	0007bb83          	ld	s7,0(a5)
    806016cc:	e43a                	sd	a4,8(sp)
    806016ce:	00001097          	auipc	ra,0x1
    806016d2:	376080e7          	jalr	886(ra) # 80602a44 <consputc>
    806016d6:	07800513          	li	a0,120
    806016da:	00001097          	auipc	ra,0x1
    806016de:	36a080e7          	jalr	874(ra) # 80602a44 <consputc>
    806016e2:	4441                	li	s0,16
    806016e4:	00002c17          	auipc	s8,0x2
    806016e8:	b4cc0c13          	addi	s8,s8,-1204 # 80603230 <digits>
    806016ec:	03cbd793          	srli	a5,s7,0x3c
    806016f0:	97e2                	add	a5,a5,s8
    806016f2:	0007c503          	lbu	a0,0(a5)
    806016f6:	347d                	addiw	s0,s0,-1
    806016f8:	0b92                	slli	s7,s7,0x4
    806016fa:	00001097          	auipc	ra,0x1
    806016fe:	34a080e7          	jalr	842(ra) # 80602a44 <consputc>
    80601702:	f46d                	bnez	s0,806016ec <printk+0x1f2>
    80601704:	6c42                	ld	s8,16(sp)
    80601706:	844a                	mv	s0,s2
    80601708:	bdc9                	j	806015da <printk+0xe0>
    8060170a:	0001f517          	auipc	a0,0x1f
    8060170e:	35650513          	addi	a0,a0,854 # 80620a60 <pr>
    80601712:	00001097          	auipc	ra,0x1
    80601716:	10a080e7          	jalr	266(ra) # 8060281c <acquire>
    8060171a:	b509                	j	8060151c <printk+0x22>
    8060171c:	0001f517          	auipc	a0,0x1f
    80601720:	34450513          	addi	a0,a0,836 # 80620a60 <pr>
    80601724:	00001097          	auipc	ra,0x1
    80601728:	196080e7          	jalr	406(ra) # 806028ba <release>
    8060172c:	60e6                	ld	ra,88(sp)
    8060172e:	64a6                	ld	s1,72(sp)
    80601730:	4501                	li	a0,0
    80601732:	610d                	addi	sp,sp,160
    80601734:	8082                	ret
    80601736:	67a2                	ld	a5,8(sp)
    80601738:	4601                	li	a2,0
    8060173a:	b7b1                	j	80601686 <printk+0x18c>
    8060173c:	67a2                	ld	a5,8(sp)
    8060173e:	844a                	mv	s0,s2
    80601740:	4388                	lw	a0,0(a5)
    80601742:	07a1                	addi	a5,a5,8
    80601744:	e43e                	sd	a5,8(sp)
    80601746:	00001097          	auipc	ra,0x1
    8060174a:	2fe080e7          	jalr	766(ra) # 80602a44 <consputc>
    8060174e:	b571                	j	806015da <printk+0xe0>
    80601750:	67a2                	ld	a5,8(sp)
    80601752:	6380                	ld	s0,0(a5)
    80601754:	07a1                	addi	a5,a5,8
    80601756:	e43e                	sd	a5,8(sp)
    80601758:	c01d                	beqz	s0,8060177e <printk+0x284>
    8060175a:	00044503          	lbu	a0,0(s0)
    8060175e:	ee0507e3          	beqz	a0,8060164c <printk+0x152>
    80601762:	00001097          	auipc	ra,0x1
    80601766:	2e2080e7          	jalr	738(ra) # 80602a44 <consputc>
    8060176a:	00144503          	lbu	a0,1(s0)
    8060176e:	0405                	addi	s0,s0,1
    80601770:	f96d                	bnez	a0,80601762 <printk+0x268>
    80601772:	844a                	mv	s0,s2
    80601774:	b59d                	j	806015da <printk+0xe0>
    80601776:	67a2                	ld	a5,8(sp)
    80601778:	4601                	li	a2,0
    8060177a:	45c1                	li	a1,16
    8060177c:	b731                	j	80601688 <printk+0x18e>
    8060177e:	02800513          	li	a0,40
    80601782:	00002417          	auipc	s0,0x2
    80601786:	8b640413          	addi	s0,s0,-1866 # 80603038 <kexec+0x572>
    8060178a:	bfe1                	j	80601762 <printk+0x268>
    8060178c:	07500713          	li	a4,117
    80601790:	eceb8ce3          	beq	s7,a4,80601668 <printk+0x16e>
    80601794:	f8b68713          	addi	a4,a3,-117
    80601798:	00173713          	seqz	a4,a4
    8060179c:	8f71                	and	a4,a4,a2
    8060179e:	ee070ee3          	beqz	a4,8060169a <printk+0x1a0>
    806017a2:	67a2                	ld	a5,8(sp)
    806017a4:	4601                	li	a2,0
    806017a6:	45a9                	li	a1,10
    806017a8:	b50d                	j	806015ca <printk+0xd0>

00000000806017aa <panic>:
    806017aa:	1101                	addi	sp,sp,-32
    806017ac:	e822                	sd	s0,16(sp)
    806017ae:	e426                	sd	s1,8(sp)
    806017b0:	4405                	li	s0,1
    806017b2:	84aa                	mv	s1,a0
    806017b4:	00002517          	auipc	a0,0x2
    806017b8:	88c50513          	addi	a0,a0,-1908 # 80603040 <kexec+0x57a>
    806017bc:	ec06                	sd	ra,24(sp)
    806017be:	00026797          	auipc	a5,0x26
    806017c2:	0e87a723          	sw	s0,238(a5) # 806278ac <panicking>
    806017c6:	00000097          	auipc	ra,0x0
    806017ca:	d34080e7          	jalr	-716(ra) # 806014fa <printk>
    806017ce:	85a6                	mv	a1,s1
    806017d0:	00002517          	auipc	a0,0x2
    806017d4:	87850513          	addi	a0,a0,-1928 # 80603048 <kexec+0x582>
    806017d8:	00000097          	auipc	ra,0x0
    806017dc:	d22080e7          	jalr	-734(ra) # 806014fa <printk>
    806017e0:	00026797          	auipc	a5,0x26
    806017e4:	0c87a423          	sw	s0,200(a5) # 806278a8 <panicked>
    806017e8:	a001                	j	806017e8 <panic+0x3e>

00000000806017ea <printkinit>:
    806017ea:	00002597          	auipc	a1,0x2
    806017ee:	86658593          	addi	a1,a1,-1946 # 80603050 <kexec+0x58a>
    806017f2:	0001f517          	auipc	a0,0x1f
    806017f6:	26e50513          	addi	a0,a0,622 # 80620a60 <pr>
    806017fa:	00001317          	auipc	t1,0x1
    806017fe:	fae30067          	jr	-82(t1) # 806027a8 <initlock>

0000000080601802 <freeproc>:
    80601802:	7179                	addi	sp,sp,-48
    80601804:	f022                	sd	s0,32(sp)
    80601806:	842a                	mv	s0,a0
    80601808:	6d28                	ld	a0,88(a0)
    8060180a:	f406                	sd	ra,40(sp)
    8060180c:	c509                	beqz	a0,80601816 <freeproc+0x14>
    8060180e:	00001097          	auipc	ra,0x1
    80601812:	22a080e7          	jalr	554(ra) # 80602a38 <kfree>
    80601816:	6828                	ld	a0,80(s0)
    80601818:	04043c23          	sd	zero,88(s0)
    8060181c:	c129                	beqz	a0,8060185e <freeproc+0x5c>
    8060181e:	643c                	ld	a5,72(s0)
    80601820:	040005b7          	lui	a1,0x4000
    80601824:	15fd                	addi	a1,a1,-1 # 3ffffff <MEGAPAGE_SIZE+0x3dfffff>
    80601826:	05b2                	slli	a1,a1,0xc
    80601828:	4681                	li	a3,0
    8060182a:	4605                	li	a2,1
    8060182c:	ec26                	sd	s1,24(sp)
    8060182e:	e42a                	sd	a0,8(sp)
    80601830:	84be                	mv	s1,a5
    80601832:	00001097          	auipc	ra,0x1
    80601836:	240080e7          	jalr	576(ra) # 80602a72 <uvmunmap>
    8060183a:	6522                	ld	a0,8(sp)
    8060183c:	020005b7          	lui	a1,0x2000
    80601840:	15fd                	addi	a1,a1,-1 # 1ffffff <MEGAPAGE_SIZE+0x1dfffff>
    80601842:	05b6                	slli	a1,a1,0xd
    80601844:	4681                	li	a3,0
    80601846:	4605                	li	a2,1
    80601848:	00001097          	auipc	ra,0x1
    8060184c:	22a080e7          	jalr	554(ra) # 80602a72 <uvmunmap>
    80601850:	6522                	ld	a0,8(sp)
    80601852:	85a6                	mv	a1,s1
    80601854:	00001097          	auipc	ra,0x1
    80601858:	20e080e7          	jalr	526(ra) # 80602a62 <uvmfree>
    8060185c:	64e2                	ld	s1,24(sp)
    8060185e:	14040c23          	sb	zero,344(s0)
    80601862:	70a2                	ld	ra,40(sp)
    80601864:	04043823          	sd	zero,80(s0)
    80601868:	04043423          	sd	zero,72(s0)
    8060186c:	02042823          	sw	zero,48(s0)
    80601870:	02043023          	sd	zero,32(s0)
    80601874:	02043423          	sd	zero,40(s0)
    80601878:	00042c23          	sw	zero,24(s0)
    8060187c:	7402                	ld	s0,32(sp)
    8060187e:	6145                	addi	sp,sp,48
    80601880:	8082                	ret

0000000080601882 <forkret>:
    80601882:	1101                	addi	sp,sp,-32
    80601884:	ec06                	sd	ra,24(sp)
    80601886:	e822                	sd	s0,16(sp)
    80601888:	00001097          	auipc	ra,0x1
    8060188c:	f54080e7          	jalr	-172(ra) # 806027dc <push_off>
    80601890:	8792                	mv	a5,tp
    80601892:	2781                	sext.w	a5,a5
    80601894:	0001f717          	auipc	a4,0x1f
    80601898:	1e470713          	addi	a4,a4,484 # 80620a78 <cpus>
    8060189c:	079e                	slli	a5,a5,0x7
    8060189e:	97ba                	add	a5,a5,a4
    806018a0:	6380                	ld	s0,0(a5)
    806018a2:	00001097          	auipc	ra,0x1
    806018a6:	fca080e7          	jalr	-54(ra) # 8060286c <pop_off>
    806018aa:	8522                	mv	a0,s0
    806018ac:	00001097          	auipc	ra,0x1
    806018b0:	00e080e7          	jalr	14(ra) # 806028ba <release>
    806018b4:	00002797          	auipc	a5,0x2
    806018b8:	9d47a783          	lw	a5,-1580(a5) # 80603288 <first.1>
    806018bc:	cb95                	beqz	a5,806018f0 <forkret+0x6e>
    806018be:	4505                	li	a0,1
    806018c0:	00002797          	auipc	a5,0x2
    806018c4:	9c07a423          	sw	zero,-1592(a5) # 80603288 <first.1>
    806018c8:	00001097          	auipc	ra,0x1
    806018cc:	1f0080e7          	jalr	496(ra) # 80602ab8 <fsinit>
    806018d0:	00001517          	auipc	a0,0x1
    806018d4:	78850513          	addi	a0,a0,1928 # 80603058 <kexec+0x592>
    806018d8:	858a                	mv	a1,sp
    806018da:	e402                	sd	zero,8(sp)
    806018dc:	e02a                	sd	a0,0(sp)
    806018de:	00001097          	auipc	ra,0x1
    806018e2:	1e8080e7          	jalr	488(ra) # 80602ac6 <kexec>
    806018e6:	6c38                	ld	a4,88(s0)
    806018e8:	57fd                	li	a5,-1
    806018ea:	fb28                	sd	a0,112(a4)
    806018ec:	02f50d63          	beq	a0,a5,80601926 <forkret+0xa4>
    806018f0:	00001097          	auipc	ra,0x1
    806018f4:	168080e7          	jalr	360(ra) # 80602a58 <prepare_return>
    806018f8:	6828                	ld	a0,80(s0)
    806018fa:	56fd                	li	a3,-1
    806018fc:	6442                	ld	s0,16(sp)
    806018fe:	04000737          	lui	a4,0x4000
    80601902:	60e2                	ld	ra,24(sp)
    80601904:	9736                	add	a4,a4,a3
    80601906:	00001797          	auipc	a5,0x1
    8060190a:	15078793          	addi	a5,a5,336 # 80602a56 <userret>
    8060190e:	00025617          	auipc	a2,0x25
    80601912:	f9a60613          	addi	a2,a2,-102 # 806268a8 <trampoline>
    80601916:	8131                	srli	a0,a0,0xc
    80601918:	16fe                	slli	a3,a3,0x3f
    8060191a:	0732                	slli	a4,a4,0xc
    8060191c:	8f91                	sub	a5,a5,a2
    8060191e:	8d55                	or	a0,a0,a3
    80601920:	97ba                	add	a5,a5,a4
    80601922:	6105                	addi	sp,sp,32
    80601924:	8782                	jr	a5
    80601926:	00001517          	auipc	a0,0x1
    8060192a:	73a50513          	addi	a0,a0,1850 # 80603060 <kexec+0x59a>
    8060192e:	00000097          	auipc	ra,0x0
    80601932:	e7c080e7          	jalr	-388(ra) # 806017aa <panic>

0000000080601936 <proc_mapstacks>:
    80601936:	7179                	addi	sp,sp,-48
    80601938:	000a57b7          	lui	a5,0xa5
    8060193c:	ec26                	sd	s1,24(sp)
    8060193e:	fa578793          	addi	a5,a5,-91 # a4fa5 <PMM_MAX_PAGES+0x9cfa5>
    80601942:	4fa504b7          	lui	s1,0x4fa50
    80601946:	07b2                	slli	a5,a5,0xc
    80601948:	a4f48493          	addi	s1,s1,-1457 # 4fa4fa4f <DRAM_SIZE+0x47a4fa4f>
    8060194c:	e84a                	sd	s2,16(sp)
    8060194e:	e44e                	sd	s3,8(sp)
    80601950:	fa578793          	addi	a5,a5,-91
    80601954:	1482                	slli	s1,s1,0x20
    80601956:	49c5                	li	s3,17
    80601958:	6919                	lui	s2,0x6
    8060195a:	f022                	sd	s0,32(sp)
    8060195c:	e052                	sd	s4,0(sp)
    8060195e:	f406                	sd	ra,40(sp)
    80601960:	94be                	add	s1,s1,a5
    80601962:	8a2a                	mv	s4,a0
    80601964:	09ee                	slli	s3,s3,0x1b
    80601966:	a0090913          	addi	s2,s2,-1536 # 5a00 <PAGE_SIZE+0x4a00>
    8060196a:	4401                	li	s0,0
    8060196c:	00001097          	auipc	ra,0x1
    80601970:	0c4080e7          	jalr	196(ra) # 80602a30 <kalloc>
    80601974:	862a                	mv	a2,a0
    80601976:	cd0d                	beqz	a0,806019b0 <proc_mapstacks+0x7a>
    80601978:	40345593          	srai	a1,s0,0x3
    8060197c:	029585b3          	mul	a1,a1,s1
    80601980:	6789                	lui	a5,0x2
    80601982:	4719                	li	a4,6
    80601984:	6685                	lui	a3,0x1
    80601986:	8552                	mv	a0,s4
    80601988:	16840413          	addi	s0,s0,360
    8060198c:	05b6                	slli	a1,a1,0xd
    8060198e:	9dbd                	addw	a1,a1,a5
    80601990:	40b985b3          	sub	a1,s3,a1
    80601994:	00001097          	auipc	ra,0x1
    80601998:	0bc080e7          	jalr	188(ra) # 80602a50 <kvmmap>
    8060199c:	fd2418e3          	bne	s0,s2,8060196c <proc_mapstacks+0x36>
    806019a0:	70a2                	ld	ra,40(sp)
    806019a2:	7402                	ld	s0,32(sp)
    806019a4:	64e2                	ld	s1,24(sp)
    806019a6:	6942                	ld	s2,16(sp)
    806019a8:	69a2                	ld	s3,8(sp)
    806019aa:	6a02                	ld	s4,0(sp)
    806019ac:	6145                	addi	sp,sp,48
    806019ae:	8082                	ret
    806019b0:	00001517          	auipc	a0,0x1
    806019b4:	6b850513          	addi	a0,a0,1720 # 80603068 <kexec+0x5a2>
    806019b8:	00000097          	auipc	ra,0x0
    806019bc:	df2080e7          	jalr	-526(ra) # 806017aa <panic>

00000000806019c0 <procinit>:
    806019c0:	7179                	addi	sp,sp,-48
    806019c2:	00001597          	auipc	a1,0x1
    806019c6:	6ae58593          	addi	a1,a1,1710 # 80603070 <kexec+0x5aa>
    806019ca:	0001f517          	auipc	a0,0x1f
    806019ce:	4ae50513          	addi	a0,a0,1198 # 80620e78 <pid_lock>
    806019d2:	f406                	sd	ra,40(sp)
    806019d4:	f022                	sd	s0,32(sp)
    806019d6:	ec26                	sd	s1,24(sp)
    806019d8:	e84a                	sd	s2,16(sp)
    806019da:	e44e                	sd	s3,8(sp)
    806019dc:	e052                	sd	s4,0(sp)
    806019de:	00001097          	auipc	ra,0x1
    806019e2:	dca080e7          	jalr	-566(ra) # 806027a8 <initlock>
    806019e6:	00001597          	auipc	a1,0x1
    806019ea:	69258593          	addi	a1,a1,1682 # 80603078 <kexec+0x5b2>
    806019ee:	0001f517          	auipc	a0,0x1f
    806019f2:	4a250513          	addi	a0,a0,1186 # 80620e90 <wait_lock>
    806019f6:	00001097          	auipc	ra,0x1
    806019fa:	db2080e7          	jalr	-590(ra) # 806027a8 <initlock>
    806019fe:	000a57b7          	lui	a5,0xa5
    80601a02:	fa578793          	addi	a5,a5,-91 # a4fa5 <PMM_MAX_PAGES+0x9cfa5>
    80601a06:	4fa50937          	lui	s2,0x4fa50
    80601a0a:	07b2                	slli	a5,a5,0xc
    80601a0c:	a4f90913          	addi	s2,s2,-1457 # 4fa4fa4f <DRAM_SIZE+0x47a4fa4f>
    80601a10:	fa578793          	addi	a5,a5,-91
    80601a14:	1902                	slli	s2,s2,0x20
    80601a16:	49c5                	li	s3,17
    80601a18:	993e                	add	s2,s2,a5
    80601a1a:	09ee                	slli	s3,s3,0x1b
    80601a1c:	4481                	li	s1,0
    80601a1e:	0001f417          	auipc	s0,0x1f
    80601a22:	48a40413          	addi	s0,s0,1162 # 80620ea8 <proc>
    80601a26:	00025a17          	auipc	s4,0x25
    80601a2a:	e82a0a13          	addi	s4,s4,-382 # 806268a8 <trampoline>
    80601a2e:	8522                	mv	a0,s0
    80601a30:	00001597          	auipc	a1,0x1
    80601a34:	65858593          	addi	a1,a1,1624 # 80603088 <kexec+0x5c2>
    80601a38:	00001097          	auipc	ra,0x1
    80601a3c:	d70080e7          	jalr	-656(ra) # 806027a8 <initlock>
    80601a40:	4034d793          	srai	a5,s1,0x3
    80601a44:	032787b3          	mul	a5,a5,s2
    80601a48:	6709                	lui	a4,0x2
    80601a4a:	00042c23          	sw	zero,24(s0)
    80601a4e:	16840413          	addi	s0,s0,360
    80601a52:	16848493          	addi	s1,s1,360
    80601a56:	07b6                	slli	a5,a5,0xd
    80601a58:	9fb9                	addw	a5,a5,a4
    80601a5a:	40f987b3          	sub	a5,s3,a5
    80601a5e:	ecf43c23          	sd	a5,-296(s0)
    80601a62:	fd4416e3          	bne	s0,s4,80601a2e <procinit+0x6e>
    80601a66:	70a2                	ld	ra,40(sp)
    80601a68:	7402                	ld	s0,32(sp)
    80601a6a:	64e2                	ld	s1,24(sp)
    80601a6c:	6942                	ld	s2,16(sp)
    80601a6e:	69a2                	ld	s3,8(sp)
    80601a70:	6a02                	ld	s4,0(sp)
    80601a72:	6145                	addi	sp,sp,48
    80601a74:	8082                	ret

0000000080601a76 <cpuid>:
    80601a76:	8512                	mv	a0,tp
    80601a78:	2501                	sext.w	a0,a0
    80601a7a:	8082                	ret

0000000080601a7c <mycpu>:
    80601a7c:	8792                	mv	a5,tp
    80601a7e:	2781                	sext.w	a5,a5
    80601a80:	079e                	slli	a5,a5,0x7
    80601a82:	0001f517          	auipc	a0,0x1f
    80601a86:	ff650513          	addi	a0,a0,-10 # 80620a78 <cpus>
    80601a8a:	953e                	add	a0,a0,a5
    80601a8c:	8082                	ret

0000000080601a8e <myproc>:
    80601a8e:	1101                	addi	sp,sp,-32
    80601a90:	ec06                	sd	ra,24(sp)
    80601a92:	00001097          	auipc	ra,0x1
    80601a96:	d4a080e7          	jalr	-694(ra) # 806027dc <push_off>
    80601a9a:	8792                	mv	a5,tp
    80601a9c:	2781                	sext.w	a5,a5
    80601a9e:	0001f717          	auipc	a4,0x1f
    80601aa2:	fda70713          	addi	a4,a4,-38 # 80620a78 <cpus>
    80601aa6:	079e                	slli	a5,a5,0x7
    80601aa8:	97ba                	add	a5,a5,a4
    80601aaa:	6388                	ld	a0,0(a5)
    80601aac:	e42a                	sd	a0,8(sp)
    80601aae:	00001097          	auipc	ra,0x1
    80601ab2:	dbe080e7          	jalr	-578(ra) # 8060286c <pop_off>
    80601ab6:	60e2                	ld	ra,24(sp)
    80601ab8:	6522                	ld	a0,8(sp)
    80601aba:	6105                	addi	sp,sp,32
    80601abc:	8082                	ret

0000000080601abe <allocpid>:
    80601abe:	1141                	addi	sp,sp,-16
    80601ac0:	0001f517          	auipc	a0,0x1f
    80601ac4:	3b850513          	addi	a0,a0,952 # 80620e78 <pid_lock>
    80601ac8:	e406                	sd	ra,8(sp)
    80601aca:	e022                	sd	s0,0(sp)
    80601acc:	00001097          	auipc	ra,0x1
    80601ad0:	d50080e7          	jalr	-688(ra) # 8060281c <acquire>
    80601ad4:	00001417          	auipc	s0,0x1
    80601ad8:	7b842403          	lw	s0,1976(s0) # 8060328c <nextpid>
    80601adc:	0001f517          	auipc	a0,0x1f
    80601ae0:	39c50513          	addi	a0,a0,924 # 80620e78 <pid_lock>
    80601ae4:	0014079b          	addiw	a5,s0,1
    80601ae8:	00001717          	auipc	a4,0x1
    80601aec:	7af72223          	sw	a5,1956(a4) # 8060328c <nextpid>
    80601af0:	00001097          	auipc	ra,0x1
    80601af4:	dca080e7          	jalr	-566(ra) # 806028ba <release>
    80601af8:	60a2                	ld	ra,8(sp)
    80601afa:	8522                	mv	a0,s0
    80601afc:	6402                	ld	s0,0(sp)
    80601afe:	0141                	addi	sp,sp,16
    80601b00:	8082                	ret

0000000080601b02 <proc_pagetable>:
    80601b02:	1101                	addi	sp,sp,-32
    80601b04:	e822                	sd	s0,16(sp)
    80601b06:	e426                	sd	s1,8(sp)
    80601b08:	ec06                	sd	ra,24(sp)
    80601b0a:	84aa                	mv	s1,a0
    80601b0c:	00001097          	auipc	ra,0x1
    80601b10:	f4e080e7          	jalr	-178(ra) # 80602a5a <uvmcreate>
    80601b14:	842a                	mv	s0,a0
    80601b16:	cd1d                	beqz	a0,80601b54 <proc_pagetable+0x52>
    80601b18:	040005b7          	lui	a1,0x4000
    80601b1c:	15fd                	addi	a1,a1,-1 # 3ffffff <MEGAPAGE_SIZE+0x3dfffff>
    80601b1e:	05b2                	slli	a1,a1,0xc
    80601b20:	4729                	li	a4,10
    80601b22:	00025697          	auipc	a3,0x25
    80601b26:	d8668693          	addi	a3,a3,-634 # 806268a8 <trampoline>
    80601b2a:	6605                	lui	a2,0x1
    80601b2c:	00001097          	auipc	ra,0x1
    80601b30:	f26080e7          	jalr	-218(ra) # 80602a52 <mappages>
    80601b34:	02054663          	bltz	a0,80601b60 <proc_pagetable+0x5e>
    80601b38:	020005b7          	lui	a1,0x2000
    80601b3c:	6cb4                	ld	a3,88(s1)
    80601b3e:	15fd                	addi	a1,a1,-1 # 1ffffff <MEGAPAGE_SIZE+0x1dfffff>
    80601b40:	05b6                	slli	a1,a1,0xd
    80601b42:	4719                	li	a4,6
    80601b44:	6605                	lui	a2,0x1
    80601b46:	8522                	mv	a0,s0
    80601b48:	00001097          	auipc	ra,0x1
    80601b4c:	f0a080e7          	jalr	-246(ra) # 80602a52 <mappages>
    80601b50:	02054563          	bltz	a0,80601b7a <proc_pagetable+0x78>
    80601b54:	60e2                	ld	ra,24(sp)
    80601b56:	8522                	mv	a0,s0
    80601b58:	6442                	ld	s0,16(sp)
    80601b5a:	64a2                	ld	s1,8(sp)
    80601b5c:	6105                	addi	sp,sp,32
    80601b5e:	8082                	ret
    80601b60:	8522                	mv	a0,s0
    80601b62:	4581                	li	a1,0
    80601b64:	00001097          	auipc	ra,0x1
    80601b68:	efe080e7          	jalr	-258(ra) # 80602a62 <uvmfree>
    80601b6c:	4401                	li	s0,0
    80601b6e:	60e2                	ld	ra,24(sp)
    80601b70:	8522                	mv	a0,s0
    80601b72:	6442                	ld	s0,16(sp)
    80601b74:	64a2                	ld	s1,8(sp)
    80601b76:	6105                	addi	sp,sp,32
    80601b78:	8082                	ret
    80601b7a:	040005b7          	lui	a1,0x4000
    80601b7e:	15fd                	addi	a1,a1,-1 # 3ffffff <MEGAPAGE_SIZE+0x3dfffff>
    80601b80:	8522                	mv	a0,s0
    80601b82:	05b2                	slli	a1,a1,0xc
    80601b84:	4681                	li	a3,0
    80601b86:	4605                	li	a2,1
    80601b88:	00001097          	auipc	ra,0x1
    80601b8c:	eea080e7          	jalr	-278(ra) # 80602a72 <uvmunmap>
    80601b90:	8522                	mv	a0,s0
    80601b92:	4581                	li	a1,0
    80601b94:	00001097          	auipc	ra,0x1
    80601b98:	ece080e7          	jalr	-306(ra) # 80602a62 <uvmfree>
    80601b9c:	4401                	li	s0,0
    80601b9e:	bfc1                	j	80601b6e <proc_pagetable+0x6c>

0000000080601ba0 <allocproc>:
    80601ba0:	1101                	addi	sp,sp,-32
    80601ba2:	e822                	sd	s0,16(sp)
    80601ba4:	e426                	sd	s1,8(sp)
    80601ba6:	ec06                	sd	ra,24(sp)
    80601ba8:	0001f417          	auipc	s0,0x1f
    80601bac:	30040413          	addi	s0,s0,768 # 80620ea8 <proc>
    80601bb0:	00025497          	auipc	s1,0x25
    80601bb4:	cf848493          	addi	s1,s1,-776 # 806268a8 <trampoline>
    80601bb8:	a809                	j	80601bca <allocproc+0x2a>
    80601bba:	16840413          	addi	s0,s0,360
    80601bbe:	00001097          	auipc	ra,0x1
    80601bc2:	cfc080e7          	jalr	-772(ra) # 806028ba <release>
    80601bc6:	0a940663          	beq	s0,s1,80601c72 <allocproc+0xd2>
    80601bca:	8522                	mv	a0,s0
    80601bcc:	00001097          	auipc	ra,0x1
    80601bd0:	c50080e7          	jalr	-944(ra) # 8060281c <acquire>
    80601bd4:	4c1c                	lw	a5,24(s0)
    80601bd6:	8522                	mv	a0,s0
    80601bd8:	f3ed                	bnez	a5,80601bba <allocproc+0x1a>
    80601bda:	0001f517          	auipc	a0,0x1f
    80601bde:	29e50513          	addi	a0,a0,670 # 80620e78 <pid_lock>
    80601be2:	00001097          	auipc	ra,0x1
    80601be6:	c3a080e7          	jalr	-966(ra) # 8060281c <acquire>
    80601bea:	00001497          	auipc	s1,0x1
    80601bee:	6a24a483          	lw	s1,1698(s1) # 8060328c <nextpid>
    80601bf2:	0001f517          	auipc	a0,0x1f
    80601bf6:	28650513          	addi	a0,a0,646 # 80620e78 <pid_lock>
    80601bfa:	0014879b          	addiw	a5,s1,1
    80601bfe:	00001717          	auipc	a4,0x1
    80601c02:	68f72723          	sw	a5,1678(a4) # 8060328c <nextpid>
    80601c06:	00001097          	auipc	ra,0x1
    80601c0a:	cb4080e7          	jalr	-844(ra) # 806028ba <release>
    80601c0e:	4785                	li	a5,1
    80601c10:	d804                	sw	s1,48(s0)
    80601c12:	cc1c                	sw	a5,24(s0)
    80601c14:	00001097          	auipc	ra,0x1
    80601c18:	e1c080e7          	jalr	-484(ra) # 80602a30 <kalloc>
    80601c1c:	ec28                	sd	a0,88(s0)
    80601c1e:	c121                	beqz	a0,80601c5e <allocproc+0xbe>
    80601c20:	8522                	mv	a0,s0
    80601c22:	00000097          	auipc	ra,0x0
    80601c26:	ee0080e7          	jalr	-288(ra) # 80601b02 <proc_pagetable>
    80601c2a:	e828                	sd	a0,80(s0)
    80601c2c:	c90d                	beqz	a0,80601c5e <allocproc+0xbe>
    80601c2e:	06040513          	addi	a0,s0,96
    80601c32:	07000613          	li	a2,112
    80601c36:	4581                	li	a1,0
    80601c38:	00001097          	auipc	ra,0x1
    80601c3c:	cc8080e7          	jalr	-824(ra) # 80602900 <memset>
    80601c40:	603c                	ld	a5,64(s0)
    80601c42:	6685                	lui	a3,0x1
    80601c44:	00000717          	auipc	a4,0x0
    80601c48:	c3e70713          	addi	a4,a4,-962 # 80601882 <forkret>
    80601c4c:	97b6                	add	a5,a5,a3
    80601c4e:	60e2                	ld	ra,24(sp)
    80601c50:	f038                	sd	a4,96(s0)
    80601c52:	f43c                	sd	a5,104(s0)
    80601c54:	8522                	mv	a0,s0
    80601c56:	6442                	ld	s0,16(sp)
    80601c58:	64a2                	ld	s1,8(sp)
    80601c5a:	6105                	addi	sp,sp,32
    80601c5c:	8082                	ret
    80601c5e:	8522                	mv	a0,s0
    80601c60:	00000097          	auipc	ra,0x0
    80601c64:	ba2080e7          	jalr	-1118(ra) # 80601802 <freeproc>
    80601c68:	8522                	mv	a0,s0
    80601c6a:	00001097          	auipc	ra,0x1
    80601c6e:	c50080e7          	jalr	-944(ra) # 806028ba <release>
    80601c72:	4401                	li	s0,0
    80601c74:	60e2                	ld	ra,24(sp)
    80601c76:	8522                	mv	a0,s0
    80601c78:	6442                	ld	s0,16(sp)
    80601c7a:	64a2                	ld	s1,8(sp)
    80601c7c:	6105                	addi	sp,sp,32
    80601c7e:	8082                	ret

0000000080601c80 <proc_freepagetable>:
    80601c80:	040007b7          	lui	a5,0x4000
    80601c84:	1101                	addi	sp,sp,-32
    80601c86:	17fd                	addi	a5,a5,-1 # 3ffffff <MEGAPAGE_SIZE+0x3dfffff>
    80601c88:	e426                	sd	s1,8(sp)
    80601c8a:	4681                	li	a3,0
    80601c8c:	84ae                	mv	s1,a1
    80601c8e:	4605                	li	a2,1
    80601c90:	00c79593          	slli	a1,a5,0xc
    80601c94:	ec06                	sd	ra,24(sp)
    80601c96:	e822                	sd	s0,16(sp)
    80601c98:	842a                	mv	s0,a0
    80601c9a:	00001097          	auipc	ra,0x1
    80601c9e:	dd8080e7          	jalr	-552(ra) # 80602a72 <uvmunmap>
    80601ca2:	020005b7          	lui	a1,0x2000
    80601ca6:	15fd                	addi	a1,a1,-1 # 1ffffff <MEGAPAGE_SIZE+0x1dfffff>
    80601ca8:	8522                	mv	a0,s0
    80601caa:	05b6                	slli	a1,a1,0xd
    80601cac:	4681                	li	a3,0
    80601cae:	4605                	li	a2,1
    80601cb0:	00001097          	auipc	ra,0x1
    80601cb4:	dc2080e7          	jalr	-574(ra) # 80602a72 <uvmunmap>
    80601cb8:	8522                	mv	a0,s0
    80601cba:	6442                	ld	s0,16(sp)
    80601cbc:	60e2                	ld	ra,24(sp)
    80601cbe:	85a6                	mv	a1,s1
    80601cc0:	64a2                	ld	s1,8(sp)
    80601cc2:	6105                	addi	sp,sp,32
    80601cc4:	00001317          	auipc	t1,0x1
    80601cc8:	d9e30067          	jr	-610(t1) # 80602a62 <uvmfree>

0000000080601ccc <userinit>:
    80601ccc:	1141                	addi	sp,sp,-16
    80601cce:	e406                	sd	ra,8(sp)
    80601cd0:	e022                	sd	s0,0(sp)
    80601cd2:	00000097          	auipc	ra,0x0
    80601cd6:	ece080e7          	jalr	-306(ra) # 80601ba0 <allocproc>
    80601cda:	842a                	mv	s0,a0
    80601cdc:	00001517          	auipc	a0,0x1
    80601ce0:	3b450513          	addi	a0,a0,948 # 80603090 <kexec+0x5ca>
    80601ce4:	00026797          	auipc	a5,0x26
    80601ce8:	bc87b623          	sd	s0,-1076(a5) # 806278b0 <initproc>
    80601cec:	00001097          	auipc	ra,0x1
    80601cf0:	dce080e7          	jalr	-562(ra) # 80602aba <namei>
    80601cf4:	478d                	li	a5,3
    80601cf6:	14a43823          	sd	a0,336(s0)
    80601cfa:	cc1c                	sw	a5,24(s0)
    80601cfc:	8522                	mv	a0,s0
    80601cfe:	6402                	ld	s0,0(sp)
    80601d00:	60a2                	ld	ra,8(sp)
    80601d02:	0141                	addi	sp,sp,16
    80601d04:	00001317          	auipc	t1,0x1
    80601d08:	bb630067          	jr	-1098(t1) # 806028ba <release>

0000000080601d0c <growproc>:
    80601d0c:	1101                	addi	sp,sp,-32
    80601d0e:	e822                	sd	s0,16(sp)
    80601d10:	ec06                	sd	ra,24(sp)
    80601d12:	e426                	sd	s1,8(sp)
    80601d14:	842a                	mv	s0,a0
    80601d16:	00001097          	auipc	ra,0x1
    80601d1a:	ac6080e7          	jalr	-1338(ra) # 806027dc <push_off>
    80601d1e:	8792                	mv	a5,tp
    80601d20:	2781                	sext.w	a5,a5
    80601d22:	0001f717          	auipc	a4,0x1f
    80601d26:	d5670713          	addi	a4,a4,-682 # 80620a78 <cpus>
    80601d2a:	079e                	slli	a5,a5,0x7
    80601d2c:	97ba                	add	a5,a5,a4
    80601d2e:	6384                	ld	s1,0(a5)
    80601d30:	00001097          	auipc	ra,0x1
    80601d34:	b3c080e7          	jalr	-1220(ra) # 8060286c <pop_off>
    80601d38:	64ac                	ld	a1,72(s1)
    80601d3a:	02805963          	blez	s0,80601d6c <growproc+0x60>
    80601d3e:	020007b7          	lui	a5,0x2000
    80601d42:	17fd                	addi	a5,a5,-1 # 1ffffff <MEGAPAGE_SIZE+0x1dfffff>
    80601d44:	00b40633          	add	a2,s0,a1
    80601d48:	07b6                	slli	a5,a5,0xd
    80601d4a:	02c7eb63          	bltu	a5,a2,80601d80 <growproc+0x74>
    80601d4e:	68a8                	ld	a0,80(s1)
    80601d50:	4691                	li	a3,4
    80601d52:	00001097          	auipc	ra,0x1
    80601d56:	d22080e7          	jalr	-734(ra) # 80602a74 <uvmalloc>
    80601d5a:	85aa                	mv	a1,a0
    80601d5c:	c115                	beqz	a0,80601d80 <growproc+0x74>
    80601d5e:	e4ac                	sd	a1,72(s1)
    80601d60:	4501                	li	a0,0
    80601d62:	60e2                	ld	ra,24(sp)
    80601d64:	6442                	ld	s0,16(sp)
    80601d66:	64a2                	ld	s1,8(sp)
    80601d68:	6105                	addi	sp,sp,32
    80601d6a:	8082                	ret
    80601d6c:	d86d                	beqz	s0,80601d5e <growproc+0x52>
    80601d6e:	68a8                	ld	a0,80(s1)
    80601d70:	00b40633          	add	a2,s0,a1
    80601d74:	00001097          	auipc	ra,0x1
    80601d78:	d04080e7          	jalr	-764(ra) # 80602a78 <uvmdealloc>
    80601d7c:	85aa                	mv	a1,a0
    80601d7e:	b7c5                	j	80601d5e <growproc+0x52>
    80601d80:	557d                	li	a0,-1
    80601d82:	b7c5                	j	80601d62 <growproc+0x56>

0000000080601d84 <kfork>:
    80601d84:	7179                	addi	sp,sp,-48
    80601d86:	f406                	sd	ra,40(sp)
    80601d88:	f022                	sd	s0,32(sp)
    80601d8a:	e052                	sd	s4,0(sp)
    80601d8c:	00001097          	auipc	ra,0x1
    80601d90:	a50080e7          	jalr	-1456(ra) # 806027dc <push_off>
    80601d94:	8792                	mv	a5,tp
    80601d96:	2781                	sext.w	a5,a5
    80601d98:	0001f717          	auipc	a4,0x1f
    80601d9c:	ce070713          	addi	a4,a4,-800 # 80620a78 <cpus>
    80601da0:	079e                	slli	a5,a5,0x7
    80601da2:	97ba                	add	a5,a5,a4
    80601da4:	0007ba03          	ld	s4,0(a5)
    80601da8:	00001097          	auipc	ra,0x1
    80601dac:	ac4080e7          	jalr	-1340(ra) # 8060286c <pop_off>
    80601db0:	00000097          	auipc	ra,0x0
    80601db4:	df0080e7          	jalr	-528(ra) # 80601ba0 <allocproc>
    80601db8:	10050b63          	beqz	a0,80601ece <kfork+0x14a>
    80601dbc:	e44e                	sd	s3,8(sp)
    80601dbe:	692c                	ld	a1,80(a0)
    80601dc0:	89aa                	mv	s3,a0
    80601dc2:	048a3603          	ld	a2,72(s4)
    80601dc6:	050a3503          	ld	a0,80(s4)
    80601dca:	00001097          	auipc	ra,0x1
    80601dce:	ca4080e7          	jalr	-860(ra) # 80602a6e <uvmcopy>
    80601dd2:	0e054363          	bltz	a0,80601eb8 <kfork+0x134>
    80601dd6:	048a3703          	ld	a4,72(s4)
    80601dda:	058a3783          	ld	a5,88(s4)
    80601dde:	0589b683          	ld	a3,88(s3)
    80601de2:	ec26                	sd	s1,24(sp)
    80601de4:	e84a                	sd	s2,16(sp)
    80601de6:	04e9b423          	sd	a4,72(s3)
    80601dea:	12078613          	addi	a2,a5,288
    80601dee:	8736                	mv	a4,a3
    80601df0:	6b8c                	ld	a1,16(a5)
    80601df2:	0007b803          	ld	a6,0(a5)
    80601df6:	6788                	ld	a0,8(a5)
    80601df8:	eb0c                	sd	a1,16(a4)
    80601dfa:	01073023          	sd	a6,0(a4)
    80601dfe:	e708                	sd	a0,8(a4)
    80601e00:	6f8c                	ld	a1,24(a5)
    80601e02:	02078793          	addi	a5,a5,32
    80601e06:	02070713          	addi	a4,a4,32
    80601e0a:	feb73c23          	sd	a1,-8(a4)
    80601e0e:	fec791e3          	bne	a5,a2,80601df0 <kfork+0x6c>
    80601e12:	0606b823          	sd	zero,112(a3) # 1070 <PAGE_SIZE+0x70>
    80601e16:	0d0a0413          	addi	s0,s4,208
    80601e1a:	0d098493          	addi	s1,s3,208
    80601e1e:	150a0913          	addi	s2,s4,336
    80601e22:	6008                	ld	a0,0(s0)
    80601e24:	c511                	beqz	a0,80601e30 <kfork+0xac>
    80601e26:	00001097          	auipc	ra,0x1
    80601e2a:	c9e080e7          	jalr	-866(ra) # 80602ac4 <filedup>
    80601e2e:	e088                	sd	a0,0(s1)
    80601e30:	0421                	addi	s0,s0,8
    80601e32:	04a1                	addi	s1,s1,8
    80601e34:	ff2417e3          	bne	s0,s2,80601e22 <kfork+0x9e>
    80601e38:	150a3503          	ld	a0,336(s4)
    80601e3c:	00001097          	auipc	ra,0x1
    80601e40:	c84080e7          	jalr	-892(ra) # 80602ac0 <idup>
    80601e44:	158a0593          	addi	a1,s4,344
    80601e48:	4641                	li	a2,16
    80601e4a:	14a9b823          	sd	a0,336(s3)
    80601e4e:	15898513          	addi	a0,s3,344
    80601e52:	00001097          	auipc	ra,0x1
    80601e56:	b9e080e7          	jalr	-1122(ra) # 806029f0 <safestrcpy>
    80601e5a:	854e                	mv	a0,s3
    80601e5c:	0309a403          	lw	s0,48(s3)
    80601e60:	00001097          	auipc	ra,0x1
    80601e64:	a5a080e7          	jalr	-1446(ra) # 806028ba <release>
    80601e68:	0001f517          	auipc	a0,0x1f
    80601e6c:	02850513          	addi	a0,a0,40 # 80620e90 <wait_lock>
    80601e70:	00001097          	auipc	ra,0x1
    80601e74:	9ac080e7          	jalr	-1620(ra) # 8060281c <acquire>
    80601e78:	0001f517          	auipc	a0,0x1f
    80601e7c:	01850513          	addi	a0,a0,24 # 80620e90 <wait_lock>
    80601e80:	0349bc23          	sd	s4,56(s3)
    80601e84:	00001097          	auipc	ra,0x1
    80601e88:	a36080e7          	jalr	-1482(ra) # 806028ba <release>
    80601e8c:	854e                	mv	a0,s3
    80601e8e:	00001097          	auipc	ra,0x1
    80601e92:	98e080e7          	jalr	-1650(ra) # 8060281c <acquire>
    80601e96:	478d                	li	a5,3
    80601e98:	00f9ac23          	sw	a5,24(s3)
    80601e9c:	854e                	mv	a0,s3
    80601e9e:	00001097          	auipc	ra,0x1
    80601ea2:	a1c080e7          	jalr	-1508(ra) # 806028ba <release>
    80601ea6:	64e2                	ld	s1,24(sp)
    80601ea8:	6942                	ld	s2,16(sp)
    80601eaa:	69a2                	ld	s3,8(sp)
    80601eac:	70a2                	ld	ra,40(sp)
    80601eae:	8522                	mv	a0,s0
    80601eb0:	7402                	ld	s0,32(sp)
    80601eb2:	6a02                	ld	s4,0(sp)
    80601eb4:	6145                	addi	sp,sp,48
    80601eb6:	8082                	ret
    80601eb8:	854e                	mv	a0,s3
    80601eba:	00000097          	auipc	ra,0x0
    80601ebe:	948080e7          	jalr	-1720(ra) # 80601802 <freeproc>
    80601ec2:	854e                	mv	a0,s3
    80601ec4:	00001097          	auipc	ra,0x1
    80601ec8:	9f6080e7          	jalr	-1546(ra) # 806028ba <release>
    80601ecc:	69a2                	ld	s3,8(sp)
    80601ece:	547d                	li	s0,-1
    80601ed0:	bff1                	j	80601eac <kfork+0x128>

0000000080601ed2 <reparent>:
    80601ed2:	715d                	addi	sp,sp,-80
    80601ed4:	fc26                	sd	s1,56(sp)
    80601ed6:	f44e                	sd	s3,40(sp)
    80601ed8:	f052                	sd	s4,32(sp)
    80601eda:	ec56                	sd	s5,24(sp)
    80601edc:	e85a                	sd	s6,16(sp)
    80601ede:	e45e                	sd	s7,8(sp)
    80601ee0:	e486                	sd	ra,72(sp)
    80601ee2:	e0a2                	sd	s0,64(sp)
    80601ee4:	f84a                	sd	s2,48(sp)
    80601ee6:	8b2a                	mv	s6,a0
    80601ee8:	0001f997          	auipc	s3,0x1f
    80601eec:	fc098993          	addi	s3,s3,-64 # 80620ea8 <proc>
    80601ef0:	00025497          	auipc	s1,0x25
    80601ef4:	9b848493          	addi	s1,s1,-1608 # 806268a8 <trampoline>
    80601ef8:	00026b97          	auipc	s7,0x26
    80601efc:	9b8b8b93          	addi	s7,s7,-1608 # 806278b0 <initproc>
    80601f00:	4a09                	li	s4,2
    80601f02:	4a8d                	li	s5,3
    80601f04:	a029                	j	80601f0e <reparent+0x3c>
    80601f06:	16898993          	addi	s3,s3,360
    80601f0a:	04998863          	beq	s3,s1,80601f5a <reparent+0x88>
    80601f0e:	0389b783          	ld	a5,56(s3)
    80601f12:	ff679ae3          	bne	a5,s6,80601f06 <reparent+0x34>
    80601f16:	000bb903          	ld	s2,0(s7)
    80601f1a:	0001f417          	auipc	s0,0x1f
    80601f1e:	f8e40413          	addi	s0,s0,-114 # 80620ea8 <proc>
    80601f22:	0329bc23          	sd	s2,56(s3)
    80601f26:	a809                	j	80601f38 <reparent+0x66>
    80601f28:	16840413          	addi	s0,s0,360
    80601f2c:	00001097          	auipc	ra,0x1
    80601f30:	98e080e7          	jalr	-1650(ra) # 806028ba <release>
    80601f34:	fc9409e3          	beq	s0,s1,80601f06 <reparent+0x34>
    80601f38:	8522                	mv	a0,s0
    80601f3a:	00001097          	auipc	ra,0x1
    80601f3e:	8e2080e7          	jalr	-1822(ra) # 8060281c <acquire>
    80601f42:	701c                	ld	a5,32(s0)
    80601f44:	8522                	mv	a0,s0
    80601f46:	fef911e3          	bne	s2,a5,80601f28 <reparent+0x56>
    80601f4a:	4c1c                	lw	a5,24(s0)
    80601f4c:	02043023          	sd	zero,32(s0)
    80601f50:	fd479ce3          	bne	a5,s4,80601f28 <reparent+0x56>
    80601f54:	01542c23          	sw	s5,24(s0)
    80601f58:	bfc1                	j	80601f28 <reparent+0x56>
    80601f5a:	60a6                	ld	ra,72(sp)
    80601f5c:	6406                	ld	s0,64(sp)
    80601f5e:	74e2                	ld	s1,56(sp)
    80601f60:	7942                	ld	s2,48(sp)
    80601f62:	79a2                	ld	s3,40(sp)
    80601f64:	7a02                	ld	s4,32(sp)
    80601f66:	6ae2                	ld	s5,24(sp)
    80601f68:	6b42                	ld	s6,16(sp)
    80601f6a:	6ba2                	ld	s7,8(sp)
    80601f6c:	6161                	addi	sp,sp,80
    80601f6e:	8082                	ret

0000000080601f70 <scheduler>:
    80601f70:	715d                	addi	sp,sp,-80
    80601f72:	e486                	sd	ra,72(sp)
    80601f74:	e0a2                	sd	s0,64(sp)
    80601f76:	fc26                	sd	s1,56(sp)
    80601f78:	f84a                	sd	s2,48(sp)
    80601f7a:	f44e                	sd	s3,40(sp)
    80601f7c:	f052                	sd	s4,32(sp)
    80601f7e:	ec56                	sd	s5,24(sp)
    80601f80:	e85a                	sd	s6,16(sp)
    80601f82:	e45e                	sd	s7,8(sp)
    80601f84:	8792                	mv	a5,tp
    80601f86:	2781                	sext.w	a5,a5
    80601f88:	079e                	slli	a5,a5,0x7
    80601f8a:	0001fa97          	auipc	s5,0x1f
    80601f8e:	aeea8a93          	addi	s5,s5,-1298 # 80620a78 <cpus>
    80601f92:	00fa89b3          	add	s3,s5,a5
    80601f96:	0001fa17          	auipc	s4,0x1f
    80601f9a:	aeaa0a13          	addi	s4,s4,-1302 # 80620a80 <cpus+0x8>
    80601f9e:	9a3e                	add	s4,s4,a5
    80601fa0:	0009b023          	sd	zero,0(s3)
    80601fa4:	00025917          	auipc	s2,0x25
    80601fa8:	90490913          	addi	s2,s2,-1788 # 806268a8 <trampoline>
    80601fac:	448d                	li	s1,3
    80601fae:	4b11                	li	s6,4
    80601fb0:	10016073          	csrsi	sstatus,2
    80601fb4:	10017073          	csrci	sstatus,2
    80601fb8:	4b81                	li	s7,0
    80601fba:	0001f417          	auipc	s0,0x1f
    80601fbe:	eee40413          	addi	s0,s0,-274 # 80620ea8 <proc>
    80601fc2:	a811                	j	80601fd6 <scheduler+0x66>
    80601fc4:	8522                	mv	a0,s0
    80601fc6:	16840413          	addi	s0,s0,360
    80601fca:	00001097          	auipc	ra,0x1
    80601fce:	8f0080e7          	jalr	-1808(ra) # 806028ba <release>
    80601fd2:	05240763          	beq	s0,s2,80602020 <scheduler+0xb0>
    80601fd6:	8522                	mv	a0,s0
    80601fd8:	00001097          	auipc	ra,0x1
    80601fdc:	844080e7          	jalr	-1980(ra) # 8060281c <acquire>
    80601fe0:	4c1c                	lw	a5,24(s0)
    80601fe2:	fe9791e3          	bne	a5,s1,80601fc4 <scheduler+0x54>
    80601fe6:	06040593          	addi	a1,s0,96
    80601fea:	8552                	mv	a0,s4
    80601fec:	01642c23          	sw	s6,24(s0)
    80601ff0:	0089b023          	sd	s0,0(s3)
    80601ff4:	fffff097          	auipc	ra,0xfffff
    80601ff8:	424080e7          	jalr	1060(ra) # 80601418 <swtch>
    80601ffc:	8792                	mv	a5,tp
    80601ffe:	2781                	sext.w	a5,a5
    80602000:	079e                	slli	a5,a5,0x7
    80602002:	97d6                	add	a5,a5,s5
    80602004:	8522                	mv	a0,s0
    80602006:	16840413          	addi	s0,s0,360
    8060200a:	0607ae23          	sw	zero,124(a5)
    8060200e:	0009b023          	sd	zero,0(s3)
    80602012:	4b85                	li	s7,1
    80602014:	00001097          	auipc	ra,0x1
    80602018:	8a6080e7          	jalr	-1882(ra) # 806028ba <release>
    8060201c:	fb241de3          	bne	s0,s2,80601fd6 <scheduler+0x66>
    80602020:	f80b98e3          	bnez	s7,80601fb0 <scheduler+0x40>
    80602024:	10500073          	wfi
    80602028:	b761                	j	80601fb0 <scheduler+0x40>

000000008060202a <sched>:
    8060202a:	1101                	addi	sp,sp,-32
    8060202c:	ec06                	sd	ra,24(sp)
    8060202e:	e822                	sd	s0,16(sp)
    80602030:	e426                	sd	s1,8(sp)
    80602032:	e04a                	sd	s2,0(sp)
    80602034:	00000097          	auipc	ra,0x0
    80602038:	7a8080e7          	jalr	1960(ra) # 806027dc <push_off>
    8060203c:	8792                	mv	a5,tp
    8060203e:	2781                	sext.w	a5,a5
    80602040:	079e                	slli	a5,a5,0x7
    80602042:	0001f417          	auipc	s0,0x1f
    80602046:	a3640413          	addi	s0,s0,-1482 # 80620a78 <cpus>
    8060204a:	97a2                	add	a5,a5,s0
    8060204c:	6384                	ld	s1,0(a5)
    8060204e:	00001097          	auipc	ra,0x1
    80602052:	81e080e7          	jalr	-2018(ra) # 8060286c <pop_off>
    80602056:	8526                	mv	a0,s1
    80602058:	00000097          	auipc	ra,0x0
    8060205c:	75c080e7          	jalr	1884(ra) # 806027b4 <holding>
    80602060:	cd31                	beqz	a0,806020bc <sched+0x92>
    80602062:	8792                	mv	a5,tp
    80602064:	2781                	sext.w	a5,a5
    80602066:	079e                	slli	a5,a5,0x7
    80602068:	97a2                	add	a5,a5,s0
    8060206a:	5fb8                	lw	a4,120(a5)
    8060206c:	4785                	li	a5,1
    8060206e:	06f71f63          	bne	a4,a5,806020ec <sched+0xc2>
    80602072:	4c98                	lw	a4,24(s1)
    80602074:	4791                	li	a5,4
    80602076:	06f70363          	beq	a4,a5,806020dc <sched+0xb2>
    8060207a:	100027f3          	csrr	a5,sstatus
    8060207e:	8b89                	andi	a5,a5,2
    80602080:	e7b1                	bnez	a5,806020cc <sched+0xa2>
    80602082:	8792                	mv	a5,tp
    80602084:	2781                	sext.w	a5,a5
    80602086:	079e                	slli	a5,a5,0x7
    80602088:	97a2                	add	a5,a5,s0
    8060208a:	07c7a903          	lw	s2,124(a5)
    8060208e:	8592                	mv	a1,tp
    80602090:	2581                	sext.w	a1,a1
    80602092:	059e                	slli	a1,a1,0x7
    80602094:	05a1                	addi	a1,a1,8
    80602096:	95a2                	add	a1,a1,s0
    80602098:	06048513          	addi	a0,s1,96
    8060209c:	fffff097          	auipc	ra,0xfffff
    806020a0:	37c080e7          	jalr	892(ra) # 80601418 <swtch>
    806020a4:	8792                	mv	a5,tp
    806020a6:	2781                	sext.w	a5,a5
    806020a8:	079e                	slli	a5,a5,0x7
    806020aa:	943e                	add	s0,s0,a5
    806020ac:	07242e23          	sw	s2,124(s0)
    806020b0:	60e2                	ld	ra,24(sp)
    806020b2:	6442                	ld	s0,16(sp)
    806020b4:	64a2                	ld	s1,8(sp)
    806020b6:	6902                	ld	s2,0(sp)
    806020b8:	6105                	addi	sp,sp,32
    806020ba:	8082                	ret
    806020bc:	00001517          	auipc	a0,0x1
    806020c0:	fdc50513          	addi	a0,a0,-36 # 80603098 <kexec+0x5d2>
    806020c4:	fffff097          	auipc	ra,0xfffff
    806020c8:	6e6080e7          	jalr	1766(ra) # 806017aa <panic>
    806020cc:	00001517          	auipc	a0,0x1
    806020d0:	ffc50513          	addi	a0,a0,-4 # 806030c8 <kexec+0x602>
    806020d4:	fffff097          	auipc	ra,0xfffff
    806020d8:	6d6080e7          	jalr	1750(ra) # 806017aa <panic>
    806020dc:	00001517          	auipc	a0,0x1
    806020e0:	fdc50513          	addi	a0,a0,-36 # 806030b8 <kexec+0x5f2>
    806020e4:	fffff097          	auipc	ra,0xfffff
    806020e8:	6c6080e7          	jalr	1734(ra) # 806017aa <panic>
    806020ec:	00001517          	auipc	a0,0x1
    806020f0:	fbc50513          	addi	a0,a0,-68 # 806030a8 <kexec+0x5e2>
    806020f4:	fffff097          	auipc	ra,0xfffff
    806020f8:	6b6080e7          	jalr	1718(ra) # 806017aa <panic>

00000000806020fc <kexit>:
    806020fc:	7139                	addi	sp,sp,-64
    806020fe:	e05a                	sd	s6,0(sp)
    80602100:	fc06                	sd	ra,56(sp)
    80602102:	e852                	sd	s4,16(sp)
    80602104:	8b2a                	mv	s6,a0
    80602106:	00000097          	auipc	ra,0x0
    8060210a:	6d6080e7          	jalr	1750(ra) # 806027dc <push_off>
    8060210e:	8792                	mv	a5,tp
    80602110:	2781                	sext.w	a5,a5
    80602112:	0001f717          	auipc	a4,0x1f
    80602116:	96670713          	addi	a4,a4,-1690 # 80620a78 <cpus>
    8060211a:	079e                	slli	a5,a5,0x7
    8060211c:	97ba                	add	a5,a5,a4
    8060211e:	0007ba03          	ld	s4,0(a5)
    80602122:	00000097          	auipc	ra,0x0
    80602126:	74a080e7          	jalr	1866(ra) # 8060286c <pop_off>
    8060212a:	00025797          	auipc	a5,0x25
    8060212e:	7867b783          	ld	a5,1926(a5) # 806278b0 <initproc>
    80602132:	f822                	sd	s0,48(sp)
    80602134:	f426                	sd	s1,40(sp)
    80602136:	f04a                	sd	s2,32(sp)
    80602138:	ec4e                	sd	s3,24(sp)
    8060213a:	e456                	sd	s5,8(sp)
    8060213c:	0d0a0413          	addi	s0,s4,208
    80602140:	150a0493          	addi	s1,s4,336
    80602144:	0f478663          	beq	a5,s4,80602230 <kexit+0x134>
    80602148:	6008                	ld	a0,0(s0)
    8060214a:	c519                	beqz	a0,80602158 <kexit+0x5c>
    8060214c:	00001097          	auipc	ra,0x1
    80602150:	976080e7          	jalr	-1674(ra) # 80602ac2 <fileclose>
    80602154:	00043023          	sd	zero,0(s0)
    80602158:	0421                	addi	s0,s0,8
    8060215a:	fe9417e3          	bne	s0,s1,80602148 <kexit+0x4c>
    8060215e:	00001097          	auipc	ra,0x1
    80602162:	956080e7          	jalr	-1706(ra) # 80602ab4 <begin_op>
    80602166:	150a3503          	ld	a0,336(s4)
    8060216a:	0001f417          	auipc	s0,0x1f
    8060216e:	d3e40413          	addi	s0,s0,-706 # 80620ea8 <proc>
    80602172:	00024497          	auipc	s1,0x24
    80602176:	73648493          	addi	s1,s1,1846 # 806268a8 <trampoline>
    8060217a:	00001097          	auipc	ra,0x1
    8060217e:	944080e7          	jalr	-1724(ra) # 80602abe <iput>
    80602182:	00001097          	auipc	ra,0x1
    80602186:	934080e7          	jalr	-1740(ra) # 80602ab6 <end_op>
    8060218a:	0001f517          	auipc	a0,0x1f
    8060218e:	d0650513          	addi	a0,a0,-762 # 80620e90 <wait_lock>
    80602192:	140a3823          	sd	zero,336(s4)
    80602196:	00000097          	auipc	ra,0x0
    8060219a:	686080e7          	jalr	1670(ra) # 8060281c <acquire>
    8060219e:	8552                	mv	a0,s4
    806021a0:	00000097          	auipc	ra,0x0
    806021a4:	d32080e7          	jalr	-718(ra) # 80601ed2 <reparent>
    806021a8:	4989                	li	s3,2
    806021aa:	038a3903          	ld	s2,56(s4)
    806021ae:	4a8d                	li	s5,3
    806021b0:	a811                	j	806021c4 <kexit+0xc8>
    806021b2:	8522                	mv	a0,s0
    806021b4:	16840413          	addi	s0,s0,360
    806021b8:	00000097          	auipc	ra,0x0
    806021bc:	702080e7          	jalr	1794(ra) # 806028ba <release>
    806021c0:	02940a63          	beq	s0,s1,806021f4 <kexit+0xf8>
    806021c4:	8522                	mv	a0,s0
    806021c6:	00000097          	auipc	ra,0x0
    806021ca:	656080e7          	jalr	1622(ra) # 8060281c <acquire>
    806021ce:	701c                	ld	a5,32(s0)
    806021d0:	fef911e3          	bne	s2,a5,806021b2 <kexit+0xb6>
    806021d4:	4c1c                	lw	a5,24(s0)
    806021d6:	02043023          	sd	zero,32(s0)
    806021da:	fd379ce3          	bne	a5,s3,806021b2 <kexit+0xb6>
    806021de:	01542c23          	sw	s5,24(s0)
    806021e2:	8522                	mv	a0,s0
    806021e4:	16840413          	addi	s0,s0,360
    806021e8:	00000097          	auipc	ra,0x0
    806021ec:	6d2080e7          	jalr	1746(ra) # 806028ba <release>
    806021f0:	fc941ae3          	bne	s0,s1,806021c4 <kexit+0xc8>
    806021f4:	8552                	mv	a0,s4
    806021f6:	00000097          	auipc	ra,0x0
    806021fa:	626080e7          	jalr	1574(ra) # 8060281c <acquire>
    806021fe:	4795                	li	a5,5
    80602200:	0001f517          	auipc	a0,0x1f
    80602204:	c9050513          	addi	a0,a0,-880 # 80620e90 <wait_lock>
    80602208:	00fa2c23          	sw	a5,24(s4)
    8060220c:	036a2623          	sw	s6,44(s4)
    80602210:	00000097          	auipc	ra,0x0
    80602214:	6aa080e7          	jalr	1706(ra) # 806028ba <release>
    80602218:	00000097          	auipc	ra,0x0
    8060221c:	e12080e7          	jalr	-494(ra) # 8060202a <sched>
    80602220:	00001517          	auipc	a0,0x1
    80602224:	ed050513          	addi	a0,a0,-304 # 806030f0 <kexec+0x62a>
    80602228:	fffff097          	auipc	ra,0xfffff
    8060222c:	582080e7          	jalr	1410(ra) # 806017aa <panic>
    80602230:	00001517          	auipc	a0,0x1
    80602234:	eb050513          	addi	a0,a0,-336 # 806030e0 <kexec+0x61a>
    80602238:	fffff097          	auipc	ra,0xfffff
    8060223c:	572080e7          	jalr	1394(ra) # 806017aa <panic>

0000000080602240 <kwait>:
    80602240:	7139                	addi	sp,sp,-64
    80602242:	e456                	sd	s5,8(sp)
    80602244:	fc06                	sd	ra,56(sp)
    80602246:	f822                	sd	s0,48(sp)
    80602248:	f426                	sd	s1,40(sp)
    8060224a:	f04a                	sd	s2,32(sp)
    8060224c:	ec4e                	sd	s3,24(sp)
    8060224e:	e852                	sd	s4,16(sp)
    80602250:	8aaa                	mv	s5,a0
    80602252:	00000097          	auipc	ra,0x0
    80602256:	58a080e7          	jalr	1418(ra) # 806027dc <push_off>
    8060225a:	8792                	mv	a5,tp
    8060225c:	2781                	sext.w	a5,a5
    8060225e:	079e                	slli	a5,a5,0x7
    80602260:	0001fa17          	auipc	s4,0x1f
    80602264:	818a0a13          	addi	s4,s4,-2024 # 80620a78 <cpus>
    80602268:	97d2                	add	a5,a5,s4
    8060226a:	6384                	ld	s1,0(a5)
    8060226c:	00000097          	auipc	ra,0x0
    80602270:	600080e7          	jalr	1536(ra) # 8060286c <pop_off>
    80602274:	0001f517          	auipc	a0,0x1f
    80602278:	c1c50513          	addi	a0,a0,-996 # 80620e90 <wait_lock>
    8060227c:	00000097          	auipc	ra,0x0
    80602280:	5a0080e7          	jalr	1440(ra) # 8060281c <acquire>
    80602284:	4995                	li	s3,5
    80602286:	00024917          	auipc	s2,0x24
    8060228a:	62290913          	addi	s2,s2,1570 # 806268a8 <trampoline>
    8060228e:	4701                	li	a4,0
    80602290:	0001f417          	auipc	s0,0x1f
    80602294:	c1840413          	addi	s0,s0,-1000 # 80620ea8 <proc>
    80602298:	a029                	j	806022a2 <kwait+0x62>
    8060229a:	16840413          	addi	s0,s0,360
    8060229e:	03240763          	beq	s0,s2,806022cc <kwait+0x8c>
    806022a2:	7c1c                	ld	a5,56(s0)
    806022a4:	fe979be3          	bne	a5,s1,8060229a <kwait+0x5a>
    806022a8:	8522                	mv	a0,s0
    806022aa:	00000097          	auipc	ra,0x0
    806022ae:	572080e7          	jalr	1394(ra) # 8060281c <acquire>
    806022b2:	4c1c                	lw	a5,24(s0)
    806022b4:	8522                	mv	a0,s0
    806022b6:	0d378163          	beq	a5,s3,80602378 <kwait+0x138>
    806022ba:	00000097          	auipc	ra,0x0
    806022be:	600080e7          	jalr	1536(ra) # 806028ba <release>
    806022c2:	16840413          	addi	s0,s0,360
    806022c6:	4705                	li	a4,1
    806022c8:	fd241de3          	bne	s0,s2,806022a2 <kwait+0x62>
    806022cc:	10070463          	beqz	a4,806023d4 <kwait+0x194>
    806022d0:	8526                	mv	a0,s1
    806022d2:	00000097          	auipc	ra,0x0
    806022d6:	54a080e7          	jalr	1354(ra) # 8060281c <acquire>
    806022da:	5480                	lw	s0,40(s1)
    806022dc:	8526                	mv	a0,s1
    806022de:	00000097          	auipc	ra,0x0
    806022e2:	5dc080e7          	jalr	1500(ra) # 806028ba <release>
    806022e6:	e47d                	bnez	s0,806023d4 <kwait+0x194>
    806022e8:	00000097          	auipc	ra,0x0
    806022ec:	4f4080e7          	jalr	1268(ra) # 806027dc <push_off>
    806022f0:	8792                	mv	a5,tp
    806022f2:	2781                	sext.w	a5,a5
    806022f4:	079e                	slli	a5,a5,0x7
    806022f6:	97d2                	add	a5,a5,s4
    806022f8:	6380                	ld	s0,0(a5)
    806022fa:	00000097          	auipc	ra,0x0
    806022fe:	572080e7          	jalr	1394(ra) # 8060286c <pop_off>
    80602302:	8522                	mv	a0,s0
    80602304:	00000097          	auipc	ra,0x0
    80602308:	518080e7          	jalr	1304(ra) # 8060281c <acquire>
    8060230c:	8522                	mv	a0,s0
    8060230e:	f004                	sd	s1,32(s0)
    80602310:	00000097          	auipc	ra,0x0
    80602314:	5aa080e7          	jalr	1450(ra) # 806028ba <release>
    80602318:	0001f517          	auipc	a0,0x1f
    8060231c:	b7850513          	addi	a0,a0,-1160 # 80620e90 <wait_lock>
    80602320:	00000097          	auipc	ra,0x0
    80602324:	59a080e7          	jalr	1434(ra) # 806028ba <release>
    80602328:	00000097          	auipc	ra,0x0
    8060232c:	4b4080e7          	jalr	1204(ra) # 806027dc <push_off>
    80602330:	8792                	mv	a5,tp
    80602332:	2781                	sext.w	a5,a5
    80602334:	079e                	slli	a5,a5,0x7
    80602336:	97d2                	add	a5,a5,s4
    80602338:	6380                	ld	s0,0(a5)
    8060233a:	00000097          	auipc	ra,0x0
    8060233e:	532080e7          	jalr	1330(ra) # 8060286c <pop_off>
    80602342:	8522                	mv	a0,s0
    80602344:	00000097          	auipc	ra,0x0
    80602348:	4d8080e7          	jalr	1240(ra) # 8060281c <acquire>
    8060234c:	701c                	ld	a5,32(s0)
    8060234e:	c799                	beqz	a5,8060235c <kwait+0x11c>
    80602350:	4789                	li	a5,2
    80602352:	cc1c                	sw	a5,24(s0)
    80602354:	00000097          	auipc	ra,0x0
    80602358:	cd6080e7          	jalr	-810(ra) # 8060202a <sched>
    8060235c:	8522                	mv	a0,s0
    8060235e:	00000097          	auipc	ra,0x0
    80602362:	55c080e7          	jalr	1372(ra) # 806028ba <release>
    80602366:	0001f517          	auipc	a0,0x1f
    8060236a:	b2a50513          	addi	a0,a0,-1238 # 80620e90 <wait_lock>
    8060236e:	00000097          	auipc	ra,0x0
    80602372:	4ae080e7          	jalr	1198(ra) # 8060281c <acquire>
    80602376:	bf21                	j	8060228e <kwait+0x4e>
    80602378:	03042903          	lw	s2,48(s0)
    8060237c:	000a8e63          	beqz	s5,80602398 <kwait+0x158>
    80602380:	64ac                	ld	a1,72(s1)
    80602382:	68a8                	ld	a0,80(s1)
    80602384:	8656                	mv	a2,s5
    80602386:	4711                	li	a4,4
    80602388:	02c40693          	addi	a3,s0,44
    8060238c:	00000097          	auipc	ra,0x0
    80602390:	70c080e7          	jalr	1804(ra) # 80602a98 <copyout>
    80602394:	04054a63          	bltz	a0,806023e8 <kwait+0x1a8>
    80602398:	8522                	mv	a0,s0
    8060239a:	02043c23          	sd	zero,56(s0)
    8060239e:	fffff097          	auipc	ra,0xfffff
    806023a2:	464080e7          	jalr	1124(ra) # 80601802 <freeproc>
    806023a6:	8522                	mv	a0,s0
    806023a8:	00000097          	auipc	ra,0x0
    806023ac:	512080e7          	jalr	1298(ra) # 806028ba <release>
    806023b0:	0001f517          	auipc	a0,0x1f
    806023b4:	ae050513          	addi	a0,a0,-1312 # 80620e90 <wait_lock>
    806023b8:	00000097          	auipc	ra,0x0
    806023bc:	502080e7          	jalr	1282(ra) # 806028ba <release>
    806023c0:	70e2                	ld	ra,56(sp)
    806023c2:	7442                	ld	s0,48(sp)
    806023c4:	74a2                	ld	s1,40(sp)
    806023c6:	69e2                	ld	s3,24(sp)
    806023c8:	6a42                	ld	s4,16(sp)
    806023ca:	6aa2                	ld	s5,8(sp)
    806023cc:	854a                	mv	a0,s2
    806023ce:	7902                	ld	s2,32(sp)
    806023d0:	6121                	addi	sp,sp,64
    806023d2:	8082                	ret
    806023d4:	0001f517          	auipc	a0,0x1f
    806023d8:	abc50513          	addi	a0,a0,-1348 # 80620e90 <wait_lock>
    806023dc:	00000097          	auipc	ra,0x0
    806023e0:	4de080e7          	jalr	1246(ra) # 806028ba <release>
    806023e4:	597d                	li	s2,-1
    806023e6:	bfe9                	j	806023c0 <kwait+0x180>
    806023e8:	8522                	mv	a0,s0
    806023ea:	00000097          	auipc	ra,0x0
    806023ee:	4d0080e7          	jalr	1232(ra) # 806028ba <release>
    806023f2:	0001f517          	auipc	a0,0x1f
    806023f6:	a9e50513          	addi	a0,a0,-1378 # 80620e90 <wait_lock>
    806023fa:	00000097          	auipc	ra,0x0
    806023fe:	4c0080e7          	jalr	1216(ra) # 806028ba <release>
    80602402:	597d                	li	s2,-1
    80602404:	bf75                	j	806023c0 <kwait+0x180>

0000000080602406 <yield>:
    80602406:	1141                	addi	sp,sp,-16
    80602408:	e406                	sd	ra,8(sp)
    8060240a:	e022                	sd	s0,0(sp)
    8060240c:	00000097          	auipc	ra,0x0
    80602410:	3d0080e7          	jalr	976(ra) # 806027dc <push_off>
    80602414:	8792                	mv	a5,tp
    80602416:	2781                	sext.w	a5,a5
    80602418:	0001e717          	auipc	a4,0x1e
    8060241c:	66070713          	addi	a4,a4,1632 # 80620a78 <cpus>
    80602420:	079e                	slli	a5,a5,0x7
    80602422:	97ba                	add	a5,a5,a4
    80602424:	6380                	ld	s0,0(a5)
    80602426:	00000097          	auipc	ra,0x0
    8060242a:	446080e7          	jalr	1094(ra) # 8060286c <pop_off>
    8060242e:	8522                	mv	a0,s0
    80602430:	00000097          	auipc	ra,0x0
    80602434:	3ec080e7          	jalr	1004(ra) # 8060281c <acquire>
    80602438:	478d                	li	a5,3
    8060243a:	cc1c                	sw	a5,24(s0)
    8060243c:	00000097          	auipc	ra,0x0
    80602440:	bee080e7          	jalr	-1042(ra) # 8060202a <sched>
    80602444:	8522                	mv	a0,s0
    80602446:	6402                	ld	s0,0(sp)
    80602448:	60a2                	ld	ra,8(sp)
    8060244a:	0141                	addi	sp,sp,16
    8060244c:	00000317          	auipc	t1,0x0
    80602450:	46e30067          	jr	1134(t1) # 806028ba <release>

0000000080602454 <sleep_prepare>:
    80602454:	1101                	addi	sp,sp,-32
    80602456:	e426                	sd	s1,8(sp)
    80602458:	ec06                	sd	ra,24(sp)
    8060245a:	e822                	sd	s0,16(sp)
    8060245c:	84aa                	mv	s1,a0
    8060245e:	00000097          	auipc	ra,0x0
    80602462:	37e080e7          	jalr	894(ra) # 806027dc <push_off>
    80602466:	8792                	mv	a5,tp
    80602468:	2781                	sext.w	a5,a5
    8060246a:	0001e717          	auipc	a4,0x1e
    8060246e:	60e70713          	addi	a4,a4,1550 # 80620a78 <cpus>
    80602472:	079e                	slli	a5,a5,0x7
    80602474:	97ba                	add	a5,a5,a4
    80602476:	6380                	ld	s0,0(a5)
    80602478:	00000097          	auipc	ra,0x0
    8060247c:	3f4080e7          	jalr	1012(ra) # 8060286c <pop_off>
    80602480:	8522                	mv	a0,s0
    80602482:	00000097          	auipc	ra,0x0
    80602486:	39a080e7          	jalr	922(ra) # 8060281c <acquire>
    8060248a:	c899                	beqz	s1,806024a0 <sleep_prepare+0x4c>
    8060248c:	f004                	sd	s1,32(s0)
    8060248e:	8522                	mv	a0,s0
    80602490:	6442                	ld	s0,16(sp)
    80602492:	60e2                	ld	ra,24(sp)
    80602494:	64a2                	ld	s1,8(sp)
    80602496:	6105                	addi	sp,sp,32
    80602498:	00000317          	auipc	t1,0x0
    8060249c:	42230067          	jr	1058(t1) # 806028ba <release>
    806024a0:	00001517          	auipc	a0,0x1
    806024a4:	c6050513          	addi	a0,a0,-928 # 80603100 <kexec+0x63a>
    806024a8:	fffff097          	auipc	ra,0xfffff
    806024ac:	302080e7          	jalr	770(ra) # 806017aa <panic>

00000000806024b0 <sleep>:
    806024b0:	1141                	addi	sp,sp,-16
    806024b2:	e406                	sd	ra,8(sp)
    806024b4:	e022                	sd	s0,0(sp)
    806024b6:	00000097          	auipc	ra,0x0
    806024ba:	326080e7          	jalr	806(ra) # 806027dc <push_off>
    806024be:	8792                	mv	a5,tp
    806024c0:	2781                	sext.w	a5,a5
    806024c2:	0001e717          	auipc	a4,0x1e
    806024c6:	5b670713          	addi	a4,a4,1462 # 80620a78 <cpus>
    806024ca:	079e                	slli	a5,a5,0x7
    806024cc:	97ba                	add	a5,a5,a4
    806024ce:	6380                	ld	s0,0(a5)
    806024d0:	00000097          	auipc	ra,0x0
    806024d4:	39c080e7          	jalr	924(ra) # 8060286c <pop_off>
    806024d8:	8522                	mv	a0,s0
    806024da:	00000097          	auipc	ra,0x0
    806024de:	342080e7          	jalr	834(ra) # 8060281c <acquire>
    806024e2:	701c                	ld	a5,32(s0)
    806024e4:	c799                	beqz	a5,806024f2 <sleep+0x42>
    806024e6:	4789                	li	a5,2
    806024e8:	cc1c                	sw	a5,24(s0)
    806024ea:	00000097          	auipc	ra,0x0
    806024ee:	b40080e7          	jalr	-1216(ra) # 8060202a <sched>
    806024f2:	8522                	mv	a0,s0
    806024f4:	6402                	ld	s0,0(sp)
    806024f6:	60a2                	ld	ra,8(sp)
    806024f8:	0141                	addi	sp,sp,16
    806024fa:	00000317          	auipc	t1,0x0
    806024fe:	3c030067          	jr	960(t1) # 806028ba <release>

0000000080602502 <wakeup>:
    80602502:	7179                	addi	sp,sp,-48
    80602504:	f022                	sd	s0,32(sp)
    80602506:	ec26                	sd	s1,24(sp)
    80602508:	e84a                	sd	s2,16(sp)
    8060250a:	e44e                	sd	s3,8(sp)
    8060250c:	e052                	sd	s4,0(sp)
    8060250e:	f406                	sd	ra,40(sp)
    80602510:	892a                	mv	s2,a0
    80602512:	0001f417          	auipc	s0,0x1f
    80602516:	99640413          	addi	s0,s0,-1642 # 80620ea8 <proc>
    8060251a:	00024497          	auipc	s1,0x24
    8060251e:	38e48493          	addi	s1,s1,910 # 806268a8 <trampoline>
    80602522:	4989                	li	s3,2
    80602524:	4a0d                	li	s4,3
    80602526:	a809                	j	80602538 <wakeup+0x36>
    80602528:	16840413          	addi	s0,s0,360
    8060252c:	00000097          	auipc	ra,0x0
    80602530:	38e080e7          	jalr	910(ra) # 806028ba <release>
    80602534:	02940a63          	beq	s0,s1,80602568 <wakeup+0x66>
    80602538:	8522                	mv	a0,s0
    8060253a:	00000097          	auipc	ra,0x0
    8060253e:	2e2080e7          	jalr	738(ra) # 8060281c <acquire>
    80602542:	701c                	ld	a5,32(s0)
    80602544:	8522                	mv	a0,s0
    80602546:	ff2791e3          	bne	a5,s2,80602528 <wakeup+0x26>
    8060254a:	4c1c                	lw	a5,24(s0)
    8060254c:	02043023          	sd	zero,32(s0)
    80602550:	fd379ce3          	bne	a5,s3,80602528 <wakeup+0x26>
    80602554:	01442c23          	sw	s4,24(s0)
    80602558:	16840413          	addi	s0,s0,360
    8060255c:	00000097          	auipc	ra,0x0
    80602560:	35e080e7          	jalr	862(ra) # 806028ba <release>
    80602564:	fc941ae3          	bne	s0,s1,80602538 <wakeup+0x36>
    80602568:	70a2                	ld	ra,40(sp)
    8060256a:	7402                	ld	s0,32(sp)
    8060256c:	64e2                	ld	s1,24(sp)
    8060256e:	6942                	ld	s2,16(sp)
    80602570:	69a2                	ld	s3,8(sp)
    80602572:	6a02                	ld	s4,0(sp)
    80602574:	6145                	addi	sp,sp,48
    80602576:	8082                	ret

0000000080602578 <kkill>:
    80602578:	cd25                	beqz	a0,806025f0 <kkill+0x78>
    8060257a:	1101                	addi	sp,sp,-32
    8060257c:	e822                	sd	s0,16(sp)
    8060257e:	e426                	sd	s1,8(sp)
    80602580:	e04a                	sd	s2,0(sp)
    80602582:	ec06                	sd	ra,24(sp)
    80602584:	84aa                	mv	s1,a0
    80602586:	0001f417          	auipc	s0,0x1f
    8060258a:	92240413          	addi	s0,s0,-1758 # 80620ea8 <proc>
    8060258e:	00024917          	auipc	s2,0x24
    80602592:	31a90913          	addi	s2,s2,794 # 806268a8 <trampoline>
    80602596:	a809                	j	806025a8 <kkill+0x30>
    80602598:	16840413          	addi	s0,s0,360
    8060259c:	00000097          	auipc	ra,0x0
    806025a0:	31e080e7          	jalr	798(ra) # 806028ba <release>
    806025a4:	03240f63          	beq	s0,s2,806025e2 <kkill+0x6a>
    806025a8:	8522                	mv	a0,s0
    806025aa:	00000097          	auipc	ra,0x0
    806025ae:	272080e7          	jalr	626(ra) # 8060281c <acquire>
    806025b2:	581c                	lw	a5,48(s0)
    806025b4:	8522                	mv	a0,s0
    806025b6:	fe9791e3          	bne	a5,s1,80602598 <kkill+0x20>
    806025ba:	4c18                	lw	a4,24(s0)
    806025bc:	4785                	li	a5,1
    806025be:	d41c                	sw	a5,40(s0)
    806025c0:	4789                	li	a5,2
    806025c2:	00f71463          	bne	a4,a5,806025ca <kkill+0x52>
    806025c6:	478d                	li	a5,3
    806025c8:	cc1c                	sw	a5,24(s0)
    806025ca:	8522                	mv	a0,s0
    806025cc:	00000097          	auipc	ra,0x0
    806025d0:	2ee080e7          	jalr	750(ra) # 806028ba <release>
    806025d4:	60e2                	ld	ra,24(sp)
    806025d6:	6442                	ld	s0,16(sp)
    806025d8:	64a2                	ld	s1,8(sp)
    806025da:	6902                	ld	s2,0(sp)
    806025dc:	4501                	li	a0,0
    806025de:	6105                	addi	sp,sp,32
    806025e0:	8082                	ret
    806025e2:	60e2                	ld	ra,24(sp)
    806025e4:	6442                	ld	s0,16(sp)
    806025e6:	64a2                	ld	s1,8(sp)
    806025e8:	6902                	ld	s2,0(sp)
    806025ea:	557d                	li	a0,-1
    806025ec:	6105                	addi	sp,sp,32
    806025ee:	8082                	ret
    806025f0:	557d                	li	a0,-1
    806025f2:	8082                	ret

00000000806025f4 <setkilled>:
    806025f4:	1101                	addi	sp,sp,-32
    806025f6:	ec06                	sd	ra,24(sp)
    806025f8:	e42a                	sd	a0,8(sp)
    806025fa:	00000097          	auipc	ra,0x0
    806025fe:	222080e7          	jalr	546(ra) # 8060281c <acquire>
    80602602:	6522                	ld	a0,8(sp)
    80602604:	60e2                	ld	ra,24(sp)
    80602606:	4785                	li	a5,1
    80602608:	d51c                	sw	a5,40(a0)
    8060260a:	6105                	addi	sp,sp,32
    8060260c:	00000317          	auipc	t1,0x0
    80602610:	2ae30067          	jr	686(t1) # 806028ba <release>

0000000080602614 <killed>:
    80602614:	1101                	addi	sp,sp,-32
    80602616:	ec06                	sd	ra,24(sp)
    80602618:	e822                	sd	s0,16(sp)
    8060261a:	e42a                	sd	a0,8(sp)
    8060261c:	00000097          	auipc	ra,0x0
    80602620:	200080e7          	jalr	512(ra) # 8060281c <acquire>
    80602624:	6522                	ld	a0,8(sp)
    80602626:	551c                	lw	a5,40(a0)
    80602628:	843e                	mv	s0,a5
    8060262a:	00000097          	auipc	ra,0x0
    8060262e:	290080e7          	jalr	656(ra) # 806028ba <release>
    80602632:	60e2                	ld	ra,24(sp)
    80602634:	8522                	mv	a0,s0
    80602636:	6442                	ld	s0,16(sp)
    80602638:	6105                	addi	sp,sp,32
    8060263a:	8082                	ret

000000008060263c <either_copyout>:
    8060263c:	7179                	addi	sp,sp,-48
    8060263e:	ec26                	sd	s1,24(sp)
    80602640:	e84a                	sd	s2,16(sp)
    80602642:	f406                	sd	ra,40(sp)
    80602644:	f022                	sd	s0,32(sp)
    80602646:	e42a                	sd	a0,8(sp)
    80602648:	892e                	mv	s2,a1
    8060264a:	84b2                	mv	s1,a2
    8060264c:	e036                	sd	a3,0(sp)
    8060264e:	00000097          	auipc	ra,0x0
    80602652:	18e080e7          	jalr	398(ra) # 806027dc <push_off>
    80602656:	8792                	mv	a5,tp
    80602658:	2781                	sext.w	a5,a5
    8060265a:	0001e717          	auipc	a4,0x1e
    8060265e:	41e70713          	addi	a4,a4,1054 # 80620a78 <cpus>
    80602662:	079e                	slli	a5,a5,0x7
    80602664:	97ba                	add	a5,a5,a4
    80602666:	6380                	ld	s0,0(a5)
    80602668:	00000097          	auipc	ra,0x0
    8060266c:	204080e7          	jalr	516(ra) # 8060286c <pop_off>
    80602670:	6522                	ld	a0,8(sp)
    80602672:	cd19                	beqz	a0,80602690 <either_copyout+0x54>
    80602674:	642c                	ld	a1,72(s0)
    80602676:	6828                	ld	a0,80(s0)
    80602678:	7402                	ld	s0,32(sp)
    8060267a:	6702                	ld	a4,0(sp)
    8060267c:	70a2                	ld	ra,40(sp)
    8060267e:	86a6                	mv	a3,s1
    80602680:	864a                	mv	a2,s2
    80602682:	64e2                	ld	s1,24(sp)
    80602684:	6942                	ld	s2,16(sp)
    80602686:	6145                	addi	sp,sp,48
    80602688:	00000317          	auipc	t1,0x0
    8060268c:	41030067          	jr	1040(t1) # 80602a98 <copyout>
    80602690:	4602                	lw	a2,0(sp)
    80602692:	85a6                	mv	a1,s1
    80602694:	854a                	mv	a0,s2
    80602696:	00000097          	auipc	ra,0x0
    8060269a:	2aa080e7          	jalr	682(ra) # 80602940 <memmove>
    8060269e:	70a2                	ld	ra,40(sp)
    806026a0:	7402                	ld	s0,32(sp)
    806026a2:	64e2                	ld	s1,24(sp)
    806026a4:	6942                	ld	s2,16(sp)
    806026a6:	4501                	li	a0,0
    806026a8:	6145                	addi	sp,sp,48
    806026aa:	8082                	ret

00000000806026ac <either_copyin>:
    806026ac:	7179                	addi	sp,sp,-48
    806026ae:	ec26                	sd	s1,24(sp)
    806026b0:	e84a                	sd	s2,16(sp)
    806026b2:	f406                	sd	ra,40(sp)
    806026b4:	f022                	sd	s0,32(sp)
    806026b6:	892a                	mv	s2,a0
    806026b8:	e42e                	sd	a1,8(sp)
    806026ba:	84b2                	mv	s1,a2
    806026bc:	e036                	sd	a3,0(sp)
    806026be:	00000097          	auipc	ra,0x0
    806026c2:	11e080e7          	jalr	286(ra) # 806027dc <push_off>
    806026c6:	8792                	mv	a5,tp
    806026c8:	2781                	sext.w	a5,a5
    806026ca:	0001e717          	auipc	a4,0x1e
    806026ce:	3ae70713          	addi	a4,a4,942 # 80620a78 <cpus>
    806026d2:	079e                	slli	a5,a5,0x7
    806026d4:	97ba                	add	a5,a5,a4
    806026d6:	6380                	ld	s0,0(a5)
    806026d8:	00000097          	auipc	ra,0x0
    806026dc:	194080e7          	jalr	404(ra) # 8060286c <pop_off>
    806026e0:	65a2                	ld	a1,8(sp)
    806026e2:	cd99                	beqz	a1,80602700 <either_copyin+0x54>
    806026e4:	642c                	ld	a1,72(s0)
    806026e6:	6828                	ld	a0,80(s0)
    806026e8:	7402                	ld	s0,32(sp)
    806026ea:	6702                	ld	a4,0(sp)
    806026ec:	70a2                	ld	ra,40(sp)
    806026ee:	86a6                	mv	a3,s1
    806026f0:	864a                	mv	a2,s2
    806026f2:	64e2                	ld	s1,24(sp)
    806026f4:	6942                	ld	s2,16(sp)
    806026f6:	6145                	addi	sp,sp,48
    806026f8:	00000317          	auipc	t1,0x0
    806026fc:	38430067          	jr	900(t1) # 80602a7c <copyin>
    80602700:	4602                	lw	a2,0(sp)
    80602702:	85a6                	mv	a1,s1
    80602704:	854a                	mv	a0,s2
    80602706:	00000097          	auipc	ra,0x0
    8060270a:	23a080e7          	jalr	570(ra) # 80602940 <memmove>
    8060270e:	70a2                	ld	ra,40(sp)
    80602710:	7402                	ld	s0,32(sp)
    80602712:	64e2                	ld	s1,24(sp)
    80602714:	6942                	ld	s2,16(sp)
    80602716:	4501                	li	a0,0
    80602718:	6145                	addi	sp,sp,48
    8060271a:	8082                	ret

000000008060271c <procdump>:
    8060271c:	7179                	addi	sp,sp,-48
    8060271e:	00000517          	auipc	a0,0x0
    80602722:	40a50513          	addi	a0,a0,1034 # 80602b28 <kexec+0x62>
    80602726:	f022                	sd	s0,32(sp)
    80602728:	ec26                	sd	s1,24(sp)
    8060272a:	e84a                	sd	s2,16(sp)
    8060272c:	e44e                	sd	s3,8(sp)
    8060272e:	f406                	sd	ra,40(sp)
    80602730:	0001f417          	auipc	s0,0x1f
    80602734:	8d040413          	addi	s0,s0,-1840 # 80621000 <proc+0x158>
    80602738:	fffff097          	auipc	ra,0xfffff
    8060273c:	dc2080e7          	jalr	-574(ra) # 806014fa <printk>
    80602740:	00024497          	auipc	s1,0x24
    80602744:	2c048493          	addi	s1,s1,704 # 80626a00 <trampoline+0x158>
    80602748:	4995                	li	s3,5
    8060274a:	00001917          	auipc	s2,0x1
    8060274e:	afe90913          	addi	s2,s2,-1282 # 80603248 <states.0>
    80602752:	ec042783          	lw	a5,-320(s0)
    80602756:	86a2                	mv	a3,s0
    80602758:	00001517          	auipc	a0,0x1
    8060275c:	9d050513          	addi	a0,a0,-1584 # 80603128 <kexec+0x662>
    80602760:	cb8d                	beqz	a5,80602792 <procdump+0x76>
    80602762:	00379713          	slli	a4,a5,0x3
    80602766:	974a                	add	a4,a4,s2
    80602768:	00001617          	auipc	a2,0x1
    8060276c:	9b860613          	addi	a2,a2,-1608 # 80603120 <kexec+0x65a>
    80602770:	00f9e363          	bltu	s3,a5,80602776 <procdump+0x5a>
    80602774:	6310                	ld	a2,0(a4)
    80602776:	ed842583          	lw	a1,-296(s0)
    8060277a:	fffff097          	auipc	ra,0xfffff
    8060277e:	d80080e7          	jalr	-640(ra) # 806014fa <printk>
    80602782:	00000517          	auipc	a0,0x0
    80602786:	3a650513          	addi	a0,a0,934 # 80602b28 <kexec+0x62>
    8060278a:	fffff097          	auipc	ra,0xfffff
    8060278e:	d70080e7          	jalr	-656(ra) # 806014fa <printk>
    80602792:	16840413          	addi	s0,s0,360
    80602796:	fa941ee3          	bne	s0,s1,80602752 <procdump+0x36>
    8060279a:	70a2                	ld	ra,40(sp)
    8060279c:	7402                	ld	s0,32(sp)
    8060279e:	64e2                	ld	s1,24(sp)
    806027a0:	6942                	ld	s2,16(sp)
    806027a2:	69a2                	ld	s3,8(sp)
    806027a4:	6145                	addi	sp,sp,48
    806027a6:	8082                	ret

00000000806027a8 <initlock>:
    806027a8:	00052023          	sw	zero,0(a0)
    806027ac:	00053823          	sd	zero,16(a0)
    806027b0:	e50c                	sd	a1,8(a0)
    806027b2:	8082                	ret

00000000806027b4 <holding>:
    806027b4:	411c                	lw	a5,0(a0)
    806027b6:	e399                	bnez	a5,806027bc <holding+0x8>
    806027b8:	4501                	li	a0,0
    806027ba:	8082                	ret
    806027bc:	691c                	ld	a5,16(a0)
    806027be:	1101                	addi	sp,sp,-32
    806027c0:	ec06                	sd	ra,24(sp)
    806027c2:	e43e                	sd	a5,8(sp)
    806027c4:	fffff097          	auipc	ra,0xfffff
    806027c8:	2b8080e7          	jalr	696(ra) # 80601a7c <mycpu>
    806027cc:	67a2                	ld	a5,8(sp)
    806027ce:	60e2                	ld	ra,24(sp)
    806027d0:	40a78533          	sub	a0,a5,a0
    806027d4:	00153513          	seqz	a0,a0
    806027d8:	6105                	addi	sp,sp,32
    806027da:	8082                	ret

00000000806027dc <push_off>:
    806027dc:	1141                	addi	sp,sp,-16
    806027de:	e406                	sd	ra,8(sp)
    806027e0:	e022                	sd	s0,0(sp)
    806027e2:	100177f3          	csrrci	a5,sstatus,2
    806027e6:	843e                	mv	s0,a5
    806027e8:	fffff097          	auipc	ra,0xfffff
    806027ec:	294080e7          	jalr	660(ra) # 80601a7c <mycpu>
    806027f0:	5d3c                	lw	a5,120(a0)
    806027f2:	cf81                	beqz	a5,8060280a <push_off+0x2e>
    806027f4:	fffff097          	auipc	ra,0xfffff
    806027f8:	288080e7          	jalr	648(ra) # 80601a7c <mycpu>
    806027fc:	5d3c                	lw	a5,120(a0)
    806027fe:	60a2                	ld	ra,8(sp)
    80602800:	6402                	ld	s0,0(sp)
    80602802:	2785                	addiw	a5,a5,1
    80602804:	dd3c                	sw	a5,120(a0)
    80602806:	0141                	addi	sp,sp,16
    80602808:	8082                	ret
    8060280a:	fffff097          	auipc	ra,0xfffff
    8060280e:	272080e7          	jalr	626(ra) # 80601a7c <mycpu>
    80602812:	00145793          	srli	a5,s0,0x1
    80602816:	8b85                	andi	a5,a5,1
    80602818:	dd7c                	sw	a5,124(a0)
    8060281a:	bfe9                	j	806027f4 <push_off+0x18>

000000008060281c <acquire>:
    8060281c:	1101                	addi	sp,sp,-32
    8060281e:	e822                	sd	s0,16(sp)
    80602820:	ec06                	sd	ra,24(sp)
    80602822:	842a                	mv	s0,a0
    80602824:	00000097          	auipc	ra,0x0
    80602828:	fb8080e7          	jalr	-72(ra) # 806027dc <push_off>
    8060282c:	401c                	lw	a5,0(s0)
    8060282e:	ef91                	bnez	a5,8060284a <acquire+0x2e>
    80602830:	4705                	li	a4,1
    80602832:	0ce427af          	amoswap.w.aq	a5,a4,(s0)
    80602836:	fff5                	bnez	a5,80602832 <acquire+0x16>
    80602838:	fffff097          	auipc	ra,0xfffff
    8060283c:	244080e7          	jalr	580(ra) # 80601a7c <mycpu>
    80602840:	60e2                	ld	ra,24(sp)
    80602842:	e808                	sd	a0,16(s0)
    80602844:	6442                	ld	s0,16(sp)
    80602846:	6105                	addi	sp,sp,32
    80602848:	8082                	ret
    8060284a:	681c                	ld	a5,16(s0)
    8060284c:	e43e                	sd	a5,8(sp)
    8060284e:	fffff097          	auipc	ra,0xfffff
    80602852:	22e080e7          	jalr	558(ra) # 80601a7c <mycpu>
    80602856:	67a2                	ld	a5,8(sp)
    80602858:	fca79ce3          	bne	a5,a0,80602830 <acquire+0x14>
    8060285c:	00001517          	auipc	a0,0x1
    80602860:	90c50513          	addi	a0,a0,-1780 # 80603168 <kexec+0x6a2>
    80602864:	fffff097          	auipc	ra,0xfffff
    80602868:	f46080e7          	jalr	-186(ra) # 806017aa <panic>

000000008060286c <pop_off>:
    8060286c:	1141                	addi	sp,sp,-16
    8060286e:	e406                	sd	ra,8(sp)
    80602870:	fffff097          	auipc	ra,0xfffff
    80602874:	20c080e7          	jalr	524(ra) # 80601a7c <mycpu>
    80602878:	100027f3          	csrr	a5,sstatus
    8060287c:	8b89                	andi	a5,a5,2
    8060287e:	e795                	bnez	a5,806028aa <pop_off+0x3e>
    80602880:	5d3c                	lw	a5,120(a0)
    80602882:	00f05c63          	blez	a5,8060289a <pop_off+0x2e>
    80602886:	37fd                	addiw	a5,a5,-1
    80602888:	dd3c                	sw	a5,120(a0)
    8060288a:	e789                	bnez	a5,80602894 <pop_off+0x28>
    8060288c:	5d7c                	lw	a5,124(a0)
    8060288e:	c399                	beqz	a5,80602894 <pop_off+0x28>
    80602890:	10016073          	csrsi	sstatus,2
    80602894:	60a2                	ld	ra,8(sp)
    80602896:	0141                	addi	sp,sp,16
    80602898:	8082                	ret
    8060289a:	00001517          	auipc	a0,0x1
    8060289e:	8ee50513          	addi	a0,a0,-1810 # 80603188 <kexec+0x6c2>
    806028a2:	fffff097          	auipc	ra,0xfffff
    806028a6:	f08080e7          	jalr	-248(ra) # 806017aa <panic>
    806028aa:	00001517          	auipc	a0,0x1
    806028ae:	8c650513          	addi	a0,a0,-1850 # 80603170 <kexec+0x6aa>
    806028b2:	fffff097          	auipc	ra,0xfffff
    806028b6:	ef8080e7          	jalr	-264(ra) # 806017aa <panic>

00000000806028ba <release>:
    806028ba:	4118                	lw	a4,0(a0)
    806028bc:	1101                	addi	sp,sp,-32
    806028be:	ec06                	sd	ra,24(sp)
    806028c0:	eb09                	bnez	a4,806028d2 <release+0x18>
    806028c2:	00001517          	auipc	a0,0x1
    806028c6:	8ce50513          	addi	a0,a0,-1842 # 80603190 <kexec+0x6ca>
    806028ca:	fffff097          	auipc	ra,0xfffff
    806028ce:	ee0080e7          	jalr	-288(ra) # 806017aa <panic>
    806028d2:	6918                	ld	a4,16(a0)
    806028d4:	e42a                	sd	a0,8(sp)
    806028d6:	e03a                	sd	a4,0(sp)
    806028d8:	fffff097          	auipc	ra,0xfffff
    806028dc:	1a4080e7          	jalr	420(ra) # 80601a7c <mycpu>
    806028e0:	6702                	ld	a4,0(sp)
    806028e2:	67a2                	ld	a5,8(sp)
    806028e4:	fca71fe3          	bne	a4,a0,806028c2 <release+0x8>
    806028e8:	0007b823          	sd	zero,16(a5)
    806028ec:	0310000f          	fence	rw,w
    806028f0:	0007a023          	sw	zero,0(a5)
    806028f4:	60e2                	ld	ra,24(sp)
    806028f6:	6105                	addi	sp,sp,32
    806028f8:	00000317          	auipc	t1,0x0
    806028fc:	f7430067          	jr	-140(t1) # 8060286c <pop_off>

0000000080602900 <memset>:
    80602900:	ca11                	beqz	a2,80602914 <memset+0x14>
    80602902:	1602                	slli	a2,a2,0x20
    80602904:	9201                	srli	a2,a2,0x20
    80602906:	87aa                	mv	a5,a0
    80602908:	962a                	add	a2,a2,a0
    8060290a:	00b78023          	sb	a1,0(a5)
    8060290e:	0785                	addi	a5,a5,1
    80602910:	fec79de3          	bne	a5,a2,8060290a <memset+0xa>
    80602914:	8082                	ret

0000000080602916 <memcmp>:
    80602916:	02061693          	slli	a3,a2,0x20
    8060291a:	9281                	srli	a3,a3,0x20
    8060291c:	96aa                	add	a3,a3,a0
    8060291e:	e601                	bnez	a2,80602926 <memcmp+0x10>
    80602920:	a831                	j	8060293c <memcmp+0x26>
    80602922:	00a68d63          	beq	a3,a0,8060293c <memcmp+0x26>
    80602926:	00054783          	lbu	a5,0(a0)
    8060292a:	0005c703          	lbu	a4,0(a1)
    8060292e:	0505                	addi	a0,a0,1
    80602930:	0585                	addi	a1,a1,1
    80602932:	fee788e3          	beq	a5,a4,80602922 <memcmp+0xc>
    80602936:	40e7853b          	subw	a0,a5,a4
    8060293a:	8082                	ret
    8060293c:	4501                	li	a0,0
    8060293e:	8082                	ret

0000000080602940 <memmove>:
    80602940:	c629                	beqz	a2,8060298a <memmove+0x4a>
    80602942:	02061713          	slli	a4,a2,0x20
    80602946:	9301                	srli	a4,a4,0x20
    80602948:	00a5ed63          	bltu	a1,a0,80602962 <memmove+0x22>
    8060294c:	972e                	add	a4,a4,a1
    8060294e:	87aa                	mv	a5,a0
    80602950:	0005c683          	lbu	a3,0(a1)
    80602954:	0585                	addi	a1,a1,1
    80602956:	0785                	addi	a5,a5,1
    80602958:	fed78fa3          	sb	a3,-1(a5)
    8060295c:	feb71ae3          	bne	a4,a1,80602950 <memmove+0x10>
    80602960:	8082                	ret
    80602962:	00e587b3          	add	a5,a1,a4
    80602966:	fef573e3          	bgeu	a0,a5,8060294c <memmove+0xc>
    8060296a:	fff6069b          	addiw	a3,a2,-1
    8060296e:	1682                	slli	a3,a3,0x20
    80602970:	9281                	srli	a3,a3,0x20
    80602972:	fff6c693          	not	a3,a3
    80602976:	972a                	add	a4,a4,a0
    80602978:	96be                	add	a3,a3,a5
    8060297a:	fff7c603          	lbu	a2,-1(a5)
    8060297e:	17fd                	addi	a5,a5,-1
    80602980:	177d                	addi	a4,a4,-1
    80602982:	00c70023          	sb	a2,0(a4)
    80602986:	fef69ae3          	bne	a3,a5,8060297a <memmove+0x3a>
    8060298a:	8082                	ret

000000008060298c <memcpy>:
    8060298c:	00000317          	auipc	t1,0x0
    80602990:	fb430067          	jr	-76(t1) # 80602940 <memmove>

0000000080602994 <strncmp>:
    80602994:	ce01                	beqz	a2,806029ac <strncmp+0x18>
    80602996:	00054783          	lbu	a5,0(a0)
    8060299a:	367d                	addiw	a2,a2,-1
    8060299c:	cb91                	beqz	a5,806029b0 <strncmp+0x1c>
    8060299e:	0005c703          	lbu	a4,0(a1)
    806029a2:	00f71763          	bne	a4,a5,806029b0 <strncmp+0x1c>
    806029a6:	0505                	addi	a0,a0,1
    806029a8:	0585                	addi	a1,a1,1
    806029aa:	f675                	bnez	a2,80602996 <strncmp+0x2>
    806029ac:	4501                	li	a0,0
    806029ae:	8082                	ret
    806029b0:	00054503          	lbu	a0,0(a0)
    806029b4:	0005c783          	lbu	a5,0(a1)
    806029b8:	9d1d                	subw	a0,a0,a5
    806029ba:	8082                	ret

00000000806029bc <strncpy>:
    806029bc:	87aa                	mv	a5,a0
    806029be:	a811                	j	806029d2 <strncpy+0x16>
    806029c0:	0005c703          	lbu	a4,0(a1)
    806029c4:	fff6069b          	addiw	a3,a2,-1
    806029c8:	0585                	addi	a1,a1,1
    806029ca:	fee78fa3          	sb	a4,-1(a5)
    806029ce:	c711                	beqz	a4,806029da <strncpy+0x1e>
    806029d0:	8636                	mv	a2,a3
    806029d2:	0785                	addi	a5,a5,1
    806029d4:	fec046e3          	bgtz	a2,806029c0 <strncpy+0x4>
    806029d8:	8082                	ret
    806029da:	9e3d                	addw	a2,a2,a5
    806029dc:	367d                	addiw	a2,a2,-1
    806029de:	deed                	beqz	a3,806029d8 <strncpy+0x1c>
    806029e0:	0785                	addi	a5,a5,1
    806029e2:	40f6073b          	subw	a4,a2,a5
    806029e6:	fe078fa3          	sb	zero,-1(a5)
    806029ea:	fee04be3          	bgtz	a4,806029e0 <strncpy+0x24>
    806029ee:	8082                	ret

00000000806029f0 <safestrcpy>:
    806029f0:	02c05063          	blez	a2,80602a10 <safestrcpy+0x20>
    806029f4:	367d                	addiw	a2,a2,-1
    806029f6:	962e                	add	a2,a2,a1
    806029f8:	87aa                	mv	a5,a0
    806029fa:	00c58963          	beq	a1,a2,80602a0c <safestrcpy+0x1c>
    806029fe:	0005c703          	lbu	a4,0(a1)
    80602a02:	0585                	addi	a1,a1,1
    80602a04:	0785                	addi	a5,a5,1
    80602a06:	fee78fa3          	sb	a4,-1(a5)
    80602a0a:	fb65                	bnez	a4,806029fa <safestrcpy+0xa>
    80602a0c:	00078023          	sb	zero,0(a5)
    80602a10:	8082                	ret

0000000080602a12 <strlen>:
    80602a12:	00054783          	lbu	a5,0(a0)
    80602a16:	cb99                	beqz	a5,80602a2c <strlen+0x1a>
    80602a18:	00150793          	addi	a5,a0,1
    80602a1c:	86be                	mv	a3,a5
    80602a1e:	0007c703          	lbu	a4,0(a5)
    80602a22:	0785                	addi	a5,a5,1
    80602a24:	ff65                	bnez	a4,80602a1c <strlen+0xa>
    80602a26:	40a6853b          	subw	a0,a3,a0
    80602a2a:	8082                	ret
    80602a2c:	4501                	li	a0,0
    80602a2e:	8082                	ret

0000000080602a30 <kalloc>:
    80602a30:	ffffe317          	auipc	t1,0xffffe
    80602a34:	bd030067          	jr	-1072(t1) # 80600600 <pmm_alloc_page>

0000000080602a38 <kfree>:
    80602a38:	c509                	beqz	a0,80602a42 <kfree+0xa>
    80602a3a:	ffffe317          	auipc	t1,0xffffe
    80602a3e:	c5a30067          	jr	-934(t1) # 80600694 <pmm_free_page>
    80602a42:	8082                	ret

0000000080602a44 <consputc>:
    80602a44:	0ff57513          	zext.b	a0,a0
    80602a48:	ffffe317          	auipc	t1,0xffffe
    80602a4c:	5d030067          	jr	1488(t1) # 80601018 <uart_putc>

0000000080602a50 <kvmmap>:
    80602a50:	8082                	ret

0000000080602a52 <mappages>:
    80602a52:	4501                	li	a0,0
    80602a54:	8082                	ret

0000000080602a56 <userret>:
    80602a56:	a001                	j	80602a56 <userret>

0000000080602a58 <prepare_return>:
    80602a58:	8082                	ret

0000000080602a5a <uvmcreate>:
    80602a5a:	ffffe317          	auipc	t1,0xffffe
    80602a5e:	ba630067          	jr	-1114(t1) # 80600600 <pmm_alloc_page>

0000000080602a62 <uvmfree>:
    80602a62:	c509                	beqz	a0,80602a6c <uvmfree+0xa>
    80602a64:	ffffe317          	auipc	t1,0xffffe
    80602a68:	c3030067          	jr	-976(t1) # 80600694 <pmm_free_page>
    80602a6c:	8082                	ret

0000000080602a6e <uvmcopy>:
    80602a6e:	4501                	li	a0,0
    80602a70:	8082                	ret

0000000080602a72 <uvmunmap>:
    80602a72:	8082                	ret

0000000080602a74 <uvmalloc>:
    80602a74:	8532                	mv	a0,a2
    80602a76:	8082                	ret

0000000080602a78 <uvmdealloc>:
    80602a78:	8532                	mv	a0,a2
    80602a7a:	8082                	ret

0000000080602a7c <copyin>:
    80602a7c:	1141                	addi	sp,sp,-16
    80602a7e:	852e                	mv	a0,a1
    80602a80:	85b2                	mv	a1,a2
    80602a82:	0006861b          	sext.w	a2,a3
    80602a86:	e406                	sd	ra,8(sp)
    80602a88:	00000097          	auipc	ra,0x0
    80602a8c:	eb8080e7          	jalr	-328(ra) # 80602940 <memmove>
    80602a90:	60a2                	ld	ra,8(sp)
    80602a92:	4501                	li	a0,0
    80602a94:	0141                	addi	sp,sp,16
    80602a96:	8082                	ret

0000000080602a98 <copyout>:
    80602a98:	1141                	addi	sp,sp,-16
    80602a9a:	852e                	mv	a0,a1
    80602a9c:	85b2                	mv	a1,a2
    80602a9e:	0006861b          	sext.w	a2,a3
    80602aa2:	e406                	sd	ra,8(sp)
    80602aa4:	00000097          	auipc	ra,0x0
    80602aa8:	e9c080e7          	jalr	-356(ra) # 80602940 <memmove>
    80602aac:	60a2                	ld	ra,8(sp)
    80602aae:	4501                	li	a0,0
    80602ab0:	0141                	addi	sp,sp,16
    80602ab2:	8082                	ret

0000000080602ab4 <begin_op>:
    80602ab4:	8082                	ret

0000000080602ab6 <end_op>:
    80602ab6:	8082                	ret

0000000080602ab8 <fsinit>:
    80602ab8:	8082                	ret

0000000080602aba <namei>:
    80602aba:	4501                	li	a0,0
    80602abc:	8082                	ret

0000000080602abe <iput>:
    80602abe:	8082                	ret

0000000080602ac0 <idup>:
    80602ac0:	8082                	ret

0000000080602ac2 <fileclose>:
    80602ac2:	8082                	ret

0000000080602ac4 <filedup>:
    80602ac4:	8082                	ret

0000000080602ac6 <kexec>:
    80602ac6:	557d                	li	a0,-1
    80602ac8:	8082                	ret
