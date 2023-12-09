#ifndef LOCAL_VARIABLES_HANDLER_H
#define LOCAL_VARIABLES_HANDLER_H

#include <stdbool.h>

#define MAX_VARIABLES 100
#define MAX_VALUE_LENGTH 100

struct Variable
{
    char name[MAX_VARIABLES];
    char value[MAX_VALUE_LENGTH];
};

/**
 * @brief Method to extract the name in a reference ($var)
 *
 * @param _name The variable reference (with '$')
 * @return The var name without the '$' reference.
 */
char *extract_var_name(char *_name);

/**
 * @brief Unsets (removes) a local variable from the array of variables
 *
 * @param variables Pointer to the array of local variables
 * @param _name Name of the variable to unset
 */
void unset_local_variables(struct Variable *variables, char *_name);

/**
 * @brief Sets or updates a local variable in the array of variables
 *
 * @param variables Pointer to the array of local variables
 * @param _name Name of the variable to set
 * @param _value Value to associate with the variable
 */
void set_local_variable(struct Variable *variables, char *_name, char *_value);

/**
 * @brief Prints the local variable stored in the array
 *
 * @param variables Pointer to the array of local variables
 * @param _name Name of the variable you want to print its value
 */
void print_variable(struct Variable *variables, char *_name);

#endif