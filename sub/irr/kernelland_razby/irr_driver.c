/*
	IRを受信し、IRQ 割り込みとして受け取って、
	/proc/driver/irr_recv を通じて結果を返す.
	
	echo start > /proc/driver/irr_recv
	[IR送信]
	echo end > /proc/driver/irr_recv
	cat /proc/driver/irr_recv
	
	IR送信については、今のところ kernellandより userlandでやったほうがよい結果がでている。
	IR受信については、kernellandでirqした方がいい結果が出ているため、本プログラムコードで行う.

	このコードは BSD/GPL でライセンスされています。
*/
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/pm_runtime.h>
#include <linux/pinctrl/consumer.h>

#include <linux/init.h>
#include <linux/proc_fs.h> // create_proc_entry
#include <asm/uaccess.h> // copy_from_user
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/ioport.h>
#include <asm/io.h>

// /proc/PROCNAME にインタフェースを作る
#define IR_PROCNAME_RECV "driver/irr_recv"
#define IR_PROCNAME_RECV_COUNT "driver/irr_recv_count"

#define DRV_NAME "irr_driver"

#define MAXBUF 64

#define IR_RECV_BUFFER_SIZE 600000
static unsigned int IRRecvBuffer[IR_RECV_BUFFER_SIZE];
static unsigned int IRRecvBufferIndex = 0;

static void* DMTIMERMemory = NULL;

#define IR_RECV_GPIO_PIN 21

//IR受信のGPIO_to_IRQ
static u16 IRRecvGPIO = 0; //GPIOを初期化したかのフラグ
static u16 IRRecvIRQ = 0;  //IRQ割り込みハンドラ

//現在の時刻を取得します。 10uの時間がほしいので、速度優先のため絶対にINLINE展開されるマクロで定義します.
#define dmtimer_get()  ((unsigned int)ioread32(DMTIMERMemory))


// 読み込み
static int ir_proc_recv_show(struct seq_file *m, void *v)
{
	int i;
	for(i = 0; i < IRRecvBufferIndex ; i++)
	{
		seq_putc(m,(IRRecvBuffer[i]) & 0xff  );//D
		seq_putc(m,(IRRecvBuffer[i]>>8) & 0xff  );//C
		seq_putc(m,(IRRecvBuffer[i]>>16) & 0xff  );//B
		seq_putc(m,(IRRecvBuffer[i]>>24) & 0xff  );//A
	}
	return 0;
}


static int
ir_proc_recv_open(struct inode *inode, struct file *file)
{
	return single_open(file, ir_proc_recv_show, NULL);
}



// 読み込み
static int ir_proc_recv_count_show(struct seq_file *m, void *v)
{
	seq_printf(m,"%u",IRRecvBufferIndex);
	return 0;
}

static int
ir_proc_recv_count_open(struct inode *inode, struct file *file)
{
	return single_open(file, ir_proc_recv_count_show, NULL);
}

static const struct file_operations proc_file_fops = {
 .owner = THIS_MODULE,
 .open	= ir_proc_recv_open,
 .read	= seq_read,
 .llseek	= seq_lseek,
 .release	= single_release,
};

static const struct file_operations proc_file_fops2 = {
 .owner = THIS_MODULE,
 .open	= ir_proc_recv_count_open,
 .read	= seq_read,
 .llseek	= seq_lseek,
 .release	= single_release,
};



static int GPIOInit(int pin,unsigned int flag)
{
	char gpiolabel[20];
	int err;

	sprintf(gpiolabel,"%s %d gpio",DRV_NAME ,pin);
	err = gpio_request_one(pin, flag, gpiolabel);
	if (err < 0) 
	{
		printk(KERN_ALERT DRV_NAME " : failed to request GPIO PIN %d. (%s)\n", pin,gpiolabel);
		return err;
	}

	return 1;
}






static irqreturn_t
test_irq_latency_interrupt_handler(int irq, void* dev_id)
{
	unsigned int a = dmtimer_get();
	if (IRRecvBufferIndex < IR_RECV_BUFFER_SIZE)
	{
		IRRecvBuffer[IRRecvBufferIndex++] = a;
	}
	return IRQ_HANDLED;
}


MODULE_LICENSE("Dual BSD/GPL");

//全部開放.
static void cleanup(void)
{
    // インターフェース削除.
	remove_proc_entry( IR_PROCNAME_RECV_COUNT, NULL );
	remove_proc_entry( IR_PROCNAME_RECV, NULL );

	if (IRRecvIRQ)
	{
		free_irq(IRRecvIRQ, NULL);
	}
	IRRecvIRQ = 0;

	if (IRRecvGPIO)
	{
		gpio_free(IR_RECV_GPIO_PIN);
	}
	IRRecvGPIO = 0;

	if (DMTIMERMemory)
	{
		iounmap(DMTIMERMemory);
	}
	DMTIMERMemory = NULL;

}

static int irr_driver_init(void)
{
	int err = 0;
	struct proc_dir_entry* entry;

	printk(KERN_ALERT "driver loaded\n");

	entry = proc_create( IR_PROCNAME_RECV, S_IRUGO | S_IWUGO | S_IXUGO, NULL,&proc_file_fops );
	if( !entry ){
		printk( KERN_ERR "create_proc_entry %s failed\n" ,IR_PROCNAME_RECV);
		cleanup();
		return -EBUSY;
	}

	entry = proc_create( IR_PROCNAME_RECV_COUNT, S_IRUGO | S_IWUGO | S_IXUGO, NULL,&proc_file_fops2 );
	if( !entry ){
		printk( KERN_ERR "create_proc_entry %s failed\n" ,IR_PROCNAME_RECV_COUNT);
		cleanup();
		return -EBUSY;
	}

	//DMTIMERメモリ
//	DMTIMERMemory = ioremap(0x2000B000,4); //rasby1
	DMTIMERMemory = ioremap(0x3F003004,4); //rasby2-3

	err = GPIOInit(IR_RECV_GPIO_PIN,GPIOF_IN);
	if (err < 0) {
		printk(KERN_ALERT DRV_NAME " : failed to apply IR_RECV_GPIO_PIN settings.\n");
		cleanup();
		return err;
	}
	IRRecvGPIO = 1;

	err = gpio_to_irq(IR_RECV_GPIO_PIN);
	if (err < 0) {
		printk(KERN_ALERT DRV_NAME " : failed to get IRQ for pin %d.\n",IR_RECV_GPIO_PIN);
		cleanup();
		return err;
	} else {
		IRRecvIRQ = (u16)err;
		err = 0;
	}

	err = request_any_context_irq(
		IRRecvIRQ,
		test_irq_latency_interrupt_handler,
		IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING | IRQF_NOBALANCING | IRQF_IRQPOLL ,
		DRV_NAME,
		NULL
	);
	if (err < 0) {
		printk(KERN_ALERT DRV_NAME " : failed to enable IRQ %d for pin %d.\n",IRRecvIRQ, IR_RECV_GPIO_PIN);
		cleanup();
		return err;
	}

	printk(KERN_ALERT "irr_driver Load.\n");

	return 0;
}

static void irr_driver_exit(void)
{
	cleanup();

	printk(KERN_ALERT "driver unloaded\n");
}

module_init(irr_driver_init);
module_exit(irr_driver_exit);
