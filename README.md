# Vortex Programming Language
A custom programming language with most basic features (control flow, variables, functions) implemented on top of a custom bytecode interpreter.

# Examples
```
// Variable declarations
x: Float -> 5.0;
y: Float -> 2.0;
message: String -> "Looping...";

// Assignment
x -> x + y;

// Conditional logic
if x > 6.0 {
  print "x is large!";
} else {
  print "x is not that big.";
}

// While loop
counter: Float -> 0.0;
while counter < 3.0 {
  print message;
  counter -> counter + 1.0;
}
```
# Building
You need CMAKE and A C++ Compiler to build this
