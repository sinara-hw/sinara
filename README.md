A somewhat outdated overview of the Sinara hardware vision is [here](https://github.com/m-labs/artiq-hardware). See Issues and Schematics for the latest.

# Sinara PCBs
- ARTIQ_EE folder is for designs made with the Mentor Graphics Xpedition Enterprise CAD tool.
- ARTIQ_ALTIUM folder is for designs made with Altium Designer CAD tool.

## Metlino
- ARTIQ_EE/PCB_Metlino is Metlino board housing ARTIQ master FPGA (MCH Tongues 3,4)
- ARTIQ_ALTIUM/PCB_MCH_mordovia is a bare bones clock distribution board (MCH Tongue 2)
- NAT-MCH-Base12-GbE is a commercial MCH that acts as crate controller (MCH Tongue 1)
- these three cards are stacked and shared a common front panel

## Sayma
- ARTIQ_EE/PCB_Sayma_AMC is Sayma board with high speed digital components 
- ARTIQ_EE/PCB_Sayma_RTM is Sayma RTM board housing DACs and ADCs
- ARTIQ_ALTIUM/PCB_RTM_loopback is test fixture
- ARTIQ_ALTIUM/PCB_baikal is Signal/CLK ditribution board for RF-Backplane, slot is eRTM15  ([#78](https://github.com/m-labs/sinara/issues/78))
- ARTIQ_ALTIUM/PCB_mezzanine_clock is DAC clock generation mezzanine
- ARTIQ_ALTIUM/PCB_mezzanine_clock_template is simple test board and template for more complex clock mezzanines 
- ARTIQ_ALTIUM/PCB_mezzanine_analog_template is simple test board and template for more complex analog mezzanines 
- ARTIQ_ALTIUM/PCB_mezzanine_analog_allaki is analog mezzanine for base band output and DC-100 kHz input 

## Kasli
- Kasli/PCB_Kasli is an FPGA peripheral interfaced with Metlino over SPI optical link (see [wiki](https://github.com/m-labs/sinara/wiki/Kasli), track the design [#129](https://github.com/m-labs/sinara/issues/129)) 

Several peripherals are designed to interface with Kasli and fit into 3U crates. 

- Kasli/3U/PCB_BNC is 2x4 channel, 50-Ohm TTL, bi-directional IO extender 
- Kasli/3U/PCB_RJ45 is 4x4 channel, RJ45, LVDS IO extender 
- Kasli/3U/PCB_VHDCI_breakout interfaces Kasli/3U peripheral cards with PCB_Metlino over VHDCI 

# Prototype Hardware

The first Sinara prototype hardware is designated v0.1. The components that will ship to collaborators is tracked in [#97](https://github.com/m-labs/sinara/issues/97). See [Issue 21](https://github.com/m-labs/sinara/issues/21) for a the list of additional hardware that's needed to get v0.1 running. Track project milestones [here](https://github.com/m-labs/sinara/milestones). Track status of prototype PCBs [here](https://github.com/m-labs/sinara/issues/155).

The first production version is designated v1.0. 

# Future

The tag [for-contract](https://github.com/m-labs/artiq/issues?q=is%3Aissue+is%3Aopen+label%3Atype%3Afor-contract) is applied Issues describing features not yet funded. Use the Issue system to discuss new features and find support in the community for joint development/funding. There is an equivalent [ARTIQ for-contract](https://github.com/m-labs/artiq/issues?q=is%3Aissue+is%3Aopen+label%3Atype%3Afor-contract) tag for tracking future ARTIQ features. 

https://github.com/m-labs/sinara/issues?q=is%3Aopen+is%3Aissue+label%3Astate%3Afor-contract

---------------------

###  TODO: Modules to add to git

- let ARTIQ_xx/BP_AMC/PCB_BP_AMC be microTCA.4 AMC backplane [#35](https://github.com/m-labs/sinara/issues/135)
- let ARTIQ_xx/BP_RTM/PCB_BP_RTM_RF be microTCA.4 RF backplane [#35](https://github.com/m-labs/sinara/issues/135)

--------------

The hardware designs for the Sinara device family are distributed under the CERN Open Hardware Licence v1.2.
