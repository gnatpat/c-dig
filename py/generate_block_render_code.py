import datetime
import subprocess

from typing import NamedTuple, List, Tuple
from enum import IntEnum, Enum

class DirectionOrder(IntEnum):
    POS_Z = 0
    POS_X = 1
    NEG_Z = 2
    NEG_X = 3
    POS_Y = 4 
    NEG_Y = 5

OFFSET_DIRECTION = [
        "x, y, z+1",
        "x+1, y, z",
        "x, y, z-1",
        "x-1, y, z",
        "x, y+1, z",
        "x, y-1, z"]

NORMALS = [
        "v3(0, 0, 1)",
        "v3(1, 0, 0)",
        "v3(0, 0, -1)",
        "v3(-1, 0, 0)",
        "v3(0, 1, 0)",
        "v3(0, -1, 0)"]

OPPOSITE = [
        DirectionOrder.NEG_Z,
        DirectionOrder.NEG_X,
        DirectionOrder.POS_Z,
        DirectionOrder.POS_X,
        DirectionOrder.NEG_Y,
        DirectionOrder.POS_Y]

# Maybe this should be an enum, but it means doing Side.FULL_SIDE everywhere and bleh - although it's very hacky now
FULL_SIDE = 0
TOP_RIGHT = 1
TOP_LEFT = 2
BOTTOM_LEFT = 3
BOTTOM_RIGHT = 4
EMPTY_SIDE = 5
SIDE_TYPES = list(range(6))


class SideType(NamedTuple):
    name: str
    occlusion: List[bool]


class Slope(NamedTuple):
    vertices: str
    normal: Tuple[float]


SIDE_TYPE_DATA = [
    SideType("FULL", [True] * 5),
    SideType("TOP_RIGHT", [False, True, False, False, False]),
    SideType("TOP_LEFT", [False, False, True, False, False]),
    SideType("BOTTOM_LEFT", [False, False, False, True, False]),
    SideType("BOTTOM_RIGHT", [False, False, False, False, True]),
    SideType("EMPTY", [False] * 5),
]


CORNER_ORDER = [TOP_RIGHT, TOP_LEFT, BOTTOM_LEFT, BOTTOM_RIGHT] * 2
REVERSED_CORNER_ORDER = [TOP_RIGHT, BOTTOM_RIGHT, BOTTOM_LEFT, TOP_LEFT] * 2


BLOCK_SHAPE_NAMES = ["AIR",
                     "CUBE",
                     "POS_Z_POS_Y_SLOPE",
                     "POS_X_POS_Y_SLOPE",
                     "NEG_Z_POS_Y_SLOPE",
                     "NEG_X_POS_Y_SLOPE",
                     "POS_Z_NEG_Y_SLOPE",
                     "POS_X_NEG_Y_SLOPE",
                     "NEG_Z_NEG_Y_SLOPE",
                     "NEG_X_NEG_Y_SLOPE",
                     "POS_NEG_NEG_CORNER",
                     "NEG_NEG_NEG_CORNER",
                     "NEG_NEG_POS_CORNER",
                     "POS_NEG_POS_CORNER",
                     "POS_POS_NEG_CORNER",
                     "NEG_POS_NEG_CORNER",
                     "NEG_POS_POS_CORNER",
                     "POS_POS_POS_CORNER",
                     "POS_POS_NEG_CORNERLESS",
                     "NEG_POS_NEG_CORNERLESS",
                     "NEG_POS_POS_CORNERLESS",
                     "POS_POS_POS_CORNERLESS",
                     "POS_NEG_NEG_CORNERLESS",
                     "NEG_NEG_NEG_CORNERLESS",
                     "NEG_NEG_POS_CORNERLESS",
                     "POS_NEG_POS_CORNERLESS",
                     "POS_NEG_DIAGONAL",
                     "NEG_NEG_DIAGONAL",
                     "NEG_POS_DIAGONAL",
                     "POS_POS_DIAGONAL"]

FACE_VERTICES = [["xyZ", "XyZ", "XYZ", "xYZ"],
                 ["XyZ", "Xyz", "XYz", "XYZ"],
                 ["Xyz", "xyz", "xYz", "XYz"],
                 ["xyz", "xyZ", "xYZ", "xYz"],
                 ["xYZ", "XYZ", "XYz", "xYz"],
                 ["xyz", "Xyz", "XyZ", "xyZ"]]

