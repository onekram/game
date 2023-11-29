package game.board;

import game.Cell;

import java.util.Arrays;

public class CircleBoard extends MNKBoard {
    public CircleBoard(int d, int k) {
        super(d, d, k);
    }
    protected void fillArr() {
        for (Cell[] row : cells) {
            Arrays.fill(row, Cell.BLOCK);
        }
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < rows; j++) {
                if (getDist(i, j) - (double) (rows - 1) / 2 < .2) {
                    cells[i][j] = Cell.EMPTY;
                    freeCells++;
                }
            }
        }
    }

    private double getDist(int x, int y) {
        return Math.sqrt(Math.pow(x - (double) (rows - 1) / 2, 2) + Math.pow(y - (double) (rows - 1) / 2, 2));
    }
}
