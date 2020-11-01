#ifndef PDM_H
#define PDM_H

#include <audio/dmic.h>
#include <zephyr.h>
#include <device.h>
#include <nrfx_pdm.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////

#define PDM_STACK_SIZE 256
#define PDM_PRIORITY   5

#define PDM_NRFX_NUMBER_OF_BUFFERS 2
#define PDM_NRFX_BUFFER_SIZE       256

#define DEV_CFG(dev) \
	((const struct pdm_config *const)(dev)->config_info)
#define DEV_DATA(dev) \
	((struct pdm_data *const)(dev)->driver_data)

/* ----- */

typedef void (*pdm_data_handler_t)(
    int16_t *data,
    uint16_t data_size
);

struct pdm_config {
    nrfx_pdm_config_t initial_config;
};

struct pdm_data {
    struct device   *dev;
    enum dmic_state state;

    pdm_data_handler_t data_handler;

    int16_t buffer[PDM_NRFX_NUMBER_OF_BUFFERS][PDM_NRFX_BUFFER_SIZE];
    uint8_t active_buffer;
};

////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif /* PDM_H */