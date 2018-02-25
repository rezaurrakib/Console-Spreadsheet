// Reza, TUM Informatics

#include <cstdio>
#include <iostream>
#include <vector>
#include <stack>
#include <utility>
#include <sstream>
#include <cstring>


using namespace std;

#define MAX             10
#define REAL_DATA        0
#define FORMULA          1
#define PROCESSING       2
#define EMPTY            3
#define CONSTANT     -1000  // Special constraint
#define ERROR        -9999
#define NAN         -99999

// Variables

int cellState[MAX][MAX];
string GridInfoAsString[MAX][MAX]; // Actual input Spreadsheet

/* Method used for converting the string into integer data */

int strToIntConv(string data)
{
    istringstream buffer(data);
    int number;
    buffer >> number;
    return number;

}

string intToStr(int data)
{
    string number;
    stringstream out ;
    out << data;
    number = out.str();

    return(number);
}

void reset()
{
    for(int i = 0; i < MAX; i++)
        for(int j = 0; j < MAX; j++)
        {
            GridInfoAsString[i][j] = "EMPTY";
            cellState[i][j] = EMPTY;
        }

}

void printData(int row, int col)
{
    for(int i = 0; i < row; i++)
    {

        for(int j = 0; j < col; j++)
        {
            if(GridInfoAsString[i][j] == "EMPTY")
                cout << "   ";
            else
                cout << GridInfoAsString[i][j] << "   ";
        }

        cout << "\n";
    }

}

bool isOperator(char ch)
{
    if(ch == '+' || ch == '*' || ch == '-' || ch == '/')
        return true;

    else
        return false;

}

bool hasPrecedence(char op1, char op2)
{
    if((op1 == '*' || op1 == '/' ) && (op2 == '+' || op2 == '-' ))
        return false;

    else
        return true;
}

int applyOperation(char op, int a, int b)
{
    if (op == '+') return a + b;
    else if (op == '-') return a - b;
    else if (op == '*') return a * b;
    else if (op == '/')
    {
        if(b == 0)
        {
            return -1; // exception should be thrown
        }
        return a/b;
    }
}

int validExpressionCalculation(string equation)
{
    stack<int> values;
    stack<char> operators;

    int eqnSize= equation.length();

    for(int i = 0; i < equation.length(); i++)
    {
        if(isdigit(equation[i]))
        {
            string num = "";
            while(isdigit(equation[i]) && i<eqnSize)
            {
                num.push_back(equation[i]);
                i++;
            }

            int val = strToIntConv(num);
            //cout << "Val is  : " << val << "\n";
            values.push(val);
        }

        if(isOperator(equation[i]))
        {
            while(!operators.empty())
            {
                if(hasPrecedence(equation[i], operators.top()))
                {
                    int a = values.top();
                    values.pop();

                    int b = values.top();
                    values.pop();

                    char op = operators.top();
                    operators.pop();

                    int res = applyOperation(op, a, b);
                    //cout << "res is  Ist pos : " << res << "\n";
                    values.push(res);
                }
                else
                    break;

            }
            operators.push(equation[i]);
            //cout << "Operator Top : " << operators.top() << "\n";

        }

    }

    // After parsing all the values with operators, apply remaining
    // operators to remaining values
    while (!operators.empty())
    {
        int a = values.top();
        values.pop();

        int b = values.top();
        values.pop();

        char op = operators.top();
        operators.pop();

        int res = applyOperation(op, a, b);
        //cout << "res is  2nd pos : " << res << "\n";

        values.push(res);
    }


    // Top of 'values' contains result
    int res = values.top();
    values.pop();

    return res;

}

