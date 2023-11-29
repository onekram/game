package game.player;

import game.Cell;
import game.Move;
import game.board.Position;

import java.util.Random;

public class RandomPlayer implements Player {
    private final Random random;
    private final String name;

    public RandomPlayer(final Random random, final String name) {
        this.random = random;
        this.name = name;
    }

    public RandomPlayer(final String name) {
        this(new Random(), name);
    }

    @Override
    public Move move(final Position position, final Cell cell) {
        while (true) {
//            Board b = (Board) position;
//            b.makeMove(new Move(10, 10, Cell.O));
            int r = random.nextInt(position.getRows());
            int c = random.nextInt(position.getColumns());
            final Move move = new Move(r, c, cell);
            if (position.isValid(move)) {
                return move;
            }
        }
    }

    @Override
    public String toString() {
        return name;
    }
}
