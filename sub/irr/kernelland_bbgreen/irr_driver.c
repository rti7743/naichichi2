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

/*
GPIO PINS MEMO

//portname		PIN	init_address
 {"P8_3",		38,0x818 }
,{"P8_4",		39,0x81c }
,{"P8_5",		34,0x808 }
,{"P8_6",		35,0x80c }
,{"P8_7",		66,0x890 }	//TIMER4
,{"P8_8",		67,0x894 }	//TIMER7
,{"P8_9",		69,0x89c }	//TIMER5
,{"P8_10",		68,0x898 }	//TIMER6
,{"P8_11",		45,0x834 }	//GPIO1_13
,{"P8_12",		44,0x830 }	//GPIO1_12
,{"P8_13",		23,0x824 }	//EHRPWM2B
,{"P8_14",		26,0x828 }	//GPIO0_26
,{"P8_15",		47,0x83c }	//GPIO1_15
,{"P8_16",		46,0x838 }	//GPIO1_14
,{"P8_17",		27,0x82c }	//GPIO0_27
,{"P8_18",		65,0x88c }	//GPIO2_1
,{"P8_19",		22,0x820 }	//EHRPWM2A
,{"P8_20",		63,0x884 }	//GPIO1_31
,{"P8_21",		62,0x880 }	//GPIO1_30
,{"P8_22",		37,0x814 }	//GPIO1_5
,{"P8_23",		36,0x810 }	//GPIO1_4
,{"P8_24",		33,0x804 }	//GPIO1_1
,{"P8_25",		32,0x800 }	//GPIO1_0
,{"P8_26",		61,0x87c }	//GPIO1_29
,{"P8_27",		86,0x8e0 }	//GPIO2_22
,{"P8_28",		88,0x8e8 }	//GPIO2_22
,{"P8_29",		87,0x8e4 }	//GPIO2_23
,{"P8_30",		89,0x8ec }	//GPIO2_25
,{"P8_31",		10,0x8d8 }	//UART5_CTSN
,{"P8_32",		11,0x8dc }	//UART5_RTSN
,{"P8_33",		9 ,0x8d4 }	//UART4_RTSN
,{"P8_34",		81,0x8cc }	//UART3_RTSN
,{"P8_35",		8 ,0x8d0 }	//UART4_CTSN
,{"P8_36",		80,0x8c8 }	//UART3_CTSN
,{"P8_37",		78,0x8c0 }	//UART5_TXD
,{"P8_38",		79,0x8c4 }	//UART5_RXD
,{"P8_39",		76,0x8b8 }	//GPIO2_12
,{"P8_40",		77,0x8bc }	//GPIO2_13
,{"P8_41",		74,0x8b0 }	//GPIO2_10
,{"P8_42",		75,0x8b4 }	//GPIO2_11
,{"P8_43",		72,0x8a8 }	//GPIO2_8
,{"P8_44",		73,0x8ac }	//GPIO2_9
,{"P8_45",		70,0x8a0 }	//GPIO2_6
,{"P8_46",		71,0x8a4 }	//GPIO2_7
,{"P9_11",		30,0x870 }	//P9_11
,{"P9_12",		60,0x878 }	//GPIO1_28
,{"P9_13",		31,0x874 }	//UART4_TXD
,{"P9_14",		50,0x848 }	//EHRPWM1A
,{"P9_15",		48,0x840 }	//GPIO1_16
,{"P9_16",		51,0x84c }	//EHRPWM1B
,{"P9_17",		5 ,0x95c }	//I2C1_SCL
,{"P9_18",		4 ,0x958 }	//I2C1_SDA
,{"P9_19",		13,0x97c }	//I2C2_SCL
,{"P9_20",		12,0x978 }	//I2C2_SDA
,{"P9_21",		3 ,0x954 }	//UART2_TXD
,{"P9_22",		2 ,0x950 }	//spi0_sclk
,{"P9_23",		49,0x844 }	//GPIO1_17
,{"P9_24",		15,0x984 }	//GPIO1_18
,{"P9_25",		117,0x9ac}	//GPIO3_21
,{"P9_26",		14 ,0x980}	//UART1_RXD
,{"P9_27",		115,0x9a4}	//GPIO3_19
,{"P9_28",		113,0x99c}	//SPI1_CS0
,{"P9_29",		111,0x994}	//SPI1_D0
,{"P9_30",		112,0x998}	//SPI1_D1
,{"P9_31",		110,0x990}	//SPI1_SCLK
,{"P9_41",		20 ,0}	//CLKOUT2
,{"P9_42",		7  ,0x9b4}	//GPIO0_7
};
*/

//IR受信のGPIO
//,{"P8_10",		68,0x898 }	//TIMER6
//#define IR_RECV_GPIO_PIN 68
//#define IR_RECV_GPIO_INITADDRESS 0x898

//,{"P8_15",		47,0x83c }	//GPIO1_15
#define IR_RECV_GPIO_PIN 47
#define IR_RECV_GPIO_INITADDRESS 0x83c

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



#define AM33XX_CONTROL_BASE                0x44e10000

//AM33XX の GPIO の向きを決定します.
static int GPIOInit(int pin,unsigned int flag,unsigned int init_address)
{
	char gpiolabel[20];
	int err;
	void* addr = ioremap(AM33XX_CONTROL_BASE+init_address, 4);
	if (NULL == addr)
	{
		printk(KERN_ALERT DRV_NAME " :can not mapping GPIO InitAddress %x.\n", AM33XX_CONTROL_BASE+init_address);
		return -EBUSY;
	}
	if (flag == GPIOF_IN)
	{
		iowrite32(0x7 | (2 << 3) | (1 << 5), addr);	//mode 7 (gpio), PULLUP, INPUT
	}
	else
	{
		iowrite32(0x7 | (2 << 3), addr);	//mode 7 (gpio), PULLUP, OUTPUT
	}
	iounmap(addr);

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
	DMTIMERMemory = ioremap(0x4804003C,4);

	err = GPIOInit(IR_RECV_GPIO_PIN,GPIOF_IN,IR_RECV_GPIO_INITADDRESS);
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
