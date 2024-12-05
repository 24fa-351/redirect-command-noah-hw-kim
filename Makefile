hello:
	echo "Hello, World!"

redirect:
	gcc -o redirect redirect.c

run:
	./redirect

clean:
	rm -f redirect