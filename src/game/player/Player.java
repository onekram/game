package game.player;

import game.Cell;
import game.Move;
import game.board.Position;

public interface Player {
    Move move(Position position, Cell cell);
}
