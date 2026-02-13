## 0 Preface

## 1 Introduction

Warble is a modern, expressive programming language designed to combine performance, simplicity, and elegance. Drawing inspiration from a diverse set of languages—including C++, Rust, and JavaScript—Warble prioritizes intuitive syntax, powerful type inference, and efficient, predictable performance. It seeks to offer programmers a language that is both easy to learn and capable of scaling to complex, high-performance applications.

At its core, Warble emphasizes clear semantics, strong type safety, and thoughtful defaults, enabling developers to write concise, readable, and maintainable code without compromising speed or flexibility. Built around innovative features like flexible unions, powerful structured literals, and seamless compile-time optimizations, Warble aims to streamline software development across a wide variety of domains—from systems programming to web development and beyond.

In this specification, you'll find comprehensive explanations and practical examples designed to clearly communicate the design decisions, syntax, and semantics that define Warble. Whether you're new to Warble or an experienced programmer seeking deep insight into its mechanics, this document will serve as your definitive guide.

### 1.1 Five-Minute Tour

Welcome to Warble! This quick tour provides an overview of Warble’s essential features and syntax, giving you a feel for what it’s like to code with the language.

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
let optionalValue = 42 || null; // Create a union of integer or void

optionalValue = null;   // now represents no value
optionalValue = 7;      // set to integer

if (optionalValue)
is (compiler.integer) { print("Got an integer: {this}"); }
is (compiler.void) { print("No value available."); }
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

That's just the beginning! Warble offers many more advanced features and constructs designed to make programming delightful, productive, and performant. Dive deeper into this specification to learn all the details about Warble’s design and capabilities.

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

Warble’s lexical structure defines how source code is broken down into meaningful sequences of characters, known as tokens. These tokens form the basic building blocks of Warble programs.

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

Identifiers are used to name variables, functions, properties, and other entities in Warble. An identifier must start with an alphabetic character (`A-Z`, `a-z`), an underscore (`_`), or any Unicode letter, and may subsequently include digits (`0-9`) as well.

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
let, mut, const, private, protected, public, export, do, null, undefined, true, false, return, yield, panic, await, async, pass, fail, try, if, else, is, from, has, as, this, that
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
  * Void literal: `null`
  * Undefined literal: `undefined`

* **Structured Literals**: Represent complex data structures.

  * Arrays: `[1, 2, 3]`
  * Strings: `"Hello, Warble!"`
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

import fs from "filesystem" in "std";
register "pkg" from "../pkg";

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

Indexed destructuring does not allow skipping positions; use `_` when you don’t care about a value. You are not required to destructure every value.

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

#### Shadowing

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

In this example, the second declaration's right-hand side sees the first declaration’s binding (`10`) and uses it to calculate the new binding (`100`). The compiler optimizes memory usage efficiently by reusing the same storage, as it easily identifies that the lifetimes of these shadowed bindings do not overlap.

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

The explicit requirement of the `do` keyword for scopes and semicolons for literals clarifies parsing rules and avoids ambiguity, maintaining Warble’s commitment to simplicity and explicitness.

## 4 Types

### 4.1 Literal Types

Literals in Warble represent explicit, directly-written values within the source code. They form the foundational building blocks of expressions and declarations throughout the language, serving as the simplest way to express data clearly and explicitly.

Warble provides a variety of literal types, broadly categorized into primitive and structured literals. Primitive literals, such as boolean, integer, float, and character, represent fundamental data values. Structured literals, including arrays, strings (including template strings), enums, tuples, objects, functions, and ranges, offer powerful and flexible ways to organize and represent more complex data patterns.

Each literal type is carefully designed to be both intuitive and expressive, balancing familiarity from other programming languages with Warble's distinctive features and syntactic clarity. The sections that follow explain each literal type in detail, along with their syntax, semantics, and notable characteristics.

#### 4.1.1 Boolean

Boolean literals represent simple truth values and are stored as 8-bit values. Warble includes two keyword literals to represent boolean values: `true` and `false`. These behave exactly as you would expect, with no deviations from conventional boolean logic.

**Examples:**

```warble
let isReady = true;
let hasFailed = false;
```

#### 4.1.2 Void

The `void` type in Warble serves as a special marker type used primarily for unions and functions without return values. A symbol with type `void` has a size of `0`, meaning it occupies no space at runtime.

The literal keyword representing the `void` type is `null`. Similar to how the literal `true` is of type `boolean`, the literal `null` is inherently of type `void`.

Warble is a strongly typed language, and normal values are never implicitly nullable. For instance, the numeric value `0` does not represent any kind of null or invalid state; it is a completely valid number with a clearly defined value. There are no inherently nullable values. Instead, when a nullable or optional value is desired, a union type is used.

An optional value in Warble is simply a union of the form `<T, void>`. This pattern has a common nickname: an "optional". However, it is not a special built-in feature; it's just a convenient naming convention for unions that can either hold a value or represent an absence of one.

