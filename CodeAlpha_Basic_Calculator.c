//Project Name: CodeAlpha Basic Calculator

//File Name: "calculator.c"

#include <stdio.h>

int main(void)
{
    double num1, num2;
    char op;

    printf("===== CodeAlpha Basic Calculator =====\n");

    // Input the first number
    printf("Enter first number: ");
    if (scanf("%lf", &num1) != 1)
    {
        printf("Error: Invalid first number.\n");
        return 1;
    }

    // Input the operator
    printf("Enter operator (+, -, *, /): ");
    if (scanf(" %c", &op) != 1)
    {
        printf("Error: Invalid operator input.\n");
        return 1;
    }

    // Input the second number
    printf("Enter second number: ");
    if (scanf("%lf", &num2) != 1)
    {
        printf("Error: Invalid second number.\n");
        return 1;
    }

    // Perform the selected arithmetic operation
    switch (op)
    {
        case '+':
            printf("Result: %.2f\n", num1 + num2);
            break;

        case '-':
        	
            printf("Result: %.2f\n", num1 - num2);
            break;

        case '*':
            printf("Result: %.2f\n", num1 * num2);
            break;

        case '/':
            if (num2 == 0.0)
            {
                printf("Error: Division by zero is not allowed.\n");
            }
            else
            {
                printf("Result: %.2f\n", num1 / num2);
            }
            break;

        default:
            printf("Error: Invalid operator.\n");
            return 1;
    }

    printf("Thank you for using the calculator!\n");

    return 0;
}
