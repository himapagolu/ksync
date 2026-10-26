# ksync

A Linux kernel module that implements the producer-consumer (bounded-buffer) problem using kernel threads and semaphores.

## Build and run

Requires a Linux system with kernel headers installed (`sudo apt install linux-headers-$(uname -r)` on Debian/Ubuntu).

```bash
# Build
make

# Load (prod = producers, cons = consumers, size = buffer capacity)
sudo insmod producer_consumer.ko prod=3 cons=2 size=5

# Watch the output
sudo dmesg -w

# Unload
sudo rmmod producer_consumer

# Clean build files
make clean
```

## Concepts demonstrated

- Linux kernel module lifecycle (`module_init` / `module_exit`)
- Kernel threads (`kthread_run`, `kthread_stop`, `kthread_should_stop`)
- Counting semaphores (`sema_init`, `down_interruptible`, `up`)
- Kernel memory allocation (`kmalloc` / `kfree`)
- Load-time configuration with `module_param`
