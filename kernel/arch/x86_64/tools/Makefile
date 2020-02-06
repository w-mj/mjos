
target := tools.o
toclean:= create_early_page early_page.S $(target)

build: $(target)

create_early_page: create_early_page.cpp
	g++ create_early_page.cpp -o create_early_page

early_page.S: create_early_page
	./create_early_page

$(target): early_page.S
	$(CC) -c $(CFLAGS) $< -o $@

clean: 
	rm -f $(toclean)
