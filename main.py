# This is the main file to run the game. It includes all the elements for animation & user interactions.

# Citations:
# CMU 112 graphics framework. Available on: https://www.cs.cmu.edu/~112/notes/notes-animations-part1.html
# Images of chess pieces. 
  # Available on: https://www.kindpng.com/imgv/hxbhmb_chess-pieces-png-chess-pieces-sprite-sheet-transparent/
# Image of leaderboard symbol. 
  # Available on: https://thenounproject.com/term/pedestal/758425/

from cmu_112_graphics import *
import random
import time

import os
# USER INPUT: Please input the path to the working directory in the line below:
workingDirectory = 'downloads/Chess-main'  # Default is set to "downloads/TP" for Macbook users
os.chdir(workingDirectory)

# Below file contains functions + attributes that reinforces chess rules, as well as chess AI
from ChessEngine import *

class NormalChessGame(Mode):
    # Makes a chess board with default starting pieces
    def makeBoard(self):
        # empty board
        board = [([None] * self.cols) for row in range(self.rows)]

        # rook, knight, bishop, queen, king
        board[0][0], board[0][self.cols-1] = Rook('b'), Rook('b')
        board[0][1], board[0][self.cols-2] = Knight('b'), Knight('b')
        board[0][2], board[0][self.cols-3] = Bishop('b'), Bishop('b')
        board[0][3], board[0][4] = Queen('b'), King('b')
        board[self.rows-1][0], board[self.rows-1][self.cols-1] = Rook('w'), Rook('w')
        board[self.rows-1][1], board[self.rows-1][self.cols-2] = Knight('w'), Knight('w')
        board[self.rows-1][2], board[self.rows-1][self.cols-3] = Bishop('w'), Bishop('w')
        board[self.rows-1][3], board[self.rows-1][4] = Queen('w'), King('w')

        # pawns
        for col in range(self.cols):
            board[1][col] = Pawn('b')
            board[self.rows-2][col] = Pawn('w')
        
        return board
    
    def loadImages(self):
        self.bK = self.loadImage("images/bK.png")    # K represents king
        self.bQ = self.loadImage("images/bQ.png")
        self.bB = self.loadImage("images/bB.png")
        self.bN = self.loadImage("images/bN.png")    # N represents knight
        self.bR = self.loadImage("images/bR.png")
        self.bP = self.loadImage("images/bP.png")
        self.wK = self.loadImage("images/wK.png")
        self.wQ = self.loadImage("images/wQ.png")
        self.wB = self.loadImage("images/wB.png")
        self.wN = self.loadImage("images/wN.png")
        self.wR = self.loadImage("images/wR.png")
        self.wP = self.loadImage("images/wP.png")

    # Stores arguments passed in from splash screen
    def __init__(self, AIDifficulty):
        super().__init__()
        self.AIDifficulty = AIDifficulty

    # Stores app variables
    def appStarted(self):
        self.loadImages()        

        # Loading board
        self.margin = 30
        self.rows = self.cols = 8
        self.board = self.makeBoard()

        # Game considerations
        self.selectedPiece = None   # (row, col)
        self.whiteToMove = True
        self.pawnPromotion = None
        self.timerDelay = 1000   # AI checking if it is his turn yet
        self.cheated = False    # Player cannot just turn on AI after wining to improve leaderboard
    
    # Function is taken from https://www.cs.cmu.edu/~112/notes/notes-animations-part1.html
    def pointInGrid(self, x, y):
        return ((self.margin <= x <= self.width-self.margin) and
                (self.margin <= y <= self.height-self.margin))
    
    # Function is taken and modified from https://www.cs.cmu.edu/~112/notes/notes-animations-part1.html
    def getCell(self, x, y):
        if not self.pointInGrid(x, y):
            return (-1, -1)
        cellWidth = (self.width - 2*self.margin) / self.cols
        cellHeight = (self.height - self.margin - self.margin) / self.rows
        row = int((y - self.margin) / cellHeight)
        col = int((x - self.margin) / cellWidth)
        return row, col

    # Function is taken and modified from https://www.cs.cmu.edu/~112/notes/notes-animations-part1.html
    def getCellBounds(self, row, col):
        cellWidth = (self.width - 2*self.margin) / self.cols
        cellHeight = (self.height - self.margin - self.margin) / self.rows
        x0 = self.margin + col * cellWidth
        x1 = self.margin + (col+1) * cellWidth
        y0 = self.margin + row * cellHeight
        y1 = self.margin + (row+1) * cellHeight
        return (x0, y0, x1, y1)

    def movePiece(self, previousRow, previousCol, row, col):
        self.board[row][col] = self.board[previousRow][previousCol]
        self.board[previousRow][previousCol] = None

    # Tracks (row, col) input for chess move that player makes. Makes decision accordingly.
    def mousePressed(self, event):
        row, col = self.getCell(event.x, event.y)
        if (row, col) == (-1, -1): return    # input not on board

        elif self.board[row][col] != None and self.selectedPiece == None:    # selecting a piece
            piece = self.board[row][col]
            if (piece.color == 'w' and self.whiteToMove) or (piece.color == 'b' and not self.whiteToMove):
                self.selectedPiece = (row, col)
        
        elif self.selectedPiece != None:    # chess piece already selected
            previousRow, previousCol = self.selectedPiece

            if (previousRow, previousCol) == (row, col):    # unselect piece
                self.selectedPiece = None

            elif (self.board[previousRow][previousCol].tryMove(
                  self.board, previousRow, previousCol, row, col)):
                # move piece
                self.movePiece(previousRow, previousCol, row, col)

                # Toggle hasMoved, castling, pawn promotion
                ChessEngine.specialRules(self.board, row, col)
                
                # Check if opponent king is in check, checkmate, or stalemate
                ChessEngine.checkGameState(self.board, self.whiteToMove)

                # Now it is the next player's turn
                self.whiteToMove = not self.whiteToMove
                self.selectedPiece = None
        

    def checkCondition(self):
        self.board[1][5] = self.board[6][4] = None
        self.board[2][5] = Pawn('b')
        self.board[4][4] = Pawn('w')

    def keyPressed(self, event):
        if event.key in 'aA':    # toggle AI on and off
            self.cheated = True
            if self.AIDifficulty == None:
                self.AIDifficulty = 1
            else:
                self.AIDifficulty = None
        elif event.key in 'rR':
            # Update leaderboard upon reset
            wKRow, wKCol = ChessEngine.findKing(self.board, 'w')
            bKRow, bKCol = ChessEngine.findKing(self.board, 'b')
            if self.board[bKRow][bKCol].inCheckmate and self.AIDifficulty != None and not self.cheated:
                self.app.wins[self.AIDifficulty - 1] += 1
            elif self.board[wKRow][wKCol].inCheckmate and self.AIDifficulty != None and not self.cheated:
                self.app.losses[self.AIDifficulty - 1] += 1
            # Reset game
            self.appStarted()
            ChessEngine.stalemate = False
        elif event.key in 'xX':
            wKRow, wKCol = ChessEngine.findKing(self.board, 'w')
            bKRow, bKCol = ChessEngine.findKing(self.board, 'b')
            self.app.setActiveMode(self.app.TitlePage)
        elif event.key in 'hH':
            self.app.setActiveMode(self.app.HelpScreen1)
        elif event.key == '1':    # check black in 1 move
            self.board = self.makeBoard()
            self.checkCondition()
            self.whiteToMove = True
        elif event.key == '2':    # checkmate black in 1 move
            self.board = self.makeBoard()
            self.checkCondition()
            self.board[1][6], self.board[3][6] = None, Pawn('b')
            self.board[6][5], self.board[4][5] = None, Pawn('w')
            self.whiteToMove = True
        elif event.key == '3':    # AI depth testing
            self.board = self.makeBoard()
            # white pieces
            self.board[6][3] = self.board[7][2] = self.board[7][1] = None
            self.board[4][3] = Pawn('w')
            self.board[4][5] = Bishop('w')
            self.board[3][1] = Knight('w')
            # black pieces
            self.board[1][3] = self.board[1][4] = None
            self.board[3][3], self.board[2][4] = Pawn('b'), Pawn('b')
            self.whiteToMove = True
        elif event.key == '4':    # castling testing
            self.board = self.makeBoard()
            # white pieces
            self.board[6][3] = self.board[7][2] = self.board[7][1] = self.board[7][3] = None
            self.board[4][3] = Pawn('w')
            self.board[4][5] = Bishop('w')
            self.board[3][1] = Knight('w')
            self.board[6][3] = Queen('w')
            # black pieces
            self.board[1][3] = self.board[1][4] = None
            self.board[3][3], self.board[2][4] = Pawn('b'), Pawn('b')
            self.whiteToMove = True

    def timerFired(self):
        if self.AIDifficulty != None and not self.whiteToMove:
            print("AI is thinking...")
            passed = time.time()
            ChessEngine.AIMove(self.board, 'b', self.AIDifficulty)
            self.whiteToMove = not self.whiteToMove
            print("...AI has moved", time.time() - passed)

    # Draws header on top: title & status of game (ie. check)
    def drawHeader(self, canvas):
        canvas.create_text(self.width / 2, 0, text = 'Chess :)', 
                            anchor = 'n', font = 'Arial 24 bold')
        wKRow, wKCol = ChessEngine.findKing(self.board, 'w')
        bKRow, bKCol = ChessEngine.findKing(self.board, 'b')
        if self.board[wKRow][wKCol].inCheckmate:
            canvas.create_text(self.width / 2, self.height - self.margin, text = 'Black wins! Press "r" to reset',
                                anchor = 'n', font = 'Arial 20 bold')
        if self.board[bKRow][bKCol].inCheckmate:
            canvas.create_text(self.width / 2, self.height - self.margin, text = 'White wins! Press "r" to reset',
                                anchor = 'n', font = 'Arial 20 bold')
        if self.board[wKRow][wKCol].inCheck: 
            canvas.create_text(self.margin, self.margin, text = 'White in check!', 
                                anchor = 'sw', font = 'Arial 16')
        if self.board[bKRow][bKCol].inCheck:
            canvas.create_text(self.width - self.margin, self.margin, text = 'Black in check!', 
                                anchor = 'se', font = 'Arial 16')
        if ChessEngine.stalemate:
            canvas.create_text(self.width - self.margin, self.margin, text = 'Stalemate!', 
                                anchor = 'se', font = 'Arial 16')
        if self.AIDifficulty != None:
            canvas.create_text(self.width / 2, self.height - self.margin, text = 'Black is thinking!', 
                                anchor = 'se', font = 'Arial 16')
        
    # Draws empty board
    def drawBoard(self, canvas):
        for row in range(self.rows):
            for col in range(self.cols):
                x0, y0, x1, y1 = self.getCellBounds(row, col)
                if (row + col) % 2 == 0: fill = 'white'
                else: fill = 'saddle brown'
                canvas.create_rectangle(x0, y0, x1, y1, fill = fill)
    
    # Finds appropriate image for chess piece
    def assignImage(self, chessPiece):
        classes = [King, Queen, Bishop, Knight, Rook, Pawn]
        if chessPiece.color == 'w':
            image = [self.wK, self.wQ, self.wB, self.wN, self.wR, self.wP]
            for i in range(len(classes)):
                if isinstance(chessPiece, classes[i]):
                    return image[i]
        else:
            image = [self.bK, self.bQ, self.bB, self.bN, self.bR, self.bP]
            for i in range(len(classes)):
                if isinstance(chessPiece, classes[i]):
                    return image[i]

    # Draws pieces on board
    def drawPieces(self, canvas):
        for row in range(self.rows):
            for col in range(self.cols):
                if self.board[row][col] != None:
                    x0, y0, x1, y1 = self.getCellBounds(row, col)
                    image = self.assignImage(self.board[row][col])
                    canvas.create_image((x0 + x1) / 2, (y0 + y1) / 2, 
                                        image = ImageTk.PhotoImage(image))
        if self.selectedPiece != None:   # draws a blue ring around selected piece
            row, col = self.selectedPiece
            if row < 8:
                x0, y0, x1, y1 = self.getCellBounds(row, col)
                canvas.create_rectangle(x0, y0, x1, y1, width = 4, outline = 'deep sky blue')
    
    # Draws app
    def redrawAll(self, canvas):
        self.drawHeader(canvas)
        self.drawBoard(canvas)
        self.drawPieces(canvas)

