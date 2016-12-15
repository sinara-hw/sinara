
Metlino
-------
- ARTIQ_EE/PCB_Metlino is Metlino board housing ARTIQ master FPGA (MCH Tongues 3,4)
- ARTIQ_ALTIUM/PCB_MCH_mordovia is a bare bones clock distribution board (MCH Tongue 2)
- NAT-MCH-Base12-GbE is a commercial MCH that acts as crate controller (MCH Tongue 1)
- these three cards are stacked and shared a common front panel

Sayma
-----
- ARTIQ_EE/PCB_Sayma_AMC is Sayma board with high speed digital components 
- ARTIQ_EE/PCB_Sayma_RTM is Sayma RTM board housing DACs and ADCs
- ARTIQ_ALTIUM/PCB_baikal is Signal/CLK ditribution board for RF-Backplane, slot is eRTM15  (Issue #78)
- ARTIQ_ALTIUM/PCB_mezzanine_clock is DAC clock generation mezzanine
- ARTIQ_ALTIUM/PCB_mezzanine_clock_template is simple test board and template for more complex clock mezzanines 
- ARTIQ_ALTIUM/PCB_mezzanine_analog_template is simple test board and template for more complex analog mezzanines 
- ARTIQ_ALTIUM/PCB_mezzanine_analog_allaki is analog mezzanine for base band output and DC-100 kHz input 

Note
----
- ARTIQ_EE folder is for designs made with the Mentor Graphics Xpedition Enterprise CAD tool.
- ARTIQ_ALTIUM folder is for designs made with Altium Designer CAD tool.
- See Issue 21_ for a list of hardware that comprises a Sinara system. 
.. _21: https://github.com/m-labs/sinara/issues/21

Modules to add to git
---------------------
- let ARTIQ_xx/BP_AMC/PCB_BP_AMC be microTCA.4 AMC backplane (no schematic currently available) 
- let ARTIQ_xx/BP_RTM/PCB_BP_RTM_RF be microTCA.4 RF backplane (no schematic currently available)

--------------
The hardware designs for the Sinara device family are distributed under the
CERN Open Hardware Licence v1.2.
