

NAME=redescomp

build: dissector.o
	g++ -shared -o $(NAME).so dissector.o

dissector.o: dissector.cpp
	g++ -fPIC -I/usr/include/wireshark -I/usr/include/wireshark/epan -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -c dissector.cpp -o dissector.o

install: build
	cp -v $(NAME).so /usr/lib/x86_64-linux-gnu/wireshark/plugins/3.2/epan/
#	mkdir -p ~/.wireshark/plugins/
#	cp -v $(NAME).so ~/.wireshark/plugins/

clean:
	rm dissector.o
	rm $(NAME).so


