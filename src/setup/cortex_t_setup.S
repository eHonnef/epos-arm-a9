
// EPOS Cortex-A SETUP

        .file "cortex_a_setup.S"

        // Interrupt Vector Table
        .section .init
        .type _vector_table, object
_vector_table:
        ldr pc, _reset_addr
        ldr pc, _undefined_instruction_addr
        ldr pc, _swi_addr
        ldr pc, _prefetch_abort_addr
        ldr pc, _data_abort_addr
        ldr pc, _reserved_addr
        ldr pc, _irq_addr
        ldr pc, _fiq_addr

 _reset:                                                                  
        // 1.MMU, L1$ disable                                                                         
        //-----------------------------------------------                                             
        MRC p15, 0, r1, c1, c0, 0   // Read System Control Register (SCTLR)                           
        BIC r1, r1, #1              // mmu off                                                        
        BIC r1, r1, #(1 << 12)      // i-cache off                                                    
        BIC r1, r1, #(1 << 2)       // d-cache & L2-$ off                                             
        MCR p15, 0, r1, c1, c0, 0   // Write System Control Register (SCTLR)                          
        //-----------------------------------------------                                             
        // 2. invalidate: L1$, TLB, branch predictor                                                  
        //-----------------------------------------------                                             
        MOV r0, #0                                                                                    
        MCR p15, 0, r0, c7, c5, 0   // Invalidate Instruction Cache                                   
        MCR p15, 0, r0, c7, c5, 6   // Invalidate branch prediction array                             
        MCR p15, 0, r0, c8, c7, 0   // Invalidate entire Unified Main TLB                             
        ISB                         // instr sync barrier                                             
        //-----------------------------------------------                                             
        // 2.a. Enable I cache + branch prediction                                                    
        //-----------------------------------------------                                             
        MRC p15, 0, r0, c1, c0, 0   // System control register                                        
        ORR r0, r0, #1 << 12        // Instruction cache enable                                       
        ORR r0, r0, #1 << 11        // Program flow prediction                                        
        MCR p15, 0, r0, c1, c0, 0   // System control register                                        
        //-----------------------------------------------                                             
                                                                                                      
                                                                                                      
        MRC p15, 1, R0, c0, c0, 1   // Read CLIDR into R0                                             
        ANDS R3, R0, #0x07000000                                                                      
        MOV R3, R3, LSR #23         // Cache level value (naturally aligned)                          
        BEQ Finished                                                                                  
        MOV R10, #0                                                                                   
    Loop1:                                                                                            
        ADD R2, R10, R10, LSR #1    // Work out 3 x cachelevel                                        
        MOV R1, R0, LSR R2          // bottom 3 bits are the Cache type for this level                
        AND R1, R1, #7              // get those 3 bits alone                                         
        CMP R1, #2                                                                                    
        BLT Skip                    // no cache or only instruction cache at this                     
        MCR p15, 2, R10, c0, c0, 0  // write CSSELR from R10                                          
        ISB                         // ISB to sync the change to the CCSIDR                           
        MRC p15, 1, R1, c0, c0, 0   // read current CCSIDR to R1                                      
        AND R2, R1, #7              // extract the line length field                                  
        ADD R2, R2, #4              // add 4 for the line length offset (log2 16 bytes)               
        LDR R4, =0x3FF                                                                                
        ANDS R4, R4, R1, LSR #3     // R4 is the max number on the way size (right aligned)           
        CLZ R5, R4                  // R5 is the bit position of the way size increment               
        MOV R9, R4                  // R9 working copy of the max way size (right aligned)            
    Loop2:                                                                                            
        LDR R7, =0x00007FFF                                                                           
        ANDS R7, R7, R1, LSR #13    // R7 is the max number of the index size (right aligned)         
    Loop3:                                                                                            
        ORR R11, R10, R9, LSL R5    // factor in the way number and cache number into R11             
        ORR R11, R11, R7, LSL R2    // factor in the index number                                     
        MCR p15, 0, R11, c7, c10, 2 // DCCSW, clean by set/way                                        
        SUBS R7, R7, #1             // decrement the index                                            
        BGE Loop3                                                                                     
        SUBS R9, R9, #1             // decrement the way number                                       
        BGE Loop2                                                                                     
                                                                                                      
    Skip:                                                                                             
        ADD R10, R10, #2            // increment the cache number                                     
        CMP R3, R10                                                                                   
        BGT Loop1                                                                                     
        DSB                                                                                           
    Finished:                                                                                         
        ADD R4, #0                                                                                    
        ADD R4, R4, #10                                                                               
                                                                                                      
