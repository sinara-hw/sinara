from artiq.experiment import *

class SAWGTest(EnvExperiment):
    def build(self):
        self.setattr_device("core")
        self.setattr_device("led")
        self.setattr_device("ttl_sma")

        self.setattr_device("sawg0")
        self.setattr_device("sawg1")

    @kernel
    def run(self):
        self.core.reset()
        self.ttl_sma.output()
        delay(3*ms)

        while True:
            self.ttl_sma.pulse(3*us)
            self.ap()
            delay(1*ms)

    @kernel
    def ap(self):
        # test: t1
        f0 = 1*MHz
        t = 1*us
        a0 = 1.0

        # amplitude should be a0
        self.sawg1.frequency0.set(0*MHz)
        self.sawg1.frequency1.set(f0)
        self.sawg1.frequency2.set(0*MHz)
        self.sawg1.amplitude1.set(a0)
        self.sawg1.amplitude2.set(a0)
        delay(t)

        # amplitude should be 2*a0
        self.sawg1.frequency0.set(0*MHz)
        self.sawg1.frequency1.set(f0)
        self.sawg1.frequency2.set(f0)
        self.sawg1.amplitude1.set(a0)
        self.sawg1.amplitude2.set(a0)
        delay(t)

        # switch RF off
        self.sawg0.amplitude1.set(0.)
        self.sawg1.amplitude1.set(0.)
