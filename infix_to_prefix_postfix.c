#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*****
			Grammar use to solve this
			Expr 		-> Term* ([+ | -] Term)*
			Term 		-> Factor* ([* | /] Factor)*
			Factor 	-> Factor* (^ Factor)*
			Factor 	-> (Expr)
			Factor 	-> \d+
*****/


/* This is a valid definition for this struct
struct ExpressionPart{
  char* tPart;
  struct ExpressionPart* next;
  struct ExpressionPart* prev;
};
typedef struct ExpressionPart ExpressionPart;
*/

typedef struct ExpressionPart{
  char* tPart;
  struct ExpressionPart* next;
  struct ExpressionPart* prev;
} ExpressionPart;

//Definition of functions
ExpressionPart *pushItem(ExpressionPart*, char*);
ExpressionPart *addItem(ExpressionPart*, char*);
ExpressionPart *pushStack(ExpressionPart*, ExpressionPart*);
ExpressionPart *addStack(ExpressionPart*, ExpressionPart*);
ExpressionPart *analyzeFactor(const char*, int*, double*, bool);
ExpressionPart *analyzeTerm(const char*, int*, double*, bool);
ExpressionPart *analyzeExpression(const char*, int*, double*, bool);

double operations(char, double, double);
double exponential(double, double);
double toNumber(char*);

char *addChar(char*, char);
char *pushChar(char*, char);
char* concat(const char*, const char*, bool);

bool isNumber(char);

void printStack(ExpressionPart*);

int main (){
	//////************Hard expression ************//////////////
 	//char *expr = "(3*(5+2)+5*3)/(3+4)*2+5*7*(2+4)";
	//////****************************************//////////////

	//////************Hard expression ************//////////////
	const char *expr = "4 *(1 + 3)^((3 * 2 - 4 * 1)^3)/4 - 76 * 4";
	//////****************************************//////////////

	//const char *expr = "(8*3/5)*(2+3)+(4*6+4*3/2)*4*3-5*4/2+4*3/2-8*5*3";
	//const char *expr = "232/2*3-14/7*42+14*63-74/2*3";
	//const char* expr = "4*3/2 + 5*(3-1) ^ (2*1 + 3 - 2*1 + 2) ^ 2";
	//const char* expr = "4^3^2";
	//const char* expr = "(4.5*4 - 16) ^ 3 ^ (4 - 1) - 5.11 * 100";
  int reader = 0;
  double value = 0;
  ExpressionPart* expressionPrefix;
  //bool isPrefix = false;
	printf("Expresion original: %s\n", expr);
  expressionPrefix = analyzeExpression(expr, &reader, &value, true);
  //printf("Expresion %s: ", isPrefix ? "prefija" : "postfija");
  printf("Expresion prefija: \n");
  printStack(expressionPrefix);

  ExpressionPart* expressionPostfix;
  reader = 0;
  value = 0;
 	expressionPostfix = analyzeExpression(expr, &reader, &value, false);
  //printf("Expresion %s: ", isPrefix ? "prefija" : "postfija");
  printf("Expresion postfija: \n");
  printStack(expressionPostfix);

  printf("\nValor de la expresion: %.2f\n", value);

  return 0;
}


ExpressionPart *pushItem(ExpressionPart *stack, char* tPart){
  ExpressionPart *newItem;
  ExpressionPart *aux;
  newItem = (ExpressionPart*) malloc(sizeof(ExpressionPart));
  newItem->tPart = tPart;
  newItem->next = NULL;
  newItem->prev = NULL;
  if(stack  ==  NULL){
    stack = newItem;
  }
  else{
    aux = stack;
    newItem->next = aux;
    aux->prev = newItem;
    stack = newItem;
  }
  return stack;  
}

ExpressionPart *addItem(ExpressionPart *stack, char* tPart){
  ExpressionPart *newItem;
  ExpressionPart *aux;
  newItem = (ExpressionPart*) malloc(sizeof(ExpressionPart));
  newItem->tPart = tPart;
  newItem->next = NULL;
  newItem->prev = NULL;
  if(stack == NULL){
    stack = newItem;
  }
  else{
    aux = stack;
    while (aux->next != NULL){
      aux = aux->next;
    }
    aux->next = newItem;
    newItem->prev = aux;
  }
  return stack;  
}

ExpressionPart *pushStack(ExpressionPart *stack, ExpressionPart* otherStack){
  ExpressionPart* aux;
  if(stack  ==  NULL) {
    stack = otherStack;
  }
  else {
    aux = stack;
    otherStack->next = aux;
    aux->prev = otherStack;
    stack = otherStack;
  }
  return stack;  
}

ExpressionPart *addStack(ExpressionPart *stack,ExpressionPart *otherStack){
  ExpressionPart *aux;
  if(stack == NULL){
    stack = otherStack;
  }
  else{
    aux = stack;
    while (aux->next != NULL){
      aux = aux->next;
    }
    aux->next = otherStack;
    otherStack->prev = aux;
  }
  return stack;  
}

