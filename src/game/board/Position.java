package game.board;

import game.Move;

public interface Position {
    boolean isValid(Move move);
    int getRows();
    int getColumns();
//    boolean CheckInBound(int x, int y);
}
