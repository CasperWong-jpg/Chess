# This is the main file to run the game. It uses a command line interface

import random
import time

# Below file contains functions + attributes that reinforces chess rules, as well as chess AI
from ChessEngine import *

def makeBoard():
    # empty board
    rows = cols = 8
    board = [([None] * cols) for row in range(rows)]

    # rook, knight, bishop, queen, king
    board[0][0], board[0][cols-1] = Rook('b'), Rook('b')
    board[0][1], board[0][cols-2] = Knight('b'), Knight('b')
    board[0][2], board[0][cols-3] = Bishop('b'), Bishop('b')
    board[0][3], board[0][4] = Queen('b'), King('b')
    board[rows-1][0], board[rows-1][cols-1] = Rook('w'), Rook('w')
    board[rows-1][1], board[rows-1][cols-2] = Knight('w'), Knight('w')
    board[rows-1][2], board[rows-1][cols-3] = Bishop('w'), Bishop('w')
    board[rows-1][3], board[rows-1][4] = Queen('w'), King('w')

    # pawns
    for col in range(cols):
        board[1][col] = Pawn('b')
        board[rows-2][col] = Pawn('w')
    
    return board

def getRow(index):
    return index // 8

def getCol(index):
    return index % 8

def makeBoardFromFen(fen: str):
    # empty board
    rows = cols = 8
    board = [([None] * cols) for row in range(rows)]
    fen_to_Pieces = {'r': Rook('b'), 'n': Knight('b'), 'b': Bishop('b'), 'q': Queen('b'), 'k': King('b'), 'p': Pawn('b'),
                     'R': Rook('w'), 'N': Knight('w'), 'B': Bishop('w'), 'Q': Queen('w'), 'K': King('w'), 'P': Pawn('w'),}
    fen = fen.split(' ')

    board_index = 0
    for fen_char in fen[0]:
        if fen_char == "/": continue
        elif fen_char.isdigit():
            board_index += int(fen_char)    # Board already None type here
        else:
            board[getRow(board_index)][getCol(board_index)] = fen_to_Pieces[fen_char]
            if fen_char in "kK":  # king piece – just make AI not castle for now
                board[getRow(board_index)][getCol(board_index)].hasMoved = True
            elif fen_char == "p" and getRow(board_index) != 1:
                board[getRow(board_index)][getCol(board_index)].hasMoved = True
            elif fen_char == "P" and getRow(board_index) != 6:
                print("White pawn moved")
                board[getRow(board_index)][getCol(board_index)].hasMoved = True
            board_index += 1
    assert(board_index == 64)    # Should have gone through whole board here
            
    return (board, fen[1])

def render(board):
    rows, cols = len(board), len(board[0])
    result = "\n"
    uni_pieces = {
        Rook('b'): "♖", Knight('b'): "♘", Bishop('b'): "♗", Queen('b'): "♕", King('b'): "♔", Pawn('b'): "♙",
        Rook('w'): "♜", Knight('w'): "♞", Bishop('w'): "♝", Queen('w'): "♛", King('w'): "♚", Pawn('w'): "♟",
        None: "·",
    }
    for row in range(rows):
        result += f"{8-row} "
        for col in range(cols):
            result += f"{uni_pieces[board[row][col]]} "
        result += "\n"
    result += "  a b c d e f g h\n"
    return result

def translate_engine_to_UCI(board, row, col, newRow, newCol):
    # Precondition: engine move is always valid
    dict_engine_to_UCI = {0:'a', 1:'b', 2:'c', 3:'d', 4:'e', 5:'f', 6:'g', 7:'h'}

    UCI_col = dict_engine_to_UCI[col]
    UCI_row = str(8 - row)
    UCI_newCol = dict_engine_to_UCI[newCol]
    UCI_newRow = str(8 - newRow)

    if isinstance(board[row][col], Pawn) and (newRow == 0 or newRow == 7):
        return UCI_col + UCI_row + UCI_newCol + UCI_newRow + 'q'  # Promotion
    return UCI_col + UCI_row + UCI_newCol + UCI_newRow

