#include <vector>

using std::vector;

class Solution {
public:
    void solveSudoku(vector<vector<char>>& board) {
        Board b(board);
        b.solve();
        b.print(board);
    }
private:
    class Cell {
    private:
        std::vector<int> possibilities;

    public:
        Cell() {
            for (int i = 1; i <= 9; i++) {
                possibilities.push_back(i);
            }
        }

        Cell(const Cell &anotherCell) {
            possibilities.insert(possibilities.end(), anotherCell.possibilities.begin(), anotherCell.possibilities.end());
        }

        void set(const Cell &anotherCell) {
            possibilities.clear();
            possibilities.insert(possibilities.end(), anotherCell.possibilities.begin(), anotherCell.possibilities.end());
        }

        bool isPossible() const {
            return !possibilities.empty();
        }

        bool isDeterministic() const {
            return possibilities.size() == 1;
        }

        int getPossibilitySize() const {
            return possibilities.size();
        }

        const std::vector<int> &getPossibilities() const {
            return possibilities;
        }

        void removePossibleNumber(int num) {
            possibilities.erase(std::remove(possibilities.begin(), possibilities.end(), num), possibilities.end());
        }

        void setNumber(int num) {
            possibilities.clear();
            possibilities.push_back(num);
        }

        int getNumber() const {
            if (isDeterministic()) {
                return possibilities[0];
            } else {
                return 0;
            }
        }
    };

    class Board {
    private:
        Cell rows [9][9];

    public:
        Board() {
            for (int rowIndex = 0; rowIndex < 9; rowIndex++) {
                for (int colIndex = 0; colIndex < 9; colIndex++) {
                    Cell &cell = rows[rowIndex][colIndex];
                }
            }
        }

        Board(vector<vector<char>>& readonlyBoard) : Board() {
            this->set(readonlyBoard);
        }

        Board(const Board &anotherBoard) : Board() {
            this->set(anotherBoard);
        }

        void set(vector<vector<char>>& readonlyBoard) {
            for (int rowIndex = 0; rowIndex < 9; rowIndex++) {
                for (int colIndex = 0; colIndex < 9; colIndex++) {
                    const char c = readonlyBoard[rowIndex][colIndex];
                    if (c != '.') {
                        setNumber(rowIndex, colIndex, (int)(c - '0'));
                    }
                }
            }
        }

        void set(const Board &anotherBoard) {
            for (int rowIndex = 0; rowIndex < 9; rowIndex++) {
                for (int colIndex = 0; colIndex < 9; colIndex++) {
                    Cell &thisCell = getCell(rowIndex, colIndex);
                    const Cell &anotherCell = anotherBoard.getCell(rowIndex, colIndex);
                    thisCell.set(anotherCell);
                }
            }
        }

        void print(vector<vector<char>>& destBoard) {
            for (int rowIndex = 0; rowIndex < 9; rowIndex++) {
                for (int colIndex = 0; colIndex < 9; colIndex++) {
                    destBoard[rowIndex][colIndex] = (char)(getCell(rowIndex, colIndex).getNumber() + '0');
                }
            }
        }

        Cell &getCell(const int rowIndex, const int colIndex) {
            return rows[rowIndex][colIndex];
        }

        const Cell &getCell(const int rowIndex, const int colIndex) const {
            return rows[rowIndex][colIndex];
        }

        void setNumber(const int rowIndex, const int colIndex, const int number) {
            // std::cout << ("setNumber(" + rowIndex + ", " + colIndex + ", " + number + ");") << std::endl;
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
                const int subboxRowIndex = rowIndex - rowIndex % 3;
                const int subboxColIndex = colIndex - colIndex % 3;
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

        void removePossibleNumber(const int rowIndex, const int colIndex, const int number) {
            // std::cout << ("removePossibleNumber(" + rowIndex + ", " + colIndex + ", " + number + ");") << std::endl;
            Cell &cell = getCell(rowIndex, colIndex);
            const bool isDeterministicBefore = cell.isDeterministic();
            cell.removePossibleNumber(number);
            if (!isDeterministicBefore && cell.isDeterministic()) {
                setNumber(rowIndex, colIndex, cell.getNumber());
            }
        }

        bool isPossible() const {
            for (int rowIndex = 0; rowIndex < 9; rowIndex++) {
                for (int colIndex = 0; colIndex < 9; colIndex++) {
                    const Cell &cell = getCell(rowIndex, colIndex);
                    if (!cell.isPossible()) {
                        return false;
                    }
                }
            }
            return true;
        }

        bool isSolved() const {
            for (int rowIndex = 0; rowIndex < 9; rowIndex++) {
                for (int colIndex = 0; colIndex < 9; colIndex++) {
                    const Cell &cell = getCell(rowIndex, colIndex);
                    if (!cell.isDeterministic()) {
                        return false;
                    }
                }
            }
            return true;
        }

        void solve() {
            if (isSolved() || !isPossible()) {
                return;
            }
            int minPossibilitySize = 9;
            Cell *minPossibleCell = nullptr;
            int minPossibleCellRow = 9;
            int minPossibleCellCol = 9;
            for (int rowIndex = 0; rowIndex < 9; rowIndex++) {
                for (int colIndex = 0; colIndex < 9; colIndex++) {
                    Cell &cell = getCell(rowIndex, colIndex);
                    if (cell.isDeterministic()) {
                        continue;
                    }
                    const int possibilitySize = cell.getPossibilitySize();
                    if (possibilitySize < minPossibilitySize) {
                        minPossibleCell = &cell;
                        minPossibleCellRow = rowIndex;
                        minPossibleCellCol = colIndex;
                        minPossibilitySize = possibilitySize;
                    }
                }
            }
            if (minPossibleCell != nullptr) {
                const std::vector<int> &possibilities = minPossibleCell->getPossibilities();
                for (const int possibleNumber : possibilities) {
                    Board trialBoard(*this);
                    trialBoard.setNumber(minPossibleCellRow, minPossibleCellCol, possibleNumber);
                    if (!trialBoard.isPossible()) {
                        continue;
                    }
                    trialBoard.solve();
                    if (trialBoard.isSolved()) {
                        this->set(trialBoard);
                    } else {
                    }
                }
            }
        }
    };
};
