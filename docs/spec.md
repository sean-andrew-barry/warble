## 0 Preface

## 1 Introduction

Warble is a modern, expressive programming language designed to combine performance, simplicity, and elegance. Drawing inspiration from a diverse set of languages—including C++, Rust, and JavaScript—Warble prioritizes intuitive syntax, powerful type inference, and efficient, predictable performance. It seeks to offer programmers a language that is both easy to learn and capable of scaling to complex, high-performance applications.

At its core, Warble emphasizes clear semantics, strong type safety, and thoughtful defaults, enabling developers to write concise, readable, and maintainable code without compromising speed or flexibility. Built around innovative features like flexible unions, powerful structured literals, and seamless compile-time optimizations, Warble aims to streamline software development across a wide variety of domains—from systems programming to web development and beyond.

In this specification, you'll find comprehensive explanations and practical examples designed to clearly communicate the design decisions, syntax, and semantics that define Warble. Whether you're new to Warble or an experienced programmer seeking deep insight into its mechanics, this document will serve as your definitive guide.

### 1.1 Five-Minute Tour

Welcome to Warble! This quick tour provides an overview of Warble's essential features and syntax, giving you a feel for what it's like to code with the language.

**Hello World**

Every introduction begins with "Hello, World!":

```warble
print("Hello, World!");
```

Warble keeps it simple, readable, and intuitive.

---

**Declarations and Type Inference**

Warble automatically infers types based on context:

```warble
let number = 42;                       // type defaults to `auto`
let mut message = "Warble is cool";        // explicitly mutable
message = "Still cool!";
```

---

**Structured Data with Literals**

Warble provides clear and concise structured literals:

```warble
let array = [1, 2, 3, 4];
let tuple = (1, "two", true);
let object = { name = "Aura", age = 2 };

print(object.name); // prints "Aura"
```

---

**Functions and Captures**

Warble's functions are intuitive and powerful:

```warble
let multiplier = (factor) {
  return [factor](x) => x * factor;
};

let double = multiplier(2);

print(double(10)); // prints 20
```

Functions can implicitly capture context:

```warble
let factor = 5;
let multiplyByFactor = (x) { return x * factor; };

print(multiplyByFactor(3)); // prints 15
```

---

**Function Calls & Pipeline**

Warble's function calls are built on the pipeline operator `->`, which invokes its right-hand side with its left-hand side:

```warble
"Hello, World!" -> print;
```

Most literals can be placed directly adjacent to a callable value as shorthand:

```warble
print("Hello, World!");                  // tuple shorthand
process{ x = 10, y = 20 };              // object shorthand
vector<i32>;                             // enum shorthand
```

The shorthand form always destructures the literal into parameters. The pipeline form first tries to pass the argument as a whole value:

```warble
func(1, 2);        // always destructures — matches two-parameter overload
(1, 2) -> func;    // first checks for an overload accepting a 2-tuple
```

---

**Pattern Matching**

Warble supports expressive and readable conditional matching:

```warble
let value = something;

if (value)
is (a) { print("Operation successful!"); }
is (b) { print("Operation failed."); }
else { print("Unknown result."); }
```

---

**Unions and Optional Values**

Easily handle optional or uncertain data:

```warble
import {integer} from "primitives" in compiler;

mut optionalValue = 42 || !null; // Create a union with a `null` absence state

optionalValue = null;   // now represents no value
optionalValue = 7;      // set to integer

if (optionalValue)
is (integer) { print("Got an integer: {this}"); }
is (null) { print("No value available."); }
```

---

**Efficient Looping and Ranges**

Clean and intuitive iteration syntax:

```warble
for i in (0..5) { // exclusive range
  print(i); // prints 0, 1, 2, 3, 4
}
```

---

**Composition with Object Spreading**

Warble prefers composition over inheritance:

```warble
let position = { x = 10, y = 20 };
let velocity = { dx = 1, dy = -1 };

let entity = {
  ...position,
  ...velocity,
  name = "Player",
};

print(entity.name); // prints "Player"
print(entity.x);    // prints 10
```

---

**Next Steps**

That's just the beginning! Warble offers many more advanced features and constructs designed to make programming delightful, productive, and performant. Dive deeper into this specification to learn all the details about Warble's design and capabilities.

### 1.2 Philosophy & Goals

Warble was designed with a clear set of guiding principles and objectives, emphasizing simplicity, performance, and clarity above all. These core ideas shape every feature, syntax choice, and design decision, making Warble distinctive yet intuitive. The key principles and goals include:

#### 1. **Simplicity and Clarity**

Warble prioritizes simple, readable, and maintainable code. Language features are designed to be intuitive, familiar, and predictable. Complexity is minimized whenever possible, and thoughtful defaults reduce unnecessary boilerplate.

#### 2. **High Performance**

Warble is built to compete directly with languages renowned for their speed and efficiency, such as C, C++, and Rust. Performance-critical features—like smart compile-time optimizations, efficient memory management, and predictable runtime behavior—are fundamental.

#### 3. **Strong, Static Typing with Type Inference**

Warble ensures type safety at compile time, catching errors early. Powerful type inference capabilities help maintain code brevity and readability, without sacrificing reliability or safety.

#### 4. **Expressive, Declarative Syntax**

The language adopts an expressive syntax that clearly conveys developer intent. Complex ideas and operations can be expressed succinctly and elegantly, resulting in code that reads naturally and reduces cognitive load.

#### 5. **Composition over Inheritance**

Warble intentionally avoids classical inheritance in favor of composition, encouraging flexible, reusable, and maintainable software designs. The ability to easily compose objects using spreading and structured literals promotes clean, modular codebases.

#### 6. **Efficient and Flexible Unions**

Unions (tagged unions) are fundamental to Warble's approach to handling complex data and control flow. The language leverages unions and intuitive conditional-matching syntax to make complex logic more straightforward and safe, enhancing code readability and reducing errors.

#### 7. **Predictable and Transparent Optimizations**

Warble aims to provide transparent, understandable optimizations. Developers can easily predict and understand how their code will perform, with minimal surprises or hidden behaviors from the compiler.

#### 8. **Intuitive Compile-Time Programming**

Metaprogramming and compile-time capabilities are seamlessly integrated into the language. Warble provides robust support for generating code and performing computations during compilation, making it easy to write concise, high-performance programs without excessive boilerplate.

#### 9. **Pragmatic Safety**

Warble promotes safety through pragmatic defaults and strong type checks, but does so without imposing overly restrictive rules or hindering expressiveness. The language aims to make it easy for developers to write correct code, while still offering the flexibility required by advanced users.

#### 10. **Thoughtful and Familiar Ergonomics**

Drawing inspiration from widely-used languages like JavaScript, Rust, and C++, Warble aims to be familiar and comfortable to developers. Features and syntax are selected carefully, often borrowing the best and most intuitive elements from existing programming paradigms.

By adhering to these principles, Warble strives to be a language that is both powerful and approachable, equally suitable for new learners and seasoned professionals seeking robust and efficient tools for modern software development.

## 2 Lexical Structure

Warble's lexical structure defines how source code is broken down into meaningful sequences of characters, known as tokens. These tokens form the basic building blocks of Warble programs.

### 2.1 Character Set & Encoding

Warble source files are Unicode text files encoded in UTF-8. This enables comprehensive support for internationalization, allowing identifiers and string literals to utilize a wide range of Unicode characters. Character literals are specifically stored as 32-bit UTF-32 code points to facilitate direct and efficient manipulation.

### 2.2 Tokens

Tokens are the smallest meaningful units in Warble source code. The Warble compiler breaks input text into a sequence of tokens that it processes according to the language grammar.

Warble tokens include:

* Identifiers (names for variables, functions, and other entities)
* Keywords (reserved words with special meaning)
* Literals (explicit values written directly in the source code)
* Operators and punctuation symbols

#### 2.2.1 Identifiers

Identifiers are used to name variables, functions, properties, and other entities in Warble.

