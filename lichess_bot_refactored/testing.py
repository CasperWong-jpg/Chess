import sys

from ctypes import *

so_file = sys.path[0] + "/ChessEngine.so"
ChessEngine = CDLL(so_file)


ChessEngine.AIMove(b"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")

"""
# NOTE: passing string input and outputs template
ChessEngine.greeting.restype = c_char_p    # func returns bytes literal
res_bytes = ChessEngine.greeting(b"Casper")
res = res_bytes.decode()

print(res)
"""