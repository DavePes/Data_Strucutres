class Matrix:
    """Interface of a matrix.

    This class provides only the matrix size N and a method for swapping
    two items. The actual storage of the matrix in memory is provided by
    subclasses in testing code.
    """

    def __init__(self, N):
        self.N = N

    def swap(self, i1, j1, i2, j2):
        """Swap elements (i1,j1) and (i2,j2)."""

        # Overridden in subclasses
        raise NotImplementedError
    def transpose_and_swap(self, row1, col1, row2, col2, size1, size2):
        if size1 <= 1 or size2 <= 1:
            for i in range(size1):
                for j in range(size2):
                    self.swap(row1 + i, col1 + j, row2 + j, col2 + i)
        else:
            mid1 = size1 // 2
            mid2 = size2 // 2
            self.transpose_and_swap(row1, col1, row2, col2, mid1, mid2)
            self.transpose_and_swap(row1, col1 + mid2, row2 + mid2, col2, mid1, size2 - mid2)
            self.transpose_and_swap(row1 + mid1, col1, row2, col2 + mid1, size1 - mid1, mid2)
            self.transpose_and_swap(row1 + mid1, col1 + mid2, row2 + mid2, col2 + mid1, size1 - mid1, size2 - mid2)
    def transpose_recursive(self, row, col, size):
        if size <= 1:
            return
        else:
            mid = size // 2
            mid2 = size - mid
            T1 = self.transpose_recursive(row,col,mid)
            T2 = self.transpose_recursive(row+mid,col+mid,mid2)
            TS = self.transpose_and_swap(row,col+mid,row+mid,col,mid,mid2)
    def transpose(self):
        """Transpose the matrix."""
        self.transpose_recursive(0, 0, self.N)
