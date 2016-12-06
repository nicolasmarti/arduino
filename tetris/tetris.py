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
        

blocks = dict()
blocks[1] = [(0,0),(-1,0),(1,0),(1,1)]
blocks[2] = [(0,0),(-1,0),(1,0),(1,-1)]
blocks[3] = [(0,0),(-1,0),(0,1),(-1,1)]
blocks[4] = [(0,0),(-1,0),(0,-1),(-1,-1)]
blocks[5] = [(0,0),(1,0),(0,1),(1,1)]
blocks[6] = [(0,0),(-1,0),(0,-1),(0,1)]
blocks[7] = [(0,0),(-1,0),(1,0),(2,0)]

#####

def rotate_l( bl ):
    return [ (-y,x) for x, y in bl ]

def rotate_r( bl ):
    return [ (y,-x) for x, y in bl ]

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

def can_fit( b, bl, x, y):
    for (dx, dy) in bl:
        nx = x + dx
        ny = y + dy
        if not (nx >= 0 and
            nx < len( b ) and
            ny >= 0 and
            ny < len( b[nx] ) and
            b[nx][ny] == 0
        ):
         return False 
    return True

def can_go_down( b, bl, x, y):
    for (dx, dy) in bl:
        nx = x + dx
        ny = y + dy
        if (nx == len( b ) - 1 or
            b[nx+1][ny] <> 0
        ):
            return False
    return True
    
def initial_x_y( b, bl ):
    min_dx = min([ dx for (dx, dy) in bl ])
    return (-min_dx, len( b[0]) / 2)

def remove_complete_line( b ):
    i = 0
    while i < len(b):
        if all( b[i] ):
            l = b.pop(i)
            b = [[ 0 for x in range(0, len(l)) ]] + b
        else:
            i += 1
    return b

               
def apply_move_left( b, bl, x, y, v ):
    if can_fit( b, bl, x, y-1):
        return (b, bl, x, y-1, v)
    else:
        return (b, bl, x, y, v)
    
def apply_move_right( b, bl, x, y, v ):
    if can_fit( b, bl, x, y+1):
        return (b, bl, x, y+1, v)
    else:
        return (b, bl, x, y, v)

def apply_rotate_right( b, bl, x, y, v ):
    if can_fit( b, rotate_r(bl), x, y):
        return (b, rotate_r(bl), x, y, v)
    else:
        return (b, bl, x, y, v)
    
def apply_rotate_left( b, bl, x, y, v ):
    if can_fit( b, rotate_l(bl), x, y):
        return (b, rotate_l(bl), x, y, v)
    else:
        return (b, bl, x, y, v)
    
def tick( b, bl, x, y, v ):
    if can_go_down( b, bl, x, y):
        return (b, bl, x+1, y, v)
    else:
        b = add_bl( b, bl, x, y, v)
        b = remove_complete_line( b )
        v = random.randint(1, 7)
        bl = blocks[ v ]
        x, y = initial_x_y( b , bl )
        if not can_fit( b, bl, x, y):
            return None
        else:
            return (b, bl, x, y, v)

def init(h, w):
    b = [ [ 0 for y in range(0, w) ] for x in range(0, h) ]
    v = random.randint(1, 7)
    bl = blocks[ v ]
    x, y = initial_x_y( b , bl )
    return ( b, bl, x, y, v )

######

cs_mcP = [ block.GLASS.id,
           block.GOLD_ORE.id,
           block.IRON_ORE.id,
           block.COAL_ORE.id,
           block.WOOD.id,
           block.GOLD_BLOCK.id,
           block.OBSIDIAN.id,
           block.CHEST.id,
           block.CLAY.id
           ]

mc = minecraft.Minecraft.create()
   
pos = mc.player.getTilePos()

# wipe-out


mc.setBlocks(
    pos.x + 30, pos.y , pos.z + 10,
    pos.x - 30, pos.y + 10, pos.z - 10,
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
                cs_mcP[ b[x][y] ]
            )
            
            

######



######

cs = [ " ", "A", "Q", "O", "M", "G", "D", "H" ]

def board2str( b ):
    result = ""
    result += "-" * (len(b[0]) + 2) + "\n"
    for l in b:
        result += "|" + "".join( [ cs[x] for x in l ] ) + "|" + "\n"
    result += "-" * (len(b[0]) + 2) + "\n"
    return result


######

freq = 5
cycle_time = 0.2

while True:

    pressed = False
    
    st = init( 15, 10 )

    ( b, bl, x, y, v ) = st
    
    board2minP( b )

    # don't understand why
    sr.write("?")
    i = sr.readline().split("\r")[0]

    while not pressed:
        sr.write("?")
        i = sr.readline().split("\r")[0]
        pressed = i <> "none"
    
    board2minP( add_bl( b, bl, x, y, v) )

    while st <> None:

        nb_cycle = 0
        
        while nb_cycle < freq:
            
            start = time.time()

            while (time.time() - start) < cycle_time:
            
                ( b, bl, x, y, v ) = st
                #print "\n" + board2str( add_bl( b, bl, x, y, v) )
        
                board2minP( add_bl( b, bl, x, y, v) )
                
                sr.write("?")
                i = sr.readline().split("\r")[0]
                
                if i == "shiftleft":
                    st = apply_move_left( *st )
                elif i == "shiftright":
                    st = apply_move_right( *st )
                elif i == "rotateright":
                    st = apply_rotate_right( *st )
                elif i == "rotateleft":
                    st = apply_rotate_left( *st )
                elif i == "down":
                    nb_cycle = freq
                    
            nb_cycle += 1
                                
        st = tick( *st )
    
    
        
    
