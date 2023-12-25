#include "math.h"

#include "lvgl.h"
#include "sl_ui/ui.h"
#include "sl_ui/ui_events.h"

#include "app/calc/calc.h"
static char content[20] = {0};
static int ptr = 0;
/*=============================================================================
                               UI controllers
 =============================================================================*/

void calc_set_fml(const char *fml_str)
{
  lv_label_set_text(ui_CalcFml, fml_str);
}

/*=============================================================================
                             UI event handlers
 =============================================================================*/

/**
 * Called when user changes the tab. Should alter the calc mode here.
 */
void CalcModeChanged(lv_event_t *e)
{
  while (ptr != 0) {
    ptr--;
    content[ptr] = 0;
  }
  calc_set_fml(content);
  lv_obj_t *tabview = lv_event_get_target(e);
  uint16_t tab_index = lv_tabview_get_tab_act(tabview);

  // 0: Common
  // 1: Equation
  // 2: Binary
  calc_set_mode((enum calc_mode)tab_index);
}

void CalcCommClicked0(lv_event_t *e)
{
  content[ptr++] = '0';
  calc_set_fml(content);
}
void CalcCommClicked1(lv_event_t *e)
{
  content[ptr++] = '1';
  calc_set_fml(content);
}
void CalcCommClicked2(lv_event_t *e)
{
  content[ptr++] = '2';
  calc_set_fml(content);
}
void CalcCommClicked3(lv_event_t *e)
{
  content[ptr++] = '3';
  calc_set_fml(content);
}
void CalcCommClicked4(lv_event_t *e)
{
  content[ptr++] = '4';
  calc_set_fml(content);
}
void CalcCommClicked5(lv_event_t *e)
{
  content[ptr++] = '5';
  calc_set_fml(content);
}
void CalcCommClicked6(lv_event_t *e)
{
  content[ptr++] = '6';
  calc_set_fml(content);
}
void CalcCommClicked7(lv_event_t *e)
{
  content[ptr++] = '7';
  calc_set_fml(content);
}
void CalcCommClicked8(lv_event_t *e)
{
  content[ptr++] = '8';
  calc_set_fml(content);
}
void CalcCommClicked9(lv_event_t *e)
{
  content[ptr++] = '9';
  calc_set_fml(content);
}
void CalcCommClickedAdd(lv_event_t *e)
{
  content[ptr++] = '+';
  calc_set_fml(content);
}
void CalcCommClickedSub(lv_event_t *e)
{
  content[ptr++] = '-';
  calc_set_fml(content);
}
void CalcCommClickedMul(lv_event_t *e)
{
  content[ptr++] = '*';
  calc_set_fml(content);
}
void CalcCommClickedDiv(lv_event_t *e)
{
  content[ptr++] = '/';
  calc_set_fml(content);
}
// 定义运算符的优先级
int getPriority(char op)
{
  if (op == '+' || op == '-')
    return 1;
  if (op == '*' || op == '/')
    return 2;
  if (op == '^')
    return 3;
  return 0; // 对于其他字符，包括括号，返回0
}

