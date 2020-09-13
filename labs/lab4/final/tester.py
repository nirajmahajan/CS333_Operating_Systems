import os
import random

if not os.path.isfile('master-worker.c'):
	print('master-worker.c absent')
	os._exit(0)

random.seed(180050069)

MAX_VAL = 6
NUM_RAND_TEST = 100

os.system('rm 123ans.txt > /dev/null 2>&1')

for a in range(1,MAX_VAL):
	for b in range(1,MAX_VAL):
		for c in range(1,MAX_VAL):
			for d in range(1,MAX_VAL):
				os.system('echo "\nTestcase: {} {} {} {}" >> 123ans.txt'.format(a,b,c,d))
				os.system('./test-master-worker.sh {} {} {} {} >> 123ans.txt'. format(a,b,c,d))

for i in range(NUM_RAND_TEST):
	a = random.randint(1,10000)
	b = random.randint(1,10000)
	c = random.randint(1,10000)
	d = random.randint(1,10000)
	os.system('echo "\nTestcase: {} {} {} {} {}" >> 123ans.txt'.format(i,a,b,c,d))
	os.system('./test-master-worker.sh {} {} {} {} >> 123ans.txt'. format(a,b,c,d))

os.system('grep -c failed 123ans.txt >> analysis.txt')
with open('analysis.txt', 'r') as f:
	a = f.read()
	if 'failed' in a:
		print('Some testcases failed')
	else:
		print('All testcases passed')


# os.system('rm 123ans.txt > /dev/null 2>&1')
os.system('rm analysis.txt > /dev/null 2>&1')