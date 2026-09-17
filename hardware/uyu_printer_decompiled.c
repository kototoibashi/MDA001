/*
 * uyu_printer.c 全関数 r2ghidra 擬似コード抽出(自動生成、未検証)
 *
 * 対象: backup_images/boot.img 由来のカーネル(Linux 4.9.193, ARM64, MSM8953)
 * 抽出元: backup_images/derived/kernel/kernel.elf (vmlinux-to-elfでシンボル復元済み)
 * ツール: radare2 6.2.2 (ソースビルド) + r2ghidra (Ghidra decompilerプラグイン)
 * コマンド: r2 -q -e bin.relocs.apply=true -c 'af @ <addr>; afu <end> @ <addr>; pdg @ <addr>' kernel.elf
 *
 * 注意:
 * - これはGhidra decompilerによる自動生成コードであり、人間による解釈・検証はまだ行っていない。
 * - 変数名の一部(_obj.XXX 等)は kallsyms から復元された実シンボル名。
 * - 関数境界判定の誤りにより、一部の関数で隣接関数のコードが混入している可能性がある。
 * - 詳細な解読・考察は docs/hardware/kernel_analysis.md を参照。
 */

/* ==================== slm758_dev_probe @ 0xffffff8008e14df0 ==================== */


int sym.slm758_dev_probe(int64_t arg1)

{
    int64_t iVar1;
    int iVar2;
    int64_t iVar3;
    uint64_t uVar4;
    int *unaff_x20;
    ulong uVar5;
    
    sym._mcount();
    iVar1 = arg1 + 0x10;
    if (*(arg1 + 0x298) != 0) {
        unaff_x20 = sym.devm_kmalloc(iVar1,0x28,0x24080c0);
        if (unaff_x20 == NULL) {
            sym.dev_err(iVar1,0xffffff80095fd781);
            return -0xc;
        }
        iVar3 = sym.devm_pinctrl_get(iVar1);
        *(unaff_x20 + 4) = iVar3;
        if (iVar3 != 0) {
            uVar4 = sym.pinctrl_lookup_state(iVar3,0xffffff8009643fde);
            if (uVar4 < 0xfffffffffffff001) {
                iVar2 = sym.pinctrl_select_state(*(unaff_x20 + 4),uVar4);
                if (iVar2 == 0) goto code_r0xffffff8008e14eac;
                uVar5 = 0xffffff8009644019;
            }
            else {
                uVar5 = 0xffffff8009643fee;
            }
            sym.printk(uVar5);
            sym.dev_err(iVar1,0xffffff800964401b);
        }
code_r0xffffff8008e14eac:
        uVar5 = *(arg1 + 0x298);
        iVar2 = sym.of_get_named_gpio_flags(uVar5,0xffffff8009644043,0,0);
        *unaff_x20 = iVar2;
        sym.gpio_to_desc();
        iVar3 = sym.gpiod_to_chip();
        if (iVar2 < *(iVar3 + 0x80)) {
code_r0xffffff8008e14f4c:
            sym.dev_err(iVar1,0xffffff8009643ec6);
        }
        else {
            iVar2 = sym.of_get_named_gpio_flags(uVar5,0xffffff8009644056,0,0);
            unaff_x20[1] = iVar2;
            sym.gpio_to_desc();
            iVar3 = sym.gpiod_to_chip();
            if (iVar2 < *(iVar3 + 0x80)) goto code_r0xffffff8008e14f4c;
            iVar2 = sym.of_get_named_gpio_flags(uVar5,0xffffff8009644067,0,0);
            unaff_x20[2] = iVar2;
            sym.gpio_to_desc();
            iVar3 = sym.gpiod_to_chip();
            if (iVar2 < *(iVar3 + 0x80)) goto code_r0xffffff8008e14f4c;
        }
        sym.platform_device_add_data(arg1,unaff_x20,0x28);
    }
    iVar2 = *unaff_x20;
    sym.gpio_to_desc(iVar2);
    iVar3 = sym.gpiod_to_chip();
    if (iVar2 < *(iVar3 + 0x80)) {
code_r0xffffff8008e14f88:
        iVar2 = unaff_x20[1];
        sym.gpio_to_desc(iVar2);
        iVar3 = sym.gpiod_to_chip();
        if (*(iVar3 + 0x80) <= iVar2) {
            iVar2 = sym.gpio_request(unaff_x20[1],0xffffff800964405d);
            if (iVar2 != 0) {
                uVar5 = 0xffffff8009643f1c;
                goto code_r0xffffff8008e15068;
            }
            uVar5 = sym.gpio_to_desc(unaff_x20[1]);
            iVar2 = sym.gpiod_direction_output_raw(uVar5,0);
            if (iVar2 != 0) {
                uVar5 = 0xffffff8009643f35;
                goto code_r0xffffff8008e150cc;
            }
        }
        iVar2 = unaff_x20[2];
        sym.gpio_to_desc(iVar2);
        iVar3 = sym.gpiod_to_chip();
        if (*(iVar3 + 0x80) <= iVar2) {
            iVar2 = sym.gpio_request(unaff_x20[2],0xffffff800964406e);
            if (iVar2 != 0) {
                uVar5 = 0xffffff8009643f59;
code_r0xffffff8008e15068:
                sym.dev_err(iVar1,uVar5);
                return iVar2;
            }
            uVar5 = sym.gpio_to_desc(unaff_x20[2]);
            iVar2 = sym.gpiod_direction_output_raw(uVar5,0);
            if (iVar2 != 0) {
                uVar5 = 0xffffff8009643f72;
                goto code_r0xffffff8008e150cc;
            }
        }
        iVar2 = sym.sysfs_create_group(arg1 + 0x20,obj.slm758_attribute_group);
        if (-1 < iVar2) {
            return 0;
        }
        sym.printk(0xffffff8009643f96);
    }
    else {
        iVar2 = sym.gpio_request(*unaff_x20,0xffffff800964404a);
        if (iVar2 != 0) {
            uVar5 = 0xffffff8009643edb;
            goto code_r0xffffff8008e15068;
        }
        uVar5 = sym.gpio_to_desc(*unaff_x20);
        iVar2 = sym.gpiod_direction_output_raw(uVar5,0);
        if (iVar2 == 0) goto code_r0xffffff8008e14f88;
        uVar5 = 0xffffff8009643ef6;
code_r0xffffff8008e150cc:
        sym.dev_err(iVar1,uVar5);
    }
    sym.devm_pinctrl_put(*(unaff_x20 + 4));
    sym.devm_kfree(iVar1,unaff_x20);
    sym.printk(0xffffff8009643fbb,0xffffff8009643fcd);
    return iVar2;
}


/* ==================== show_fingerprint @ 0xffffff8008e15118 ==================== */


int64_t sym.show_fingerprint(int64_t arg1,uchar noname_1 [16],int64_t arg2)

{
    int iVar1;
    uint64_t uVar2;
    uint *puVar3;
    
    sym._mcount();
    puVar3 = *(arg1 + 0xb0);
    sym.gpio_to_desc(*puVar3);
    sym.gpiod_direction_input();
    sym.gpio_to_desc(*puVar3);
    uVar2 = sym.gpiod_get_raw_value();
    iVar1 = sym.sprintf(arg2,0xffffff8009556c60,uVar2 & 0xffffffff);
    return iVar1;
}


/* ==================== store_fingerprint @ 0xffffff8008e15170 ==================== */


int64_t sym.store_fingerprint(int64_t arg1,uchar noname_1 [16],int64_t arg2,int64_t arg3)

{
    int iVar1;
    uint uVar2;
    ulong uVar3;
    int64_t iVar4;
    uint *puVar5;
    uint *puVar6;
    uint uStack_3c;
    int64_t iStack_38;
    
    iStack_38 = _obj.__stack_chk_guard;
    sym._mcount();
    puVar6 = *(arg1 + 0xb0);
    puVar5 = &uStack_3c;
    iVar1 = sym.sscanf(arg2,0xffffff800953e840,puVar5);
    if (iVar1 == 1) {
        sym.printk(0xffffff80096440a3,arg2);
        uVar3 = sym.gpio_to_desc(*puVar6);
        sym.gpiod_direction_output_raw(uVar3,uStack_3c);
    }
    else {
        sym.printk(0xffffff8009644078,0xffffff8009644091);
        arg3 = -0x16;
    }
    if (_obj.__stack_chk_guard == iStack_38) {
        return arg3;
    }
    iVar4 = sym.__stack_chk_fail();
    sym._mcount();
    iVar4 = *(iVar4 + 0xb0);
    sym.gpio_to_desc(*(iVar4 + 4));
    sym.gpiod_direction_input();
    sym.gpio_to_desc(*(iVar4 + 4));
    uVar2 = sym.gpiod_get_raw_value();
    iVar1 = sym.sprintf(puVar5,0xffffff8009556c60,uVar2);
    return iVar1;
}


/* ==================== show_cam_light @ 0xffffff8008e15230 ==================== */


int64_t sym.show_cam_light(int64_t arg1,uchar noname_1 [16],int64_t arg2)

{
    int iVar1;
    uint64_t uVar2;
    int64_t iVar3;
    
    sym._mcount();
    iVar3 = *(arg1 + 0xb0);
    sym.gpio_to_desc(*(iVar3 + 4));
    sym.gpiod_direction_input();
    sym.gpio_to_desc(*(iVar3 + 4));
    uVar2 = sym.gpiod_get_raw_value();
    iVar1 = sym.sprintf(arg2,0xffffff8009556c60,uVar2 & 0xffffffff);
    return iVar1;
}


/* ==================== store_cam_light @ 0xffffff8008e15288 ==================== */


int64_t sym.store_cam_light(int64_t arg1,uchar noname_1 [16],int64_t arg2,int64_t arg3)

{
    int iVar1;
    uint uVar2;
    ulong uVar3;
    uint *puVar4;
    int64_t iVar5;
    uint uStack_3c;
    int64_t iStack_38;
    
    iStack_38 = _obj.__stack_chk_guard;
    sym._mcount();
    iVar5 = *(arg1 + 0xb0);
    puVar4 = &uStack_3c;
    iVar1 = sym.sscanf(arg2,0xffffff800953e840,puVar4);
    if (iVar1 == 1) {
        sym.printk(0xffffff80096440ce,arg2);
        uVar3 = sym.gpio_to_desc(*(iVar5 + 4));
        sym.gpiod_direction_output_raw(uVar3,uStack_3c);
    }
    else {
        sym.printk(0xffffff8009644078,0xffffff80096440be);
        arg3 = -0x16;
    }
    if (_obj.__stack_chk_guard == iStack_38) {
        return arg3;
    }
    iVar5 = sym.__stack_chk_fail();
    sym._mcount();
    iVar5 = *(iVar5 + 0xb0);
    sym.gpio_to_desc(*(iVar5 + 8));
    sym.gpiod_direction_input();
    sym.gpio_to_desc(*(iVar5 + 8));
    uVar2 = sym.gpiod_get_raw_value();
    iVar1 = sym.sprintf(puVar4,0xffffff8009556c60,uVar2);
    return iVar1;
}


