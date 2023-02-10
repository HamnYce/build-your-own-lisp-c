# Lispi


### Building my own lisp while following on:

## https://buildyourownlisp.com/

### Credits:
Thank you <a href="https://github.com/orangeduck">OrangeDuck</a>
for the <a href="https://github.com/orangeduck/mpc">parser</a> and
creating <a href"https://github.com/orangeduck/BuildYourOwnLisp">this</a>.



#### language currently is mathematics in polish notation.

Syntax Examples:
- (+ 1 2) # 1 + 2
- (+ 1 2 3 4 5) # 1 + 2 + 3 + 4 + 5
- (+ 1 (- 3 2)) # 1 + (3 - 2)

# Installation / Run :
- require make and C-compiler to be installed
- if you do not have make run:
  - "cc -std=c99 -Wall prompt.c mpc.c -ledit -lm -o ./build/prompt"
  - "build/prompt"

# Features:
(currently minimal but it's coming along :))
- can handle integers and decimals
- math: +, -, *, /, %, ^ (exponentiations)
- functions: min, max # (min 1 2 3 4)

- if you would like any features added please let me know


# Bugs:
- if you encounter any bugs also please let me know

hope you find some value in this :)

