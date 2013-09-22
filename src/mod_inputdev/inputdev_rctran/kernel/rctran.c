/****************************************************************************
 * Copyright (C) 1999 - Jan Edward Kansky
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 * 
 */
// Title: rctransmitter.c  Date: 10/16/99
// Author: Jan Edward Kansky
// Purpose:  Radio control flight simulator require some form of
// custom electronics to provide an interface between the transmitter gimbals
// and the flight simulation software.  The most common approach is to require
// the purchase of a dummy transmitter that plugs into a PC game port.
// This approach only allows for 4 axes of input and is not compatible with
// real RC transmitters.  See http://home-t-online.de/home/C_Jhm/
//
//  Another better approach is to use a special module that plugs into the
// transmitter "buddy cord" connector and provides an interface to the game
// port.  This allows for the real transmitter and mixing to be used, but is
// still limited to 4 axes of proportional motion.
// See http://www.welwyn.demon.co.uk/joystick.htm or
// http://home.t-online.de/home/C_Jhm/rcfspic.htm
//
//  A third approach is to interface the transmitter to the computer parallel
// port.  RealFlight sells an interface that does this, but it costs $90, and
// is only available to owners of their simulator.  I assume that this module
// has a small microcontroller inside that times the transmitter pulses and 
// communicates the results over the pc parallel port.
//
//  My approach is the "wire" approach.  I simply use the rc-transmitter PWM
// signal to generate interrupts in the pc using the parallel port interrupt
// line input.  The computer simply times the arrival of the interrupt signals
// corresponding to each rising edge of the transmitter PWM signal, and
// determines the channel positions based on these timing events.  This
// requires a two wire connection to the transmitter "trainer cord" connector.
// There is no hardware to buy.  The cost is that the computer must handle
// 450 interrupts a second.
//
//  This code is the linux module for handling parallel port interrupts
// that are generated when a radio control transmitter is connected to the 
// interrupt line on a pc parallel port.  The code measures the arrival time of
// the interrupts pulses, and reconstructs the positions of the 8 rc
// transmitter channels hased on these arrival times.   Shared memory between
// the kernel module and the user program is used for communication.
//
/****************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>

#if CONFIG_MODVERSION==1
# define MODVERSIONS
# include <linux/modversions.h>
#endif

#include <asm/irq.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include "mbuff.h"

/****************************************************************************/
#define LPT_BASE 0x378          // Use 0x3bc-LPT0, 0x378-LPT1, 0x278-LPT2
#define IRQ_LINE 7              // Parallel port IRQ line.
#define DATA_PORT LPT_BASE+0    // Address of data register for bits D0-D7
#define STATUS_PORT LPT_BASE+1  // Address of LPT status register
#define CONTROL_PORT LPT_BASE+2 // Address of LPT control register
#define CYCLES(x) __asm__ __volatile__ ("rdtsc" : "=A" (x))
#define THREE_MS 1369523

/****************************************************************************/
static void handle_transmitter_pulse(int irq_number,void *dev_id,
                                     struct pt_regs *regs);
int init_module(void);

/****************************************************************************/
static unsigned long long cycles,last,diff;
static int current_channel=1;
volatile unsigned long long *results;

/****************************************************************************/
static void handle_transmitter_pulse(int irq_number,void *dev_id,
                                     struct pt_regs *regs)
{
 if (irq_number == IRQ_LINE)
  {
   CYCLES(cycles);
   diff=cycles-last;
   last=cycles;
   if (diff > THREE_MS)
      current_channel=0;
   if (current_channel == 1) 
     results[0]=diff;
   else if (current_channel== 2)
     results[1]=diff;
   else if (current_channel==3)
     results[2]=diff;
   else if (current_channel==4)
     results[3]=diff;
   else if (current_channel==5)
     results[4]=diff;
   else if (current_channel==6)
     results[5]=diff;
   else if (current_channel==7)
     results[6]=diff;
   else if (current_channel==8)
     results[7]=diff;
   current_channel++;
  }
}

/****************************************************************************/
int init_module(void)
{
 int res;
 void (*handler)(int, void *, struct pt_regs *);

 handler=handle_transmitter_pulse;
 res=request_irq(IRQ_LINE,handler,SA_INTERRUPT,"rctran",NULL);
 if (res != 0)
  {
   printk("Request_irq for rc transmitter use failed.\n");
   if (res == -EINVAL)
    {
     printk("The IRQ number you requested was either invalid or reserved.\n");
    }
   else if (res == -ENOMEM)
    {
     printk("request_irq could not allocate memory for the new interrupt.\n");
    }
   else if (res == -EBUSY)
    {
     printk("The IRQ is already being handled and can not be shared.\n");
    }
  }
 results=(volatile unsigned long long *)mbuff_alloc("crrcsim",
         sizeof(volatile unsigned long long)*8);
 outb(inb(CONTROL_PORT) | 0x10,CONTROL_PORT);  // Enable Parallel Port IRQ
 printk("Started rctran module.\n");
 if (inb(CONTROL_PORT) & 0x10)
  {
   printk("Parallel port interrupt is enabled.\n");
  }
 else
  {
   printk("Parallel port interrupt is disabled.\n");
  }
 return 0;
}

/****************************************************************************/
void cleanup_module(void)
{
 free_irq(IRQ_LINE,NULL);
 outb(inb(CONTROL_PORT)& 0xEF,CONTROL_PORT);  // Disable Parallel Port IRQ
 mbuff_free("crrcsim",(void *)results);
 if (inb(CONTROL_PORT) & 0x10)
  {
   printk("Parallel port interrupt is enabled.\n");
  }
 else
  {
   printk("Parallel port interrupt is disabled.\n");
  }
 printk("Removed rctran module.\n");
}

/****************************************************************************/

MODULE_LICENSE("GPL");






