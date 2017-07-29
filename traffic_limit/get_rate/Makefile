UTILS_PATH := $(HOME)/Documents/Practices/C/utils
INC_PATH := $(UTILS_PATH)

SRC := main.c $(UTILS_PATH)/alarm.c $(UTILS_PATH)/read_file.c $(UTILS_PATH)/rw_all.c

getrate: $(SRC)
	$(CC) -I$(INC_PATH) $(SRC) -o $@
