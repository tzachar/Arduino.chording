import collections
import fileinput
import curses.ascii

def all_combinations(num_bits, pressed):
    if num_bits < pressed:
        return []
    if num_bits == 0:
        return []
    if pressed == 0:
        return [0]
    if num_bits == pressed:
        ans = 0
        for i in xrange(pressed):
            ans += 2 ** i
        return [ans]

    ans = all_combinations(num_bits - 1, pressed)
    tmp = all_combinations(num_bits - 1, pressed - 1)

    for i in tmp:
        ans.append(i + 2 ** (num_bits - 1))
    return ans


def frequencies():
    freqs = collections.defaultdict(lambda: 0)
    for line in fileinput.input():
        for c in line.lower():
            freqs[c] = freqs[c] + 1

    return freqs


def sort_by_freq(freqs):
    return reversed(sorted(freqs.keys(), key=lambda x: freqs[x]))


if __name__ == '__main__':
    freqs = frequencies()
    chars_by_freq = [x for x in sort_by_freq(freqs) if curses.ascii.isprint(x)]
    presses = 1
    btns = all_combinations(8, presses)
    for c in chars_by_freq:
        if len(btns) == 0:
            presses = presses + 1
            btns = all_combinations(8, presses)
        print 'regular_scancodes[%d] = \'%s\';' % (btns[0], c)
        btns = btns[1:]
