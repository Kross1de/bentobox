#include <kernel/sched.h>
#include <kernel/pci.h>
#include <kernel/vfs.h>
#include <kernel/elf64.h>
#include <kernel/printf.h>
#include <kernel/version.h>

extern void generic_load_modules(void);
extern void debugcon_entry(void);

void generic_startup(void) {
    vfs_install();
	pci_scan();
    generic_load_modules();
	sched_install();
}

void generic_main(void) {
    printf("\nWelcome to \033[96mbentobox\033[0m!\n%s %d.%d-%s %s %s %s\n\n",
        __kernel_name, __kernel_version_major,__kernel_version_minor,
        __kernel_commit_hash, __kernel_build_date, __kernel_build_time, __kernel_arch);

    //sched_add_task(sched_new_task(debugcon_entry, "bentobox debug shell"), NULL);
    spawn("/bin/sh", 0, NULL, NULL);
    //spawn("/bin/fork", 0, NULL, NULL);
	sched_start_all_cores();
}
