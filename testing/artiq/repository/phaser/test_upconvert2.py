from artiq.experiment import *

class SAWGTest(EnvExperiment):
    """
    purpose: test two-tone up-conversion feature of sawg
    test: mix LO = 150 MHz, IF1 = 25-75 MHz, IF2 = 25-75 MHz
    expectation:
        RF: LO+IF1 and LO+IF2 are the desired features
        RF: LO-IF1 and LO-IF2
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

        f0 = 150
        fmin = 25
        fmax = 75
        a = 0.50

        self.sawg0.frequency0.set(f0*MHz)
        self.sawg0.amplitude1.set(a)
        self.sawg0.amplitude2.set(a)

        for f1 in range(fmin, fmax, 5):
            self.sawg0.frequency1.set(f1 * MHz)
            for f2 in range(fmin, fmax, 5):
                self.sawg0.frequency2.set(f2 * MHz)
                delay(100 * ms)