/* ==================== show_alc_board @ 0xffffff8008e15348 ==================== */


int64_t sym.show_alc_board(int64_t arg1,uchar noname_1 [16],int64_t arg2)

{
    int iVar1;
    uint64_t uVar2;
    int64_t iVar3;
    
    sym._mcount();
    iVar3 = *(arg1 + 0xb0);
    sym.gpio_to_desc(*(iVar3 + 8));
    sym.gpiod_direction_input();
    sym.gpio_to_desc(*(iVar3 + 8));
    uVar2 = sym.gpiod_get_raw_value();
    iVar1 = sym.sprintf(arg2,0xffffff8009556c60,uVar2 & 0xffffffff);
    return iVar1;
}


/* ==================== store_alc_board @ 0xffffff8008e153a0 ==================== */


int64_t sym.store_alc_board(int64_t arg1,uchar noname_1 [16],int64_t arg2,int64_t arg3)

{
    int iVar1;
    uint uVar2;
    ulong uVar3;
    int64_t iVar4;
    uint uStack_3c;
    int64_t iStack_38;
    
    iStack_38 = _obj.__stack_chk_guard;
    sym._mcount();
    iVar4 = *(arg1 + 0xb0);
    iVar1 = sym.sscanf(arg2,0xffffff800953e840,&uStack_3c);
    if (iVar1 == 1) {
        sym.printk(0xffffff80096440f7,arg2);
        uVar3 = sym.gpio_to_desc(*(iVar4 + 8));
        sym.gpiod_direction_output_raw(uVar3,uStack_3c);
    }
    else {
        sym.printk(0xffffff8009644078,0xffffff80096440e7);
        arg3 = -0x16;
    }
    if (_obj.__stack_chk_guard != iStack_38) {
        sym.__stack_chk_fail();
        sym._mcount();
        uVar2 = sym.gpio_request(_obj.Heat_En,0xffffff8009644110);
        sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x79,uVar2);
        iVar1 = sym.gpio_request(_obj.Print_En,0xffffff800964412f);
        sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x7b,iVar1);
        if (iVar1 == 0) {
            iVar1 = sym.gpio_request(_obj.PRN_MT_A,0xffffff8009644138);
            sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x7e,iVar1);
            if (iVar1 == 0) {
                iVar1 = sym.gpio_request(_obj.PRN_MT_B,0xffffff8009644141);
                sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x81,iVar1);
                if (iVar1 == 0) {
                    iVar1 = sym.gpio_request(_obj.PRN_MT_A_0,0xffffff800964414a);
                    sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x84,iVar1);
                    if (iVar1 == 0) {
                        iVar1 = sym.gpio_request(_obj.PRN_MT_B_0,0xffffff8009644155);
                        sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x8a,iVar1);
                        if (iVar1 == 0) {
                            iVar1 = sym.gpio_request(_obj.PRN_STB,0xffffff8009644160);
                            sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x8d,iVar1);
                            if (iVar1 == 0) {
                                iVar1 = sym.gpio_request(_obj.P_Paper,0xffffff8009644168);
                                sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x90,iVar1);
                                if (iVar1 == 0) {
                                    uVar3 = sym.gpio_to_desc(_obj.PRN_MT_A);
                                    sym.gpiod_direction_output_raw(uVar3,0);
                                    uVar3 = sym.gpio_to_desc(_obj.PRN_MT_B);
                                    sym.gpiod_direction_output_raw(uVar3,0);
                                    uVar3 = sym.gpio_to_desc(_obj.PRN_MT_A_0);
                                    sym.gpiod_direction_output_raw(uVar3,0);
                                    uVar3 = sym.gpio_to_desc(_obj.PRN_MT_B_0);
                                    sym.gpiod_direction_output_raw(uVar3,0);
                                    uVar3 = sym.gpio_to_desc(_obj.Print_En);
                                    sym.gpiod_direction_output_raw(uVar3,0);
                                    uVar3 = sym.gpio_to_desc(_obj.Heat_En);
                                    sym.gpiod_direction_output_raw(uVar3,0);
                                    uVar3 = sym.gpio_to_desc(_obj.PRN_STB);
                                    sym.gpiod_direction_output_raw(uVar3,0);
                                    sym.gpio_to_desc(_obj.P_Paper);
                                    sym.gpiod_direction_input();
                                    return 0;
                                }
                            }
                        }
                    }
                }
            }
        }
        return 0xffffffed;
    }
    return arg3;
}


/* ==================== gpio_init @ 0xffffff8008e15460 ==================== */


ulong sym.gpio_init(void)

{
    uint uVar1;
    int iVar2;
    ulong uVar3;
    
    sym._mcount();
    uVar1 = sym.gpio_request(_obj.Heat_En,0xffffff8009644110);
    sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x79,uVar1);
    iVar2 = sym.gpio_request(_obj.Print_En,0xffffff800964412f);
    sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x7b,iVar2);
    if (iVar2 == 0) {
        iVar2 = sym.gpio_request(_obj.PRN_MT_A,0xffffff8009644138);
        sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x7e,iVar2);
        if (iVar2 == 0) {
            iVar2 = sym.gpio_request(_obj.PRN_MT_B,0xffffff8009644141);
            sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x81,iVar2);
            if (iVar2 == 0) {
                iVar2 = sym.gpio_request(_obj.PRN_MT_A_0,0xffffff800964414a);
                sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x84,iVar2);
                if (iVar2 == 0) {
                    iVar2 = sym.gpio_request(_obj.PRN_MT_B_0,0xffffff8009644155);
                    sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x8a,iVar2);
                    if (iVar2 == 0) {
                        iVar2 = sym.gpio_request(_obj.PRN_STB,0xffffff8009644160);
                        sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x8d,iVar2);
                        if (iVar2 == 0) {
                            iVar2 = sym.gpio_request(_obj.P_Paper,0xffffff8009644168);
                            sym.printk(0xffffff8009644118,0xffffff80095b7e89,0x90,iVar2);
                            if (iVar2 == 0) {
                                uVar3 = sym.gpio_to_desc(_obj.PRN_MT_A);
                                sym.gpiod_direction_output_raw(uVar3,0);
                                uVar3 = sym.gpio_to_desc(_obj.PRN_MT_B);
                                sym.gpiod_direction_output_raw(uVar3,0);
                                uVar3 = sym.gpio_to_desc(_obj.PRN_MT_A_0);
                                sym.gpiod_direction_output_raw(uVar3,0);
                                uVar3 = sym.gpio_to_desc(_obj.PRN_MT_B_0);
                                sym.gpiod_direction_output_raw(uVar3,0);
                                uVar3 = sym.gpio_to_desc(_obj.Print_En);
                                sym.gpiod_direction_output_raw(uVar3,0);
                                uVar3 = sym.gpio_to_desc(_obj.Heat_En);
                                sym.gpiod_direction_output_raw(uVar3,0);
                                uVar3 = sym.gpio_to_desc(_obj.PRN_STB);
                                sym.gpiod_direction_output_raw(uVar3,0);
                                sym.gpio_to_desc(_obj.P_Paper);
                                sym.gpiod_direction_input();
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0xffffffed;
}


/* ==================== Print_On @ 0xffffff8008e156e0 ==================== */


void sym.Print_On(void)

{
    ulong uVar1;
    
    sym._mcount();
    uVar1 = sym.gpio_to_desc(_obj.Print_En);
    sym.gpiod_direction_output_raw(uVar1,1);
    return;
}


/* ==================== Print_Off @ 0xffffff8008e15708 ==================== */


void sym.Print_Off(void)

{
    ulong uVar1;
    
    sym._mcount();
    uVar1 = sym.gpio_to_desc(_obj.Print_En);
    sym.gpiod_direction_output_raw(uVar1,0);
    return;
}


/* ==================== Heat_On @ 0xffffff8008e15730 ==================== */


void sym.Heat_On(void)

{
    ulong uVar1;
    
    sym._mcount();
    uVar1 = sym.gpio_to_desc(_obj.Heat_En);
    sym.gpiod_direction_output_raw(uVar1,1);
    _obj.task_busy = 1;
    sym.printk(0xffffff8009644170);
    return;
}


/* ==================== Heat_Off @ 0xffffff8008e15778 ==================== */


void sym.Heat_Off(void)

{
    ulong uVar1;
    
    sym._mcount();
    uVar1 = sym.gpio_to_desc(_obj.Heat_En);
    sym.gpiod_direction_output_raw(uVar1,0);
    _obj.task_busy = 0;
    sym.printk(0xffffff8009644187);
    return;
}


/* ==================== motor_step1 @ 0xffffff8008e157b8 ==================== */


void sym.motor_step1(int64_t arg1)

{
    ulong uVar1;
    
    sym._mcount();
    uVar1 = sym.gpio_to_desc(_obj.PRN_MT_A);
    sym.gpiod_direction_output_raw(uVar1,1);
    uVar1 = sym.gpio_to_desc(_obj.PRN_MT_A_0);
    sym.gpiod_direction_output_raw(uVar1,0);
    uVar1 = sym.gpio_to_desc(_obj.PRN_MT_B);
    sym.gpiod_direction_output_raw(uVar1,1);
    uVar1 = sym.gpio_to_desc(_obj.PRN_MT_B_0);
    sym.gpiod_direction_output_raw(uVar1,arg1 != 0);
    sym.__udelay(_obj.MOTOR_DELAY);
    return;
}


/* ==================== motor_step2 @ 0xffffff8008e15840 ==================== */


void sym.motor_step2(int64_t arg1)

{
    uint uVar1;
    ulong uVar2;
    
    sym._mcount();
    if (arg1 == 0) {
        uVar1 = _obj.PRN_MT_A_0;
    }
    else {
        uVar1 = _obj.PRN_MT_B;
    }
    uVar2 = sym.gpio_to_desc(uVar1);
    sym.gpiod_direction_output_raw(uVar2,0);
    sym.__udelay(_obj.MOTOR_DELAY);
    return;
}


/* ==================== motor_step3 @ 0xffffff8008e15890 ==================== */


void sym.motor_step3(int64_t arg1)

{
    bool bVar1;
    ulong uVar2;
    
    sym._mcount();
    bVar1 = arg1 == 0;
    if (bVar1) {
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_A);
    }
    else {
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_A_0);
    }
    sym.gpiod_direction_output_raw(uVar2,!bVar1);
    sym.__udelay(_obj.MOTOR_DELAY);
    return;
}