Unions internally include a small type tag indicating the current active type they hold. Because of this type tag, unions inherently have the capacity to represent a null state without needing additional space for the data. Assigning `null` to an optional union will not overwrite its data payload, but it will change the internal type tag to indicate that it currently holds the type `void`, representing the absence of a meaningful value. Attempting to access or read a union holding `void` is considered undefined behavior and should be avoided.

The reason the `void` type has a size of `0` is specifically to avoid increasing the memory footprint of union types. If `void` occupied actual memory (for instance, 64 bits), it would force all unions containing it, such as `<u8, void>`, to increase their size unnecessarily. Because `void` has zero size, the memory footprint is minimized, containing only the size necessary for the non-void type and the union's type tag.

Functions that do not explicitly return a value implicitly return `void`. The following forms are equivalent:

```warble
return;
return null;
```

Even the absence of an explicit `return` statement at the end of a function is identical to these two forms. All imply returning a value of type `void`.

#### 4.1.3 Undefined

The `undefined` literal is a keyword that evaluates to a dedicated **marker symbol**.

Each module reserves its first symbol (symbol index `0`) as kind `Undefined`. The literal keyword `undefined` refers to that symbol.

This is intentionally simple and extremely useful internally: any field that is zero-initialized naturally points at the `Undefined` symbol.

For example:

* If a symbol has not been named, its `name` slot may be `0`, meaning its name is `undefined`.
* If a declaration omits its initializer in a context where that is permitted, the initializer defaults to `undefined`.

`undefined` is not the same as `null`. The `null` literal represents a value of type `void` (an intentionally empty runtime value), while `undefined` is a marker used to mean “not provided / not set / not present”.

#### 4.1.4 Character

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

#### 4.1.5 Numeric (Integer, Float)

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

#### 4.1.6 String

String literals in Warble represent textual data and are essentially a specialized subtype of array literals. However, they have their own dedicated syntax and internal representation for efficiency and convenience.

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

#### 4.1.7 Array

Array literals represent sequential collections of elements, where each element shares a common data type. The type of an array is always inferred from its first element. Once the type is defined by this initial element, every subsequent element must either match this type exactly or be implicitly convertible to it.

For instance, this is valid:

```warble
let nums = [0u8, 1, 2, 3];
```

Here, the first element explicitly defines the array as an array of unsigned 8-bit integers (`u8`). The following elements, although lacking explicit type suffixes, are implicitly converted to match the first element's type.

In contrast, the following is invalid because the second element (`null`) is of type `void`, which cannot be converted to a boolean:

```warble
// Invalid example:
let flags = [true, null]; // Error: Cannot place 'null' (void) into boolean array
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

#### 4.1.8 Enum

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

#### 4.1.9 Tuple

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
let arrayOfInts = [...tupleOfInts]; // Not permitted, even though semantically valid.
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

#### 4.1.10 Object

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

  <compiler.constructor> = (){}, // object-mode enum key
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

#### 4.1.11 Range

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

#### 4.1.12 Function

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

Warble does not have a distinct “template list” syntax for function literals.

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

In this example, `vector<i32>()` is two postfix calls:

1. `vector<i32>` calls `vector` with the enum literal `<i32>` and returns a function.
2. `()` then calls the returned function to produce the instance.

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

##### Explicit Copy or Reference Capturing

By default, the compiler intelligently decides whether captures should occur by copy or by reference. This decision is guided by the same logic used in function calls.

To explicitly force one behavior or the other, you can prefix captures with special unary prefix operators:

* `&a`: explicitly capture by reference.
* `@a`: explicitly capture by copy.

Examples:

```warble
let byRef = [&x] { /* x captured by reference */ };
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
* If a function lacks a return statement or returns explicitly without a value (`return;`), its inferred return type is `void` (equivalent to `return null;`).

Example:

```warble
// Inferred return type: int
let getValue = () {
  if (condition) return 10;
  return 20;
};

// Inferred return type: void
let logSomething = (msg) {
  print(msg);
  return; // equivalent to `return null;`
};
```

Function symbol structure in the compiler’s internal representation (child slice):

1. Return symbol (flagged as `Returned`)
2. Parameter symbols (flagged as `Parameter`, with `ParameterPack` for packs)
3. Captures (flagged as `Captured`)
4. Locals

##### Shorthand (Arrow) Syntax

Warble also supports a concise shorthand for single-expression functions, inspired by JavaScript’s arrow functions:

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
* Supports explicit capture modes (`&` by reference, `@` by copy).
* Parameters inherently templated, may initially have type `undefined`.
* Return type inferred automatically, must be consistent across returns.
* Concise arrow syntax available for single-expression functions.

> TODO: Cover parameter packs.

### 4.2 Compiler-Generated Non-Literal Types

#### 4.2.1 Reference

#### 4.2.2 Union

A **union** in Warble is a type-safe tagged union: it can represent exactly one of several possible types at runtime.

