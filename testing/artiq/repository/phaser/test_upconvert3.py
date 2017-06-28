from artiq.experiment import *

class SAWGTest(EnvExperiment):
    """

    purpose: test amplitude dependence of two-tone up-conversion
    expectation:
        Since mixing is all digital spectral content should not vary with amplitude. That is,
        digital mixer should have no amplitude-dependent non-linearity.
    setup: spectrum analyzer with range set to 100 kHz to 500 MHz
    """
    def build(self):
        print(self.__doc__)
        self.setattr_device("core")
        self.setattr_device("sawg0")

    @kernel
    def run(self):
        self.core.reset()
        delay(300*us)
        self.sawg0.reset()

        amin = 1
        amax = 49

        self.sawg0.frequency0.set(150*MHz)
        self.sawg0.frequency1.set(25*MHz)
        self.sawg0.frequency2.set(40*MHz)

        for a in range(amin, amax, 5):
            self.sawg0.amplitude1.set(a/100.0)
            self.sawg0.amplitude2.set(a/100.0)
            delay(1000 * ms)