// 执行二元运算
double executeOperation(double operand1, char operator, double operand2)
{
  switch (operator) {
  case '+':
    return operand1 + operand2;
  case '-':
    return operand1 - operand2;
  case '*':
    return operand1 * operand2;
  case '/':
    return operand1 / operand2;
  case '^':
    return pow(operand1, operand2);
  default:
    return 0.0; // 处理未知运算符
  }
}
double customAtof(const char *str)
{
  double result = 0.0;
  double fraction = 0.1; // 用于处理小数部分的权重

  // 处理整数部分
  while (*str >= '0' && *str <= '9') {
    result = result * 10.0 + (*str - '0');
    str++;
  }

  // 处理小数部分
  if (*str == '.') {
    str++; // 跳过小数点
    while (*str >= '0' && *str <= '9') {
      result += (*str - '0') * fraction;
      fraction *= 0.1; // 更新权重
      str++;
    }
  }

  return result;
}
double customAtob(const char *str)
{
  // 将二进制的字符串转成十进制
  double result = 0.0;

  // 处理整数部分
  while (*str >= '0' && *str <= '1') {
    result = result * 2.0 + (*str - '0');
    str++;
  }
  return result;
}
// 计算表达式的函数
double calculateExpression(const char *expression, int bin)
{
  int len = strlen(expression);

  // 操作数栈
  double operandStack[len];
  int operandTop = -1;

  // 运算符栈
  char operatorStack[len];
  int operatorTop = -1;

  // 遍历表达式中的每个字符
  for (int i = 0; i < len; ++i) {
    char currentChar = expression[i];

    // 如果是空格，跳过
    if (currentChar == ' ') {
      continue;
    }

    // 如果是数字，读取整个数字并将其入栈
    if ((currentChar >= '0' && currentChar <= '9') || currentChar == '.') {
      if (bin == 1) {
        operandStack[++operandTop] =
            customAtob(&expression[i]); // 使用 atob 将字符串转换为浮点数
      } else {
        operandStack[++operandTop] =
            customAtof(&expression[i]); // 使用 atof 将字符串转换为浮点数
      }
      while (i < len && ((expression[i] >= '0' && expression[i] <= '9') ||
                         expression[i] == '.')) {
        ++i; // 移动到数字的下一个字符
      }
      --i; // 因为在循环末尾有 i++，所以这里需要减1
    } else if (currentChar == '(') {
      // 如果是左括号，直接入栈
      operatorStack[++operatorTop] = currentChar;
    } else if (currentChar == ')') {
      // 如果是右括号，将栈顶的运算符弹出并执行，直到遇到左括号
      while (operatorTop >= 0 && operatorStack[operatorTop] != '(') {
        double operand2 = operandStack[operandTop--];
        double operand1 = operandStack[operandTop--];
        char op = operatorStack[operatorTop--];
        operandStack[++operandTop] = executeOperation(operand1, op, operand2);
      }
      // 弹出左括号
      --operatorTop;
    } else {
      // 如果是运算符，处理运算符栈中的运算符
      while (operatorTop >= 0 && getPriority(operatorStack[operatorTop]) >=
                                     getPriority(currentChar)) {
        double operand2 = operandStack[operandTop--];
        double operand1 = operandStack[operandTop--];
        char op = operatorStack[operatorTop--];
        operandStack[++operandTop] = executeOperation(operand1, op, operand2);
      }
      // 将当前运算符入栈
      operatorStack[++operatorTop] = currentChar;
    }
  }

  // 处理剩余的运算符
  while (operatorTop >= 0) {
    double operand2 = operandStack[operandTop--];
    double operand1 = operandStack[operandTop--];
    char op = operatorStack[operatorTop--];
    operandStack[++operandTop] = executeOperation(operand1, op, operand2);
  }

  // 返回最终结果
  return operandStack[0];
}
void CalcCommClickedEqual(lv_event_t *e)
{

  double result = calculateExpression(content, 0);
  while (ptr != 0) {
    ptr--;
    content[ptr] = 0;
  }
  char result_show[50]; // Adjust the size based on your needs
  sprintf(result_show, "%g", result);
  calc_set_fml(result_show);
}
void CalcCommClickedClear(lv_event_t *e)
{
  while (ptr != 0) {
    ptr--;
    content[ptr] = 0;
  }
  calc_set_fml(content);
}

