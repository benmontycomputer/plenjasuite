all: terminal browser pint

terminal:
	sh -c "cd plenjaterminal && ./compile.sh"

browser:
	sh -c "cd plenjabrowser && ./compile.sh"

pint:
	make -C pint pint

install_terminal:
	make -C plenjaterminal install
