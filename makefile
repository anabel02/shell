.PHONY: dev
dev:
	mkdir -p build && gcc -o build/lsh src/main.c src/list.c src/execute.c src/utils.c src/builtin.c src/glist.c src/help.c && build/lsh