/* ==================== motor_step4 @ 0xffffff8008e158e8 ==================== */


void sym.motor_step4(int64_t arg1)

{
    bool bVar1;
    ulong uVar2;
    
    sym._mcount();
    bVar1 = arg1 == 0;
    if (bVar1) {
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_B_0);
    }
    else {
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_A);
    }
    sym.gpiod_direction_output_raw(uVar2,bVar1);
    sym.__udelay(_obj.MOTOR_DELAY);
    return;
}


/* ==================== motor_step5 @ 0xffffff8008e15940 ==================== */


void sym.motor_step5(int64_t arg1)

{
    ulong uVar1;
    
    sym._mcount();
    uVar1 = sym.gpio_to_desc(_obj.PRN_MT_B);
    sym.gpiod_direction_output_raw(uVar1,arg1 != 0);
    sym.__udelay(_obj.MOTOR_DELAY);
    return;
}


/* ==================== motor_step6 @ 0xffffff8008e15990 ==================== */


void sym.motor_step6(int64_t arg1)

{
    bool bVar1;
    ulong uVar2;
    
    sym._mcount();
    bVar1 = arg1 == 0;
    if (bVar1) {
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_A);
    }
    else {
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_B_0);
    }
    sym.gpiod_direction_output_raw(uVar2,bVar1);
    sym.__udelay(_obj.MOTOR_DELAY);
    return;
}


/* ==================== motor_step7 @ 0xffffff8008e159e8 ==================== */


void sym.motor_step7(int64_t arg1)

{
    bool bVar1;
    ulong uVar2;
    
    sym._mcount();
    bVar1 = arg1 == 0;
    if (bVar1) {
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_A_0);
    }
    else {
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_A);
    }
    sym.gpiod_direction_output_raw(uVar2,!bVar1);
    sym.__udelay(_obj.MOTOR_DELAY);
    return;
}


/* ==================== motor_step8 @ 0xffffff8008e15a40 ==================== */


void sym.motor_step8(int64_t arg1)

{
    bool bVar1;
    ulong uVar2;
    
    sym._mcount();
    bVar1 = arg1 == 0;
    if (bVar1) {
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_B);
    }
    else {
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_A_0);
    }
    sym.gpiod_direction_output_raw(uVar2,bVar1);
    sym.__udelay(_obj.MOTOR_DELAY);
    return;
}


/* ==================== motor_forward_one_step @ 0xffffff8008e15a98 ==================== */


void sym.motor_forward_one_step(void)

{
    uint32_t uVar1;
    uint uVar2;
    ulong uVar3;
    
    sym._mcount();
    if (_obj.Motor_Cycle == 0) {
        uVar3 = sym.gpio_to_desc(_obj.PRN_MT_B);
        sym.gpiod_direction_output_raw(uVar3,1);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar3 = sym.gpio_to_desc(_obj.PRN_MT_B_0);
        sym.gpiod_direction_output_raw(uVar3,0);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar3 = sym.gpio_to_desc(_obj.PRN_MT_A);
        sym.gpiod_direction_output_raw(uVar3,1);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar2 = _obj.PRN_MT_A_0;
    }
    else {
        sym.motor_step1(1);
        uVar3 = sym.gpio_to_desc(_obj.PRN_MT_B);
        sym.gpiod_direction_output_raw(uVar3,0);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar3 = sym.gpio_to_desc(_obj.PRN_MT_A_0);
        sym.gpiod_direction_output_raw(uVar3,1);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar2 = _obj.PRN_MT_A;
    }
    uVar3 = sym.gpio_to_desc(uVar2);
    sym.gpiod_direction_output_raw(uVar3,0);
    sym.__udelay(_obj.MOTOR_DELAY);
    uVar1 = _obj.Motor_Cycle + 2;
    if (-1 < _obj.Motor_Cycle + 1) {
        uVar1 = _obj.Motor_Cycle + 1;
    }
    _obj.Motor_Cycle = (_obj.Motor_Cycle + 1) - (uVar1 & 0xfffffffe);
    return;
}


/* ==================== motor_backward_one_step @ 0xffffff8008e15ba8 ==================== */


void sym.motor_backward_one_step(void)

{
    uint32_t uVar1;
    ulong uVar2;
    
    sym._mcount();
    if (_obj.Motor_Cycle == 0) {
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_B);
        sym.gpiod_direction_output_raw(uVar2,0);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_A);
        sym.gpiod_direction_output_raw(uVar2,1);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_A_0);
        sym.gpiod_direction_output_raw(uVar2,0);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_B);
        sym.gpiod_direction_output_raw(uVar2,1);
    }
    else {
        sym.motor_step1(0);
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_A_0);
        sym.gpiod_direction_output_raw(uVar2,0);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_A);
        sym.gpiod_direction_output_raw(uVar2,0);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_B_0);
        sym.gpiod_direction_output_raw(uVar2,1);
    }
    sym.__udelay(_obj.MOTOR_DELAY);
    uVar1 = _obj.Motor_Cycle + 2;
    if (-1 < _obj.Motor_Cycle + 1) {
        uVar1 = _obj.Motor_Cycle + 1;
    }
    _obj.Motor_Cycle = (_obj.Motor_Cycle + 1) - (uVar1 & 0xfffffffe);
    return;
}


/* ==================== motor_move_lines @ 0xffffff8008e15cc8 ==================== */


void sym.motor_move_lines(int64_t arg1)

{
    int iVar1;
    int iVar2;
    
    iVar1 = arg1;
    sym._mcount();
    if (iVar1 < 1) {
        if (iVar1 < 0) {
            iVar2 = 0;
            do {
                sym.motor_backward_one_step();
                iVar2 = iVar2 + -1;
            } while (iVar1 < iVar2);
        }
    }
    else {
        iVar2 = 0;
        do {
            sym.motor_forward_one_step();
            iVar2 = iVar2 + 1;
        } while (iVar2 != iVar1);
    }
    return;
}


/* ==================== PaperCheck @ 0xffffff8008e15d20 ==================== */


uchar sym.PaperCheck(void)

{
    uchar uVar1;
    
    sym._mcount();
    sym.gpio_to_desc(_obj.Print_En);
    uVar1 = sym.gpiod_get_raw_value();
    sym.printk(0xffffff800964419f,uVar1);
    sym.gpio_to_desc(_obj.P_Paper);
    uVar1 = sym.gpiod_get_raw_value();
    sym.printk(0xffffff80096441af,uVar1);
    return uVar1;
}


/* ==================== printer_dowork @ 0xffffff8008e15d88 ==================== */


ulong sym.printer_dowork(int64_t arg1)

{
    sym._mcount();
    sym.queue_work_on(8,*(arg1 + 0x110),arg1 + 0x118);
    return 0;
}


/* ==================== LineColorRankData_Manage @ 0xffffff8008e15dc0 ==================== */


void sym.LineColorRankData_Manage(int64_t arg1,int64_t arg2)

{
    uint32_t uVar1;
    uint16_t uVar2;
    uint32_t uVar3;
    uint64_t uVar4;
    
    sym._mcount();
    uVar1 = arg2 & 0xff;
    uVar3 = 0;
    uVar2 = 0;
    do {
        if ((uVar3 < uVar1) || (uVar1 + 0xf0 <= uVar3)) {
            obj.LineColorRankData[uVar2] = 0x0;
        }
        else {
            uVar4 = uVar2;
            uVar2 = uVar2 + 1;
            obj.LineColorRankData[uVar4] = *arg1 >> 4;
            if (0x17f < uVar2) {
                return;
            }
            obj.LineColorRankData[uVar2] = *arg1 & 0xf;
            arg1 = arg1 + 1;
        }
        uVar2 = uVar2 + 1;
        uVar3 = uVar2;
    } while (uVar3 < 0x180);
    return;
}


/* ==================== IdlePrint @ 0xffffff8008e15e58 ==================== */


//WARNING: Variable defined which should be unmapped: var_28h

void sym.IdlePrint(void)

