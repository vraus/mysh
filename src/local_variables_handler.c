#include "../include/local_variables_handler.h"

char *extract_var_name(char *_name)
{
    char *var_name = (char *)calloc(strlen(_name), sizeof(char));
    strcpy(var_name, _name + 1);
    var_name[strlen(var_name)] = '\0';

    return var_name;
}

void unset_local_variables(struct Variable *variables, char *_name)
{
    for (int i = 0; i < MAX_VARIABLES; i++)
    {
        if (strcmp(variables[i].name, _name) == 0)
        {
            // Variable found, remove it by shifting remaining elements
            for (int j = i; j < MAX_VARIABLES - 1; ++j)
            {
                strcpy(variables[j].name, variables[j + 1].name);
                strcpy(variables[j].value, variables[j + 1].value);
            }

            // Clear the last element
            variables[MAX_VARIABLES - 1].name[0] = '\0';
            variables[MAX_VARIABLES - 1].value[0] = '\0';

            return;
        }
    }

    handle_error_noexit("Error: Variable not found.\n");
}

void set_local_variable(struct Variable *variables, char *_name, char *_value)
{
    for (int i = 0; i < MAX_VARIABLES; ++i)
    {
        // Check if the current slot is empty or if the variable with the same name exists
        if (variables[i].name[0] == '\0' || strcmp(variables[i].name, _name) == 0)
        {
            // Set or update the variable
            if (_value[0] == '$')
            {
                char *var_name = extract_var_name(_value);
                bool found = false;

                for (int j = 0; j < MAX_VARIABLES; ++j)
                {
                    if (strcmp(variables[j].name, var_name) == 0)
                    {
                        strcpy(variables[i].name, _name);
                        strcpy(variables[i].value, variables[j].value);
                        found = true;
                    }
                }
                if (!found)
                    handle_error("Error: Unknown variable name. Nothing is done.", -1);
            }
            else
            {
                strcpy(variables[i].name, _name);
                strcpy(variables[i].value, _value);
            }
            return;
        }
    }
    // HuuHoo... something went wrong ...
    handle_error_noexit("Error: Maximum number of local variables reached.\n");
}

void print_variable(struct Variable *variables, char *_name)
{
    char *var_name = extract_var_name(_name);
    for (int i = 0; i < MAX_VARIABLES && variables[i].name[0] != '\0'; ++i)
    {
        if (strcmp(variables[i].name, var_name) == 0)
        {
            printf("%s\n", variables[i].value);
            return;
        }
    }

    handle_error_noexit("Error: Unknown variable name.");
}