COLOURS = ["BETH", "BETH", "BETH", "BETH", "GREEN", "BETH"]


def swap_xz(side):
    if side == TOP_RIGHT:
        return TOP_LEFT
    if side == TOP_LEFT:
        return TOP_RIGHT
    if side == BOTTOM_LEFT:
        return BOTTOM_RIGHT
    if side == BOTTOM_RIGHT:
        return BOTTOM_LEFT
    return side


def swap_y(side):
    if side == TOP_RIGHT:
        return BOTTOM_RIGHT
    if side == TOP_LEFT:
        return BOTTOM_LEFT
    if side == BOTTOM_LEFT:
        return TOP_LEFT
    if side == BOTTOM_RIGHT:
        return TOP_RIGHT
    return side


def rotate_vertex_string_around_xz(vertex, rotations):
    if rotations == 0:
        return vertex
    x_pos = vertex[0] == 'X'
    z_pos = vertex[2] == 'Z'
    return rotate_vertex_string_around_xz(("X" if z_pos else "x") + vertex[1] + ("z" if x_pos else "Z"), rotations-1)


def rotate_direction_around_xz(direction, rotations):
    if rotations == 0:
        return direction
    return rotate_direction_around_xz((direction[2], direction[1], -direction[0]), rotations-1)


def all_four_slopes(vertices, normal):
    for direction in range(4):
        yield Slope(list(map(lambda v: rotate_vertex_string_around_xz(v, direction), vertices)), rotate_direction_around_xz(normal, direction))

def direction_to_c_string(direction):
    return f"v3({', '.join(str(d) for d in direction)})"


def air():
    return [EMPTY_SIDE] * 6

def cube():
    return [FULL_SIDE] * 6


def ground_slopes():
    blocks = {}
    xz_directions = [DirectionOrder(i) for i in range(4)] * 2
    for direction in range(4):
        blocks[direction] = {DirectionOrder.POS_Y: EMPTY_SIDE,
                             DirectionOrder.NEG_Y: FULL_SIDE,
                             xz_directions[direction    ]: EMPTY_SIDE,
                             xz_directions[direction + 1]: BOTTOM_RIGHT,
                             xz_directions[direction + 2]: FULL_SIDE, 
                             xz_directions[direction + 3]: BOTTOM_LEFT}
    return [blocks[direction][side] for direction in range(4) for side in DirectionOrder]


def ceiling_slopes():
    blocks = {}
    xz_directions = [DirectionOrder(i) for i in range(4)] * 2
    for direction in range(4):
        blocks[direction] = {DirectionOrder.NEG_Y: EMPTY_SIDE,
                             DirectionOrder.POS_Y: FULL_SIDE,
                             xz_directions[direction    ]: EMPTY_SIDE,
                             xz_directions[direction + 1]: TOP_RIGHT,
                             xz_directions[direction + 2]: FULL_SIDE, 
                             xz_directions[direction + 3]: TOP_LEFT}
    return [blocks[direction][side] for direction in range(4) for side in DirectionOrder]


class CornerOrder(Enum):
    POS_NEG = 0
    NEG_NEG = 1
    NEG_POS = 2
    POS_POS = 3


def ground_corner_slopes():
    blocks = {}
    xz_directions = [DirectionOrder(i) for i in range(4)] * 2
    for direction in range(4):
        bottom = REVERSED_CORNER_ORDER[direction+1]
        blocks[direction] = {DirectionOrder.POS_Y: EMPTY_SIDE,
                             DirectionOrder.NEG_Y: bottom,
                             xz_directions[direction    ]: EMPTY_SIDE,
                             xz_directions[direction + 1]: BOTTOM_RIGHT,
                             xz_directions[direction + 2]: BOTTOM_LEFT, 
                             xz_directions[direction + 3]: EMPTY_SIDE}
    return [blocks[direction][side] for direction in range(4) for side in DirectionOrder]


