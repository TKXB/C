#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/unistd.h>

#define PROC_V    "/proc/version"
#define BOOT_PATH "/boot/System.map-"
#define MAX_VERSION_LEN   256

void ** syscall_table;

asmlinkage int (*original_call) (const char*, int, int);

asmlinkage int our_sys_open(const char* file, int flags, int mode)
{
    printk("A file was opened, filename is %s\n",file);
    return original_call(file, flags, mode);
}

char *acquire_kernel_version (char *buf){
    struct file *proc_version;
    char *kernel_version;
    mm_segment_t oldfs;

    oldfs = get_fs();
    set_fs (KERNEL_DS);
    proc_version = filp_open(PROC_V, O_RDONLY, 0);
    if (IS_ERR(proc_version) || (proc_version == NULL)) {
        return NULL;
    }

    memset(buf, 0, MAX_VERSION_LEN);
    vfs_read(proc_version, buf, MAX_VERSION_LEN, &(proc_version->f_pos));
    kernel_version = strsep(&buf, " ");
    //printk(KERN_EMERG "kernel version is %s\n",kernel_version);
    kernel_version = strsep(&buf, " ");
    //printk(KERN_EMERG "kernel version is %s\n",kernel_version);
    kernel_version = strsep(&buf, " ");
    printk(KERN_EMERG "kernel version is %s\n",kernel_version);
    filp_close(proc_version, 0);
    set_fs(oldfs);
    return kernel_version;
}

int find_sys_call_table (char *kern_ver){
    char system_map_entry[MAX_VERSION_LEN];
    int i = 0;

/*
 * Holds the /boot/System.map-<version> file name as we build it
 */
    char *filename;

/*
 * Length of the System.map filename, terminating NULL included
 */
    size_t filename_length = strlen(kern_ver) + strlen(BOOT_PATH) + 1;

/*
 * This will point to our /boot/System.map-<version> file
 */
    struct file *f = NULL;

    mm_segment_t oldfs;
    oldfs = get_fs();
    set_fs (KERNEL_DS);
    printk(KERN_EMERG "Kernel version: %s\n", kern_ver);
    filename = kmalloc(filename_length, GFP_KERNEL);
    if (filename == NULL) {
        printk(KERN_EMERG "kmalloc failed on System.map-<version> filename allocation");
        return -1;
    }
    memset(filename, 0, filename_length);
    strncpy(filename, BOOT_PATH, strlen(BOOT_PATH));
    strncat(filename, kern_ver, strlen(kern_ver));
    f = filp_open(filename, O_RDONLY, 0);
    if (IS_ERR(f) || (f == NULL)) {
        printk(KERN_EMERG "Error opening System.map-<version> file: %s\n", filename);
        return -1;
    }
    memset(system_map_entry, 0, MAX_VERSION_LEN);
    while (vfs_read(f, system_map_entry + i, 1, &f->f_pos) == 1) {
        /*
         * If we've read an entire line or maxed out our buffer,
         * check to see if we've just read the sys_call_table entry.
         */
        if ( system_map_entry[i] == '\n' || i == MAX_VERSION_LEN ) {
            // Reset the "column"/"character" counter for the row
            i = 0;

            if (strstr(system_map_entry, "sys_call_table") != NULL) {
                char *sys_string;
                char *system_map_entry_ptr = system_map_entry;

                sys_string = kmalloc(MAX_VERSION_LEN, GFP_KERNEL);
                if (sys_string == NULL) {
                    filp_close(f, 0);
                    set_fs(oldfs);

                    kfree(filename);

                    return -1;
                }

                memset(sys_string, 0, MAX_VERSION_LEN);

                strncpy(sys_string, strsep(&system_map_entry_ptr, " "), MAX_VERSION_LEN);

                kstrtoul(sys_string, 16, &syscall_table);
                printk(KERN_EMERG "syscall_table retrieved\n");

                kfree(sys_string);

                break;
            }

            memset(system_map_entry, 0, MAX_VERSION_LEN);
            continue;
        }

        i++;
    }
    filp_close(f, 0);
    set_fs(oldfs);

    kfree(filename);

    return 0;
}



MODULE_LICENSE("Dual BSD/GPL");
static int hook_init(void)
{
    printk(KERN_EMERG "Hello, HOOK/n");
    char *kernel_version = kmalloc(MAX_VERSION_LEN, GFP_KERNEL);
    find_sys_call_table(acquire_kernel_version(kernel_version));
    if (syscall_table != NULL) {
        write_cr0 (read_cr0 () & (~ 0x10000));
        original_call = syscall_table[__NR_open];
        syscall_table[__NR_open] = our_sys_open;
        write_cr0 (read_cr0 () | 0x10000);
        printk(KERN_EMERG "[+] onload: sys_call_table hooked\n");
    } else {
        printk(KERN_EMERG "[-] onload: syscall_table is NULL\n");
    }

    kfree(kernel_version);
    return 0;
}
static void hook_exit(void)
{
    if (syscall_table != NULL) {
        write_cr0 (read_cr0 () & (~ 0x10000));
        syscall_table[__NR_open] = original_call;
        write_cr0 (read_cr0 () | 0x10000);
        printk(KERN_EMERG "[+] onunload: sys_call_table unhooked\n");
    } else {
        printk(KERN_EMERG "[-] onunload: syscall_table is NULL\n");
    }

    printk(KERN_INFO "Goodbye world!\n");
}
module_init(hook_init);
module_exit(hook_exit);