Conceptually, a union value has two components:

* A **type tag** (an index identifying which arm is active).
* A **value slot** sized to hold the largest arm type.

Unions are used for optional values, multi-type expression results, and (most importantly) union-returning control flow such as generators and promises.

##### Creation and Syntax

Unlike many languages, Warble does not directly specify unions through type annotations. Instead, unions are created by expressions or by union-returning control flow.

The binary operators `||` and `&&` are **union operators**. They always produce a union type.

Unlike most languages, these operators do **not** use truthiness. Instead, they branch based on whether the left-hand side is **passing** or **failing**:

* A non-union value is considered **passing**.
* A union value is **passing** when its runtime tag is in the union’s pass region (`tag >= fail_count`) and **failing** otherwise.

**`a || b`**

* **Result type:** all of `b`’s states (pass and fail), plus `a`’s pass states.
* **Runtime:** if `a` is passing, the result is `a`’s pass value and `b` is not evaluated; otherwise the result is `b`.

**`a && b`**

* **Result type:** all of `b`’s states (pass and fail), plus `a`’s fail states.
* **Runtime:** if `a` is failing, the result is `a`’s failing value and `b` is not evaluated; otherwise the result is `b`.

Example:

```warble
let data = 42 || ""; // type <compiler.integer, compiler.string>, value is 42
```

##### Union Functions (`return` / `yield`)

Warble functions do not explicitly declare union return types. Instead, the compiler infers a union return type automatically when the function contains union-producing control flow.

A function is a **union function** if it contains any of the following:

* A `yield` statement.
* A `return` or `yield` statement augmented with `async`, `pass`, or `fail`.

A function with only plain `return` statements is a normal function: its explicit returns must all resolve to a single type.

Any function that contains a `yield` statement is automatically a **generator function**.

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

The union returned by a union function is constructed from the set of types produced by its `return` and `yield` statements. Arms are deduplicated within their category (pass vs fail). If the same type is produced as both `pass` and `fail`, it appears twice in the union's arm list: once as a passing arm and once as a failing arm.

##### Promises and Generators (Special Unions)

Promises and generators are both special forms of union:

* A **generator** is a union that includes a special **exhausted** arm. The compiler recognizes this arm in `for` loops to determine when iteration is complete.
* A **promise** is a union that includes a special **unresolved** arm. The `await` operator atomically waits until the union's tag changes from unresolved to some other arm before continuing.

Both exhausted and unresolved are classified as **fail** states. When a union is in an exhausted or unresolved state, its value slot is undefined and must not be accessed.

##### Limits

Unions have a hard limit of $2^{16}-1$ total arms (65535). The type tag is one byte when there are 256 or fewer arms; otherwise it is two bytes.

#### 4.2.3 Module

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

#### 4.2.4 Symbol

Symbols are fundamental building blocks of Warble's type and runtime systems. Every value in Warble, whether it's a literal, an object, a function, or any other entity, is represented internally by a **symbol**. In practice, a symbol is simply a 32‑bit index into a module's columnar symbol table. Each column stores one property (such as the kind or name) for all symbols. Users do not manipulate these columns directly; instead, they obtain a symbol index and query properties through compiler-provided functions.

##### Symbol Structure and Properties

Because a symbol is just an index, each property lives in its own column and is accessed through compiler-defined helpers. The core columns are:

* `kindof(sym)` – an 8-bit **kind** ID (stored in a standalone `kinds` column). This is the primary classification of a symbol.
* `flagsof(sym)` – a 64-bit bitset of flags (modifiers, size hints, etc).
* `payloadof(sym)` – a 64-bit typeless payload interpreted according to the symbol’s kind (immediate values, packed indexes, pointers, etc).
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

Warble utilizes symbols extensively for efficient address resolution through a two-phase lookup process:

* **Downward Resolution**: To access nested symbols, Warble sums the displacements of child symbols.
* **Upward Resolution**: Starting from a given symbol, Warble recursively sums parent displacements until reaching a context symbol (such as a module or function).

Addressing strategies depend on the context:

* **Module Symbols**: Utilize RIP-relative addressing for top-level variables.
* **Function Symbols**: Utilize RSP-relative addressing for local variables.

##### Reflection and Runtime Access

Users access symbol metadata at runtime through Warble’s reflection operator `$`:

```warble
let sym = $namedValue;
print(nameof(sym)); // Prints "namedValue"
```

Reflection provides a powerful, dynamic interface for debugging, logging, and runtime type introspection.

##### Type-Checking and Identity Operators

Warble offers powerful type-checking operators leveraging symbols’ structural metadata:

* **`is`**: Verifies structural equivalence between symbols.

  ```warble
  if (dog1 is dog2) { /* structurally identical */ }
  ```

* **`has`**: Verifies that a symbol structurally includes required properties or methods.

  ```warble
  if (socket has { send(){}, recv(){} }) { /* satisfies the interface */ }
  ```

