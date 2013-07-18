#!/usr/bin/env python
import argparse
import logging
import sys
import serial

DEFAULT_PORT = None

FORMAT = '%(asctime)s %(message)s'
logging.basicConfig(stream=sys.stderr, format=FORMAT, level=logging.DEBUG)

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
		logging.info("%s", ser.readline()[:-1])

if __name__ == '__main__':
	main()