{
    code *pcVar1;
    code *pcVar2;
    code *pcVar3;
    code *pcVar4;
    code *pcVar5;
    code *pcVar6;
    code *pcVar7;
    uint uVar8;
    code cVar9;
    code cVar10;
    char cVar11;
    uint64_t uVar12;
    uint8_t *extraout_x1;
    uint16_t uVar13;
    int64_t iVar14;
    uint8_t *puVar15;
    uint32_t uVar16;
    code *pcVar17;
    ulong uVar18;
    code cVar19;
    ulong ****ppppuStack_248;
    ulong ****ppppuStack_240;
    ulong ****ppppuStack_238;
    ulong ****ppppuStack_230;
    ulong ****ppppuStack_1e0;
    ulong ****ppppuStack_1d8;
    uint8_t auStack_1d0 [48];
    int64_t iStack_1a0;
    ulong ****ppppuStack_e0;
    ulong ****ppppuStack_d8;
    ulong ****ppppuStack_d0;
    ulong ****ppppuStack_c8;
    ulong ****ppppuStack_78;
    ulong ****ppppuStack_70;
    ulong uStack_68;
    ulong uStack_60;
    int64_t var_58h;
    ulong uStack_50;
    int64_t var_48h;
    ulong uStack_40;
    int64_t var_38h;
    int64_t var_28h;
    
    var_38h = _obj.__stack_chk_guard;
    sym._mcount();
    var_48h = 0;
    uStack_40 = 0;
    var_58h = 0;
    uStack_50 = 0;
    uStack_68 = 0;
    uStack_60 = 0;
    uVar18 = *(_obj.gPrn + 0x68);
    ppppuStack_e0 = NULL;
    ppppuStack_d8 = NULL;
    sym.__memset(&ppppuStack_d0,0,0x68);
    ppppuStack_78 = &ppppuStack_78;
    ppppuStack_d0 = &ppppuStack_d0;
    ppppuStack_c8 = &ppppuStack_d0;
    ppppuStack_70 = ppppuStack_78;
    uVar12 = sym.__list_add_valid(&ppppuStack_e0,&ppppuStack_d0,&ppppuStack_d0);
    if ((uVar12 & 1) != 0) {
        ppppuStack_e0 = &ppppuStack_d0;
        ppppuStack_d8 = &ppppuStack_d0;
        ppppuStack_d0 = &ppppuStack_e0;
        ppppuStack_c8 = &ppppuStack_e0;
    }
    sym.spi_sync(uVar18,&ppppuStack_d0);
    uVar18 = sym.gpio_to_desc(_obj.PRN_STB);
    sym.gpiod_direction_output_raw(uVar18,1);
    sym.msleep(1000);
    uVar18 = sym.gpio_to_desc(_obj.PRN_STB);
    sym.gpiod_direction_output_raw(uVar18,0);
    if (_obj.__stack_chk_guard == var_38h) {
        return;
    }
    cVar10 = sym.__stack_chk_fail();
    iStack_1a0 = _obj.__stack_chk_guard;
    sym._mcount();
    uVar16 = 0;
    uVar13 = 0;
    puVar15 = extraout_x1;
    do {
        if (uVar16 - 0x48 < 0xf0) {
            uVar12 = uVar13;
            uVar13 = uVar13 + 1;
            obj.LineColorRankData[uVar12] = *puVar15 >> 4;
            if (0x17f < uVar13) break;
            obj.LineColorRankData[uVar13] = *puVar15 & 0xf;
            puVar15 = puVar15 + 1;
        }
        else {
            obj.LineColorRankData[uVar13] = 0x0;
        }
        uVar13 = uVar13 + 1;
        uVar16 = uVar13;
    } while (uVar16 < 0x180);
    if (1 < cVar10) {
        cVar19 = 0x1;
        do {
            iVar14 = 0;
            puVar15 = auStack_1d0;
            pcVar17 = obj.LineColorRankData;
            do {
                cVar9 = *pcVar17;
                pcVar1 = pcVar17 + 1;
                iVar14 = iVar14 + 1;
                pcVar2 = pcVar17 + 2;
                pcVar3 = pcVar17 + 3;
                pcVar4 = pcVar17 + 4;
                pcVar5 = pcVar17 + 5;
                pcVar6 = pcVar17 + 6;
                pcVar7 = pcVar17 + 7;
                pcVar17 = pcVar17 + 8;
                *puVar15 = cVar19 <= *pcVar7 |
                           (cVar19 <= *pcVar6 |
                           (cVar19 <= *pcVar5 |
                           (cVar19 <= *pcVar4 |
                           (cVar19 <= *pcVar3 |
                           (cVar19 <= *pcVar2 | (cVar19 <= *pcVar1 | (cVar19 <= cVar9) << 1) << 1) << 1) << 1) << 1) <<
                           1) << 1;
                puVar15 = puVar15 + 1;
            } while (iVar14 != 0x30);
            ppppuStack_248 = NULL;
            ppppuStack_240 = NULL;
            uVar18 = *(_obj.gPrn + 0x68);
            sym.__memset(&ppppuStack_238,0,0x68);
            ppppuStack_238 = &ppppuStack_238;
            ppppuStack_230 = &ppppuStack_238;
            ppppuStack_1e0 = &ppppuStack_1e0;
            ppppuStack_1d8 = &ppppuStack_1e0;
            uVar12 = sym.__list_add_valid(&ppppuStack_248,&ppppuStack_238,&ppppuStack_238);
            if ((uVar12 & 1) != 0) {
                ppppuStack_248 = &ppppuStack_238;
                ppppuStack_240 = &ppppuStack_238;
                ppppuStack_238 = &ppppuStack_248;
                ppppuStack_230 = &ppppuStack_248;
            }
            sym.spi_sync(uVar18,&ppppuStack_238);
            uVar12 = obj.TK;
            cVar9 = obj.TB;
            if (cVar19 != 0x1) {
                cVar9 = 0x0;
            }
            uVar18 = sym.gpio_to_desc(_obj.PRN_STB);
            sym.gpiod_direction_output_raw(uVar18,1);
            sym.__udelay(uVar12 + cVar9);
            uVar18 = sym.gpio_to_desc(_obj.PRN_STB);
            sym.gpiod_direction_output_raw(uVar18,0);
            cVar19 = cVar19 + '\x01';
        } while (cVar19 != cVar10);
    }
    sym.motor_forward_one_step();
    if (_obj.__stack_chk_guard == iStack_1a0) {
        return;
    }
    cVar11 = sym.__stack_chk_fail();
    sym._mcount();
    if (cVar11 == '\0') {
        uVar18 = sym.gpio_to_desc(_obj.PRN_MT_B);
        sym.gpiod_direction_output_raw(uVar18,1);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar18 = sym.gpio_to_desc(_obj.PRN_MT_B_0);
        sym.gpiod_direction_output_raw(uVar18,0);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar18 = sym.gpio_to_desc(_obj.PRN_MT_A);
        sym.gpiod_direction_output_raw(uVar18,1);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar8 = _obj.PRN_MT_A_0;
    }
    else {
        sym.motor_step1(1);
        uVar18 = sym.gpio_to_desc(_obj.PRN_MT_B);
        sym.gpiod_direction_output_raw(uVar18,0);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar18 = sym.gpio_to_desc(_obj.PRN_MT_A_0);
        sym.gpiod_direction_output_raw(uVar18,1);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar8 = _obj.PRN_MT_A;
    }
    uVar18 = sym.gpio_to_desc(uVar8);
    sym.gpiod_direction_output_raw(uVar18,0);
    sym.__udelay(_obj.MOTOR_DELAY);
    return;
}


/* ==================== LineColorRank_Print @ 0xffffff8008e15f70 ==================== */


void sym.LineColorRank_Print(int64_t arg1,int64_t arg2)

{
    code *pcVar1;
    code *pcVar2;
    code *pcVar3;
    code *pcVar4;
    code *pcVar5;
    code *pcVar6;
    code *pcVar7;
    uint uVar8;
    code cVar9;
    char cVar10;
    uint16_t uVar11;
    int64_t iVar12;
    uint8_t *puVar13;
    uint32_t uVar14;
    code *pcVar15;
    uint64_t uVar16;
    ulong uVar17;
    code cVar18;
    ulong ****ppppuStack_118;
    ulong ****ppppuStack_110;
    ulong ****ppppuStack_108;
    ulong ****ppppuStack_100;
    ulong ****ppppuStack_b0;
    ulong ****ppppuStack_a8;
    uint8_t auStack_a0 [48];
    int64_t iStack_70;
    
    iStack_70 = _obj.__stack_chk_guard;
    sym._mcount();
    uVar14 = 0;
    uVar11 = 0;
    do {
        if (uVar14 - 0x48 < 0xf0) {
            uVar16 = uVar11;
            uVar11 = uVar11 + 1;
            obj.LineColorRankData[uVar16] = *arg2 >> 4;
            if (0x17f < uVar11) break;
            obj.LineColorRankData[uVar11] = *arg2 & 0xf;
            arg2 = arg2 + 1;
        }
        else {
            obj.LineColorRankData[uVar11] = 0x0;
        }
        uVar11 = uVar11 + 1;
        uVar14 = uVar11;
    } while (uVar14 < 0x180);
    if (1 < (arg1 & 0xff)) {
        cVar18 = 0x1;
        do {
            iVar12 = 0;
            puVar13 = auStack_a0;
            pcVar15 = obj.LineColorRankData;
            do {
                cVar9 = *pcVar15;
                pcVar1 = pcVar15 + 1;
                iVar12 = iVar12 + 1;
                pcVar2 = pcVar15 + 2;
                pcVar3 = pcVar15 + 3;
                pcVar4 = pcVar15 + 4;
                pcVar5 = pcVar15 + 5;
                pcVar6 = pcVar15 + 6;
                pcVar7 = pcVar15 + 7;
                pcVar15 = pcVar15 + 8;
                *puVar13 = cVar18 <= *pcVar7 |
                           (cVar18 <= *pcVar6 |
                           (cVar18 <= *pcVar5 |
                           (cVar18 <= *pcVar4 |
                           (cVar18 <= *pcVar3 |
                           (cVar18 <= *pcVar2 | (cVar18 <= *pcVar1 | (cVar18 <= cVar9) << 1) << 1) << 1) << 1) << 1) <<
                           1) << 1;
                puVar13 = puVar13 + 1;
            } while (iVar12 != 0x30);
            ppppuStack_118 = NULL;
            ppppuStack_110 = NULL;
            uVar17 = *(_obj.gPrn + 0x68);
            sym.__memset(&ppppuStack_108,0,0x68);
            ppppuStack_108 = &ppppuStack_108;
            ppppuStack_100 = &ppppuStack_108;
            ppppuStack_b0 = &ppppuStack_b0;
            ppppuStack_a8 = &ppppuStack_b0;
            uVar16 = sym.__list_add_valid(&ppppuStack_118,&ppppuStack_108,&ppppuStack_108);
            if ((uVar16 & 1) != 0) {
                ppppuStack_118 = &ppppuStack_108;
                ppppuStack_110 = &ppppuStack_108;
                ppppuStack_108 = &ppppuStack_118;
                ppppuStack_100 = &ppppuStack_118;
            }
            sym.spi_sync(uVar17,&ppppuStack_108);
            uVar16 = obj.TK;
            cVar9 = obj.TB;
            if (cVar18 != 0x1) {
                cVar9 = 0x0;
            }
            uVar17 = sym.gpio_to_desc(_obj.PRN_STB);
            sym.gpiod_direction_output_raw(uVar17,1);
            sym.__udelay(uVar16 + cVar9);
            uVar17 = sym.gpio_to_desc(_obj.PRN_STB);
            sym.gpiod_direction_output_raw(uVar17,0);
            cVar18 = cVar18 + '\x01';
        } while (cVar18 != SUB81(arg1,0));
    }
    sym.motor_forward_one_step();
    if (_obj.__stack_chk_guard == iStack_70) {
        return;
    }
    cVar10 = sym.__stack_chk_fail();
    sym._mcount();
    if (cVar10 == '\0') {
        uVar17 = sym.gpio_to_desc(_obj.PRN_MT_B);
        sym.gpiod_direction_output_raw(uVar17,1);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar17 = sym.gpio_to_desc(_obj.PRN_MT_B_0);
        sym.gpiod_direction_output_raw(uVar17,0);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar17 = sym.gpio_to_desc(_obj.PRN_MT_A);
        sym.gpiod_direction_output_raw(uVar17,1);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar8 = _obj.PRN_MT_A_0;
    }
    else {
        sym.motor_step1(1);
        uVar17 = sym.gpio_to_desc(_obj.PRN_MT_B);
        sym.gpiod_direction_output_raw(uVar17,0);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar17 = sym.gpio_to_desc(_obj.PRN_MT_A_0);
        sym.gpiod_direction_output_raw(uVar17,1);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar8 = _obj.PRN_MT_A;
    }
    uVar17 = sym.gpio_to_desc(uVar8);
    sym.gpiod_direction_output_raw(uVar17,0);
    sym.__udelay(_obj.MOTOR_DELAY);
    return;
}


