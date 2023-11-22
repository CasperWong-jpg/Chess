import sys

import ctypes

so_file = sys.path[0] + "/bin/ChessEngine.so"
ChessEngine = ctypes.CDLL(so_file)
ChessEngine.lichess.restype = ctypes.c_char_p

res = ""
res = ChessEngine.lichess(b"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", res)
print(res)