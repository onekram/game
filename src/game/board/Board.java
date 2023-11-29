package game.board;

import game.Cell;
import game.Move;
import game.Result;

public interface Board {
    Position getPosition();
    Cell getCell();
    Result makeMove(Move move);
    void clear();
}