int expressionEvaluation(string exprsn, int c_i, int c_j)
{
    cellState[c_i][c_j] = PROCESSING; // Mark cell(i,j) as processing

    vector<pair<int, int> > indexInfo;
    vector<char> oprtr;
    vector<int> operandVal;

    /* As exprsn[0] = '=' , so don't need to process that */
    for(int i = 1; i<exprsn.length(); i++)
    {
        if(exprsn[i] >= 'A' && exprsn[i] <= 'Z')
        {
            int cell_j = exprsn[i] - 'A';
            i++;
            string st = "";
            while(!isOperator(exprsn[i]) && i < exprsn.length())
            {
                st.push_back(exprsn[i]);
                i++;
            }
            int cell_i = strToIntConv(st);
            //cout << "Cell_i value is : " << cell_i << "\n";
            indexInfo.push_back(make_pair(cell_i - 1, cell_j));
        }

        // For operation of constant ......
        else if(exprsn[i] >= '0' && exprsn[i] <= '9')
        {
            string st = "";
            st.push_back(exprsn[i]);
            i++;
            while(!isOperator(exprsn[i]) && i < exprsn.length())
            {
                st.push_back(exprsn[i]);
                i++;
            }
            indexInfo.push_back(make_pair(CONSTANT, strToIntConv(st)));
        }


        if(isOperator(exprsn[i]))
        {
            oprtr.push_back(exprsn[i]);
        }

    }


    /*for(int i = 0; i<oprtr.size(); i++)
        cout << oprtr[i] << "\n";*/

    // checking the cell index ......
    // If the cell is still processing , mark it as Error
    // Otherwise, if the cell contains integer/real value,
    // then,


    for(int i = 0; i < indexInfo.size(); i++)
    {
        int x = indexInfo[i].first;
        int y = indexInfo[i].second;

        if(indexInfo[i].first == CONSTANT) // A constant is present
        {
            operandVal.push_back(y);

        }

        else
        {
            if(cellState[x][y] == FORMULA)
            {
                int val = expressionEvaluation(GridInfoAsString[x][y], x, y); // Recursively call the cell
                //cout << "The val after expression : " << val << "\n";
                if(val == ERROR)
                {
                    GridInfoAsString[c_i][c_j] = "#ERROR";
                    return ERROR;
                }

                else if(val == NAN)
                {
                    GridInfoAsString[c_i][c_j] = "#NAN";
                    return NAN;
                }

                else
                    operandVal.push_back(val);
            }


            else if(cellState[x][y] == REAL_DATA)
            {
                int val = strToIntConv(GridInfoAsString[x][y]);
                //cout << "The val in here : " << val << "\n";
                operandVal.push_back(val);
            }

            else if(cellState[x][y] == EMPTY)
            {
                GridInfoAsString[c_i][c_j] = "#NAN";
                return NAN;
            }

            else if(cellState[x][y] == PROCESSING)
            {
                GridInfoAsString[c_i][c_j] = "#ERROR";
                return ERROR;
            }

        }
    }



    // If the pointer comes in this point, that means , the equation is OK
    // Now calculate the value with the precedence
    // Creating the equation using value ...

    string eqn = intToStr(operandVal[0]);

    for(int i=1; i < operandVal.size(); i++)
    {
        eqn.push_back(oprtr[i-1]);
        eqn += intToStr(operandVal[i]);
    }

    //cout << eqn << "\n";

    cellState[c_i][c_j] = REAL_DATA;
    int result  = validExpressionCalculation(eqn);
    GridInfoAsString[c_i][c_j] = intToStr(result);
    //cout << "Valid Expression outcome : " << result << "\n";

    return 0;
}

int main()
{
    freopen("input.txt", "r", stdin); // Read Input from the text file
    //freopen("input1.txt", "r", stdin);
    //freopen("input2.txt", "r", stdin);
    //freopen("input3.txt", "r", stdin);

    reset();
    string str;
    int rowCount = 0;
    int colCount = 0;
    vector<string> formulaList;
    vector<pair<int, int> > formulaCoordinate;

    while(getline(cin, str))
    {
        istringstream iss(str);
        string token;
        vector<string> cellData;
        while(getline(iss, token, '\t'))
        {
            //cout << token << "\n";
            cellData.push_back(token);

        }

        if(cellData.size() > colCount)
            colCount = cellData.size();

        for(int i = 0; i<cellData.size(); i++)
        {
            if(cellData[i][0] >='0' && cellData[i][0] <= '9') // It's a value
            {
                GridInfoAsString[rowCount][i] = cellData[i];
                cellState[rowCount][i] = REAL_DATA;

            }

            else if(cellData[i][0] == '=') // It's a Formula
            {
                GridInfoAsString[rowCount][i] = cellData[i];
                cellState[rowCount][i] = FORMULA;
                formulaList.push_back(cellData[i]);
                formulaCoordinate.push_back(make_pair(rowCount, i));

            }
        }

        rowCount++;
    }
    //printData(rowCount, colCount);
    // calculate all the formulas

    for(int i = 0; i < formulaList.size(); i++)
    {
        int c_i = formulaCoordinate[i].first;
        int c_j = formulaCoordinate[i].second;

        if(cellState[c_i][c_j] == FORMULA)
            expressionEvaluation(formulaList[i], c_i, c_j);

    }
    printData(rowCount, colCount); // Print the Final Output

    return 0;
}
