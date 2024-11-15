CXX = g++
CXXFLAGS = -std=c++17

SRC_DIR = src
OBJ_DIR = obj


OUT_CAS = warehouseManager.out
OUT_STORE = store.out
OUT_PROFIT = profit.out

VPATH = $(SRC_DIR)


all: $(OUT_CAS)

$(OUT_CAS): $(OBJ_DIR)/warehouseManager.o $(OBJ_DIR)/logger.o $(OBJ_DIR)/strops.o $(OBJ_DIR)/csv.o
	$(CXX) $(CXXFLAGS) $^ -o $@

# $(OUT_STORE): $(OBJ_DIR)/store.o $(OBJ_DIR)/logger.o $(OBJ_DIR)/strops.o
# 	$(CXX) $(CXXFLAGS) $^ -o $@

# $(OUT_PROFIT): $(OBJ_DIR)/bill.o $(OBJ_DIR)/logger.o $(OBJ_DIR)/strops.o $(OBJ_DIR)/csv.o
# 	$(CXX) $(CXXFLAGS) $^ -o $@



$(OBJ_DIR)/warehouseManager.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/logger.hpp $(SRC_DIR)/ansi-color.hpp \
 								 $(SRC_DIR)/strops.hpp $(SRC_DIR)/consts.hpp $(SRC_DIR)/utils.hpp $(SRC_DIR)/csv.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# $(OBJ_DIR)/store.o: $(SRC_DIR)/store.cpp $(SRC_DIR)/logger.hpp $(SRC_DIR)/ansi-color.hpp \
# 								 $(SRC_DIR)/strops.hpp $(SRC_DIR)/consts.hpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# $(OBJ_DIR)/bill.o: $(SRC_DIR)/bill.cpp $(SRC_DIR)/logger.hpp $(SRC_DIR)/csv.hpp $(SRC_DIR)/consts.hpp \
# 								$(SRC_DIR)/strops.hpp $(SRC_DIR)/utils.hpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/logger.o: $(SRC_DIR)/logger.cpp $(SRC_DIR)/logger.hpp $(SRC_DIR)/ansi-color.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/csv.o: $(SRC_DIR)/csv.cpp $(SRC_DIR)/csv.hpp $(SRC_DIR)/ansi-color.hpp $(SRC_DIR)/consts.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/strops.o: $(SRC_DIR)/strops.cpp $(SRC_DIR)/strops.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


.PHONY: all clean

clean:
	rm -f $(OBJ_DIR)/*.o ./*.out 	
# namedPipes/*
