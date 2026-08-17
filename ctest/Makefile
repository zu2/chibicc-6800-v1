CC_URL ?= https://github.com/hsjoihs/c-compiler.git
CC_REF ?= master
CC_WORK = .c-compiler

all:
	mkdir -p s out
	./test_cases.sh

fetch: $(CC_WORK)
	cp $(CC_WORK)/test_cases.sh .
	mkdir -p misc
	cp $(CC_WORK)/misc/supplement0.c misc/
	cp $(CC_WORK)/misc/supplement1.c misc/
	patch -p1 < test_cases.patch

$(CC_WORK):
	git clone --depth 1 --branch $(CC_REF) --filter=blob:none \
		--sparse $(CC_URL) $(CC_WORK)
	cd $(CC_WORK) && git sparse-checkout set --no-cone \
		/test_cases.sh /misc/supplement0.c /misc/supplement1.c

.PHONY: all fetch
