#include "typedefs.h"
#include "bootdefs.h"
#include "simple_serial.h"

void print_exception(u32 *regs)
{
   serial_put_string("Exception caught Dump:\n"); 
   serial_put_string("Zero 0x"); 
   serial_put_num_hex(regs[0],8);
   serial_put_string(" AT 0x"); 
   serial_put_num_hex(regs[1],8);
   serial_put_string(" v0 0x"); 
   serial_put_num_hex(regs[2],8);
   serial_put_string(" v1 0x"); 
   serial_put_num_hex(regs[3],8);
   serial_put_string("\na0 0x"); 
   serial_put_num_hex(regs[4],8);
   serial_put_string(" a1 0x"); 
   serial_put_num_hex(regs[5],8);
   serial_put_string(" a2 0x"); 
   serial_put_num_hex(regs[6],8);
   serial_put_string(" a3 0x"); 
   serial_put_num_hex(regs[7],8);
   
   
   serial_put_string("\nt0 0x"); 
   serial_put_num_hex(regs[8],8);
   serial_put_string(" t1 0x"); 
   serial_put_num_hex(regs[9],8);
   serial_put_string(" t2 0x"); 
   serial_put_num_hex(regs[10],8);
   serial_put_string(" t3 0x"); 
   serial_put_num_hex(regs[11],8);
   serial_put_string("\nt4 0x"); 
   serial_put_num_hex(regs[12],8);
   serial_put_string(" t5 0x"); 
   serial_put_num_hex(regs[13],8);
   serial_put_string(" t6 0x"); 
   serial_put_num_hex(regs[14],8);
   serial_put_string(" t7 0x"); 
   serial_put_num_hex(regs[15],8);

   serial_put_string("\ns0 0x"); 
   serial_put_num_hex(regs[16],8);
   serial_put_string(" s1 0x"); 
   serial_put_num_hex(regs[17],8);
   serial_put_string(" s2 0x"); 
   serial_put_num_hex(regs[18],8);
   serial_put_string(" s3 0x"); 
   serial_put_num_hex(regs[19],8);
   serial_put_string("\ns4 0x"); 
   serial_put_num_hex(regs[20],8);
   serial_put_string(" s5 0x"); 
   serial_put_num_hex(regs[21],8);
   serial_put_string(" s6 0x"); 
   serial_put_num_hex(regs[22],8);
   serial_put_string(" s7 0x"); 
   serial_put_num_hex(regs[23],8);   
   
   serial_put_string("\nt8 0x"); 
   serial_put_num_hex(regs[24],8);
   serial_put_string(" t9 0x"); 
   serial_put_num_hex(regs[25],8);
   serial_put_string(" k0 0x"); 
   serial_put_num_hex(regs[26],8);
   serial_put_string(" k1 0x"); 
   serial_put_num_hex(regs[27],8);
   serial_put_string("\ngp 0x"); 
   serial_put_num_hex(regs[28],8);
   serial_put_string(" sp 0x"); 
   serial_put_num_hex(regs[29],8);
   serial_put_string(" fp 0x"); 
   serial_put_num_hex(regs[30],8);
   serial_put_string(" ra 0x"); 
   serial_put_num_hex(regs[31],8);
   
   serial_put_string("\nHI 0x"); 
   serial_put_num_hex(regs[32],8);
   serial_put_string(" LO 0x"); 
   serial_put_num_hex(regs[33],8);
   serial_put_string(" CONTEXT 0x"); 
   serial_put_num_hex(regs[4+34],8);
   serial_put_string(" BADVADDR 0x"); 
   serial_put_num_hex(regs[8+34],8);
   serial_put_string("\nSTATUS 0x"); 
   serial_put_num_hex(regs[12+34],8);
   serial_put_string(" CAUSE 0x"); 
   serial_put_num_hex(regs[13+34],8);
   serial_put_string(" EPC 0x"); 
   serial_put_num_hex(regs[14+34],8);
   serial_put_string(" CONFIG 0x"); 
   serial_put_num_hex(regs[16+34],8);
   
   serial_put_string("\nXCONTEXT 0x"); 
   serial_put_num_hex(regs[20+34],8);
   serial_put_string(" ERROREPC 0x"); 
   serial_put_num_hex(regs[30+34],8);
   serial_put_string("\n\nStack Dump:"); 
   u32 *sp = (u32*) regs[29];
   int i = 0;
   while (sp >= (u32 *) KSEG0 && i < 128 && sp < (u32 *) STACK_START) {
     if (i % 8 == 0)
        serial_put_string("\n"); 
     serial_put_string(" "); 
     serial_put_num_hex(*sp,8);
     i++;
     sp++;
   }
   serial_put_string("\n\nCode Dump:"); 
   u32 *epc = (u32*) regs[14+34];
   i = 0;
   while (epc >= (u32 *) KSEG0 && i < 128) {
     if (i % 8 == 0)
        serial_put_string("\n"); 
     serial_put_string(" "); 
     serial_put_num_hex(*epc,8);
     i++;
     epc++;
   }
   serial_put_string("\n\nPANIC!\n\n"); 

   while(1) ;
}
