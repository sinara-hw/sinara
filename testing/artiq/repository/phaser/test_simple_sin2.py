from artiq.experiment import *

class SAWGTest(EnvExperiment):
    """
    purpose: test sinusoidal output
    expectation:
      - 10 MHz from sawg.frequency2 on sawg0 and sawg1
      - playback continues after program termination
    setup: scope single run, force trigger manually, 500ns/div, 100mV/div
    """
    def build(self):
        print(self.__doc__)
        self.setattr_device("core")
        self.setattr_device("led")
        self.setattr_device("ttl_sma")
        self.setattr_device("sawg0")
        self.setattr_device("sawg1")

    @kernel
    def run(self):
        self.core.reset()
        delay(300*us)
        self.sawg0.reset()
        self.sawg1.reset()
        self.ttl_sma.output()

        f0 = 15*MHz
        a0 = 0.5

        self.sawg0.frequency2.set(f0)
        self.sawg0.amplitude2.set(a0)
        self.sawg1.frequency2.set(f0)
        self.sawg1.amplitude2.set(a0)
