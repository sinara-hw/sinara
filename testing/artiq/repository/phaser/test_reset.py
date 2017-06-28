from artiq.experiment import *

class SAWGTest(EnvExperiment):
    """
    purpose: test sawg.reset()
    expectation:
      waveform stops after program termination
      timeline margin should be sufficient, no additional delay()
    setup: scope single run, force trigger manually, 500ns/div, 100mV/div
    observation: additional 200*us delay required
    """
    def build(self):
        self.setattr_device("core")
        self.setattr_device("sawg0")
        self.setattr_device("sawg1")
        self.setattr_device("ttl_sma")

    @kernel
    def run(self):

        # test for sufficient timeline margin in .reset()
        self.core.reset()
        self.ttl_sma.output()
        self.ttl_sma.pulse(1*us)

        # test for sufficient timeline margin in .reset()
        self.core.reset()
        delay(200 * us)
        self.sawg0.reset()
        self.sawg1.reset()
        self.ttl_sma.output()
        self.ttl_sma.pulse(1*us)
