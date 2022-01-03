project : webserver.c

webserver.c : server/webserver.c
	gcc -lc -pthread -g  -D_FORTIFY_SOURCE=0 -frecord-gcc-switches -fno-stack-protector -g  -o server/webserver -lpthread  -lnsl -lresolv -D_TS_ERRNO server/webserver.c
install :
	sudo /etc/init.d/fhttpd.init stop
	sudo cp server/webserver /usr/local/fhttpd/fhttpd
	sudo chmod 4755 /usr/local/fhttpd/fhttpd
	sudo /etc/init.d/fhttpd.init start

clean : 
	rm server/webserver
