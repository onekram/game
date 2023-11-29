package game.player;

import game.Cell;
import game.Move;
import game.board.Position;
import game.StepValueScanner;

import java.io.PrintStream;
import java.util.Scanner;

public class HumanPlayer implements Player {
    private final PrintStream out;
    private final PrintStream err;
    private final StepValueScanner in;
    private final String name;

    public HumanPlayer(final PrintStream out, final PrintStream err, final StepValueScanner in, final String name) {
        this.out = out;
        this.err = err;
        this.in = in;
        this.name = name;
    }

    public HumanPlayer(final String name) {
        this(System.out, System.err, new StepValueScanner(new Scanner(System.in)), name);
    }

    @Override
    public Move move(final Position position, final Cell cell) {
        while (true) {
            out.println(cell + "'s move");
            out.println("Enter row and column");
            final int rowMove = in.nextInt();
            final int columnMove = in.nextInt();
            if (rowMove == -1 || columnMove == -1) {
                err.println("Error: Values should be integer and positive! Try one more time!");
                continue;
            }
            final Move move = new Move(rowMove - 1, columnMove - 1, cell);
            if (position.isValid(move)) {
                return move;
            }
            err.println("Error: Move " + move + " is invalid! Try one more time!");
        }
    }
    @Override
    public String toString() {
        return name;
    }
}
