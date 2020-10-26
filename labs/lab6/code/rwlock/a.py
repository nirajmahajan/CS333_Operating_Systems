import os

os.system("g++ test-reader-pref.cpp rwlock-reader-pref.cpp -o rwlock-reader-pref -lpthread")
for i in range(20):
	for j in range(20):
		print(i,j)
		os.system("./rwlock-reader-pref {} {}".format(i,j))

os.system("g++ test-writer-pref.cpp rwlock-writer-pref.cpp -o rwlock-writer-pref -lpthread")
for i in range(20):
	for j in range(20):
		print(i,j)
		os.system("./rwlock-writer-pref {} {}".format(i,j))