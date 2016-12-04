Sayma
-----
- ARTIQ_EE/PCB_Sayma_AMC is Sayma board housing FPGA
- ARTIQ_EE/PCB_Sayma_RTM is Sayma RTM board housing DACs and ADCs
- ARTIQ_ALTIUM/PCB_mezzanine_clock is DAC clock generation mezzanine
- ARTIQ_ALTIUM/PCB_mezzanine_clock_template is simple test board and template for more complex clock mezzanines 
- ARTIQ_ALTIUM/PCB_mezzanine_analog_template is simple test board and template for more complex analog mezzanines 
- ARTIQ_ALTIUM/PCB_mezzanine_analog_allaki is analog mezzanine for base band output and DC-100 kHz input

Metlino
-------
- ARTIQ_EE/PCB_Metlino is Metlino board housing ARTIQ master FPGA
- XX is RF backplane clock distibution board  

Note
----
- ARTIQ_EE folder is for designs made with the Mentor Graphics Xpedition Enterprise CAD tool.
- ARTIQ_ALTIUM folder is for designs made with Altium Designer CAD tool.

Modules to add to git
---------------------
- let ARTIQ_xx/BP_AMC/PCB_BP_AMC be microTCA.4 AMC backplane (no schematic currently available) 
- let ARTIQ_xx/BP_RTM/PCB_BP_RTM_RF be microTCA.4 RF backplane (no schematic currently available)
- let ARTIQ_xx/BP_RTM/PCB_baikal be Signal/CLK ditribution board for RF-Backplane, slot is eRTM15  (Issue #78)
- let ARTIQ_xx/MCH/PCB_MCH_Tongues12 be MCH Tongues 1, 2 module for AMC clock distribution (in future this will replace use of NAT MCH Tongue 1 module) 

--------------
The hardware designs for the Sinara device family are distributed under the
CERN Open Hardware Licence v1.2.
