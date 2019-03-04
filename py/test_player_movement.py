import math
import tkinter

GRID_SIZE = 80
WIDTH = 1920//GRID_SIZE
HEIGHT = 1080//GRID_SIZE

PLAYER_X = 446/GRID_SIZE
PLAYER_Y = 349/GRID_SIZE

def create_line(w, x1, y1, x2, y2, **kwargs):
    return w.create_line(x1*GRID_SIZE, y1*GRID_SIZE, x2*GRID_SIZE, y2*GRID_SIZE, **kwargs)

def circle(w, x, y, r, **kwargs):
    return w.create_oval(x*GRID_SIZE-r, y*GRID_SIZE-r, x*GRID_SIZE+r, y*GRID_SIZE+r, **kwargs)

def main():
    master = tkinter.Tk()
    w = tkinter.Canvas(master, width=WIDTH*GRID_SIZE, height=HEIGHT*GRID_SIZE)
    w.pack()

    for x in range(WIDTH):
        create_line(w, x, 0, x, HEIGHT, fill="black", width=1)
    for y in range(HEIGHT):
        create_line(w, 0, y, WIDTH, y, fill="black", width=1)

    circle(w, PLAYER_X, PLAYER_Y, 5, fill='red')

    facing = 0
    line = None
    next_x_line = None
    next_y_line = None
    next_xy_line = None
    next_yx_line = None
    points = []

    def mouse(event):
        nonlocal line, facing, next_x_line, next_y_line, points, next_xy_line, next_yx_line
        facing = math.atan2(event.y/GRID_SIZE - PLAYER_Y, event.x/GRID_SIZE - PLAYER_X)
        x_dir = math.cos(facing)
        y_dir = math.sin(facing)
        w.delete(line)
        w.delete(next_x_line)
        w.delete(next_y_line)
        w.delete(next_xy_line)
        w.delete(next_yx_line)
        for point in points:
            w.delete(point)

        line = create_line(w, PLAYER_X, PLAYER_Y, x_dir*2000+PLAYER_X, y_dir*2000+PLAYER_Y, width=2)
        next_x_line_pos = math.floor(PLAYER_X) + 0.5 + math.copysign(0.5, x_dir)
        next_y_line_pos = math.floor(PLAYER_Y) + 0.5 + math.copysign(0.5, y_dir)
        x_dist = (next_x_line_pos - PLAYER_X)/x_dir
        y_dist = (next_y_line_pos - PLAYER_Y)/y_dir

        x_col, y_col = ('red', 'yellow') if x_dist > y_dist else ('yellow', 'red')
        next_x_line = create_line(w, next_x_line_pos, 0, next_x_line_pos, HEIGHT, width=3, fill=x_col)
        next_y_line = create_line(w, 0, next_y_line_pos, WIDTH, next_y_line_pos, width=3, fill=y_col)
        nxx, nxy = next_x_line_pos, x_dist * y_dir + PLAYER_Y
        nyx, nyy = y_dist * x_dir + PLAYER_X, next_y_line_pos

        next_xy = math.floor(PLAYER_X + PLAYER_Y) + 0.5 + math.copysign(0.5, x_dir + y_dir)
        next_yx = math.floor(PLAYER_Y - PLAYER_X) + 0.5 + math.copysign(0.5, y_dir - x_dir)

        next_yx_line = create_line(w, -next_yx, 0, WIDTH-next_yx, WIDTH, width=3)
        next_xy_line = create_line(w, next_xy-HEIGHT-1000, HEIGHT+1000, next_xy+1000, -1000, width=3)

        c = PLAYER_Y - (y_dir/x_dir)*PLAYER_X

        nxy_y = (y_dir*next_xy + x_dir * c)/(y_dir+x_dir)
        nxy_x = next_xy - nxy_y
        nyx_y = (y_dir*next_yx - x_dir * c)/(y_dir-x_dir)
        nyx_x = nyx_y - next_yx
        xy_dist = (nxy_x - PLAYER_X)/x_dir
        yx_dist = (nyx_x - PLAYER_X)/x_dir

        points = sorted([(x_dist, nxx, nxy), (y_dist, nyx, nyy), (xy_dist, nxy_x, nxy_y), (yx_dist, nyx_x, nyx_y)])
        colours = ['red', 'yellow', 'green', 'blue']
        sizes = [4, 3, 2, 1]

        for (dist, x, y), colour, size in zip(points, colours, sizes):
            points.append(circle(w, x, y, size, fill=colour))

    w.bind("<Motion>", mouse)

    tkinter.mainloop()

if __name__ == "__main__":
    main()
