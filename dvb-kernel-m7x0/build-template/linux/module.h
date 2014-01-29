
#ifndef _LINUX_MODULE_H_FAKE
#define _LINUX_MODULE_H_FAKE


struct module {};

#warning implement try_module_get() and module_put() if you use dynamically loadable modules!!
static inline int try_module_get(void *mod) { return 0; }
static inline void  module_put(void *mod) {}

#define THIS_MODULE ((void*)0)

#define MODULE_PARM(x...)
#define MODULE_PARM_DESC(x...)
#define MODULE_AUTHOR(x)
#define MODULE_DESCRIPTION(x)
#define MODULE_LICENSE(x)

#define EXPORT_SYMBOL(x)
#define EXPORT_SYMBOL_GPL(x)

#warning implement module_init()/module_exit() so that your modules get initialized!!
#define module_init(x)
#define module_exit(x)

#endif