def translate_UCI_to_engine(board, move):
    # UCI move may not be valid (since it is an user input)

    if not (4 <= len(move) <= 5):
        return None

    dict_UCI_to_engine = {'a':0, 'b':1, 'c':2, 'd':3, 'e':4, 'f':5, 'g':6, 'h':7}
    UCI_col, UCI_row, UCI_newCol, UCI_newRow = move[0], move[1], move[2], move[3]

    try:
        row, col = 8 - int(UCI_row), dict_UCI_to_engine[UCI_col]
        newRow, newCol = 8 - int(UCI_newRow), dict_UCI_to_engine[UCI_newCol]
        if isinstance(board[row][col], Pawn) and (newRow == 0 or newRow == 7):
            promotePiece = move[4]
        else:
            promotePiece = None
    except:
        return None
    
    if (0 <= row < 8) and (0 <= newRow < 8) and (promotePiece in {'q', 'r', 'b', 'n', None}):
        return (row, col, newRow, newCol, promotePiece)
    else: 
        return None

def movePiece(board, previousRow, previousCol, row, col):
        board[row][col] = board[previousRow][previousCol]
        board[previousRow][previousCol] = None

def start():
    # Start the command line user interface & initialize game state
    board = makeBoard()
    whiteToMove = True
    bool2color = {True: 'w', False: 'b'}
    abbr = {'w': "white", 'b': "black"}
    colorToMove = bool2color[whiteToMove]
    while True:
        playerColor = input("Start as [w]hite or [b]lack:\n")
        if playerColor == 'b' or playerColor == 'w': break

    # Ask for move input & make AI move
    gameOver = None
    while gameOver == None:
        print(render(board))
        if playerColor == colorToMove:
            # Obtain player input and make move
            possibleMoves = ChessEngine.generateMoves(board, playerColor)
            possibleMove = translate_engine_to_UCI(board, *random.choice(possibleMoves))
            
            while True:
                UCI_move = input(f"{abbr[colorToMove]} move (ie. {possibleMove}): \n")

                ###### DEBUGGING STUFF
                if UCI_move == "debug1":
                    board = boardSet1()
                    print(render(board))
                    continue
                elif UCI_move == "debug2":
                    board = boardSet2()
                    print(render(board))
                    continue
                ###### DEBUGGING STUFF

                engine_move = translate_UCI_to_engine(board, UCI_move)
                if (engine_move != None and 
                    ChessEngine.tryMove(board, *engine_move[0:4])): break
                print("Invalid move, try again \n")
            movePiece(board, *engine_move[0:4])
            ChessEngine.specialRules(board, *engine_move[2:4], False, engine_move[4])
        else:
            print("AI is thinking... \n")
            ChessEngine.AIMove(board, colorToMove, 3)  # Depth of three for now

        gameOver = ChessEngine.checkGameState(board, whiteToMove)
        whiteToMove = not whiteToMove
        colorToMove = bool2color[whiteToMove]
    
    print(render(board))
    print(f"Game ended: {gameOver}")
    return

##### DEBUGGING STUFF
def boardSet1():
    board = makeBoard()
    board[1][6], board[3][6] = None, Pawn('b')
    board[6][5], board[4][5] = None, Pawn('w')
    board[1][5] = board[6][4] = None
    board[2][5], board[4][4] = Pawn('b'), Pawn('w')
    return board

def boardSet2():
    board = makeBoard()
    board[6][7] = board[7][7] = None
    board[5][5] = Rook('w')
    board[1][6] = None
    board[6][7] = Pawn('b')
    return board
##### DEBUGGING STUFF

if __name__ == "__main__":
    try:
        start()
    except KeyboardInterrupt:
        pass
