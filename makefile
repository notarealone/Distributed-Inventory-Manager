CXX = g++
CXXFLAGS = -std=c++17

SRC_DIR = src
OBJ_DIR = obj


OUT_CAS = warehouseManager.out
OUT_STORE = store.out
OUT_PART = part.out

VPATH = $(SRC_DIR)


all: $(OUT_CAS) $(OUT_STORE) $(OUT_PART)

$(OUT_CAS): $(OBJ_DIR)/warehouseManager.o $(OBJ_DIR)/logger.o $(OBJ_DIR)/strOperations.o $(OBJ_DIR)/csv.o $(OBJ_DIR)/utils.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OUT_STORE): $(OBJ_DIR)/store.o $(OBJ_DIR)/logger.o $(OBJ_DIR)/strOperations.o $(OBJ_DIR)/csv.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OUT_PART): $(OBJ_DIR)/part.o $(OBJ_DIR)/logger.o $(OBJ_DIR)/strOperations.o $(OBJ_DIR)/utils.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/warehouseManager.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/logger.hpp $(SRC_DIR)/strOperations.hpp $(SRC_DIR)/csv.hpp $(SRC_DIR)/utils.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/store.o: $(SRC_DIR)/store.cpp $(SRC_DIR)/logger.hpp $(SRC_DIR)/strOperations.hpp $(SRC_DIR)/csv.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/part.o: $(SRC_DIR)/part.cpp $(SRC_DIR)/logger.hpp $(SRC_DIR)/strOperations.hpp $(SRC_DIR)/utils.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/logger.o: $(SRC_DIR)/logger.cpp $(SRC_DIR)/logger.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/csv.o: $(SRC_DIR)/csv.cpp $(SRC_DIR)/csv.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/strOperations.o: $(SRC_DIR)/strOperations.cpp $(SRC_DIR)/strOperations.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/utils.o: $(SRC_DIR)/utils.cpp $(SRC_DIR)/utils.hpp $(SRC_DIR)/logger.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: all clean

clean:
	rm -f $(OBJ_DIR)/*.o ./*.out 
	rm -rf namedPipes