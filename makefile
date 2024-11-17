CXX = g++
CXXFLAGS = -std=c++17

SRC_DIR = src
OBJ_DIR = $(SRC_DIR)/obj
INC_DIR = $(SRC_DIR)/include


OUT_MAIN = warehouseManager.out
OUT_STORE = store.out
OUT_PART = part.out

VPATH = $(SRC_DIR)


all: $(OBJ_DIR) $(OUT_MAIN) $(OUT_STORE) $(OUT_PART)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OUT_MAIN): $(OBJ_DIR)/warehouseManager.o $(OBJ_DIR)/logger.o $(OBJ_DIR)/csv.o $(OBJ_DIR)/utils.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OUT_STORE): $(OBJ_DIR)/store.o $(OBJ_DIR)/logger.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/csv.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OUT_PART): $(OBJ_DIR)/part.o $(OBJ_DIR)/logger.o $(OBJ_DIR)/utils.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/warehouseManager.o: $(SRC_DIR)/main.cpp $(INC_DIR)/logger.hpp $(INC_DIR)/csv.hpp $(INC_DIR)/utils.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/store.o: $(SRC_DIR)/store.cpp $(INC_DIR)/logger.hpp $(INC_DIR)/utils.hpp $(INC_DIR)/csv.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/part.o: $(SRC_DIR)/part.cpp $(INC_DIR)/logger.hpp $(INC_DIR)/utils.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/logger.o: $(SRC_DIR)/logger.cpp $(INC_DIR)/logger.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/csv.o: $(SRC_DIR)/csv.cpp $(INC_DIR)/csv.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/utils.o: $(SRC_DIR)/utils.cpp $(INC_DIR)/utils.hpp $(INC_DIR)/logger.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: all clean

clean:
	rm -f $(OBJ_DIR)/*.o ./*.out 
	rm -rf namedPipes