* **`from`**: Checks provenance, confirming if a value originates from a particular constructor.

  ```warble
  if (cat from Cat) { /* cat was created by Cat constructor */ }
  ```

##### Object-Oriented Programming via Symbols

Warble’s object-oriented system is fully symbol-based:

* Constructor functions define object structures.
* Spread (`...`) syntax supports composition by marking child symbols with the `Spread` flag.
* Visibility modifiers (`Export`, `Protected`, `Private`) enforce encapsulation at compile-time.
* Overriding methods rely on symbol shadowing. To call an overridden implementation, reference the parent symbol directly.

##### Performance and Memory Considerations

Warble’s columnar internal symbol layout provides excellent memory locality and efficiency. Rather than pretending a symbol is a contiguous struct, Warble exposes its columnar nature directly through the accessor functions above. Symbols are lightweight indexes, and all property lookups go through these compiler‑defined helpers. This straightforward model keeps memory compact while still allowing high‑level reflection and analysis.

This columnar design allows Warble symbols to scale linearly with the number of program operations, ensuring predictable memory usage even in large codebases.

Symbols thus form the backbone of Warble’s powerful and expressive type system, providing efficient representation, strong type guarantees, flexible reflection capabilities, and performance-oriented internal design.

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
- Impure / Used / Inline / Immediate / Internal / ThreadSafe / Accumulator: Miscellaneous analysis and lowering flags.
- Comtime / Runtime: Compile-time vs. runtime knowledge.
- NonZero / NonMax: Optimization hints.
- Halts: Indicates a function is proven to halt.
- Topic: Indicates that this symbol is a topic and is accessible via `this` or `that`.

###### Kinds

Every symbol has a standalone 8-bit `kinds` column storing its **kind**. Kind names are written in PascalCase.

Core kinds:

- Undefined, Unresolved, Auto, Void, Null
- Raw, Boolean, Character, Integer, Float
- Symbol, Reference
- Identifier, String, Enum, Union
- Array, Tuple, TemplateString, Object, Range
- Phi, Function, Module, Label
- Optional, Variant, Expectation, Promise, Declaration

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
- Symbol: `std::pair<uint32_t, uint32_t>` of (dependency module index, symbol index).
- Reference: `std::pair<uint32_t, uint32_t>` of (dependency module index, symbol index).
- Identifier / String / Enum: `std::pair<uint32_t, uint32_t>` of (data slice start, slice length).
- Union: `std::tuple<uint32_t, uint32_t, uint16_t, uint16_t>` containing a data slice plus 16-bit counts (including `fail_count`).
- Label: `std::pair<uint32_t, uint32_t>` of (instruction start index, length).
- Array / Tuple / Object / TemplateString / Function: `std::pair<uint32_t, uint32_t>` of (symbol slice start index, length).
- Range: `std::pair<uint32_t, uint32_t>` of (start symbol index, end symbol index).
- Module: a `compiler::program::Module*` during compilation.

### 4.3 Union Mechanics

This section provides a detailed explanation of how unions are created, manipulated, and represented in memory. Unions in Warble are conceptually straightforward as tagged unions, and Warble provides several powerful and ergonomic operators for working with them, ensuring both expressive power and performance efficiency.

#### 4.3.1 Creation (`||`, `&&`, `return`, `yield`)

Unions in Warble are created implicitly through certain expressions rather than explicit type annotations. The main ways unions arise are:

**Union Operators (`||`, `&&`):**

Both `||` and `&&` are union operators. They produce a union and short-circuit based on whether the left-hand side is passing or failing (rather than on truthiness).

* `a || b` returns `a` (without evaluating `b`) when `a` is passing; otherwise it returns `b`.
* `a && b` returns `a` (without evaluating `b`) when `a` is failing; otherwise it returns `b`.

The resulting union type always includes all of the right-hand side’s states, plus either the left-hand side’s pass states (`||`) or fail states (`&&`).

Example:

```warble
let parse_int = (text) {
  if (compiler.can_parse_int(text)) {
    return pass compiler.parse_int(text);
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
// Instead of <compiler.integer, <compiler.boolean, compiler.string>>, Warble flattens it to <compiler.integer, compiler.boolean, compiler.string>
```

Flattening occurs at compile-time, merging all potential union types into a single, flat union type.

#### 4.3.3 Type-Tag Layout

Every union stores a type tag alongside its data to identify which arm is currently active.

The total size of a union is determined as:

* The size of the largest possible arm type.
* Plus a tag (one byte when there are 256 or fewer arms; otherwise two bytes).

Unions have a hard limit of $2^{16}-1$ arms (65535).

##### Pass/Fail Ordering

Unions created by union functions have their arms ordered so that all failing arms come first and all passing arms come after them.

Each union type has compile-time metadata that includes:

* `arm_count`: total number of arms.
* `fail_count`: number of failing arms (a prefix of the arm list).

If the current runtime tag is `tag`, then:

