#include <stdint.h>
#include <stdlib.h>

#define STM32_GPIOC_BASE     0x40020800
#define STM32_GPIOA_BASE     0x40020000
#define STM32_RCC_BASE       0x40023800

#define STM32_RCC_AHB1ENR_OFFSET  0x0030
#define STM32_GPIO_MODER_OFFSET   0x0000
#define STM32_GPIO_OTYPER_OFFSET  0x0004
#define STM32_GPIO_IDR_OFFSET     0x0010
#define STM32_GPIO_BSRR_OFFSET    0x0018

#define STM32_RCC_AHB1ENR        (STM32_RCC_BASE + STM32_RCC_AHB1ENR_OFFSET)
#define STM32_GPIOC_MODER        (STM32_GPIOC_BASE + STM32_GPIO_MODER_OFFSET)
#define STM32_GPIOC_OTYPER       (STM32_GPIOC_BASE + STM32_GPIO_OTYPER_OFFSET)
#define STM32_GPIOC_BSRR         (STM32_GPIOC_BASE + STM32_GPIO_BSRR_OFFSET)
#define STM32_GPIOA_IDR          (STM32_GPIOA_BASE + STM32_GPIO_IDR_OFFSET)

#define RCC_AHB1ENR_GPIOCEN      (1 << 2)
#define GPIO_MODER_INPUT         (0)
#define GPIO_MODER_OUTPUT        (1)
#define GPIO_MODER_MASK(n)       (3 << (2 * n))
#define GPIO_MODER_SHIFT(n)      (2 * n)
#define GPIO_BSRR_SET(n)         (1 << n)
#define GPIO_BSRR_RESET(n)       (1 << (n + 16))

int main(void) {
    uint32_t reg;
    uint32_t last_button_state = 0;
    uint32_t led_state = 0;

    /* Enable GPIOC clock */
    reg = *(volatile uint32_t *)STM32_RCC_AHB1ENR;
    reg |= RCC_AHB1ENR_GPIOCEN;
    *(volatile uint32_t *)STM32_RCC_AHB1ENR = reg;

    /* Configure GPIOC pin 13 as output */
    reg = *(volatile uint32_t *)STM32_GPIOC_MODER;
    reg &= ~GPIO_MODER_MASK(13);
    reg |= GPIO_MODER_OUTPUT << GPIO_MODER_SHIFT(13);
    *(volatile uint32_t *)STM32_GPIOC_MODER = reg;

    while (1) {
        uint32_t button_state = *(volatile uint32_t *)STM32_GPIOA_IDR & (1 << 0);
        if (button_state && !last_button_state) {
            led_state = !led_state;
            if (led_state) {
                *(volatile uint32_t *)STM32_GPIOC_BSRR = GPIO_BSRR_RESET(13);
            } else {
                *(volatile uint32_t *)STM32_GPIOC_BSRR = GPIO_BSRR_SET(13);
            }
        }
        last_button_state = button_state;
    }

    return 0;
}