mmu_start:                                                                                
        // Disable MMU                                                                        
        MRC p15, 0, r1, c1, c0, 0	// Atribui-se ao R1 o valor do registrador 1 do           
                                    // coprocessor 15                                         
        BIC r1, r1, #0x1           	// Atribui-se ao bit 0 em R1 o valor 0, para              
                                    // desligar a MMU                                         
        MCR p15, 0, r1, c1, c0, 0	// Escreve-se no reg 1 do coprocessor 15                  
                                    // o que ha em R1, desabilitando a MMU                    
                                                                                              
        // Disable L1 Caches                                                                  
        MRC p15, 0, r1, c1, c0, 0	// Read Control Register configuration data               
        BIC r1, r1, #(0x1 << 12)	// Disable I Cache                                        
        BIC r1, r1, #(0x1 << 2)		// Disable D Cache                                        
        MCR p15, 0, r1, c1, c0, 0	// Write Control Register configuration data              
                                                                                              
        // Invalidate L1 Caches                                                               
        // Invalidate Instruction cache                                                       
        MOV r1, #0                                                                            
        MCR p15, 0, r1, c7, c5, 0                                                             
                                                                                              
        // Invalidate Data cache                                                              
        // to make the code general purpose, we calculate the                                 
        // cache size first and loop through each set + way                                   
        MRC p15, 1, r0, c0, c0, 0  	// Read Cache Size ID                                     
        LDR r3, =#0x1ff                                                                       
        AND r0, r3, r0, LSR #13		// r0 = no. of sets - 1                                   
        MOV r1, #0			        // r1 = way counter way_loop                              
        way_loop:                                                                             
        MOV r3, #0			        // r3 = set counter set_loop                              
        set_loop:                                                                             
        MOV r2, r1, LSL #30                                                                   
        ORR r2, r3, LSL #5 		    // r2 = set/way cache operation format                    
        MCR p15, 0, r2, c7, c6, 2 	// Invalidate line described by r2                        
        ADD r3, r3, #1 			    // Increment set counter                                  
        CMP r0, r3 			        // Last set reached yet?                                  
        BGT set_loop 			    // if not, iterate set_loop                               
        ADD r1, r1, #1 			    // else, next                                             
        CMP r1, #4 			        // Last way reached yet?                                  
        BNE way_loop 			    // if not, iterate way_loop                               
                                                                                              
        // Invalidate TLB                                                                     
        MCR p15, 0, r1, c8, c7, 0                                                             
                                                                                              
        // Aqui é criada uma L1 translation table na RAM que divide                           
        // todo o espaço de endereçamento de 4GB em seções de 1 MB,                           
        // todas com Full Access e Strongly Ordered                                           
        LDR r0, =0xDE2			        // Atribui-se ao R0 parte do descriptor               
        LDR r1, =0xFA0000       	    // Atribui-se ao R1 endereço base                     
                                        // da L1 tranlastion table                            
        LDR r3, = 4095         		    // R3 se torna o contador para o loop                 
                                                                                              
        write_pte:                  	// Label do loop para escrita das                     
                                        // page table entry (PTE) da translation table        
            ORR r2, r0, r3, LSL #20     // Atribui-se ao R2 OR entre o endereço               
                                        // e os bits padrão da PTE                            
        STR r2, [r1, r3, LSL #2]    	// Escreve-se a PTE na translation table              
                                        // (endereço de escrita é o ttb_address somado        
                                        // com contador e multiplicado por 4)                 
        SUB r3, r3, #1              	// Decrementa-se contador do loop                     
        CMP r3, #-1                 	// Faz-se a comparação para verificar                 
                                        // se loop acabou                                     
        BNE write_pte               	// Caso o loop não tenha acabado,                     
                                        // escreve mais uma pte                               
                                                                                              
        // Faz-se a primeira entrada da translation table                                     
        // cacheable, normal, write-back, write allocate                                      
        BIC r0, r0, #0xC		    // Limpa-se CB bits                                       
        ORR r0, r0, #0X4 		    // Write-back, write allocate                             
        BIC r0, r0, #0x7000 		// Limpa-se TEX bits                                      
        ORR r0, r0, #0x5000 		// Faz-se TEX write-back e write allocate                 
        ORR r0, r0, #0x10000 		// Torna compartilhável                                   
        STR r0, [r1]			    // Escreve-se na primeira entrada                         
                                                                                              
        // Inicializa a MMU                                                                   
        MOV r1,#0x0                                                                           
            MCR p15, 0, r1, c2, c0, 2   // Escrita do Translation Table Base Control Register 
        LDR r1, =0xFA0000   		    // Atribui-se ao R1 endereço base                     
                                        // da L1 tranlastion table                            
        MCR p15, 0, r1, c2, c0, 0	    // Escreve-se no reg 1 do coprocessor 15 o que ha     
                                        // em r1 (endereco base da tranlastion table)         
                                                                                              
        // In this simple example, we don't use TRE or Normal Memory Remap Register.          
        // Set all Domains to Client                                                          
        LDR r1, =0x55555555                                                                   
        MCR p15, 0, r1, c3, c0, 0       // Write Domain Access Control Register               
                                                                                              
        // Enable MMU                                                                         
        MRC p15, 0, r1, c1, c0, 0	    // Atribui-se ao R1 o valor do registrador 1 do       
                                        // coprocessor 15                                     
        ORR r1, r1, #0x1           	    // Atribui-se ao bit 0 em R1 o valor 1, para          
                                        // ligar a MMU                                        
        MCR p15, 0, r1, c1, c0, 0	    // Escreve-se no reg 1 do coprocessor 15              
                                        // o que há em R1, habilitando a MMU                  
                                                                                              
        b _start_addr                                                                              

_start_addr:
        .word _start
_reset_addr:
        .word _reset
_undefined_instruction_addr:
        .word _undefined_instruction
_swi_addr:
        .word _software_interrupt
_prefetch_abort_addr:
        .word _prefetch_abort
_data_abort_addr:
        .word _data_abort
_reserved_addr:
        .word _reserved
_irq_addr:
        .word _int_entry
_fiq_addr:
        .word _fiq
