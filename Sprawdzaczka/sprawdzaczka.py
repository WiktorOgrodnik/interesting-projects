#!/bin/python

import os 
import sys

TESTS = 50

def create_test(level):
    # Place tests generator here

    return 1

def generate(cmd, system):
    for i in range(TESTS):

        with open('testy/test{}.in'.format(i), 'w') as f:
            f.write('{}'.format(create_test(i)))

        if system == 'linux':
            output = os.popen("cat testy/test{}.in | {}".format(i, cmd)).read()
        elif system == 'windows':
            output = os.popen("type .\\testy\\test{}.in | {}".format(i, cmd)).read()
        
        with open('testy/test{}.out'.format(i), 'w') as f:
            f.write(str(output))

def validate(cmd, system):
    for i in range(TESTS):
        
        if system == 'linux':
            output = os.popen("cat testy/test{}.in | {}".format(i, cmd)).read()
        elif system == 'windows':
            output = os.popen("type .\\testy\\test{}.in | {}".format(i, cmd)).read()

        with open('testy/test{}.out'.format(i), 'r') as f:
            data = f.read()
            if str(output) == str(data):
                print("Test {}: OK".format(i))
            else:
                print("Wrong answer for test {}! Expected: {}, returned: {}".format(i, data, output))

n = len(sys.argv)

if (n == 3):
    mode = sys.argv[1]
    cmd = sys.argv[2]
    if mode == 'test':
        validate(cmd, 'linux')
    elif mode == 'wintest':
        validate(cmd, 'windows')
    elif mode == 'gen':
        generate(cmd, 'linux')
    elif mode == 'wingen':
        generate(cmd, 'windows')