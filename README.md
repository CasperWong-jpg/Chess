# Project Description

__________

## Python Chess App and AI
See: https://www.youtube.com/watch?v=XATnYZ4gT1Y
Description: This project is a fully functional version of the board game chess, which checks for and enforces rules such as castling, pawn promotion, and checkmate. 

In the GUI version (which requires module installation), the game can be played by two people on the same computer, and also by one person against various AI difficulties. Users may also play another chess variant called Crazyhouse, where users can place any piece that they capture on the board as their own!

In the command line interface version (which does not require external modules), the game can be played against an AI using long algebraic notation commands (same as UCI protocol).

Finally, this chess engine can also be easily connected to a Lichess bot account, where it can automatically play others on this online chess platform!
__________

### How to run GUI project:
0. Download Code as a ZIP. Install the PIL/Pillow and Requests modules (see below for instructions).

1. Open main.py file 
  – I would recommend using VSCode and installing the Python extension by Microsoft within the software

2. Change line 16 of main.py so that you provide the path from current terminal directory to the "Chess-main" folder
 
3. Run main.py and enjoy!


Modules and libraries to download:
PIL/Pillow and Requests
 - Instructions for download found here: https://www.cs.cmu.edu/~112/notes/notes-animations-part1.html
__________

### How to run command line interface project:
If there are issues downloading PIL/Pillow and Requests, you can play on a simple command line interface, which does not require module installment.

1. Open simple-main.py (using an editor like VSCode) and run!
__________

### How to run as Lichess bot:
See instructions here: https://github.com/ShailChoksi/lichess-bot

Directions for Mac
1. From project root, `cd lichess_bot`
2. In `lichess_bot/config.yml`, change OAuth token to personal bot account
3. Run: ` python3 lichess-bot.py ` 

__________
## Advanced C Chess Engine
Description: There is a second chess engine made in C using bitboards (for better efficiency) and improved evaluation heuristics (ie. Piece-square tables, opening book). This can be found in `lichess_bot_refactored`. Note that this is only the chess engine and does not have a playable GUI - it takes in a FEN string representing current board state, and returns the best move as another string. This bot is made to interface with the Lichess API

Note: Development temporarily moved to [Parallel Chess Repo](https://github.com/CasperWong-jpg/ParallelChess), as part of a group project to parallelize the execution of Chess Engine using OpenMP.
__________

### How to run
1. `cd lichess_bot_refactored`
2. Run `make lichess` to create shared library file (chess engine can be involved through Python script), or `make playable` to make executable in which user manually supplies FEN string and engine returns best move.
3. For Lichess bot: change OAuth token in `config.yml` to personal bot account
4. Run: ` python3 lichess-bot.py ` 
