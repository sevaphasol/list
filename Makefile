CC = g++
CFLAGS = -I include -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security 		 \
		 -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor \
		 -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing 		 \
		 -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -fexceptions -pipe 						 \

LIST_LDFLAGS 	     =
TEST_LDFLAGS         = -lgtest -lgmock -pthread

SOURCES_DIR          = src
OBJECTS_DIR          = bin
BUILD_DIR            = build
LOGS_DIR 	         = logs
DOT_FILES_DIR        = $(LOGS_DIR)/dot_files
IMAGES_DIR           = $(LOGS_DIR)/images
SOURCE_FILES         = $(wildcard $(SOURCES_DIR)/*.cpp)
OBJECT_FILES         = $(subst $(SOURCES_DIR), $(OBJECTS_DIR), $(SOURCE_FILES:.cpp=.o))
EXECUTABLE           = list
EXECUTABLE_PATH      = $(BUILD_DIR)/$(EXECUTABLE)

TEST_DIR 		     = unit_tests
TEST_SOURCE_DIR      = $(TEST_DIR)/src
TEST_OBJECT_DIR      = $(TEST_DIR)/bin
TEST_BUILD_DIR       = $(TEST_DIR)/build
TEST_SOURCE_FILES    = $(wildcard $(TEST_SOURCE_DIR)/*.cpp)
TEST_OBJECT_FILES    = $(subst $(TEST_SOURCE_DIR), $(TEST_OBJECT_DIR), $(TEST_SOURCE_FILES:.cpp=.o)) \
					   $(filter-out $(OBJECTS_DIR)/main.o, $(OBJECT_FILES))							 \

TEST_EXECUTABLE      = test
TEST_EXECUTABLE_PATH = $(TEST_BUILD_DIR)/$(TEST_EXECUTABLE)

#############################

all: $(EXECUTABLE_PATH)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJECTS_DIR):
	mkdir -p $(OBJECTS_DIR)
	mkdir -p $(LOGS_DIR)
	mkdir -p $(DOT_FILES_DIR)
	mkdir -p $(IMAGES_DIR)

$(EXECUTABLE_PATH): $(OBJECT_FILES) $(BUILD_DIR)
	$(CC) $(LIST_LDFLAGS) $(OBJECT_FILES) -o $@

$(OBJECTS_DIR)/%.o: $(SOURCES_DIR)/%.cpp $(OBJECTS_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

#############################

unit_tests: $(TEST_EXECUTABLE_PATH)

$(TEST_BUILD_DIR):
	mkdir -p $(TEST_BUILD_DIR)

$(TEST_OBJECT_DIR):
	mkdir -p $(TEST_OBJECT_DIR)

$(TEST_EXECUTABLE_PATH): $(TEST_OBJECT_FILES) $(TEST_BUILD_DIR)
	echo $(TEST_OBJECT_FILES)
	$(CC) $(TEST_OBJECT_FILES) -o $@ $(TEST_LDFLAGS)

$(TEST_OBJECT_DIR)/%.o : $(TEST_SOURCE_DIR)/%.cpp $(TEST_OBJECT_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

#############################

clean_logs:
	rm $(DOT_FILES_DIR)/*.dot $(IMAGES_DIR)/*.png

#############################

clean_unit_tests:
	rm -fr $(TEST_OBJECT_DIR) $(TEST_BUILD_DIR)

#############################

clean:
	rm -fr $(OBJECTS_DIR) $(BUILD_DIR) $(LOGS_DIR)

