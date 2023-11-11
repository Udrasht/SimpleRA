# SimpleRA
Data-Systems-Project || A simplified relational database management system with support for integer-only tables and matrices. Supports two-phase merge sort, buffer management and aggregate queries.
## Compilation Instructions
We use ```make``` to compile all the files and creste the server executable. ```make``` is used primarily in Linux systems, so those of you who want to use Windows will probably have to look up alternatives (I hear there are ways to install ```make``` on Windows). To compile

```cd``` into the SimpleRA directory
```
cd SimpleRA
```
```cd``` into the soure directory (called ```src```)
```
cd src
```
To compile
```
make clean
make
```
## To run

Post compilation, an executable names ```server``` will be created in the ```src``` directory
```
./server
```



## Commands / Queries
### Table Commands
- Look at the [Overview.html](./docs/Overview.md) to understand the syntax and working of the table related queries.
- Look at the [Report](./docs/Report(Phase2).md) to understand logic and working of below mention commands.
- ```SORT <table_name> BY <column_name1, column_name2,..., column_namek> IN <ASC|DESC, ASC|DESC,..., ASC|DESC>```
- ```<new_relation_name> <- JOIN <tablename1>, <tablename2> ON <column1> <bin_op> <column2>```
- ```<new_table> <- ORDER BY <attribute> ASC|DESC ON <table_name>```
- ```<new_table> <- GROUP BY <grouping_attribute> FROM <table_name> HAVING <aggregate(attribute)> <bin_op><attribute_value> RETURN <aggregate_func(attribute)>```
  
  
### Matrix Commands
- Look at the [Report](./docs/Report.md) to understand the syntax, working and Logic for Matrix related queries.
- ```LOAD MATRIX <matrix_name>```
  
The LOAD MATRIX command loads contents of the .csv (stored in ```data``` folder) and stores it as blocks in the ```data/temp``` directory.

- ```PRINT MATRIX <matrix_name>```
  
PRINT MATRIX command prints the first 20 rows of the matrix on the terminal.


- ```TRANSPOSE <matrix_name>```
  
TRANSPOSE command transposes the matrix IN PLACE (without using any additional disk blocks) and writes it back into the same blocks the matrix was stored in.


- ```EXPORT MATRIX <matrix_name>```
  
EXPORT command writes the contents of the matrix named <matrix_name> into a file called <matrix_name>.csv in ```data``` folder.


- ```RENAME MATRIX <matrix_name> <new_matrix_name>```
  
RENAME command change the name of matrix <matrix_name> to <new_matrix_name>.


- ```CHECKSYMMETRY < matrix_name >```
CHECKSYMMETRY command check the given matrix <matrix_name> is symmetric or not.


- ```COMPUTE < matrix_name >```

COMPUTE command ccompute A - A_Result(A<sup>T</sup>) of matrix and store the result into A_Result matrix.

---
