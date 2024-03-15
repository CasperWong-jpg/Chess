"""
Some example strategies for people who want to create a custom, homemade bot.
And some handy classes to extend
"""

import chess
from chess.engine import PlayResult
import random
from engine_wrapper import EngineWrapper
import ctypes
import sys

class FillerEngine:
    """
    Not meant to be an actual engine.

    This is only used to provide the property "self.engine"
    in "MinimalEngine" which extends "EngineWrapper"
    """
    def __init__(self, main_engine, name=None):
        self.id = {
            "name": name
        }
        self.name = name
        self.main_engine = main_engine

    def __getattr__(self, method_name):
        main_engine = self.main_engine

        def method(*args, **kwargs):
            nonlocal main_engine
            nonlocal method_name
            return main_engine.notify(method_name, *args, **kwargs)

        return method


class MinimalEngine(EngineWrapper):
    """
    Subclass this to prevent a few random errors

    Even though MinimalEngine extends EngineWrapper,
    you don't have to actually wrap an engine.

    At minimum, just implement `search`,
    however you can also change other methods like
    `notify`, `first_search`, `get_time_control`, etc.
    """
    def __init__(self, *args, name=None):
        super().__init__(*args)

        self.engine_name = self.__class__.__name__ if name is None else name

        self.last_move_info = []
        self.engine = FillerEngine(self, name=self.name)
        self.engine.id = {
            "name": self.engine_name
        }

    def search_with_ponder(self, board, wtime, btime, winc, binc, ponder, draw_offered):
        timeleft = 0
        if board.turn:
            timeleft = wtime
        else:
            timeleft = btime
        return self.search(board, timeleft, ponder, draw_offered)

    def search(self, board, timeleft, ponder, draw_offered):
        """
        The method to be implemented in your homemade engine

        NOTE: This method must return an instance of "chess.engine.PlayResult"
        """
        raise NotImplementedError("The search method is not implemented")

    def notify(self, method_name, *args, **kwargs):
        """
        The EngineWrapper class sometimes calls methods on "self.engine".
        "self.engine" is a filler property that notifies <self> 
        whenever an attribute is called.

        Nothing happens unless the main engine does something.

        Simply put, the following code is equivalent
        self.engine.<method_name>(<*args>, <**kwargs>)
        self.notify(<method_name>, <*args>, <**kwargs>)
        """
        pass


class ExampleEngine(MinimalEngine):
    pass

class C_Engine(ExampleEngine):
    """C engine: uses minimax with depth 4"""

    def search(self, board, *args):
        so_file = sys.path[0] + "/engines/ChessEngine.so"
        ChessEngine = ctypes.CDLL(so_file)
        ChessEngine.lichess.restype = ctypes.c_char_p
        print(f"Input string is: {board.fen()}")

        UCI_move = ChessEngine.lichess(bytes(board.fen(), 'ascii'), "")
        UCI_move = UCI_move.decode()
        print(f"Move: {UCI_move}")
        return PlayResult(UCI_move, None)

# Strategy names and ideas from tom7's excellent eloWorld video
class RandomMove(ExampleEngine):
    def search(self, board, *args):
        # print(board.fen())
        move = random.choice(list(board.legal_moves))
        # print(move)
        return PlayResult(move, None)


class Alphabetical(ExampleEngine):
    def search(self, board, *args):
        moves = list(board.legal_moves)
        moves.sort(key=board.san)
        return PlayResult(moves[0], None)


class FirstMove(ExampleEngine):
    """Gets the first move when sorted by uci representation"""
    def search(self, board, *args):
        moves = list(board.legal_moves)
        moves.sort(key=str)
        return PlayResult(moves[0], None)