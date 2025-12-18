import java.util.HashSet;
import java.util.Set;

class Solution {
    public void solveSudoku(char[][] board) {
        final Board b = new Board(board);
        b.solve();
        b.print(board);
    }

    private final class Cell {
        private final Set<Integer> possibilities;

        public Cell() {
            this.possibilities = new HashSet<>();
            for (int i = 1; i <= 9; i++) {
                this.possibilities.add(i);
            }
        }

        public void set(Cell anotherCell) {
            this.possibilities.clear();
            this.possibilities.addAll(anotherCell.possibilities);
        }

        public boolean isPossible() {
            return !possibilities.isEmpty();
        }

        public boolean isDeterministic() {
            return possibilities.size() == 1;
        }

        public int getPossibilitySize() {
            return possibilities.size();
        }

        public Set<Integer> getPossibilities() {
            return Set.copyOf(possibilities);
        }

        public void removePossibleNumber(int num) {
            possibilities.remove(num);
        }

        public void setNumber(int num) {
            possibilities.clear();
            possibilities.add(num);
        }

        public int getNumber() {
            if (isDeterministic()) {
                return possibilities.iterator().next();
            } else {
                return 0;
            }
        }
    }

    private final class Board {
        private final Cell[][] rows;

        public Board() {
            rows = new Cell[9][9];
            for (int rowIndex = 0; rowIndex < 9; rowIndex++) {
                for (int colIndex = 0; colIndex < 9; colIndex++) {
                    rows[rowIndex][colIndex] = new Cell();
                }
            }
        }

        public Board(final char[][] readonlyBoard) {
            this();
            this.set(readonlyBoard);
        }

        public Board(final Board anotherBoard) {
            this();
            this.set(anotherBoard);
        }

        public void set(final char[][] readonlyBoard) {
            for (int rowIndex = 0; rowIndex < 9; rowIndex++) {
                for (int colIndex = 0; colIndex < 9; colIndex++) {
                    final char c = readonlyBoard[rowIndex][colIndex];
                    if (c != '.') {
                        setNumber(rowIndex, colIndex, (int)(c - '0'));
                    }
                }
            }
        }

        public void set(final Board anotherBoard) {
            for (int rowIndex = 0; rowIndex < 9; rowIndex++) {
                for (int colIndex = 0; colIndex < 9; colIndex++) {
                    final Cell thisCell = getCell(rowIndex, colIndex);
                    final Cell anotherCell = anotherBoard.getCell(rowIndex, colIndex);
                    thisCell.set(anotherCell);
                }
            }
        }

        public void print(final char[][] destBoard) {
            for (int rowIndex = 0; rowIndex < 9; rowIndex++) {
                for (int colIndex = 0; colIndex < 9; colIndex++) {
                    destBoard[rowIndex][colIndex] = (char)(getCell(rowIndex, colIndex).getNumber() + '0');
                }
            }
        }

        private Cell getCell(final int rowIndex, final int colIndex) {
            return rows[rowIndex][colIndex];
        }

        public void setNumber(final int rowIndex, final int colIndex, final int number) {
            // System.out.println("setNumber(" + rowIndex + ", " + colIndex + ", " + number + ");");
            getCell(rowIndex, colIndex).setNumber(number);
            // remove possibilities in the same row
            for (int colI = 0; colI < 9; colI++) {
                if (colI != colIndex) {
                    removePossibleNumber(rowIndex, colI, number);
                }
            }
            // remove possibilities in the same column
            for (int rowI = 0; rowI < 9; rowI++) {
                if (rowI != rowIndex) {
                    removePossibleNumber(rowI, colIndex, number);
                }
            }
            // remove possibilities in the same subbox
            {
                final int subboxRowIndex = rowIndex - rowIndex % 3;
                final int subboxColIndex = colIndex - colIndex % 3;
                for (int rowI = subboxRowIndex; rowI < subboxRowIndex + 3; rowI++) {
                    for (int colI = subboxColIndex; colI < subboxColIndex + 3; colI++) {
                        if (rowI == rowIndex && colI == colIndex) {
                            continue;
                        }
                        removePossibleNumber(rowI, colI, number);
                    }
                }
            }
        }

        public void removePossibleNumber(final int rowIndex, final int colIndex, final int number) {
            // System.out.println("removePossibleNumber(" + rowIndex + ", " + colIndex + ", " + number + ");");
            final Cell cell = getCell(rowIndex, colIndex);
            final boolean isDeterministicBefore = cell.isDeterministic();
            cell.removePossibleNumber(number);
            if (!isDeterministicBefore && cell.isDeterministic()) {
                setNumber(rowIndex, colIndex, cell.getNumber());
            }
        }

        public boolean isPossible() {
            for (final Cell[] row : rows) {
                for (final Cell cell : row) {
                    if (!cell.isPossible()) {
                        return false;
                    }
                }
            }
            return true;
        }

        public boolean isSolved() {
            for (final Cell[] row : rows) {
                for (final Cell cell : row) {
                    if (!cell.isDeterministic()) {
                        return false;
                    }
                }
            }
            return true;
        }

        public void solve() {
            if (isSolved() || !isPossible()) {
                return;
            }
            int minPossibilitySize = 9;
            Cell minPossibleCell = null;
            int minPossibleCellRow = 9;
            int minPossibleCellCol = 9;
            for (int rowIndex = 0; rowIndex < 9; rowIndex++) {
                for (int colIndex = 0; colIndex < 9; colIndex++) {
                    final Cell cell = getCell(rowIndex, colIndex);
                    if (cell.isDeterministic()) {
                        continue;
                    }
                    final int possibilitySize = cell.getPossibilitySize();
                    if (possibilitySize < minPossibilitySize) {
                        minPossibleCell = cell;
                        minPossibleCellRow = rowIndex;
                        minPossibleCellCol = colIndex;
                        minPossibilitySize = possibilitySize;
                    }
                }
            }
            if (minPossibleCell != null) {
                final Set<Integer> possibilities = minPossibleCell.getPossibilities();
                for (final int possibleNumber : possibilities) {
                    final Board trialBoard = new Board(this);
                    trialBoard.setNumber(minPossibleCellRow, minPossibleCellCol, possibleNumber);
                    if (!trialBoard.isPossible()) {
                        continue;
                    }
                    trialBoard.solve();
                    if (trialBoard.isSolved()) {
                        this.set(trialBoard);
                    }
                }
            }
        }
    }
}