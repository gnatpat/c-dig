# 18 block types.
# 6 sides.
# 5 block faces. Which block covers which? 
# Array of 540 bools, indexed by block_type * 30 + side * 5 + face_type. True if the block obscures that type. 
# face direction 
#    - for side faces, face side with pos Y up. it's then top right, anticlockwise
#    - for Y faces, face pos Z face. rotate so you're facing face. then top right, anticlockwise
# side order
# pos z, pos x, neg z, neg x, pos y, neg y

from enum import Enum

class SideOrder(Enum):
    POS_Z = 0
    POS_X = 1
    NEG_Z = 2
    NEG_X = 3
    POS_Y = 4 
    NEG_Y = 5

FULL_SIDE = [True] * 5
EMPTY_SIDE = [False] * 5

def air():
    return [False] * 30

def cube():
    return [True] * 30

def up_slopes():
    blocks = {}
    xz_directions = [SideOrder(i) for i in range(4)] * 2
    print(xz_directions)
    for direction in range(4):
        blocks[direction] = {SideOrder.NEG_Y: EMPTY_SIDE,
                             SideOrder.POS_Y: FULL_SIDE,
                             xz_directions[direction    ]: EMPTY_SIDE,
                             xz_directions[direction + 1]: [False, True, False, False, False],
                             xz_directions[direction + 2]: FULL_SIDE, 
                             xz_directions[direction + 3]: [False, False, True, False, False]}
    return [blocks[direction][side][i] for direction in range(4) for side in SideOrder for i in range(5)]

def down_slopes():
    blocks = {}
    xz_directions = [SideOrder(i) for i in range(4)] * 2
    for direction in range(4):
        blocks[direction] = {SideOrder.POS_Y: EMPTY_SIDE,
                             SideOrder.NEG_Y: FULL_SIDE,
                             xz_directions[direction    ]: EMPTY_SIDE,
                             xz_directions[direction + 1]: [False, False, False, False, True],
                             xz_directions[direction + 2]: FULL_SIDE, 
                             xz_directions[direction + 3]: [False, False, False, True, False]}
    return [blocks[direction][side][i] for direction in range(4) for side in SideOrder for i in range(5)]

all_bits = air() + cube() + down_slopes() + up_slopes()
print(all_bits)
print("Expected 10 * 6 * 5 = 300")
print(len(all_bits))
print(all_bits[5 * 30 + 0 * 5 + 4])

