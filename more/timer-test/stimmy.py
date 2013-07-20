#!/usr/bin/env python
from __future__ import division
import argparse
import logging
import sys
import serial
import json
import time

DEFAULT_PORT = None

FORMAT = '%(asctime)s %(message)s'
logging.basicConfig(stream=sys.stderr, format=FORMAT, level=logging.DEBUG)

def start_test(ser, line1):
	t_start = time.time()
	t_end = 0
	expected = json.loads(line1[9:])["expected"]
	logging.debug("%s", line1)
	test_done = False
	while not test_done:
		line = ser.readline()[:-1]
		logging.debug("%s", line)
		if "TESTEND" in line:
			test_done = True
			t_end = time.time()
	real = (t_end - t_start) * 1000
	delta = abs((expected - real) / expected) * 100
	logging.info("test took %f vs expected %f, delta: %f", real, expected, delta)
	if delta > 0.5:
		logging.warn("timing drift greater than 0.5%?")
	

def main():
	p = argparse.ArgumentParser(
		formatter_class=argparse.ArgumentDefaultsHelpFormatter,
		description="""
		Simple serial port monitor, timestamping entire lines
		as they are received.  Designed for testing platform
		timer implementations in ContikiOS
		""")
	p.add_argument(
		"-d", "--device", required=True,
		help="serial port to use")
	p.add_argument("-b", "--baud", default=115200, type=int)
	options = p.parse_args()

	ser = serial.Serial(options.device, options.baud)
	while True:
		# Strip new lines, so reprinting looks better
		line = ser.readline()[:-1]
		if "TESTSTART" in line:
			start_test(ser, line)
		else:
			logging.debug("%s", line)
			

if __name__ == '__main__':
	main()