/* ==================== pic_motor_forward_one_step @ 0xffffff8008e161f8 ==================== */


void sym.pic_motor_forward_one_step(int64_t arg1)

{
    uint uVar1;
    ulong uVar2;
    
    sym._mcount();
    if ((arg1 & 0xffU) == 0) {
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_B);
        sym.gpiod_direction_output_raw(uVar2,1);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_B_0);
        sym.gpiod_direction_output_raw(uVar2,0);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_A);
        sym.gpiod_direction_output_raw(uVar2,1);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar1 = _obj.PRN_MT_A_0;
    }
    else {
        sym.motor_step1(1);
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_B);
        sym.gpiod_direction_output_raw(uVar2,0);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar2 = sym.gpio_to_desc(_obj.PRN_MT_A_0);
        sym.gpiod_direction_output_raw(uVar2,1);
        sym.__udelay(_obj.MOTOR_DELAY);
        uVar1 = _obj.PRN_MT_A;
    }
    uVar2 = sym.gpio_to_desc(uVar1);
    sym.gpiod_direction_output_raw(uVar2,0);
    sym.__udelay(_obj.MOTOR_DELAY);
    return;
}


/* ==================== Print_Picture @ 0xffffff8008e162e0 ==================== */


void sym.Print_Picture(int64_t arg1)

{
    bool bVar1;
    uint8_t uVar2;
    
    sym._mcount();
    uVar2 = 1;
    do {
        sym.LineColorRank_Print(0x10,arg1);
        sym.motor_forward_one_step();
        arg1 = arg1 + 0x78;
        bVar1 = uVar2 < 0xf0;
        uVar2 = uVar2 + 1;
    } while (bVar1);
    return;
}


/* ==================== printer_work @ 0xffffff8008e16328 ==================== */


uint64_t sym.printer_work(int64_t arg1)

{
    int iVar1;
    uint32_t uVar2;
    uint uVar3;
    ulong uVar4;
    uint64_t uVar5;
    int64_t iVar6;
    int64_t iVar7;
    uchar *puVar8;
    uchar *unaff_x21;
    ulong unaff_x22;
    uint *puVar9;
    uchar auStack_20c [20];
    int64_t iStack_1f8;
    ulong uStack_1f0;
    uchar *puStack_1e8;
    uchar *puStack_1e0;
    int64_t iStack_1d8;
    uchar *puStack_1d0;
    code *pcStack_1c8;
    ulong uStack_1c0;
    uchar auStack_1b0 [72];
    uchar auStack_168 [120];
    uint8_t auStack_f0 [128];
    int64_t iStack_70;
    
    iStack_70 = _obj.__stack_chk_guard;
    sym._mcount();
    uStack_1c0 = 0;
    auStack_f0[0x78] = 0;
    auStack_f0[0x68] = 0;
    auStack_f0[0x69] = 0;
    auStack_f0[0x6a] = 0;
    auStack_f0[0x6b] = 0;
    auStack_f0[0x6c] = 0;
    auStack_f0[0x6d] = 0;
    auStack_f0[0x6e] = 0;
    auStack_f0[0x6f] = 0;
    auStack_f0[0x70] = 0;
    auStack_f0[0x71] = 0;
    auStack_f0[0x72] = 0;
    auStack_f0[0x73] = 0;
    auStack_f0[0x74] = 0;
    auStack_f0[0x75] = 0;
    auStack_f0[0x76] = 0;
    auStack_f0[0x77] = 0;
    auStack_f0[0x58] = 0;
    auStack_f0[0x59] = 0;
    auStack_f0[0x5a] = 0;
    auStack_f0[0x5b] = 0;
    auStack_f0[0x5c] = 0;
    auStack_f0[0x5d] = 0;
    auStack_f0[0x5e] = 0;
    auStack_f0[0x5f] = 0;
    auStack_f0[0x60] = 0;
    auStack_f0[0x61] = 0;
    auStack_f0[0x62] = 0;
    auStack_f0[99] = 0;
    auStack_f0[100] = 0;
    auStack_f0[0x65] = 0;
    auStack_f0[0x66] = 0;
    auStack_f0[0x67] = 0;
    auStack_f0[0x48] = 0;
    auStack_f0[0x49] = 0;
    auStack_f0[0x4a] = 0;
    auStack_f0[0x4b] = 0;
    auStack_f0[0x4c] = 0;
    auStack_f0[0x4d] = 0;
    auStack_f0[0x4e] = 0;
    auStack_f0[0x4f] = 0;
    auStack_f0[0x50] = 0;
    auStack_f0[0x51] = 0;
    auStack_f0[0x52] = 0;
    auStack_f0[0x53] = 0;
    auStack_f0[0x54] = 0;
    auStack_f0[0x55] = 0;
    auStack_f0[0x56] = 0;
    auStack_f0[0x57] = 0;
    auStack_f0[0x38] = 0;
    auStack_f0[0x39] = 0;
    auStack_f0[0x3a] = 0;
    auStack_f0[0x3b] = 0;
    auStack_f0[0x3c] = 0;
    auStack_f0[0x3d] = 0;
    auStack_f0[0x3e] = 0;
    auStack_f0[0x3f] = 0;
    auStack_f0[0x40] = 0;
    auStack_f0[0x41] = 0;
    auStack_f0[0x42] = 0;
    auStack_f0[0x43] = 0;
    auStack_f0[0x44] = 0;
    auStack_f0[0x45] = 0;
    auStack_f0[0x46] = 0;
    auStack_f0[0x47] = 0;
    auStack_f0[0x28] = 0;
    auStack_f0[0x29] = 0;
    auStack_f0[0x2a] = 0;
    auStack_f0[0x2b] = 0;
    auStack_f0[0x2c] = 0;
    auStack_f0[0x2d] = 0;
    auStack_f0[0x2e] = 0;
    auStack_f0[0x2f] = 0;
    auStack_f0[0x30] = 0;
    auStack_f0[0x31] = 0;
    auStack_f0[0x32] = 0;
    auStack_f0[0x33] = 0;
    auStack_f0[0x34] = 0;
    auStack_f0[0x35] = 0;
    auStack_f0[0x36] = 0;
    auStack_f0[0x37] = 0;
    auStack_f0[0x18] = 0;
    auStack_f0[0x19] = 0;
    auStack_f0[0x1a] = 0;
    auStack_f0[0x1b] = 0;
    auStack_f0[0x1c] = 0;
    auStack_f0[0x1d] = 0;
    auStack_f0[0x1e] = 0;
    auStack_f0[0x1f] = 0;
    auStack_f0[0x20] = 0;
    auStack_f0[0x21] = 0;
    auStack_f0[0x22] = 0;
    auStack_f0[0x23] = 0;
    auStack_f0[0x24] = 0;
    auStack_f0[0x25] = 0;
    auStack_f0[0x26] = 0;
    auStack_f0[0x27] = 0;
    auStack_f0[8] = 0;
    auStack_f0[9] = 0;
    auStack_f0[10] = 0;
    auStack_f0[0xb] = 0;
    auStack_f0[0xc] = 0;
    auStack_f0[0xd] = 0;
    auStack_f0[0xe] = 0;
    auStack_f0[0xf] = 0;
    auStack_f0[0x10] = 0;
    auStack_f0[0x11] = 0;
    auStack_f0[0x12] = 0;
    auStack_f0[0x13] = 0;
    auStack_f0[0x14] = 0;
    auStack_f0[0x15] = 0;
    auStack_f0[0x16] = 0;
    auStack_f0[0x17] = 0;
    auStack_f0[0] = 0;
    auStack_f0[1] = 0;
    auStack_f0[2] = 0;
    auStack_f0[3] = 0;
    auStack_f0[4] = 0;
    auStack_f0[5] = 0;
    auStack_f0[6] = 0;
    auStack_f0[7] = 0;
    sym.printk(0xffffff80096441c5);
    sym.mutex_lock(obj.printer_work_lock);
    uVar4 = sym.gpio_to_desc(_obj.Heat_En);
    puVar8 = 0x1;
    sym.gpiod_direction_output_raw(uVar4,1);
    _obj.task_busy = 1;
    sym.printk(0xffffff8009644170);
    arg1 = arg1 + -0xa8;
    iVar1 = sym.__kfifo_out(arg1,&stack0xffffffffffffff10,1);
    if (iVar1 != 0) {
        unaff_x21 = &stack0xfffffffffffffe50;
        puVar8 = &stack0xfffffffffffffe98;
        unaff_x22 = 0xffffff80091ec9d0;
        do {
            if (auStack_f0[0] - 0xf0 < 4) {
    //switch table (3 cases) at 0xffffff80091ec9d0
    //WARNING: Could not recover jumptable at 0xffffff8008e16420. Too many branches
    //WARNING: Treating indirect jump as call
                uVar5 = (*(*((auStack_f0[0] - 0xf0) - 0x7ff6e13630) * 4 + -0x7ff71e9bdc))();
                return uVar5;
            }
            iVar1 = sym.__kfifo_out(arg1,&stack0xffffffffffffff10,1);
        } while (iVar1 != 0);
    }
    uVar4 = sym.gpio_to_desc(_obj.Heat_En);
    sym.gpiod_direction_output_raw(uVar4,0);
    _obj.task_busy = 0;
    sym.printk(0xffffff8009644187);
    sym.mutex_unlock(obj.printer_work_lock);
    uVar5 = sym.printk(0xffffff800964421e);
    if (_obj.__stack_chk_guard == iStack_70) {
        return uVar5;
    }
    iVar6 = sym.__stack_chk_fail();
    pcStack_1c8 = sym.uyu_prn_probe;
    iStack_1f8 = _obj.__stack_chk_guard;
    uStack_1f0 = unaff_x22;
    puStack_1e8 = unaff_x21;
    puStack_1e0 = puVar8;
    iStack_1d8 = arg1;
    puStack_1d0 = &stack0xfffffffffffffff0;
    sym._mcount();
    uVar4 = *(iVar6 + 0x288);
    sym.printk(0xffffff800964425d,0xffffff800964426d,0x3a3);
    _obj.Heat_En = sym.of_get_named_gpio_flags(uVar4,0xffffff800964427b,0,0);
    _obj.Print_En = sym.of_get_named_gpio_flags(uVar4,0xffffff80094f56ba,0,0);
    _obj.PRN_MT_A = sym.of_get_named_gpio_flags(uVar4,0xffffff8009644283,0,0);
    _obj.PRN_MT_B = sym.of_get_named_gpio_flags(uVar4,0xffffff8009644288,0,0);
    _obj.PRN_MT_A_0 = sym.of_get_named_gpio_flags(uVar4,0xffffff800964428d,0,0);
    _obj.PRN_MT_B_0 = sym.of_get_named_gpio_flags(uVar4,0xffffff8009644293,0,0);
    _obj.PRN_STB = sym.of_get_named_gpio_flags(uVar4,0xffffff8009644299,0,0);
    _obj.P_Paper = sym.of_get_named_gpio_flags(uVar4,0xffffff800964429d,0,0);
    uVar2 = sym.gpio_init();
    if (uVar2 == 0) {
        sym.printk(0xffffff800964425d,0xffffff800964426d,0x3b7);
        iVar7 = sym.kmem_cache_alloc_trace(*0xffffff8009fce108,0x24080c0,0x138);
        if (iVar7 == 0) {
            sym.dev_err(iVar6,0xffffff8009644310);
            uVar5 = 0xfffffff4;
        }
        else {
            sym.printk(0xffffff800964425d,0xffffff800964426d,0x3bf);
            _obj.gPrn = iVar7;
            iVar1 = sym.__kfifo_alloc(iVar7 + 0x70,0x100000,1,0x24000c0);
            if (iVar1 == 0) {
                *(iVar7 + 0x68) = iVar6;
                *(iVar7 + 0x70) = 0;
                *(iVar7 + 0xd8) = iVar6;
                *(iVar6 + 0x326) = 1;
                *(iVar6 + 0x325) = 8;
                *(iVar6 + 800) = 5000000;
                uVar3 = sym.spi_setup(iVar6);
                sym.printk(0xffffff800964433b,uVar3);
                *(iVar6 + 0xb8) = iVar7;
                puVar9 = iVar7 + 0x88;
                *puVar9 = 0xff;
                *(iVar7 + 0x90) = 0xffffff8009644255;
                *(iVar7 + 0x98) = obj.prn_dev_fops;
                sym.printk(0xffffff8009644352,0xffffff800964426d,0x3d8);
                uVar2 = sym.misc_register(puVar9);
                uVar5 = uVar2;
                sym.printk(0xffffff8009644118,0xffffff800964426d,0x3da,uVar5);
                if (uVar2 == 0) {
                    sym.printk(0xffffff800964425d,0xffffff800964426d,0x3e1);
                    sym.sprintf(&stack0xfffffffffffffdf4,0xffffff8009644398);
                    iVar6 = sym.__alloc_workqueue_key(0xffffff80094640ae,0xe000a,1,0,0,&stack0xfffffffffffffdf4);
                    *(iVar7 + 0x110) = iVar6;
                    if (iVar6 == 0) {
                        sym.printk(0xffffff80096443aa,0xffffff80096442bb);
                        uVar5 = 0xfffffff0;
                    }
                    else {
                        sym.__init_work(iVar7 + 0x118,0);
                        uVar5 = 0;
                        *(iVar7 + 0x118) = 0x1fffffffc0;
                        *(iVar7 + 0x120) = iVar7 + 0x120;
                        *(iVar7 + 0x128) = iVar7 + 0x120;
                        *(iVar7 + 0x130) = sym.printer_work;
                    }
                    goto code_r0xffffff8008e16968;
                }
                sym.printk(0xffffff8009644377,0xffffff80096442bb);
                sym.misc_deregister(puVar9);
            }
            else {
                uVar5 = 0xfffffff4;
            }
        }
        sym.kfree(iVar7);
    }
    else {
        uVar5 = uVar2;
        sym.printk(0xffffff80096442a5,0xffffff80096442bb);
    }
code_r0xffffff8008e16968:
    if (_obj.__stack_chk_guard == iStack_1f8) {
        return uVar5;
    }
    iVar6 = sym.__stack_chk_fail();
    sym._mcount();
    iVar6 = *(iVar6 + 0xb8);
    sym.misc_deregister(iVar6 + 0x88);
    sym.kfree(iVar6);
    sym.gpio_free(_obj.Print_En);
    sym.gpio_free(_obj.Heat_En);
    sym.gpio_free(_obj.PRN_MT_A);
    sym.gpio_free(_obj.PRN_MT_B);
    sym.gpio_free(_obj.PRN_MT_A_0);
    sym.gpio_free(_obj.PRN_MT_B_0);
    sym.gpio_free(_obj.PRN_STB);
    sym.gpio_free(_obj.P_Paper);
    return 0;
}


