## Python Chess App and AI
Description: This project is a fully functional version of the board game chess, which checks for and enforcing rules such as castling, pawn promotion, and checkmate. 

In the GUI version (which requires module installation), the game can be played by two people on the same computer, and also by one person against various AI difficulties. Users may also play another chess variant called Crazyhouse, where users can place any piece that they capture on the board as their own!

In the command line interface version (which does not require external modules), the game can be played against an AI using long algebraic notation commands (same as UCI protocol).

Finally, this chess engine can also be easily connected to a Lichess bot account, where it can automatically play others on this online chess platform!
__________
### How to run GUI project:
1. Install the PIL/Pillow and Requests modules.
  - `pip3 install pillow`
  - `pip3 install requests`
 
2. From repo directory, `python3 main_GUI.py` 

3. Enjoy!

__________

### How to run command line interface project:
If there are issues downloading PIL/Pillow and Requests, you can play on a simple command line interface, which does not require module installment.

1. From repo home directory, run `python3 main_CLI.py`
__________

### How to implement as Lichess bot:
See instructions here: https://github.com/ShailChoksi/lichess-bot

Directions:
1. From repo directory, run on terminal `cd lichess_bot_Python`
2. Change lichess_bot/config.yml OAuth token to bot account you own
3. Run `python3 lichess-bot.py`

__________
## Advanced C Chess Engine
Description: There is a second chess engine made in C using bitboards (for better efficiency in move generation) and improved evaluation heuristics (ie. Piece-square tables, opening book). This can be found in `lichess_bot_C`. Note that this is purely a chess engine - it takes in a FEN string representing current board state, and returns the best move as another string. It does not keep track of previous game states, so is not playable standalone. This bot is made to interface with the Lichess API.

OpenMP has been used to provide linear speedup on a standard 8 core computer. See [Parallel Chess](https://github.com/CasperWong-jpg/ParallelChess) repo for more documentation and detailed commits.

__________

### How to run
1. Change to lichess_bot_C directory
2. Run `make lichess` to create shared library file (chess engine can be involved through Python script) or `make playable` in which user manually supplies FEN string and engine returns best move.
3. cd `lichess_bot`
4. Change `lichess_bot/config.yml` OAuth token to bot account you own
5. `python3 lichess-bot.py`