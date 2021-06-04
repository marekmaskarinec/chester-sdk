#ifndef HIO_NET_LTE_H
#define HIO_NET_LTE_H

#include <hio_sys.h>

// Standard includes
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    bool auto_connect;
    int plmn_id;
    // TODO Add eDRX period
} hio_net_lte_cfg_t;

#define HIO_NET_LTE_CFG_DEFAULTS { \
    .auto_connect = true,          \
    .plmn_id = 23003               \
}

typedef enum {
    HIO_NET_LTE_EVENT_ATTACHED = 0,
    HIO_NET_LTE_EVENT_DETACHED = 1,
    HIO_NET_LTE_EVENT_RECEIVED = 2
} hio_net_lte_event_t;

typedef void (*hio_net_lte_callback_t)(hio_net_lte_event_t event,
                                       void *param);

int
hio_net_lte_init(const hio_net_lte_cfg_t *cfg);

int
hio_net_lte_set_callback(hio_net_lte_callback_t *cb);

int
hio_net_lte_attach(void);

int
hio_net_lte_detach(void);

int
hio_net_lte_send(int port, const void *buf, size_t len,
                 hio_sys_timeout_t ttl);

int
hio_net_lte_recv(int *port, void *buf, size_t size, size_t *len);

#endif