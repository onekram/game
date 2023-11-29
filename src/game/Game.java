package game;

import game.board.Board;
import game.player.Player;

import java.util.Arrays;
import java.util.List;
import java.util.Random;

import static java.lang.Thread.sleep;

public class Game {
    private final boolean log;
    private final Player player1, player2;

    public Game(final boolean log, final Player player1, final Player player2) {
        this.log = log;
        this.player1 = player1;
        this.player2 = player2;
    }

    public int play(Board board) {
        while (true) {
            int result1;
            do {
                result1 = move(board, player1, 1);
            } while (result1 == 3);
            if (result1 != -1) {
                return result1;
            }

            int result2;
            do {
                result2 = move(board, player2, 2);
            } while (result2 == 3);
            if (result2 != -1) {
                return result2;
            }
        }
    }

    public Player playTillWinner(Board board) {
        int result;
        int steps = 3;
        do {
            result = play(board);
            board.clear();
            steps--;
        } while (result == 0 && steps >=0);
        List<Player> playerList = Arrays.asList(player1, player2);
        if (result == 0) {
            Random rand = new Random();
            return playerList.get(rand.nextInt(2));
        }
            return playerList.get(result - 1);
    }
    private int move(final Board board, final Player player, final int no) {
        final Move move;
        log("Position:\n" + board.getPosition());
        try {
            move = player.move(board.getPosition(), board.getCell());
        } catch (Exception e) {
            logError("Incorrect move for  "  + player +  ": " + e.getMessage());
            return 3 - no;
        }
        if (move == null) {
            logError("Incorrect move for  "  + player);
            return 3 - no;
        }
        final Result result = board.makeMove(move);
        log("Position:\n" + board.getPosition());
        log("Player " + no + " move: " + move + '\n');
        if (result == Result.WIN) {
            log(player + " won");
            return no;
        } else if (result == Result.LOSE) {
            log(player + " lose");
            return 3 - no;
        } else if (result == Result.DRAW) {
            log("Draw");
            return 0;
        } else if (result == Result.EXTRA_MOVE) {
            log("Extra move for " + player);
            return 3;
        } else {
            return -1;
        }
    }

    private void log(final String message) {
        if (log) {
            System.out.println(message);
        }
    }

    private void logError(final String message) {
        if (log) {
            System.err.println(message);
        }
    }
}
