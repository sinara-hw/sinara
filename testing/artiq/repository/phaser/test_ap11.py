from artiq.experiment import *

class SAWGTest(EnvExperiment):
    """
    purpose: test continuous phase mode
    test: sawg0.frequency0 is reference tone; advance phase of sawg1.frequency1
    setup: scope trigger on ttl_sma; 500ns/div, 100mV/div
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
        delay(300 * us)
        self.ttl_sma.output()

        delay(10 * ms)

        t0 = now_mu()
        while now_mu() < t0 + 2e9:
            self.test()

    @kernel
    def test(self):
        f0 = 5*MHz
        t0 = 1*us
        a0 = 0.5

        self.ttl_sma.pulse(3 * us)

        # sawgN.reset() defaults to absolute phase mode
        self.sawg0.amplitude1.set(a0)
        self.sawg0.frequency0.set(f0)
        self.sawg0.phase0.set(0.)

        self.sawg1.amplitude1.set(a0)
        self.sawg1.frequency1.set(f0)
        self.sawg1.phase1.set(0.)

        delay(t0)

        self.sawg1.config.set_clr(1, 0, 1)
        self.sawg1.phase1.set(0.5)
        self.sawg1.config.set_clr(1, 1, 1)

        delay(t0)

        self.sawg0.amplitude1.set(0.)
        self.sawg1.amplitude1.set(0.)

        delay(1*ms)