* The union is in a failing state when `tag < fail_count`.
* The union is in a passing state when `tag >= fail_count`.

The special unresolved (promise) and exhausted (generator) states are classified as failing states.

This ordering is important, because it allows any union to be tested for pass/fail status by a single comparison.

#### 4.3.4 Narrowing (`?.`)

**TODO: This needs to be rewritten.**

The narrowing operator (`?.`) safely refines a union by verifying the presence of a property or method. It produces a new union whose candidate types include only those that define the requested property, plus an implicit `void` arm to represent failure at runtime.

```warble
let animal = getAnimal(); // union<Cat, Dog, Fish>
let sound = animal?.bark(); // union<void, Dog.bark> then resolves to union<void, string>
```

At compile-time:

* The compiler prunes union arms that don't define the accessed property.
* If no arms remain, the resulting union type is simply `void`.
* If some arms remain, the compiler generates a small readonly mapping table to translate the original union’s type tag to the new narrowed union’s type tag. Filtered-out types map to the `void` state.

At runtime:

* The current union’s tag is used to index this compact mapping table.
* If the original union's current type doesn't match the filter criteria, the result immediately short-circuits to the `void` arm.
* If it matches, the tag is updated to reflect the new union, with the payload simply aliased rather than copied, ensuring optimal performance.

Union payload copying is only triggered if a filtered union arm is mutated while the original union remains alive. This is rare in practice, ensuring most operations remain zero-copy and performant.

### 4.4 Type Inference & Compatibility

### 4.5 Conversions & Coercions

## 5 Expressions & Operators

### 5.1 Evaluation Order & Short-Circuiting

### 5.2 Built-in Operators

### 5.2.1 Control-flow Unary Operators `await`, `expect`, `assume`

| Operator     | Operand requirement | Fast path                                            | Slow path                                                 | Result type |
| ------------ | ------------------- | ---------------------------------------------------- | --------------------------------------------------------- | ----------- |
| `await`      | `promise<T>`        | If not unresolved: returns the resolved value.       | Atomically waits until the promise is no longer unresolved | `T`         |
| **`expect`** | `expectation<T,E>`  | Unwraps the *expected* arm.                          | `return fail <unexpected-value>`                          | `T`         |
| **`assume`** | `expectation<T,E>`  | Unwraps the *expected* arm.                          | **Debug:** `panic`<br>**Release:** `trap` (`unreachable`) | `T`         |

* All three are **prefix** verbs with the same precedence (just above unary
  `&` and `@`).

* Each may skip the remainder of the current statement: `await` can suspend,
  `expect` can early-return, `assume` can abort.

* The compiler lowers `expect`/`assume` to:

  ```warble
  let __tmp = expr;
  if (compiler.is_unexpected(__tmp)) {
      // expect → return fail compiler.unwrap_unexpected(__tmp);
      // assume → panic; or compiler.trap();
  }
  let value = compiler.unwrap_expected(__tmp);
  ```

* `assume` inserts an `unreachable` IR node, enabling additional optimization
  while guaranteeing a hard stop if the assumption is violated.

> **Guideline:** Default to `expect`.  Reach for `assume` only when recovery is
> impossible or not worthwhile (e.g. out-of-memory on a desktop program).

### 5.3 Operator Overloading via Symbols

### 5.4 Call Forms (explicit `->`, implicit destructuring, chaining)

## 6 Functions & Calling Convention

### 6.1 Function Declarations

### 6.2 Parameters, Default Values, Packs

### 6.3 Implicit vs. Explicit Calls

### 6.4 Overload Resolution

### 6.5 Compile-Time Specialization

## 7 Statements & Control Flow

Warble’s statements provide structured control flow, clearly and predictably managing how code executes. Statements include blocks, conditionals, loops, and jumps, each serving distinct roles in controlling program execution.

### 7.1 Block Statements

In Warble, a **block statement** groups multiple statements into a single, compound statement. Unlike many curly-brace languages, Warble explicitly distinguishes between **object literals** (using `{}`) and **scope blocks** (using `do {}`). This explicit differentiation prevents parsing ambiguity and enhances readability.

Warble block statements have these characteristics:

* They form an explicit scope boundary, introducing a new nested scope.
* Declarations inside a block are scoped to that block and inaccessible from outside.
* Blocks may contain any number of statements, including further nested blocks, declarations, expressions, and control-flow constructs.

### 7.1.1 Scoped Block (`do`)

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