void CalcCommClickedLB(lv_event_t *e)
{
  content[ptr++] = '(';
  calc_set_fml(content);
}
void CalcCommClickedRB(lv_event_t *e)
{
  content[ptr++] = ')';
  calc_set_fml(content);
}
void CalcCommClickedPow(lv_event_t *e)
{
  content[ptr++] = '^';
  calc_set_fml(content);
}
void CalcCommClickedBackspace(lv_event_t *e)
{
  content[--ptr] = '\0';
  calc_set_fml(content);
}
void Equation(lv_event_t *e)
{
  content[ptr++] = '=';
  calc_set_fml(content);
}
void CalcEqualConfirm(lv_event_t *e)
{
  char result[40] = {0};
  char str[20];
  strcpy(str, content);
  while (ptr != 0) {
    ptr--;
    content[ptr] = 0;
  }
  calc_set_fml(content);
  int ptr1 = -1;
  int ptr2 = -1;
  int ptr3 = -1; // equal
  double a = 0, b = 0, c = 0;

  for (int i = 0; i < sizeof(str) / sizeof(char); ++i) {
    if (str[i] == 'x') {
      if (str[i + 1] == '^') {
        ptr2 = i;
      } else {
        ptr1 = i;
      }
    }
    if (str[i] == '=') {
      ptr3 = i;
    }
  }

  for (int i = 0; i < sizeof(str) / sizeof(char); ++i) {
    if (ptr2 == -1) { // No quadratic
      a = 0;
      break;
    }
    if (ptr2 == 0) { // No quadratic coefficient
      a = 1;
      break;
    }
    char ch = str[ptr2 - i - 1];
    if (ch == '-') {
      a = -1 * a;
      break;
    }
    if (ch == '+') {
      break;
    }
    if ('0' <= ch && ch <= '9') {
      a += pow(10, i) * (ch - '0');
    }
  }

  for (int i = 0; i < sizeof(str) / sizeof(char); ++i) {
    if (ptr1 == -1) { // No quadratic
      b = 0;
      break;
    }
    if (ptr1 == 0) { // No quadratic coefficient
      b = 1;
      break;
    }
    char ch = str[ptr1 - i - 1];
    if (ch == '-') {
      b = -1 * b;
      break;
    }
    if (ch == '+') {
      break;
    }
    if ('0' <= ch && ch <= '9') {
      b += pow(10, i) * (ch - '0');
    }
  }

  for (int i = 0; i < sizeof(str) / sizeof(char); ++i) {
    char ch = str[ptr3 - i - 1];
    if (ch == 'x') {
      c = 0;
    }
    if (ch == '-') {
      c = -1 * c;
      break;
    }
    if (ch == '+') {
      break;
    }
    if ('0' <= ch && ch <= '9') {
      c += pow(10, i) * (ch - '0');
    }
  }

  double temp = 0;
  for (int i = 0; i < sizeof(str) / sizeof(char); ++i) {
    char ch = str[sizeof(str) / sizeof(char) - i - 2];
    if (ch == '=') {
      c -= temp;
      break;
    }
    if (ch == '+') {
      c -= temp;
      break;
    }
    if (ch == '-') {
      c += temp;
      break;
    }
    if ('0' <= ch && ch <= '9') {
      temp += pow(10, i) * (ch - '0');
    }
  }

  if (a == 0) { // 一元一次方程
    if (b == 0) {
      if (c == 0) {
        sprintf(result, "Infinite");
      } else {
        sprintf(result, "No solution");
      }
    } else {
      if (c == 0) {
        sprintf(result, "x=0");
      } else {
        sprintf(result, "x=%.2f", -c / b);
      }
    }
  } else {
    double delta = b * b - 4 * a * c;
    if (delta < 0) {
      sprintf(result, "No solution");
    } else if (delta == 0) {
      sprintf(result, "x=%g", -b / (2 * a));
    } else {
      sprintf(result, "x=%g | x=%g", (-b + sqrt(delta)) / (2 * a),
              (-b - sqrt(delta)) / (2 * a));
    }
  }
  calc_set_fml(result);
}

void CalcCommClickedX(lv_event_t *e)
{
  content[ptr++] = 'x';
  calc_set_fml(content);
}
void CalcCommClickedY(lv_event_t *e)
{
  content[ptr++] = 'y';
  calc_set_fml(content);
}
char *intToBinaryString(int num)
{
  int size = sizeof(int) * 8; // 获取整数的位数
  if (num == 0) {
    char *binaryStr = (char *)malloc(2);
    binaryStr[0] = '0';
    binaryStr[1] = '\0';
    return binaryStr;
  }

  char *binaryStr =
      (char *)malloc(size + 1); // 为字符串分配足够的空间，加上终止符 '\0'
  binaryStr[size] = '\0'; // 设置字符串终止符

  int result = 0; // 结果
  int k = 1;
  int i = 0;
  while (num) // 除到0就停止
  {
    i = num % 2; // 余数i=1	//i=1		//i=1			//i=1
    result =
        k * i +
        result; // result=1	//result=11	//result=111	//result=1111
    k = k * 10; // k=10		//k=100		//k=1000
    num =
        num / 2; // n=7		//n=3		//n=1 //n=0退出
  }
  sprintf(binaryStr, "%d", result); // 1111

  return binaryStr;
}

void BinaryClickedAdd(lv_event_t *e) {}

void BinarySub(lv_event_t *e) {}

void BinaryMul(lv_event_t *e) {}

void BinaryEqual(lv_event_t *e)
{
  // 将算式转换成十进制
  double result = calculateExpression(content, 1);
  int intResult = (int)result;
  // 将整数转换为二进制字符串
  char *binaryStr = intToBinaryString(intResult);
  while (ptr != 0) {
    ptr--;
    content[ptr] = 0;
  }
  calc_set_fml(binaryStr);

  free(binaryStr);
}

void BinaryClear(lv_event_t *e) {}

void BinaryClicked0(lv_event_t *e) {}

void BinaryClicked1(lv_event_t *e) {}