def ceiling_corner_slopes():
    blocks = {}
    xz_directions = [DirectionOrder(i) for i in range(4)] * 2
    for direction in range(4):
        top = [False] * 5
        top = CORNER_ORDER[direction]
        blocks[direction] = {DirectionOrder.POS_Y: top,
                             DirectionOrder.NEG_Y: EMPTY_SIDE,
                             xz_directions[direction    ]: EMPTY_SIDE,
                             xz_directions[direction + 1]: TOP_RIGHT,
                             xz_directions[direction + 2]: TOP_LEFT, 
                             xz_directions[direction + 3]: EMPTY_SIDE}
    return [blocks[direction][side] for direction in range(4) for side in DirectionOrder]


def ground_cornerless():
    blocks = {}
    xz_directions = [DirectionOrder(i) for i in range(4)] * 2
    for direction in range(4):
        blocks[direction] = {DirectionOrder.POS_Y: CORNER_ORDER[direction + 2],
                             DirectionOrder.NEG_Y: FULL_SIDE,
                             xz_directions[direction    ]: FULL_SIDE,
                             xz_directions[direction + 1]: BOTTOM_LEFT,
                             xz_directions[direction + 2]: BOTTOM_RIGHT,
                             xz_directions[direction + 3]: FULL_SIDE}
    return [blocks[direction][side] for direction in range(4) for side in DirectionOrder]


def ceiling_cornerless():
    blocks = {}
    xz_directions = [DirectionOrder(i) for i in range(4)] * 2
    for direction in range(4):
        blocks[direction] = {DirectionOrder.POS_Y: FULL_SIDE,
                             DirectionOrder.NEG_Y: REVERSED_CORNER_ORDER[direction + 3],
                             xz_directions[direction    ]: FULL_SIDE,
                             xz_directions[direction + 1]: TOP_LEFT,
                             xz_directions[direction + 2]: TOP_RIGHT,
                             xz_directions[direction + 3]: FULL_SIDE}
    return [blocks[direction][side] for direction in range(4) for side in DirectionOrder]


def diagonals():
    blocks = {}
    xz_directions = [DirectionOrder(i) for i in range(4)] * 2
    for direction in range(4):
        bottom = REVERSED_CORNER_ORDER[direction+1]
        top = CORNER_ORDER[direction]
        blocks[direction] = {DirectionOrder.POS_Y: top,
                             DirectionOrder.NEG_Y: bottom,
                             xz_directions[direction    ]: EMPTY_SIDE,
                             xz_directions[direction + 1]: FULL_SIDE,
                             xz_directions[direction + 2]: FULL_SIDE,
                             xz_directions[direction + 3]: EMPTY_SIDE}
    return [blocks[direction][side] for direction in range(4) for side in DirectionOrder]


def chunk(l, n):
    for i in range(0, len(l), n):
        yield l[i:i + n]

