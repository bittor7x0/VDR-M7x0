#ifndef __MORECRAP_H
#define __MORECRAP_H

/* USB compatibility */

#include <linux/usb.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
typedef struct iso_packet_descriptor usb_iso_packet_descriptor;
#define URB_ISO_ASAP	USB_ISO_ASAP
#define URB_MEM_FLAG
#else
#define URB_MEM_FLAG	, GFP_ATOMIC
#endif

#endif

