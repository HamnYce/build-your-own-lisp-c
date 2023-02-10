# Lispi


### building my own lisp while following:
## https://buildyourownlisp.com/

#### syntax currently is mathematics in polish notation.

syntax examples:
- (+ 1 2) # 1 + 2
- (+ 1 2 3 4 5) # 1 + 2 + 3 + 4 + 5
- (+ 1 (- 3 2)) # 1 + (3 - 2)

## installation / run :
- require make and C-compiler to be installed
- if you do not have make run:
  - "cc -std=c99 -Wall prompt.c mpc.c -ledit -lm -o ./build/prompt"

# features:
(currently minimal but it's coming along :))
- can handle integers and decimals
- math: +, -, *, /, %, ^ (exponentiations)
- functions: min, max # (min 1 2 3 4)

- if you would like any features added please let me know


# bugs:
- if you encounter any bugs also please let me know

hope you find some value :)

