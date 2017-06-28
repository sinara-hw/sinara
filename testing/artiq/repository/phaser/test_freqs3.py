from artiq.experiment import *

''' Demo of how to synchronize. '''

class SAWGTest(EnvExperiment):
    def build(self):
        print(self.__doc__)
        self.setattr_device("core")
        self.setattr_device("led")
        self.setattr_device("ttl_sma")

        self.setattr_device("sawg0")

    @kernel
    def run(self):
        self.core.reset()
        self.ttl_sma.output()

        # prepare
        t = 1*us
        a0 = 0.5
        self.sawg0.config.set_clr(1, 1, 1)
        self.sawg0.frequency0.set(2*MHz)
        self.sawg0.frequency1.set(0*MHz)
        self.sawg0.frequency2.set(0*MHz)
        self.sawg0.amplitude1.set(0.)
        self.sawg0.amplitude2.set(0.)
        delay(3 * ms)

        while True:
            with parallel:
                self.ttl_sma.pulse(3*us)
                self.sawg0.phase0.set(0.)

            delay(t)
            self.sawg0.amplitude1.set(a0)
            delay(t)
            self.sawg0.amplitude1.set(0.)
            delay(1*ms)