class CrazyhouseGame(NormalChessGame):
    def appStarted(self):
        super().appStarted()
        self.margin = 50
        self.whitePieces = {Queen: 0, Rook: 0, Bishop: 0, Knight: 0, Pawn: 0}
        self.blackPieces = {Queen: 0, Rook: 0, Bishop: 0, Knight: 0, Pawn: 0}
    
    def movePiece(self, previousRow, previousCol, row, col):
        if self.board[row][col] != None:
            if self.whiteToMove: self.whitePieces[type(self.board[row][col])] += 1
            else: self.blackPieces[type(self.board[row][col])] += 1
        self.board[row][col] = self.board[previousRow][previousCol]
        self.board[previousRow][previousCol] = None

    def getCrazyCell(self, x, y, color):
        pieces = [Queen, Rook, Bishop, Knight, Pawn]
        if color == 'b':
            x0, x1 = 5, self.width / 2 - self.margin
            row = 8
        else:
            x0, x1 = self.width / 2 + self.margin, self.width - 5
            row = 9
        cellWidth = (x1 - x0) / len(pieces)
        col = int((x - x0) / cellWidth)
        if color == 'b' and self.blackPieces[pieces[col]] != 0 and not self.whiteToMove:
            return (row, col)
        elif color == 'w' and self.whitePieces[pieces[col]] != 0 and self.whiteToMove: 
            return (row, col)
        else:
            return (-1, -1)

    def mousePressed(self, event):
        row, col = self.getCell(event.x, event.y)
        if (row, col) == (-1, -1): 
            # Additional crazyhouse selections, if piece not selected already
            if (event.x <= self.width / 2 - self.margin and 
                event.y >= self.height - self.margin):
                row, col = self.getCrazyCell(event.x, event.y, 'b')
            elif (event.x >= self.width / 2 + self.margin and 
                event.y >= self.height - self.margin):
                row, col = self.getCrazyCell(event.x, event.y, 'w')
            if (row, col) != (-1, -1) and self.selectedPiece == None: 
                self.selectedPiece = (row, col)
            elif self.selectedPiece == (row, col):
                self.selectedPiece = None
            
        elif self.board[row][col] != None and self.selectedPiece == None:    # selecting a piece
            piece = self.board[row][col]
            if (piece.color == 'w' and self.whiteToMove) or (piece.color == 'b' and not self.whiteToMove):
                self.selectedPiece = (row, col)
        
        elif self.selectedPiece != None:    # chess piece already selected
            previousRow, previousCol = self.selectedPiece

            if (previousRow, previousCol) == (row, col):    # unselect piece
                self.selectedPiece = None
                return

            elif previousRow >= 8:
                if not ChessEngine.tryCrazyhouseMove(self.board, previousRow, previousCol, row, col):
                    return    # illegal move: do nothing
                elif previousRow == 8 and self.board[row][col] == None:    # crazyhouse move
                    pieces = list(self.blackPieces)
                    self.board[row][col] = pieces[previousCol]('b')
                    self.blackPieces[pieces[previousCol]] -= 1
                elif previousRow == 9 and self.board[row][col] == None:    # crazyhouse move
                    pieces = list(self.whitePieces)
                    self.board[row][col] = pieces[previousCol]('w')
                    self.whitePieces[pieces[previousCol]] -= 1

            elif (self.board[previousRow][previousCol].tryMove(
                  self.board, previousRow, previousCol, row, col)):
                # move piece
                self.movePiece(previousRow, previousCol, row, col)
            
            else: return    # no legal move made

            # Toggle hasMoved, castling, pawn promotion
            ChessEngine.specialRules(self.board, row, col)
                
            # Check if opponent king is in check, checkmate, or stalemate
            if self.whiteToMove: crazyhousePieces = self.blackPieces
            else: crazyhousePieces = self.whitePieces
            ChessEngine.checkGameState(self.board, self.whiteToMove, crazyhousePieces)

            # Now it is the next player's turn
            self.whiteToMove = not self.whiteToMove
            self.selectedPiece = None
        
        # if row < 8: print(self.board[row][col])

    def keyPressed(self, event):
        super().keyPressed(event)
        if event.key in 'hH':
            self.app.setActiveMode(self.app.HelpScreen2)

    def drawHeader(self, canvas):
        super().drawHeader(canvas)
        canvas.create_text(self.width / 2, self.margin, text = 'Crazyhouse variant!', 
                            anchor = 's', font = 'Arial 16 bold')

    def drawCrazyhousePieces(self, canvas, color, x0, y0, x1, y1):
        pieces = [Queen, Rook, Bishop, Knight, Pawn]
        cellWidth = (x1 - x0) / len(pieces)
        yMid = y1 - 15

        for i in range(len(pieces)):
            image = self.assignImage(pieces[i](color))
            image = self.scaleImage(image, 2/3)
            startX, endX = x0 + i * cellWidth, x0 + (i + 1) * cellWidth
            canvas.create_image((startX + endX) / 2, (y0 + yMid) / 2, 
                                image = ImageTk.PhotoImage(image))
            
            if color == 'w': 
                d = self.whitePieces
            else: 
                d = self.blackPieces
            canvas.create_text((startX + endX) / 2, yMid, text = d[pieces[i]],
                                font = 'Arial 10', anchor = 'n')

        if self.selectedPiece != None:
            row, col = self.selectedPiece
            if row == 8 and color == 'b':
                canvas.create_rectangle(x0 + col * cellWidth, y0, x0 + (col + 1) * cellWidth, 
                                        yMid, width = 3, outline = 'deep sky blue')
            elif row == 9 and color == 'w':
                canvas.create_rectangle(x0 + col * cellWidth, y0, x0 + (col + 1) * cellWidth, 
                                        yMid, width = 3, outline = 'deep sky blue')

    def redrawAll(self, canvas):
        super().redrawAll(canvas)
        self.drawCrazyhousePieces(canvas, 'b', 5, self.height - self.margin, 
                                    self.width / 2 - self.margin, self.height)
        self.drawCrazyhousePieces(canvas, 'w', self.width / 2 + self.margin, 
                                    self.height - self.margin, self.width - 5, self.height)

