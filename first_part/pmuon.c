// THIS IS THE TA SOLUTION
// Enables 6 perf counters, for WI20 CSE237A

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YOUR NAME");
MODULE_DESCRIPTION("PMUON");

void set_pmu(void* dummy) {
    unsigned int v;
    printk("Turn PMU on Core %d\n", smp_processor_id());

    // 1. Enable "User Enable Register"
    asm volatile("mcr p15, 0, %0, c9, c14, 0\n\t" :: "r" (0x00000001)); 

    // 2. Reset i) Performance Monitor Control Register(PMCR),
    // ii) Count Enable Set Register, and
    // iii) Overflow Flag Status Register
    asm volatile ("mcr p15, 0, %0, c9, c12, 0\n\t" :: "r"(0x00000017));
    asm volatile ("mcr p15, 0, %0, c9, c12, 1\n\t" :: "r"(0x8000003f));
    asm volatile ("mcr p15, 0, %0, c9, c12, 3\n\t" :: "r"(0x8000003f));

    // 3. Disable Interrupt Enable Clear Register
    asm volatile("mcr p15, 0, %0, c9, c14, 2\n\t" :: "r" (~0));

    // 4. Read how many event counters exist 
    asm volatile("mrc p15, 0, %0, c9, c12, 0\n\t" : "=r" (v)); // Read PMCR
    printk("We have %d configurable event counters on Core %d\n",
            (v >> 11) & 0x1f, smp_processor_id());

    // 5. Set six event counter registers (Project Assignment you need to IMPLEMENT)
	asm volatile("mcr p15, 0, %0, c9, c12, 5\n\t" :: "r"(0x00000000));
	asm volatile("mcr p15, 0, %0, c9, c13, 1\n\t" :: "r"(0x00000000));
	asm volatile("mrc p15, 0, %0, c9, c13, 1\n\t" : "=r" (v));
	printk("First event counter: %d of instructions architecturally executed",v);

	asm volatile("mcr p15, 0, %0, c9, c12, 5\n\t" :: "r"(0x00000001));
	asm volatile("mcr p15, 0, %0, c9, c13, 1\n\t" :: "r"(0x00000001));
	asm volatile("mrc p15, 0, %0, c9, c13, 1\n\t" : "=r" (v));
	printk("Second event counter: %d of L1 data cache access",v);

	asm volatile("mcr p15, 0, %0, c9, c12, 5\n\t" :: "r"(0x00000002));
	asm volatile("mcr p15, 0, %0, c9, c13, 1\n\t" :: "r"(0x00000002));
	asm volatile("mrc p15, 0, %0, c9, c13, 1\n\t" : "=r" (v));
	printk("Third event counter: %d of L1 data cache miss (refill)",v);

	asm volatile("mcr p15, 0, %0, c9, c12, 5\n\t" :: "r"(0x00000003));
	asm volatile("mcr p15, 0, %0, c9, c13, 1\n\t" :: "r"(0x00000003));
	asm volatile("mrc p15, 0, %0, c9, c13, 1\n\t" : "=r" (v));
	printk("Fourth event counter: %d of L2 data cache miss (refill)",v);

	asm volatile("mcr p15, 0, %0, c9, c12, 5\n\t" :: "r"(0x00000004));
	asm volatile("mcr p15, 0, %0, c9, c13, 1\n\t" :: "r"(0x00000004));
	asm volatile("mrc p15, 0, %0, c9, c13, 1\n\t" : "=r" (v));
	printk("Fifth event counter: %d of data memory access",v);

	asm volatile("mcr p15, 0, %0, c9, c12, 5\n\t" :: "r"(0x00000005));
	asm volatile("mcr p15, 0, %0, c9, c13, 1\n\t" :: "r"(0x00000005));
	asm volatile("mrc p15, 0, %0, c9, c13, 1\n\t" : "=r" (v));
	printk("Sixth event counter: %d of Mispredicted or not predicted branch speculatively executed",v);

	//asm volatile("mcr p15, 0, %0, c9, c13, 1\n\t" :: "r"(0x00000000))
}


int init_module(void) {
    // Set Performance Monitoring Unit (aka Performance Counter) across all cores 
    on_each_cpu(set_pmu, NULL, 1);
    printk("Ready to use PMU\n");
    return 0;
}

void cleanup_module(void) {
    printk("PMU Kernel Module Off\n");
}


