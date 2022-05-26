all: libplenja terminal browser pint

libplenja:
	make -C libplenja

terminal:
	sh -c "cd plenjaterminal && ./compile.sh"

browser:
	sh -c "cd plenjabrowser && ./compile.sh"

pint:
	make -C pint pint

install_terminal:
	make -C plenjaterminal install

install_pint:
	make -C pint install