class TitlePage(Mode):
    def appStarted(self):
        self.margin = 30
        self.cy = []
        for box in range(1, 5):
            self.cy += [self.margin + box * self.height / 5]
        self.boxWidth, self.boxHeight = 160, 40
        self.leaderboard = self.loadImage("images/leaderboard.png")
    
    def redrawAll(self, canvas):
        canvas.create_rectangle(0, 0, self.width, self.height, fill = "saddle brown")
        font1, font2, font3 = 'Arial 30 bold underline', 'Arial 22', 'Arial 16'
        canvas.create_text(self.width / 2, 2 * self.margin, text = 'Chess!', font = font1, fill = 'white')
        s = """Click to play any of the modes below\n\nPress 'h' for help and hot keys"""
        canvas.create_text(self.width / 2, self.cy[0], text = s, font = font3, fill = 'white', justify = 'center')
        canvas.create_rectangle(self.width / 2 - self.boxWidth, self.cy[1] - self.boxHeight, 
                                self.width / 2 + self.boxWidth, self.cy[1] + self.boxHeight,
                                fill = 'black', outline = 'white')
        canvas.create_text(self.width / 2, self.cy[1], text = '2 Player Chess', font = font2, fill = 'white')
        canvas.create_rectangle(self.width / 2 - self.boxWidth, self.cy[2] - self.boxHeight, 
                                self.width / 2 + self.boxWidth, self.cy[2] + self.boxHeight,
                                fill = 'black', outline = 'white')
        canvas.create_text(self.width / 2, self.cy[2], text = 'Play against the Kosbie (AI)!', font = font2, fill = 'white')
        canvas.create_rectangle(self.width / 2 - self.boxWidth, self.cy[3] - self.boxHeight, 
                                self.width / 2 + self.boxWidth, self.cy[3] + self.boxHeight,
                                fill = 'black', outline = 'white')
        canvas.create_text(self.width / 2, self.cy[3], text = 'Crazyhouse!', font = font2, fill = 'white')
        canvas.create_image(self.width - 2 * self.margin, 2 * self.margin, image=ImageTk.PhotoImage(self.leaderboard))
    
    def mousePressed(self, event):
        imageMidLength = 25
        if (self.width / 2 - self.boxWidth <= event.x <= self.width / 2 + self.boxWidth and 
            self.cy[1] - self.boxHeight <= event.y <= self.cy[1] + self.boxHeight):
            self.app.setActiveMode(self.app.NormalChessGame)
        elif (self.width / 2 - self.boxWidth <= event.x <= self.width / 2 + self.boxWidth and 
            self.cy[2] - self.boxHeight <= event.y <= self.cy[2] + self.boxHeight):
            self.app.setActiveMode(self.app.AIDifficultyScreen)
        elif (self.width / 2 - self.boxWidth <= event.x <= self.width / 2 + self.boxWidth and 
            self.cy[3] - self.boxHeight <= event.y <= self.cy[3] + self.boxHeight):
            self.app.setActiveMode(self.app.Crazyhouse)
        elif (self.width - 2 * self.margin - imageMidLength <= event.x <= self.width - 2 * self.margin + imageMidLength 
            and 2 * self.margin - imageMidLength <= event.y <= 2 * self.margin + imageMidLength):
            self.app.setActiveMode(self.app.Leaderboard)
    
    def keyPressed(self, event):
        if event.key in 'hH':
            self.app.setActiveMode(self.app.HelpScreen)
        elif event.key in 'xX':
            self.app.setActiveMode(self.app.TitlePage)

