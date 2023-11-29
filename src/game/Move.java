package game;

public record Move(int row, int column, Cell value) {

    @Override
    public String toString() {
        return "row=" + (row + 1) + ", column=" + (column + 1) + ", value=" + value;
    }
}