/* ==================== uyu_prn_probe @ 0xffffff8008e166a0 ==================== */


int sym.uyu_prn_probe(int64_t arg1)

{
    int iVar1;
    uint uVar2;
    int64_t iVar3;
    int64_t iVar4;
    ulong uVar5;
    uint *puVar6;
    uchar auStack_4c [20];
    int64_t iStack_38;
    
    iStack_38 = _obj.__stack_chk_guard;
    sym._mcount();
    uVar5 = *(arg1 + 0x288);
    sym.printk(0xffffff800964425d,0xffffff800964426d,0x3a3);
    _obj.Heat_En = sym.of_get_named_gpio_flags(uVar5,0xffffff800964427b,0,0);
    _obj.Print_En = sym.of_get_named_gpio_flags(uVar5,0xffffff80094f56ba,0,0);
    _obj.PRN_MT_A = sym.of_get_named_gpio_flags(uVar5,0xffffff8009644283,0,0);
    _obj.PRN_MT_B = sym.of_get_named_gpio_flags(uVar5,0xffffff8009644288,0,0);
    _obj.PRN_MT_A_0 = sym.of_get_named_gpio_flags(uVar5,0xffffff800964428d,0,0);
    _obj.PRN_MT_B_0 = sym.of_get_named_gpio_flags(uVar5,0xffffff8009644293,0,0);
    _obj.PRN_STB = sym.of_get_named_gpio_flags(uVar5,0xffffff8009644299,0,0);
    _obj.P_Paper = sym.of_get_named_gpio_flags(uVar5,0xffffff800964429d,0,0);
    iVar1 = sym.gpio_init();
    if (iVar1 == 0) {
        sym.printk(0xffffff800964425d,0xffffff800964426d,0x3b7);
        iVar4 = sym.kmem_cache_alloc_trace(*0xffffff8009fce108,0x24080c0,0x138);
        if (iVar4 == 0) {
            sym.dev_err(arg1,0xffffff8009644310);
            iVar1 = -0xc;
        }
        else {
            sym.printk(0xffffff800964425d,0xffffff800964426d,0x3bf);
            _obj.gPrn = iVar4;
            iVar1 = sym.__kfifo_alloc(iVar4 + 0x70,0x100000,1,0x24000c0);
            if (iVar1 == 0) {
                *(iVar4 + 0x68) = arg1;
                *(iVar4 + 0x70) = 0;
                *(iVar4 + 0xd8) = arg1;
                *(arg1 + 0x326) = 1;
                *(arg1 + 0x325) = 8;
                *(arg1 + 800) = 5000000;
                uVar2 = sym.spi_setup(arg1);
                sym.printk(0xffffff800964433b,uVar2);
                *(arg1 + 0xb8) = iVar4;
                puVar6 = iVar4 + 0x88;
                *puVar6 = 0xff;
                *(iVar4 + 0x90) = 0xffffff8009644255;
                *(iVar4 + 0x98) = obj.prn_dev_fops;
                sym.printk(0xffffff8009644352,0xffffff800964426d,0x3d8);
                iVar1 = sym.misc_register(puVar6);
                sym.printk(0xffffff8009644118,0xffffff800964426d,0x3da,iVar1);
                if (iVar1 == 0) {
                    sym.printk(0xffffff800964425d,0xffffff800964426d,0x3e1);
                    sym.sprintf(auStack_4c,0xffffff8009644398);
                    iVar3 = sym.__alloc_workqueue_key(0xffffff80094640ae,0xe000a,1,0,0,auStack_4c);
                    *(iVar4 + 0x110) = iVar3;
                    if (iVar3 == 0) {
                        sym.printk(0xffffff80096443aa,0xffffff80096442bb);
                        iVar1 = -0x10;
                    }
                    else {
                        sym.__init_work(iVar4 + 0x118,0);
                        iVar1 = 0;
                        *(iVar4 + 0x118) = 0x1fffffffc0;
                        *(iVar4 + 0x120) = iVar4 + 0x120;
                        *(iVar4 + 0x128) = iVar4 + 0x120;
                        *(iVar4 + 0x130) = sym.printer_work;
                    }
                    goto code_r0xffffff8008e16968;
                }
                sym.printk(0xffffff8009644377,0xffffff80096442bb);
                sym.misc_deregister(puVar6);
            }
            else {
                iVar1 = -0xc;
            }
        }
        sym.kfree(iVar4);
    }
    else {
        sym.printk(0xffffff80096442a5,0xffffff80096442bb);
    }
code_r0xffffff8008e16968:
    if (_obj.__stack_chk_guard == iStack_38) {
        return iVar1;
    }
    iVar4 = sym.__stack_chk_fail();
    sym._mcount();
    iVar4 = *(iVar4 + 0xb8);
    sym.misc_deregister(iVar4 + 0x88);
    sym.kfree(iVar4);
    sym.gpio_free(_obj.Print_En);
    sym.gpio_free(_obj.Heat_En);
    sym.gpio_free(_obj.PRN_MT_A);
    sym.gpio_free(_obj.PRN_MT_B);
    sym.gpio_free(_obj.PRN_MT_A_0);
    sym.gpio_free(_obj.PRN_MT_B_0);
    sym.gpio_free(_obj.PRN_STB);
    sym.gpio_free(_obj.P_Paper);
    return 0;
}


/* ==================== uyu_prn_remove @ 0xffffff8008e16a38 ==================== */


ulong sym.uyu_prn_remove(int64_t arg1)

