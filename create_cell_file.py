#! /usr/bin/python3

import argparse
from random import randint


parser = argparse.ArgumentParser()
parser.add_argument(
  '-c', type=int, default=3, help='number of coordinates')
parser.add_argument(
  '-d', type=int, default=3, help='number of decimal places')
parser.add_argument(
  '-b', type=int, default=10, help='coordinate bound')
parser.add_argument(
  '-n', type=int, help='number of points')
parser.add_argument(
  'output_name', metavar='output', type=str,
  help='output file name')
args = parser.parse_args()


def create_point_string(cd, bd, bdp, dp):
  coords = [ float(randint(-bd * 10**dp, bd * 10**dp)) / float(10**dp)
             for _ in range(cd) ]
  return " ".join( ("{" + ":+0{}.{}f".format(1+bdp+1+dp,dp) + "}").format(c) for c  in coords ) + "\n"


with open(args.output_name, 'w') as output:
  output.writelines( create_point_string(args.c, args.b, len(str(args.b)), args.d)
                     for _ in range(args.n) )