class HelpScreen(TitlePage):
    def __init__(self, previousGamemode):
        super().__init__()
        self.previousGamemode = previousGamemode

    def redrawAll(self, canvas):
        canvas.create_rectangle(0, 0, self.width, self.height, fill = "saddle brown")
        font1, font2, font3 = 'Arial 30 bold underline', 'Arial 22', 'Arial 16'
        canvas.create_text(self.width / 2, 2 * self.margin, text = 'Help', font = font1, fill = 'white')
        s0 = ("""You can play various modes! \n Play with a friend in a normal 2 player game or in the """ + 
              """whacky crazyhouse variant\n Alternatively, you can practice against Prof. Kosbie, the AI >:)""")
        s1 = ("""While in game, press 'r' to restart game on same settings \n""" +
              """Press 'a' to let Kosbie (easy difficulty) take over the black pieces!""")
        s2 = """If currently in game, press 'b' to return to game"""
        s3 = ("""Click anywhere or press 'x' to return to main menu and try a another gamemode!\n""")
        canvas.create_text(self.width / 2, self.cy[0], text = s0, font = font3, fill = 'white', justify = 'center')
        canvas.create_text(self.width / 2, self.cy[1], text = s1, font = font3, fill = 'white', justify = 'center')
        canvas.create_text(self.width / 2, self.cy[2], text = s2, font = font3, fill = 'white', justify = 'center')
        canvas.create_text(self.width / 2, self.cy[2], text = s2, font = font3, fill = 'white', justify = 'center')
        canvas.create_text(self.width / 2, self.cy[3], text = s3, font = font3, fill = 'white', justify = 'center')
    
    def mousePressed(self, event):
        self.app.setActiveMode(self.app.TitlePage)
    
    def keyPressed(self, event):
        super().keyPressed(event)
        if event.key in 'bB' and self.previousGamemode == "Normal":
            self.app.setActiveMode(self.app.NormalChessGame)
        elif event.key in 'bB' and self.previousGamemode == "Crazyhouse":
            self.app.setActiveMode(self.app.Crazyhouse)

