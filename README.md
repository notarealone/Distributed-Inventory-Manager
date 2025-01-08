# Distributed Inventory Manager

## Overview

This project implements a distributed inventory management system using a Map-Reduce approach in C++. It simulates a food distribution company that needs to track the inventory of various products across multiple warehouses (simulated as CSV files) located in different cities. The system calculates the total quantity and monetary value of each product across all warehouses and computes the overall profit generated during an inventory period.

The project demonstrates inter-process communication (IPC) using both unnamed pipes (for parent-child communication) and named pipes (for communication between unrelated processes). It utilizes the `fork` and `exec` system calls to create and manage multiple processes representing warehouses and product types.

## Project Structure

The project consists of the following components:

*   **`main.cpp` :** The main program that initializes the system, creates warehouse and product processes, handles user input, and displays the final results.
*   **`store.cpp` :**  Represents a warehouse process. Each store process reads data from its corresponding CSV file, calculates product quantities and values within that warehouse, computes profit, and communicates with product processes and the main process.
*   **`part.cpp` :** Represents a product process. Each product process receives inventory data from multiple store processes via named pipes, aggregates the data, and sends the total quantity and value to the main process.
*   **`csv.cpp` :** A utility class for parsing CSV files.
*   **`utils.cpp` :** Provides helper functions like splitting strings and retrieving files from a directory.
*   **`logger.cpp` :** A simple logging class for debugging and displaying informational messages during process execution.
*   **`src/include/`:** Contains header files for constants, ANSI color codes, CSV, logger, and utility functions.
*   **`Makefile`:** Automates the compilation process.
*   **`stores/`:** A directory containing CSV files that represent the warehouses. This directory also contains a `Parts.csv` file that lists all available products.

## Key Features

*   **Map-Reduce Paradigm:** The project follows a simplified Map-Reduce model:
    *   **Map:** Store processes act as mappers, processing data from individual warehouse CSV files.
    *   **Reduce:** Product processes act as reducers, aggregating data from different store processes based on product type.
*   **Inter-Process Communication:**
    *   **Unnamed Pipes:** Used for communication between the main process and its child processes (store and product processes).
    *   **Named Pipes (FIFOs):** Used for communication between store processes and product processes.
*   **Process Management:** `fork` and `exec` are used to create and manage child processes.
*   **User Interaction:** The program prompts the user to select which products they want to track.
*   **Logging:** The `Logger` class provides detailed logs to track the execution flow, including process creation, communication, and potential errors.
*   **Error Handling:** The code includes basic error handling for file operations, pipe creation, and process execution.

## Compilation and Execution

1. **Clone the repository:**

    ```bash
    git clone <your_repository_url>
    cd Distributed-Inventory-Manager
    ```

2. **Compile the project using the provided Makefile:**

    ```bash
    make
    ```

3. **Run the program:**

    ```bash
    ./warehouseManager <path_to_stores_directory>
    ```

    *   Replace `<path_to_stores_directory>` with the actual path to the `stores` directory.

4. **Follow the on-screen prompts** to select the products you want to track.

## Dependencies

*   A C++ compiler (g++)
*   Make
*   A Linux environment (as the project uses Linux-specific system calls)

## Notes

*   Ensure that the `stores` directory exists and contains valid CSV files in the expected format.
*   The `Parts.csv` file within the `stores` directory should contain a single line with comma-separated product names.
*   The project uses ANSI color codes for output formatting. If your terminal does not support these codes, you might see escape sequences in the output.

## Authors

*   **Alireza Hosseini** - [GitHub Profile](https://github.com/notarealone)
*   **Soheil Haji Manesh** - [GitHub Profile](https://github.com/SoheilHajianManesh)