Within a statement function, `return` statements indicate the expression’s resulting value. All possible code paths must return a value of the same (or compatible) type. Warble enforces this at compile-time, ensuring safety and consistency:

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
// This incurs no overhead; it’s fully optimized:
let value = if (a) { return b; } else { return c; };
```

The function interpretation is purely conceptual—an abstraction for understanding control flow. The compiled program never creates a real callable function object. You can freely use statement functions without concern about performance.

Statement functions are syntactic sugar—convenience shorthand for constructs you could write explicitly as an immediately-invoked function. However, because Warble handles this optimization seamlessly, there's no reason to write such functions manually. Statement functions are particularly useful given Warble’s omission of a traditional ternary operator. They provide a clear, expressive alternative:

```warble
let minValue = if (a < b) { return a; } else { return b; };
```

This construct neatly captures conditional logic within expression contexts, promoting clarity, consistency, and conciseness throughout Warble programs.

### 7.3 Conditional Statements

Conditional statements in Warble allow branching execution paths based on runtime conditions. Warble’s primary conditional construct is the familiar `if` statement, optionally followed by an `else` branch.

#### 7.3.1 Branching (`if`, `else`)

The basic syntax of Warble’s `if` statement resembles that of many traditional languages. The condition must be enclosed in parentheses, followed by a body, either inline or within curly braces (`{}`):

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

Each conditional arm that has a condition expression (an `if` or `else if` arm) introduces a new topic for the duration of that arm’s body.

Within that arm’s body, `this` refers to the value produced by evaluating the condition expression.

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

Warble provides standard loop constructs familiar from other programming languages, enabling repeated execution of statements or expressions. Loops in Warble include `while`, `repeat ... while`, and a powerful single-form `for` loop for concise iteration over iterables or ranges.

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

  The loop advances each iterable in lockstep. Iteration stops as soon as **any** of the iterables is exhausted.

This flexible `for` loop design makes Warble iterations expressive, concise, and efficient, covering most common iteration patterns without the complexity of a traditional `for` loop syntax.

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
}
```

* A function without an explicit `return` statement implicitly returns `null`. This behavior matches a function explicitly written as `return null;`.

**Multiple Returns (Structured Literals):**

Warble does not directly support multiple return values using the `return` keyword. However, you can easily emulate multiple return values using structured literals, like tuples or objects, without performance overhead:

```warble
let getCoordinates() {
  return (x, y, z); // Returns a tuple
}

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
}
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
}
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

When a generator is exhausted, it transitions to a special exhausted failing-state arm. In the exhausted state, the union's value slot is undefined and must not be accessed.

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
* **Statement arms**: each arm with a condition introduces a topic for the duration of that arm’s body. This includes `if` / `else if` arms, `while` bodies (condition checked before entry), and conditional-matching clause bodies (`is`, `has`, `from`). Unconditional `else` arms do not introduce a topic.

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

  // Does not capture anything; `this` is an error unless the function body has its own topic.
  get_a3 = [] { return this.a; },
};
```

### 8.4 Custom Property Resolution (operator overrides)

### 8.5 Function-Based “Constructors”

## 9 Lifetime & Memory Model

### 9.1 Ownership & Moves

### 9.2 Branching Rules

### 9.3 Loop Interaction

### 9.4 Destructor Ordering

### 9.5 Allocation Strategies (power-of-2, etc.)

## 10 Modules & Packages

Warble organizes code through a carefully designed system of **packages** and **modules**. This system emphasizes security, clarity, and explicit dependency management, ensuring both efficiency and trustworthiness in project management and third-party code integration.

### 10.1 Overview

* A **package** represents the root directory of a Warble project, explicitly defined via a `register` statement.
* A **module** corresponds to a single source file (`.wbl`) within a package, imported and exported explicitly to manage code reuse and visibility.

Both packages and modules prioritize security through clear permissions and explicit declarations of dependencies.

### 10.2 Packages

A **package** is explicitly registered and named through the `register` statement. The structure of a `register` statement is:

```
register "package_name" from "package_url" [with permissions_array in "target_package"];
```

#### 10.2.1 Registering Packages

Packages are registered at the top level of the project. The registration includes:

* A **package name** as a string literal, for referencing in imports.
* A **URL**, specifying the root directory location of the package. URLs can be:
  * Absolute (`file://...`)
  * Relative (`../local/path`)
  * Remote (`https://...`)
* Optional **permissions**, explicitly allowing the registered package to import specified modules from the standard library (`std`) or other packages.

**Examples**:

```warble
register "local_package" from "../relative/path";
register "absolute_package" from "file:///C:/absolute/path" with ["vector", "string", "math"] in "std";
register "remote_package" from "https://github.com/user/package" with ["math", "memory"] in "std";
register "extended_package" from "https://github.com/user/extended_package" with [...compiler.permissions.safe, "filesystem"] in "std";
```

If the same package is imported multiple times, it is shared, not duplicated.

#### 10.2.2 Permissions & Security Model

Warble adopts a security-first approach to package dependencies. Packages must explicitly declare permissions they require. These permissions form an **allow list**, specifying precisely what the package can import.

* Built-in permission sets simplify security declarations:

  * `compiler.permissions.safe`: Grants safe standard library modules (no dangerous access like filesystem or network).
  * `compiler.permissions.all`: Grants access to the entire standard library. Should be used only for fully trusted packages (typically your own code).

Permissions are inherited strictly; a package cannot grant permissions it doesn't have itself. If a package is registered without certain permissions, it cannot grant those permissions to packages it registers.

