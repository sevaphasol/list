CC = gcc
CFLAGS = -I include -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security 	\
	-Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor \
	-Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing 			\
	-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -fexceptions -pipe

LDFLAGS =

SOURCES_DIR   = src
OBJECTS_DIR   = bin
BUILD_DIR     = build
LOGS_DIR 	  = logs
DOT_FILES_DIR = $(LOGS_DIR)/dot_files
IMAGES_DIR    = $(LOGS_DIR)/images

EXECUTABLE = list
EXECUTABLE_PATH = $(BUILD_DIR)/$(EXECUTABLE)

SOURCE_FILES = $(wildcard $(SOURCES_DIR)/*.cpp)
OBJECT_FILES = $(subst $(SOURCES_DIR), $(OBJECTS_DIR), $(SOURCE_FILES:.cpp=.o))

all: $(EXECUTABLE_PATH)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJECTS_DIR):
	mkdir -p $(OBJECTS_DIR)
	mkdir -p $(LOGS_DIR)
	mkdir -p $(DOT_FILES_DIR)
	mkdir -p $(IMAGES_DIR)

$(EXECUTABLE_PATH): $(OBJECT_FILES) $(BUILD_DIR)
	$(CC) $(LDFLAGS) $(OBJECT_FILES) -o $@

$(OBJECTS_DIR)/%.o: $(SOURCES_DIR)/%.cpp $(OBJECTS_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

clean_logs:
	rm $(DOT_FILES_DIR)/*.dot $(IMAGES_DIR)/*.png

clean:
	rm -fr $(OBJECTS_DIR) $(BUILD_DIR) $(LOGS_DIR)
