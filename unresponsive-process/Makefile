
APP=unresponsive_process
SOURCE=src/unresponsive_process.cpp
CC=g++
CPPFLAGS=-std=c++11
OUTDIR=bin

all: $(APP)

outdir:
	mkdir -p $(OUTDIR)

$(APP): $(SOURCE) outdir
	$(CC) $(CPPFLAGS) $(SOURCE) -o $(OUTDIR)/$(APP)

clean:
	rm -f $(OUTDIR)/$(APP)

dist-clean:
	rm -rf $(OUTDIR)
