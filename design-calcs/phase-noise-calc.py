import numpy as np
import matplotlib.pyplot as plt
import argparse
from artiq.protocols import pyon
# $ export KMP_AFFINITY=disabled


def adc_snr():
	"""Calculate some things related to SNR of ADC.
	See AD9656 spec sheet Fig 61"""

	print("Assume: ADC differential nonlinearity and input noise is 0.")
	print("Focus: given an signal frequency and desired resolution what is maximum tolerable clock jitter?")	
	fas = [62.5e6, 187.5e6]
	nbits = [12, 15]
	print("{:>12} {:>6}    {:>6} "
		.format("fa (MHz)","nbit", "fs RMS"))
	for fa in fas:
		for nbit in nbits:
			trms = 10**((6.02*nbit+1.76)/-20)/(2*np.pi*fa)
			print("{:12} {:6}    {:6.0f}"
				.format(fa/1e6, nbit, trms*1e15))


class PhaseNoiseDataset():
	"""Class to collect and analyze phase noise data"""

	def __init__(self, fname: str):
		self.db = pyon.load_file(fname) 

	def extrapolate_high_end(self):
		"""Many manufacturers don't measure 
		far-from-carrier phase noise for oscillators. This 
		routine assumes it is white out to 200 MHz."""
		db = self.db
		for key in db:
			pn = np.array(db[key]['pn'])
			if pn[-1,0]<200e6:
				pn = np.append(pn,[[200e6,pn[-1,1]]], axis=0)
				db[key]['pn'] = pn
		self.db = db

	def keys(self) -> list:
		return [x for x in self.db.keys()]

	def jitter(self, keys=[]) -> []:
		"""Jitter calculation based on Analog tech note
		MT-008 "Converting Oscillator Phase Noise to Time Jitter". 
		Assumes phase noise is single sideband in units
		of dBc/Hz. 
		"""
		db = self.db
		debug = False
		if keys == []:
			keys = [x for x in db.keys()]
			debug = True
		if debug:
			print("{:30} {b:}".format("", b="RMS jitter (s)"))
		jitters = []
		for key in keys:
			fosc = db[key]['fosc']
			pn = np.array(db[key]['pn'])
			source = np.array(db[key]['source'])
			f = pn[:,0]
			dbc = pn[:,1]
			area = np.trapz(10**(dbc/10), x=f)
			rms_phase_jitter = np.sqrt(2*area)
			rms_time_jitter = rms_phase_jitter/(2*np.pi*fosc)
			if debug: 
				print("{:<30} {:<6.0f} fs  {}".
					format(key, rms_time_jitter*1e15, source))
			jitters.append(rms_time_jitter)
		return jitters

	def plot(self, fcomp:int, keys:list = []) -> None:
		"""compare phase noise normalized to fcomp"""
		db = self.db
		if keys == []:
			keys = [x for x in db.keys()]
		plt.clf()		
		for key in keys:
			fosc = db[key]['fosc']
			pn = np.array(db[key]['pn'])
			f = pn[:,0]
			dbc = pn[:,1] + 20*np.log10(fcomp/fosc)
			jitter = self.jitter([key])[0]*1e15
			s = '{:} ({:.0f}fs)'.format(key, jitter)
			plt.semilogx(f,dbc, label=s)
		plt.ylim([-180, -80])
		plt.grid()
		plt.legend()
		plt.show()



pndata = PhaseNoiseDataset('phase-noise-osc.dat')

pndata.extrapolate_high_end()
pndata.plot(100e6, [
	'SDI_LNFR-100', 'wenzel_std_100', 'HMC7044_122_Fig6','LTC6957-1_100'])
adc_snr()
