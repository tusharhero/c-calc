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

union token {
  char operator;
  int number;
};

struct token_stream {
  char *source;
  union token *tokens;
  int number_of_tokens;
};

bool is_in_set(char character, const char *set_string) {
  for (int i = 0; i < (int)strlen(set_string); ++i) {
    if (character == set_string[i]) {
      return true;
    }
  }
  return false;
}

struct token_stream tokenizer(char *expression) {
  struct token_stream token_stream;
  const char *operators = "+-*/";
  const char *digits = "123456790";

  // making a copy of the expression.
  token_stream.source = malloc(strlen(expression) + 1);
  strcpy(token_stream.source, expression);
  // allocate more memory to tokens.
  token_stream.tokens = malloc(strlen(expression) * 5);

  // allocating memory to a number.
  char *number_string = malloc(strlen(expression) * 2);

  // loop through the expression, and put all the tokens seperately in
  // the tokens array in our struct.
  bool is_digits = false;
  bool is_operator = false;
  char current_character;
  int source_index = 0, tokens_index = 0, number_index = 0;
  int string_length = (int)strlen(token_stream.source);
  while (source_index <= string_length) {
    current_character = token_stream.source[source_index];

    is_digits = is_in_set(current_character, digits);
    is_operator = is_in_set(current_character, operators);
    if (is_operator || (source_index == string_length)) {
      if (number_index != 0) {
        number_string[number_index + 1] = 0;
        token_stream.tokens[tokens_index] = (union token)atoi(number_string);
        is_digits = 0;
        number_index = 0;
        free(number_string);
        number_string = malloc(strlen(expression) * 2);
      } else {
        token_stream.tokens[tokens_index] = (union token)current_character;
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

  token_stream.number_of_tokens = tokens_index - 1;
  return token_stream;
}

int calc(struct token_stream token_stream) {
  int sum = token_stream.tokens[0].number;
  char current_operation = token_stream.tokens[1].operator;
  for (int i = 2; i < token_stream.number_of_tokens; i += 1) {
    if (i % 2 == 0) {
      if (current_operation == '+') {
        sum = sum + token_stream.tokens[i].number;
      }
      if (current_operation == '-') {
        sum = sum - token_stream.tokens[i].number;
      }
      if (current_operation == '*') {
        sum = sum * token_stream.tokens[i].number;
      }
      if (current_operation == '/') {
        sum = sum / token_stream.tokens[i].number;
      }
    } else {
      current_operation = token_stream.tokens[i].operator;
    }
  }
  return sum;
}

int main(void) {
  return 0;
}
