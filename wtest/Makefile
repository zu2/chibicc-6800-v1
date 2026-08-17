ACWJ_URL ?= https://github.com/DoctorWkt/acwj.git
ACWJ_REF ?= master
ACWJ_SUBDIR = 64_6809_Target/tests
ACWJ_WORK = .acwj

all:
	./runtests

fetch: $(ACWJ_WORK)
	cp $(ACWJ_WORK)/$(ACWJ_SUBDIR)/input*.c .
	cp $(ACWJ_WORK)/$(ACWJ_SUBDIR)/out.input*.c .
	cp $(ACWJ_WORK)/$(ACWJ_SUBDIR)/err.input*.c .
	cp $(ACWJ_WORK)/$(ACWJ_SUBDIR)/runtests .
	git apply port.patch
	$(MAKE) regen-err

regen-err:
	for f in err.input*.c; do \
		i=`echo $$f | sed 's/^err\.//'`; \
		if [ ! -f $$i ]; then rm -f $$f; continue; fi; \
		../chibicc -o out $$i 2> $$f; \
		if [ -s $$f ]; then continue; fi; \
		rm -f $$f; \
		emu6800 6800 out.bin out.map > out.$$i; \
	done
	rm -f out out.bin out.map out.s

$(ACWJ_WORK):
	git clone --depth 1 --branch $(ACWJ_REF) --filter=blob:none \
		--sparse $(ACWJ_URL) $(ACWJ_WORK)
	cd $(ACWJ_WORK) && git sparse-checkout set $(ACWJ_SUBDIR)

.PHONY: all fetch regen-err
