from artiq.experiment import *

class SAWGTest(EnvExperiment):
    """
    purpose: test single-tone up conversion
    test: mix LO = 150 MHz, IF = 25 MHz
    expectation:
        RF: LO+IF at approximately -13 dBm is the desired feature
        RF: LO-IF at approximately -13 dBm
    setup: spectrum analyzer with range set to 100 kHz to 500 MHz
    """
    def build(self):
        print(self.__doc__)
        self.setattr_device("core")
        self.setattr_device("sawg0")
        self.setattr_device("sawg1")

    @kernel
    def run(self):
        self.core.reset()
        delay(300*us)
        self.sawg0.reset()
        self.sawg1.reset()

        self.sawg0.frequency0.set(150*MHz)
        self.sawg0.amplitude1.set(0.5)
        self.sawg0.frequency1.set(25*MHz)