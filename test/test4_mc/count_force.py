files = ['test4.log', 'test4_orca.log']

if __name__ == '__main__':
    for file in files:
        with open(file) as f:
            count_force = 0
            count_path = 0
            for line in f.readlines():
                if 'NUMBER OF FORCE CALCULATIONS' in line:
                    count_force += int(line.split(':')[1].strip())
                    count_path += 1

        print('file: {}, path: {}, force: {}'.format(file, count_path, count_force)) 
