line1 = input()
line2 = input()

count = 0

for index, base_char in enumerate(line1):
    if line1[index] != line2[index]:
        count++

print(count)
