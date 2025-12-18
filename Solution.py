from typing import Self

class Solution:
    def solveSudoku(self, board: List[List[str]]) -> None:
        """
        Do not return anything, modify board in-place instead.
        """
        b: Board = Board(charBoard=board)
        b.solve()
        b.print(board)


class Cell:

    def __init__(self):
        self.possibilities: set[int] = {1, 2, 3, 4, 5, 6, 7, 8, 9}

    def set(self, anotherCell: "Cell") -> None:
        self.possibilities = set()
        self.possibilities.update(anotherCell.possibilities)

    def isPossible(self) -> bool:
        return len(self.possibilities) != 0

    def isDeterministic(self) -> bool:
        return len(self.possibilities) == 1

    def getPossibilitySize(self) -> int:
        return len(self.possibilities)

    def getPossibilities(self) -> set:
        return self.possibilities.copy()

    def removePossibleNumber(self, num: int) -> None:
        self.possibilities.discard(num)

    def setNumber(self, num: int) -> None:
        self.possibilities = {num}

    def getNumber(self) -> int:
        if (self.isDeterministic()):
            return next(iter(self.possibilities))
        else:
            return 0


class Board:
    rows: list[list[Cell]]

    def __init__(self, *, charBoard: list[list[str]] | None = None, anotherBoard: Self | None = None):
        self.rows: list[list[Cell]] = []

        for rowIndex in range(9):
            row: list[Cell] = []
            for colIndex in range(9):
                row.append(Cell())
            self.rows.append(row)

        if charBoard is not None:
            for rowIndex in range(9):
                for colIndex in range(9):
                    c: str = charBoard[rowIndex][colIndex]
                    if c != ".":
                        self.setNumber(rowIndex, colIndex, int(c))
        elif anotherBoard is not None:
            self.set(anotherBoard=anotherBoard)

    def set(self, anotherBoard: "Board") -> None:
        for rowIndex in range(9):
            for colIndex in range(9):
                thisCell: Cell = self.getCell(rowIndex, colIndex)
                anotherCell: Cell = anotherBoard.getCell(rowIndex, colIndex)
                thisCell.set(anotherCell)

    def print(self, destBoard: list[list[str]]) -> None:
        for rowIndex in range(9):
            for colIndex in range(9):
                destBoard[rowIndex][colIndex] = str(self.getCell(rowIndex, colIndex).getNumber() + int('0'))

    def getCell(self, rowIndex: int, colIndex: int) -> Cell:
        return self.rows[rowIndex][colIndex]

    def setNumber(self, rowIndex: int, colIndex: int, number: int) -> None:
        self.getCell(rowIndex, colIndex).setNumber(number)
        # remove possibilities in the same row
        for colI in range(9):
            if colI != colIndex:
                self.removePossibleNumber(rowIndex, colI, number)
        # remove possibilities in the same column
        for rowI in range(9):
            if rowI != rowIndex:
                self.removePossibleNumber(rowI, colIndex, number)
        # remove possibilities in the same subbox
        subboxRowIndex: int = rowIndex - rowIndex % 3
        subboxColIndex: int = colIndex - colIndex % 3
        for rowI in range(subboxRowIndex, subboxRowIndex + 3):
            for colI in range(subboxColIndex, subboxColIndex + 3):
                if (rowI == rowIndex) and (colI == colIndex):
                    continue
                self.removePossibleNumber(rowI, colI, number)

    def removePossibleNumber(self, rowIndex: int, colIndex: int, number: int) -> None:
        cell: Cell = self.getCell(rowIndex, colIndex)
        isDeterministicBefore: bool = cell.isDeterministic()
        cell.removePossibleNumber(number)
        if not isDeterministicBefore and cell.isDeterministic():
            self.setNumber(rowIndex, colIndex, cell.getNumber())

    def isPossible(self) -> bool:
        for row in self.rows:
            for cell in row:
                if not cell.isPossible():
                    return False
        return True

    def isSolved(self) -> bool:
        for row in self.rows:
            for cell in row:
                if not cell.isDeterministic():
                    return False
        return True

    def solve(self) -> None:
        if self.isSolved() or not self.isPossible():
            return
        minPossibilitySize: int = 9
        minPossibleCell: Cell | None = None
        minPossibleCellRow: int = 9
        minPossibleCellCol: int = 9
        for rowIndex in range(9):
            for colIndex in range(9):
                cell: Cell = self.getCell(rowIndex, colIndex)
                if cell.isDeterministic():
                    continue
                possibilitySize: int = cell.getPossibilitySize()
                if possibilitySize < minPossibilitySize:
                    minPossibleCell = cell
                    minPossibleCellRow = rowIndex
                    minPossibleCellCol = colIndex
                    minPossibilitySize = possibilitySize
        if minPossibleCell is not None:
            possibilities: set[int] = minPossibleCell.getPossibilities()
            for possibleNumber in possibilities:
                trialBoard: Board = Board(anotherBoard=self)
                trialBoard.setNumber(minPossibleCellRow, minPossibleCellCol, possibleNumber)
                if not trialBoard.isPossible():
                    continue
                trialBoard.solve()
                if trialBoard.isSolved():
                    self.set(trialBoard)
