syscall/readme.txt
==================

This directory supports a syscall layer from communication between a
monolithic, kernel-mode Melis kernel and a separately built, user-mode
application, driver, module set.

With most MCUs on most occasions, RTOS is built as a flat, single executable
image containing the  RTOS along with all application code.  The RTOS code
and the application run in the same address space and at the same kernel-
mode privileges.  In order to exploit security features of certain
processors, an alternative build model is also supported:  Melis can
be built separately as a monolithic, kernel-mode module and the applications
can be added as a separately built, user-mode module.

The syscall layer provided in this directory serves as the communication
layer from the user-mode application into the kernel-mode RTOS.  The
switch from user-mode to kernel-mode is accomplished using software
interrupts (SWIs).  SWIs are implemented differently and named differently
by different manufacturers but all work essentially the same:  A special
instruction is executed in user-mode that causes a software generated
interrupt.  The software generated interrupt is caught within the kernel
and handle in kernel-mode.
