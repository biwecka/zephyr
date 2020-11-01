#include "pdm.h"
#define DT_DRV_COMPAT nordic_nrf_pdm

#define LOG_LEVEL CONFIG_AUDIO_DMIC_LOG_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(pdm);

#define CLK_PIN       DT_PROP(DT_NODELABEL(pdm), clk_pin)
#define DIN_PIN       DT_PROP(DT_NODELABEL(pdm), din_pin)

K_THREAD_STACK_DEFINE(pdm_stack, PDM_STACK_SIZE);

//static struct buffer_released_data m_buff_released;
static int16_t m_next_buffer[PDM_NRFX_BUFFER_SIZE][PDM_NRFX_NUMBER_OF_BUFFERS];
static uint8_t m_active_buffer;

////////////////////////////////////////////////////////////////////////////////
// Data and config
static struct pdm_data pdm_data;

static const struct pdm_config pdm_config = {
    .initial_config = NRFX_PDM_DEFAULT_CONFIG(CLK_PIN, DIN_PIN)
};


////////////////////////////////////////////////////////////////////////////////
// Handlers
static void pdm_event_handler(nrfx_pdm_evt_t const *p_evt)
{
    if (p_evt->error == NRFX_PDM_ERROR_OVERFLOW) {
        LOG_ERR("Overflow error when handling event.\n");
        return;
    }

    /* If a buffer was requested,
     * provide a new one, and alternate the active buffer
     */
    if (p_evt->buffer_requested) {
        printk("Buffer requested\n");

        nrfx_err_t result = nrfx_pdm_buffer_set(
            pdm_data.buffer[pdm_data.active_buffer],
            PDM_NRFX_BUFFER_SIZE
        );

        if (result != NRFX_SUCCESS) {
            LOG_ERR("Failed to set new buffer, error %d.\n", result);
        }

        // Alternate active buffer
        pdm_data.active_buffer = (~pdm_data.active_buffer) & 0x01;
    }

    /* If a buffer has been released,
     * save it and submit it to the workqueue for the event handler
     */
    if (p_evt->buffer_released != NULL) {
        printk("Buffer released\n");

        // for (i = 0; i < PDM_NRFX_BUFFER_SIZE; i++) {
        //     //m_buff_released.buffer[i] = p_evt->buffer_released[i];
        //     printk("%d, ", p_evt->buffer_released[i]);
        // }
        printk("%d  ", p_evt->buffer_released[0]);
        printk("%d  ", p_evt->buffer_released[1]);
        printk("\n");
        // k_work_submit(&m_buff_released.work);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Init, Configure, trigger, read - Function definitions
static int pdm_init(struct device *dev) {
    printk("---------------------------------\n");
    printk("Hallo Welt!\n");

    const struct pdm_config *config = dev->config_info;
    struct pdm_data *data = dev->driver_data;

    nrfx_err_t result = nrfx_pdm_init(
        &config->initial_config,
        pdm_event_handler
    );

    if (result != NRFX_SUCCESS) {
        LOG_ERR("Failed to initialize device: %s\n", dev->name);
        return -EBUSY;
    }

    // k_work_q_start(&m_pdm_work_q, pdm_stack,
    //     K_THREAD_STACK_SIZEOF(pdm_stack),
    //     PDM_PRIORITY);
    // k_work_init(&m_buff_released.work, release_buffer);

    IRQ_CONNECT(
        DT_INST_IRQN(0),
        DT_INST_IRQ(0, priority),
        nrfx_isr,
        nrfx_pdm_irq_handler,
        0
    );

    // Set active buffer
    data->active_buffer = 0x00;

    return 0;
}

static int pdm_configure(struct device *dev, struct dmic_cfg *cfg) {
    return -1;
}

static int pdm_trigger(struct device *dev, enum dmic_trigger cmd) {
    struct pdm_data *data = dev->driver_data;
    nrfx_err_t result;

    switch (cmd)
    {
        ////////////////////////////////////////////////////////////////////////
        // Start
        case DMIC_TRIGGER_START:
            printk("DMIC_TRIGGER_START\n");

            result = nrfx_pdm_buffer_set(
                data->buffer[data->active_buffer],
                PDM_NRFX_BUFFER_SIZE
            );

            // Alternate active buffer
            data->active_buffer = (~data->active_buffer) & 0x01;

            if (result != NRFX_SUCCESS) {
                LOG_ERR("Failed to set new buffer, error %d\n", result);
            }

            result = nrfx_pdm_start();
            if (result == NRFX_ERROR_BUSY) {
                LOG_ERR(
                    "Failed to start PDM sampling, device %s is busy.\n",
                    dev->name
                );
            }

            break;
        ////////////////////////////////////////////////////////////////////////
        // Stop
        case DMIC_TRIGGER_STOP:
            printk("DMIC_TRIGGER_STOP\n");

            result = nrfx_pdm_stop();

            if (result == NRFX_ERROR_BUSY) {
                LOG_ERR(
                    "Failed to stop PDM sampling, device %s is busy.\n",
                    dev->name
                );
                return -EBUSY;
            }

            break;
        ////////////////////////////////////////////////////////////////////////
        // Default
        default:
            break;
    }
    return 0;
}

static int pdm_read(struct device *dev, u8_t stream, void **buffer,
    size_t *size, s32_t timeout) {
////
    return -1;
}
////////////////////////////////////////////////////////////////////////////////
// Driver API struct
static const struct _dmic_ops pdm_driver_api = {
    .configure      = pdm_configure,
    .trigger        = pdm_trigger,
    .read           = pdm_read
};

////////////////////////////////////////////////////////////////////////////////
// Initialize driver
DEVICE_AND_API_INIT(
    pdm,
    DT_INST_LABEL(0),
    pdm_init,
    &pdm_data,
    &pdm_config,
    POST_KERNEL,
    CONFIG_KERNEL_INIT_PRIORITY_DEVICE,
    &pdm_driver_api
);