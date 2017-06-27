YACC := yacc

SRC := translate.c regex.c bitmap.c symbol_table.c
INC := translate.h regex.h bitmap.h symbol_table.h

BM_TST_SRC := bitmap.c bitmap_tst.c
BM_TST_INC := bitmap.h common.h

YACC_SRC := translate.y

CFLAGS += -Wall -Werror -g

default : regx

regx : $(SRC) $(INC)
	$(CC) $(CFLAGS) $(SRC) -o $@

translate.c : $(YACC_SRC)
	$(YACC) $^ -o $@

bitmap_tst : $(BM_TST_SRC) $(BM_TST_INC)
	$(CC) $(CFLAGS) $(BM_TST_SRC) -o $@