class AIDifficultyScreen(TitlePage):
    def appStarted(self):
        super().appStarted()
        self.difficulty = None
    
    def redrawAll(self, canvas):
        canvas.create_rectangle(0, 0, self.width, self.height, fill = "saddle brown")
        font1, font2, font3 = 'Arial 30 bold underline', 'Arial 22', 'Arial 16'
        canvas.create_text(self.width / 2, 2 * self.margin, text = 'Prof. Kosbie difficulty:', font = font2, fill = 'white')
        canvas.create_rectangle(self.width / 2 - self.boxWidth, self.cy[0] - self.boxHeight, 
                                self.width / 2 + self.boxWidth, self.cy[0] + self.boxHeight,
                                fill = 'black', outline = 'white')
        canvas.create_text(self.width / 2, self.cy[0], text = 'Easy', font = font2, fill = 'white')
        canvas.create_rectangle(self.width / 2 - self.boxWidth, self.cy[1] - self.boxHeight, 
                                self.width / 2 + self.boxWidth, self.cy[1] + self.boxHeight,
                                fill = 'black', outline = 'white')
        canvas.create_text(self.width / 2, self.cy[1], text = 'Medium', font = font2, fill = 'white')
        canvas.create_rectangle(self.width / 2 - self.boxWidth, self.cy[2] - self.boxHeight, 
                                self.width / 2 + self.boxWidth, self.cy[2] + self.boxHeight,
                                fill = 'black', outline = 'white')
        canvas.create_text(self.width / 2, self.cy[2], text = '¡Hard!', font = font2, fill = 'white')
        if self.difficulty != None:
            s = f"Kosbie thinks {self.difficulty} moves ahead"
            canvas.create_text(self.width / 2, self.cy[3], text = s, font = font3, fill = 'white')
    
    def mousePressed(self, event):
        if (self.width / 2 - self.boxWidth <= event.x <= self.width / 2 + self.boxWidth and 
            self.cy[0] - self.boxHeight <= event.y <= self.cy[0] + self.boxHeight):
            self.app.setActiveMode(self.app.EasyDifficulty)
        elif (self.width / 2 - self.boxWidth <= event.x <= self.width / 2 + self.boxWidth and 
            self.cy[1] - self.boxHeight <= event.y <= self.cy[1] + self.boxHeight):
            self.app.setActiveMode(self.app.NormalDifficulty)
        elif (self.width / 2 - self.boxWidth <= event.x <= self.width / 2 + self.boxWidth and 
            self.cy[2] - self.boxHeight <= event.y <= self.cy[2] + self.boxHeight):
            self.app.setActiveMode(self.app.HardDifficulty)
    
    def mouseMoved(self, event):
        if (self.width / 2 - self.boxWidth <= event.x <= self.width / 2 + self.boxWidth and 
            self.cy[0] - self.boxHeight <= event.y <= self.cy[0] + self.boxHeight):
            self.difficulty = 1
        elif (self.width / 2 - self.boxWidth <= event.x <= self.width / 2 + self.boxWidth and 
            self.cy[1] - self.boxHeight <= event.y <= self.cy[1] + self.boxHeight):
            self.difficulty = 2
        elif (self.width / 2 - self.boxWidth <= event.x <= self.width / 2 + self.boxWidth and 
            self.cy[2] - self.boxHeight <= event.y <= self.cy[2] + self.boxHeight):
            self.difficulty = 3
        else:
            self.difficulty = None

