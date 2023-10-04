// The GPLv3 License (GPLv3)
//
// Copyright © 2023 Tushar Maharana
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/* C-calc
 A calculator written in C.*/

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum symbol {
  addition = 0,
  subtraction = 1,
  multiplication = 2,
  division = 3,
};

union token {
  enum symbol operator;
  int number;
};

struct token_stream {
  char *source;
  union token *tokens;
  int number_of_tokens;
};

union token atot(char *token_string) {
  if (token_string[0] == '+') {
    return (union token)(enum symbol)addition;
  }
  if (token_string[0] == '-') {
    return (union token)(enum symbol)subtraction;
  }
  if (token_string[0] == '*') {
    return (union token)(enum symbol)multiplication;
  }
  if (token_string[0] == '/') {
    return (union token)(enum symbol)division;
  } else {
    return (union token)(atoi(token_string) + (int)sizeof(enum symbol));
  }
}

int ttoi(union token number_token) {
  return number_token.number > (int)sizeof(enum symbol)
             ? number_token.number - (int)sizeof(enum symbol)
             : -1;
}

bool is_in_set(char character, const char *set_string) {
  for (int i = 0; i < (int)strlen(set_string); ++i) {
    if (character == set_string[i]) {
      return true;
    }
  }
  return false;
}

struct token_stream *tokenizer(char *expression) {
  struct token_stream *token_stream = malloc(sizeof(struct token_stream));
  const char *operators = "+-*/";
  const char *digits = "1234567890";

  // making a copy of the expression.
  token_stream-> source = malloc(strlen(expression) + 1);
  strcpy(token_stream-> source, expression);
  // allocate more memory to tokens.
  token_stream-> tokens = malloc(strlen(expression) * 5);

  // allocating memory to a number.
  char *number_string = malloc(strlen(expression) * 2);

  // loop through the expression, and put all the tokens seperately in
  // the tokens array in our struct.
  bool is_digits = false;
  bool is_operator = false;
  char current_character;
  int source_index = 0, tokens_index = 0, number_index = 0;
  int string_length = (int)strlen(token_stream-> source);
  while (source_index <= string_length) {
    current_character = token_stream-> source[source_index];

    is_digits = is_in_set(current_character, digits);
    is_operator = is_in_set(current_character, operators);
    if (is_operator || (source_index == string_length)) {
      if (number_index != 0) {
        number_string[number_index + 1] = 0;
        token_stream-> tokens[tokens_index] = atot(number_string);
        is_digits = 0;
        number_index = 0;
        free(number_string);
        number_string = malloc(strlen(expression) * 2);
      } else {
        token_stream-> tokens[tokens_index] = atot(&current_character);
        ++source_index;
      }
      ++tokens_index;
    }
    if (is_digits) {
      number_string[number_index] = current_character;
      ++number_index;
      ++source_index;
    }
  }

  free(number_string);

  token_stream-> number_of_tokens = tokens_index - 1;
  return token_stream;
}

double calc_tokens(struct token_stream token_stream) {
  double sum = (double)ttoi(token_stream.tokens[0]);
  char current_operation = token_stream.tokens[1].operator;
  for (int i = 2; i < token_stream.number_of_tokens; i += 1) {
    if (i % 2 == 0) {
      if (current_operation == (enum symbol)addition) {
        sum += ttoi(token_stream.tokens[i]);
      }
      if (current_operation == (enum symbol)subtraction) {
        sum -= ttoi(token_stream.tokens[i]);
      }
      if (current_operation == (enum symbol)multiplication) {
        sum *= ttoi(token_stream.tokens[i]);
      }
      if (current_operation == (enum symbol)division) {
        sum /= ttoi(token_stream.tokens[i]);
      }
    } else {
      current_operation = token_stream.tokens[i].operator;
    }
  }
  return sum;
}

double calc(char *expression) {
  struct token_stream *token_stream = tokenizer(expression);
  double calculated_value = calc_tokens(*token_stream);
  free(token_stream-> source);
  free(token_stream-> tokens);
  free(token_stream);
  return calculated_value;
}

int main(void) {
  printf("%f\n", calc("6-6+1/2+1*2"));
  return 0;
}