def main():
    side_data = air() + cube() + ground_slopes() + ceiling_slopes() + ground_corner_slopes() + ceiling_corner_slopes() + ground_cornerless() + ceiling_cornerless() + diagonals() 
    slopes = ([None, None] +
            # Ground slopes
            list(all_four_slopes(["xyZ", "XyZ", "XYz", "xYz"], (0, 1, 1))) +
            # Ceiling slopes
            list(all_four_slopes(["Xyz", "XYZ", "xYZ", "xyz"], (0, -1, 1))) +
            # Ground corner slopes
            list(all_four_slopes(["XYz", "xyz", "XyZ"], (-1, 1, 1))) +
            # Ceiling corner slopes
            list(all_four_slopes(["Xyz", "XYZ", "xYz"], (-1, -1, 1))) +
            # ground cornerless
            list(all_four_slopes(["XYZ", "Xyz", "xYz"], (1, 1, -1))) +
            # ceiling cornerless
            list(all_four_slopes(["XYz", "XyZ", "xyz"], (1, -1, -1))) +
            # diagonals
            list(all_four_slopes(["XyZ", "XYZ", "xYz", "xyz"], (-1, 0, 1))))

    new_bits = []
    for i, side in enumerate(side_data):
        if(i % 6 < 4):
            new_bits.extend(SIDE_TYPE_DATA[swap_xz(side)].occlusion)
        else:
            new_bits.extend(SIDE_TYPE_DATA[swap_y(side)].occlusion)

    render_order = {side: {side_type: [] for side_type in SIDE_TYPES} for side in DirectionOrder}
    for shape_name, sides_data in zip(BLOCK_SHAPE_NAMES, chunk(side_data, 6)):
        for direction, side_type in zip(DirectionOrder, sides_data):
            render_order[direction][side_type].append(shape_name)

    with open('src/generated/block_render.cpp', 'w') as f:
        f.write(f"// Generated by generate_block_render_code.py on {datetime.datetime.now().strftime('%I:%M%p, %B %d, %Y') }\n\n")
        f.write('bool BLOCK_SIDE_OCCLUSION_BITFIELD[] = {' + ', '.join('true' if bit else 'false' for bit in new_bits) + '};\n\n\n')
        f.write("""
                inline bool* getBlockSideOcclusion(BlockShape block_shape, Direction side) {
                  return BLOCK_SIDE_OCCLUSION_BITFIELD + (block_shape * 30 + side * 5);
                }\n\n
                """);
        f.write("void pushBlockVertices(ChunkVertex** vertex_cursor, Chunk* chunk, LoadedWorld* world, int x, int y, int z, BlockShape block_shape) {\n")
        f.write("bool* block_side_occulusion;\n")
        f.write("V3 normal;\n");
        f.write("""
                float fx = x;
                float fy = y;
                float fz = z;
                V3 xyz = { fx    , fy    , fz    };
                V3 Xyz = { fx + 1, fy    , fz    };
                V3 xYz = { fx    , fy + 1, fz    };
                V3 xyZ = { fx    , fy    , fz + 1};
                V3 xYZ = { fx    , fy + 1, fz + 1};
                V3 XyZ = { fx + 1, fy    , fz + 1};
                V3 XYz = { fx + 1, fy + 1, fz    };
                V3 XYZ = { fx + 1, fy + 1, fz + 1};\n\n
                """)
        for direction in DirectionOrder:
            f.write(f"\n// {direction.name}\n")
            f.write(f"block_side_occulusion = getBlockSideOcclusion(getBlockShapeAt(world, chunk, {OFFSET_DIRECTION[direction]}), {OPPOSITE[direction].name});\n")
            f.write(f"normal = {NORMALS[direction]};\n")
            f.write("switch(block_shape) {\n")
            for side_type in SIDE_TYPES:
                block_types = render_order[direction][side_type]
                for block_type in block_types:
                    f.write(f"case {block_type}:\n")
                if side_type == EMPTY_SIDE:
                    f.write("case BLOCK_SHAPE_COUNT:\n")
                    f.write("break;\n")
                    continue

                f.write(f"if (block_side_occulusion[{SIDE_TYPE_DATA[side_type].name}]) {{\nbreak;\n}}\n")
                if side_type == FULL_SIDE:
                    face_vertices = FACE_VERTICES[int(direction)]
                    push_function = 'pushSquare'
                else:
                    face_vertices = [v for v, b in zip(FACE_VERTICES[direction], SIDE_TYPE_DATA[side_type].occlusion[1:]) if not b]
                    push_function = 'pushTriangle'
                f.write(f"{push_function}(vertex_cursor, {', '.join(face_vertices)}, {COLOURS[direction]}, normal);\n")
                f.write("break;\n\n")
            f.write("}\n")
        f.write("\n\n//SLOPES\n");
        f.write("switch(block_shape) {\n")
        for block_shape, slope in zip(BLOCK_SHAPE_NAMES, slopes):
            f.write(f"case {block_shape}:\n")
            if slope:
                if len(slope.vertices) == 3:
                    push_function = 'pushTriangle'
                else:
                    push_function = 'pushSquare'
                colour = 'BETH'
                f.write(f"{push_function}(vertex_cursor, {', '.join(slope.vertices)}, {colour}, {direction_to_c_string(slope.normal)});\n")
            f.write("break;\n")
        f.write("case BLOCK_SHAPE_COUNT:\nbreak;\n")
        f.write("}\n")
        f.write("}\n")

    subprocess.run(['clang-format', '-i', 'src/generated/block_render.cpp'])


if __name__ == "__main__":
    main()
