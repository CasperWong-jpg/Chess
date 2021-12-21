Project Description

Name: Chess!
Description: This project is a fully functional version of the board game chess, which checks for and enforcing rules such as castling, pawn promotion, and checkmate. 

In the GUI version (which requires module installation), the game can be played by two people on the same computer, and also by one person against various AI difficulties. Users may also play another chess variant called Crazyhouse, where users can place any piece that they capture on the board as their own!

In the command line interface version (which does not require external modules), the game can be played against an AI using long algebraic notation commands (same as UCI protocol).

Finally, this chess engine can also be easily connected to a Lichess bot account, where it can automatically play others on this online chess platform!
__________

How to run GUI project:
0. Download Code as a ZIP. Install the PIL/Pillow and Requests modules (see below for instructions).

1. Open main.py file 
  – I would recommend using VSCode and installing the Python extension by Microsoft within the software

2. Change line 16 of main.py so that you provide the path from current terminal directory to the "Chess-main" folder
 
3. Run main.py and enjoy!


Modules and libraries to download:
PIL/Pillow and Requests
 - Instructions for download found here: https://www.cs.cmu.edu/~112/notes/notes-animations-part1.html
__________

How to run command line interface project:
If there are issues downloading PIL/Pillow and Requests, you can play on a simple command line interface, which does not require module installment.

1. Open simple-main.py (using an editor like VSCode) and run!
__________

How to implement as Lichess bot:
See instructions here: https://github.com/ShailChoksi/lichess-bot

Directions for Mac
1. Change terminal to lichess_bot directory
2. Run: " python3 lichess-bot.py " 

__________
Coming soon:
Better evaluation heuristics (ie. Piece-square tables) https://www.freecodecamp.org/news/simple-chess-ai-step-by-step-1d55a9266977/ 
