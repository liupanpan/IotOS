#define GPIO_DIR_IN    1
#define GPIO_DIR_OUT   0

#define GPIO_SIZE   0x1000
#define NUM_GPIO_BANKS 8

extern int omap_gpio_set_direction(int gpio, int num, int dir);
extern int omap_gpio_get_direction(int gpio, int num);
extern int omap_gpio_set_output_state(int gpio, int num, int value);
extern int omap_gpio_get_output_state(int gpio, int num);
extern int omap_gpio_get_input_state(int gpio, int num);
extern int omap_gpio_set_highleveldetect(int gpio, int num, int enabled);
extern int omap_gpio_get_highleveldetect(int gpio, int num);
extern int omap_gpio_set_lowleveldetect(int gpio, int num, int enabled);
extern int omap_gpio_get_lowleveldetect(int gpio, int num);
extern int omap_gpio_set_risingdetect(int gpio, int num, int enabled);
extern int omap_gpio_get_risingdetect(int gpio, int num);
extern int omap_gpio_set_fallingdetect(int gpio, int num, int enabled);
extern int omap_gpio_get_fallingdetect(int gpio, int num);
extern int omap_gpio_set_debouncetime(int gpio_bank, int debounceTime);
extern int omap_gpio_get_debouncetime(int gpio_bank, int num);
extern int omap_gpio_set_debounce_state(int gpio, int num, int enabled);
extern int omap_gpio_get_debounce_state(int gpio, int num);
extern int omap_gpio_get_irq_status(int gpio, int num);
extern int omap_gpio_clear_irq(int gpio, int num);
extern int omap_gpio_set_irqenable_state(int gpio, int num, int enabled);
extern int omap_gpio_get_irqenable_state(int gpio, int num);
extern int omap_gpio_init(void);
extern void omap_gpio_deinit(void);