**Recommended Practice**:
Package authors clearly document required permissions. Users review and manually insert these into their `register` statements, making permission grants deliberate and transparent.

### 10.3 Modules

A **module** corresponds to a single Warble source file (`.wbl`) and is accessed via explicit `import` and `export` statements.

#### 10.3.1 Imports

Modules import code explicitly. The syntax for importing is:

```warble
import identifier from "specifier" [in "package_name"];
import {a, b as alias} from "specifier" [in "package_name"];
```

* The **specifier** is resolved as a relative URL from the package root directory.
* If no file extension is specified, `.wbl` is implicitly assumed.
* If the optional `in` portion is omitted, imports resolve within the current package context.

**Examples**:

```warble
import fs from "filesystem" in "std"; // Imports from standard library
import {util, helper as h} from "./local_module"; // Local module imports
```

Imports always produce immutable bindings, regardless of the original export mutability. Modules are shared: multiple imports of the same module access the same instance.

#### 10.3.2 Exports

To expose functionality from a module, Warble uses explicit `export` declarations at the module’s top-level scope:

```warble
export fn = (){};
export mut mutableValue = 42; // Only mutable internally, dependency modules cannot mutate
```

Only declarations explicitly marked with `export` become visible outside the module.

#### 10.3.3 Asynchronous Imports (`import async`)

Warble provides an asynchronous import syntax (`import async`) to handle situations like circular dependencies or delayed module loading:

```warble
import async modulePromise from "./module";
import async {fn as promisedFn} from "./module";
```

* Imported modules and their properties are available as promises.
* Async imports are primarily useful to resolve otherwise impossible circular dependencies.
* The structure and names of module exports are always known to the compiler, so named destructuring remains legal, but properties are available as promises.

### 10.4 Dependency Graph & Build Process

Warble packages and modules form a clear, explicit dependency graph defined by their registration, imports, and permissions:

* The compilation process begins with an implicitly trusted project file, granted `compiler.permissions.all`.
* Each package registers other packages explicitly, clearly defining access and security permissions.
* Modules import other modules explicitly, creating a deterministic and transparent dependency graph.

The dependency graph determines build order and module initialization. Modules execute concurrently (details covered later), ensuring efficient execution without sacrificing safety or predictability.

### 10.5 Security-First Philosophy

The entire Warble package and module system is built around transparency, explicit permissions, and proactive security considerations. Unlike traditional package managers (e.g., Node.js), Warble intentionally avoids implicit trust in imported packages:

* Packages must explicitly declare the permissions they require. No implicit file-system, network, or sensitive access occurs without explicit consent.
* Developers can quickly identify packages requiring deeper security reviews (e.g., those requesting `filesystem`, `http`, or other sensitive modules).
* Harmless packages requesting only safe permissions require minimal scrutiny.

**Example Security Consideration**:

* A package declaring `["filesystem", "http"]` explicitly informs you of its elevated capabilities. This doesn't imply malicious intent—such a package might legitimately implement an HTTP server—but it clearly communicates where review and trust matter most.

Warble’s approach ensures no hidden surprises, fostering a safer and more responsible ecosystem.

## 11 Compile-Time & Metaprogramming

### 11.1 Attributes

### 11.2 Conditional Compilation

### 11.3 Symbol Introspection APIs

## 12 Error Handling & Diagnostics

### 12.1 Compile-Time Errors

### 12.2 Runtime Traps

### 12.3 Assertion Facilities

### 12.4 Expectation-based Error Propagation

Warble’s primary structured error flow is the **expectation** pair plus the
`expect` operator:

* A function that may fail returns `expectation<Value, Error>`.
* Call-sites use `let v = expect may_fail();` to propagate errors
  transparently; the enclosing function’s return type automatically becomes
  the same expectation.
* Higher layers eventually *match* on the expectation or convert it to a
  panic via `assume`.

`assume` should be treated like `assert`: it documents an invariant and aborts
if reality disagrees.  In debug builds a full stack-trace is produced; in
optimized builds the compiler emits a single `trap` instruction.

## 13 Concurrency & Parallelism

Warble programs are inherently concurrent. A pool of worker threads executes
tasks drawn from per-thread work-stealing queues. Modules form a dependency
graph that drives safe parallel execution while the runtime schedules async
functions, timers and other work units.

### 13.1 Tasks & Schedulers

* **Worker Threads** – Every thread owns a Chase‑Lev deque. Tasks pushed to the
  bottom run locally while idle threads may steal from the top.
* **Task Types** – Module ticks, async function promises, timers and I/O events
  are all scheduled as tasks.
* **Work Loop** – After completing a task or suspending on `await`/`yield`, a
  thread:
  1. Pops another task from its deque.
  2. Runs pending module ticks in dependency order (leaves first).
  3. Attempts to steal from another thread if no local work exists.
  4. Sleeps when the global system has no work.
