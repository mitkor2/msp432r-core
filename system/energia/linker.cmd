/*
 * This file was generated by linkcmd_gnu.xdt from the ti.platforms.emt432 package.
 */

__STACK_SIZE = 0x400;
__TI_STACK_SIZE = __STACK_SIZE;

INPUT(
    configPkg/package/cfg/energia_pm4fg.om4fg
    /* ti/runtime/wiring/msp432/lib/wiring_msp432.m4fg.lib commented out by mkbrd.ksh */
    /* lib/board.m4fg.lib commented out by mkbrd.ksh */
    ti/drivers/lib/drivers_msp432p4xx.am4fg
    ti/dpl/lib/dpl_msp432p4xx.am4fg
    src/sysbios/sysbios.am4fg
    third_party/fatfs/lib/fatfs.am4fg
    gnu/targets/arm/rtsv7M/lib/gnu.targets.arm.rtsv7M.am4fg
    gnu/targets/arm/rtsv7M/lib/boot.am4fg
    gnu/targets/arm/rtsv7M/lib/syscalls.am4fg
)

/*
 * symbolic aliases for static instance objects
 */
xdc_runtime_Startup__EXECFXN__C = 1;
xdc_runtime_Startup__RESETFXN__C = 1;

INCLUDE "ti/platforms/emt432//include_gnu/MSP432P401R.lds"

SECTIONS {
        .bootVecs (DSECT) : {*(.bootVecs)} 


    .data : {
         *(.data*)
    } > REGION_DATA AT> REGION_TEXT
    __data_end__ = __data_start__ + SIZEOF(.data);

    /*
     * Linker command file contributions from all loaded packages:
     */
    
/* Content from ti.sysbios.family.arm (ti/sysbios/family/arm/linkcmd.xdt): */

/* Content from ti.sysbios.rts (ti/sysbios/rts/linkcmd.xdt): */

/* Content from ti.sysbios.family.arm.m3 (ti/sysbios/family/arm/m3/linkcmd.xdt): */
    _intvecs_base_address = 0;
    _vtable_base_address = 536870912;
ti_sysbios_family_arm_m3_Hwi_nvic = 0xe000e000;


    __TI_STACK_BASE = __stack;
}

SECTIONS {
    /* create an empty sections at the end of SRAM and FLASH */
    .empty : { *(.empty) KEEP(*(xdc.meta)) } > SRAM
    .emptyFlash : { *(.emptyFlash) } > FLASH

    /* the UNUSED symbols define reusable heap memory */
    __UNUSED_SRAM_start__ = ADDR(.empty);
    __UNUSED_SRAM_end__   = ORIGIN(SRAM) + LENGTH(SRAM);
    __SRAM_LENGTH__       = LENGTH(SRAM);
    
    __UNUSED_FLASH_start__ = ADDR(.emptyFlash);
    __UNUSED_FLASH_end__   = ORIGIN(FLASH) + LENGTH(FLASH);
    __FLASH_LENGTH__       = LENGTH(FLASH);
}

ENTRY(_c_int00)

/* function aliases */
xdc_runtime_System_asprintf_va__E = xdc_runtime_System_asprintf_va__F;
xdc_runtime_System_snprintf_va__E = xdc_runtime_System_snprintf_va__F;
xdc_runtime_System_printf_va__E = xdc_runtime_System_printf_va__F;
xdc_runtime_System_aprintf_va__E = xdc_runtime_System_aprintf_va__F;
xdc_runtime_System_sprintf_va__E = xdc_runtime_System_sprintf_va__F;

