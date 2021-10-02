all:
	echo "Clean"
	rm -rf pwdtest
	rm -rf hackpwdtest
	echo "Make start"
	gcc -shared -o hackpwdtest.so hackpwdtest.c -fPIC
	gcc -shared -o ldpreload.so ldpreload.c -fPIC
	gcc -shared -o hackcontrol.so hackcontrol.c -fPIC -D_GNU_SOURCE -ldl
	gcc -o pwdtest pwdtest.c
	gcc -o control -g control.c 

clean:
	rm -rf *.so
	rm -rf *.o
	rm -rf control
	rm -rf pwdtest


