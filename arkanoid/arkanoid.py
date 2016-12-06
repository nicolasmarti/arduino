import mcpi.minecraft as minecraft
import mcpi.block as block
import time

import copy
import random

import serial
import glob


ports = glob.glob("/dev/ttyUSB*") + glob.glob("/dev/ttyACM*")

sr = None
for p in ports:
    try:
        sr = serial.Serial(p, 9600, timeout=0.5)
        sr.write("?")
        rl = sr.readline().split("\r")[0]
        print rl
    except Exception as e:
        print e
        sr = None

assert sr <> None

cs = [ block.GLASS.id,
       block.COAL_ORE.id,
       block.IRON_ORE.id,
       block.GOLD_ORE.id,
       block.GOLD_BLOCK.id,
       block.OBSIDIAN.id
]

h = 12
w = 20

board = (
    [ [ len(cs) -1 for x in range(0, w) ] ] +
