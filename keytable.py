import sys

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


if __name__ == '__main__':
    chars_by_freq = [x.strip() for x in open(sys.argv[1]).readlines()]
    presses = 1
    btns = all_combinations(6, presses)
    for c in chars_by_freq:
        if len(btns) == 0:
            presses = presses + 1
            btns = all_combinations(6, presses)
        print 'regular_scancodes[%d] = \'%s\';' % (btns[0], c)
        btns = btns[1:]
