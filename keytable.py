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
    ans = reversed(sorted(freqs.keys(), key=lambda x: freqs[x]))
    return ans


if __name__ == '__main__':
    freqs = frequencies()
    chars_by_freq = [x for x in sort_by_freq(freqs) if curses.ascii.isprint(x)]
    chars_by_freq = ['KEY_RETURN',
           'KEY_ESC',
           'KEY_TAB',
           'KEY_BACKSPACE',
           'KEY_SPACE', ] + chars_by_freq
    cpp_code = []
    tex_code = []
    presses = 1
    btns = all_combinations(8, presses)
    for c in chars_by_freq:
        if len(btns) == 0:
            presses = presses + 1
            btns = all_combinations(8, presses)
        cpp_code.append('regular_scancodes[%d] = \'%s\';' % (btns[0], c))
        if c != '|':
            tex_code.append('\\verb|%s| \dispbyte{%d}\\\\' % (c, btns[0]))
        else:
            tex_code.append('\\verb#%s# \dispbyte{%d}\\\\' % (c, btns[0]))
        btns = btns[1:]
    print "\n".join(cpp_code)
    print "\n".join(tex_code)
