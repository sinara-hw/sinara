from artiq.experiment import *

class SAWGTest(EnvExperiment):
    """
    purpose: test cancellation of sawg0.frequency0 and sawg0.frequency1
    setup: trigger scope on ttl_sma
    expectation: sawg0 amplitude is zero
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

        while True:
            for f in range(1, 75):
                for a in range(0, 49):
                    self.test(f*MHz, a/100.0)

    @kernel
    def test(self, f, a):
        t0 = 1*us

        self.ttl_sma.pulse(3 * us)

        self.sawg0.amplitude1.set(a)
        self.sawg0.frequency1.set(f)
        self.sawg0.phase1.set(0.)

        self.sawg0.amplitude2.set(a)
        self.sawg0.frequency2.set(f)
        self.sawg0.phase2.set(0.5)

        delay(1*ms)

