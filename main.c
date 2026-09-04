#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

struct Budget
{
    char categorey[50];
    float limit;
};

struct Transaction 
{
    int type; // 0 = expense, 1 = income
    float amount;
    char categorey[50];
    char date[50];
    char description[364];
};

void get_current_datetime(char *buffer, int size);
void add_transaction(struct Transaction **transactions, int *count, int *capacity);
void view_transaction(struct Transaction *transaction, int count);
void set_budget(struct Budget **budgets, int *count, int *capacity);
void view_reports(struct Transaction *transactions, int count);
void save_transactions(struct Transaction *transactions, int count);
void save_budgets(struct Budget *budgets, int count);

int main()
{
    initscr();   
    keypad(stdscr, TRUE);
    curs_set(0);

    struct Budget *budget = NULL;
    struct Transaction *transaction = NULL;

    int highlight = 0;
    int choice = 0;

    int transaction_count = 0;
    int capacity = 2;
    int budget_count = 0;
    int budget_capacity = 2;

    budget = malloc(budget_capacity * sizeof(struct Budget));
    transaction = malloc(capacity * sizeof(struct Transaction));
    
    char *options[] = 
    {
        "1. Add Transaction",
        "2. View Transactions",
        "3. Budget Management",
        "4. Reports",
        "5. Save/Load",
        "6. Exit"
    };

    while(1)
    {
        box(stdscr, 0, 0);
        mvprintw(1, 2, "Personal Finance Manager");

        for(int i = 0; i < 6; i++)
        {
            if(i == highlight)
            {
                attron(A_REVERSE);
                mvprintw(3 + i, 2, "%s", options[i]);
                attroff(A_REVERSE);
            }
            else
            {
                mvprintw(3 + i, 2, "%s", options[i]);
            }
        }
    
    
        refresh();
        choice = getch();

        if(choice == KEY_UP)
        {
            highlight--;
            if(highlight < 0) highlight = 0;
        }
        else if(choice == KEY_DOWN)
        {
            highlight++;
            if (highlight > 5) highlight = 5;
        }
        else if(choice == '\n' || choice == '\r')
        {
            if(highlight == 0)
            {
                clear();
                add_transaction(&transaction, &transaction_count, &capacity);
                clear();
            }
            if(highlight == 1)
            {
                clear();
                view_transaction(transaction, transaction_count);
                clear();
            }
            if(highlight == 2)
            {
                clear();
                set_budget(&budget, &budget_count, &budget_capacity);
                clear();
            }
            if(highlight == 3)
            {
                clear();
                view_reports(transaction, transaction_count);
                clear();
            }
            if(highlight == 4)
            {
                clear();
                box(stdscr, 0, 0);
                mvprintw(1, 2, "SAVE DATA");
                mvprintw(3, 2, "1. Save Transactions");
                mvprintw(4, 2, "2. Save Budgets");
                mvprintw(5, 2, "3. Save All");
                mvprintw(6, 2, "4. Cancel");
                refresh();
    
                int sub = getch();
    
                if(sub == '1')
                {
                    save_transactions(transaction, transaction_count);
                }
                else if (sub == '2')
                {
                    save_budgets(budget, budget_count);
                }
                else if (sub == '3')
                {
                    save_transactions(transaction, transaction_count);
                    save_budgets(budget, budget_count);
                }
                clear();
            }
            if(highlight == 5)
            {
                save_transactions(transaction, transaction_count);
                save_budgets(budget, budget_count);
                break;
            }
        }
    }

    endwin();
    printf("GoodBye!!\n");
    return 0;
}

void add_transaction(struct Transaction **transactions, int *count, int *capacity)
{
    if(*count == *capacity)
    {
        *capacity *= 2;
        struct Transaction *temp = realloc(*transactions, *capacity * sizeof(struct Transaction));
        if(temp == NULL) return;
        *transactions = temp;
    }

    int type = 0;
    char description[364] = "";
    char categorey[50] = "";
    float amount = 0.0f;

    echo();
    curs_set(1);
    box(stdscr, 0, 0);

    mvprintw(1, 2, "Add Transaction");
    mvprintw(2, 2, "==============================");

    mvprintw(4, 2, "Categorey: ");
    getstr(categorey);

    mvprintw(5, 2, "Description: ");
    getstr(description);

    mvprintw(6, 2, "Amount: ");
    scanw("%f", &amount);

    mvprintw(7, 2, "Type(1 = income, 0 = expense): ");
    scanw("%d", &type);

    strcpy((*transactions)[*count].categorey, categorey);
    strcpy((*transactions)[*count].description, description);
    (*transactions)[*count].amount = amount;
    (*transactions)[*count].type = type;
    get_current_datetime((*transactions)[*count].date, 50);

    mvprintw(9, 2, "✅ Transaction added!");
    (*count)++;
    refresh();
    getch();
    noecho();
    curs_set(0);

}

