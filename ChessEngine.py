# This file contains the functions and attributes that reinforces chess rules, as well as the chess AI

# Citations:
# CMU 112 graphics framework. Available on: https://www.cs.cmu.edu/~112/notes/notes-animations-part1.html
# Images of chess pieces. 
  # Available on: https://www.kindpng.com/imgv/hxbhmb_chess-pieces-png-chess-pieces-sprite-sheet-transparent/
# Royalty free image of leaderboard png. 
  # Available on: https://thenounproject.com/term/pedestal/758425/

from cmu_112_graphics import *
import random
import time

class ChessEngine(object):
    stalemate = False

    def isLegalMove(self, board, currentRow, currentCol, nextRow, nextCol, generatedMove = False):
        dRow, dCol = nextRow - currentRow, nextCol - currentCol
        # Check piece can actually do inputted move
        if (dRow, dCol) not in self.moves or isinstance(board[currentRow][currentCol], Pawn):
            # Pawn & king has special moves. Pawn also cannot move normally with piece in front
            if isinstance(self, Pawn):
                if (self.hasMoved == False and (dRow, dCol) in self.doubleMove 
                    and board[nextRow][nextCol] == None): # double move
                    pass
                elif ((dRow, dCol) in self.attackMoves and board[nextRow][nextCol] != None and
                        board[nextRow][nextCol].color != self.color): # capture move
                    pass
                elif (dRow, dCol) in self.moves and board[nextRow][nextCol] == None: # normal move
                    pass
                else: return False
            elif isinstance(self, King) and self.hasMoved == False:
                if ((dRow, dCol) == (0, -2) and isinstance(board[currentRow][0], Rook)
                    and board[currentRow][0].hasMoved == False): pass
                elif ((dRow, dCol) == (0, 2) and isinstance(board[currentRow][7], Rook)
                    and (board[currentRow][7].hasMoved == False)): pass
                else: return False 
            else: return False
        if not isinstance(self, Knight):    # Piece cannot pass through others (except Knight)
            stepR, stepC = nextRow - currentRow, nextCol - currentCol
            unitR, unitC = stepR // max(abs(stepR), 1), stepC // max(abs(stepC), 1)
            # (unitR unitC) is one unit in the direction of movement
            tempRow, tempCol = currentRow + unitR, currentCol + unitC
            while (tempRow, tempCol) != (nextRow, nextCol): # NOTE: infinite loop if this is not legal move!
                if board[tempRow][tempCol] != None:
                    return False
                tempRow, tempCol = tempRow + unitR, tempCol + unitC
        if (board[nextRow][nextCol] != None) and (board[nextRow][nextCol].color == self.color):   
            # Pieces can't eat own team! That is evil D:
            return False
        return True

    @staticmethod
    def findKing(board, color):
        for row in range(len(board)):
            if color == 'w':
                if King('w') in board[row]:
                    col = board[row].index(King('w'))
                    return (row, col)
            elif color == 'b':
                if King('b') in board[row]:
                    col = board[row].index(King('b'))
                    return (row, col)

    @staticmethod
    def isInCheck(board, kingRow, kingCol): # This is a more efficient implementation of check
        #Check by knight
        for (drow, dcol) in Knight('opponent color').moves:
            checkRow, checkCol = kingRow + drow, kingCol + dcol
            if ((0 <= checkRow < len(board) and 0 <= checkCol < len(board)) and isinstance(board[checkRow][checkCol], Knight)
                and board[checkRow][checkCol].color != board[kingRow][kingCol].color):
                return True

        #Check by other pieces: check for pins / obstacles
        for (drow, dcol) in Queen('opponent color').moves:
            checkRow, checkCol = kingRow + drow, kingCol + dcol
            if ((0 <= checkRow < len(board) and 0 <= checkCol < len(board)) and board[checkRow][checkCol] != None
                and board[checkRow][checkCol].color != board[kingRow][kingCol].color):
                if board[checkRow][checkCol].isLegalMove(board, checkRow, checkCol, kingRow, kingCol):
                    return True
        return False

    # Check to see if move is legal and safe (does not put king in check)
    @staticmethod
    def tryMove(board, row, col, nextRow, nextCol):
        playerColor = board[row][col].color
        if board[row][col].isLegalMove(board, row, col, nextRow, nextCol):
            # Do move
            temp = board[nextRow][nextCol]
            board[nextRow][nextCol] = board[row][col]
            board[row][col] = None

            #Check if king is in check 
            kingRow, kingCol = ChessEngine.findKing(board, playerColor)
            inCheck = ChessEngine.isInCheck(board, kingRow, kingCol)

            # Undo move
            board[row][col] = board[nextRow][nextCol]
            board[nextRow][nextCol] = temp

            # if move puts player in check (True), it is not a possible move (False)!
            return not inCheck
        else: # not legal move, hence not possible
            return False 

    # Helper function for specialMoves – does pawn promotion conditionals
    def promotePawn(board, row, col):
        counter = 0
        while counter < 3:
            promotedPiece = simpledialog.askstring('getUserInput', 
            "What piece would you like to promote to? (Queen, Rook, Knight, Bishop): ")
            if promotedPiece.strip().lower() == 'queen':
                board[row][col] = Queen(board[row][col].color)
                return
            if promotedPiece.strip().lower() == 'rook':
                board[row][col] = Rook(board[row][col].color)
                return
            if promotedPiece.strip().lower() == 'bishop':
                board[row][col] = Bishop(board[row][col].color)
                return
            if promotedPiece.strip().lower() == 'knight':
                board[row][col] = Knight(board[row][col].color)
                return
            else:
                counter += 1
        board[row][col] = Queen(board[row][col].color)
        return

    @staticmethod
    def specialRules(board, row, col):
        if isinstance(board[row][col], King) and not board[row][col].hasMoved and col == 2:
            board[row][3], board[row][0] = board[row][0], None    # castling left side
        elif isinstance(board[row][col], King) and not board[row][col].hasMoved and col == 6:
            board[row][5], board[row][7] = board[row][7], None    # castling left side
        elif isinstance(board[row][col], Pawn) and (row == 0 or row == 7):
            ChessEngine.promotePawn(board, row, col)    # Pawn promotion
        if (isinstance(board[row][col], Pawn) or isinstance(board[row][col], King) 
            or isinstance(board[row][col], Rook)):    # Toggle hasMoved
            board[row][col].hasMoved = True

    # Check if there are any moves possible for opponent (ie. intercept check)
    @staticmethod
    def isInMate(board, kingRow, kingCol, crazyhousePieces):
        # Same algorithm as the generateMoves function, but returns bool as soon as move is found (faster)
        for row in range(len(board)):
            for col in range(len(board[row])):
                if board[row][col] != None and board[row][col].color == board[kingRow][kingCol].color:
                    for (drow, dcol) in board[row][col].moves:
                        nextRow, nextCol = row + drow, col + dcol
                        if (0 <= nextRow < len(board) and 0 <= nextCol < len(board) and
                            ChessEngine.tryMove(board, row, col, nextRow, nextCol)): 
                            return False    # there are possible moves!
                    if isinstance(board[row][col], Pawn):    # Pawn has special moves
                        for (drow, dcol) in board[row][col].attackMoves + board[row][col].doubleMove:
                            nextRow, nextCol = row + drow, col + dcol
                            if (0 <= nextRow < len(board) and 0 <= nextCol < len(board) and
                                ChessEngine.tryMove(board, row, col, nextRow, nextCol)): 
                                return False
                    # King cannot castle to get out of a check. So we can ignore castle as possible move
        if crazyhousePieces != None:    # In crazyhouse variant, can intercept check by dropping pieces
            # setting up – gets the playing color and corresponding dictionary
            pieces = [Queen, Rook, Bishop, Knight, Pawn]
            playerColor = board[kingRow][kingCol]
            if playerColor == 'b': row = 8
            else: row = 9
            # checks if the player can drop a crazyhouse piece to save king
            for col in range(len(crazyhousePieces)):
                if crazyhousePieces[pieces[col]] != 0:
                    for (drow, dcol) in King(playerColor).moves:
                        # can stop the check by placing around king
                        interceptRow, interceptCol = kingRow + drow, kingCol + dcol
                        if (0 <= interceptRow <= 7 and 0 <= interceptCol <= 7 and 
                            ChessEngine.tryCrazyhouseMove(board, row, col, interceptRow, interceptCol)):
                            return False    # legal move and king is safe!
                            # tryCrazyhouseMove takes care of checks by knights
        return True

    @staticmethod
    def checkGameState(board, whiteToMove, crazyhousePieces = None):
        wKRow, wKCol = ChessEngine.findKing(board, 'w')
        bKRow, bKCol = ChessEngine.findKing(board, 'b')
        board[wKRow][wKCol].inCheck = ChessEngine.isInCheck(board, wKRow, wKCol)
        board[bKRow][bKCol].inCheck = ChessEngine.isInCheck(board, bKRow, bKCol)
        if whiteToMove and ChessEngine.isInMate(board, bKRow, bKCol, crazyhousePieces):
            if board[bKRow][bKCol].inCheck: # black is in checkmate!
                board[bKRow][bKCol].inCheckmate = True
            else: 
                ChessEngine.stalemate = True # it is a stalemate!              
        elif not whiteToMove and ChessEngine.isInMate(board, wKRow, wKCol, crazyhousePieces):
            if board[wKRow][wKCol].inCheck: # white is in checkmate!
                board[wKRow][wKCol].inCheckmate = True
            else: 
                ChessEngine.stalemate = True # stalemate!

    @staticmethod
    def generateMoves(board, color):
        possibleMoves = []
        for row in range(len(board)):
            for col in range(len(board[row])):
                if board[row][col] != None and board[row][col].color == color:
                    for (drow, dcol) in board[row][col].moves:
                        nextRow, nextCol = row + drow, col + dcol
                        if (0 <= nextRow < len(board) and 0 <= nextCol < len(board) and
                            ChessEngine.tryMove(board, row, col, nextRow, nextCol)):
                            possibleMoves += [(row, col, nextRow, nextCol)]
                    if isinstance(board[row][col], Pawn):    # Pawn has special moves
                        for (drow, dcol) in board[row][col].attackMoves + board[row][col].doubleMove:
                            nextRow, nextCol = row + drow, col + dcol
                            if (0 <= nextRow < len(board) and 0 <= nextCol < len(board) and
                                ChessEngine.tryMove(board, row, col, nextRow, nextCol)): 
                                possibleMoves += [(row, col, nextRow, nextCol)]
                    elif isinstance(board[row][col], King) and not board[row][col].hasMoved: # King can castle
                        for (drow, dcol) in [(0, 2), (0, -2)]:
                            nextRow, nextCol = row + drow, col + dcol
                            if ChessEngine.tryMove(board, row, col, nextRow, nextCol):
                                possibleMoves += [(row, col, nextRow, nextCol)]
        return possibleMoves

    @staticmethod
    def positionEvaluation(board):
        points = 0
        for row in range(len(board)):
            for col in range(len(board[row])):
                if board[row][col] != None:
                    points += board[row][col].points
        return (points, [])

    @staticmethod
    def maxi(board, alpha, beta, depth):    # White wants to maximize (white pieces +)
        if depth == 0:    # base case
            return ChessEngine.positionEvaluation(board)
        # recursive case
        possibleMoves = ChessEngine.generateMoves(board, 'w')
        bestMoves = []
        alpha, beta = -100000, 100000    
        for (row, col, nextRow, nextCol) in possibleMoves:
            # make move
            temp = board[nextRow][nextCol]
            board[nextRow][nextCol] = board[row][col]
            board[row][col] = None
            # evaluate move
            (points, enemyMoves) = ChessEngine.mini(board, alpha, beta, depth - 1)
            # undo move
            board[row][col] = board[nextRow][nextCol]
            board[nextRow][nextCol] = temp

            # check if move was good
            if points > beta:
                return (100000, [])    # Beta-cutoff (a previous move had better outcome)

            if points > alpha:
                alpha = points
                bestMoves = [(row, col, nextRow, nextCol, points)]
            elif points == alpha: 
                bestMoves += [(row, col, nextRow, nextCol, points)]
        return (alpha, bestMoves)

    @staticmethod
    def mini(board, alpha, beta, depth):    # Black wants to minimize (black pieces -)
        if depth == 0:    # base case
            return ChessEngine.positionEvaluation(board)
        
        # recursive case
        possibleMoves = ChessEngine.generateMoves(board, 'b')
        bestMoves = []
        alpha, beta = -100000, 100000    
        for (row, col, nextRow, nextCol) in possibleMoves:
            # make move
            temp = board[nextRow][nextCol]
            board[nextRow][nextCol] = board[row][col]
            board[row][col] = None
            # evaluate move
            (points, enemyMoves) = ChessEngine.maxi(board, alpha, beta, depth - 1)
            # undo move
            board[row][col] = board[nextRow][nextCol]
            board[nextRow][nextCol] = temp

            # check if move was good
            if points < alpha:
                return (-100000, [])    # Alpha-cutoff (a previous move had better outcome)
            
            if points < beta:
                beta = points
                bestMoves = [(row, col, nextRow, nextCol, points)]
            elif points == beta: 
                bestMoves += [(row, col, nextRow, nextCol, points)]
        return (beta, bestMoves)

    @staticmethod
    def AIMove(board, color, depth):
        # Find best move
        if color == 'b':
            (score, bestMoves) = ChessEngine.mini((board), -100000, 100000, depth)
        # print(score, bestMoves)
        # Make move
        if len(bestMoves) == 0: return
        (row, col, nextRow, nextCol, points) = random.choice(bestMoves)
        board[nextRow][nextCol] = board[row][col]
        board[row][col] = None
        # Check rules & game status (ie. checkmates)
        ChessEngine.specialRules(board, nextRow, nextCol)
        if color == 'w': whiteToMove = True
        else: whiteToMove = False
        ChessEngine.checkGameState(board, whiteToMove)
    
    @staticmethod
    def tryCrazyhouseMove(board, previousRow, previousCol, row, col):
        pieces = [Queen, Rook, Bishop, Knight, Pawn]
        # print(pieces[previousCol])
        if board[row][col] != None:
            return False    # cannot place in another piece
        elif pieces[previousCol] == Pawn and (row == 0 or row == 7): 
            return False    # pawn cannot be dropped on end ranks
        else:    # This is legal move; still need to check if king is in danger
            if previousRow == 8: playerColor = 'b'
            else: playerColor = 'w'
            # Make move
            board[row][col] = pieces[previousCol](playerColor)
            # See if king is in check 
            kingRow, kingCol = ChessEngine.findKing(board, playerColor)
            inCheck = ChessEngine.isInCheck(board, kingRow, kingCol)
            # Undo move
            board[row][col] = None
            # if move puts player in check (True), it is not a possible move (False)!
            return not inCheck

