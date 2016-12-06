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
       block.WOOD.id,
       block.COAL_ORE.id,
       block.IRON_ORE.id,
       block.GOLD_ORE.id,
       block.GOLD_BLOCK.id,
       block.OBSIDIAN.id
]

h = 12
w = 20

board = (
    [ [ len(cs) - 1 for x in range(0, w) ] ] +
    [ [ len(cs) - 1 ] + [ 0 for x in range(0, w-2) ] + [ len(cs) - 1] for y in range(0, h-1) ]
    )
    
def move_ball( b, x, y, dx, dy ):

    if dx > 0 and x == len(b) - 1:
        return None

    if b[x][y+dy] <> 0:

        if b[x][y+dy] < len(cs) - 1:
            b[x][y+dy] -= 1

        dy = -dy

    elif b[x+dx][y] <> 0:

        if b[x+dx][y] < len(cs) - 1:
            b[x+dx][y] -= 1
                
        dx = -dx
                
    if b[x+dx][y+dy] == 0:
        return (b, x + dx, y + dy, dx, dy)
    else:
        return move_ball(b, x, y, dx, dy)
    
mc = minecraft.Minecraft.create()
   
pos = mc.player.getTilePos()

# wipe-out


mc.setBlocks(
    pos.x + 60, pos.y , pos.z + 20,
    pos.x - 60, pos.y + 10, pos.z - 20,
    block.AIR.id
)


    



def board2minP( b ):
   
    mc = minecraft.Minecraft.create()
    
    pos = mc.player.getTilePos()
        
    dx = -13
        
    pos = mc.player.getTilePos()

    h = len( b )
    w = len( b[ 0 ] )

    half_w = w / 2
    
    for x in range(0, len( b ) ):

        y3d = pos.y + len( b ) - x

        for y in range(0, len( b[ x ] )):

            z3d = half_w - y

            pi_x = pos.x + dx
            pi_y = pos.y + y3d
            pi_z = pos.z + z3d
            
            mc.setBlocks(
                pi_x, pi_y, pi_z,
                pi_x, pi_y, pi_z,
                cs[ b[x][y] ]
            )
            

            
st = ( board, 3, 4, -1, -1)

def add_bl( b, bl, x, y, v ):
    new_b = copy.deepcopy( b )
    for (dx, dy) in bl:
        nx = x + dx
        ny = y + dy
        if (nx >= 0 and
            nx < len( b ) and
            ny >= 0 and
            ny < len( b[nx] )
        ):
            new_b[nx][ny] = v
    return new_b

bl = [ (0,0), (0, -1), (0, 1) ]
bl_y = w/2


freq = 5
cycle_time = 0.2

while st <> None:


    nb_cycle = 0
    
    while nb_cycle < freq:
        
        start = time.time()

        while (time.time() - start) < cycle_time:
    
            b, x, y, dx, dy = st
            print x, y, dx, dy

            sr.write("?")
            i = sr.readline().split("\r")[0]

            if i <> "":
                bl_y = max(2, min( len(b[0])-3, bl_y + int(i)) )

            b[ -1 ] = [ len(cs) - 1 ] + [ 0 for i in range(0, w-2) ] + [ len(cs) - 1]
            b = add_bl( b, bl, len(b)-1, bl_y, 6)
        
            bb = copy.deepcopy( b )
            bb[x][y] = 1
        
            board2minP( bb )

        nb_cycle += 1
        st = move_ball( b, x, y, dx, dy )



board2minP( b )
    
