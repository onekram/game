package game.board;

import game.Cell;
import game.Move;
import game.Result;

import java.util.Arrays;
import java.util.Map;
public class MNKBoard implements Board {
    protected final int rows, cols, k;
    protected int freeCells;
    private static final Map<Cell, Character> SYMBOLS = Map.of(
            Cell.X, 'X',
            Cell.O, 'O',
            Cell.EMPTY, '.',
            Cell.BLOCK, ' '
    );

    protected final Cell[][] cells;
    private Cell turn;
    private final  Position position;
    public MNKBoard(int n, int m, int k) {
        this.rows = m;
        this.cols = n;
        this.k = k;
        this.position = new MNKPosition();
        cells = new Cell[m][n];
        fillArr();
        turn = Cell.X;
    }

    protected void fillArr() {
        freeCells = rows * cols;
        for (Cell[] row : cells) {
            Arrays.fill(row, Cell.EMPTY);
        }
    }

    @Override
    public Position getPosition() {
        return position;
    }

    @Override
    public Cell getCell() {
        return turn;
    }


    private enum Direction {
    VERTICAL, HORIZONTAL, DIAGONAL, SECOND_DIAGONAL
    }

    protected static final class CellRunner {
        private final int row, col;
        private int currentRow, currentCol;
        public CellRunner(int row, int col) {
            this.row = row;
            this.col = col;
            currentRow = row;
            currentCol = col;
        }
        public void next(Direction direction, boolean isStepUp) {
            switch (direction) {
                case VERTICAL:
                    if (isStepUp) {
                        currentRow++;
                    } else {
                        currentRow--;
                    }
                    break;
                case HORIZONTAL:
                    if (isStepUp) {
                        currentCol++;
                    } else {
                        currentCol--;
                    }
                    break;
                case DIAGONAL:
                    if (isStepUp) {
                        currentCol++;
                        currentRow++;
                    } else {
                        currentCol--;
                        currentRow--;
                    }
                    break;
                case SECOND_DIAGONAL:
                    if (isStepUp) {
                        currentCol--;
                        currentRow++;
                    } else {
                        currentCol++;
                        currentRow--;
                    }
            }
        }

        public void setPrev() {
            currentCol = col;
            currentRow = row;
        }
        public int getCurrentRow() {
            return currentRow;
        }
        public int getCurrentCol() {
            return currentCol;
        }

    }
    public boolean checkInBound(int x, int y) {
        return x >= 0 && x < rows && y >= 0 && y < cols;
    }

    private Result checkSeq(Move move) {
        CellRunner cr = new CellRunner(move.row(), move.column());
        Cell currentCell = move.value();
        int sizeUp = 0;
        int sizeDown = 0;
        boolean extraMove = false;
        for (Direction direction : Direction.values()) {
            while (checkInBound(cr.getCurrentRow(), cr.getCurrentCol()) && cells[cr.getCurrentRow()][cr.getCurrentCol()] == currentCell) {
                sizeUp++;
                cr.next(direction, true);
            }
            cr.setPrev();
            cr.next(direction, false);
            while (checkInBound(cr.getCurrentRow(), cr.getCurrentCol()) && cells[cr.getCurrentRow()][cr.getCurrentCol()] == currentCell) {
                sizeDown++;
                cr.next(direction, false);
            }
            cr.setPrev();
            if (sizeUp + sizeDown >= k) {
                return Result.WIN;
            }
            extraMove = extraMove || (sizeUp <= 5) && (sizeDown <= 4) && (sizeUp + sizeDown > 4);
            sizeUp = 0;
            sizeDown = 0;
        }
        if (extraMove) {
            return Result.EXTRA_MOVE;
        }
        return Result.UNKNOWN;
    }

    private boolean CheckDraw() {
        return freeCells == 0;
    }

    private void putCell(int row, int col, Cell value) {
        if (cells[row][col] == Cell.EMPTY) {
            cells[row][col] = value;
            freeCells--;
        }
    }
    @Override
    public Result makeMove(final Move move) {
        if (!position.isValid(move)) {
            return Result.LOSE;
        }
        putCell(move.row(), move.column(), move.value());
        Result res = checkSeq(move);
        if (res == Result.WIN) {
            return res;
        }
        if (CheckDraw()) {
            return Result.DRAW;
        }
        if (res == Result.EXTRA_MOVE) {
            return res;
        }
        swapStep();
        return Result.UNKNOWN;
    }

    private void swapStep() {
        turn = turn == Cell.X ? Cell.O : Cell.X;
    }



    private static void addSpaces(final StringBuilder sb, final int n) {
        sb.append(" ".repeat(n));
    }

    private static int getLength(final int value) {
        return (int) Math.floor(Math.log10(value)) + 1;
    }
    private class MNKPosition implements Position {

        @Override
        public boolean isValid(final Move move) {
            return checkInBound(move.row(), move.column())
                    && cells[move.row()][move.column()] == Cell.EMPTY
                    && turn == getCell();
        }
        @Override
        public int getRows() {
            return rows;
        }

        @Override
        public int getColumns() {
            return cols;
        }

        @Override
        public String toString() {
            final int spacesRow = getLength(rows);
            final int spacesCol = getLength(cols);
            final StringBuilder sb = new StringBuilder();
            addSpaces(sb, spacesCol + spacesRow);
            for (int c = 0; c < cols; c++) {
                sb.append(c + 1);
                addSpaces(sb, spacesCol - getLength(c + 1) + 1);

            }
            for (int r = 0; r < rows; r++) {
                sb.append("\n");
                addSpaces(sb, spacesRow - getLength(r + 1));
                sb.append(r + 1);
                addSpaces(sb, spacesCol);
                for (int c = 0; c < cols; c++) {
                    sb.append(SYMBOLS.get(cells[r][c]));
                    addSpaces(sb, spacesCol);
                }
            }
            return sb.toString();
        }

    }
    public void clear() {
        fillArr();
        turn = Cell.X;
    }
}