Identifier spelling follows the Unicode identifier stability rules (UAX #31):

* The first code point must have the Unicode property `XID_Start`. Additionally, `_` is permitted as a special-case starter.
* Each subsequent code point must have the Unicode property `XID_Continue` (and `_` is permitted as well).

In particular, ASCII letters (`A-Z`, `a-z`) satisfy `XID_Start`, and ASCII digits (`0-9`) satisfy `XID_Continue`.

Examples of valid identifiers:

```warble
count
_myVariable
Player1
日本語
```

Identifiers are case-sensitive: `Count`, `count`, and `COUNT` are distinct identifiers.

#### 2.2.2 Keywords

Warble reserves a small set of keywords that have special meanings within the language. These keywords cannot be used as identifiers. Examples include:

```
let, mut, const, private, protected, public, export, do, tick, null, undefined, readonly, true, false, return, yield, panic, await, async, pass, fail, try, if, else, is, from, has, as, this, that
```

A full list of reserved keywords is available in Appendix 18.2.

#### 2.2.3 Literals (overview—full details in §4)

Literals represent fixed values directly embedded in Warble source code. They form the simplest expressions in the language, clearly communicating explicit values. Warble defines two categories of literals: **primitive literals** and **structured literals**.

* **Primitive Literals**: Represent simple data values directly.

  * Boolean literals: `true`, `false`
  * Character literals: `'a'`, `'\n'`
  * Numeric literals:
    * Integer: `42`, `0x2A`, `0b101010`
    * Float: `3.14`, `1.0e3`
  * String literals: `"Hello, Warble!"`
  * Marker symbol literals: `undefined`, `null`, `readonly`

* **Structured Literals**: Represent complex data structures.

  * Arrays: `[1, 2, 3]`
  * Enums: `<Red, Green, Blue>`
  * Tuples: `(1, "Warble", true)`
  * Objects: `{ x = 1, y = 2 }`
  * Template strings: `"Value: {x}"`
  * Functions: `[capture](param) => expression`
  * Ranges: `0..10`

Complete details on each literal type, including their syntax and semantics, can be found in **§4 Types**.

### 2.3 Comments & Whitespace

Warble treats whitespace characters (spaces, tabs, and newlines) as token separators. Aside from this role, whitespace is ignored by the compiler, allowing programmers flexibility in formatting and indenting code to enhance readability.

**Comments** allow adding explanatory notes and documentation directly in Warble source code. They are ignored by the compiler and have no effect on program execution. Warble supports two styles of comments:

* **Single-line comments** start with `//` and continue until the end of the line:

  ```warble
  // This is a single-line comment
  let x = 42; // This comment follows a statement
  ```

* **Multi-line comments** are enclosed between `/*` and `*/`. They can span multiple lines and may include other comment markers without special interpretation:

  ```warble
  /*
    This is a multi-line comment.
    It can span multiple lines.
  */
  let y = 10;
  ```

Whitespace and comments do not affect the logic or execution of the program. They are purely for developer clarity and code organization.

### 2.4 Statement Terminators (Semicolons)

Warble uses **explicit semicolons** to mark the end of most statements, but the rule is purposely simple:

| construct                                                                                                             | last syntactic token     | semicolon required?                   | rationale                                                                                                                 |
| --------------------------------------------------------------------------------------------------------------------- | ------------------------ | ------------------------------------- | ------------------------------------------------------------------------------------------------------------------------- |
| **Declaration statements** (`let`, `import`, `register`, `export`, etc.)                                              | *initializer/expression* | **yes**                               | Needed to separate consecutive declarations and to disambiguate from an expression that might follow.                     |
| **Scope-level expression statements** (assignment, function literal, object literal, call, etc.)                      | *expression*             | **yes**                               | The parser cannot know whether another operator or statement will follow; the semicolon closes the expression.            |
| **Compound control-flow statements whose body ends with `}`**<br>(`if…`, `for…`, `while…`, bare `repeat`, `do { } …`) | `}`                      | *optional* (allowed but not required) | The closing brace is an unambiguous terminator; adding a semicolon is accepted for stylistic consistency.                 |
| **`repeat { … } while (condition)` loop**                                                                             | `)`                      | *optional*                            | The closing parenthesis after the condition is the unambiguous terminator; a trailing semicolon is legal but unnecessary. |
| **Empty statement**                                                                                                   | **`;`**                  | —                                     | A lone semicolon is always legal and is occasionally useful as a no-op placeholder (e.g., after a label).                 |

> **Summary rule** – *A semicolon is mandatory after any statement whose final token **could** begin another statement or expression; it is optional when the grammar already supplies an unambiguous closing token (`}` or the final `)` of `repeat … while`).* This covers every case without relying on newline heuristics.

#### Examples

```warble
// 1. Declarations & expressions – semicolon required
let count = 0;
count += 1;
let fn = () { return count; };      // function literal is an expression ⇒ needs ;
let fn2(x) { return x * 2; };       // callable literal shorthand form ⇒ still needs ;

import fs from "filesystem" in compiler;
register pkg from "../pkg";

// 2. Compound statements – semicolon *optional*
if (cond) {
  do {                                // 'do' block terminates on closing brace
    print("inside");
  }
}; // stylistically tolerated, but unnecessary

repeat { work(); } while (needMore()) // final parenthesis already closes the loop
```

## 3 Fundamentals

### 3.1 Declarations

Declarations introduce new bindings into a scope.

Warble supports a small set of **modifier keywords** that may appear before a declaration name:

* `mut`, `const`
* `private`, `protected`, `public`
* `export`

The keyword `let` is **not** a modifier keyword. It is a statement keyword that begins a declaration statement.

Warble classifies declarations into four **declaration modes** depending on context: statement mode, parameter mode, property mode, and capture mode. These modes share a common core syntax but have slightly different rules.

#### 3.1.1 Statement Declaration Mode

Statement declarations appear at scope level (modules, function bodies, and `do {}` blocks).

To disambiguate a declaration from an expression, a statement declaration must begin with either:

* the keyword `let`, or
* at least one modifier keyword.

If `let` is present, it must be **first**, before any modifiers.

Legal:

```warble
let const i = 42;
const i = 42;
mut i = 42;
let export public mut i = 42;
```

Illegal:

```warble
const let i = 42;
```

Conflicting modifiers are a compile-time error (or may be diagnosed as a warning):

```warble
mut const i = 42;
private public i = 42;
```

All bindings are immutable by default. Use `mut` to explicitly declare mutability.

##### Destructuring

Statement declaration mode permits destructuring. There are four destructuring forms:

* **Array destructuring**: `[...]`
* **Tuple destructuring**: `(...)`
* **Enum destructuring**: `<...>`
* **Object destructuring**: `{...}`

Indexed destructuring (arrays/tuples/enums) is positional:

```warble
let [a, b, c] = [1, 2, 3];
let (a, b, c) = (1, true, 'c');
let <a, b> = <A, B>;
```

You may use a rest element to pack the remaining values into a new container of the same kind:

```warble
let [a, ...arr] = [1, 2, 3]; // arr is [2, 3]
```

Nested destructuring is permitted:

```warble
let (a, (b, c)) = (1, (true, 'c'));
```

Indexed destructuring does not allow skipping positions; use `_` when you don't care about a value. You are not required to destructure every value.

Object destructuring is name-based (order-independent):

```warble
let { c, a, b } = { a = 1, b = true, c = 'c' };
```

Object destructuring supports aliasing (and nested destructuring) using `as`:

```warble
let { a as x, b as y, c as z } = { a = 1, b = true, c = 'c' };
let { a, b as { x, y }, c } = { a = 1, b = { x = true, y = "Hello" }, c = 'c' };
```

**Example:**

```warble
// Scope-level declarations require `let` or at least one modifier keyword
let pi = 3.14159;            // immutable by default
let mut count = 0;           // explicitly mutable
count += 10;                 // mutation allowed

// Object literal illustrating both declaration modes
let obj = {
  value,                     // shorthand, object-mode
  "greeting" = "hi",         // literal key, object-mode
  let temp,                  // normal-mode, defaults to undefined
  let [x, y] = [X, Y],       // normal-mode destructuring
};
```

This distinction ensures clear and intuitive syntax, simplifies parsing, and maintains flexibility in declarations.

##### Callable Literal Initializer Shorthand

When a declaration has no constructor annotation (i.e., the annotation is implicitly `: auto`) and the initializer is a **callable literal**, the `=` operator may be omitted. The callable literal is placed directly after the name and any modifiers:

```warble
let double(x) => x * 2;                        // equivalent to: let double = (x) => x * 2;
let add(a, b) { return a + b; };               // equivalent to: let add = (a, b) { return a + b; };
let greet[name](msg) { print(name, msg); };    // equivalent to: let greet = [name](msg) { print(name, msg); };
```

This shorthand applies to any callable literal kind—tuples `()`, arrays `[]`, enums `<>`, strings, characters, and numerics—but it is most naturally used with function literals.

The declaration still requires a trailing semicolon, just like any other declaration statement (see §2.4).

This shorthand is **not** available when a constructor annotation is present. The annotation syntax requires an explicit `=`:

```warble
let add: MyFunc = (a, b) { return a + b; };  // annotation requires explicit `=`
```

#### 3.1.2 Parameter Declaration Mode

Parameter declarations appear inside function parameter lists. Because the compiler already expects declarations there, parameters do not require a leading `let` or a modifier keyword.

Parameter declarations support destructuring, and may include a rest parameter that gathers the remaining arguments into a tuple.

#### 3.1.3 Property Declaration Mode

Property declarations appear inside object literals. Because the compiler already expects declarations there, properties do not require a leading `let` or a modifier keyword.

Property declaration mode forbids destructuring. Using `let` inside an object literal switches that declaration into statement declaration mode (and enables destructuring).

Property declarations support shorthand, plus shorthand-prefix operators:

* `@`, `&`, `*`, `$`, `...`

#### 3.1.4 Capture Declaration Mode

Capture lists are a comma-separated list of capture declarations. The full set of capture rules is still being refined.

#### 3.1.5 Constructor Annotations

A declaration may include a **constructor annotation** using the colon syntax:

```warble
let name: Ctor = value;
```

The annotation expression (`Ctor`) is evaluated at compile time to produce a callable. At runtime the compiler initializes `name` by invoking that callable via the **pipeline operator**:

```warble
value -> Ctor
```

This is the formal definition. The compiler does **not** wrap the initializer in a container to call it via the shorthand form, as that would subtly change the meaning. Concretely:

```warble
const p: Point = { x = 1, y = 2 };
// Initialized as: { x = 1, y = 2 } -> Point
// NOT as:         Point({ x = 1, y = 2 })
```

Using the pipeline operator means the two-phase resolution rules (§6.4) apply normally. In the example above, phase 1 checks for a `Point` overload that accepts the object as a whole value. If none is found, phase 2 destructures the object and looks for a `Point` overload with parameters named `x` and `y`. Had the compiler used the shorthand form `Point({...})` instead, phase 1 would be skipped and the object would always be passed as a single argument—a meaningfully different call.

##### The Annotation Is an Expression, Not a Type

Warble has no `class` or `struct` keywords. There are no nominal types, no compiler-recognized constructor forms, and no dedicated syntax for defining a type. The annotation is simply an **arbitrary compile-time expression** that evaluates to a callable. Any function can fill this role:

```warble
let x: i32 = 42;              // i32 is a built-in callable that produces an integer
let p: Point = { x=1, y=2 }; // Point is a user-defined factory function
let v: vector<i32> = [];      // vector<i32> is a partially-applied function call
```

Functions used this way are conventionally called **constructors**, but this is purely a naming convention. They are not special to the compiler in any way—they are just factory functions that accept some input and return an initialized value. See §8.5 for the constructor pattern in detail.

#### 3.1.6 Shadowing

Shadowing occurs whenever a new declaration uses the same name as a previous declaration. This is permitted not only across nested scopes but also within the same scope:

```warble
// Shadowing within the same scope:
let value = 42;
let value = "hello";
let value = true;
```

Each subsequent declaration of the same name "shadows" the previous ones. A lookup like `print(value)` refers exclusively to the most recent declaration, effectively making earlier declarations of the same name invisible from that point onward.

Shadowing allows you to create a sequence of updated immutable values clearly and safely, enabling code that behaves similarly to mutation but preserves immutability. This works because, in Warble, the right-hand side of a declaration executes before the name on the left-hand side is introduced, allowing the declaration to refer to the previous binding of the same name:

```warble
let value = 10;
let value = value * 10;
print(value); // Prints "100"
```

In this example, the second declaration's right-hand side sees the first declaration's binding (`10`) and uses it to calculate the new binding (`100`). The compiler optimizes memory usage efficiently by reusing the same storage, as it easily identifies that the lifetimes of these shadowed bindings do not overlap.

**Note on Lifetimes:**
In Warble, the lifetime of a declaration begins at its first usage and ends immediately after its last usage. Thus, shadowed bindings with non-overlapping lifetimes can efficiently reuse memory. This behavior contributes significantly to Warble's performance and clarity.

### 3.2 Scopes & Name Lookup

Scopes in Warble define the visibility and lifetime of declared identifiers, helping organize code into manageable contexts. Warble scopes are created by:

* **Modules** (the top-level scope of a file)
* **Functions**
* **Loops** (`for`, `while`, `repeat-while`)
* **Conditionals** (`if`, `else`)
* Explicit **`do`** blocks

Unlike many curly-brace languages, Warble **does not** interpret standalone curly braces (`{ ... }`) as scope blocks. Instead, Warble uses the explicit `do` keyword to introduce scope blocks, eliminating ambiguity with object literals:

```warble
let x = 10;

do {
  let x = 20; // shadows outer `x` within this block
  print(x);     // prints 20
}

print(x);       // prints 10
```

**Name lookup** in Warble follows a straightforward rule: when you reference an identifier, Warble searches for the most recent declaration of that identifier, starting from the current scope and moving outward through parent scopes. This lookup proceeds in reverse declaration order, meaning the latest matching declaration shadows any previous declarations with the same name.

Shadowing also applies within the same scope:

```warble
let a = 1;
let a = 2; // shadows previous declaration of `a`
print(a);    // prints 2
```

The earlier `a` remains in memory and available via reflection but is inaccessible through standard identifier lookup from this point forward.

### 3.3 Semicolons vs. Empty Literals

In Warble, the curly brace notation `{}` always represents an **object literal**, never a scope or a block of statements. This contrasts with languages like C or JavaScript, where `{}` can represent an empty scope block.

To clearly differentiate scopes from object literals:

* **Empty object literal**:

  ```warble
  let emptyObject = {}; // empty object literal, must end with a semicolon
  ```

* **Empty scope block**:

  ```warble
  do {
    // This is an empty scope block
  }
  ```

A top-level empty object literal used as a standalone statement requires a terminating semicolon:

```warble
{}; // standalone empty object literal, semicolon required
```

The explicit requirement of the `do` keyword for scopes and semicolons for literals clarifies parsing rules and avoids ambiguity, maintaining Warble's commitment to simplicity and explicitness.

## 4 Types

### 4.1 Literal Types

Literals in Warble represent explicit, directly-written values within the source code. They form the foundational building blocks of expressions and declarations throughout the language, serving as the simplest way to express data clearly and explicitly.

Warble provides a variety of literal types, broadly categorized into primitive and structured literals. Primitive literals, such as boolean, integer, float, character, and string, represent fundamental data values. Structured literals, including arrays, template strings, enums, tuples, objects, functions, and ranges, offer powerful and flexible ways to organize and represent more complex data patterns.

Each literal type is carefully designed to be both intuitive and expressive, balancing familiarity from other programming languages with Warble's distinctive features and syntactic clarity. The sections that follow explain each literal type in detail, along with their syntax, semantics, and notable characteristics.

#### 4.1.1 Boolean

Boolean literals represent simple truth values and are stored as 8-bit values. Warble includes two keyword literals to represent boolean values: `true` and `false`. These behave exactly as you would expect, with no deviations from conventional boolean logic.

**Examples:**

```warble
let isReady = true;
let hasFailed = false;
```

#### 4.1.2 Marker Symbols (`undefined`, `null`, `readonly`)

Warble has three keyword literals that evaluate to compiler-defined **symbols** rather than normal runtime values:

* `undefined`
* `null`
* `readonly`

Unlike `true` and `false`, which are ordinary boolean values, these three literals evaluate to symbols and are primarily used as markers.

These are **marker symbols**. Their primary job is to act as flags and state labels for other constructs (especially unions), not to provide meaningful value bits.

This distinction matters because the same raw value bits can represent many different concepts depending on the active symbol/tag. For example, the integer `0` and the marker `null` may both be all-zero in memory, but they are different concepts and are never treated as interchangeable.

##### Union-Tag Semantics

When a marker symbol appears as an arm of a union, comparing the union to that marker is a **tag test**.

For example, testing a union against `null` asks whether the union's active tag indicates the `null` arm is active. It does not inspect (or validate) the union's value payload.

Similarly, transitioning a union into a marker state (for example assigning `null`) only updates the tag. The compiler does not waste time zeroing or rewriting the value slot, because the payload is not meaningful in a marker state.

When a union is in a marker state, the union's value slot is not meaningful and must not be accessed.

##### `null`

The keyword literal `null` evaluates to the compiler-defined marker symbol of kind `Null`.

Functions that do not explicitly return a value implicitly return `null`. The following forms are equivalent:

```warble
return;
return null;
```

##### Optional Values

Warble is strongly typed, and normal values are never implicitly nullable. Optionality is expressed with unions.

An "optional" in Warble is not a distinct type; it is just a common union pattern:

* Exactly one passing arm (the value).
* Exactly one failing arm (the absence case), commonly failing `null`.

In this specification, failing arms are written with a leading `!` marker in type notation. For example, an optional `T` is written as `!null | T`.

##### `undefined`

The keyword `undefined` evaluates to a dedicated marker symbol of kind `Undefined`.

This is intentionally simple and extremely useful internally: many fields can default to `undefined` via zero-initialization.

Common uses include:

* A symbol with no name may have its `name` slot set to `undefined`.
* A declaration that defaults its initializer uses `undefined` to mean "not provided / not set / not present".

##### `readonly`

The keyword `readonly` evaluates to a marker symbol used as an interface-policy contract.

Libraries may accept `readonly` as a flag to request that a dynamic container present an immutable interface. For example, a standard library vector constructed with a `readonly` policy must not permit slot mutation (elements may be initialized and destroyed, but not updated):

```warble
let const ints = vector<i32, readonly>();
```

#### 4.1.3 Character

Character literals represent a single Unicode code point, stored as a 32-bit UTF-32 value. Character literals must use single quotes (`'`) and may only contain one Unicode code point.

Multiple characters are allowed only if they form a valid escape sequence representing a single Unicode code point.

Warble supports the following escape sequences:

* Basic single character escape: `'\X'` (e.g., `'\n'`)
* Hexadecimal escape: `'\xHH'` (exactly 2 hex digits)
* Unicode code point escape: `'\uHHHH'` (exactly 4 hex digits)
* Unicode code point extended escape: `'\u{HHHHHH}'` (1 to 6 hex digits)

**Examples:**

```warble
let newline = '\n';
let heart = '\u2764';
let smiley = '\u{1F600}';
```

Invalid example (multiple code points):

```warble
// let invalidChar = 'ab'; // Error: Multiple code points
```

#### 4.1.4 Numeric (Integer, Float)

##### Integer Literals

Integer literals represent whole numbers. Numeric literals without a decimal point are classified as integers.

Semantically, integer literals behave as arbitrary-precision integers.

Internally, integer literals default to a BigInt-style representation: their value is stored in the module's per-module data buffer as a sequence of 32-bit limbs (`u32`). The integer literal symbol then references the corresponding limb slice.

Warble supports multiple integer forms:

* Binary literals (`0b`): `0b1101`
* Octal literals (`0o`): `0o755`
* Hexadecimal literals (`0x`): `0x1A3F`

Case is irrelevant for both form prefixes and hexadecimal digits.

##### Float Literals

Float literals represent floating-point numbers. Float literals are identified by the presence of a single decimal point (`.`).

Semantically, float literals behave as arbitrary-precision floating-point values.

Internally, float literals default to a BigFloat-style representation. Their significand (mantissa) is stored as a sequence of 32-bit limbs (`u32`) in the module data buffer, followed immediately by the exponent encoded as two additional `u32` limbs.

Floats also support exponential notation (`0.255e3`). Case is irrelevant for the exponent marker (`e`).

Float literals must begin and end with a digit (`0.0` rather than `.0` or `0.`). Similarly, exponential notation must be surrounded by digits (`1.0e2` rather than `.1e2` or `1.e2`).

##### Numeric Literal Separators

All numeric literals (integers and floats) can use underscores (`_`) as separators to improve readability. Underscores have no effect on the actual value and cannot start or end a numeric literal.

**Examples:**

```warble
let largeInteger = 1_000_000;
let binaryLiteral = 0b1010_1101;
let hexLiteral = 0xDE_AD_BE_EF;
let floatValue = 123_456.789_012;
let scientificFloat = 1.23e4;
```

##### Summary of Syntax Rules:

* Floats must begin and end with digits (no `.5` or `5.`; use `0.5` or `5.0`).
* Exponential notation (`e`) must be between digits (e.g., `1.0e2`).
* Numeric separators (`_`) must not lead or trail literals.
* Integers can end in valid hex digits, including alphabetic characters for hexadecimal literals.

#### 4.1.5 String

String literals in Warble represent textual data. Despite their conceptual similarity to arrays of characters, they are classified as primitive literals: they store their content as a slice into a per-module data buffer rather than as child symbols, and they cannot be destructured.

##### Syntax and Representation

String literals use double quotes (`"`) to denote the start and end of the string:

```warble
let greeting = "Hello, Warble!";
```

String literals may contain line breaks.

To use `{` or `}` as literal characters inside any string, you must escape them: `\{` and `\}`.

If a quoted string contains one or more unescaped template expressions of the form `{ expression }`, it is treated as a **template string**. Template strings are still written with double quotes, but they are structured values rather than plain text.

Internally, the compiler stores string data (including identifiers) in a large, per-module data buffer. String data is stored as UTF-32 code points (32-bit values) inside this shared buffer.

Plain string literal symbols (kind `String`) store their text by encoding a data slice in their payload:

* **Payload**: A `std::pair<uint32_t, uint32_t>` of (data start index, length).

##### Template Strings

Template strings in Warble are specialized structured containers designed to mix literal text chunks with embedded expressions. They are not flattened into a single string; they behave more like a tuple whose elements alternate between string chunks and arbitrary expression values.

```warble
let greeting = "Hello, {name}! You have {unreadCount} unread messages.";
```

Within the curly braces, you can embed expressions of any type. The sections outside the braces become string chunks, while the embedded expressions retain their original types without automatic stringification.

###### Internal Representation and Type Distinction

Internally, a template string is structured similarly to a tuple literal—an indexed sequence of values. The string portions between template expressions become regular string chunks, while each embedded value is preserved exactly as-is, maintaining its original type.

A template string literal is kind `TemplateString` and, like other structured literals, typically encodes a child slice in its payload (start index and length in the preorder symbol table).

```warble
let count = 42;
let template = "You have {count} messages.";
// Internally: tuple ("You have ", 42, " messages.")
```

###### Stringification Responsibility

Template string literals do **not** automatically stringify their embedded values. They simply store the original values directly. The responsibility for converting these values into a meaningful textual representation lies solely with whatever function or context consumes the template string.

For example, if you pass a template string to a printing function, that function must explicitly handle stringifying non-string values:

```warble
print("The answer is {42}");
// The print function itself decides how to convert 42 into "42"
```

###### Interaction with Tuples

Because template strings structurally resemble tuples, you can freely spread a template string into a tuple literal:

```warble
let tpl = "Value: {10}";
let tplAsTuple = (...tpl);
// tplAsTuple becomes ("Value: ", 10)
```

##### Escape Sequences

String literals support the same set of escape sequences as character literals:

* Basic single character escape: `"\X"` (e.g., `"\n"`)
* Hexadecimal escape: `"\xHH"` (exactly 2 hex digits)
* Unicode code point escape: `"\uHHHH"` (exactly 4 hex digits)
* Unicode code point extended escape: `"\u{HHHHHH}"` (1 to 6 hex digits)

Example:

```warble
let message = "Line 1\nLine 2\tTabbed";
let unicodeExample = "I \u2764 Warble!";
```

##### Interaction with Arrays

Because strings are effectively specialized arrays of characters, they can be directly converted into arrays of characters through the spread syntax (`...`). This conversion is allowed explicitly due to the close relationship between strings and arrays:

```warble
let charArray = [..."Warble"];
// charArray is now an array of UTF-32 characters: ['W', 'a', 'r', 'b', 'l', 'e']
```

Note that this behavior is a special allowance. Generally, spreading one type of structured literal into another incompatible type (like spreading an object into an array) is not permitted. However, due to their internal relationship, strings naturally and conveniently support this conversion to arrays.

##### Summary of String Literal Features:

* Defined using double quotes (`"`).
* May contain line breaks.
* `{` and `}` must be escaped (`\{`, `\}`) unless used for a template expression.
* Stored internally as UTF-32 characters in a per-module data buffer.
* Plain strings (kind `String`) encode a `(data_start, length)` slice in their payload.
* Support standard escape sequences consistent with character literals.
* Convertible to arrays of characters using spread syntax (`[...]`).
* If it contains one or more unescaped `{ expression }` portions, it is a template string (kind `TemplateString`): a structured sequence of string chunks and embedded values.

#### 4.1.6 Array

Array literals represent sequential collections of elements, where each element shares a common data type. The type of an array is always inferred from its first element. Once the type is defined by this initial element, every subsequent element must either match this type exactly or be implicitly convertible to it.

For instance, this is valid:

```warble
let nums = [0u8, 1, 2, 3];
```

Here, the first element explicitly defines the array as an array of unsigned 8-bit integers (`u8`). The following elements, although lacking explicit type suffixes, are implicitly converted to match the first element's type.

In contrast, the following is invalid because the second element (`null`) is a marker symbol, which cannot be converted to a boolean:

```warble
// Invalid example:
let flags = [true, null]; // Error: Cannot place `null` into boolean array
```

##### Runtime Data vs. Compile-time Symbol Structure

Structured symbols in Warble record their compile-time structure as a slice of child symbols (typically encoded in the symbol's payload as a start index and length into a preorder traversal). For explicitly enumerated arrays, each child directly corresponds to an individual element. However, it's crucial to distinguish between this compile-time symbol representation and the actual runtime data structure.

Consider an array explicitly enumerating its elements:

```warble
let explicitArray = [1, 2, 3];
```

This array symbol will have three children, each representing the integer values `1`, `2`, and `3`.

##### Special Syntax: Repeated Array Initialization

Warble provides a convenient syntax (borrowed from Rust) to initialize arrays with repeated elements, useful for large arrays:

```warble
let repeatedArray = [0; 100]; // Creates an array of 100 zeros
```

This syntax indicates a single value (`0`) repeated to fill an array of length specified by a compile-time constant (`100`). The runtime memory footprint matches an array of 100 elements, but its compile-time structure will only have a single child representing the repeated element, rather than explicitly listing each element. Such arrays are marked internally with the `Repeat` flag.

##### Spreading Arrays into Other Arrays

Warble also supports array expansion or "spreading" via the `...` syntax:

```warble
let firstArray = [1, 2];
let combinedArray = [0, ...firstArray, 3];
```

In this example, the array `combinedArray` contains `[0, 1, 2, 3]` at runtime. However, its compile-time child slice differs for efficiency reasons. Instead of cloning each element from `firstArray`, the symbol representing `combinedArray` will have three child entries:

* The integer literal `0`
* A single symbol referencing the array `firstArray` marked with the `Spread` flag
* The integer literal `3`

This avoids excessive duplication in the compiler's internal representation, especially important for large arrays.

##### Empty Arrays

Empty arrays are valid and simply contain no children in their compile-time representation:

```warble
let emptyArray = [];
```

Such arrays initially have no defined element type but can still be validly used in contexts where the element type is inferred later from usage.

##### Summary of Array Literal Rules:

* **Type Definition**: Determined by the first element. All subsequent elements must match or convert implicitly.
* **Runtime vs. Compile-time Structure**: The compile-time child slice describes structure but is not a direct one-to-one mapping to runtime data, especially for repeated arrays and spread syntax.
* **Special Flags**:
  * `Repeat`: Indicates repeated-initialization syntax `[value; count]`.
  * `Spread`: Indicates array spreading (`...[otherArray]`).
* **Empty Arrays**: Legal with zero children and initially undefined type.

#### 4.1.7 Enum

Enum literals in Warble represent sequences of symbols. Like string literals, enums are specialized subtypes of arrays designed explicitly to hold references to existing symbols. They provide a convenient, efficient way to represent collections of symbolic identifiers.

##### Syntax and Representation

Enum literals use angle brackets (`< >`) as their defining syntax:

```warble
let colors = <Red, Green, Blue>;
```

Each entry inside an enum literal must already exist as a defined symbol. If you attempt to reference an undeclared identifier within an enum literal, you will encounter a compilation error:

```warble
// Error example:
let invalidEnum = <Apple, Banana>; // Error if Apple or Banana aren't previously defined
```

This behavior emphasizes a critical difference between Warble enums and traditional enums found in languages like C++ or Java. Warble enums reference pre-existing identifiers rather than creating new symbolic names. If your goal is to define a new set of identifiers (similar to a traditional `enum class`), you should instead use object literals (covered in a later section).

##### Internal Storage and Implementation

Each symbol reference within an enum literal is internally stored as a 32-bit unsigned index into the module's symbol table. Thus, enums consist entirely of these symbol indices rather than direct runtime values.

Enum literal payloads are stored in the same per-module data buffer used for string data. For enums, the data slice is a sequence of 32-bit symbol indexes.

Enum symbols encode their data slice in their payload:

* **Payload**: A `std::pair<uint32_t, uint32_t>` of (data start index, length).

##### Interaction with Arrays

Similar to strings, enum literals can be spread directly into arrays. This is explicitly permitted due to their inherent structural similarity. Enums are essentially considered a specialized form of array, and thus they naturally support this type of conversion:

```warble
let enumValues = <One, Two, Three>;
let arrayOfSymbols = [...enumValues];
// arrayOfSymbols now contains [$One, $Two, $Three]
```

##### Distinction from Arrays of Symbols

Despite this close relationship, there is a significant difference between enums and runtime arrays of symbols:

* **Enums**: Represent statically allocated sequences referencing existing symbols. They use specialized internal buffers optimized by the compiler.
* **Arrays of Symbols**: Represent runtime-local arrays explicitly created and initialized at runtime.

Example illustrating this difference clearly:

```warble
let enumExample = <a, b, c>;     // Enum literal (static, symbolic references)
let arrayExample = [$a, $b, $c]; // Array of symbols (runtime-local memory)
```

##### Summary of Enum Literal Features:

* Defined using angle brackets (`< >`).
* Must reference previously defined symbols (no declarations within enum literals).
* Internally represented using specialized static buffers, similar to strings.
* Efficiently represents sequences of existing symbolic references.
* Convertible directly to arrays of symbols using spread syntax (`[...]`).

#### 4.1.8 Tuple

Tuples in Warble represent flexible, sequential, indexable containers capable of holding elements of varying types. Unlike arrays, enums, and strings, tuples impose no requirement for uniformity among their contained values. Each element within a tuple can independently be of any type.

##### Syntax and Representation

Tuple literals are enclosed using parentheses `( )`:

```warble
let exampleTuple = (1, true, "Warble");
```

This tuple contains three elements, each of different types: an integer, a boolean, and a string literal.

##### Parentheses and Tuples

Warble uses parentheses in a few different roles.

In many statements, parentheses are **mandatory structural delimiters** that simply wrap an expression. They do not create a tuple and have no meaning beyond delimiting the expression. This includes (but is not limited to) `if ( ... )`, `while ( ... )`, and the iterable portion of `for x in ( ... )`.

Because these statement forms require parentheses, you will often see *two* sets of parentheses when the expression itself begins with a tuple literal:

```warble
if ((true)) {
  // The outer parentheses are `if ( ... )` structural delimiters.
  // The inner parentheses form the tuple literal `(true)`.
}
```

Parentheses also appear in declarations as part of **tuple destructuring** (e.g. `let (a, b) = value;`). These are easy to recognize because they occur in declaration syntax rather than expression syntax.

In expression syntax, parentheses denote a **tuple literal**.

Unlike many languages, Warble does not require a trailing comma for single-element tuples: both `()` and `(a)` are tuples.

When parentheses appear in an expression, the parser first attempts to interpret them as a function literal's parameter list. If the parenthesized list is followed by a function body (`{ ... }`) or an arrow marker (`=>`), it is a parameter list; otherwise it is a tuple literal.

##### Spreading into Tuples

Tuples are the most permissive container type in Warble, capable of accepting elements of varying types and sources. Because of this, you can freely spread arrays, enums, and strings into tuples:

```warble
let arrayExample = [1, 2, 3];
let stringExample = "Warble";
let enumExample = <Red, Green>;

let combinedTuple = (...arrayExample, ..."Warble", ...enumExample, true, 3.14);
```

This works naturally due to tuples' inherent flexibility. They do not impose strict type requirements, making them a convenient "catch-all" container.

However, this flexibility is deliberately one-directional. While you can spread arrays, enums, and strings into tuples, you cannot spread tuples back into stricter container types such as arrays or enums—even if the tuple technically meets the type requirements:

```warble
let tupleOfInts = (1, 2, 3);

// Invalid:
let arrayOfInts = [...tupleOfInts]; // Not permitted, even though the types are valid.
```

This restriction ensures clarity and prevents confusion by keeping automatic conversions intuitive: from stricter containers to the more flexible tuple. To perform the reverse, you must explicitly convert or destructure the tuple manually.

##### Summary of Tuple Features

* Defined using parentheses in expression syntax: `( ... )`.
* `()` and `(a)` are tuples (no trailing comma requirement).
* In statement forms like `if ( ... )`, parentheses are structural delimiters, not tuple constructors.
* Parenthesized lists are parsed as function parameters only when immediately followed by `{ ... }` or `=>`.
* Supports mixed element types with no uniformity constraints.
* Allow spreading arrays, enums, and strings into tuples.
* Do not allow spreading tuples into stricter containers like arrays or enums.

#### 4.1.9 Object

Object literals are the most versatile and complex type of literal in Warble. They represent structured, sequential containers of named declarations. While similar in concept to tuples (a sequential container of arbitrary types), objects differ fundamentally by using named lookups instead of numeric indexing.

##### Basic Syntax and Declarations

Object literals use curly braces `{ }` to enclose their declarations, with each declaration separated by a comma:

```warble
let point = { x = 10, y = 20 };
```

Declarations inside object literals are parsed in one of two declaration modes:

* **Object mode** (default)
  * Begins with an identifier, enum literal, or string literal and does **not** use the `let` keyword.
  * Allows shorthand declarations (`foo` ⇢ `foo = foo`).
  * Allows a number of prefixes to augment shorthand declarations, such as `{ @value }` being `{ value = @value }`.
  * **Forbids destructuring**—tokens such as `<a, b>` are taken as a single literal name.

* **Normal mode**
  * Begins with the keyword `let`.
  * Behaves exactly like a scope-level declaration: identifier keys only, destructuring allowed, no shorthand (omitting the RHS defaults to `undefined`).

In either mode visibility and mutability are controlled via modifier keywords such as `private`, `protected`, `public`, `mut`, and `const`.

For example:

```warble
let name = "Sean";

let user = {
  name, // object-mode shorthand
  mut age = 32,            // mutable
  private email = "...",  // visibility modifier

  <MyTag> = (){}, // object-mode enum key
  "label" = "OK", // object-mode string key

  let counter = 0,         // normal-mode
  let <lhs, rhs> = <A, B>, // normal-mode destructuring
};
```

##### Internal Representation and Lookup

Each declaration inside an object literal becomes a child symbol of the object, stored within its child slice. Children are listed strictly in declaration order.

Lookups for object properties (`obj.a`) search backward through that child slice, beginning at the end, to find a symbol matching the requested name. This backward lookup allows for shadowing of properties, enabling properties defined later to override earlier ones with the same name:


```warble
let obj = {
  a = 1,
  a = 2, // shadows the first `a`
};

obj.a; // resolves to the second declaration (value: 2)
```

This mechanism also supports function overloads, allowing multiple functions with identical names but differing signatures to coexist:

```warble
let mathOps = {
  add = (a, b){ return a + b; },
  add = (a, b, c){ return a + b + c; }, // overloads previous `add`
};
```

During lookup, Warble initially hits the most recent definition (the last listed). If the immediate match is not compatible (such as due to differing function parameters), the compiler continues searching backward for a suitable overload.

##### Topic Binding (`this` / `that`)

When the compiler begins building an object literal expression, it creates a symbol to represent the object literal and pushes that symbol index onto an internal stack known as the **topic stack**.

Within the body of the object literal (while that object literal remains the active topic), the keywords `this` and `that` provide access to this topic information:

* `this` resolves to the topic symbol at the top of the topic stack.
* `that` resolves to the topic symbol one below the top of the topic stack.

For object literals, this means:

* Within an object literal body (when no nested topic expression is active), `this` refers to the symbol for that object literal.
* Inside a nested object literal, `this` refers to the inner object literal, and `that` refers to the immediately enclosing topic (commonly the outer object literal).

Examples:

```warble
let obj = {
  a = 1,
  b = this.a,
};

let nested = {
  a = 10,
  inner = {
    a = 20,
    // `this` is the inner object literal; `that` is the outer object literal.
    outer_a = that.a,
    inner_a = this.a,
  },
};
```

The topic stack is not otherwise accessible; `this` and `that` are the only way to refer to it.

Using `this` when the topic stack is empty is a compile-time error. Using `that` when the topic stack contains fewer than two entries is a compile-time error.

##### Spreading Objects

Warble emphasizes composition over inheritance, making object spreading an essential feature. You can spread one object literal's properties into another using the spread syntax (`...`):

```warble
let position = { x = 0, y = 0 };
let velocity = { vx = 1, vy = 1 };

let movingObject = {
  ...position,
  ...velocity,
  name = "Player",
};
```

Internally, this spreading does not deeply clone each property. Instead, it adds a single symbol of kind `Object`, flagged with `Spread`, to the target's child slice. During property lookups, the compiler transparently recognizes these spread symbols, recursively searching their contained properties as needed.

Spreading can include visibility modifiers as well:

```warble
let obj = {
  private ...secretProperties,
  public ...publicProperties,
};
```

Note: Only object literals can be spread into object literals; other containers (arrays, enums, tuples) cannot, due to their lack of named properties.

Spreading is actually just shorthand for `{ obj = ...obj }`, just like how `{ @value }` is shorthand for `{ value = @value }`.

##### Enum-like Properties via Range Destructuring

Warble offers a concise and flexible alternative to traditional enum-class style auto-incremented properties through destructuring of range literals. Instead of using special prefix notation, you can simply destructure a compile-time constant range:

```warble
let Colors = {
  let [Red, Green, Blue] = 0..3,
};
```

This expands equivalently to:

```warble
let Colors = {
  Red = 0,
  Green = 1,
  Blue = 2,
};
```

The range literal used for destructuring must have a compile-time known start and end. Attempting to destructure more elements than the range contains, or using a range with runtime-dependent bounds, results in a compile-time error:

```warble
// Error: Range has only one value to destructure but two are required
let [a, b] = 0..1;

// Error: Range bounds are not compile-time constants
let dynamicEnd = getDynamicEnd();
let [x, y] = 0..dynamicEnd;
```

This design intentionally does not allow skipping elements within a single destructuring, but you can easily achieve similar results by using multiple ranges:

```warble
let Indices = {
  let [First, Second] = 0..2,
  let [Fourth] = 3..4,
};
// Results in First=0, Second=1, Fourth=3
```

##### Summary of Object Literal Features:

* Defined using curly braces `{ }`.
* Consist of named declarations with two different syntax modes, depending on if the `let` keyword is used or not. Properties are immutable by default; use `mut` for mutability.
* Support modifier keywords (`public`, `private`, `protected`, `mut`, `const`).
* Allow shadowing and overloads via backward lookup.
* Allow property names defined as strings or enums.
* Enable composition via object spreading (`...`).

#### 4.1.10 Range

Range literals provide a concise syntax for defining numeric intervals to be iterated over. They serve primarily as simple iterators for loops or similar iteration contexts.

##### Syntax

Range literals use a single form:

* `start..end` (end-exclusive)

Both `start` and `end` must resolve to numeric values at compile time, but they do not have to be literal constants. Identifiers and even more complex expressions are allowed, as long as they ultimately produce numeric values compatible with iteration.

##### Examples

A basic example of an exclusive range used in iteration:

```warble
// Iterates from 0 up to (but not including) 10
for let i in (0..10) {
  print(i);
}
```

To iterate through an endpoint value, write the end as one past the last desired value:

```warble
// Iterates 1, 2, 3, 4, 5
for value in (1..6) {
  print(value);
}
```

##### Summary of Range Literal Features:

* Defined using `start..end` (end-exclusive).
* Accept numeric literals, identifiers, or more complex numeric expressions.
* Primarily used for concise numeric iteration.

#### 4.1.11 Function

Function literals in Warble represent first-class functions defined directly as values, inspired primarily by C++ lambda syntax. Unlike traditional function definitions found in many languages, Warble treats all functions as literals, allowing flexibility in capturing context, declaring parameters, and defining behavior.

##### Syntax Overview

Function literals have three main parts, closely mirroring C++ lambdas:

* **Capture list**: Defined with square brackets (`[ ]`). *(Optional)*
* **Parameter list**: Defined with parentheses (`( )`). *(Optional, but at least one of captures or parameters is required)*
* **Function body**: Defined with curly braces (`{ }`). *(Required, except in shorthand arrow syntax)*

Examples:

```warble
// Full syntax with captures and parameters
let add = [x](y) { return x + y; };

// Captures omitted (implicit capture)
let square = (x) { return x * x; };

// Parameters omitted (captures required)
let printX = [x] { print(x); };
```

If both captures and parameters are present, captures must come first. If neither are specified, the function literal would be indistinguishable from an object literal, thus one of them must always be present.

##### Generic Functions and `auto`

Warble does not have a distinct "template list" syntax for function literals.

All functions are inherently capable of being specialized based on how they are called. The most common way to express this is by using `auto` for parameter types. Each call-site determines a specialization.

```warble
let multiply = (x: auto, y: auto) {
  return x * y;
};

let a = multiply(3i32, 4i32); // specialization: multiply(i32, i32)
let b = multiply(1.5, 2.0);   // specialization: multiply(f64, f64)
```

##### Template-like Patterns via Enum Destructuring

When you want to pass symbols (such as types) explicitly—especially when there are no runtime values from which the compiler could infer them—the typical pattern is to accept an enum literal argument and destructure it.

This lets you create template-like APIs using normal function calls.

```warble
let vector = (<T>) {
  // Return a function which captures the symbol `T`, and returns an object literal instance.
  return () => {
    elements = [],      // placeholder for heap-managed storage
    element_type = T,
  };
};

let ints = vector<i32>();

// Equivalent spelling that makes the two distinct calls explicit:
let temp = vector<i32>;
let ints2 = temp();
```

In this example, `vector<i32>()` is two callable literal shorthand calls (see §5.4):

1. `vector<i32>` — the enum literal `<i32>` is placed adjacent to `vector`, destructuring its single element into the parameter `T`.
2. `()` — the empty tuple literal is placed adjacent to the returned function, destructuring into zero parameters.

##### Capture List

The capture list (`[ ]`) allows functions to access variables from their defining scope. Captures are optional. If omitted entirely, the function implicitly captures referenced external variables. To prevent implicit capturing, provide an explicit empty capture list (`[]`).

The special topic keywords `this` and `that` can also be captured. This matters because a function body is a distinct compilation context; it does not automatically inherit the surrounding topic stack.

The syntax inside capture list closely resembles object literal declarations:

* Comma-separated declarations.
* Bindings are immutable unless declared with `mut`.
* Short-hand declarations allowed; writing `a` alone is equivalent to writing `let a = a;`. Because captures are always declarations, the `let` keyword may be omitted.
* Captures can be renamed using explicit assignment: `[x = a]` captures `a` from the outer scope and names it `x` inside the function.

Examples:

```warble
let x = 10;
let y = 20;

// Implicit captures (automatically captures x)
let implicit = () { return x; };

// Explicit captures (no implicit capturing)
let explicit = [x, y] { return x + y; };

// Renamed captures
let renamed = [total = x] { return total; };
```

##### Explicit Copy or Borrow Capturing

By default, the compiler intelligently decides whether captures should occur by copy or by borrow. This decision is guided by the same logic used in function calls.

To explicitly force one behavior or the other, you can prefix captures with special unary prefix operators:

* `&a`: explicitly capture by borrow.
* `@a`: explicitly capture by copy.

Examples:

```warble
let byRef = [&x] { /* x captured by borrow */ };
let byCopy = [@x] { /* x captured by copy */ };
```

Symbols generated from capturing are flagged as `Captured`.

##### Parameters

Parameters (`( )`) are also declared as a comma-separated list, following similar rules as captures:

* Parameters are immutable by default; use the `mut` modifier keyword for mutability.
* Short-hand notation allowed; explicit `let` can be omitted.
* Parameters uniquely allow incomplete declarations (no initial value), because their values are provided upon invocation.

Examples:

```warble
let add = (a, b) { return a + b; };
let increment = (mut x) { x++; return x; };
```

All parameters are inherently templates (traditional template concept, not to be confused with template literals). Their types are determined by the function invocation arguments. Before specialization, parameter symbols are assigned the type `undefined`, marking them incomplete.

Symbols generated from parameters are flagged as `Parameter`.

##### Function Body and Return Values

The function body (`{ }`) is essentially a standard scope, allowing normal scope-level declarations, control-flow constructs, and expressions.

Functions do not explicitly declare return types; instead, the return type is inferred from their usage:

* Multiple `return` statements are allowed, but they must all return exactly the same type.
* The return type is represented by a singular symbol flagged as `Returned`.
* If a function lacks a return statement or returns explicitly without a value (`return;`), its inferred return symbol is `null` (equivalent to `return null;`).

Example:

```warble
// Inferred return type: int
let getValue = () {
  if (condition) { return 10; }
  return 20;
};

// Inferred return: null
let logSomething = (msg) {
  print(msg);
  return; // equivalent to `return null;`
};
```

Function symbol structure in the compiler's internal representation (child slice):

1. Return symbol (flagged as `Returned`)
2. Parameter symbols (flagged as `Parameter`, with `ParameterPack` for packs)
3. Captures (flagged as `Captured`)
4. Locals

##### Shorthand (Arrow) Syntax

Warble also supports a concise shorthand for single-expression functions, inspired by JavaScript's arrow functions:

* The syntax uses an arrow (`=>`) to denote the function.
* Omits the curly-braced function body.
* Immediately returns the single provided expression.

Examples:

```warble
let double = (x) => x * 2;

// Single parameter shorthand (no parentheses needed) is only allowed
// when there is no capture list.
let square = x => x * x;

// With a capture list, the parameter list must be written.
let factor = 3;
let multiplier = [factor](x) => x * factor;
```

Unlike JavaScript, Warble explicitly disallows arrow functions with a curly-braced body. If a body is needed, a normal function literal must be used instead:

```warble
// Invalid: curly-braced bodies are disallowed in arrow functions
// let invalid = (x) => { return x + 1; };

// Correct form for explicit bodies:
let valid = (x) { return x + 1; };
```

This avoids ambiguity when distinguishing between implicit object literals and explicit function bodies.

##### Summary of Function Features:

* Defined using square brackets `[captures]`, parentheses `(parameters)`, and curly braces `{body}`.
* Captures are optional; omitted captures imply implicit capturing.
* Parameters are optional; either captures or parameters must be present.
* Supports explicit capture modes (`&` by borrow, `@` by copy).
* Parameters inherently templated, may initially have type `undefined`.
* Return type inferred automatically, must be consistent across returns.
* Concise arrow syntax available for single-expression functions.

> TODO: Cover parameter packs.

### 4.2 Compiler-Generated Non-Literal Types

Warble presents all dynamic memory through a small set of compiler-defined **reference primitives**. These are fully known to and implemented by the compiler, and each corresponds to a dedicated symbol kind:

* **Non-owning:** `&T` / `*T` (borrow) and `view`.
* **Symbolic:** `$T` (symbolic borrow) — designates a symbol address, not a value address.
* **Owning:** `unique<T>`, `shared<T>`, `weak<T>`, `future<F>`, and `buffer`.

These primitives define how heap-allocated blocks are created, accessed, and freed in Warble. There is no raw-pointer escape hatch.

Among owning references, `unique<T>`, `shared<T>`, `weak<T>`, and `future<F>` have fixed runtime size and use compile-time-known deallocation sizes, while `buffer` carries a runtime `size_bytes`.

#### 4.2.1 Borrow

A **borrow** is a symbol of kind `Borrow` that designates an existing symbol. There are three borrow variants, distinguished by their flag:

| Prefix operator | Flag | Meaning |
|---|---|---|
| `&x` | `Const` | Immutable borrow — designates the **value address** of `x`. Cannot be used to mutate the target. |
| `*x` | `Mut` | Mutable borrow — designates the **value address** of `x`. Permits mutation of the target. |
| `$x` | `Symbolic` | Symbolic borrow — designates the **symbol address** of `x`. Has no value address; accessing it as a runtime value is a compile-time error. |

All three variants share the same kind (`Borrow`) and the same payload encoding: a `std::pair<uint32_t, uint32_t>` of (dependency module index, symbol index).

The parser disambiguates these prefix operators from their infix counterparts based on position. For example, `&` between two expressions is the intersection operator (§4.2.3); `*` between two expressions is multiplication; `&` or `*` before a single operand is a borrow.

##### Immutable Borrow (`&`)

An immutable borrow (`&x`) produces a kind `Borrow` flagged `Const`. It designates the value address of an existing object without owning it.

Conceptually, immutable borrows behave like const pointers with strict, compiler-enforced lifetime rules. Warble uses them to access existing storage without copying it.

Warble does not have raw pointers. Address-like values exist only in the form of compiler-defined reference types (borrows and the owning reference primitives described below). The type system cannot express "raw address" as a value. This restriction applies equally to user code and the standard library.

##### Mutable Borrow (`*`)

A mutable borrow (`*x`) produces a kind `Borrow` flagged `Mut`. It designates the value address of an existing object and permits mutation through the borrow.

Mutable borrows follow the same lifetime and non-escaping rules as immutable borrows. The additional constraint is that while a mutable borrow to a value is live, no other borrow (mutable or immutable) to the same value may be live.

##### Symbolic Borrow (`$`)

A symbolic borrow (`$x`) produces a kind `Borrow` flagged `Symbolic`. It designates the **symbol address** of `x`—that is, it refers to the symbol table entry itself rather than to any runtime value storage.

A symbolic borrow has no value address. Its offset column is unused. Any operation that would require a value address is a compile-time error. Symbolic borrows are used for reflection, runtime type introspection, and metaprogramming:

```warble
let sym = $namedValue;
print(nameof(sym)); // Prints "namedValue"
print(kindof(sym)); // Prints the kind of namedValue
```

Because symbolic borrows carry no runtime data beyond a symbol index, they are always compile-time transparent and never need to be materialized as pointer-sized runtime values.

##### Borrow optimization (materialized vs. optimized away)

Immutable and mutable borrows are always *conceptually* the same kind of value. However, the compiler aggressively optimizes redundant storage.

If the compiler knows exactly what a borrow designates for the entire lifetime of that borrow, it may choose not to allocate an 8-byte slot to hold an address. Instead, it folds the address calculation directly into each use site (for example, lowering to a base register plus displacement). This is a normal optimization (constant folding / propagation and related lowering).

When the compiler cannot prove what a borrow designates (the borrow is **opaque**), it materializes it as a normal runtime value that occupies storage (pointer-sized) and is treated as an actual address by generated instructions.

The specification sometimes calls a fully transparent, fully optimizable borrow a **pure alias**. This is not a distinct type; it is an optimization outcome.

##### Non-escaping rule (stack/register only)

When a function receives a runtime borrow (for example, as a parameter), that borrow is treated as **opaque** to the compiler unless the call is inlined.

To prevent dangling borrows, Warble enforces the following rule:

* A runtime borrow value must not be copied into storage whose lifetime is not bounded by the current call frame.

In practice, this means a runtime borrow may live only in:

* registers,
* the current function's stack frame,
* temporary values that do not outlive the call.

It is therefore a compile-time error to:

* store a runtime borrow into a heap-allocated container (for example, pushing it into a vector),
* store a runtime borrow into an object/array that outlives the call,
* capture a runtime borrow into a closure that can escape the call.

Passing a runtime borrow through other function calls is permitted, but the same non-escaping rule applies transitively: callees must not store the borrow beyond their own call frames.

This non-escaping rule applies to immutable and mutable borrows. Symbolic borrows do not carry runtime addresses and are not subject to these restrictions.

##### External-borrow transparency

External borrows (borrows to imported bindings) must remain transparent to the compiler.

In particular, the compiler must always be able to determine exactly which imported binding is being borrowed, so it can preserve memory safety rules and (for the module scheduler model) insert barriers at every external read site (§13.2.4).

Therefore, it is a compile-time error to make an external borrow opaque by copying it into constructs that hide its identity, such as unions. For example, storing an external borrow into a union (such as `!null | &T`) is a compile-time error.

This transparency requirement applies specifically to borrows to imports. Borrows internal to a module are not required to remain transparent; they may be treated as opaque runtime borrows if desired (at the cost of optimization opportunities).

#### 4.2.2 Union

A **union** in Warble is a type-safe tagged union: it can represent exactly one of several possible types at runtime.

Conceptually, a union value has two components:

* A **type tag** (an index identifying which arm is active).
* A **value slot** sized to hold the largest arm type.

Unions are used for optional values, multi-type expression results, and (most importantly) union-returning control flow such as generators and promises.

##### Creation and Syntax

Unions are created by expressions or by union-returning control flow.

The binary operator `|` is the **symbolic union operator**. It produces a kind `Union` flagged `Symbolic`—a union that describes which arms are valid but holds no value:

```warble
let PetType = Cat | Dog;
// PetType is kind Union, flagged Symbolic
// It has no value address — using it as a runtime value is a compile-time error
```

A symbolic union acquires a concrete value through a constructor annotation (§3.1.5):

```warble
let pet: Cat | Dog = Dog("Fido", 3);
// Initialized as: Dog("Fido", 3) -> (Cat | Dog)
// pet is kind Union, NOT flagged Symbolic — it holds a live Dog value
```

The `|` operator may be chained. Chained symbolic unions are flattened into a single union with all listed arms.

The binary operators `||` and `&&` are **value-level union operators**. They always produce a union that holds a value (not flagged `Symbolic`).

Unlike most languages, these operators do **not** use truthiness. Instead, they branch based on whether the left-hand side is **passing** or **failing**:

* A non-union value is considered **passing**.
> TODO: The above isn't quite right. A non-union value is default to passing, but it's more complex than that.
* A union value is **passing** when its runtime tag is in the union's pass region (`tag >= fail_count`) and **failing** otherwise.

**`a || b`**

* **Result type:** all of `b`'s states (pass and fail), plus `a`'s pass states.
* **Runtime:** if `a` is passing, the result is `a`'s pass value and `b` is not evaluated; otherwise the result is `b`.

**`a && b`**

* **Result type:** all of `b`'s states (pass and fail), plus `a`'s fail states.
* **Runtime:** if `a` is failing, the result is `a`'s failing value and `b` is not evaluated; otherwise the result is `b`.

Example:

```warble
let data = 42 || ""; // type <integer, string>, value is 42
```

##### Union Functions (`return` / `yield`)

Warble functions do not explicitly declare union return types. Instead, the compiler infers a union return type automatically when the function contains union-producing control flow.

A function is a **union function** if it contains any of the following:

* A `yield` statement.
* A `return` or `yield` statement augmented with `async`, `pass`, or `fail`.

A function with only plain `return` statements is a normal function: its explicit returns must all resolve to a single type.

Any function that contains a `yield` statement is automatically a **generator function**.

Any function that contains an `async`-augmented `return` or `yield` is an **async function**.

For an async function, the compiler constructs a **promise union type** by taking the inferred union from its `return`/`yield` arms and adding `unresolved` and `detached` as additional fail arms (deduplicated if already present).

Both `return` and `yield` may be augmented with the keywords `async`, `pass`, and `fail`:

* `async` may appear at most once and must come first.
* `pass` and `fail` are mutually exclusive.
* `pass`/`fail` may appear without `async`, or alongside `async`.

Examples:

```warble
return pass i32(42);
return fail "Error message!";
return async pass value;
yield async fail errorValue;
```

If any `return` or `yield` in a function uses `pass` or `fail`, then all `return` and `yield` statements in that function must also use exactly one of `pass` or `fail`. Mixing `return expr;` with `return fail expr;` in the same function is a compile-time error.

If a function is an async function (contains any `async`-augmented `return`/`yield`), then all `return` and `yield` statements in that function must be `async`-augmented. Mixing `return async ...` with a non-async `return`/`yield` in the same function is a compile-time error.

The union returned by a union function is constructed from the set of types produced by its `return` and `yield` statements. Arms are deduplicated within their category (pass vs fail). If the same type is produced as both `pass` and `fail`, it appears twice in the union's arm list: once as a passing arm and once as a failing arm.

##### Promises and Generators (Union Patterns)

Promises and generators are not distinct types; they are common patterns of union:

* A **generator** is a union that includes the compiler-defined symbol `exhausted` as one of its fail arms. `for` loops treat `exhausted` as the iteration-termination state.
* A **promise** is a union that includes the compiler-defined symbols `unresolved` and `detached` as fail arms. `unresolved` means "not complete yet". `detached` means "the owning future has been dropped; completion results will be discarded".

`exhausted`, `unresolved`, and `detached` are classified as **fail** states. When a union is in one of these states, its value slot is not meaningful and must not be accessed.

In an async function, the promise union is produced asynchronously. The call returns a `future` handle (§4.2.4) that designates the promise union in an async frame. The `await` operator consumes a `future` and waits until its promise is complete.

##### Limits

Unions have a hard limit of $2^{16}-1$ total arms (65535). The type tag is one byte when there are 256 or fewer arms; otherwise it is two bytes.

#### 4.2.3 Intersection

An **intersection** in Warble is a compile-time constraint that requires a value to satisfy *all* of the listed arms simultaneously. Where a union says "this value is one of these types", an intersection says "this value must be compatible with every one of these types".

In the compiler's internal representation, an intersection is represented as a symbol of kind `Intersection`.

##### Creation and Syntax

Intersections are created with the infix `&` operator, applied to symbols or functions:

```warble
let I = Cat & Dog;
```

This produces an intersection with two arms: `Cat` and `Dog`. The result is kind `Intersection` flagged `Symbolic`—it describes the constraint but holds no value. Its offset column is unused, and using it as a runtime value is a compile-time error.

The `&` operator may be chained:

```warble
let J = Cat & Dog & Animal;
```

Chained intersections are flattened into a single intersection with all listed arms, analogous to how nested unions are flattened (§4.3.2).

The `&` operator used here is the **infix** (binary) form. It is syntactically distinct from the **prefix** (unary) `&` that creates a borrow (§4.2.1). The parser disambiguates the two based on position: when `&` appears between two expressions it is the intersection operator; when it appears before a single operand it is the borrow operator.

##### Internal Representation

An intersection symbol's payload is stored as a `std::pair<uint32_t, uint32_t>` representing (data start index, length) into the module's per-module data buffer.

This is similar to unions, but meaningfully different: a union's payload splits its second `u32` into two `u16` fields in order to encode both a total arm count and a fail count. An intersection has no concept of pass/fail arms, so no such split is needed—the full `u32` is used for the length.

The data slice is a sequence of `u32` symbol indexes. The **first slot** (index 0 of the slice) is always reserved for the **true type** of the intersection—the concrete type of the value currently held. The remaining slots (index 1 onward) are the **constraint arms**.

When an intersection is created via the `&` operator, the true-type slot is set to the symbol index for `Undefined` and the intersection is flagged `Symbolic`:

```warble
let I = Cat & Dog;
// I is kind Intersection, flagged Symbolic
// true type = Undefined, arms = [Cat, Dog]
// No value address — using I as a runtime value is a compile-time error
```

##### Instantiation

An intersection acquires a concrete value through a constructor annotation (§3.1.5). The annotation uses the pipeline operator to pass the initializer through the intersection:

```warble
let value: I = Dog("Fido", 3);
// Initialized as: Dog("Fido", 3) -> I
```

The compiler evaluates `Dog("Fido", 3)` to produce a concrete `Dog` value, then checks that this value satisfies every constraint arm of `I`. If the check succeeds, the resulting symbol is of kind `Intersection` with its true-type slot set to the resolved type of the `Dog` value. The `Symbolic` flag is not present on the resulting symbol—it has a real value address. At runtime, the memory layout is that of the true type (`Dog`).

If any arm is not satisfied—for example, if `Dog` does not support an operation that `Cat` requires—the compiler emits an error at the point of instantiation.

##### Constraint Checking (Operation Forwarding)

The defining behavior of an intersection is its static constraint check. Before any operation applied to an intersection value is forwarded to the underlying true type, the compiler verifies that **every** constraint arm of the intersection also supports that operation. Only then is the operation applied to the true type.

For example:

```warble
let Animal(noise: string, name: string, age: i32) => {
  noise,
  name,
  age,

  public speak() => "{this.name} says {this.noise}",
  public getName() => this.name,
  public getAge() => this.age,
};

let Cat(name: string, age: i32) => {
  ...Animal("Meow", name, age),

  public purr() => "Purr...",
};

let Dog(name: string, age: i32) => {
  ...Animal("Woof", name, age),

  public fetch() => "Fetching!",
};

let I = Cat & Dog;
let pet: I = Dog("Fido", 3);

pet.speak();  // OK — both Cat and Dog have `speak`
pet.fetch();  // Error — Cat does not have `fetch`
pet.purr();   // Error — Dog does not have `purr`
```

The constraint check is purely static. At runtime there is no tag, no dispatch table, and no overhead—the value is stored exactly as its true type. The intersection's arms serve only as a compile-time contract.

This is the key distinction from a union:

| | Union | Intersection |
|---|---|---|
| Meaning | "one of" (the value is *one* of the arms) | "all of" (the value must satisfy *every* arm) |
| Runtime representation | value slot + type tag | value slot only (true type); no tag |
| Operation dispatch | jump-table dispatch across arms | forwarded to true type after static arm check |
| Memory overhead | largest arm + tag | true type only |

##### Common Use Case: Constraining Template Parameters

Intersections are especially useful for constraining generic (template) parameters. A bare parameter declaration or `p: auto` accepts any type. By intersecting `auto` with one or more constraint arms, you narrow what may be passed:

```warble
let greet = (creature: auto & Animal) {
  creature.speak();
};
```

Here `auto & Animal` is an intersection whose first arm is `auto` (resolved at each call site) and whose second arm is `Animal`. Whatever concrete type `auto` resolves to, the compiler verifies that `Animal`'s interface is also satisfied. This is conceptually similar to bounded generics or trait bounds in other languages, expressed through Warble's uniform intersection mechanism.

Multiple constraints compose naturally:

```warble
let process = (x: auto & Serializable & Printable) {
  x.serialize();
  x.print();
};
```

##### Intersections with `auto`

When `auto` appears as an arm of an intersection, the `auto` arm is resolved at each call site to the concrete type of the argument. The remaining arms act as static constraints on that resolved type.

Because `auto` on its own already means "accept anything", writing `auto & C` is the idiomatic way to say "accept anything that also satisfies `C`".

#### 4.2.4 Future

A **future** is a primitive, compiler-generated type used to represent an in-flight async function call.

In the compiler's internal representation, a future value is represented as a symbol of kind `Future`.

Representation:

* A future value is a pointer-sized memory address (typically 8 bytes).
* The address points to a **promise union** stored at the beginning of a heap-allocated async frame.
* Because the promise union begins at offset 0 of the async frame, the future's address is also the base address of the async frame.

Semantics:

* A future is an **owning handle** for its async frame.
* Futures have **unique ownership**: they cannot be copied, but they may be moved.
* Futures are typed by the async function they correspond to (not by the promise union's resolved value type). This gives the compiler static knowledge of the frame layout, enabling cleanup without dynamic dispatch.

Dropping a future:

* If the promise tag is no longer `unresolved`, the future destroys the async frame immediately.
* Otherwise, the future attempts to detach by atomically compare-exchanging the tag from `unresolved` to `detached`.
  * If the compare-exchange succeeds, the async task becomes responsible for cleanup when it eventually completes.
  * If the compare-exchange fails because the task completed concurrently, the future destroys the async frame immediately.

The only way to extract the result from a future is `await` (§5.2.1). `await` forwards to the underlying promise union and consumes the future handle.

#### 4.2.5 Unique

A **unique** is an owning reference with unique ownership, similar to `std::unique_ptr<T>`.

In the compiler's internal representation, a unique owning reference is represented as a symbol of kind `Unique`.

Representation:

* A unique value is pointer-sized (typically 8 bytes).
* It designates a single heap allocation containing a value of type `T`.
* Unique owning references do not carry a runtime size; `sizeof(T)` is a compile-time constant.

Semantics:

* A unique has **unique ownership**: it cannot be copied, but it may be moved.
* A unique is **non-empty**: it always designates an allocation. To represent an empty state, use an optional union `!null | unique<T>`.
* Using a unique to access members or elements forwards to the owned value (conceptually via an implicit borrow of the owned storage).

Dropping a unique:

* Dropping a unique destroys the owned value and frees its allocation.
* The compiler knows the deallocation size as `sizeof(T)` and passes that size to the heap allocator (§9.5).

#### 4.2.6 Shared

A **shared** is an owning reference with shared ownership, similar to `std::shared_ptr<T>`.

In the compiler's internal representation, a shared owning reference is represented as a symbol of kind `Shared`.

Representation:

* A shared value is pointer-sized (typically 8 bytes).
* A shared allocation contains a header followed immediately by the `T` object.
* The allocation size is a compile-time constant: `sizeof(shared_header) + sizeof(T)`.
* The header contains:
  * an atomic strong reference counter (`strong_count`), and
  * an atomic weak reference counter (`weak_count`).

The shared value designates the `T` object (not the header). The header is located at a fixed negative offset from the `T` address. This layout is compiler-defined but fixed for a target.

Semantics:

* A shared has **shared ownership**: it may be copied.
* Copying a shared atomically increments `strong_count`.
* Dropping a shared atomically decrements `strong_count`.
* A shared is **non-empty**: it always designates an allocation. To represent an empty state, use an optional union `!null | shared<T>`.

Finalization and deallocation:

* When `strong_count` transitions to 0, the `T` object is destroyed exactly once.
* The allocation remains alive as long as `weak_count > 0`.
* When `weak_count` transitions to 0 (and `strong_count` is already 0), the allocation (header + storage) is freed.

Note: the exact initial values and the sequencing between destroying `T` and releasing the allocation follow the conventional weak-pointer control-block design. The intent is that weak references can observe liveness and attempt to acquire shared ownership without risking use-after-free.

#### 4.2.7 Weak

A **weak** is a non-owning observer handle associated with a `shared<T>` allocation, similar to `std::weak_ptr<T>`.

In the compiler's internal representation, a weak reference is represented as a symbol of kind `Weak`.

Representation:

* A weak value is pointer-sized (typically 8 bytes).
* A weak value designates the shared allocation's header/control block.

Semantics:

* A weak may be copied.
* Copying a weak atomically increments `weak_count`.
* Dropping a weak atomically decrements `weak_count`.
* A weak does not provide direct access to `T`. The only way to obtain access is to attempt to upgrade it to a `shared<T>`.

Upgrading (locking):

* Upgrading a weak produces `!null | shared<T>`.
* If the `T` object is already destroyed (`strong_count == 0`), the result is `null`.
* Otherwise, upgrading atomically increments `strong_count` and returns a `shared<T>` designating the `T` object.

#### 4.2.8 Buffer

A **buffer** is an owning reference to a dynamically-sized heap allocation.

In the compiler's internal representation, a buffer owning reference is represented as a symbol of kind `Buffer`.

Representation:

* A buffer value is 16 bytes: an address plus an 8-byte size (`size_bytes`).
* The allocation contains `size_bytes` logically-addressable bytes.
* The buffer's stored `size_bytes` is the deallocation size passed to the heap allocator (§9.5).
* The buffer's address is never 0. (The address is not observable as a raw value; this invariant exists to support optimizations such as `!null | buffer`.)

Semantics:

* A buffer has **unique ownership**: it cannot be copied, but it may be moved.
* A buffer is **non-empty**: it always designates an allocation. To represent an empty state, use an optional union `!null | buffer`.
* A buffer's designated allocation is fixed for the lifetime of the buffer value (it is not retargetable).
Operations that conceptually "resize" storage therefore produce a new buffer value. Code that needs to represent "no allocation yet" or "may replace the allocation" uses an optional buffer (`!null | buffer`) and replaces the union value as needed.

The standard library uses this pattern for containers such as `vector` and `string`: their internal storage is represented as an optional buffer so they can be empty and can grow/shrink by rebuilding that optional buffer.

Warble provides no way to extract a raw address from a buffer. All interaction with buffer contents occurs through compiler-defined operations that preserve bounds safety.

#### 4.2.9 View

A **view** is a non-owning, dynamically-sized reference to a contiguous region of memory.

In the compiler's internal representation, a view is represented as a symbol of kind `View`.

Representation:

* A view value is 16 bytes: an address plus an 8-byte size (`size_bytes`).
* A view does not own storage; it does not free memory when dropped.
* A view's address is never 0, even when `size_bytes == 0`.

Semantics:

* A view is non-owning, and therefore must obey non-escaping rules similar to borrows.
* A view may be created from an owning reference (such as `buffer`, `unique<T>`, or `shared<T>`) or from other views.
* Warble provides no way to extract a raw address from a view.

##### View stability (no retargeting while a view is live)

In addition to the general non-escaping rule, the compiler enforces a *stability* rule for views:

* A view must not outlive the stability of the region it designates.
* In particular, if a view is derived (directly or indirectly) from an *optional buffer slot* (`!null | buffer`), then the underlying slot must not be replaced (retargeted) for the lifetime of the view.

To enforce this, the compiler performs a conservative analysis based on a per-function **retarget set**:

* A function's retarget set is the set of optional-buffer slots (including fields/paths) whose value the function may replace, either directly or transitively through calls.
* While a view derived from a particular optional-buffer slot is live, it is a compile-time error to perform any operation that the compiler cannot prove does not replace that slot.

This rule may forbid calls that are *logically* safe but not provably safe under the compiler's analysis; this is permitted.

##### Optional reference values (`!null | ...`)

Unique, shared, weak, future, buffer, and view values are non-empty primitives. The conventional way to represent an "optional reference" is to use a union with a failing `null` arm:

* `!null | unique<T>`
* `!null | shared<T>`
* `!null | weak<T>`
* `!null | future<F>`
* `!null | buffer<T>`
* `!null | view<T>`

Such unions typically arise from union-producing control flow (for example, `return pass ref` on one path and `return fail null` on another).

##### Automatic destruction and leaks

These reference primitives are designed to make "forgot to free" a non-problem in Warble: dropping an owning reference deterministically triggers destruction and deallocation.

It is still possible to exhaust memory in safe Warble code (for example, by growing a container without bound). Additionally, cyclic graphs built purely out of `shared<T>` may keep memory alive indefinitely; Warble does not attempt to provide general cycle collection.

#### 4.2.10 Module

In Warble, a **module** represents a single imported source file. For every source file included in a project, exactly one corresponding module object is created—no matter how many times that file is imported. The module acts as the root scope and primary container for all information defined within that source file.

##### Module Structure and Data Layout

Modules store extensive metadata required by the compiler, debugger, and runtime. Each module includes the following structures:

* **Symbol Table**
  The symbol table is stored in a columnar (struct-of-arrays) format rather than an array-of-structs. Each symbol's properties are stored as separate columns of data. Symbols are accessed exclusively via 32-bit indexes rather than pointers. Even constructs like symbol references and lookups internally resolve to these 32-bit symbol indexes. This design ensures memory compactness, cache efficiency, and simplified symbol management.

* **Data Buffer**
  The module stores a single per-module data buffer used to hold literal payloads. It stores UTF-32 text (for identifiers and string literals), sequences of 32-bit symbol indexes (for enum literals), and sequences of 32-bit limbs (for numeric literals). Literal symbols reference slices of this shared buffer, which improves locality and simplifies module storage.

* **Source Mapping Information**
  To support debugging, error reporting, and source reconstruction, the module maintains precise source mapping data. This consists of two tightly-coupled buffers:

  * **Tokens Buffer**: An array of token entries representing the lexed source code. Each entry identifies the token kind and also encodes its width (in characters), enabling faithful source reconstruction and precise character ranges for diagnostics and highlighting.

  Each symbol in the symbol table includes source mapping references that index into this buffer, enabling precise location information retrieval.

* **Lines Table**
  A module also maintains an array of 32-bit indexes called the "lines table", containing the index for each newline character (`\n`) in both the original source text and in the tokens buffer. This table enables efficient binary search operations to determine line and column information, essential for readable diagnostics and source code display.

##### Static Allocation and Memory

All module data described above are statically allocated and stored directly within data pages embedded in the compiled executable. This static embedding eliminates dynamic allocation overhead at runtime, ensuring rapid initialization and predictable memory usage.

Additionally, the module object may contain user-defined top-level declarations. To accommodate these declarations, the compiler reserves a dedicated static memory block for each module. Each top-level declaration is assigned a specific slot within this memory block, analogous to how a function call uses a stack frame for its local variables.

#### 4.2.11 Symbol

Symbols are fundamental building blocks of Warble's type and runtime systems. Every value in Warble, whether it's a literal, an object, a function, or any other entity, is represented internally by a **symbol**. In practice, a symbol is simply a 32‑bit index into a module's columnar symbol table. Each column stores one property (such as the kind or name) for all symbols. Users do not manipulate these columns directly; instead, they obtain a symbol index and query properties through compiler-provided functions.

##### Symbol Structure and Properties

Because a symbol is just an index, each property lives in its own column and is accessed through compiler-defined helpers. The core columns are:

* `kindof(sym)` – an 8-bit **kind** ID (stored in a standalone `kinds` column). This is the primary classification of a symbol.
* `flagsof(sym)` – a 64-bit bitset of flags (modifiers, size hints, etc).
* `payloadof(sym)` – a 64-bit typeless payload interpreted according to the symbol's kind (immediate values, packed indexes, pointers, etc).
* `offsetof(sym)` – a 32-bit byte offset used for runtime memory layout.
* `parentof(sym)` – the parent symbol index.
* `nameof(sym)` – a symbol index naming this symbol (typically a `String` or `Enum` symbol), or `0` for anonymous symbols.
* `tokenof(sym)` – the token index where the symbol begins, used for source mapping.

##### Symbol Creation

Symbols are primarily produced by expressions in Warble. Each expression, no matter how simple or complex, generates exactly one symbol. Literals, arithmetic operations, function calls, and other constructs all yield symbols upon evaluation:

```warble
let a = 42; // creates a symbol representing the literal '42'
let b = a + 1; // creates symbols for 'a', '1', and the result of '+'
```

Symbols created by expressions are temporary and anonymous by default. To preserve a symbol beyond its immediate expression, it must be named explicitly through a declaration:

```warble
let namedValue = 100; // assigns the symbol for '100' a name
```

##### Symbol Addressing and Lookup

Every symbol has the capability to represent two addresses simultaneously:

* **Symbol address**: The offset into the module's symbol table, calculated from the symbol's index. Every symbol has a symbol address.
* **Value address**: A byte offset relative to some enclosing context (such as a function's stack frame or a module's static memory block), stored in the symbol's `offsetof` column.

For example, `const i = 42;` inside a function is represented by a symbol of kind `Integer` with its offset column set to the byte displacement from the enclosing function. This allows the runtime value address to be calculated as a displacement from `RSP`. But the symbol also has a symbol address—the index that leads to the entry in the symbol table where the kind, flags, name, and other metadata can be read. Symbol table data is embedded into the compiled binary as read-only data, so symbol addresses are available at runtime.

A symbol flagged as `Symbolic` only has a symbol address. It does not have a value address. Its offset column is unused. Any operation that requires a value address is a compile-time error.

Warble utilizes symbols extensively for efficient address resolution through a two-phase lookup process:

* **Downward Resolution**: To access nested symbols, Warble sums the displacements of child symbols.
* **Upward Resolution**: Starting from a given symbol, Warble recursively sums parent displacements until reaching a context symbol (such as a module or function).

Addressing strategies depend on the context:

* **Module Symbols**: Utilize RIP-relative addressing for top-level variables.
* **Function Symbols**: Utilize RSP-relative addressing for local variables.

##### Reflection and Runtime Access

Users access symbol metadata at runtime through the symbolic borrow operator `$` (§4.2.1). The expression `$x` produces a kind `Borrow` flagged `Symbolic`, designating the symbol table entry for `x`:

```warble
let sym = $namedValue;
print(nameof(sym)); // Prints "namedValue"
print(kindof(sym)); // Prints the kind of namedValue
```

Because a symbolic borrow carries no value address, it is purely a handle to symbol metadata. Reflection provides a powerful, dynamic interface for debugging, logging, and runtime type introspection.

##### Type-Checking and Identity Operators

Warble offers powerful type-checking operators leveraging symbols' structural metadata:

* **`is`**: Verifies structural equivalence between symbols.

  ```warble
  if (dog1) is (dog2) { /* structurally identical */ }
  ```

* **`has`**: Verifies that a symbol structurally includes required properties or methods.

  ```warble
  if (socket) has ({ send(){}, recv(){} }) { /* satisfies the interface */ }
  ```

* **`from`**: Checks provenance, confirming if a value originates from a particular constructor.

  ```warble
  if (cat) from (Cat) { /* cat was created by Cat constructor */ }
  ```

##### Object-Oriented Programming via Symbols

Warble's object-oriented system is fully symbol-based:

* Constructor functions define object structures.
* Spread (`...`) syntax supports composition by marking child symbols with the `Spread` flag.
* Visibility modifiers (`Export`, `Protected`, `Private`) enforce encapsulation at compile-time.
* Overriding methods rely on symbol shadowing. To call an overridden implementation, reference the parent symbol directly.

##### Performance and Memory Considerations

Warble's columnar internal symbol layout provides excellent memory locality and efficiency. Rather than pretending a symbol is a contiguous struct, Warble exposes its columnar nature directly through the accessor functions above. Symbols are lightweight indexes, and all property lookups go through these compiler‑defined helpers. This straightforward model keeps memory compact while still allowing high‑level reflection and analysis.

This columnar design allows Warble symbols to scale linearly with the number of program operations, ensuring predictable memory usage even in large codebases.

Symbols thus form the backbone of Warble's powerful and expressive type system, providing efficient representation, strong type guarantees, flexible reflection capabilities, and performance-oriented internal design.

##### Symbol Columns

###### Flags

One of the symbol columns is a 64 bit bitset of flags. Every flag has a specific meaning and describes how the symbol should be used.

- Size8, Size16, Size32, Size64, Size128, Size256, Size512: Size-class flags used for layout and lowering.
- Extended: Marks an extended property in an object literal; lookups can transparently pass through it.
- Spread: Marks a symbol spread into another using the `...` operator.
- Repeat: Marks an array as being created via the repeat syntax, such as `[0; 10]`.
- Import: References a symbol from another module.
- Export / Private / Protected / Public / Static / Mut / Const / Let: Common visibility and declaration modifiers.
- Async / Generator / Varied: Function behavior flags.
- Returned / Captured / Parameter / ParameterPack: Function-structure flags.
- Impure / Used / Inline / Immediate / Internal / Accumulator: Miscellaneous analysis and lowering flags.
- Comtime / Runtime: Compile-time vs. runtime knowledge.
- NonZero / NonMax: Optimization hints.
- Symbolic: Symbol only has a symbol address, no value address. Its offset column is unused. Using it as a runtime value (any operation that requires a value address) is a compile-time error. See §4.2.1 for how borrows use this flag, and §4.2.2/§4.2.3 for how unions and intersections use it.
- Halts: Indicates a function is proven to halt.
- Topic: Indicates that this symbol is a topic and is accessible via `this` or `that`.

###### Kinds

Every symbol has a standalone 8-bit `kinds` column storing its **kind**. Kind names are written in PascalCase.

Core kinds:

- Undefined, Unresolved, Auto, Null, Readonly
- Raw, Boolean, Character, Integer, Float
- Borrow, Unique, Shared, Weak, Future, Buffer, View
- Identifier, String, Enum, Union, Intersection
- Array, Tuple, TemplateString, Object, Range
- Phi, Function, Module, Label

Statement kinds:

- Do, Error, If, ElseIf, Else, When
- For, Repeat, While, RepeatWhile
- Is, Has, From, Default
- Register, Import

The CFG block kinds are mostly treated as the same. Their payload typically encodes a contiguous slice of child symbols (in preorder) for everything declared in the block. This does not necessarily mean their lifetime begins in the block; that is determined by their constructor and destructor instructions. It is possible for the constructor instruction to be deferred into a different block. Blocks are used as operands for jump instructions.

###### Payloads

Symbols have a `payloads` column, which is a 64-bit piece of data. The exact interpretation of the payload depends on the symbol's kind (and sometimes its flags).

Common payload encodings (in terms of C++ types):

- Boolean: `bool`.
- Character: `char32_t`.
- Integer: `std::pair<uint32_t, uint32_t>` of (data slice start, limb count).
- Float: `std::pair<uint32_t, uint32_t>` of (data slice start, limb count). The referenced slice stores significand limbs followed by two exponent limbs.
- Borrow: `std::pair<uint32_t, uint32_t>` of (dependency module index, symbol index). The flag (`Const`, `Mut`, or `Symbolic`) determines the borrow variant.
- Identifier / String / Enum: `std::pair<uint32_t, uint32_t>` of (data slice start, slice length).
- Union: `std::tuple<uint32_t, uint32_t, uint16_t, uint16_t>` containing a data slice plus 16-bit counts (including `fail_count`).
- Label: `std::pair<uint32_t, uint32_t>` of (instruction start index, length).
- Array / Tuple / Object / TemplateString / Function: `std::pair<uint32_t, uint32_t>` of (symbol slice start index, length).
- Range: `std::pair<uint32_t, uint32_t>` of (start symbol index, end symbol index).
- Module: a `compiler::program::Module*` during compilation.

### 4.3 Union Mechanics

This section provides a detailed explanation of how unions are created, manipulated, and represented in memory. Unions in Warble are conceptually straightforward as tagged unions, and Warble provides several powerful and ergonomic operators for working with them, ensuring both expressive power and performance efficiency.

#### 4.3.1 Creation (`|`, `||`, `&&`, `return`, `yield`)

Unions in Warble are created through expressions or by union-returning control flow. The main ways unions arise are:

**Symbolic Union Operator (`|`):**

The binary `|` operator produces a kind `Union` flagged `Symbolic`. It describes which arms are valid but holds no runtime value:

```warble
let PetType = Cat | Dog;
// kind Union, flagged Symbolic — no value address
```

A symbolic union is used purely as a type descriptor. It can appear in constructor annotations to specify which types a union may hold. When a value is actually initialized into a symbolic union (via a constructor annotation or assignment), the result is a normal (non-symbolic) union with a live value.

**Value-Level Union Operators (`||`, `&&`):**

Both `||` and `&&` are value-level union operators. They produce a union that holds a value (not flagged `Symbolic`) and short-circuit based on whether the left-hand side is passing or failing (rather than on truthiness).

* `a || b` returns `a` (without evaluating `b`) when `a` is passing; otherwise it returns `b`.
* `a && b` returns `a` (without evaluating `b`) when `a` is failing; otherwise it returns `b`.

The resulting union type always includes all of the right-hand side's states, plus either the left-hand side's pass states (`||`) or fail states (`&&`).

A non-union value is treated as a union with a single passing arm for the purposes of `||` and `&&`. This is a semantic model; the compiler is expected to optimize the "single-arm union" case by eliding any runtime tag, since the active arm is statically known.

**Pass/Fail flip (`!`):**

Warble defines the unary prefix operator `!` as a built-in **union operator** that flips pass vs. fail classification.

* If `x` is a non-union value of type `T` (treated as a single passing arm), then `!x` is a single failing arm of type `T`.
* If `x` is a union value, then `!x` flips the category of every arm: passing arms become failing arms and failing arms become passing arms.

In this specification's type notation, a union arm prefixed with `!` denotes a failing arm. For example:

* `A | B` means "pass `A`, pass `B`".
* `!A | B` means "fail `A`, pass `B`".
* `!(A | B)` is equivalent to `!A | !B` (all arms flipped).

`!` is the primary way to explicitly communicate "this value is failing" in expression form.

Example:

```warble
let parse_int = (text) {
  if (can_parse_int(text)) {
    return pass parse_int(text);
  }
  return fail "not an int";
};

let a = parse_int("123") || 0;        // pass → holds 123
let b = parse_int("nope") || 0;       // fail → holds 0

let c = parse_int("123") && parse_int("456");  // pass → holds 456
let d = parse_int("nope") && parse_int("456"); // fail → holds "not an int" (RHS not evaluated)
```

**Union Functions (`return` / `yield`):**

Union-returning functions are formed by using `yield`, `async`, `pass`, or `fail` in `return`/`yield` statements. See section 4.2.2 Union for the surface syntax rules.

#### 4.3.2 Flattening

Warble automatically flattens nested unions, simplifying union handling and preventing overly complex type trees.

Example:

```warble
let nested = 1 || false || "Hello";
// Instead of <integer, <boolean, string>>, Warble flattens it to <integer, boolean, string>
```

Flattening occurs at compile-time, merging all potential union types into a single, flat union type.

#### 4.3.3 Type-Tag Layout

Every union stores a type tag alongside its data to identify which arm is currently active.

The total size of a union is determined as:

* The size of the largest possible arm type.
* Plus a tag (one byte when there are 256 or fewer arms; otherwise two bytes).

Unions have a hard limit of $2^{16}-1$ arms (65535).

When a union has exactly one arm, the compiler is expected to avoid materializing a runtime tag:

* A union with a single **passing** arm is equivalent to its non-union value type and needs no runtime tag.
* A union with a single **failing** arm has a compile-time-known failing classification; the compiler may treat its tag as a compile-time constant.

##### Pass/Fail Ordering

Unions created by union functions have their arms ordered so that all failing arms come first and all passing arms come after them.

Each union type has compile-time metadata that includes:

* `arm_count`: total number of arms.
* `fail_count`: number of failing arms (a prefix of the arm list).

If the current runtime tag is `tag`, then:

* The union is in a failing state when `tag < fail_count`.
* The union is in a passing state when `tag >= fail_count`.

The compiler-defined `unresolved` (promise) and `exhausted` (generator) states are classified as failing states.

This ordering is important, because it allows any union to be tested for pass/fail status by a single comparison.

##### Tag remapping for `!`

For a multi-arm union, `!` produces a new union type whose arm list is the old pass arms followed by the old fail arms (so failing arms remain a prefix).

Let:

* `n = arm_count`
* `f = fail_count`

Then the resulting union has `new_fail_count = n - f`, and the runtime tag is remapped by a simple rotation:

* if `tag < f` (was failing, becomes passing): `new_tag = tag + (n - f)`
* otherwise (was passing, becomes failing): `new_tag = tag - f`

This remapping preserves the active payload and keeps pass/fail testing as a single comparison.

#### 4.3.4 Operator forwarding and postfix question (`?`)

Warble provides a unary postfix operator `?` for producing a pass/fail union.

The only special behavior of `?` is that it introduces a control-flow split (a failing path and a passing path) at that point in the expression.

This operator is intentionally designed so that optional chaining is spelled as `?` followed by the normal operator token, not as a dedicated `?.` token. For example, `obj?.prop` is lexed as `obj` `?` `.` `prop`.

##### Operator forwarding on unions (tag dispatch / jump tables)

Many operators in Warble are *applied to* their left operand and are therefore subject to overload resolution and structural checking (for example member access `.` and calls `()`). Borrows and other reference-like types are specified in terms of forwarding; unions follow the same overall pattern.

In contrast, the union operators `||` and `&&` are built-in control operators defined directly in terms of pass/fail state; they are not overloadable and are not "applied to" their left operand in this dispatch sense.

When such an operation is applied to a **union** value, the compiler must account for the fact that the active arm is not known at compile-time. Conceptually, the operation is lowered as a dispatch on the union's runtime tag:

* For each arm type, select the appropriate member / overload / implementation for that arm.
* Generate a code region for each arm that performs the operation with full static type knowledge.
* Emit one indirect jump (or equivalent dispatch) up front, indexed by the runtime tag, to enter the correct region.
* Each region ends with a direct jump to a common join point.

Type rule:

* If the union has arms `A0 | A1 | ... | An` and the operation is well-typed on every arm, then the overall result type is the union of the per-arm result types.
* The compiler flattens the resulting union (§4.3.2).
* When the union has only a single arm, the compiler is expected to elide the runtime tag and treat the value equivalently to the non-union case.

Implementations typically use a compact jump table indexed by the union's tag, but any equivalent strategy is permitted.

For an unguarded operation on a union (for example `pet.speak()` where `pet` is a union), the operation must be well-typed for **every** arm of the union. If any arm does not support the operation (for example `null.speak()`), it is a compile-time error.

The postfix `?` operator is the primary mechanism for making such operations conditional on the union's pass/fail state by splitting evaluation into a failing path and a passing path.

##### 4.3.4.1 Standalone form: `x?`

The expression `x?` evaluates `x` and produces a union value `u` (a pass/fail union).

Overload hook:

* If the static type of `x` defines a member function `question()` that is callable with zero arguments, then `x?` is lowered as a call to `x.question()`.
* The return type of `question()` must be a union type. Otherwise it is a compile-time error.

Default behavior (when there is no `question()` member):

* If `x` is already a union value, `x?` is the identity (it yields the same union).
* Otherwise, `x?` treats `x` as a union with a single passing arm.

Because a single passing arm does not require a runtime tag, `x?` on a non-union value is typically optimized to a no-op unless it is immediately followed by guarded postfix chaining.

##### 4.3.4.2 Guarded postfix chaining

Because `?` is a high-precedence postfix operator, it naturally composes with other high-precedence postfix operators (such as `.` member access or `()` call) using normal operator precedence and associativity.

In a postfix chain like `x?.member()` (lexed as `x` `?` `.` `member` `(` `)`), the compiler lowers `x?` into a pass/fail split:

* In the failing path, the expression stops and yields the failing union state.
* In the passing path, the remainder of the postfix chain is evaluated on the passing arms only.

No special fusion rules are required: the set of operations that occur in the passing path is determined entirely by the parsed expression tree.

Examples:

```warble
let name = user?.name;
let sound = animal?.bark();
```

Conceptually:

1. Evaluate `u = x?` (as specified above).
2. If `u` is failing, the rest of the postfix chain is skipped and `u` is yielded.
3. Otherwise, `u` is passing and the postfix chain continues, applying each postfix operator by union forwarding on the passing arms.

The result type is a union containing:

* all failing arms of `u`, plus
* the result states produced by applying the remainder of the postfix chain to the passing state(s) of `u`.

The failing arms of `u` appear as the prefix of the result union in the same order, so a failing `u` state can be forwarded as a failing result state without tag remapping.

The postfix-chain operations in the passing path must be well-typed for **every passing arm** of `u` after the fail arms are excluded. If any passing arm does not support the operation, it is a compile-time error.

This is what makes patterns like `pet?.speak()` usable when `pet` is `null || Cat || Dog`: the failing `null` arm short-circuits, and the `Cat` and `Dog` arms dispatch to their respective `speak()` implementations.

Illustrative lowering (conceptual):

```warble
let u = pet?;
if (is_failing(u)) {
  return u; // propagate fail state (e.g. null)
}

// passing: dispatch based on active arm
if (u)
is (Cat) { return pass this.speak(); }
is (Dog) { return pass this.speak(); }
```

This pseudo-code is illustrative only. Real lowering uses jump-table-style dispatch and may avoid materializing intermediate unions.

##### 4.3.4.3 Two-stage dispatch for `?` chaining

When `x` is a union that includes failing arms (for example `null || Cat || Dog`) and an operation is written as `x?.member` / `x?.member()` / `x?[index]` (lexed as `x` `?` `.` ...), the compiler typically performs two distinct dispatch steps:

1. A fast pass/fail split based on `fail_count`.
2. In the passing path only, an arm-dispatch jump table to apply the operation to each passing arm.

This is why `x.member()` may be ill-typed (because it would require generating code for failing arms like `null.member()`), while `x?.member()` is well-typed: the `?` causes the failing arms to take a separate path that does not attempt the operation.

After these steps, evaluation continues normally at the join point. Any subsequent lower-precedence operators (such as `&&` and `||`) operate on the resulting union value produced by the join point, using their usual union semantics (§4.3.1).

### 4.4 Type Inference & Compatibility

### 4.5 Conversions & Coercions

## 5 Expressions & Operators

### 5.1 Evaluation Order & Short-Circuiting

### 5.2 Built-in Operators

#### 5.2.1 Control-flow Unary Operators `await`, `expect`, `assume`

| Operator     | Operand requirement | Fast path                                            | Slow path                                                 | Result type |
| ------------ | ------------------- | ---------------------------------------------------- | --------------------------------------------------------- | ----------- |
| `await`      | a promise union (a union with `unresolved` and `detached` fail arms), or a `future` that forwards to such a promise | If not unresolved: returns the union with `unresolved` and `detached` removed. If the operand is a future, it is consumed and its frame is destroyed as part of this fast path. | Suspends the current thread while the promise tag is `unresolved`, repeatedly returning to the work queue between checks. If the operand is a future, it remains owned by the awaiting site and is destroyed once the await completes. | union without `unresolved`/`detached` |
| **`expect`** | a union value        | If passing: returns the union with all fail arms removed. | If failing: `return fail <fail-value>`                    | pass-only union |
| **`assume`** | a union value        | If passing: returns the union with all fail arms removed. | If failing: `panic <fail-value>`                          | pass-only union |

* All three are **prefix** verbs with the same precedence (just above unary
  `&` and `@`).

* Each may skip the remainder of the current statement: `await` can suspend,
  `expect` can early-return, `assume` can abort.

* `expect` is only legal inside a function body. Because its failure path performs an implicit `return fail`, using `expect` forces the enclosing function to use explicit `pass`/`fail` markers for any other `return` or `yield` statements.

* `assume` may be used anywhere (including module top-level). Its failure path uses `panic`, so it does not impose any restrictions on the surrounding function's return/yield forms.

* The compiler lowers `expect`/`assume` to:

  ```warble
  let __tmp = expr;
  if (is_failing(__tmp)) {
    // expect → return fail fail_value(__tmp);
    // assume → panic fail_value(__tmp);
  }
  let value = drop_fails(__tmp);
  ```

* Because `assume` aborts the program on failure, the compiler may treat the
  failing path as non-returning (for example by emitting an `unreachable` IR
  node), enabling additional optimization.

> **Guideline:** Default to `expect`.  Reach for `assume` only when recovery is
> impossible or not worthwhile (e.g. out-of-memory on a desktop program).

#### 5.2.2 Postfix Union Operator `?`

`?` is a unary postfix operator that produces a pass/fail union and introduces a control-flow split (fail vs pass) at that point in the expression.

* Standalone: `x?` yields a union view of `x` (identity for unions; a single passing arm for non-unions).
* Chaining: `x?.prop` and `x?.method()` short-circuit when `x?` is failing.

Full semantics (including the overload hook `question()`) are defined in §4.3.4.

### 5.3 Operator Overloading via Symbols

### 5.4 Call Forms (Pipeline `->` and Callable Literal Shorthand)

The foundation of function calling in Warble is the **pipeline operator** `->`. This binary operator invokes its right-hand side with its left-hand side as the argument:

```warble
arg -> func;           // calls func with arg
"Hello!" -> print;     // calls print with "Hello!"
```

Like any binary operator, `->` produces a result—the return value of the called function.

A pipeline call is resolved in two phases (detailed in §6.4):

1. **Phase 1 (whole-value match):** The compiler searches for an overload that accepts the argument as a single value.
2. **Phase 2 (automatic destructuring):** If phase 1 fails and the argument's kind supports destructuring, the compiler attempts to map the argument's elements onto the function's parameters.

Because `->` works with any expression, not just literals, it is equally effective when the left-hand side is an identifier or a more complex expression—the compiler uses the static type to decide how to resolve the call:

```warble
const tuple = (1, true, 'c');
tuple -> func;    // compiler knows it's a tuple and can destructure it
```

#### Callable Literal Shorthand

Most literals in Warble are classified as **callable literals**. A callable literal can be placed directly adjacent to a callable value to form a function call, without writing the `->` operator explicitly. The arguments appear to the right of the function name:

```warble
print("Hello, World!");        // tuple literal
func{ y = 2, x = 1 };         // object literal
vector<i32>;                   // enum literal
process[data];                 // array literal
func'c';                       // character literal
func"Hello world";             // string literal
```

Numeric literals are also callable literals, but because digits can appear in identifiers, the shorthand form is written in the **opposite direction**—the number comes first:

```warble
42func;             // shorthand for 42 -> func
3.14process;        // shorthand for 3.14 -> process
```

Writing `func42` would be parsed as the single identifier `func42`, so the reversed direction is required.

**Keyword literals** (`true`, `false`, `null`, `undefined`, `readonly`) do **not** have a shorthand form. To call a function with a keyword literal, either wrap it in a destructurable container or use the pipeline operator:

```warble
func(null);         // tuple is destructured — passes null as parameter
null -> func;       // pipeline — passes null directly
```

#### Pipeline vs. Shorthand: The Phase 1 Distinction

The pipeline form and callable literal shorthand are *almost* identical, but there is one critical difference:

* **Pipeline** (`arg -> func`): Always begins with phase 1—checking whether there is an overload that accepts the argument as a whole, single value. Only if that fails does the compiler proceed to phase 2 (destructuring).
* **Shorthand** (`func(args)`): **Skips phase 1** for literal kinds that support destructuring (tuples, objects, arrays, enums). The literal is always destructured directly into parameters. In this sense, the literal itself takes the place of the call operator—it *is* the argument list, not a single value being passed.

```warble
func();             // always destructures the empty tuple — matches zero-parameter overload
() -> func;         // first checks for an overload accepting an empty tuple; only then destructures

func(1, 2);         // always destructures — matches a two-parameter overload
(1, 2) -> func;     // first checks for an overload accepting a 2-tuple; only then destructures
```

This distinction does **not** apply to non-destructurable literals (characters, strings, numerics), because destructuring is never an option for them. For those kinds, the shorthand and pipeline forms are truly identical:

```warble
func"Hello";        // identical to "Hello" -> func
42func;             // identical to 42 -> func
```

To pass a destructurable literal as a single value using the shorthand form, wrap it in another literal that will be destructured:

```warble
func(());           // outer tuple is destructured, passing the inner empty tuple as one parameter
func((1, 2));       // outer tuple is destructured, passing the inner (1, 2) as one tuple parameter
```

## 6 Functions & Calling Convention

### 6.1 Function Declarations

All functions in Warble are first-class values declared using function literal syntax (see §4.1.11). There is no separate `fn` or `function` keyword. A function is simply a value assigned to a binding:

```warble
let add = (a, b) { return a + b; };
let greet = (name) => "Hello, {name}!";
```

Functions are called using the pipeline operator `->` or callable literal shorthand (see §5.4).

### 6.2 Parameters, Default Values, Packs

Function parameters are declared in the parameter list and are immutable by default (use `mut` for mutable parameters). See §4.1.11 for full parameter syntax.

#### Parameter Packs

A parameter list may end with a **parameter pack**, written as `...` followed by an identifier. The pack collects any remaining arguments that were not matched by preceding parameters.

```warble
let first = (head, ...tail) { return head; };
```

The type of the pack depends on the kind of the argument being destructured:

* **Tuple argument** → the pack is a tuple containing the remaining elements.
* **Array argument** → the pack is an array containing the remaining elements.
* **Enum argument** → the pack is an enum containing the remaining elements.
* **Object argument** → the pack is an object containing all properties that did not match a named parameter.

Only one parameter pack is permitted per parameter list, and it must appear at the end.

A parameter pack may be empty (collecting zero elements), so a function with a pack can accept shorter argument sequences than the total count might suggest—as long as every non-pack parameter receives a value.

#### Destructuring-Typed Parameters

A parameter list may itself contain a **destructuring declaration** in place of a flat list of individual parameter names. When it does, the function's parameter list explicitly states what kind of argument it expects to destructure, restricting invocation to only that kind.

Warble supports this for all four structured literal kinds: objects, tuples, arrays, and enums.

```warble
const byObject = ({ x, y }) {};     // requires object argument
const byTuple  = ((a, b)) {};       // requires tuple argument
const byArray  = ([a, b]) {};       // requires array argument
const byEnum   = (<T>) {};          // requires enum argument
```

Compare this with a flat parameter list, which places no restriction on the argument kind:

```warble
const byAny = (a, b) {};   // accepts tuple, array, object, or any other destructurable kind
```

A destructuring-typed parameter list is simply a more specific overload signature. It participates in the same two-phase resolution as any other overload (see §6.4). The destructuring form in the parameter list is only used to match arguments of the stated kind; it will not match arguments of a different kind.

```warble
byObject{ x = 1, y = 2 };          // valid — object matches
byObject(1, 2);                     // error — tuple does not match an object parameter list

byTuple(10, 20);                    // valid — tuple matches
byArray[10, 20];                    // valid — array matches
byEnum<MyType>;                     // valid — enum matches
```

This feature is only available for the four structured literal kinds. Strings, characters, and primitive types do not have a destructuring form that would make this meaningful, so they cannot appear as destructuring-typed parameter lists.

### 6.3 Implicit vs. Explicit Calls

Warble provides two ways to call a function:

* **Explicit (pipeline):** `arg -> func` — uses the pipeline operator.
* **Implicit (shorthand):** `func(arg1, arg2)` — a callable literal adjacent to a callable value.

The key semantic difference: the shorthand form **always destructures** callable literals that support destructuring (tuples, objects, arrays, enums), skipping the whole-value match phase entirely. The pipeline form first attempts to pass the argument as a whole value before falling back to destructuring.

For non-destructurable kinds (characters, strings, numerics), both forms are identical in behavior. See §5.4 for syntax details and §6.4 for the full resolution algorithm.

### 6.4 Overload Resolution

When a function is called via the pipeline operator `arg -> func`, the compiler resolves the call in two phases:

#### Phase 1: Whole-Value Match

The compiler searches for an overload of `func` that accepts exactly one parameter whose type is compatible with the type of `arg` as a whole.

```warble
let process = (values: [i32; 3]) { /* handles the array as one value */ };
let process = (a, b, c) { /* handles three separate integers */ };

[1, 2, 3] -> process; // Phase 1 matches the first overload (array of 3 i32)
```

If a matching overload is found, the call is resolved and phase 2 is skipped.

#### Phase 2: Automatic Destructuring

If no whole-value overload matches, the compiler attempts to destructure the argument into the function's parameters. This phase is skipped entirely if the argument is a **non-destructurable** type (booleans, integers, floats, characters, strings, marker symbols). These types are treated as indivisible units.

The destructuring strategy depends on the argument's kind:

##### Indexed Destructuring (Arrays, Tuples, Enums)

For indexed containers, elements are mapped to parameters **by position**:

* The first element maps to the first parameter, the second to the second, and so on.
* If any element's type is incompatible with the corresponding parameter's type, that overload is rejected.
* The number of elements must exactly equal the number of parameters—unless the parameter list ends with a parameter pack (`...identifier`), which collects any remaining elements into a new container of the same kind.

```warble
let plot = (x, y) { /* ... */ };

(10, 20) -> plot;          // Phase 1 fails; Phase 2 destructures: x=10, y=20
[10, 20] -> plot;          // same two-phase process, indexed destructuring of the array

let first = (head, ...rest) { return head; };

(1, 2, 3, 4) -> first;    // head=1, rest=(2, 3, 4) — rest is a tuple
[1, 2, 3, 4] -> first;    // head=1, rest=[2, 3, 4] — rest is an array
```

##### Named Destructuring (Objects)

For objects, properties are mapped to parameters **by name**:

* Each parameter name must exactly match a property name in the argument object.
* Every non-pack parameter must find a matching property. However, extra properties in the argument object that have no corresponding parameter are allowed—they are simply ignored (or collected by a pack).
* If a parameter pack is present, it collects all properties that did not match any named parameter, forming a new object.
* Because matching is by name, the order of properties in the argument and parameters in the function is irrelevant (though the pack must still be last in the parameter list).

```warble
let greet = (name, age) { print("{name} is {age}"); };

{ age = 30, name = "Alice" } -> greet;              // name="Alice", age=30 (order irrelevant)
{ name = "Bob", age = 25, role = "dev" } -> greet;  // name="Bob", age=25 (extra "role" ignored)

let extract = (id, ...rest) { return rest; };

{ id = 1, x = 10, y = 20 } -> extract;             // id=1, rest={ x = 10, y = 20 }
```

#### Shorthand Form and Phase Skipping

When using callable literal shorthand (§5.4), phase 1 is **skipped** for argument literals whose kinds support destructuring. The compiler proceeds directly to phase 2:

```warble
greet("Alice", 30);                        // always destructures — never checks for a tuple overload
greet{ age = 30, name = "Alice" };         // always destructures — never checks for an object overload

// Pipeline form runs both phases:
("Alice", 30) -> greet;                    // checks for tuple overload first, then destructures
{ age = 30, name = "Alice" } -> greet;     // checks for object overload first, then destructures
```

#### Destructuring-Typed Parameter Matching

When a function overload uses a destructuring-typed parameter list (§6.2), the overload is only eligible when the argument is of the required kind. An argument of a different kind cannot match that overload in any phase:

```warble
let func = ({ x, y }) { /* object only */ };
let func = ((a, b))  { /* tuple only  */ };

{ x = 1, y = 2 } -> func;   // matches the object overload
(1, 2) -> func;              // matches the tuple overload
[1, 2] -> func;              // no match — no array overload exists
```

Destructuring-typed overloads (§6.2) are only eligible in phase 2. Phase 1 does not consider them, so an argument of the matching kind simply falls through to phase 2, where it is destructured normally.

### 6.5 Compile-Time Specialization

Functions in Warble are inherently specializable. Parameters typed as `auto` create a new specialization for each distinct set of argument types encountered at call sites (see §4.1.11). Enum destructuring in parameter lists enables template-like patterns for passing type symbols explicitly:

```warble
let make = (<T>) => { element_type = T, data = [] };
let instance = make<i32>;   // specializes with T = i32
```

## 7 Statements & Control Flow

Warble's statements provide structured control flow, clearly and predictably managing how code executes. Statements include blocks, conditionals, loops, and jumps, each serving distinct roles in controlling program execution.

### 7.1 Block Statements

In Warble, a **block statement** groups multiple statements into a single, compound statement. Unlike many curly-brace languages, Warble explicitly distinguishes between **object literals** (using `{}`) and **scope blocks** (using `do {}`). This explicit differentiation prevents parsing ambiguity and enhances readability.

Warble block statements have these characteristics:

* They form an explicit scope boundary, introducing a new nested scope.
* Declarations inside a block are scoped to that block and inaccessible from outside.
* Blocks may contain any number of statements, including further nested blocks, declarations, expressions, and control-flow constructs.

#### 7.1.1 Scoped Block (`do`)

To explicitly define a scoped block in Warble, you must use the `do` keyword followed by curly braces:

```warble
do {
  let localValue = 42; // Scoped to this block
  print(localValue);     // Accessible here
}

print(localValue); // Error: `localValue` is out of scope
```

Unlike object literals (`{}`), `do {}` blocks do not produce values or data structures. They exist purely to create local scopes and organize code logically.

**Nested blocks** clearly manage inner scopes and shadow outer-scope variables:

```warble
let value = 1;

do {
  let value = 2;  // shadows outer `value`
  
  do {
    let value = 3; // shadows the previous block's `value`
    print(value);    // prints 3
  }
  
  print(value);      // prints 2
}

print(value);        // prints 1
```

This explicit scope management, combined with clearly defined shadowing behavior, ensures code clarity and maintainability.

### 7.2 Statement Functions

Warble provides a special construct known as a **statement function**, allowing statements to be directly embedded within expressions. This capability enhances expressiveness and conciseness, enabling a smooth integration between statements and expressions.

Statement functions automatically translate into **immediately-invoked function expressions (IIFEs)**. The compiler wraps your statement inside a function that is immediately invoked, making it behave just like any other expression, allowing its result to be assigned to a declaration or passed directly into other expressions.

#### 7.2.1 Syntax & Motivation

Any single Warble statement can appear in an expression context, such as on the right-hand side of a declaration. This is especially useful for conditionals, loops, and other control-flow constructs that you want to use directly within expressions:

```warble
let value = if (condition) {
  return 42;
} else {
  return 0;
};
```

The compiler interprets this as:

```warble
let value = [condition]() {
  if (condition) {
    return 42;
  } else {
    return 0;
  }
}(); // Immediately invoked
```

Using a statement function makes your intent clear and concise, removing the need for explicit function wrapping.

#### 7.2.2 Return Rules & Type Consistency

Within a statement function, `return` statements indicate the expression's resulting value. All possible code paths must return a value of the same (or compatible) type. Warble enforces this at compile-time, ensuring safety and consistency:

```warble
// Valid: consistent return types
let result = if (x > 0) {
  return "Positive";
} else if (x < 0) {
  return "Negative";
} else {
  return "Zero";
};
```

However, some statements—like loops—do not inherently guarantee that a `return` is executed on every path. For these cases, you use the special `default` keyword at the end of the statement function to provide a fallback value:

```warble
let found = for v in (values) {
  if (v == target) {
    return true;
  }
} default false;
```

Here, the compiler effectively interprets this as:

```warble
let found = [values, target]() {
  for v in (values) {
    if (v == target) {
      return true;
    }
  }
  return false; // From `default`
}();
```

**Note:** The `default` keyword used in this manner is exclusive to statement functions. You cannot use it with normal loops or other statements outside this context, as it wouldn't make semantic sense.

#### 7.2.3 Inlining & Zero-Cost Guarantee

Because statement functions are always immediately invoked and have no references stored anywhere, the compiler guarantees they will always be fully inlined. No actual function calls or runtime overhead occurs:

```warble
// This incurs no overhead; it's fully optimized:
let value = if (a) { return b; } else { return c; };
```

The function interpretation is purely conceptual—an abstraction for understanding control flow. The compiled program never creates a real callable function object. You can freely use statement functions without concern about performance.

Statement functions are syntactic sugar—convenience shorthand for constructs you could write explicitly as an immediately-invoked function. However, because Warble handles this optimization seamlessly, there's no reason to write such functions manually. Statement functions are particularly useful given Warble's omission of a traditional ternary operator. They provide a clear, expressive alternative:

```warble
let minValue = if (a < b) { return a; } else { return b; };
```

This construct neatly captures conditional logic within expression contexts, promoting clarity, consistency, and conciseness throughout Warble programs.

### 7.3 Conditional Statements

Conditional statements in Warble allow branching execution paths based on runtime conditions. Warble's primary conditional construct is the familiar `if` statement, optionally followed by an `else` branch.

#### 7.3.1 Branching (`if`, `else`)

The basic syntax of Warble's `if` statement resembles that of many traditional languages. The condition must be enclosed in parentheses, followed by a body, either inline or within curly braces (`{}`):

```warble
if (x > 0) {
  print("Positive");
} else {
  print("Not positive");
}
```

An inline form is also currently permitted:

```warble
if (condition) print("Condition met"); // Currently valid, though this may change
```

However, allowing inline bodies remains a design decision under consideration and may be disallowed in the future for clarity or stylistic reasons.

**Condition Topics (`this`)**

Each conditional arm that has a condition expression (an `if` or `else if` arm) introduces a new topic for the duration of that arm's body.

Within that arm's body, `this` refers to the value produced by evaluating the condition expression.

Unconditional `else` arms do not introduce a topic.

Example:

```warble
if (a || b) {
  // `this` is the union produced by `a || b`
  print(this);
} else {
  // `this` is not introduced by an unconditional `else` arm
  print("no match");
}
```

#### 7.3.2 Conditional Matching (`if`, `is`, `has`, `from`)

Warble supports **conditional matching** as an extension of the normal `if` statement.

In this form, the expression inside `if ( ... )` produces a value, and the subsequent `is`, `has`, or `from` clauses test that value. The first clause that passes executes its body. This integrates naturally with `else if` and `else`.

```warble
if (value)
is (i32) { print("Is i32"); }
has (i32) { print("Has i32"); }
else if (another_value > 0) {
  print("Another value is positive");
} else {
  print("No conditions met");
}
```

There is no dedicated fallback clause for conditional matching; use `else` when you want a catch-all.

**Matching Clauses**

* **`is (pattern)`**: Matches by type or value, depending on the type of the tested value.
* **`has (pattern)`**: Matches based on the presence of required structure (properties, methods, or other constraints).
* **`from (pattern)`**: Matches based on provenance (for example, whether a value originates from a particular constructor).

Each matching clause introduces a topic for the duration of its body. In a clause body, `this` refers to the matched value produced by that clause. An unconditional `else` arm does not introduce a topic.

#### 7.3.3 Type-Validity Branching (`try`)

Warble includes a `try` statement for **type-validity branching**.

`try` is not related to exceptions or error catching; Warble does not use exceptions. Instead, `try` is a compile-time mechanism for selecting code based on whether an expression (or a block of code) is *semantically valid* under the type system.

Conceptually, every `try` desugars into an `if` whose condition is a compile-time constant (`true` or `false`). When the condition is `false`, the corresponding body is deleted, preventing invalid code from becoming part of the program.

**Two Forms**

1) **Expression form**: `try (expression) { ... }`

The compiler attempts to type-check the expression. If it is valid, the branch becomes `if (true) { ... }`. If it is not valid, the branch becomes `if (false) { ... }` and the body is deleted.

```warble
try (obj.method()) {
  obj.method();
} else {
  fallback_operation();
}
```

2) **Body-only form**: `try { ... }`

This form uses the validity of the body itself as the condition. One way to think about it is that the compiler checks whether the body *could* be type-checked as-if it were inside a separate compilation context, and uses the result as the `try` condition.

```warble
try {
  some_operation();
} else try {
  another_operation();
} else {
  fallback_operation();
}
```

**Chaining and Mixing with `if`**

Because `try` is an alternative spelling of an `if` with a compile-time-known condition, it composes naturally with other conditional arms:

* `else try` chains multiple mutually exclusive type-validity tests.
* `else if` and `else` work normally.

**What "Validity" Means**

Validity is determined by semantic/type checking of otherwise well-formed code. `try` can be used to guard operations that are syntactically correct but may be ill-typed, such as a member access that may not exist, or a call that may not be callable.

`try` does not suppress structural errors.

* **Syntax errors** are still errors.
* **Unresolved identifiers** are still errors.

For example, this still produces an error because `hi` does not resolve to any declaration:

```warble
try {
  hi;
} else {
  // ...
}
```

In contrast, an operation like `obj.property()` may be syntactically valid, yet semantically invalid if `obj` has no such member or the member is not callable; `try` turns that semantic failure into a compile-time `false` branch, deleting the body.

### 7.4 Loop Statements

Warble provides standard loop constructs familiar from other programming languages, enabling repeated execution of statements or expressions. Loops in Warble include `while`, `repeat ... while`, a powerful single-form `for` loop for concise iteration over iterables or ranges, and the module-level `tick` loop.

#### 7.4.1 While (`while`)

The `while` loop repeatedly evaluates a condition and executes its body as long as the condition remains true. Its syntax closely resembles the Warble `if` statement:

```warble
while (condition) {
  // loop body executes repeatedly while condition is true
}
```

The `while` body is a statement arm that introduces a topic. Within the loop body, `this` refers to the value produced by evaluating the condition expression for that iteration:

```warble
// The loop body can use `this` to refer to the condition value.
while (getNextItem()) {
  print(this);
}
```

This feature simplifies loops where the condition yields useful data. The loop continues until the condition returns a falsy value, ending the iteration.

#### 7.4.2 Repeat While (`repeat ... while`)

The `repeat ... while` loop executes its body at least once before evaluating its condition. Because the condition is checked **after** each iteration, there is no condition topic available within the body before the first execution. If the body needs access to the condition value, use `while`.

```warble
repeat {
  // This body always executes at least once
} while (condition)
```

A trailing semicolon after `while (condition)` is optional.

The loop continues as long as the condition remains true. Once the condition evaluates to false, iteration stops immediately.

#### 7.4.3 For (`for`)

Warble's `for` loop is specifically designed for concise iteration over ranges or iterables. Warble does **not** support a traditional C-style `for` loop with explicit initialization, condition, and increment expressions. Instead, it provides a simpler and more powerful syntax designed explicitly for iteration.

The general syntax is:

```warble
for declaration in (iterable) {
  // loop body
}
```

While this differs from many other languages, the intent is to make the structure of the loop header unambiguous. A `for` header is made of two distinct parts: a *declaration* and an *iterable expression*. The combined text `declaration in iterable` is **not** a legal expression in Warble, and `in` is **not** a binary operator. By keeping `declaration in` outside the parentheses, Warble avoids accidentally suggesting that the whole header is a single parenthesized expression.

Conceptually, evaluation proceeds as follows:

1. Evaluate the parenthesized expression to produce an iterable.
2. On each iteration, extract the next value from that iterable.
3. Use that value to initialize the declaration for that iteration.

* **Declaration**:
  Declares the loop variable. Because the loop header expects a declaration, the `let` keyword may be omitted. Loop variables are immutable unless declared with `mut`:

  ```warble
  for i in (0..10) {
    print(i);
  }
  ```

* **Iterable**:
  Any expression evaluating to an iterable structure (range, array, string, or user-defined iterable types).

Stepping (changing an iterable to skip values or reverse iteration) is intentionally not a syntactic feature. If you need a different step behavior, use library helpers that transform an iterable into a new iterable before passing it to `for`.

* **Multiple Iteration Variables**:
  You may declare multiple iteration variables simultaneously by separating each declaration-expression pair with commas:

  ```warble
  for a in (arrayA), b in (arrayB), c in (arrayC) {
    print(a, b, c);
  }
  ```

  The loop advances each iterable in lockstep. Iteration stops as soon as **any** of the iterables transitions to `exhausted`.

This flexible `for` loop design makes Warble iterations expressive, concise, and efficient, covering most common iteration patterns without the complexity of a traditional `for` loop syntax.

#### 7.4.4 Tick (`tick`)

The `tick` loop is a special loop form used for module-level scheduling.

Syntax:

```warble
tick {
  // body
}
```

Rules:

* A `tick` loop may appear anywhere in **module context**. It is a compile-time error to place a `tick` loop inside a function body.
  * **Module context** means code that executes as part of the module's top-level evaluation, including nested statement arms (`if`, `while`, `for`, `do`, etc.). Function bodies (including function literals) are not module context.
* A module may contain multiple `tick` loops. Control flows forward: after a `tick` loop terminates, execution continues with the statements that follow it.
* A `tick` loop runs until it terminates via `break` or until the program is shutting down (in which case it terminates as if `break` had executed).
* A module must not contain any `export` declaration after the compiler has encountered any `tick` loop while scanning the module from top to bottom. In other words: all exported declarations must appear (textually) before the first `tick` loop in the file, even if that `tick` loop is nested inside other module-context statements.
  * This restriction ensures that once a module **successfully suspends** at a `tick` boundary (advancing its cycle counter), its exported state is fully initialized and will never be partially initialized due to later module-context code.

Scheduling semantics:

* Each iteration of a `tick` loop ends with an implicit **suspension point**. This suspension point is not written by the user, but it is part of the loop's semantics.
* Suspending returns control back to the runtime scheduler, which may run other modules or other work before resuming this module for the next iteration. The runtime may also immediately continue the same module to catch up (§13.2.6).

The `tick` loop is the primary surface syntax hook for the module scheduler described in §13.

### 7.5 Jump Statements (`break`, `continue`, `return`, `yield`, `panic`)

Jump statements alter the normal execution flow by abruptly transferring control to a different part of your program. Warble provides five types of jump statements: `break`, `continue`, `return`, `yield`, and `panic`. These statements behave similarly to those in many traditional languages but include some useful and distinctive features.

#### 7.5.1 Break & Continue (`break`, `continue`)

* **`break`** immediately terminates the nearest enclosing loop (`for`, `while`, or `repeat ... while`). Control transfers to the statement immediately following that loop.

* **`continue`** immediately skips the rest of the current loop iteration, jumping directly to evaluating the loop condition again and starting the next iteration if applicable.

Example of basic usage:

```warble
for let i in (0..10) {
  if (i == 5) break;     // Exits loop when i == 5
  if (i % 2 == 0) continue; // Skips even numbers
  print(i); // Prints odd numbers less than 5
}
```

**Stacked Break and Continue:**

Warble uniquely allows stacking of `break` and `continue` statements to directly exit multiple nested loops without extra logic:

```warble
for x in (0..10) {
  for y in (0..10) {
    if (condition_met(x, y)) break break; // Exits both loops immediately
  }
}
```

This is convenient, avoiding cumbersome solutions like flag variables to communicate loop termination between nested loops.

* You must not stack more `break` or `continue` keywords than there are loops. Doing so results in a compile-time error.
* `break` and `continue` keywords are only valid inside loops. Using them outside a loop context results in a compile-time error.
* Stacking applies only within a single function context. You cannot use them to exit loops defined in another function or a surrounding context.

**No Expressions Allowed:**

In Warble, neither `break` nor `continue` allows expressions (unlike some languages that permit a value to be returned from these keywords). They are simple, expressionless jump statements.

#### 7.5.2 Return (`return`)

The `return` keyword immediately terminates execution of the current function context and returns control to the caller. Optionally, a `return` may provide an expression whose value is returned to the caller:

```warble
let add(a, b) {
  return a + b; // Returns the sum of `a` and `b`
};
```

* A function without an explicit `return` statement implicitly returns `null`. This behavior matches a function explicitly written as `return null;`.

**Multiple Returns (Structured Literals):**

Warble does not directly support multiple return values using the `return` keyword. However, you can easily emulate multiple return values using structured literals, like tuples or objects, without performance overhead:

```warble
let getCoordinates() {
  return (x, y, z); // Returns a tuple
};

let (x, y, z) = getCoordinates(); // Destructures the tuple
```

The compiler optimizes this pattern efficiently, ensuring no runtime overhead compared to traditional multi-return semantics.

**Consistent Return Types (non-union functions):**

For non-union functions, Warble requires all explicit `return` statements within a function to return the same type. Any mismatch results in a compile-time error:

```warble
// Error: Inconsistent return types
let example(a) {
  if (a) return "string";
  return 42; // Compiler error due to type mismatch
};
```

**Union Returns (`return` modifiers):**

Union functions may use `return` to produce a union value by categorizing each returned arm as `pass` or `fail`, and optionally producing the result asynchronously using `async`.

Examples:

```warble
let getValue(condition) {
  if (condition == "number") {
    return pass 42;
  } else if (condition == "string") {
    return pass "Warble";
  } else {
    return fail "Unknown condition";
  }
};
```

Rules:

* `async` must appear before `pass`/`fail` when present.
* `pass` and `fail` are mutually exclusive.
* If any return/yield in a function uses `pass` or `fail`, all return/yield statements in that function must use exactly one of them.

#### 7.5.3 Yield (`yield`)

The `yield` keyword is a control-flow statement, like `return`. It suspends a generator function and produces a value for the caller. Any function that contains a `yield` statement is a generator function.

Like `return`, `yield` may be augmented with `async`, `pass`, and `fail`:

```warble
yield pass value;
yield fail error;
yield async pass value;
```

When a generator is exhausted, its union tag transitions to `exhausted` (a failing-state arm). In the `exhausted` state, the union's value slot is not meaningful and must not be accessed.

#### 7.5.4 Panic (`panic`)

The `panic` keyword immediately aborts execution by producing an error message and exiting the program.

Like `return`, `panic` may optionally include an expression:

```warble
panic;
panic "unreachable";
```

Unlike `return` and `yield`, `panic` cannot be augmented with any keywords such as `async`, `pass`, or `fail`.

## 8 Objects & Composition

### 8.1 Object Literals & Property Declarations

### 8.2 Spread (`...`) & Shallow Copy

### 8.3 `this` Binding

Warble uses an internal compiler concept called the **topic stack** to track certain in-progress constructs while building IR.

Some expressions are **topic expressions**: when the compiler begins building such an expression it pushes a topic onto the stack, and when it finishes building that expression it pops that topic.

Users can access the topic stack only through two keywords:

* `this` resolves to the topic symbol at the top of the topic stack.
* `that` resolves to the topic symbol one below the top of the topic stack.

Internally, topics are stored as symbol indexes; `this`/`that` behave like identifier lookup, producing the corresponding topic symbol.

Errors:

* `this` is a compile-time error if the topic stack is empty.
* `that` is a compile-time error if the topic stack has fewer than two entries.

Topic expressions specified so far:

* **Object literals**: when the compiler begins building an object literal, it pushes the symbol index of that object literal onto the topic stack.
* **Statement arms**: each arm with a condition introduces a topic for the duration of that arm's body. This includes `if` / `else if` arms, `while` bodies (condition checked before entry), and conditional-matching clause bodies (`is`, `has`, `from`). Unconditional `else` arms do not introduce a topic.

These rules are what make `this` (and in nested cases, `that`) meaningful inside object literal bodies and inside statement arm bodies.

Topic capture across contexts:

* The topic stack is not shared across compilation contexts. In particular, a function body does not automatically inherit the surrounding topic stack.
* If a function needs to use `this`/`that` from the surrounding context (for example, a member function declared inside an object literal), it must capture them.
* If a function literal omits an explicit capture list (`[ ... ]`), it uses automatic capturing and can capture `this`/`that` simply by referencing them in its body.

Example:

```warble
let obj = {
  a = 1,

  // Implicitly captures `this`.
  get_a = () { return this.a; },

  // Explicitly captures `this`.
  get_a2 = [this] { return this.a; },

  // Does not capture anything; `this` is an error.
  get_a3 = [] { return this.a; },
};
```

### 8.4 Custom Property Resolution (operator overrides)

### 8.5 Function-Based "Constructors"

Warble has no `class`, `struct`, or `new` keyword. There is no built-in notion of a nominal type or a compiler-privileged constructor. Instead, Warble relies entirely on ordinary functions to produce structured values, using a pattern conventionally called a **constructor**.

A constructor is any function that returns a value of a particular shape. Nothing in the language distinguishes a constructor from any other function—the term is purely conventional, describing *how* a function is used rather than anything special about how it is defined:

```warble
// A simple constructor for a 2D point
let Point = ({ x, y }) => { x, y };

const p: Point = { x = 1, y = 2 };   // calls: { x = 1, y = 2 } -> Point
const q = Point{ x = 3, y = 4 };     // same call via shorthand
const r = { x = 5, y = 6 } -> Point; // same call via explicit pipeline
```

Because a constructor is just a function, it participates in normal overload resolution. Multiple overloads can represent different ways to construct the same conceptual value:

```warble
let Color = (r, g, b) => { r, g, b };                    // from components
let Color = (hex: u32) => { /* decompose hex */ };        // from a packed integer
let Color = (name: string) => { /* lookup */ };  // from a name
```

The three overloads are simply shadow declarations of the same name (§3.1 Shadowing and §4.1.9 backward lookup). Calling `Color` resolves to the appropriate overload based on the argument.

Constructors work identically everywhere a callable is valid: as standalone calls, as the target of a pipeline, or as a constructor annotation (§3.1.5). In each case the same two-phase overload resolution applies.

## 9 Lifetime & Memory Model

### 9.1 Ownership & Moves

### 9.2 Branching Rules

### 9.3 Loop Interaction

### 9.4 Destructor Ordering

### 9.5 Allocation Strategies (power-of-2, etc.)

Warble's runtime uses a power-of-two free-list allocator optimized for frequent, short-lived allocations.

The heap allocator is organized into:

* A **per-thread local free list**, used for the fast path.
* An **atomic global free list**, used to share memory between threads.
* An **OS allocation path**, used for very large allocations and for refilling the global pool.

This section describes the allocator's observable behavior and its internal invariants. Exact data structures may vary as long as the semantics below are preserved.

#### 9.5.1 Size classes

Warble heap blocks are pooled in **size classes** that are powers of two.

* The minimum pooled block size is **8 bytes**.
  * This ensures that a free block can store a `next` pointer in its first 8 bytes.
* Size class `i` corresponds to blocks of size:

  $\mathrm{class\_size}(i) = 2^{i + 3}$ bytes.

Blocks in size class `i` are aligned to at least $2^{i + 3}$ bytes (and at least 8 bytes in all cases). This invariant ensures that splitting a larger block into two equal halves produces two valid, correctly aligned blocks of the next smaller size class.

The runtime is configured with a compile-time constant maximum pooled class index `max_index`.

* The maximum pooled block size is $2^{max\_index + 3}$ bytes.
* Example: `max_index = 23` implies a maximum pooled block size of $2^{26}$ bytes (64 MiB).

If a requested allocation exceeds the maximum pooled block size, the allocator bypasses the free lists and delegates directly to the OS.

Pooled allocations return a block of size `class_size(index(n))`, which may be larger than the requested `n`.

#### 9.5.2 Converting byte counts to a class index

When allocating a buffer of `n` bytes from the pooled allocator, the runtime rounds up to the next power of two and converts to a class index.

Let:

* $s = \max(n, 8)$
* $p = 2^{\lceil \log_2(s) \rceil}$ (the next power of two, with $p \ge s$)

Then the class index is:

$$
\mathrm{index}(n) = \log_2(p) - 3
$$

Equivalently, for unsigned integers, an efficient implementation is:

* `index(n) = bit_width(max(n, 8) - 1) - 3`

Where `bit_width(x)` is $\lfloor \log_2(x) \rfloor + 1$ for $x > 0$.

#### 9.5.3 Free-list representation

The heap allocator does not maintain per-allocation headers.

* When the runtime hands out a block, it does not retain any record of that block's size or origin.
* Freeing is therefore **size-aware**: the caller must provide the correct byte count for the allocation being freed.
* Freeing with an incorrect size is undefined behavior.

This design keeps allocations headerless and allows blocks to freely migrate between threads (it is common for one thread to allocate and another to free).

Each free list (local or global) contains:

* An array of slot heads `slots[0..max_index]`, where each slot is a singly-linked list of free blocks of that size class.
  * A free block stores its `next` pointer in the first 8 bytes of the block.
* A **bitset** `present`, with one bit per slot.
  * If `present[i]` is set, the list **may** contain at least one block of class `i`.
  * For the local free list, this bitset is authoritative.
  * For the global free list, this bitset is a hint and may contain false positives.
* A `size_bytes` counter tracking memory ownership (see §9.5.6).

Global free-list slot heads and metadata are atomic.

#### 9.5.4 Allocation (local fast path)

To allocate `n` bytes:

1. If `n` exceeds the maximum pooled block size, allocate directly from the OS and return.
2. Compute `i = index(n)`.
3. If the local slot `i` is non-empty, pop its head and return it.
    * Accounting: `local.size_bytes -= class_size(i)`.
    * If the slot becomes empty, clear `local.present[i]`.
4. Otherwise, search for a larger available local class `j > i`.
   * The runtime should use the local `present` bitset to hop directly between candidate slots rather than linearly scanning every slot.
5. If a larger class `j` is found, pop a block from slot `j` and repeatedly **split it in half** until it reaches size class `i`:
    * Accounting: popping the class `j` block decrements `local.size_bytes` by `class_size(j)`; each half that is pushed back increments `local.size_bytes` by its class size.
    * Each split of a class `k` block produces two class `k-1` blocks.
    * One half is pushed onto local slot `k-1`.
    * The other half is split again (or returned once `k-1 == i`).

At the end of this process, the net effect is that exactly `class_size(i)` bytes leave the local free list and become owned by the allocation result.

This allocator is a buddy-style splitter. Coalescing is not required for correctness; it is optional runtime maintenance.

#### 9.5.5 Allocation (global refill path)

If the local free list cannot satisfy an allocation (no suitable `j >= i` is available locally), the thread attempts to refill from the global free list.

Global search:

* The thread computes `i = index(n)` and searches for a class `j >= i` that may contain blocks.
  * The global `present` bitset is used as a hint to skip empty ranges.
  * Because the bitset is not authoritative, the thread must still validate that `slots[j]` is non-null when attempting to claim.

Claiming a global chain:

* To refill from class `j`, the thread claims the entire global chain in `slots[j]` by compare-exchanging the head pointer to `null`.
* The claimed chain is transferred into a private thread-local staging list for class `j`.
  * Until the thread performs its local "checkout" decision, the claimed chain is still considered to belong to the global free list for accounting purposes (§9.5.6).

Partial "checkout" and republishing:

* After claiming a chain from global, the thread may keep only some number of blocks locally and republish the remainder back to global.
* The number of blocks the thread keeps is implementation-defined and may be guided by heuristics.

One reasonable strategy is:

* Iterate the claimed chain only up to the heuristic limit, counting how many blocks `k` will be kept locally.
* Detach the remaining suffix of the chain (if any) and republish that suffix back to the global slot `j`.
* Transfer ownership for the `k` kept blocks by adjusting `local.size_bytes` and `global.size_bytes` (§9.5.6).

After ownership transfer, the thread may treat the `k` kept blocks as part of its local free list (updating `local.present` accordingly) and allocate from them.

After refilling locally (and optionally republishing), the thread continues with the local allocation algorithm (§9.5.4), splitting blocks down to the requested class as needed.

If no suitable global chain can be claimed for any `j >= i`, the allocator must allocate from the OS (§9.5.7).

#### 9.5.6 Ownership and accounting

Each local free list and the global free list maintain a `size_bytes` counter.

* `local.size_bytes` is authoritative: it is the total number of bytes currently held by the local free list.
* `global.size_bytes` represents the total number of bytes that **belong to** the global free list.
  * This is an ownership accounting concept, not an availability guarantee.
  * Claiming a global chain does not immediately change `global.size_bytes`.
  * Ownership transfers only when the thread decides how many blocks it will keep locally (the "checkout" step).

When a thread keeps `k` blocks of class `j` from a claimed global chain:

* `local.size_bytes += k * class_size(j)`
* `global.size_bytes -= k * class_size(j)` (atomically)

When a thread publishes `k` blocks of class `j` from its local free list to the global free list (for example, as part of rebalancing):

* `local.size_bytes -= k * class_size(j)`
* `global.size_bytes += k * class_size(j)` (atomically)

Additionally, the runtime maintains a global atomic counter `os_bytes` tracking the total number of bytes currently allocated from the OS.

* Each OS allocation increments `os_bytes` by the allocated byte count.
* Each OS free decrements `os_bytes` by the freed byte count.

#### 9.5.7 OS allocation and seeding the pool

Large allocations:

* If an allocation request exceeds the maximum pooled block size, the allocator bypasses the free lists and allocates directly from the OS.
  * Unlike pooled allocations, OS allocations are not required to round up to a power of two; the runtime should allocate the requested size (subject to OS constraints such as page rounding).
* When such a block is freed, it is returned directly to the OS.

Seeding when global is empty:

* If a thread fails to find any suitable global chain, it allocates a block from the OS of size $2^{max\_index + 4}$ bytes (the next power of two beyond the maximum pooled size).
* It splits that OS block into two blocks of size $2^{max\_index + 3}$ bytes.
  * One block is added to the thread's local slot `max_index`.
  * The other block is published to the global slot `max_index`.

#### 9.5.8 Publishing to the global free list

Publishing a chain to the global list is done by compare-exchange.

* The thread first attempts to publish by CAS'ing `slots[j]` from `null` to `chain_head`.
* If the CAS fails, some other thread published first. In that case:
  * The thread claims the newly-published chain (by CAS'ing `slots[j]` to `null`),
  * links it onto its own chain (merging),
  * and retries the publish.

This retry-merge loop continues until the publish succeeds.

##### ABA safety

The claim/publish protocol is inherently free of the ABA problem.

The two CAS operations are asymmetric: claiming always transitions a slot from a non-null chain head to `null`, while publishing always transitions from `null` to a non-null chain head. This means:

* **Publish CAS** (`null → Y`): An ABA scenario would require the slot to cycle `null → X → null` between a thread's read and its CAS. If that happens, the publishing thread's CAS simply succeeds into a freshly-empty slot. Correctness is not affected.

* **Claim CAS** (`X → null`): An ABA scenario would require `X` to be claimed by another thread, then a new chain starting at address `X` to be published before the first thread's CAS fires. Even in this case, the first thread's CAS atomically captures the *entire* chain currently attached to `X`. There is no pre-CAS read of `X.next` that becomes stale, because the thread reads the chain from scratch (starting at `X`) only *after* the CAS succeeds. No blocks are silently lost.

The root reason the ABA problem cannot manifest injuriously here is that claim always takes whole chains: there is no partial-pop operation that leaves a stale `.next` pointer dangling.

#### 9.5.9 Bitset maintenance

The local `present` bitset is authoritative and must be updated whenever a local slot transitions between empty and non-empty.

The global `present` bitset is a performance hint.

* Setting a bit (indicating that a slot may be non-empty) should occur when publishing a chain to that slot.
* A thread that claims a global chain is not required to immediately clear the corresponding `present` bit.
  * Leaving the bit set may cause extra probing by other threads (a false positive), but it does not affect correctness.
* Clearing a bit is best-effort and should avoid creating false negatives.
  * The runtime should not clear a bit unless it has reason to believe the corresponding global slot head is still `null`.
  * If the global bitset contains false positives, correctness is unaffected; it only impacts performance.

#### 9.5.10 Freeing pooled blocks

Freeing is size-aware and headerless: the free operation must be provided the same byte count `n` that was used to allocate the block.

When freeing a heap block with size `n`:

1. If `n` exceeds the maximum pooled block size, free the block to the OS and decrement `os_bytes` by the freed byte count.
2. Otherwise, compute `i = index(n)`.
3. Push the block onto the freeing thread's local free list slot `i`:
   * Write the current slot head pointer into the first 8 bytes of the block.
   * Update the slot head to point at the freed block.
   * Set `local.present[i]`.
4. Update accounting:
   * `local.size_bytes += class_size(i)`

Because blocks may be freed by threads other than the allocating thread, implementations should include a rebalancing policy to prevent one thread from permanently hoarding a disproportionate fraction of the heap (§9.5.11).

#### 9.5.11 Rebalancing local vs global memory

Blocks may freely migrate between threads: a block allocated by one thread may be freed by another, and the free operation pushes it onto the freeing thread's local free list.

To prevent long-term imbalance, each worker thread should implement a rebalancing policy that publishes excess local blocks to the global free list.

Inputs available to a rebalancing heuristic include:

* `os_bytes`: total bytes allocated from the OS for the program.
* `worker_count`: the number of worker threads.
* `local.size_bytes`: bytes currently held by this thread's local free list.

Exact thresholds are implementation-defined. A reasonable approach is to compare `local.size_bytes` to an estimate of a fair share (for example, `os_bytes / worker_count`) and publish blocks to global until the local share falls under a chosen multiple.

#### 9.5.12 Coalescing (buddy merges) and OS release

In addition to rebalancing, workers may perform background heap maintenance when they are otherwise idle (for example, while waiting for barriers to clear; see §13.2.6).

Coalescing is an optional optimization that merges free buddy blocks into larger blocks.

Buddy relationship:

* Two blocks of size $S$ are buddies if they are adjacent, their combined region is aligned to $2S$, and together they form a block of size $2S$.
* One common implementation test is: for a block at address `p`, its buddy is at `p ^ S`.

Global coalescing workflow (one reasonable strategy):

1. Claim an entire chain from a global slot `j` (CAS head pointer to `null`).
2. Locally partition the claimed blocks into:
   * an "unmerged" chain of class `j` blocks that could not be paired with their buddies, and
   * a "merged" chain of class `j+1` blocks produced by successful buddy merges.
3. Republish the unmerged chain back to global slot `j`.
4. To continue coalescing upward, claim the global chain from slot `j+1`, merge it together with the locally-produced merged chain, and repeat the process.

This process may continue until maintenance work should stop or until `max_index` is reached.

At the maximum size class, implementations may choose to either:

* republish the resulting blocks back to the global slot `max_index`, or
* return some of them to the OS to reduce memory usage.

When returning pooled blocks that belong to the global free list to the OS, the runtime must decrement `os_bytes` and also decrement `global.size_bytes` by the freed byte count.

The aggressiveness of coalescing and OS release is implementation-defined and may be guided by system memory pressure signals.

## 10 Modules & Packages

Warble organizes code through a carefully designed system of **packages** and **modules**. This system emphasizes security, clarity, and explicit dependency management, ensuring both efficiency and trustworthiness in project management and third-party code integration.

### 10.1 Overview

* A **package** represents the root directory of a Warble project. External packages are explicitly registered via a `register` statement.
* A **module** corresponds to a single source file (`.wbl`) within a package, imported and exported explicitly to manage code reuse and visibility.
* The **standard library** is represented by the pre-bound keyword `compiler`, which is a symbol of kind `Package` implicitly available in every module without needing to be registered.

Both packages and modules prioritize security through clear permissions and explicit declarations of dependencies.

### 10.2 Packages

A **package** is explicitly registered through the `register` statement. The structure of a `register` statement is:

```
register name from "package_url" [with <symbols>];
```

The `register` statement creates a local binding of kind `Package` with the given identifier name. This identifier is then used in `import` statements to reference the package.

#### 10.2.1 Registering Packages

Packages are registered at the top level of the project. The registration includes:

* A **package name** as a plain identifier, creating a local binding of kind `Package`.
* A **URL** as a string literal, specifying the root directory location of the package. URLs can be:
  * Absolute (`file:///...`)
  * Relative (`../local/path`)
  * Remote (`https://...`)
* Optional **permissions** via a `with` clause, explicitly allowing the registered package to import specified modules from the standard library (see §10.2.2).

**Examples**:

```warble
import safe from "permissions/safe" in compiler;
import {vector, string, math} from "permissions/safe" in compiler;
import fs from "filesystem" in compiler;

register local_package from "../relative/path";
register absolute_package from "file:///C:/absolute/path" with <vector, string, math>;
register remote_package from "https://github.com/user/package" with <math>;
register extended_package from "https://github.com/user/extended_package" with <safe, fs>;
```

If the same package is registered multiple times, it is shared, not duplicated.

##### Exported registrations (`register export`)

A `register` statement may be followed by the `export` keyword. This makes the resulting `Package` binding available both as a local name within the current module and as an export visible to any module that imports the current module.

This is useful when a module acts as a central place to declare and expose third-party dependencies, so that importers of that aggregating module gain access to the registered package identifiers without having to register them again themselves.

```warble
// deps.wbl — centralizes all third-party dependency registrations for the project
import {string, vector, math} from "permissions/safe" in compiler;
import fs from "filesystem" in compiler;
import http from "http" in compiler;

register export serializer from "https://github.com/example/serializer" with <string, vector>;
register export data_store from "https://github.com/example/data_store" with <string, fs>;
register export web_client from "https://github.com/example/web_client" with <string, http>;
```

```warble
// app.wbl — imports deps.wbl and uses the re-exported package bindings directly
import {serializer, data_store, web_client} from "./deps";

import {serialize} from "encoding" in serializer;
import {connect} from "client" in web_client;
```

Only the package binding itself is re-exported, not the contents of the package. Any module that receives the exported binding must still issue its own `import` statements to access modules within that package.

#### 10.2.2 Permissions & Security Model

Warble adopts a security-first approach to package dependencies. Packages must be explicitly granted permission to access standard library modules. These permissions form an **allow list**, specifying precisely what the package can import from the standard library.

The `with` clause on a `register` statement specifies which standard library modules the registered package is allowed to access. The clause uses enum literal syntax (`<>`), and its items must be symbols of kind `Module` or `Borrow` pointing to an external module:

```warble
import safe from "permissions/safe" in compiler;
register pkg from "file:///path" with <safe>;
// pkg can import anything that safe re-exports
```

```warble
import {vector} from "permissions/safe" in compiler;
register pkg from "file:///path" with <vector>;
// pkg can only import vector, nothing else from safe
```

The permission grant follows a hierarchy:

* **Giving a `Module` symbol** (a whole-module import) grants the registered package access to all of that module's exports.
* **Giving a `Borrow` symbol** (a specific named export from a module) grants access to only that export and whatever *it* exports—not the entire module it came from.

The compiler always resolves symbols to their canonical module identity, regardless of how many times they have been imported or re-exported. Granting `vector` obtained through the `safe` permission aggregate is identical to granting `vector` obtained any other way—it always resolves to the same canonical module.

##### Scope of Permissions

The permission system is **only** concerned with what packages are allowed to import from the standard library (`compiler`). It does not govern what one third-party package can access from another third-party package.

The reasoning: the standard library is the only thing that connects Warble code to the outside world (filesystem, network, hardware, etc.). Two third-party packages communicating with each other cannot cause harm without the standard library being involved. Restricting inter-package access would add complexity without addressing any real security concern.

Inter-package access is still implicitly constrained by the propagation rule: a package cannot grant permissions it does not have. If package B wants to register package A, and package A requires `filesystem`, then B must itself have `filesystem` in its own grant—otherwise it cannot grant it to A and the registration is a compile-time error. This handles the inter-package case naturally without needing explicit syntax.

##### Permission Aggregates

The standard library provides permission aggregate modules that re-export groups of standard library modules. These are ordinary modules within the standard library:

* `"permissions/safe"`: A standard library module that re-exports all standard library modules considered safe (no filesystem, network, or other sensitive capabilities).
* `"permissions/all"`: A standard library module that re-exports the entire standard library.

These aggregate modules are accessed through normal imports:

```warble
import safe from "permissions/safe" in compiler;
import all from "permissions/all" in compiler;
```

They can then be used in `with` clauses to grant broad permission sets:

```warble
register trusted_pkg from "../trusted" with <all>;
register sandboxed_pkg from "../sandboxed" with <safe>;
```

To grant a permission aggregate plus additional specific modules, list them together:

```warble
import safe from "permissions/safe" in compiler;
import fs from "filesystem" in compiler;
register pkg from "file:///path" with <safe, fs>;
```

**Recommended Practice**:
Package authors clearly document required permissions. Users review and manually insert these into their `register` statements, making permission grants deliberate and transparent.

### 10.3 Modules

A **module** corresponds to a single Warble source file (`.wbl`) and is accessed via explicit `import` and `export` statements.

#### 10.3.1 Imports

Modules import code explicitly. The syntax for importing is:

```warble
import identifier from "specifier" [in package];
import {a, b as alias} from "specifier" [in package];

import deferred identifier from "specifier" [in package];
import deferred {a, b as alias} from "specifier" [in package];
```

* The **specifier** is a string literal resolved as a relative path from the package root directory.
* If no file extension is specified, `.wbl` is implicitly assumed.
* The optional `in` clause takes a package identifier (a binding of kind `Package`). If omitted, imports resolve within the current package context.

**Examples**:

```warble
import fs from "filesystem" in compiler;          // Imports from standard library
import {util, helper as h} from "./local_module"; // Local module imports
import vector from "vector" in compiler;           // Import a specific standard library module
```

Imports always produce immutable bindings, regardless of the original export mutability. Modules are shared: multiple imports of the same module access the same instance.

##### Exported imports (`import export`)

An `import` statement may be followed by the `export` keyword. This makes the imported binding both locally available within the current module and visible as an export to any module that imports the current module. In other words, `import export` is a single-statement way to simultaneously import and re-export.

This works with both whole-module imports and destructured imports:

```warble
// io.wbl — assembles a focused public I/O API by re-exporting from the standard library
import export fs from "filesystem" in compiler;
import export net from "network" in compiler;
// Consumers of io.wbl receive fs and net as Module bindings, exactly as if they had
// imported those modules from the standard library themselves.
```

```warble
// math.wbl — selectively re-exports individual symbols from the standard library math module
import export {sin, cos, tan, sqrt, floor, ceil} from "math" in compiler;
// Only the six named exports are made available. Other exports of the "math" module
// (e.g. log, exp) are not re-exported and remain inaccessible through this module.
```

```warble
// geometry.wbl — uses re-exported symbols locally and exposes results
import export {sin, cos, pi} from "math" in compiler;

export to_cartesian = (r: float, theta: float) {
  return {
    x = r * cos(theta),
    y = r * sin(theta),
  };
};

export tau = 2.0 * pi; // pi is also available locally for use within this module
```

With destructuring, only the specifically named bindings are exported — not the module they came from, and not any of its other exports. This makes `import export {a, b} from ...` a precise tool for building minimal, deliberate public APIs.

##### Deferred imports (`import deferred`)

`import deferred` is an explicit escape hatch that relaxes same-pass ordering between modules.

Semantics:

* Deferred imports may participate in cycles.
* Reads through a deferred import observe the dependency module's exported state from the **previous successful suspension point** (the previous committed module cycle), not necessarily the state produced in the current scheduling pass.
* Because this can observe "stale" values by design, deferred imports are intended for cyclic dataflow and feedback loops where a one-cycle delay is acceptable (or desired).

Initialization restriction:

* It is a compile-time error for a module to read any binding imported via `import deferred` before that module has successfully suspended at least once (i.e. before its first `tick` boundary that advances the module cycle counter).
* In practice, this means deferred imports are only used inside `tick` loops. The compiler must be able to prove the restriction.

#### 10.3.2 Exports

To expose functionality from a module, Warble uses explicit `export` declarations at the module's top-level scope:

```warble
export fn = (){};
export mut mutableValue = 42; // Only mutable internally, dependency modules cannot mutate
```

Only declarations explicitly marked with `export` become visible outside the module. The `export` keyword may also follow `import` and `register` statements to simultaneously create a local binding and export it (see §10.3.1 and §10.2.1).

#### 10.3.3 Pseudo-Modules

Not every module in the standard library maps to a real `.wbl` source file. The compiler may define **pseudo-modules** that expose internal compiler concepts through the normal module/import interface, without having a backing file on disk.

For example, the compiler-defined symbols `unresolved`, `detached`, and `exhausted` (used by promises and generators) are exports from pseudo-modules within the standard library. Similarly, primitive type symbols such as `integer`, `float`, `boolean`, and `character` are exports from the `"primitives"` pseudo-module:

```warble
import {integer, float, boolean, character} from "primitives" in compiler;
```

The access pattern is uniform (normal import syntax), even though the backing implementation is compiler-internal. From the user's perspective, pseudo-modules are indistinguishable from regular modules—they are imported the same way and their exports behave identically.

### 10.4 Dependency Graph & Build Process

Warble packages and modules form a clear, explicit dependency graph defined by their registration, imports, and permissions:

* The project's own root package is the **trust root** of the entire dependency graph. It is implicitly granted full access to the standard library without needing an explicit `with` clause.
* All other packages must be explicitly registered and granted permissions via `register` statements with `with` clauses.
* Each package registers other packages explicitly, clearly defining access and security permissions.
* Modules import other modules explicitly, creating a deterministic and transparent dependency graph.

The dependency graph determines build order and module initialization. Modules execute concurrently (details covered later), ensuring efficient execution without sacrificing safety or predictability.

### 10.5 Security-First Philosophy

The entire Warble package and module system is built around transparency, explicit permissions, and proactive security considerations. Unlike traditional package managers (e.g., Node.js), Warble intentionally avoids implicit trust in imported packages:

* Packages must be explicitly granted the standard library modules they are allowed to access. No implicit file-system, network, or sensitive access occurs without explicit consent.
* Developers can quickly identify packages requiring deeper security reviews (e.g., those granted `filesystem` or `http` modules).
* Packages granted only safe permissions (via the `"permissions/safe"` aggregate) require minimal scrutiny.

**Example Security Consideration**:

```warble
import fs from "filesystem" in compiler;
import http from "http" in compiler;
register server_pkg from "https://example.com/server" with <fs, http>;
```

Granting `filesystem` and `http` explicitly informs you of the package's elevated capabilities. This does not imply malicious intent—such a package might legitimately implement an HTTP server—but it clearly communicates where review and trust matter most.

Warble's approach ensures no hidden surprises, fostering a safer and more responsible ecosystem.

## 11 Compile-Time & Metaprogramming

### 11.1 Attributes

### 11.2 Conditional Compilation

### 11.3 Symbol Introspection APIs

## 12 Error Handling & Diagnostics

### 12.1 Compile-Time Errors

### 12.2 Runtime Traps

### 12.3 Assertion Facilities

### 12.4 Union-based Error Propagation (`expect`, `assume`)

Warble's primary structured error flow is an extension of the existing union system.

Any union may include both **pass** arms and **fail** arms. A union is **passing** when its current runtime tag is in the pass region, and **failing** when its current runtime tag is in the fail region.

The `expect` keyword is a unary prefix operator that propagates failure. Its operand must be a union:

* If its operand is passing, the result is the same union type with all fail arms removed.
* If its operand is failing, it performs an implicit `return fail` using the operand's current failing value, preserving all possible fail states.

Because `expect` may synthesize a `return fail`, it is only legal inside a function body. Using `expect` also forces the enclosing function to use explicit `pass`/`fail` markers for any other `return` or `yield` statements (mixing a plain `return` with a `return fail` is a compile-time error).

Example:

```warble
let parse_int = (text) {
  if (can_parse_int(text)) {
    return pass parse_int(text);
  }
  return fail "not an int";
};

let add_one = (text) {
  let v = expect parse_int(text);
  return pass (v + 1);
};
```

The `assume` keyword performs the same pass/fail test, but aborts instead of returning. Its operand must be a union:

* If its operand is passing, the result is the same union type with all fail arms removed.
* If its operand is failing, it performs `panic <fail-value>`.

Because `assume` does not produce a `return fail`, it does not affect the enclosing function's return type rules, and it may be used at module top-level.

At module top-level, `assume` is a convenient way to enforce initialization invariants:

```warble
let config = assume load_config();
```

## 13 Concurrency & Parallelism

Warble programs are inherently concurrent. A pool of worker threads executes modules in parallel, respecting the static module import graph and providing explicit suspension points (`tick`, `await`) that allow work to be interleaved efficiently.

Note: the `yield` statement is a feature of union / generator functions (§4.2.2, §7.5) and is unrelated to module scheduling.

### 13.1 Program Lifecycle

A Warble program begins execution on a single operating-system thread: the **main thread**. The main thread's first responsibility is to create and initialize the **thread pool**, spawning one worker thread per logical CPU core. Once all workers are active, the main thread's role as launcher is complete.

The main thread then enters a waiting state, waking periodically on a repeating interval (for example, once per second) to perform lightweight health monitoring. Its remaining responsibilities are to confirm that the pool is running smoothly, and to shut it down when it finishes—whether normally or due to an error.

#### 13.1.1 Normal Execution

Under normal operation, the main thread sleeps while the worker threads execute modules concurrently. When the pool finishes—all workers exhaust their work queues and stop—the pool wakes the main thread. The main thread tears down the pool (joining all worker threads and releasing pool resources) and returns, allowing the process to exit cleanly.

#### 13.1.2 Health Monitoring

On each monitoring wakeup, the main thread reads the pool's shared metadata to verify that progress is still being made. Workers publish lightweight diagnostics—their current worker cycle counter and a recent timestamp—that the main thread can inspect without acquiring locks. Using these values alongside the cycle history buffer (§13.2.9), the main thread checks whether the pool is advancing at a reasonable rate. Under normal operation this check is cheap and the main thread returns to sleep immediately.

#### 13.1.3 Stall Recovery

A **stall** occurs when workers are failing to make progress but have not detected this themselves. The no-progress backoff mechanism (§13.2.6) handles most stalls internally: when workers detect they are stuck, they stop and wake the main thread. If a stall goes undetected by the workers, the main thread's periodic monitoring wakeup provides the fallback.

When the main thread determines—via cycle history and timing heuristics (§13.2.9)—that the pool is in an unrecoverable stall, it:

1. Sets the pool's stop flag to signal all workers to halt.
2. Waits for all workers to exit.
3. Collects debug information (cycle counters, timestamps, scheduling state).
4. Tears down the pool and returns with an error status.

#### 13.1.4 Panic Handling

When a worker encounters a fatal runtime error, it immediately transfers control to a small **panic stub**. The stub performs exactly the following actions:

1. Updates the worker's metadata to record that a panic occurred and document relevant state for debugging.
2. Atomically sets the pool's stop flag.
3. Exits the worker thread.

The panic stub deliberately performs **no stack allocation**. This constraint is critical: a panic may originate from a stack overflow, in which case further stack use would be unsafe or impossible. All state written by the stub resides in the worker's pre-allocated metadata region.

When the main thread wakes—either because other workers stopped or because the monitoring interval fired—it observes the panic flag in the pool metadata. It then collects debug information from the panicked worker's metadata, tears down the pool, and returns with an error status.

### 13.2 Module Scheduling

This section describes the runtime model used to execute **modules** concurrently.

#### 13.2.1 Modules as Singleton Tasks

At runtime, each module is a singleton instance:

* The module has persistent storage for its module-level bindings and internal bookkeeping.
* The module is compiled as a function with a fixed **entry point**.
* The module also stores a **resume point** in its static data.

Module entry points are runtime-owned (users do not call them directly). Conceptually, every module has the signature:

* `bool module_entry(u64 target_cycle)`

Where `target_cycle` is the scheduling anchor used for eligibility checks, and the return value indicates whether the module should be retried again in the current scheduling pass (§13.2.6).

Execution model:

* The runtime always calls the same function entry point when attempting to run a module.
* The entry point performs a fast eligibility check and then attempts to claim the module.
* After a successful claim, control transfers (via an internal jump) to the module's current resume point.

Initially, the resume point is the location immediately after this jump, so the first successful claim starts execution at the top of the module.

#### 13.2.2 Dependency Lists (Imports)

Each module instance carries a dependency list of pointers to other module instances.

* A module depends on another module exactly when it imports it via a normal `import`.
* The normal import graph must be acyclic (a DAG) and is known at compile time.
* Using `import deferred` is an explicit escape hatch that relaxes same-pass ordering and may participate in cycles (§10.3.1, §13.2.4).

Additionally, the runtime and compiler conceptually also operate on the reverse relationship:

* A module has a **dependent** when that other module imports it.

Dependents are used by the compiler to build write barriers for exported state (§13.2.4).

#### 13.2.3 Module Cycle Counters (Half-Cycles)

Each module maintains an atomic unsigned 64-bit **cycle counter**.

* The counter is advanced by **one** when a worker thread successfully claims the module for execution.
* The counter is advanced by **one** again when the worker thread finishes its current run of the module (either by suspending at a `tick` boundary, or by reaching module completion).

This means each full "cycle" of module execution advances the counter by 2; the intermediate value is a **half-cycle**.

Interpretation:

* **Even** cycle count: the module is idle (not currently owned by any worker thread).
* **Odd** cycle count: the module is in progress (currently owned by exactly one worker thread).

The runtime stores cycle counters in `u64` and assumes they do not overflow during a program run.

A module may also temporarily *abort* a run attempt after claiming (for example, because it hit a compiler-inserted barrier). In this case the module must undo the claim by decrementing its cycle counter back to the prior even value before returning to the scheduler (§13.2.6).

##### Worker Cycle Counter (Core)

Each worker thread maintains an atomic, publicly observable unsigned 64-bit **worker cycle counter**.

* The worker advances its cycle counter by one when it begins a scheduling pass.
* The worker advances its cycle counter by one again when it completes that pass (when it swaps its buffers; see §13.2.6).

This makes the worker's cycle counter a half-cycle counter in the same sense as module cycle counters.

The worker cycle counter is a core part of the scheduling design: it provides an anchor value that prevents a worker from accidentally running the same module multiple times within a single scheduling pass.

#### 13.2.4 Eligibility and Barriers

This scheduler model uses **optimistic execution with compiler-inserted barriers**.

At the start of a module entry call, the entry point performs a fast eligibility check based on the `target_cycle` parameter and the module's cycle counter `cm`:

* If `cm >= target_cycle`, the module is considered already handled for this scheduling pass and returns `false`.
* If `cm` is odd, the module is currently in progress on some worker and returns `false`.

Otherwise, `cm` is even and `cm < target_cycle`, so the entry point attempts to claim the module (§13.2.5). If the claim succeeds, the module begins executing user code at its resume point.

Instead of attempting to fully establish dependency readiness up front, the compiler inserts read and write barriers directly into module code to prevent data races and to enforce safe ordering close to where data is actually accessed. A minimum of one read barrier is always emitted per import, even when no bindings are read (see below).

##### Read barriers (imports)

For each read of an imported binding (an **external borrow**), the compiler inserts a barrier that suspends the current module run attempt if the dependency module is not safe to read.

Conceptually, before an imported binding like `A.value` is read in module `B`, the compiler emits logic equivalent to:

* If `A.cycle` is odd (in progress), suspend the current run attempt.
* If `A.cycle <= target_cycle`, the dependency has not completed this scheduling pass. In this case:
  * `B` must suspend its current run attempt (it cannot proceed with the read).
  * If `A.cycle` is **even**, `B` should attempt to run `A` once for the same `target_cycle` *before* returning to the scheduler.

To avoid deadlocking write barriers, `B` must release its claim before attempting to run `A`:

* `B` updates its resume point to the barrier site (or an equivalent internal block).
* `B` undoes its claim (odd → prior even) by decrementing its cycle counter by 1.
* The worker then attempts to call `A`'s entry point once with the same `target_cycle`.
* `B` then returns `true` (blocked) so it will be retried later in the current scheduling pass.

The second check enforces ordering: while module `M` is running for `target_cycle`, it may only read an imported binding from a dependency module that has already **completed** this scheduling pass. Since successful completion advances a module cycle counter by 2, this is exactly the condition `dependency_cycle > target_cycle` (and even).

##### Guaranteed barrier per import

The compiler must ensure that every normal `import` produces **at least one read barrier** against the imported module somewhere in the importing module's code, regardless of whether any binding from that module is ever actually read.

If a module imports another module purely for side effects and never reads any of its bindings, the compiler must insert a synthetic read barrier against that dependency. This barrier is logically equivalent to a read of a non-existent binding — it checks the dependency's cycle counter and suspends if the dependency has not yet completed the current scheduling pass, but it does not access any data.

**Rationale:** Without this guarantee, an import with no reads creates no ordering constraint at runtime. The two modules would be free to cycle independently, violating the semantic contract of `import`: that the dependency has run at least once before the importer runs.

The barrier must cover every import individually. It is not sufficient for the importing module to happen to be constrained *transitively* through a different import that does have reads. Transitive constraints do not compose in the right direction to guarantee per-edge ordering.

In practice, the compiler can check this trivially at compile time: for each import edge, if no read barrier was emitted against that module anywhere in the compilation of the module body, emit one synthetic barrier at the start of the module's entry point, before any user code.

##### Deferred read barriers (`import deferred`)

Reads through `import deferred` do not require the dependency to be ahead of the current module's `target_cycle`.

Instead, a deferred import reads the dependency's exports from the **previous committed cycle** (the previous successful suspension point). This makes cyclic dataflow possible without deadlocking on same-pass ordering.

Barrier requirements for deferred reads:

* A deferred read is a compile-time error before the importing module's first successful suspension point (§10.3.1).
* At runtime, a deferred read must not observe uninitialized dependency state, and it must read from the dependency's committed snapshot for the **previous** scheduling pass (one full module cycle behind the current pass).
  * Therefore, after the usual "in progress" check (`A.cycle` odd), the deferred read barrier must suspend unless `A.cycle + 2 > target_cycle`.
    * This is equivalent to checking `A.cycle > target_cycle - 2` ("completed at least the previous scheduling pass"), but expressed without unsigned underflow.
  * If the barrier blocks and `A.cycle` is even, the importing module may attempt to run `A` once for the same `target_cycle` before returning (as with strict imports).

The exact implementation strategy is runtime-defined. One common strategy is to publish exported state at suspension points and retain at least one previous published version so deferred reads can always observe the prior committed state.

After the barrier succeeds, the read may proceed.

The compiler may avoid repeating checks along a single control path once the dependency has already been observed safe to read.

##### Write barriers (mutable exports)

For each write to a mutable exported binding in module `A`, the compiler inserts a barrier that suspends the current module run attempt if any dependent module that may read that binding is currently in progress.

If module `B` contains a read of `A.value`, then before a write to `value` inside `A`, the compiler emits logic equivalent to:

* If `B.cycle` is odd (in progress), suspend the current run attempt.

If multiple dependents may read the binding, the check is the disjunction of those dependents.

The compiler may omit dependents that never read the binding, and may avoid repeating checks along a single control path once a dependent has already been observed not in progress.

If a dependent reads a binding only through `import deferred`, the compiler may omit that dependent from the write barrier for that binding, because deferred reads observe the prior committed state.

##### External borrows must remain analyzable

These barriers rely on the compiler being able to observe and reason about every read of an imported binding.

Warble therefore requires that external borrows remain transparent and must not be made opaque to the compiler (§4.2.1).

#### 13.2.5 Claiming a Module

To claim a runnable module `m`, the worker thread performs an atomic compare-exchange:

* Expected value: `cm` (even)
* Desired value: `cm + 1` (odd)

If the compare-exchange succeeds, the worker exclusively owns the module until it advances the cycle counter again.

If it fails, some other worker either claimed the module or advanced it; the worker must abandon this attempt and look for other work.

#### 13.2.6 Suspension and Rescheduling (`tick`)

The primary source-level suspension mechanism for modules is the `tick` loop (§7.4.4).

Warble has no general-purpose keyword to manually suspend module execution. Module suspension points arise only from `tick` loop boundaries and compiler-inserted barriers.

At the implicit suspension point at the end of each `tick` iteration, the module either schedules itself to run again later or (if it is lagging) immediately begins another cycle to catch up:

* The module updates its resume point to the location where execution should continue (typically the `tick` loop's iteration check).
* The worker then decides whether to release the module or immediately run another cycle to catch up.

Let `c` be the module's current cycle counter value at the suspension point. At this moment the module is owned by the worker and `c` is odd (the half-cycle acquired by the successful claim).

Catch-up rule (to prevent falling behind a worker's target cycle):

* If releasing the module would still leave it behind the current `target_cycle` (i.e. if $c + 1 < target\_cycle$), then the worker must not release the module yet. Instead it:
  * advances the module cycle counter by 2 (`c` → `c + 2`), keeping it odd and still owned,
  * immediately transfers control to the resume point it just selected.

This represents "finish this run" (+1) and "claim the next run immediately" (+1) without ever transitioning the module to an idle (even) state.

If the module remains behind `target_cycle` after beginning the next cycle, this catch-up behavior may repeat, allowing the module to run multiple cycles back-to-back while it remains owned by the same worker.

Otherwise (the module is caught up for this `target_cycle`), the normal suspension behavior occurs:

* The worker enqueues the module's (fixed) entry point into a thread-local **inactive** buffer.
* The worker advances the module cycle counter by 1 (`c` → `c + 1`) to mark the module idle again and returns to the scheduler.

**Implementation note:** The cycle advancement at the end of a run (whether `+1` for release or `+2` for catch-up) does not require a compare-exchange. While the cycle counter is odd, the module is exclusively owned by one worker, so this step can be performed as an atomic store of the previously observed value plus the increment (for example `cycle.store(c + 1)` or `cycle.store(c + 2)`).

In practice, this catch-up behavior is only expected to matter for top-level module execution where `target_cycle` is derived from `worker_cycle`.

##### Barrier-triggered suspension (blocked run attempt)

In addition to `tick` suspension, module code may suspend due to a compiler-inserted read or write barrier (§13.2.4).

When a barrier suspends a run attempt, the module is considered **blocked** and it must not advance its cycle:

* The module updates its resume point to the barrier site (or an equivalent internal block) so that the barrier is re-evaluated on the next attempt.
* The module undoes its claim by decrementing its cycle counter by 1 (odd → prior even).
* The module returns `true` to request a retry later in the current scheduling pass.

This rollback rule ensures that a barrier collision does not incorrectly "consume" the current scheduling pass.

Each worker thread maintains two private buffers of pending module entry points.

These buffers store module entry points (conceptually, functions of the form `u64 -> bool`).

* **Active** buffer: entry points to run in the current scheduler pass.
* **Inactive** buffer: entry points scheduled for a later pass (including modules that suspended at a `tick` boundary).

Each worker also maintains an index into the active buffer.

At the beginning of each scheduling pass, the worker also records the active buffer's initial length `start_length`.

* To get work, the worker reads the function pointer at `active[index]`, increments `index`, and calls it with `target_cycle = worker_cycle`.
* When `index` reaches `active.length`, the worker has no more work in the active buffer. It then:
  * sets `active.length = 0` (clearing without destructing),
  * sets `index = 0`,
  * swaps the active and inactive buffers by swapping their backing pointers (no copying),
  * advances the worker cycle counter by one to mark completion of the scheduling pass.

##### Return Value and Same-Pass Retry

The module entry point returns a boolean that represents whether the attempt was **blocked**.

* `true` means: **blocked**. The module began a run attempt but suspended at a compiler-inserted barrier (§13.2.4), rolled back its claim, and should be retried later in the same scheduling pass.
* `false` means: **not blocked**. Do not requeue it for retry in this scheduling pass. This includes cases where the module successfully ran (or suspended and scheduled itself via `tick`), chose not to run because it was already handled for this `target_cycle`, observed that it is already in progress, or observed that it is complete.

When a worker calls a module entry point from its active buffer:

* If the call returns `true`, the worker arranges for that same module entry point to be reconsidered later in the same scheduling pass. The exact mechanism depends on which phase the worker is in (§13.2.6).

This same-pass retry rule applies to top-level scheduling calls anchored to the worker cycle. Module code does not recursively schedule dependencies in general; the only dependency-running behavior is the read-barrier attempt described in §13.2.4.

##### Progress Detection and Backoff

Same-pass retry by requeueing into the active buffer is an optimization that can become pathological when a worker has exhausted all other work: if the only runnable candidate(s) repeatedly hit barriers and yield, a naive loop would continuously re-attempt the same blocked modules, rapidly appending duplicates and wasting CPU.

To prevent this, each worker must implement a backoff strategy that detects "no progress" while processing rescheduled work and avoids tight spinning.

Definitions:

* `start_length` is the active buffer length recorded at the beginning of the scheduling pass.
* When `index` first reaches `start_length`, the worker has exhausted everything that was in `active` at pass start; any remaining work in `active[index..]` exists only because it was rescheduled during this pass.

Two-phase scheduling pass:

* **Initial phase** (`index < start_length`): the worker runs the modules that were present at pass start.
  * If a call returns `true` (blocked), the worker pushes that module entry point to the end of the **active** buffer, so it will be retried later in the same pass.
* **Rescheduled phase** (`index >= start_length`): the worker is iterating only over modules that were rescheduled during this pass.
  * In this phase, the worker must not repeatedly append the same blocked modules to `active`.
  * Instead, the worker treats the tail of `active` as a set (order is irrelevant) and performs in-place filtering.

Rescheduled-phase filtering (one reasonable strategy):

* The worker processes the rescheduled region using a swap-pop strategy:
  * Let `rescheduled_begin = start_length`.
  * While `index < active.length`:
    * Call `active[index]`.
    * If it returns `true` (blocked), leave it in place and advance `index`.
    * If it returns `false` (not blocked), remove it from the rescheduled region by swapping it with `active.last` and popping the last element. (Do not advance `index` in this case, since a new element was swapped into `active[index]`.)

This filtering is correct because module correctness does not depend on active-buffer order.

No-progress detection:

* If the worker completes a full sweep of the rescheduled region (the loop terminates) without removing any entries, then every rescheduled attempt remained blocked and the worker has made no progress.

Backoff requirements:

* Upon detecting no progress, the worker must avoid repeatedly re-attempting the same blocked module(s) in a tight loop.
* The worker should perform one or more **fallback actions** before continuing, such as:
  * attempting to find other work by scanning the global module table (implementation-defined) and attempting to run eligible modules using the current `worker_cycle` as the `target_cycle`,
  * performing runtime maintenance work (for example, heap free-list coalescing / defragmentation heuristics),
  * yielding or sleeping for a short duration, ideally with an exponential (or otherwise increasing) backoff that resets once progress is observed.

After the backoff action(s), the worker should retry the rescheduled region by resetting `index = start_length`.

Additionally, while in a no-progress backoff state, the worker may change the destination of blocked reschedules:

* Instead of appending blocked modules to the active buffer during the initial phase, it may enqueue them into the inactive buffer so they are retried only after the next buffer swap / worker-cycle advance.

The exact heuristics (how often to scan globals, how long to yield/sleep, and how to prioritize maintenance) are implementation-defined, but the runtime must ensure blocked modules cannot force unbounded tight spinning.

To reduce contention at startup, the runtime may partition the initial module set across workers (for example, by giving each worker a contiguous range of module indexes to seed its active buffer). The exact distribution strategy is implementation-defined.

#### 13.2.7 Module Completion

When a module reaches the end of its top-level code (falls off the end of the file), it is complete and is never scheduled again.

Warble represents completion using a distinguished cycle counter value and a special resume block:

* The module cycle counter is set to the maximum *even* `u64` value.
* The module resume point is set to a small epilogue block that **decrements** the module cycle counter (undoing a claim) and returns.

This makes module completion idempotent and safe:

* Eligibility checks treat the module as already handled because its cycle count is far ahead and *even*.
* If a worker erroneously claims a completed module, its compare-exchange makes the cycle counter *odd* (to the maximum `u64`). The resume-point epilogue immediately decrements it back to the maximum *even* value and returns without rescheduling.

#### 13.2.8 Deadlock and Livelock Freedom

Given the static guarantee that the normal import graph is a DAG (no true circular dependencies), the scheduling system is provably free of both deadlock and livelock.

##### Structural setup

Assign each module a **topological depth** in the import DAG: leaf modules (no dependencies) have depth 0; a module's depth is one greater than the maximum depth of its dependencies. Because the graph is acyclic, this depth is well-defined and finite for every module.

The two ways a module can block are:

* **Read barrier** — Module B (depth D) is waiting for dependency A (depth < D) to complete. Before blocking, B releases its claim (B becomes even) and the worker tries to run A.
* **Write barrier** — Module A (depth D) is waiting for dependent B (depth > D) to stop being in-progress. Before blocking, A releases its claim (A becomes even).

In both cases the key invariant holds: **a blocked module is always even — it is never in-progress while waiting**.

##### Deadlock freedom

Deadlock requires a cycle: M₁ waits for M₂, M₂ waits for M₃, …, Mₖ waits for M₁, with every module stuck.

* A read barrier on Mᵢ → Mⱼ requires Mⱼ to be a **dependency** of Mᵢ (strictly lower depth).
* A write barrier on Mᵢ → Mⱼ requires Mⱼ to be a **dependent** of Mᵢ (strictly higher depth).

Any cycle in the wait graph would have to include at least one edge going up (write barrier) and at least one edge going down (read barrier). Consider the critical crossing: A (depth D) is blocked by a write barrier because B (depth > D) is odd (in-progress). For this to be part of a deadlock cycle, B must simultaneously be waiting for A in some way — but if B is waiting for A, B hit a read barrier and already released its claim, making B **even**. An even B does not trigger A's write barrier. The conflict is self-resolving: the act of B releasing its claim to pursue its read barrier simultaneously unblocks A's write barrier.

More formally: for a deadlock cycle to exist, some module in the cycle would have to be simultaneously **odd** (blocking another's write barrier) and **even** (waiting at a barrier of its own). That is a contradiction.

##### Livelock freedom

Livelock would require modules to cycle through claim and release indefinitely without any module completing. The argument proceeds by induction on topological depth.

**Base case (depth 0):** A depth-0 module has no dependencies, so it has no read barriers. It runs to completion unconditionally. No module can hold a blocking write barrier against it for more than a bounded number of retries, because its dependents (depth 1+) can also always release their claims.

**Inductive step (depth D):** Assume every module at depth < D eventually completes within the current scheduling pass.

A module A at depth D has read barriers only on depth < D dependencies, all of which complete by the inductive hypothesis. So A's read barriers pass in finite time.

A may have write barriers on dependents (depth > D). That dependent B is initially in-progress (odd). Within a finite number of executed instructions, B either:

1. **Completes** — B's cycle advances to the completion sentinel. A's write barrier passes permanently on retry.
2. **Hits its own barriers and releases** — B becomes even (not in-progress). This immediately unblocks A's write barrier. A resumes, and its read barriers on depth < D modules have already passed (by hypothesis), so A completes. Once A is complete, B's read barrier on A passes, and B can complete.

In either case, A completes in finite time. By induction, all modules at all depths complete.

##### Role of deferred imports

`import deferred` is exempt from same-cycle ordering: it reads the dependency's previous committed cycle and is therefore never part of the same-cycle barrier graph. Deferred reads cannot form a wait cycle with normal read barriers, and the circular graph structure they allow is resolved across cycles rather than within a single scheduling pass. This means the presence of deferred imports does not invalidate either argument above.

#### 13.2.9 Cycle History and Pool Health Monitoring

The runtime maintains a global **cycle history buffer**: a fixed-size circular buffer of timestamps, one entry per completed scheduling pass.

At the start of each scheduling pass, each worker compares its own worker cycle counter to a global atomic **fastest cycle counter**. If the worker is ahead, it attempts to advance the fastest cycle counter to match (a single CAS; most threads will not attempt this). The thread that successfully advances the fastest cycle counter is responsible for recording the current timestamp into the next slot of the cycle history buffer.

This keeps the write rate to the buffer at exactly one entry per cycle, originating only from the leading thread, with no contention on the buffer itself.

The buffer provides a rolling history of how long each cycle took. Because all entries are written by a single thread at a time, no synchronization is required on the buffer beyond the CAS that determines the writer.

**Uses of the cycle history:**

* **Stall detection.** If all workers are simultaneously in the no-progress backoff state (§13.2.6) and the elapsed time since the last recorded cycle greatly exceeds historical norms, the pool can declare itself stalled. The cycle history enables a relative threshold rather than a fixed one: a program that has been averaging 3 seconds per cycle should not be declared stalled at 5 seconds, but a program averaging 10ms should. A practical approach is to compute the mean and spread of recent cycle durations and trigger only when the current gap is a significant outlier (e.g., exceeds `mean + k * spread` for some multiplier `k`).

* **Cold-start fallback.** Until the buffer contains enough entries to compute a meaningful spread, stall detection falls back to a conservative fixed timeout.

* **Throttle calibration.** A module that wants to limit its own execution rate (§13.2.6) can read recent cycle durations to make informed decisions — for example, skipping a cycle if it ran less than N milliseconds ago.

* **Runtime diagnostics and profiling.** The buffer provides a lightweight built-in history of scheduling throughput, useful for developer tooling, performance dashboards, and detecting gradual slowdowns over time.

The buffer length is implementation-defined. A length sufficient to cover several seconds of history at typical cycle rates is recommended.

### 13.3 Atomic Operations

The standard library provides atomic counters and flags. They offer lock‑free read, write and compare‑exchange operations with explicit memory ordering.

### 13.4 Async Tasks (Futures)

In addition to module execution, worker threads also execute **async tasks** created by async function calls.

#### 13.4.1 Async function calls

Calling an async function:

1. Heap-allocates an **async frame** large enough to hold the function's state (promise union, parameters, locals, and internal bookkeeping).
2. Initializes the promise tag in the frame to `unresolved`.
3. Returns a `future` handle pointing to the promise union at the beginning of the frame (§4.2.4).
4. Enqueues an async task entry into the calling worker's async-task queue.

The async task entry consists of:

* a function pointer (the async function's entry point), and
* a context pointer (the async frame pointer).

#### 13.4.2 Per-thread async-task queues

Each worker thread maintains private async-task queues distinct from the module entry-point queues described in §13.2.

Unlike module scheduling, async task scheduling requires both a code pointer and a context pointer, so each queue element is a pair `(fn_ptr, ctx_ptr)`.

To prevent pathological unbounded growth during a single scheduling pass (an async task enqueues another async task which enqueues another, etc.), workers may use the same active/inactive buffering strategy as module scheduling, but specialized for task pairs.

Scheduling point:

* At the beginning of each worker scheduling pass (before processing the module active buffer), the worker should process its async-task active queue.

Exact ordering between module work and async-task work is implementation-defined, but the runtime must ensure that async tasks are eventually executed (they must not starve behind module work forever).

#### 13.4.3 Completion and detachment

When an async task reaches completion:

* It writes its result payload into the promise union's value slot in its async frame.
* It atomically compare-exchanges the promise tag from `unresolved` to the correct tag representing the completed result arm.

If the compare-exchange succeeds, the result is now observable via `await`.

If the compare-exchange fails because the tag is `detached`, the future was dropped while the task was still unresolved. In this case the task must destroy the async frame (run any required cleanup and free the allocation).

#### 13.4.4 Awaiting

The `await` operator is defined in terms of the **promise union**: it waits until the promise tag transitions away from `unresolved`.

A `future` is a transparent handle to a promise union stored at the beginning of an async frame (§4.2.4). Using `await` on a future forwards to `await` on the underlying promise union.

Once complete, `await` destroys the async frame and returns the completed result as a union value with `unresolved` and `detached` removed.

## 14 Standard Library (overview)

## 15 Tooling & Ecosystem

### 15.1 Compiler Flags

### 15.2 Package Manager

### 15.3 IDE Integration

## 16 Best Practices & Idioms

## 17 Glossary

## 18 Appendices

### 18.1 Operator Precedence

> TODO: Include a small table or paragraph cross-referencing the earlier operator precedence definition.

See §5.2 for operator descriptions; a dedicated precedence table is TODO.

### 18.2 Semantics Cross-References

For detailed semantics and behavior of specific constructs, see relevant chapters:

* **Declarations & Scope**: §3
* **Types & Signatures**: §4
* **Expressions & Operators**: §5
* **Functions & Calling Convention**: §6
* **Statements & Control Flow**: §7
* **Objects & Composition**: §8
* **Unions**: §4.2.2
* **Intersections**: §4.2.3
* **Conditional Matching**: §7.3.2
* **Lifetime & Memory Model**: §9
* **Modules & Packages**: §10
* **Compile-Time & Metaprogramming**: §11
* **Error Handling & Diagnostics**: §12
* **Concurrency & Parallelism**: §13

### 18.3 Reserved Words

This appendix lists all reserved keywords in Warble. A reserved keyword is a token that is explicitly recognized by the Warble lexer and parser and cannot be redefined or used as an identifier name within the language.

Warble emphasizes minimalism; most language functionality is implemented as identifiers bound to built-in library symbols rather than reserved keywords. However, the following words form the minimal set of fundamental keywords required by the language syntax, and as such, they are strictly reserved.

#### Declaration and Binding

* **`let`** — Begins a declaration statement, introducing a new binding.

#### Declaration Modifiers

* **`mut`** — Declares a binding as mutable.
* **`const`** — Declares a binding as constant.
* **`private`** — Declares a binding or property as private.
* **`protected`** — Declares a binding or property as protected.
* **`public`** — Declares a binding or property as public.
* **`export`** — Declares a binding as exported from its module. May also follow `import` and `register` statements to simultaneously create a local binding and re-export it.

#### Control Flow

* **`if`** — Begins a conditional block.
* **`try`** — Begins a type-validity branch (compile-time `true`/`false` conditional).
* **`else`** — Provides an alternative branch in a conditional statement.
* **`for`** — Begins a looping construct over an iterable.
* **`while`** — Begins a loop construct based on a boolean condition.
* **`do`** — Introduces a scoped block.
* **`repeat`** — Begins a loop, which can optionally end with a `while (condition)` portion.
* **`tick`** — Begins a module-context tick loop (only legal outside function bodies). Each iteration ends with an implicit suspension point.
* **`await`** — Unary prefix operator that suspends the current thread while a promise is `unresolved`, repeatedly returning to the work queue between checks. Futures transparently forward `await` to their underlying promise.
* **`expect`** — Unary prefix operator that removes a union's fail arms when passing, or propagates failure via an implicit `return fail`.
* **`assume`** — Unary prefix operator that removes a union's fail arms when passing, or aborts via `panic` when failing.

#### Conditional Matching and Constraints

* **`is`** — Indicates a match clause in conditional matching.
* **`from`** — Indicates a provenance match clause (and is also used in imports).
* **`has`** — Indicates a structural constraint match clause.
* **`as`** — Introduces aliases in object destructuring.

#### Jump Statements

* **`return`** — Returns a value from a function.
* **`yield`** — Produces a value from a generator function and suspends it.
* **`panic`** — Produces an error message and exits the program.
* **`break`** — Exits the nearest enclosing loop.
* **`continue`** — Skips to the next iteration of the nearest enclosing loop.
* **`async`** — May augment `return` and `yield` to define an async function. Async functions execute as scheduled tasks and produce a promise union that includes `unresolved` and `detached` as fail arms; calling an async function returns a future handle.
* **`pass`** — May augment `return` and `yield` to mark an arm as passing.
* **`fail`** — May augment `return` and `yield` to mark an arm as failing.

#### Module and Imports

* **`import`** — Imports symbols from external modules. May be followed by `export` to simultaneously import and re-export the binding.
* **`register`** — Registers a URL as a package, creating a local binding of kind `Package` used as the base for imports. May be followed by `export` to simultaneously register and re-export the package binding.
* **`with`** — Used in conjunction with `register` statements to specify allowed standard library modules for a package.
* **`in`** — Used in `import` statements to specify the package to import from. Also used in `for` loops.

#### Literals

* **`undefined`** — Keyword literal that evaluates to a compiler-defined marker symbol used to mean "not provided / not set / not present".
* **`null`** — Keyword literal that evaluates to a compiler-defined marker symbol used as an absence / empty / failing state label.
* **`readonly`** — Keyword literal that evaluates to a compiler-defined marker symbol used as an immutable-interface policy contract for libraries.
* **`true`** — Boolean literal for logical truth.
* **`false`** — Boolean literal for logical falsehood.

#### Special Compiler Reserved

* **`this`** — Resolves to the top topic symbol.
* **`that`** — Resolves to the second-to-top topic symbol.
* **`compiler`** — Pre-bound keyword symbol of kind `Package` that represents the standard library. Implicitly available in every module without needing to be registered. Used with `in compiler` in import statements to access standard library modules.
* **`auto`** — Serves as a placeholder type, replaced by the compiler with an inferred or specified type during type checking.

---

#### Important Notes:

* The modifier keywords (`mut`, `const`, `private`, `protected`, `public`, `export`) are reserved and participate directly in declaration syntax.

This reserved keyword set is deliberately minimal and focused on structural, syntactic, and foundational compiler-level operations, preserving maximum flexibility for user-defined functionality within Warble.