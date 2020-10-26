import os
import random

os.system('g++ -w test-reader-pref.cpp rwlock-reader-pref.cpp -o rwlock-reader-pref -lpthread')
os.system('g++ -w test-writer-pref.cpp rwlock-writer-pref.cpp -o rwlock-writer-pref -lpthread')
random.seed(180050067)

outfile = 'output.txt'
MAX_VAL = 10
NUM_RAND_TEST = 10

os.system(f'rm {outfile} > /dev/null 2>&1')

for a in range(1, MAX_VAL + 1):
	for b in range(1, MAX_VAL + 1):
		os.system(f'printf "\nTestcase: {a} {b}\n" >> {outfile}')
		os.system(f'printf "reader_pref : " >> {outfile}')
		os.system(f'./rwlock-reader-pref {a} {b} >> {outfile}')
		os.system(f'printf "writer_pref : " >> {outfile}')
		os.system(f'./rwlock-writer-pref {a} {b} >> {outfile}')

for i in range(NUM_RAND_TEST):
	a = random.randint(1, 100)
	b = random.randint(1, 100)
	os.system(f'printf "\nTestcase: {a} {b}\n" >> {outfile}')
	os.system(f'printf "reader_pref : " >> {outfile}')
	os.system(f'./rwlock-reader-pref {a} {b} >> {outfile}')
	os.system(f'printf "writer_pref : " >> {outfile}')
	os.system(f'./rwlock-writer-pref {a} {b} >> {outfile}')
