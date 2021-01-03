all: parse argtest

parse: parse.cpp
	g++-9 -o parse parse.cpp
	
argtest: argtest.cpp
	g++-9 -o argtest argtest.cpp
	echo "a b\\' c" \'d e
	./argtest "a b\\' c" \'d e
	echo "a b c" d e
	./argtest "a b c" d e
	echo "ab\"c" "\\" d
	./argtest "ab\"c" "\\" d
	echo a\\\b d"e f"g h
	./argtest a\\\b d"e f"g h
	echo a\\\"b c d
	./argtest a\\\"b c d
	echo a\\\\"b c" d e
	./argtest a\\\\"b c" d e
	echo a"b"" c d
	./argtest a"b"" c d

clean:
	rm parse argtest

 