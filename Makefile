
#======================================
# Projekt neve
#======================================
PROJECT_NAME := Teszt

#======================================
# Fordit
#======================================
CC := g++

#======================================
# Konyvtarak
#======================================
SRC_DIR := .
LIB_DIR += ./Libraries

BUILD_DIR := ./Build
OBJ_DIR := $(BUILD_DIR)/Obj
FULL_PROJ := ./$(BUILD_DIR)/$(PROJECT_NAME)

#======================================
# Forras fajlok
#======================================
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)

#======================================
# Object fajlok
#======================================
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

#======================================
# Projekt specifikus include konyvtarak
#======================================
INC := -I./Includes

HEADERS := $(wildcard ./Includes/*.hpp)


#======================================
# Libek
#======================================

#======================================
# Optimalizacio
#======================================
OPT := -O0

#======================================
# Flag-ek
#======================================
LDFLAGS := 
CPPFLAGS := -ggdb -Wall $(OPT)
CXXFLAGS :=


#======================================
# Op. rendszer specifikus utasitasok
#======================================
RM := -rm -r
MKRIR := mkdir

#======================================
# Kiirasok
#======================================
MSG_ERRORS_NONE = Errors: none
MSG_CLEANING = Cleaning project
MSG_LINKING = Linking:
MSG_COMPILING = Compiling:

.SILENT :

#======================================
# Forditas
#======================================
all: directories program end
script:
	$(shell rm -f Build/Script.zip>/dev/null)
	$(shell rm -f Build/Script.b64>/dev/null)
	$(shell zip -r Build/Script.zip Script ShuttleStart.sh >/dev/null)
	$(shell base64 Build/Script.zip > Build/Script.b64)
	$(shell rm -f Build/Script.zip>/dev/null)

program: $(OBJ_FILES)
	@echo
	@echo $(MSG_LINKING) $(PROJECT_NAME)
	$(CC) $(LDFLAGS) -o $(FULL_PROJ) $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo $(MSG_COMPILING) $< "=>" $@
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(INC) -c -o $@ $<

b64:
	$(shell base64 ./ShuttlePi > Build/ShuttlePi.b64)

directories: $(BUILD_DIR) $(OBJ_DIR)


begin:
	@echo
	@echo $(MSG_BEGIN)

end:
	@echo
	@echo "====================================="
	@echo $(MSG_ERRORS_NONE)
	@echo "====================================="
	@echo


$(BUILD_DIR):
	$(MKRIR) "$(BUILD_DIR)"

$(OBJ_DIR):
	$(MKRIR) "$(OBJ_DIR)"


#======================================
# Forditas + futtatas
#======================================
run: all
	@echo "====================================="
	@echo Run: $(PROJECT_NAME)
	@echo "====================================="
	@echo
	@ $(FULL_PROJ)

#======================================
# Forditas + futtatas (sudo)
#======================================
srun: all
	@echo "====================================="
	@echo Sudo run: $(PROJECT_NAME)
	@echo "====================================="
	@echo
	@ sudo $(FULL_PROJ)

#======================================
# Clean
#======================================
clean:
	@echo $(MSG_CLEANING)
	@echo $(RM) "$(BUILD_DIR)"
	$(RM) "$(FULL_PROJ)"
	$(RM) "$(BUILD_DIR)"