class King(ChessEngine):
    def __init__(self, color):
        self.color = color
        self.moves = []
        for drow in [-1, 0, 1]:
            for dcol in [-1, 0, 1]:
                if not (drow == 0 and dcol == 0):
                    self.moves += [(drow, dcol)]
        self.hasMoved = False    # For castling
        self.inCheck = False    # Make sure next move gets king to safety
        self.inCheckmate = False    # End game condition
        if color == 'w':
            self.points = 900
        elif color == 'b':
            self.points = -900
    def __eq__(self, other):
        return isinstance(other, King) and self.color == other.color

class Queen(ChessEngine):
    def __init__(self, color):
        self.color = color
        self.moves = []
        for drow in [-1, 0, 1]:
            for dcol in [-1, 0, 1]:
                if not (drow == 0 and dcol == 0):
                    for distance in range(1, 8):
                        self.moves += [(drow  * distance, dcol * distance)]
        if color == 'w':
            self.points = 90
        elif color == 'b':
            self.points = -90

class Bishop(ChessEngine):
    def __init__(self, color):
        self.color = color
        self.moves = []
        for drow in [-1, 1]:
            for dcol in [-1, 1]:
                for distance in range(1, 8):
                    self.moves += [(drow  * distance, dcol * distance)]
        if color == 'w':
            self.points = 30
        elif color == 'b':
            self.points = -30

