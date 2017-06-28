from artiq.experiment import *

class SAWGTest(EnvExperiment):
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
        self.ttl_sma.output()
        delay(3*ms)

        while True:
            self.ttl_sma_compensated_pulse(3*us)
            self.freqs()
            delay(1*ms)

    @kernel
    def freqs(self):
        # test: t1
        tarm = 1*us
        tpi = 1*us
        f = 5*MHz
        df = 5*MHz
        a0 = 0.5

        # prepare reference tone
        self.sawg0.config.set_clr(1, 1, 1)
        self.sawg0.frequency0.set(f)
        self.sawg0.frequency1.set(0*MHz)
        self.sawg0.frequency2.set(0*MHz)
        self.sawg0.amplitude1.set(a0)
        self.sawg0.amplitude2.set(a0)

        # prepare RF tone for SE
        self.sawg1.config.set_clr(0, 0, 0)
        self.sawg1.frequency0.set(0*MHz)
        self.sawg1.frequency1.set(0*MHz)
        self.sawg1.frequency2.set(0*MHz)
        self.sawg1.amplitude1.set(a0)
        self.sawg1.amplitude2.set(a0)

        # pulse 1
        self.sawg1.frequency0.set(f)
        self.ttl_sma_compensated_pulse(tpi/2)
        self.sawg1.frequency0.set(f+df)

        delay(tarm)  # arm 1

        # pulse 2
        self.sawg1.frequency0.set(f)
        self.sawg1.phase0.set(0.5)
        self.ttl_sma_compensated_pulse(tpi)
        self.sawg1.phase0.set(0.)
        self.sawg1.frequency0.set(f+df)

        delay(tarm)  # arm 2

        # pulse 3
        self.sawg1.frequency0.set(f)
        self.ttl_sma_compensated_pulse(tpi)
        self.sawg1.frequency0.set(f+df)

        delay(tarm)

        # switch RF off
        self.sawg1.amplitude1.set(0.)
        self.sawg1.amplitude2.set(0.)
