from artiq.experiment import *

class SAWGTest(EnvExperiment):
    """
    sawg0 is a reference channel
    sawg1 has its phase swept in a loop
    """
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
        self.sawg_reset()

        for ph in range(0, 100):
            for i in range(0, 100):
                self.test(ph/100.0)
                self.ttl_sma.pulse(3 * us)
                delay(1 * ms)

    @kernel
    def sawg_reset(self):
        self.sawg0.frequency0.set(0.0)
        self.sawg0.frequency0.set(0.0)
        self.sawg0.phase0.set(0.0)
        self.sawg0.amplitude1.set(0.0)
        self.sawg0.frequency1.set(0.0)
        self.sawg0.phase1.set(0.0)
        self.sawg0.amplitude2.set(0.0)
        self.sawg0.frequency2.set(0.0)
        self.sawg0.phase2.set(0.0)
        self.sawg0.config.set_clr(1, 1, 1)

        self.sawg1.frequency0.set(0.0)
        self.sawg1.frequency0.set(0.0)
        self.sawg1.phase0.set(0.0)
        self.sawg1.amplitude1.set(0.0)
        self.sawg1.frequency1.set(0.0)
        self.sawg1.phase1.set(0.0)
        self.sawg1.amplitude2.set(0.0)
        self.sawg1.frequency2.set(0.0)
        self.sawg1.phase2.set(0.0)
        self.sawg1.config.set_clr(1, 1, 1)

    @kernel
    def test(self, ph):
        # test: t1
        f0 = 1*MHz
        t = 1*us
        a0 = 0.78

        # prepare
        self.sawg0.frequency1.set(f0)
        self.sawg0.amplitude1.set(a0)
        self.sawg1.frequency1.set(f0)
        self.sawg1.amplitude1.set(a0)

        delay(t)
        self.sawg1.phase1.set(ph)
        delay(t)

        # switch RF off
        self.sawg0.amplitude1.set(0.)
        self.sawg1.amplitude1.set(0.)
