#include <linux/module.h>

/**
 * module initialization entry point
 * Returns 0 to indicate module init successfully,
 * otherwise, the module will not be loaded.
 */
static int empty_module_init(void) {
    pr_info("Init Empty Module...");
    return 0;
}

/** module clean-up entry point */
static void empty_module_exit(void) {
    pr_info("Exit Empty Module...");
}

/* register module entry point to kernel */
module_init(empty_module_init);
/* register module clean-up entry point to kernel */
module_exit(empty_module_exit);

MODULE_DESCRIPTION("Module Getting Started");
MODULE_VERSION("1.0");
MODULE_AUTHOR("Muheng Lee");
MODULE_LICENSE("GPL");