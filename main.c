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

enum Symbol
{
  addition = '+',
  subtraction = '-',
  multiplication = '*',
  division = '/',
};

typedef struct
{
  union
  {
    enum Symbol operator;
    int number;
  } token_value;
  enum
  {
    number, operator} tag;
} Token;

typedef struct
{
  Token *tokens;
  int number_of_tokens;
} TokenStream;

bool
is_in_set (char character, const char *set_string)
{
  for (int i = 0; i < (int)strlen (set_string); ++i)
    {
      if (character == set_string[i])
        {
          return true;
        }
    }
  return false;
}

TokenStream *
tokenizer (char *expression)
{
  TokenStream *token_stream = malloc (sizeof (TokenStream));
  const char *operators = "+-*/";
  const char *digits = "1234567890";

  // allocate more memory to tokens.
  int token_mem_alloc = strlen (expression) * sizeof (char);
  token_stream->tokens = malloc (token_mem_alloc);

  // allocating memory to a number.
  int numstr_mem_alloc = strlen (expression) * sizeof (char);
  char *number_string = malloc (numstr_mem_alloc);

  // loop through the expression, and put all the tokens seperately in
  // the tokens array in our struct.
  bool is_digits = false;
  bool is_operator = false;
  char current_character;
  int source_index = 0, tokens_index = 0, number_index = 0;
  int string_length = (int)strlen (expression);
  while (source_index <= string_length)
    {
      current_character = expression[source_index];

      is_digits = is_in_set (current_character, digits);
      is_operator = is_in_set (current_character, operators);
      if (is_operator || (source_index == string_length))
        {
          if (tokens_index == token_mem_alloc)
            {
              token_mem_alloc *= 2;
              token_stream->tokens
                  = realloc (token_stream->tokens, token_mem_alloc);
            }
          if (number_index == numstr_mem_alloc)
            {
              numstr_mem_alloc *= 2;
              number_string = realloc (number_string, numstr_mem_alloc);
            }
          if (number_index != 0)
            {
              number_string[number_index + 1] = 0;
              token_stream->tokens[tokens_index].token_value.number
                  = atoi (number_string);
              token_stream->tokens[tokens_index].tag = number;
              is_digits = 0;
              number_index = 0;
              free (number_string);
              number_string = malloc (strlen (expression) * 2);
            }
          else
            {
              token_stream->tokens[tokens_index].token_value.
              operator= current_character;
              token_stream->tokens[tokens_index].tag = operator;
              ++source_index;
            }
          ++tokens_index;
        }
      if (is_digits)
        {
          number_string[number_index] = current_character;
          ++number_index;
          ++source_index;
        }
    }

  free (number_string);

  token_stream->number_of_tokens = tokens_index - 1;
  return token_stream;
}

double
calc_tokens (TokenStream token_stream)
{
  enum Symbol current_operation;
  double sum = 0.0;
  Token current_token;
  for (int i = 0; i < token_stream.number_of_tokens; ++i)
    {
      current_token = token_stream.tokens[i];
      if (current_token.tag == operator)
        {
          current_operation = current_token.token_value.operator;
        }
      else if (i == 0 && current_token.tag == number)
        {
          sum += current_token.token_value.number;
        }
      else
        {
          switch (current_operation)
            {
            case addition:
              {
                sum += current_token.token_value.number;
                break;
              }
            case subtraction:
              {
                sum -= current_token.token_value.number;
                break;
              }
            case multiplication:
              {
                sum *= current_token.token_value.number;
                break;
              }
            case division:
              {
                sum /= current_token.token_value.number;
                break;
              }
            }
        }
    }
  return sum;
}

double
calc (char *expression)
{
  TokenStream *token_stream = tokenizer (expression);
  double calculated_value = calc_tokens (*token_stream);
  free (token_stream->tokens);
  free (token_stream);
  return calculated_value;
}

char *
get_line ()
{
  int alloc_size = 1024;
  char *line_ptr = realloc (NULL, sizeof (char) * alloc_size);
  int character;
  int isEOF = false;
  int index;
  for (index = 0; ((character = getchar ()) != '\n'); ++index)
    {
      if (isEOF) {return NULL;}
      if (character == EOF){isEOF = true;}
      if (index == alloc_size)
        {
          alloc_size *= 2;
          line_ptr = realloc (line_ptr, sizeof (char) * alloc_size);
        }
      line_ptr[index] = character;
    }
  line_ptr[index] = 0;
  return line_ptr;
}

int
main (void)
{
  char *input_line;
  while ((input_line = get_line ()) != NULL)
    {
      printf ("%s\n", input_line);
      free (input_line);
    }
  return 0;
}
