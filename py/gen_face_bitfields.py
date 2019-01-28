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

TOP_RIGHT = [False, True, False, False, False]
TOP_LEFT = [False, False, True, False, False]
BOTTOM_LEFT = [False, False, False, True, False]
BOTTOM_RIGHT = [False, False, False, False, True]

def air():
    return [False] * 30

def cube():
    return [True] * 30


def ground_slopes():
    blocks = {}
    xz_directions = [SideOrder(i) for i in range(4)] * 2
    for direction in range(4):
        blocks[direction] = {SideOrder.POS_Y: EMPTY_SIDE,
                             SideOrder.NEG_Y: FULL_SIDE,
                             xz_directions[direction    ]: EMPTY_SIDE,
                             xz_directions[direction + 1]: BOTTOM_RIGHT,
                             xz_directions[direction + 2]: FULL_SIDE, 
                             xz_directions[direction + 3]: BOTTOM_LEFT}
    return [blocks[direction][side][i] for direction in range(4) for side in SideOrder for i in range(5)]


def ceiling_slopes():
    blocks = {}
    xz_directions = [SideOrder(i) for i in range(4)] * 2
    for direction in range(4):
        blocks[direction] = {SideOrder.NEG_Y: EMPTY_SIDE,
                             SideOrder.POS_Y: FULL_SIDE,
                             xz_directions[direction    ]: EMPTY_SIDE,
                             xz_directions[direction + 1]: TOP_RIGHT,
                             xz_directions[direction + 2]: FULL_SIDE, 
                             xz_directions[direction + 3]: TOP_LEFT}
    return [blocks[direction][side][i] for direction in range(4) for side in SideOrder for i in range(5)]


class CornerOrder(Enum):
    POS_NEG = 0
    NEG_NEG = 1
    NEG_POS = 2
    POS_POS = 3


def ground_corner_slopes():
    blocks = {}
    xz_directions = [SideOrder(i) for i in range(4)] * 2
    for direction in range(4):
        bottom = [False] * 5
        bottom[(3 - direction) + 1] = True
        blocks[direction] = {SideOrder.POS_Y: EMPTY_SIDE,
                             SideOrder.NEG_Y: bottom,
                             xz_directions[direction    ]: EMPTY_SIDE,
                             xz_directions[direction + 1]: BOTTOM_RIGHT,
                             xz_directions[direction + 2]: BOTTOM_LEFT, 
                             xz_directions[direction + 3]: EMPTY_SIDE}
    return [blocks[direction][side][i] for direction in range(4) for side in SideOrder for i in range(5)]


def ceiling_corner_slopes():
    blocks = {}
    xz_directions = [SideOrder(i) for i in range(4)] * 2
    for direction in range(4):
        top = [False] * 5
        top[direction + 1] = True
        blocks[direction] = {SideOrder.POS_Y: top,
                             SideOrder.NEG_Y: EMPTY_SIDE,
                             xz_directions[direction    ]: EMPTY_SIDE,
                             xz_directions[direction + 1]: TOP_RIGHT,
                             xz_directions[direction + 2]: TOP_LEFT, 
                             xz_directions[direction + 3]: EMPTY_SIDE}
    return [blocks[direction][side][i] for direction in range(4) for side in SideOrder for i in range(5)]


def diagonals():
    blocks = {}
    xz_directions = [SideOrder(i) for i in range(4)] * 2
    for direction in range(4):
        bottom = [False] * 5
        bottom[(3- direction) + 1] = True
        top = [False] * 5
        top[direction + 1] = True
        blocks[direction] = {SideOrder.POS_Y: top,
                             SideOrder.NEG_Y: bottom,
                             xz_directions[direction    ]: EMPTY_SIDE,
                             xz_directions[direction + 1]: FULL_SIDE,
                             xz_directions[direction + 2]: FULL_SIDE,
                             xz_directions[direction + 3]: EMPTY_SIDE}
    return [blocks[direction][side][i] for direction in range(4) for side in SideOrder for i in range(5)]


all_bits = air() + cube() + ground_slopes() + ceiling_slopes() + ground_corner_slopes() + ceiling_corner_slopes() + diagonals()
print('bool BLOCK_SIDE_OCCLUSION_BITFIELD[] = {' + ', '.join('true' if bit else 'false' for bit in all_bits) + '};')
print("Expected " + str(22 * 6 * 5))
print(len(all_bits))