void get_current_datetime(char *buffer, int size)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

void view_transaction(struct Transaction *transaction, int count)
{
    if(count == 0) return;

    int row = 3;

    box(stdscr, 0, 0);                                                                                                                                                                                    
    mvprintw(1, 2, "View Transaction");
    mvprintw(2, 2, "====================");

    for(int i = 0; i < count; i++)
    {
        mvprintw(row, 2, "====================================");
        row++;
        mvprintw(row, 2, "Transaction Number #%d", i + 1);
        row++;
        mvprintw(row, 2, "====================================");
        row++;
        mvprintw(row, 2, "Type: %s", transaction[i].type == 1 ? "Income(+)" : "Expense(-)");
        row++;
        mvprintw(row, 2, "Categorey: %s", transaction[i].categorey);
        row++;
        mvprintw(row, 2, "Description: %s", transaction[i].description);
        row++;
        mvprintw(row, 2, "Amount: %.2f", transaction[i].amount);
        row++;
        mvprintw(row, 2, "Date: %s", transaction[i].date);
        row++;
        mvprintw(row,2 ,"====================================");
        row++;
    }

    mvprintw(row + 2, 2, "Press any key to return...");
    refresh();
    getch();
}

void set_budget(struct Budget **budgets, int *count, int *capacity)
{
    if(*count == *capacity)
    {
        *capacity *= 2;
        struct Budget *temp = realloc(*budgets, *capacity * sizeof(struct Budget));
        if(temp == NULL) return;
        *budgets = temp;
    }

    echo();
    curs_set(1);

    box(stdscr, 0, 0);
    mvprintw(1, 2, "Budget Management");
    mvprintw(2, 2, "=============================");

    char categorey[50] = "";
    float limit = 0.0f;

    mvprintw(4, 2, "Categorey: ");
    getstr(categorey);

    mvprintw(5, 2, "Limit: ");
    scanw("%f", &limit);

    strcpy((*budgets)[*count].categorey, categorey);
    (*budgets)[*count].limit = limit;

    mvprintw(9, 2, "Budget Has Been Set...");
    (*count)++;
    refresh();
    getch();
    noecho();
    curs_set(0);
}

void view_reports(struct Transaction *transactions, int count)
{
    if(count == 0) return;

    box(stdscr, 0, 0);
    mvprintw(1, 2, "View Reports");
    mvprintw(2, 2, "==========================");

    int row = 3;
    float total_income = 0.0f;
    float total_expenses = 0.0f;
    float savings = 0.0f;

    for (int i = 0; i < count; i++)
    {
        if (transactions[i].type == 1)
        {
        // Add to income
            total_income += transactions[i].amount;
        }
        else
        {
        // Add to expenses
            total_expenses += transactions[i].amount;
        }
    }
    savings = total_income - total_expenses;

    mvprintw(row, 2, "Total Income: $%.2f", total_income);
    row++;
    mvprintw(row, 2, "Total Expenses: $%.2f", total_expenses);
    row++;
    mvprintw(row, 2, "Net Savings: $%.2f", savings);
    row++;
    mvprintw(row, 2, "Total Transactions: %d", count);
    row++;

    mvprintw(row + 2, 2, "Press any key to return...");
    refresh();
    getch();
}
void save_transactions(struct Transaction *transactions, int count)
{
    FILE *file = fopen("save_transactions.txt", "w");
    
    if(count == 0) return;

    for(int i = 0; i < count; i++)
    {
        fprintf(file, "====================================\n");
        fprintf(file, "Transaction Number #%d\n", i + 1);
        fprintf(file, "====================================\n");
        fprintf(file, "Type: %s\n", transactions[i].type == 1 ? "Income" : "Expense");
        fprintf(file, "Categorey: %s\n", transactions[i].categorey);
        fprintf(file, "Description: %s\n", transactions[i].description);
        fprintf(file, "Amount: %.2f\n", transactions[i].amount);
        fprintf(file, "Date: %s\n", transactions[i].date);
        fprintf(file, "====================================\n");
    }
    fclose(file);
    mvprintw(8, 2, "Press any key to return...");
    refresh();
    getch();
}

void save_budgets(struct Budget *budgets, int count)
{
    if(count == 0) return;

    FILE *file = fopen("save_budgets.txt", "w");

    for(int i = 0; i < count; i++)
    {
        fprintf(file, "====================================\n");
        fprintf(file, "Budget #%d\n", i + 1);
        fprintf(file, "====================================\n");
        fprintf(file, "Categorey: %s\n", budgets[i].categorey);
        fprintf(file, "Limit: %.2f\n", budgets[i].limit);
        fprintf(file, "====================================\n");
    }

    fclose(file);
    mvprintw(8, 2, "Press any key to return...");
    refresh();
    getch();
}