* **Module DAG** – Regular imports create a directed acyclic graph. A module
  tick executes only when all dependencies have finished their tick. Using
  `import async` breaks this ordering and exposes promises that must be awaited
  before their values are used.
* **Suspension** – When an async task suspends it is requeued at the steal end
  or in a background queue, allowing other tasks to proceed before it resumes.
* **THREADSAFE** – Mutable data may cross thread boundaries only when the
  symbol is marked `THREADSAFE`. Async functions cannot capture or accept `mut`
  references to non‑`THREADSAFE` values.

### 13.2 Atomic Operations

The standard library provides atomic counters and flags that are implicitly
`THREADSAFE`. They offer lock‑free read, write and compare‑exchange operations
with explicit memory ordering. Only atomic or otherwise `THREADSAFE` values may
be shared mutably between threads.

## 14 Standard Library (overview)

## 15 Tooling & Ecosystem

### 15.1 Compiler Flags

### 15.2 Package Manager

### 15.3 IDE Integration

## 16 Best Practices & Idioms

## 17 Glossary

## 18 Appendices

#### Operator Precedence

> TODO: Include a small table or paragraph cross-referencing the earlier operator precedence definition.

See §5.2 for operator descriptions; a dedicated precedence table is TODO.

#### Semantics Cross-References

For detailed semantics and behavior of specific constructs, see relevant chapters:

* **Declarations & Scope**: §3
* **Types & Signatures**: §4
* **Expressions & Operators**: §5
* **Functions & Calling Convention**: §6
* **Statements & Control Flow**: §7
* **Objects & Composition**: §8
* **Unions**: §4.2.2
* **Conditional Matching**: §7.3.2
* **Lifetime & Memory Model**: §9
* **Modules & Packages**: §10
* **Compile-Time & Metaprogramming**: §11
* **Error Handling & Diagnostics**: §12
* **Concurrency & Parallelism**: §13

### 18.2 Reserved Words

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
* **`export`** — Declares a binding as exported from its module.

#### Control Flow

* **`if`** — Begins a conditional block.
* **`try`** — Begins a type-validity branch (compile-time `true`/`false` conditional).
* **`else`** — Provides an alternative branch in a conditional statement.
* **`for`** — Begins a looping construct over an iterable.
* **`while`** — Begins a loop construct based on a boolean condition.
* **`do`** — Introduces a scoped block.
* **`repeat`** — Begins a loop, which can optionally end with a `while (condition)` portion.
* **`await`** — Unary prefix operator that suspends the current thread until a promise resolves, repeatedly returning to the work queue between checks.
* **`expect`** — Unary prefix operator that unwraps an expectation or propagates its unexpected arm via `return fail`.
* **`assume`** — Unary prefix operator that unwraps an expectation or aborts if it is unexpected.

#### Conditional Matching and Constraints

* **`is`** — Indicates a match clause in conditional matching.
* **`from`** — Indicates a provenance match clause (and is also used in imports).
* **`has`** — Indicates a structural constraint match clause.
* **`as`** — Introduces aliases (for example in imports) and type aliases.

#### Jump Statements

* **`return`** — Returns a value from a function.
* **`yield`** — Produces a value from a generator function and suspends it.
* **`panic`** — Produces an error message and exits the program.
* **`break`** — Exits the nearest enclosing loop.
* **`continue`** — Skips to the next iteration of the nearest enclosing loop.
* **`async`** — May augment `return` and `yield` to produce a promise-style union.
* **`pass`** — May augment `return` and `yield` to mark an arm as passing.
* **`fail`** — May augment `return` and `yield` to mark an arm as failing.

#### Module and Imports

* **`import`** — Imports symbols from external modules.
* **`register`** — Registers a URL as a package, which serves as the base for imports.
* **`with`** — Used in conjunction with register statements to specify allowed modules for a package.

#### Literals

* **`null`** — Represents a null literal (type `void`).
* **`undefined`** — Evaluates to the module’s marker symbol of kind `Undefined` (symbol index `0`).
* **`true`** — Boolean literal for logical truth.
* **`false`** — Boolean literal for logical falsehood.

#### Special Compiler Reserved

* **`this`** — Resolves to the top topic symbol.
* **`that`** — Resolves to the second-to-top topic symbol.
* **`compiler`** — Provides access to compiler intrinsics and built-in symbols. This namespace is fundamental and cannot be rebound.
* **`auto`** — Serves as a placeholder type, replaced by the compiler with an inferred or specified type during type checking.

---

#### Important Notes:

* The modifier keywords (`mut`, `const`, `private`, `protected`, `public`, `export`) are reserved and participate directly in declaration syntax.
* The keyword `super`, which was present in earlier drafts, has been removed and is no longer reserved.
* The previously defined keyword `local` has also been removed.

This reserved keyword set is deliberately minimal and focused on structural, syntactic, and foundational compiler-level operations, preserving maximum flexibility for user-defined functionality within Warble.