package game.player;

import game.Cell;
import game.Move;
import game.board.Position;

public class SequentialPlayer implements Player {
    final String name;

    public SequentialPlayer(final String name) {
        this.name = name;
    }
    @Override
    public Move move(final Position position, final Cell cell) {
//        Board board = (Board) position;
//        board.makeMove()
        for (int r = 0; r < position.getRows(); r++) {
            for (int c = 0; c < position.getColumns(); c++) {
                final Move move = new Move(r, c, cell);
                if (position.isValid(move)) {
                    return move;
                }
            }
        }
        throw new IllegalStateException("No valid moves");
    }

    @Override
    public String toString() {
        return name;
    }
}
