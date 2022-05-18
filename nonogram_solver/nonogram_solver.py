from copy import deepcopy
from queue import SimpleQueue

def gen_row(w, s):
    def gen_seg(o, sp):
        if not o:
            return [[0] * sp]
        return [[0] * x + o[0] + tail
                for x in range(1, sp - len(o) + 2)
                for tail in gen_seg(o[1:], sp - x)]
 
    return [x[1:] for x in gen_seg([[1] * i for i in s], w + 1 - sum(s))]


def ac3(wiezy, n, m):

	rows = wiezy[:n]
	cols = wiezy[n:]

	def gen_cor(wiezy: list[list[tuple]]):

		def gen_cor_row(wiezy: list[tuple]):
			tab = list(wiezy[0])
			size = len(tab)
			for row in wiezy[1:]:
				tab[:] = [tab[i] if tab[i] == row[i] else 2 for i in range(size)]
			return tab

		return [gen_cor_row(i) for i in wiezy]

	def resolve_cols_rows(correct1, correct2, queue1: SimpleQueue, queue2: SimpleQueue, set1: list[tuple], size1: int, size2: int):

		def row_and_col_matching(correct, line: list[tuple], idx, line_size):
			size = len(line)
			line[:] = [i for i in line if all([(i[j] == correct[j][idx] or correct[j][idx] == 2) for j in range(line_size)])]
			return size == len(line)

		while not queue1.empty():
			nr = queue1.get()
			line = set1[nr]
			if not row_and_col_matching(correct1, line, nr, size1) and len(line) == 0:
				return False

		correct2[:] = gen_cor(set1)
		for i in range(size1):
			if correct1[i] != [correct2[j][i] for j in range(size2)]:
				queue2.put(i)

		return True

	queue_cols = SimpleQueue()
	queue_rows = SimpleQueue()

	for i in range(m):
		queue_cols.put(i)

	while not queue_cols.empty():

		correct_cols = gen_cor(rows)
		correct_rows = []

		if not resolve_cols_rows(correct_cols, correct_rows, queue_cols, queue_rows, cols, n, m):
			return False
		
		if not resolve_cols_rows(correct_rows, correct_cols, queue_rows, queue_cols, rows, m, n):
			return False

	return True

def puzzle_solver(wiezy: list[list[tuple]], n, m, lvl):

	if lvl == n:
		save_puzzle(n, m, [x[0] for x in wiezy[:n]])
		quit()

	if not ac3(wiezy, n, m):
		return

	for i in wiezy[lvl]:
		puzzle_solver(deepcopy(wiezy[:lvl]) + [[i]] + deepcopy(wiezy[lvl + 1:]), n, m, lvl + 1)

def save_puzzle(n, m, tab):

	letters = '.#'
	with open('output.txt', 'w') as f:
		for i in range(n):
			for j in range(m):
				f.write(letters[tab[i][j]])
			f.write('\n')

def print_puzzle(n, m, tab):

	letters = '.#'
	for i in range(n):
		for j in range(m):
			print(letters[tab[i][j]], end='')
		print('\n', end='')

with open('input.txt', 'r') as f:
	n, m = [int(x) for x in next(f).split()]
	tabk = [[int(x) for x in next(f).split()] for i in range(n + m)]

wiezy = [[tuple(x) for x in gen_row(n if i >= n else m, tabk[i])] for i in range(n + m)]

puzzle_solver(wiezy, n, m, 0)
print('No answer!')