class Leaderboard(TitlePage):
    # Algorithm for calculating ELO rating: https://en.wikipedia.org/wiki/Elo_rating_system#Performance_rating
    def redrawAll(self, canvas):
        canvas.create_rectangle(0, 0, self.width, self.height, fill = "saddle brown")
        font1, font2, font3 = 'Arial 30 bold underline', 'Arial 22', 'Arial 16'
        canvas.create_text(self.width / 2, 2 * self.margin, text = 'Leaderboard', font = font1, fill = 'white')
        subheader = ("""Play against Kosbie to improve your ELO rating!""")
        s0 = (f"""Wins against Easy Kosbie (ELO ~1350): {self.app.wins[0]} \nWins against Medium Kosbie (ELO ~1420):""" +
              f""" {self.app.wins[1]} \nWins against Hard Kosbie (ELO ~1500): {self.app.wins[2]}""")
        s1 = (f"""Losses against Easy Kosbie (ELO ~1350): {self.app.losses[0]} \nLosses against Medium Kosbie """ +
              f"""(ELO ~1420): {self.app.losses[1]} \nLosses against Hard Kosbie (ELO ~1500): {self.app.losses[2]}""")
        gamesPlayed = sum(self.app.wins) + sum(self.app.losses)
        if gamesPlayed == 0: 
            playerELO = '0 – Go play Kosbie!'
        else:
            playerELO, AIELO = 0, [1350, 1420, 1500]
            for difficulty in range(3):
                playerELO += (AIELO[difficulty] + 400) * self.app.wins[difficulty]
                playerELO += (AIELO[difficulty] - 400) * self.app.losses[difficulty]
            playerELO //= gamesPlayed
        s2 = f"""Your ELO rating: {playerELO} \nDisclamer: all ELOs provided are rough estimates!"""
        s3 = "Press 'x' or click the mouse to return to main menu!"
        canvas.create_text(self.width / 2, 2 * self.margin + 36, text = subheader, font = font2, fill = 'white', 
                            justify = 'center')
        canvas.create_text(self.width / 2, self.cy[0], text = s0, font = font3, fill = 'white', justify = 'center')
        canvas.create_text(self.width / 2, self.cy[1], text = s1, font = font3, fill = 'white', justify = 'center')
        canvas.create_text(self.width / 2, self.cy[2], text = s2, font = font3, fill = 'white', justify = 'center')
        canvas.create_text(self.width / 2, self.cy[3], text = s3, font = font3, fill = 'white', justify = 'center')
    
    def mousePressed(self, event):
        self.app.setActiveMode(self.app.TitlePage)

class MyModalApp(ModalApp):
    def appStarted(app):
        app.TitlePage = TitlePage()
        app.NormalChessGame = NormalChessGame(None)    # None = no AI
        app.Crazyhouse = CrazyhouseGame(None)
        app.EasyDifficulty = NormalChessGame(1)    # AI difficulty
        app.NormalDifficulty = NormalChessGame(2)
        app.HardDifficulty = NormalChessGame(3)
        app.AIDifficultyScreen = AIDifficultyScreen()
        app.HelpScreen = HelpScreen(None)    # help screen when called from main menu
        app.HelpScreen1 = HelpScreen("Normal")    # when called from a NormalChessGame
        app.HelpScreen2 = HelpScreen("Crazyhouse")    # when called from a CrazyhouseGame
        app.Leaderboard = Leaderboard()
        app.setActiveMode(app.TitlePage)
        app.wins, app.losses = [0, 0, 0], [0, 0, 0]

MyModalApp(width = 650, height = 650)