{
    int64_t iVar1;
    
    sym._mcount();
    iVar1 = *(arg1 + 0xb8);
    sym.misc_deregister(iVar1 + 0x88);
    sym.kfree(iVar1);
    sym.gpio_free(_obj.Print_En);
    sym.gpio_free(_obj.Heat_En);
    sym.gpio_free(_obj.PRN_MT_A);
    sym.gpio_free(_obj.PRN_MT_B);
    sym.gpio_free(_obj.PRN_MT_A_0);
    sym.gpio_free(_obj.PRN_MT_B_0);
    sym.gpio_free(_obj.PRN_STB);
    sym.gpio_free(_obj.P_Paper);
    return 0;
}


/* ==================== prn_dev_read @ 0xffffff8008e16ad0 ==================== */


ulong sym.prn_dev_read(ulong noname_0,uchar noname_1 [16],int64_t arg1)

{
    sym._mcount();
    sym.printk(0xffffff80096443ca,arg1 & 0xffffffff);
    return 0;
}


/* ==================== prn_dev_write @ 0xffffff8008e16b08 ==================== */


int64_t sym.prn_dev_write(int64_t arg1,int64_t arg2,int64_t arg3)

{
    int64_t iVar1;
    uint64_t uVar2;
    uint64_t uVar3;
    uint uVar4;
    bool bVar5;
    uchar uVar6;
    char cVar7;
    char *pcVar8;
    char **ppcVar9;
    int64_t iVar10;
    int64_t iVar11;
    ulong uVar12;
    int64_t extraout_x1;
    uint32_t uVar13;
    int64_t iVar14;
    int64_t iVar15;
    uint8_t *puVar16;
    int *piVar17;
    int iVar18;
    uchar auVar19 [16];
    ulong uStack_d0;
    char *pcStack_c8;
    uint uStack_c0;
    uchar uStack_bc;
    int64_t iStack_b8;
    uint32_t uStack_4c;
    char *pcStack_48;
    uint64_t uStack_40;
    int64_t iStack_38;
    
    iStack_38 = _obj.__stack_chk_guard;
    sym._mcount();
    uStack_4c = 0;
    pcVar8 = sym.__kmalloc(arg3,0x24000c0);
    sym.__check_object_size(pcVar8,arg3,0);
    iVar10 = sp_el0;
    bVar5 = CARRY8(arg2,arg3);
    uVar2 = arg2 + arg3;
    uVar3 = 0;
    if (!bVar5 || uVar2 == 0) {
        uVar3 = *(iVar10 + 0x40);
    }
    if (bVar5) {
        uVar2 = 0xffffffffffffffff;
    }
    ppcVar9 = arg3;
    if (!CARRY8(uVar2,~uVar3) && !CARRY8(uVar2 + ~uVar3,bVar5) || uVar2 == uVar3 + !bVar5) {
        ppcVar9 = sym.__arch_copy_from_user(pcVar8,arg2,arg3);
    }
    if (ppcVar9 == NULL) {
        sym.printk(0xffffff80096443d9,arg3 & 0xffffffff);
        cVar7 = *pcVar8;
        if (cVar7 == 'g') {
            pcStack_48 = pcVar8 + 1;
            cVar7 = 'g';
        }
        else {
            sym.__memcpy(&uStack_4c,pcVar8 + 1,4);
            uStack_40 = uStack_4c;
            pcStack_48 = pcVar8 + 5;
        }
        ppcVar9 = &pcStack_48;
        iVar10 = sym.prn_dev_unlocked_ioctl(arg1,cVar7);
        sym.kfree(pcVar8);
    }
    else {
        sym.__memset(pcVar8 + (arg3 - ppcVar9),0);
        iVar10 = -1;
    }
    if (_obj.__stack_chk_guard == iStack_38) {
        return iVar10;
    }
    auVar19 = sym.__stack_chk_fail();
    iStack_b8 = _obj.__stack_chk_guard;
    sym._mcount();
    pcVar8 = ppcVar9[1];
    uStack_bc = 0;
    uStack_c0 = 0;
    iVar10 = *(auVar19._0_8_ + 0x108);
    puVar16 = *ppcVar9;
    pcStack_c8 = pcVar8;
    sym.printk(0xffffff80096443e9,auVar19._8_8_ & 0xffffffff,pcVar8,_obj.Motor_Cycle);
    uVar13 = 0;
    iVar18 = auVar19._8_4_;
    if (0x87 < iVar18) {
        if (iVar18 == 0x88) {
            sym.printk(0xffffff8009644471,*puVar16);
            _obj.HEAT_DELAY = (*puVar16 - 1 & 0xff) * 100 + 0x44c;
            if (0x13 < *puVar16 - 1) {
                _obj.HEAT_DELAY = 2000;
            }
            goto code_r0xffffff8008e16f74;
        }
        if (iVar18 != 0x99) goto code_r0xffffff8008e17078;
        if (pcVar8 != NULL) {
            uVar6 = 0xf2;
            goto code_r0xffffff8008e16fdc;
        }
code_r0xffffff8008e17024:
        if (_obj.printer_work_lock == 1) {
            sym.queue_work_on(8,*(iVar10 + 0x110),iVar10 + 0x118);
        }
        goto code_r0xffffff8008e17074;
    }
    switch(iVar18) {
    case 0x55:
        if (*puVar16 - 1 < 0x28) {
            _obj.MOTOR_DELAY = *((*puVar16 - 1) * 4 + -0x7ff6e0c308);
        }
        else {
            _obj.MOTOR_DELAY = 1000;
        }
        if (pcVar8 != NULL) {
            uVar6 = 0xf1;
code_r0xffffff8008e16fdc:
            uStack_c0 = CONCAT31(uStack_c0._1_3_,uVar6);
            sym.__memcpy(&uStack_c0 | 1,&pcStack_c8,4);
            if (*(iVar10 + 0x78) < *(iVar10 + 0x70) - *(iVar10 + 0x74)) {
                sym.printk(0xffffff8009644428);
            }
            sym.__kfifo_in(iVar10 + 0x70,&uStack_c0,5);
            goto code_r0xffffff8008e17024;
        }
        break;
    case 0x56:
    case 0x57:
    case 0x58:
    case 0x59:
    case 0x5a:
    case 0x5b:
    case 0x5c:
    case 0x5d:
    case 0x5e:
    case 0x5f:
    case 0x60:
    case 0x61:
    case 0x62:
    case 99:
    case 100:
    case 0x65:
    case 0x68:
    case 0x69:
    case 0x6a:
    case 0x6b:
    case 0x6c:
    case 0x6d:
    case 0x6e:
    case 0x6f:
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
        goto code_r0xffffff8008e17078;
    case 0x66:
        iVar11 = sym.__kmalloc(pcVar8 * 0x30,0x2080020);
        sym.__memcpy(iVar11,puVar16,pcStack_c8 * 0x30);
        if (pcStack_c8 == NULL) {
            iVar18 = 0;
        }
        else {
            iVar18 = 0;
            pcVar8 = NULL;
            iVar1 = iVar10 + 0x70;
            iVar15 = iVar11;
            do {
                iVar14 = 0;
                do {
                    if (*(iVar15 + iVar14) != '\0') {
                        uStack_c0 = CONCAT31(uStack_c0._1_3_,0xf0);
                        if (*(iVar10 + 0x78) < *(iVar10 + 0x70) - *(iVar10 + 0x74)) {
                            sym.printk(0xffffff8009644428);
                        }
                        sym.__kfifo_in(iVar1,&uStack_c0,1);
                        sym.__kfifo_in(iVar1,iVar11 + pcVar8 * 0x30,0x30);
                        iVar18 = iVar18 + 1;
                        goto code_r0xffffff8008e16e60;
                    }
                    iVar14 = iVar14 + 1;
                } while (iVar14 < 0x30);
                uStack_d0 = 1;
                uStack_c0 = CONCAT31(uStack_c0._1_3_,0xf1);
                sym.__memcpy(&uStack_c0 | 1,&uStack_d0,4);
                if (*(iVar10 + 0x78) < *(iVar10 + 0x70) - *(iVar10 + 0x74)) {
                    sym.printk(0xffffff8009644428);
                }
                sym.__kfifo_in(iVar1,&uStack_c0,5);
code_r0xffffff8008e16e60:
                if (_obj.printer_work_lock == 1) {
                    sym.queue_work_on(8,*(iVar10 + 0x110),iVar10 + 0x118);
                }
                pcVar8 = pcVar8 + 1;
                iVar15 = iVar15 + 0x30;
            } while (pcVar8 < pcStack_c8);
        }
        sym.kfree(iVar11);
        sym.printk(0xffffff8009644442,iVar18,_obj.HEAT_DELAY,_obj.MOTOR_DELAY);
        break;
    case 0x67:
        uVar12 = sym.kmem_cache_alloc_trace(*0xffffff8009fce0f8,0x2080020,0x78);
        sym.__memcpy(uVar12,puVar16,0x78);
        uStack_c0 = CONCAT31(uStack_c0._1_3_,0xf3);
        piVar17 = iVar10 + 0x70;
        if (*(iVar10 + 0x78) < *piVar17 - *(iVar10 + 0x74)) {
            sym.printk(0xffffff8009644428);
        }
        sym.__kfifo_in(piVar17,&uStack_c0,1);
        sym.__kfifo_in(piVar17,uVar12,0x78);
        if (_obj.printer_work_lock == 1) {
            sym.queue_work_on(8,*(iVar10 + 0x110),iVar10 + 0x118);
        }
        sym.kfree(uVar12);
        break;
    case 0x77:
code_r0xffffff8008e16f74:
        sym.gpio_to_desc(_obj.Print_En);
        uVar6 = sym.gpiod_get_raw_value();
        sym.printk(0xffffff800964419f,uVar6);
        sym.gpio_to_desc(_obj.P_Paper);
        cVar7 = sym.gpiod_get_raw_value();
        sym.printk(0xffffff80096441af,cVar7);
        uVar13 = cVar7 == '\0';
        goto code_r0xffffff8008e17078;
    default:
        if (iVar18 == 0x44) {
            uVar13 = _obj.task_busy;
        }
        goto code_r0xffffff8008e17078;
    }
code_r0xffffff8008e17074:
    uVar13 = 0;
code_r0xffffff8008e17078:
    if (_obj.__stack_chk_guard == iStack_b8) {
        return uVar13;
    }
    sym.__stack_chk_fail();
    sym._mcount();
    iVar10 = *(extraout_x1 + 0x108);
    sym.printk(0xffffff8009644484);
    uVar4 = _obj.Print_En;
    *(extraout_x1 + 0x108) = iVar10 + -0x88;
    uVar12 = sym.gpio_to_desc(uVar4);
    sym.gpiod_direction_output_raw(uVar12,1);
    iVar10 = 100;
    do {
        iVar10 = iVar10 + -1;
        sym.__const_udelay(0x418958);
    } while (iVar10 != 0);
    return 0;
}


