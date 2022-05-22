all: terminal browser

terminal:
	sh -c "cd plenjaterminal && ./compile.sh"

browser:
	sh -c "cd plenjabrowser && ./compile.sh"