class Knight(ChessEngine):
    def __init__(self, color):
        self.color = color
        self.moves = []
        for drow in [-2, -1, 1, 2]:
            for dcol in [-2, -1, 1, 2]:
                if not (abs(drow) == abs(dcol)):
                    self.moves += [(drow, dcol)]
        if color == 'w':
            self.points = 30
        elif color == 'b':
            self.points = -30

class Rook(ChessEngine):
    def __init__(self, color):
        self.color = color
        self.moves = []
        for drow in [-1, 1]:
            for distance in range(1, 8):
                self.moves += [(drow  * distance, 0)]
        for dcol in [-1, 1]:
            for distance in range(1, 8):
                self.moves += [(0, dcol * distance)]
        self.hasMoved = False    # For castling
        if color == 'w':
            self.points = 50
        elif color == 'b':
            self.points = -50

class Pawn(ChessEngine):
    def __init__(self, color):
        self.color = color
        if color == "w":
            forward = -1
        elif color == 'b':
            forward = 1
        self.moves = [(forward, 0)]
        self.attackMoves = [(forward, 1), (forward, -1)]
        self.doubleMove = [(2 * forward, 0)]
        self.hasMoved = False    #For doubleMove
        if color == 'w':
            self.points = 10
        elif color == 'b':
            self.points = -10