ExpressionPart *analyzeFactor(const char* expr, int* reader, double* value, bool isPrefix){
	ExpressionPart *factor = NULL;
	int pendingCloses = 0;
	char sign;
	char* tSign; //Sign represented in string
	while (expr[*reader] != '\0'){
		char tChar = expr[*reader];
		//printf("factor %c\n", tChar);
		switch(tChar){
			case ' ':
				(*reader)++;
			break;

			case '('://Start of subexpression
			{ 
				double subExpressionValue = 0;
				(*reader)++;
				//printf("Starting subexpresson analysis at %d\n", *reader);
				ExpressionPart* subExpression = analyzeExpression(expr, reader, &subExpressionValue, isPrefix);
				//printf("Subexpresson analysis finished at %d, subexpr value : %f\n", *reader, subExpressionValue);
				//printStack(subExpression);
				if (pendingCloses > 0){
					if (isPrefix){
						factor = addItem(factor, " ");
						factor = addStack(factor, subExpression);
					}else{
						factor = addStack(factor, subExpression);
						factor = addItem(factor, " ");
						factor = addItem(factor, tSign);
					}
					factor = addItem(factor, ")");
					pendingCloses--;
					*value = operations(sign, *value, subExpressionValue);
				}
				else{
					factor = addStack(factor, subExpression);
					*value = subExpressionValue;
				}
				(*reader)++;
			}
			break;

			case '^':
			{
				sign = tChar;
				tSign = (char*) malloc(2);
				tSign[0] = sign;
				tSign[1] = '\0';
				factor = (isPrefix) ? pushItem(factor, " ") : addItem(factor, " ");
				if (isPrefix){
					factor = pushItem(factor, tSign);
				}
				factor = pushItem(factor, "(");
				pendingCloses++;
				(*reader)++;
			}
			break;

			
			default: //This must be the number case
			{
				char* number = (char*) malloc(sizeof(char));
				number[0] = '\0';
				while (isNumber(expr[*reader]) || expr[*reader] == '.'){ //Reading number
					number = addChar(number, expr[*reader]);
					(*reader)++;
				}
				double currentNumber = toNumber(number);
				//number = isPrefix ? pushChar(number, ' ') : addChar(number, ' ');
				if (pendingCloses > 0){
					//number = isPrefix ? pushChar(number, ' ') : addChar(number, ' ');
					if (isPrefix){
						factor = addItem(factor, " ");
						factor = addItem(factor, number);
					}else{
						factor = addItem(factor, number);
						factor = addItem(factor, " ");
						factor = addItem(factor, tSign);
					}
					factor = addItem(factor, ")");
					pendingCloses--;
					*value = operations(sign, *value, currentNumber);
				}
				else{
					//number = isPrefix ? pushChar(number, ' ') : addChar(number, ' ');
					factor = addItem(factor, number);
					*value = currentNumber;
				}
				
				
				break;
			}

			case ')': //End of subexpression
				return factor;

			//This method doesn't know how to handle this operands, so in this point it has finished its work and return the factor to higher methods (analyzeTerm)
			case '+':
				return factor;

			case '-':
				return factor;

			case '*':
				return factor;

			case '/':
				return factor;

			case 'x':
				return factor;
		}
	}
	return factor;
}

ExpressionPart *analyzeTerm(const char* expr, int* reader, double* value, bool isPrefix){
	ExpressionPart *term = NULL;
	int pendingCloses = 0;
	char sign;
	char* tSign; //Sign represented in string
	while (expr[*reader] != '\0'){
		char tChar = expr[*reader];
		//printf("Term %c\n", tChar);
		switch(tChar){

			case '*':
			{
				sign = tChar;
				tSign = (char*) malloc(2);
				tSign[0] = sign;
				tSign[1] = '\0';
				term = (isPrefix) ? pushItem(term, " ") : addItem(term, " ");
				if (isPrefix){
					term = pushItem(term, tSign);
				}
				term = pushItem(term, "(");
				pendingCloses++;
				(*reader)++;
				break;
			}
			case 'x':
			{
				sign = tChar;
				tSign = (char*) malloc(2);
				tSign[0] = sign;
				tSign[1] = '\0';
				term = (isPrefix) ? pushItem(term, " ") : addItem(term, " ");
				if (isPrefix){
					term = pushItem(term, tSign);
				}
				term = pushItem(term, "(");
				pendingCloses++;
				(*reader)++;
				break;
			}

			case '/':
			{
				sign = tChar;
				tSign = (char*) malloc(2);
				tSign[0] = sign;
				tSign[1] = '\0';
				term = (isPrefix) ? pushItem(term, " ") : addItem(term, " ");
				if (isPrefix){
					term = pushItem(term, tSign);
				}
				term = pushItem(term, "(");
				pendingCloses++;
				(*reader)++;
				break;
			}

			
			default: //This must be the number case or the start of subexpression case (, or even the space case that is handled by analyzeFactor method
			{
				double factorValue = 0;
				ExpressionPart* factor = analyzeFactor(expr, reader, &factorValue, isPrefix);
				//term = addStack(term, factor);
				if (pendingCloses > 0){
					if (isPrefix){
						term = addItem(term, " ");
						term = addStack(term, factor);
					}else{
						term = addStack(term, factor);
						term = addItem(term, " ");
						term = addItem(term, tSign);
					}
					term = addItem(term, ")");
					pendingCloses--;
					*value = operations(sign, *value, factorValue);
				} else {
					term = addStack(term, factor);
					*value = factorValue;
				}
				break;
			}

			case ')': //End of subexpression
				return term;

			//This method doesn't know how to handle this operands, so in this point it has finished its work and return the term to higher methods (analyzeExpression)
			case '+':
				return term;

			case '-':
				return term;
		}
	}
	return term;
}


