# Efficient Database Table Search

## Project Overview
This project implements a B+ tree index for efficient searching in the CUSTOMER_ACCOUNT table, a benchmark database table for performance testing. The table structure is as follows:
- **CA_ID** (Primary Key, Integer)
- **CA_B_ID** (Integer, Administrative Data)
- **CA_C_ID** (Integer, Administrative Data)
- **CA_NAME** (String, Account Name)
- **CA_TAX_ST** (Integer, Administrative Data)
- **CA_BAL** (Float, Account Balance)
The table is provided as a text file with | as the delimiter.

## Technologies Used
- **Backend:** C

## Features
- **Creating and Destroying the B+ Tree Index:** Users specify the order m (between 3 and 10) when creating the index.
- **Displaying the Index:** The index is printed in a readable format, showing tree nodes with proper spacing.
- **Inserting Records:** Users can add new records to the table.
- **Deleting Records:** Records can be deleted based on the primary key.
- **Searching for a Single Record:** Given a primary key, the search function retrieves the corresponding record and displays the search steps.
- **Searching for Multiple Records:** Users specify k keys for retrieval, and the program outputs the search steps and writes found records to a file.
- **Searching for k Successive Records:** The search starts from a given primary key and retrieves k consecutive records, logging search steps and writing results to a file.

## Usage
The program provides a menu-based interface where users can interact via keyboard input.
Users enter the number of a menu option and provide any necessary additional parameters.
The program continues running until the user selects the exit option.

## Performance Evaluation
The program evaluates search performance by measuring the average number of steps for searching records using the B+ tree index.
