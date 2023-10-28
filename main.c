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
  paren_open = '(',
  paren_close = ')',
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
  size_t number_of_tokens;
} TokenStream;

typedef struct
{
  size_t *array;
  size_t length;
} Stack;

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
slice_token_stream (TokenStream token_stream, size_t starting_index,
                    size_t ending_index)
{
  size_t alloc_size = token_stream.number_of_tokens;
  TokenStream *sliced_token_stream = malloc (sizeof (TokenStream));
  sliced_token_stream->tokens = malloc (sizeof (Token) * alloc_size);
  sliced_token_stream->number_of_tokens = ending_index - starting_index;
  for (size_t index = 0; index < sliced_token_stream->number_of_tokens;
       ++index)
    {
      sliced_token_stream->tokens[index]
          = token_stream.tokens[index + starting_index];
    }
  return sliced_token_stream;
}

TokenStream *
tokenizer (char *expression)
{
  TokenStream *token_stream = malloc (sizeof (TokenStream));
  const char *operators = "+-*/()";
  const char *digits = "1234567890";

  // allocate more memory to tokens.
  token_stream->tokens = malloc (strlen (expression) * 10);

  // allocating memory to a number.
  char *number_string = malloc (strlen (expression) * 2);

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

int
calc_token_arithmetic (TokenStream token_stream)
{
  enum Symbol current_operation;
  double sum = 0.0;
  Token current_token;
  for (size_t i = 0; i < token_stream.number_of_tokens; ++i)
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
            default:
              break;
            }
        }
    }
  return sum;
}

size_t
get_no_of_parens (TokenStream token_stream)
{
  size_t no_of_parens = 0;
  for (size_t i = 0; i < token_stream.number_of_tokens; ++i)
    {
      if (token_stream.tokens[i]
              .token_value.
              operator== paren_open && token_stream.tokens[i]
              .tag ==
              operator)
        {
          ++no_of_parens;
        }
    }
  return no_of_parens;
}

double
calc_token_parens (TokenStream token_stream)
{
  if (get_no_of_parens (token_stream) == 0)
    {
      return calc_token_arithmetic (token_stream);
    }
  else
    {
      TokenStream *sliced_token_stream;
      size_t starting_index = -1;
      size_t ending_index = -1;
      bool in_parens = false;

      size_t alloc_size = 256;
      TokenStream *new_token_stream = malloc (sizeof (TokenStream));
      new_token_stream->tokens = malloc (sizeof (Token) * alloc_size);
      new_token_stream->number_of_tokens = 0;

      Stack *opening_stack = malloc (sizeof (Stack));
      opening_stack->array
          = malloc (sizeof (size_t) * token_stream.number_of_tokens);
      opening_stack->length = 0;
      Stack *closing_stack = malloc (sizeof (Stack));
      closing_stack->array
          = malloc (sizeof (size_t) * token_stream.number_of_tokens);
      closing_stack->length = 0;

      for (size_t index = 0; index < token_stream.number_of_tokens; ++index)
        {
          if (alloc_size < index)
            {
              alloc_size *= 2;
              new_token_stream->tokens = realloc (new_token_stream->tokens,
                                                  sizeof (Token) * alloc_size);
            }
          if (token_stream.tokens[index].tag == operator)
            {
              if (token_stream.tokens[index].token_value.operator== paren_open)
                {
                  in_parens = true;
                  starting_index
                      = opening_stack->length == 0 ? index : starting_index;
                  opening_stack->array[opening_stack->length++] = index;
                }
              if (token_stream.tokens[index].token_value.
                  operator== paren_close)
                {
                  closing_stack->array[closing_stack->length++] = index;
                  if (opening_stack->length == closing_stack->length)
                    {
                      opening_stack->length = closing_stack->length = 0;
                      in_parens = false;
                    }
                }
            }
          if (!in_parens)
            {
              if (starting_index != (size_t)-1)
                {
                  ending_index = index;

                  sliced_token_stream = slice_token_stream (
                      token_stream, starting_index + 1, ending_index);

                  new_token_stream->tokens[new_token_stream->number_of_tokens]
                      = (Token){ .token_value.number = (int)calc_token_parens (
                                     *sliced_token_stream),
                                 .tag = number };
                  new_token_stream->number_of_tokens++;

                  free (sliced_token_stream->tokens);
                  free (sliced_token_stream);
                  starting_index = -1;
                }
              else
                {
                  new_token_stream->tokens[new_token_stream->number_of_tokens]
                      = token_stream.tokens[index];
                  new_token_stream->number_of_tokens++;
                }
            }
        }

      double answer = calc_token_arithmetic (*new_token_stream);

      free (opening_stack->array);
      free (opening_stack);
      free (closing_stack->array);
      free (closing_stack);
      free (new_token_stream->tokens);
      free (new_token_stream);
      return answer;
    }
}

double
calc (char *expression)
{
  TokenStream *token_stream = tokenizer (expression);
  double calculated_value = calc_token_parens (*token_stream);
  free (token_stream->tokens);
  free (token_stream);
  return calculated_value;
}

char *
get_line ()
{
  size_t alloc_size = 256;
  char *line = malloc (sizeof (char) * alloc_size);
  size_t size = 0;
  int character;
  while ((character = getchar ()) != '\n' && character != EOF)
    {
      if (size > alloc_size)
        {
          alloc_size *= 2;
          line = realloc (line, sizeof (char) * alloc_size);
        }
      line[size] = character;
      ++size;
    }
  line[size] = 0;
  return line;
}

int
main (void)
{
  char *input_line;
  while ((input_line = get_line ())[0] != 0)
    {
      printf ("%f\n", calc (input_line));
      free (input_line);
    }
  return 0;
}
