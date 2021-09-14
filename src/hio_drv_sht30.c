#include <hio_drv_sht30.h>
#include <hio_sys.h>

// Zephyr includes
#include <logging/log.h>
#include <zephyr.h>

// Standard includes
#include <string.h>

LOG_MODULE_REGISTER(hio_drv_sht30, LOG_LEVEL_DBG);

int
hio_drv_sht30_init(hio_drv_sht30_t *ctx, hio_bus_i2c_t *i2c, uint8_t dev_addr)
{
    memset(ctx, 0, sizeof(*ctx));

    ctx->i2c = i2c;
    ctx->dev_addr = dev_addr;

    return 0;
}

static int
reset(hio_drv_sht30_t *ctx)
{
    uint8_t buf[] = {0x30, 0xa2};

    hio_bus_i2c_xfer_t xfer = {
        .dev_addr = ctx->dev_addr,
        .buf = buf,
        .len = sizeof(buf)
    };

    if (hio_bus_i2c_write(ctx->i2c, &xfer) < 0) {
        LOG_ERR("Call `hio_bus_i2c_write` failed [%p]", ctx);
        return -1;
    }

    return 0;
}

static int
measure(hio_drv_sht30_t *ctx)
{
    uint8_t buf[] = {0x24, 0x00};

    hio_bus_i2c_xfer_t xfer = {
        .dev_addr = ctx->dev_addr,
        .buf = buf,
        .len = sizeof(buf)
    };

    if (hio_bus_i2c_write(ctx->i2c, &xfer) < 0) {
        LOG_ERR("Call `hio_bus_i2c_write` failed [%p]", ctx);
        return -1;
    }

    return 0;
}

static int
read(hio_drv_sht30_t *ctx, uint8_t *buf)
{
    hio_bus_i2c_xfer_t xfer = {
        .dev_addr = ctx->dev_addr,
        .buf = buf,
        .len = 6
    };

    if (hio_bus_i2c_read(ctx->i2c, &xfer) < 0) {
        LOG_ERR("Call `hio_bus_i2c_read` failed [%p]", ctx);
        return -1;
    }

    return 0;
}

static uint8_t
calc_crc(uint8_t *data, size_t len)
{
    uint16_t crc = 0xff;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (size_t j = 0; j < 8; j++) {
            if ((crc & 0x80) != 0) {
                crc <<= 1;
                crc ^= 0x131;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

int
hio_drv_sht30_measure(hio_drv_sht30_t *ctx, float *t, float *rh)
{
    if (!ctx->ready) {
        if (reset(ctx) < 0) {
            LOG_ERR("Call `reset` failed [%p]", ctx);
            return -1;
        }

        ctx->ready = true;

        hio_sys_task_sleep(HIO_SYS_MSEC(100));
    }

    if (measure(ctx) < 0) {
        LOG_ERR("Call `measure` failed [%p]", ctx);
        ctx->ready = false;
        return -2;
    }

    hio_sys_task_sleep(HIO_SYS_MSEC(100));

    uint8_t buf[6];

    if (read(ctx, buf) < 0) {
        LOG_ERR("Call `read` failed [%p]", ctx);
        ctx->ready = false;
        return -3;
    }

    if (calc_crc(&buf[0], 2) != buf[2] ||
        calc_crc(&buf[3], 2) != buf[5]) {
        LOG_ERR("CRC mismatch [%p]", ctx);
        return -4;
    }

    uint16_t reg_t = buf[0] << 8 | buf[1];
    *t = -45.f + 175.f * (float)reg_t / 65535.f;

    uint16_t reg_rh = buf[3] << 8 | buf[4];
    *rh = 100.f * (float)reg_rh / 65535.f;

    return 0;
}
