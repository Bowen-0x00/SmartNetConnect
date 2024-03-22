#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm-generic/gpio.h>
#include <linux/gpio.h>

static irqreturn_t gpio_test_handler(int irq, void *data)
{
	printk("[%s] line=%d\n", __func__, __LINE__);
	return IRQ_HANDLED;
}
static int __init hello_init(void)
{
    printk(KERN_INFO "Hello, World!\n");
    int gpio = ('B'-'A')*32+12;
	int ret, irq;
	unsigned int flags = IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING;

	ret = gpio_request(gpio, "gpio_test");
	printk("gpio_request return %d\n", ret);

	ret = gpio_direction_input(gpio);
	printk("gpio_direction_input return %d\n", ret);

	irq = gpio_to_irq(gpio);
	printk("gpio to irq:%d\n", irq);

	ret = request_irq(irq, gpio_test_handler, flags, dev_name(dev), dev);
	printk("request irq return:%d\n", ret);

	disable_irq(irq);
	printk("disable irq\n");

	enable_irq(irq);
	printk("enable irq\n");
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "Goodbye, World!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple hello world module");