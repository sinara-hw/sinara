from artiq.experiment import *

class SAWGTest(EnvExperiment):
    """
    purpose: test saturation of summing junction prior to DUC
    test: sawg0 is reference tone
    setup: scope run control single, manual trigger; 500ns/div, 100mV/div
    expectation:
        a0=1 is 160 mV amplitude
        if max=0.5 then sawg1 should not exceed 80 mV
        if min=-0.5 then sawg1 should not exceed -80 mV
    observation:
        both minimum and maximum are violated
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
        delay(10 * ms)
        self.test()

    @kernel
    def test(self):
        f0 = 2*MHz
        t0 = 1*us
        a0 = 0.5

        # starting condition
        self.sawg0.amplitude1.set(2*a0)
        self.sawg0.frequency1.set(f0)
        self.sawg1.amplitude1.set(a0)
        self.sawg1.frequency1.set(f0)
        self.sawg1.amplitude2.set(a0)
        self.sawg1.frequency2.set(f0)

        # set clipping amplitude
        self.sawg1.config.set_duc_max(0.5)
        self.sawg1.config.set_duc_min(-0.5)
