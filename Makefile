exec:
	g++ main.cpp -fopenmp -o lev
	./lev Egeo "Frans Hals" > ans.txt
input:
	g++ input.cpp -fopenmp -o input -std=c++11
	./input
	rm input
lev:
	g++ MODEL/lvstn.cpp -fopenmp -o MODEL/lvstn -std=c++11
	./MODEL/lvstn < in.txt
levF:
	g++ MODEL/lvstnFor.cpp -fopenmp -o MODEL/lvstnFor -std=c++11
	./MODEL/lvstnFor 
levS:
	g++ MODEL/lvstn.cpp -o MODEL/lvstn -std=c++11
	./MODEL/lvstn
levD:
	g++ MODEL/lvstnFor.cpp -fopenmp -o MODEL/lvstnFor -std=c++11
	./MODEL/lvstnFor