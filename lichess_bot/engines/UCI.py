import sys
import chess
import argparse
from simple_main import *

# This code is largely adapted from https://github.com/healeycodes/andoma
# Big thanks to andoma for his UCI guide :) 

def talk():
    """
    The main input/output loop.
    This implements a slice of the UCI protocol.
    """
    board = chess.Board()
    depth = get_depth()

    while True:
        msg = input()
        command(depth, board, msg)


def command(depth: int, board: chess.Board, msg: str):
    """
    Accept UCI commands and respond.
    The board state is also updated.
    """

    bool2color = {True: 'w', False: 'b'}

    msg = msg.strip()
    tokens = msg.split(" ")
    while "" in tokens:
        tokens.remove("")

    if msg == "quit":
        print("Goodbye!")
        sys.exit()

    if msg == "uci":
        print("id name casperw")  # Andrew/Roma -> And/oma
        print("id author Casper Wong, Carnegie Mellon University")
        print("uciok")
        return

    if msg == "isready":
        print("readyok")
        return

    if msg == "ucinewgame":
        return

    if msg.startswith("position"):
        if len(tokens) < 2:
            return

        # Set starting position
        if tokens[1] == "startpos":
            board.reset()
            moves_start = 2
        elif tokens[1] == "fen":
            fen = " ".join(tokens[2:8])
            board.set_fen(fen)
            moves_start = 8
        else:
            return

        # Apply moves
        if len(tokens) <= moves_start or tokens[moves_start] != "moves":
            return

        for move in tokens[(moves_start+1):]:
            board.push_uci(move)

    if msg == "d":
        # Non-standard command, but supported by Stockfish and helps debugging
        print(board)
        print(board.fen())

    if msg[0:2] == "go":
        print(f"Original board fen: {board.fen()}")
        board_engineFormat, color = makeBoardFromFen(board.fen())
        engine_move = ChessEngine.AIMove(board_engineFormat, color, depth, True)
        UCI_move = translate_engine_to_UCI(board_engineFormat, *engine_move)
        print(f"bestmove {UCI_move}")
        return


def get_depth() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--depth", default=3, help="provide an integer (default: 3)")
    args = parser.parse_args()
    return max([1, int(args.depth)])
