CFLAGS = -D _DEBUG -ggdb3 -std=c++20 -O3 -mavx2 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -pie -Wlarger-than=8192 -Wstack-usage=8192

SFML  = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
LOG_O = ../lib/logs/log.o
ALG_O = ../lib/algorithm/algorithm.o
#---------------------------------------------------------------------------
MALD     = maldenbrot
MALD_H   = $(MALD).h
MALD_CPP = $(MALD).cpp

FRAME     = maldenbrot_frame
FRAME_H   = $(FRAME).h
FRAME_CPP = $(FRAME).cpp

SET_H	  = $(MALD)_settings.h
#---------------------------------------------------------------------------
SOURCE    = $(MALD_CPP) $(FRAME_CPP)
HEADER    = $(MALD_H)   $(FRAME_H)   $(SET_H);
LIB       = $(LOG_O) $(ALG_O)

all: intrin separated all_in simple time

intrin:    main.cpp $(SOURCE) $(LIB) $(HEADER)
	g++    $< 	    $(SOURCE) $(LIB) $(CFLAGS) $(SFML) -o $@

separated: main.cpp $(SOURCE) $(LIB) $(HEADER)
	g++    $< 	    $(SOURCE) $(LIB) $(CFLAGS) $(SFML) -DCYCLE_SEPARATED -o $@

all_in:    main.cpp $(SOURCE) $(LIB) $(HEADER)
	g++    $<  		$(SOURCE) $(LIB) $(CFLAGS) $(SFML) -DCYCLE_ALL_IN    -o $@

simple:    main.cpp $(SOURCE) $(LIB) $(HEADER)
	g++    $< 		$(SOURCE) $(LIB) $(CFLAGS) $(SFML) -DSIMPLE			-o $@

time: maldenbrot_time.cpp $(SOURCE) $(LIB) $(HEADER)
	g++ $< 				  $(SOURCE) $(LIB) $(CFLAGS) $(SFML) -o $@
#---------------------------------------------------------------------------
clean:
	rm -f intrin
	rm -f separated
	rm -f all_in
	rm -f simple
	rm -f time

.PHONY: clean