ExpressionPart *analyzeExpression(const char* expr, int* reader, double* value, bool isPrefix){
	ExpressionPart *expression = NULL;
	int pendingCloses = 0;
	char sign;
	char* tSign;
	while (expr[*reader] != '\0'){
		char tChar = expr[*reader];
		//printf("Expr %c\n", tChar);
		switch(tChar){

			case '+':
			{
				sign = tChar;
				tSign = (char*) malloc(2);
				tSign[0] = sign;
				tSign[1] = '\0';
				expression = (isPrefix) ? pushItem(expression, " ") : addItem(expression, " ");
				if (isPrefix){
					expression = pushItem(expression, tSign);
				}
				expression = pushItem(expression, "(");
				pendingCloses++;
				(*reader)++;
				break;
			}
			case '-':
			{
				sign = tChar;
				tSign = (char*) malloc(2);
				tSign[0] = sign;
				tSign[1] = '\0';
				expression = (isPrefix) ? pushItem(expression, " ") : addItem(expression, " ");
				if (isPrefix){
					expression = pushItem(expression, tSign);
				}
				expression = pushItem(expression, "(");
				pendingCloses++;
				(*reader)++;
				break;
			}
			
			default:  //This must be the number case or the start of subexpression case (, or even the space case that is handled by analyzeFactor method
			{
				double termValue = 0;
				ExpressionPart* term = analyzeTerm(expr, reader, &termValue, isPrefix);
				//expression = addStack(expression, term);
				if (pendingCloses > 0){
					if (isPrefix){
						expression = addItem(expression, " ");
						expression = addStack(expression, term);
					}else{
						expression = addStack(expression, term);
						expression = addItem(expression, " ");
						expression = addItem(expression, tSign);
					}
					expression = addItem(expression, ")");
					pendingCloses--;
					*value = operations(sign, *value, termValue);
				} else {
					expression = addStack(expression, term);
					*value = termValue;
				}
				break;
			}

			case ')': //end of subexpression
				return expression;
		}
	}

	return expression;
}

double operations(char sign, double a, double b){
  switch (sign){
    case '+':
      return a + b;
    case '-':
      return a - b;
    case '*':
      return a * b;
    case '/':
      return a / b;
    case 'x':
      return a * b;
    case '^':
    return exponential(a, b);
  }
  return 0;
}

double exponential(double base, double exp){
	return (exp == 0)? 1 : base*exponential(base, exp - 1);
}

double toNumber(char *number){
	int i= 0;
	double value = 0;
	double divisionFactor = 10;
	bool decimalPointReached = false;
	while (number[i] != '\0'){
		if (number[i] == '.'){
			decimalPointReached = true;
			i++;
		}
		if (decimalPointReached){
			value += (number[i] - 48)/divisionFactor;
			divisionFactor*=10;
		} 
		else{
			value = value*10 + (number[i] - 48);
		}
		i++;
	}
	return value;
}

char *addChar(char *array, char c) {
	size_t sz = strlen(array);
	char *s = (char*) malloc(sz + 2); 
	strcpy(s, array);
	s[sz] = c;
	s[sz + 1] = '\0';
	free(array);
	return (s);
}

char *pushChar(char *array, char c) {
	size_t sz = strlen(array);
	char *s = (char*) malloc(sz + 2); 
	s[0] = c;
	s[1] = '\0';
	strcat(s, array);
	s[sz + 1] = '\0';
	free(array);
	return (s);
}

bool isNumber(char c){
	return (c > 47 && c < 58);
}

char* concat(const char* a, const char*b, bool atStart){
	size_t size = strlen(a) + strlen(b);
	char *str = (char*) malloc(size + 1);
	if (atStart) {
		strcpy(str, b);
		strcat(str, a);
	} else {
		strcpy(str, a);
		strcat(str, b);
	}
	str[size] = '\0';
	return str;
}

void printStack(ExpressionPart* stack){
	ExpressionPart* aux = stack;
	while (aux  != NULL){
		printf("%s", aux-> tPart);
		aux = aux ->next;
	}
	printf("\n");
}