from artiq.experiment import *

class SAWGTest(EnvExperiment):
    """
    purpose: test phase coherence of frequency jumps
    test:  sawg0.frequency0 is reference tone
           sawg1.frequency0 is tone that jumps
    setup: trigger scope on ttl_sma, 500ns/, 100mv/
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
        delay(1*ms)

        t0 = now_mu()
        while now_mu() < t0 + 2e9:
            self.test()

    @kernel
    def test(self):
        t = 1*us
        a0 = 0.5

        self.ttl_sma.pulse(3 * us)

        # sawg0 is reference tone
        self.sawg0.frequency0.set(2*MHz)
        self.sawg0.phase0.set(0.)
        self.sawg0.amplitude1.set(a0)

        self.sawg1.frequency0.set(2 * MHz)
        self.sawg1.phase0.set(0.)
        self.sawg1.amplitude1.set(a0)

        delay(t)

        self.sawg1.frequency0.set(4*MHz)
        delay(t)

        self.sawg1.frequency0.set(2*MHz)
        delay(t)

        # switch RF off
        self.sawg0.amplitude1.set(0.)
        self.sawg1.amplitude1.set(0.)
        delay(1 * ms)