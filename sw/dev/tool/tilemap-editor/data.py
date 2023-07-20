from typing import NamedTuple


class Pos(NamedTuple):
    x: int
    y: int


class Size(NamedTuple):
    w: int
    h: int


class Rect():
    def __init__(self, *args):
        assert(len(args) in (0, 4))
        self.x: int = args[0] if len(args) else 0
        self.y: int = args[1] if len(args) else 0
        self.w: int = args[2] if len(args) else 0
        self.h: int = args[3] if len(args) else 0

    @property
    def pos(self):
        return Pos(self.x, self.y)

    @property
    def size(self):
        return Size(self.w, self.h)


class Tile():
    def __init__(self):
        self.image_path: str = str()
        self.rect: Rect = Rect()
        self.collider: Rect = Rect()


class TileSet():
    MODE_GRID: int = 0
    MODE_FREE: int = 1

    def __init__(self):
        self.images = list()
        self.tiles = list()
        self.mode: int = TileSet.MODE_GRID

    @property
    def tilesize(self):
        if len(self.tiles) == 0:
            return (0, 0)
        if self.mode == Tile.MODE_GRID:
            return self.tiles[0].rect.size
        else:
            ws = [t.rect.w for t in self.tiles]
            hs = [t.rect.h for t in self.tiles]
            size = Size(max(ws), max(hs))
            return size


class TileMap():
    def __init__(self):
        self.tilesets = list()
        self.map = list()  # 2-dim array of tileIdx


class TileMapEditor():
    def __init__(self):
        self.tilemap = None