/* ==================== prn_dev_unlocked_ioctl @ 0xffffff8008e16c50 ==================== */


int64_t sym.prn_dev_unlocked_ioctl(int64_t arg1,int64_t arg2,int64_t arg3)

{
    int64_t iVar1;
    uint uVar2;
    uchar uVar3;
    char cVar4;
    int64_t iVar5;
    ulong uVar6;
    int64_t extraout_x1;
    uint32_t uVar7;
    int64_t iVar8;
    uint64_t uVar9;
    int64_t iVar10;
    uint8_t *puVar11;
    int *piVar12;
    int iVar13;
    int64_t iVar14;
    ulong uStack_80;
    uint64_t uStack_78;
    uint uStack_70;
    uchar uStack_6c;
    int64_t iStack_68;
    
    iStack_68 = _obj.__stack_chk_guard;
    sym._mcount();
    uVar9 = *(arg3 + 8);
    uStack_6c = 0;
    uStack_70 = 0;
    iVar14 = *(arg1 + 0x108);
    puVar11 = *arg3;
    uStack_78 = uVar9;
    sym.printk(0xffffff80096443e9,arg2 & 0xffffffff,uVar9,_obj.Motor_Cycle);
    uVar7 = 0;
    iVar13 = arg2;
    if (0x87 < iVar13) {
        if (iVar13 == 0x88) {
            sym.printk(0xffffff8009644471,*puVar11);
            _obj.HEAT_DELAY = (*puVar11 - 1 & 0xff) * 100 + 0x44c;
            if (0x13 < *puVar11 - 1) {
                _obj.HEAT_DELAY = 2000;
            }
            goto code_r0xffffff8008e16f74;
        }
        if (iVar13 != 0x99) goto code_r0xffffff8008e17078;
        if (uVar9 != 0) {
            uVar3 = 0xf2;
            goto code_r0xffffff8008e16fdc;
        }
code_r0xffffff8008e17024:
        if (_obj.printer_work_lock == 1) {
            sym.queue_work_on(8,*(iVar14 + 0x110),iVar14 + 0x118);
        }
        goto code_r0xffffff8008e17074;
    }
    //switch table (68 cases) at 0xffffff80091ec9d4
    switch(iVar13) {
    case 0x55:
        if (*puVar11 - 1 < 0x28) {
            _obj.MOTOR_DELAY = *((*puVar11 - 1) * 4 + -0x7ff6e0c308);
        }
        else {
            _obj.MOTOR_DELAY = 1000;
        }
        if (uVar9 != 0) {
            uVar3 = 0xf1;
code_r0xffffff8008e16fdc:
            uStack_70 = CONCAT31(uStack_70._1_3_,uVar3);
            sym.__memcpy(&uStack_70 | 1,&uStack_78,4);
            if (*(iVar14 + 0x78) < *(iVar14 + 0x70) - *(iVar14 + 0x74)) {
                sym.printk(0xffffff8009644428);
            }
            sym.__kfifo_in(iVar14 + 0x70,&uStack_70,5);
            goto code_r0xffffff8008e17024;
        }
        break;
    case 0x56:
    case 0x57:
    case 0x58:
    case 0x59:
    case 0x5a:
    case 0x5b:
    case 0x5c:
    case 0x5d:
    case 0x5e:
    case 0x5f:
    case 0x60:
    case 0x61:
    case 0x62:
    case 99:
    case 100:
    case 0x65:
    case 0x68:
    case 0x69:
    case 0x6a:
    case 0x6b:
    case 0x6c:
    case 0x6d:
    case 0x6e:
    case 0x6f:
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
        goto code_r0xffffff8008e17078;
    case 0x66:
        iVar5 = sym.__kmalloc(uVar9 * 0x30,0x2080020);
        sym.__memcpy(iVar5,puVar11,uStack_78 * 0x30);
        if (uStack_78 == 0) {
            iVar13 = 0;
        }
        else {
            iVar13 = 0;
            uVar9 = 0;
            iVar1 = iVar14 + 0x70;
            iVar10 = iVar5;
            do {
                iVar8 = 0;
                do {
                    if (*(iVar10 + iVar8) != '\0') {
                        uStack_70 = CONCAT31(uStack_70._1_3_,0xf0);
                        if (*(iVar14 + 0x78) < *(iVar14 + 0x70) - *(iVar14 + 0x74)) {
                            sym.printk(0xffffff8009644428);
                        }
                        sym.__kfifo_in(iVar1,&uStack_70,1);
                        sym.__kfifo_in(iVar1,iVar5 + uVar9 * 0x30,0x30);
                        iVar13 = iVar13 + 1;
                        goto code_r0xffffff8008e16e60;
                    }
                    iVar8 = iVar8 + 1;
                } while (iVar8 < 0x30);
                uStack_80 = 1;
                uStack_70 = CONCAT31(uStack_70._1_3_,0xf1);
                sym.__memcpy(&uStack_70 | 1,&uStack_80,4);
                if (*(iVar14 + 0x78) < *(iVar14 + 0x70) - *(iVar14 + 0x74)) {
                    sym.printk(0xffffff8009644428);
                }
                sym.__kfifo_in(iVar1,&uStack_70,5);
code_r0xffffff8008e16e60:
                if (_obj.printer_work_lock == 1) {
                    sym.queue_work_on(8,*(iVar14 + 0x110),iVar14 + 0x118);
                }
                uVar9 = uVar9 + 1;
                iVar10 = iVar10 + 0x30;
            } while (uVar9 < uStack_78);
        }
        sym.kfree(iVar5);
        sym.printk(0xffffff8009644442,iVar13,_obj.HEAT_DELAY,_obj.MOTOR_DELAY);
        break;
    case 0x67:
        uVar6 = sym.kmem_cache_alloc_trace(*0xffffff8009fce0f8,0x2080020,0x78);
        sym.__memcpy(uVar6,puVar11,0x78);
        uStack_70 = CONCAT31(uStack_70._1_3_,0xf3);
        piVar12 = iVar14 + 0x70;
        if (*(iVar14 + 0x78) < *piVar12 - *(iVar14 + 0x74)) {
            sym.printk(0xffffff8009644428);
        }
        sym.__kfifo_in(piVar12,&uStack_70,1);
        sym.__kfifo_in(piVar12,uVar6,0x78);
        if (_obj.printer_work_lock == 1) {
            sym.queue_work_on(8,*(iVar14 + 0x110),iVar14 + 0x118);
        }
        sym.kfree(uVar6);
        break;
    case 0x77:
code_r0xffffff8008e16f74:
        sym.gpio_to_desc(_obj.Print_En);
        uVar3 = sym.gpiod_get_raw_value();
        sym.printk(0xffffff800964419f,uVar3);
        sym.gpio_to_desc(_obj.P_Paper);
        cVar4 = sym.gpiod_get_raw_value();
        sym.printk(0xffffff80096441af,cVar4);
        uVar7 = cVar4 == '\0';
        goto code_r0xffffff8008e17078;
    default:
        if (iVar13 == 0x44) {
            uVar7 = _obj.task_busy;
        }
        goto code_r0xffffff8008e17078;
    }
code_r0xffffff8008e17074:
    uVar7 = 0;
code_r0xffffff8008e17078:
    if (_obj.__stack_chk_guard == iStack_68) {
        return uVar7;
    }
    sym.__stack_chk_fail();
    sym._mcount();
    iVar14 = *(extraout_x1 + 0x108);
    sym.printk(0xffffff8009644484);
    uVar2 = _obj.Print_En;
    *(extraout_x1 + 0x108) = iVar14 + -0x88;
    uVar6 = sym.gpio_to_desc(uVar2);
    sym.gpiod_direction_output_raw(uVar6,1);
    iVar14 = 100;
    do {
        iVar14 = iVar14 + -1;
        sym.__const_udelay(0x418958);
    } while (iVar14 != 0);
    return 0;
}


/* ==================== prn_dev_open @ 0xffffff8008e170b8 ==================== */


ulong sym.prn_dev_open(ulong noname_0,int64_t arg1)

{
    uint uVar1;
    ulong uVar2;
    int64_t iVar3;
    
    sym._mcount();
    iVar3 = *(arg1 + 0x108);
    sym.printk(0xffffff8009644484);
    uVar1 = _obj.Print_En;
    *(arg1 + 0x108) = iVar3 + -0x88;
    uVar2 = sym.gpio_to_desc(uVar1);
    sym.gpiod_direction_output_raw(uVar2,1);
    iVar3 = 100;
    do {
        iVar3 = iVar3 + -1;
        sym.__const_udelay(0x418958);
    } while (iVar3 != 0);
    return 0;
}


/* ==================== prn_dev_release @ 0xffffff8008e17128 ==================== */


ulong sym.prn_dev_release(void)

{
    ulong uVar1;
    
    sym._mcount();
    while (_obj.printer_work_lock != 1) {
        sym.msleep(10);
    }
    uVar1 = sym.gpio_to_desc(_obj.Print_En);
    sym.gpiod_direction_output_raw(uVar1,0);
    uVar1 = sym.gpio_to_desc(_obj.Heat_En);
    sym.gpiod_direction_output_raw(uVar1,0);
    _obj.task_busy = 0;
    sym.printk(0xffffff8009644187);
    sym.printk(0xffffff8009644499);
    return 0;
}


/* ==================== slm758_dev_init @ 0xffffff8009778480 ==================== */


void sym.slm758_dev_init(void)

{
    sym.__platform_driver_register(obj.slm758_dev_driver,0);
    return;
}


/* ==================== uyu_prn_init @ 0xffffff80097784a0 ==================== */


void sym.uyu_prn_init(void)

{
    sym.printk(0xffffff80096444b0);
    sym.__spi_register_driver(0,obj.uyu_prn_driver);
    return;
}


/* ==================== slm758_dev_exit @ 0xffffff8009794330 ==================== */


void sym.slm758_dev_exit(void)

{
    sym.platform_driver_unregister(obj.slm758_dev_driver);
    return;
}


/* ==================== uyu_prn_exit @ 0xffffff8009794350 ==================== */


void sym.uyu_prn_exit(void)

{
    sym.printk(0xffffff8009644240);
    sym.driver_unregister(0xffffff8009edbb28);
    return;
}


