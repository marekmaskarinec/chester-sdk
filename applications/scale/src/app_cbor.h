#ifndef APP_CBOR_H_
#define APP_CBOR_H_

/* Zephyr includes */
#include <zephyr.h>
#include <zcbor_encode.h>

#ifdef __cplusplus
extern "C" {
#endif

int app_cbor_encode(zcbor_state_t *zs);

#ifdef __cplusplus
}
#endif

#endif /* APP_CBOR_H_ */
