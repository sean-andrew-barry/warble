## 0 Preface

## 1 Introduction

Warble is a modern, expressive programming language designed to combine performance, simplicity, and elegance. Drawing inspiration from a diverse set of languages—including C++, Rust, and JavaScript—Warble prioritizes intuitive syntax, powerful type inference, and efficient, predictable performance. It seeks to offer programmers a language that is both easy to learn and capable of scaling to complex, high-performance applications.

At its core, Warble emphasizes clear semantics, strong type safety, and thoughtful defaults, enabling developers to write concise, readable, and maintainable code without compromising speed or flexibility. Built around innovative features like flexible variants, powerful structured literals, and seamless compile-time optimizations, Warble aims to streamline software development across a wide variety of domains—from systems programming to web development and beyond.

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
const number = 42;              // inferred as integer
let message = "Warble is cool"; // inferred as string
message = "Still cool!";        // mutable (`let`) declaration
```

---

**Structured Data with Literals**

Warble provides clear and concise structured literals:

```warble
const array = [1, 2, 3, 4];
const tuple = (1, "two", true);
const object = { name = "Aura", age = 2 };

print(object.name); // prints "Aura"
```

---

**Functions and Captures**

Warble's functions are intuitive and powerful:

```warble
const multiplier = [factor](x) => x * factor;
const double = multiplier(2);

print(double(10)); // prints 20
```

Functions can implicitly capture context:

```warble
const factor = 5;
const multiplyByFactor = (x) { return x * factor; };

print(multiplyByFactor(3)); // prints 15
```

---

**Pattern Matching**

Warble supports expressive and readable pattern matching:

```warble
const value = something;

match (value) {
  is (a) { print("Operation successful!"); }
  is (b) { print("Operation failed."); }
  default { print("Unknown result."); } // default case
}
```

---

**Variants and Optional Values**

Easily handle optional or uncertain data:

```warble
let optionalValue = 42 || null; // Create a variant of integer or void

optionalValue = null;   // now represents no value
optionalValue = 7;      // set to integer

match (optionalValue) {
  is (compiler.integer) as i { print(`Got an integer: {i}`); }
  is (compiler.void) { print("No value available."); }
}
```

---

**Efficient Looping and Ranges**

Clean and intuitive iteration syntax:

```warble
for (i in 0..5) { // exclusive range
  print(i); // prints 0, 1, 2, 3, 4
}

for (i in 1...3) { // inclusive range
  print(i); // prints 1, 2, 3
}
```

---

**Composition with Object Spreading**

Warble prefers composition over inheritance:

```warble
const position = { x = 10, y = 20 };
const velocity = { dx = 1, dy = -1 };

const entity = {
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

#### 6. **Efficient and Flexible Variants**

Variants (tagged unions) are fundamental to Warble's approach to handling complex data and control flow. The language leverages variants and intuitive pattern-matching syntax to make complex logic more straightforward and safe, enhancing code readability and reducing errors.

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
const, let, do, null, undefined, true, false, return, match, is, from, has, default, local
```

A full list of reserved keywords is available in Appendix 18.2.

#### 2.2.3 Literals (overview—full details in §4)

Literals represent fixed values directly embedded in Warble source code. They form the simplest expressions in the language, clearly communicating explicit values. Warble defines two categories of literals: **primitive literals** and **structured literals**.

* **Primitive Literals**: Represent simple data values directly.

  * Boolean literals: `true`, `false`
  * Character literals: `'a'`, `'\n'`
  * Numeric literals:
    * Integer: `42`, `0x2A`, `0b101010`
    * Decimal: `3.14`, `1.0e3`
  * Void literal: `null`
  * Undefined literal: `undefined`

* **Structured Literals**: Represent complex data structures.

  * Arrays: `[1, 2, 3]`
  * Strings: `"Hello, Warble!"`
  * Enums: `<Red, Green, Blue>`
  * Tuples: `(1, "Warble", true)`
  * Objects: `{ x = 1, y = 2 }`
  * Templates: `` `Value: {x}` ``
  * Functions: `[capture](param) => expression`
  * Ranges: `0..10` (exclusive), `1...5` (inclusive)

Complete details on each literal type, including their syntax and semantics, can be found in **§4 Types**.

### 2.3 Comments & Whitespace

Warble treats whitespace characters (spaces, tabs, and newlines) as token separators. Aside from this role, whitespace is ignored by the compiler, allowing programmers flexibility in formatting and indenting code to enhance readability.

**Comments** allow adding explanatory notes and documentation directly in Warble source code. They are ignored by the compiler and have no effect on program execution. Warble supports two styles of comments:

* **Single-line comments** start with `//` and continue until the end of the line:

  ```warble
  // This is a single-line comment
  const x = 42; // This comment follows a statement
  ```

* **Multi-line comments** are enclosed between `/*` and `*/`. They can span multiple lines and may include other comment markers without special interpretation:

  ```warble
  /*
    This is a multi-line comment.
    It can span multiple lines.
  */
  const y = 10;
  ```

Whitespace and comments do not affect the logic or execution of the program. They are purely for developer clarity and code organization.

### 2.4 Line Terminators & Semicolons

Warble employs explicit statement termination using semicolons (`;`). Each top-level statement must conclude with a semicolon, clearly marking its end and avoiding ambiguity.

Unlike some languages that allow implicit semicolons, Warble explicitly requires semicolons to separate individual statements. This ensures clarity and predictability in code parsing:

```warble
const x = 1; // valid statement termination
const y = 2; // another valid termination
```

At the top level, object literals and similar structured literals used as expressions must also end with a semicolon to form a valid standalone statement:

```warble
const obj = { x = 1, y = 2 }; // semicolon required here
```

**Line terminators** (newlines, carriage returns, and other Unicode line-ending characters) serve only as whitespace. They do not implicitly terminate statements. Statements may span multiple lines, provided they end with a semicolon:

```warble
const total = 1 +
              2 +
              3; // valid across multiple lines
```

## 3 Fundamentals

### 3.1 Declarations (`const`, `let`, shadowing)

Declarations introduce named bindings (variables or constants) into Warble code. Warble provides two primary declaration keywords:

* **`const`**: Declares an immutable (read-only) binding. Once initialized, the bound value cannot change.
* **`let`**: Declares a mutable binding. The bound value can be reassigned after initialization.

Example:

```warble
const pi = 3.14159; // Immutable binding
let count = 0;      // Mutable binding
count = 10;         // Allowed reassignment
```

#### Shadowing

Shadowing occurs whenever a new declaration uses the same name as a previous declaration. This is permitted not only across nested scopes but also within the same scope:

```warble
// Shadowing within the same scope:
const value = 42;
const value = "hello";
const value = true;
```

Each subsequent declaration of the same name "shadows" the previous ones. A lookup like `print(value)` refers exclusively to the most recent declaration, effectively making earlier declarations of the same name invisible from that point onward.

Shadowing allows you to create a sequence of updated immutable values clearly and safely, enabling code that behaves similarly to mutation but preserves immutability. This works because, in Warble, the right-hand side of a declaration executes before the name on the left-hand side is introduced, allowing the declaration to refer to the previous binding of the same name:

```warble
const value = 10;
const value = value * 10;
print(value); // Prints "100"
```

In this example, the second declaration's right-hand side sees the first declaration’s binding (`10`) and uses it to calculate the new binding (`100`). The compiler optimizes memory usage efficiently by reusing the same storage, as it easily identifies that the lifetimes of these shadowed bindings do not overlap.

**Note on Lifetimes:**
In Warble, the lifetime of a declaration begins at its first usage and ends immediately after its last usage. Thus, shadowed bindings with non-overlapping lifetimes can efficiently reuse memory. This behavior contributes significantly to Warble's performance and clarity.

### 3.2 Scopes & Name Lookup

Scopes in Warble define the visibility and lifetime of declared identifiers, helping organize code into manageable contexts. Warble scopes are created by:

* **Modules** (the top-level scope of a file)
* **Functions**
* **Loops** (`for`, `while`, `do-while`)
* **Conditionals** (`if`, `else`)
* Explicit **`do`** blocks

Unlike many curly-brace languages, Warble **does not** interpret standalone curly braces (`{ ... }`) as scope blocks. Instead, Warble uses the explicit `do` keyword to introduce scope blocks, eliminating ambiguity with object literals:

```warble
const x = 10;

do {
  const x = 20; // shadows outer `x` within this block
  print(x);     // prints 20
}

print(x);       // prints 10
```

**Name lookup** in Warble follows a straightforward rule: when you reference an identifier, Warble searches for the most recent declaration of that identifier, starting from the current scope and moving outward through parent scopes. This lookup proceeds in reverse declaration order, meaning the latest matching declaration shadows any previous declarations with the same name.

Shadowing also applies within the same scope:

```warble
const a = 1;
const a = 2; // shadows previous declaration of `a`
print(a);    // prints 2
```

The earlier `a` remains in memory and available via reflection but is inaccessible through standard identifier lookup from this point forward.

### 3.3 Semicolons vs. Empty Literals

In Warble, the curly brace notation `{}` always represents an **object literal**, never a scope or a block of statements. This contrasts with languages like C or JavaScript, where `{}` can represent an empty scope block.

To clearly differentiate scopes from object literals:

* **Empty object literal**:

  ```warble
  const emptyObject = {}; // empty object literal, must end with a semicolon
  ```

* **Empty scope block**:

  ```warble
  do {
    // This is an empty scope block; no declarations here
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

Warble provides a variety of literal types, broadly categorized into primitive and structured literals. Primitive literals, such as boolean, integer, decimal, and character, represent fundamental data values. Structured literals, including arrays, strings, enums, tuples, objects, templates, functions, and ranges, offer powerful and flexible ways to organize and represent more complex data patterns.

Each literal type is carefully designed to be both intuitive and expressive, balancing familiarity from other programming languages with Warble's distinctive features and syntactic clarity. The sections that follow explain each literal type in detail, along with their syntax, semantics, and notable characteristics.

#### 4.1.1 Boolean

Boolean literals represent simple truth values and are stored as 8-bit values. Warble includes two keyword literals to represent boolean values: `true` and `false`. These behave exactly as you would expect, with no deviations from conventional boolean logic.

**Examples:**

```warble
const isReady = true;
const hasFailed = false;
```

#### 4.1.2 Void

The `void` type in Warble serves as a special marker type used primarily for variants and functions without return values. A symbol with type `void` has a size of `0`, meaning it occupies no space at runtime.

The literal keyword representing the `void` type is `null`. Similar to how the literal `true` is of type `boolean`, the literal `null` is inherently of type `void`.

Warble is a strongly typed language, and normal values are never implicitly nullable. For instance, the numeric value `0` does not represent any kind of null or invalid state; it is a completely valid number with a clearly defined value. There are no inherently nullable values. Instead, when a nullable or optional value is desired, a variant type is used.

An optional value in Warble is simply a variant of the form `<T, void>`. This pattern has a common nickname: an "optional". However, it is not a special built-in feature; it's just a convenient naming convention for variants that can either hold a value or represent an absence of one.

Variants internally include a small type tag indicating the current active type they hold. Because of this type tag, variants inherently have the capacity to represent a null state without needing additional space for the data. Assigning `null` to an optional variant will not overwrite its data payload, but it will change the internal type tag to indicate that it currently holds the type `void`, representing the absence of a meaningful value. Attempting to access or read a variant holding `void` is considered undefined behavior and should be avoided.

The reason the `void` type has a size of `0` is specifically to avoid increasing the memory footprint of variant types. If `void` occupied actual memory (for instance, 64 bits), it would force all variants containing it, such as `<u8, void>`, to increase their size unnecessarily. Because `void` has zero size, the memory footprint is minimized, containing only the size necessary for the non-void type and the variant's type tag.

Additionally, functions in Warble without explicit return types are automatically assigned a return type of `void`. The following forms of returning from such a function are equivalent:

```warble
return;
return null;
```

Even the absence of an explicit `return` statement at the end of a function is identical to these two forms. All imply returning a value of type `void`.

#### 4.1.3 Undefined

The `undefined` literal is both a keyword and its own distinct type within Warble. Unlike `void` (represented by the literal `null`), which explicitly indicates the absence of a meaningful value but can still exist at runtime, values of type `undefined` cannot exist at runtime at all—not even within a variant.

This might seem odd, but it's intentional: `undefined` serves exclusively as a compile-time marker indicating invalid, incomplete, or uninitialized states. It represents conditions or entities that the compiler must acknowledge internally but must never permit during program execution.

A key scenario involving `undefined` is function templates. In Warble, function parameters default to being template parameters, capable of accepting any type unless explicitly constrained. As the compiler encounters calls to these functions, it generates specialized versions tailored to the specific argument types provided. The original, unspecialized function template—prior to any specialization—still exists within the compiler's symbol hierarchy. In this original form, its parameters have the type `undefined`, clearly marking them as placeholders without valid runtime representations.

Attempting to directly call or instantiate such an unspecialized function is illegal and meaningless, and the compiler will not allow it. Thus, `undefined` acts as a clear signal indicating a symbol that should never be utilized directly.

To summarize, `undefined`:

* Exists only at compile-time; it has no valid runtime representation.
* Marks invalid, incomplete, or template states.
* Clearly differentiates truly illegal states from valid but empty states (represented by `null`).

For most users, directly interacting with `undefined` is rare or nonexistent—typically, the appropriate literal for representing the absence of value at runtime is `null`. However, the existence and understanding of `undefined` are crucial for completeness of the language specification and internal compiler logic.

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
const newline = '\n';
const heart = '\u2764';
const smiley = '\u{1F600}';
```

Invalid example (multiple code points):

```warble
// const invalidChar = 'ab'; // Error: Multiple code points
```

#### 4.1.5 Numeric (Integer, Decimal)

##### Integer Literals

Integer literals represent whole numbers and are stored as unsigned 64-bit values (`uint64_t`). Numeric literals without a decimal point are classified as integers. Integers use the `NEGATIVE` flag to indicate negative values.

Integer literals may be prefixed with `+` (which is purely symbolic and has no effect) or `-` (which sets the negative flag).

Warble supports multiple integer forms:

* Binary literals (`0b`): `0b1101`
* Octal literals (`0o`): `0o755`
* Hexadecimal literals (`0x`): `0x1A3F`

Case is irrelevant for both form prefixes and hexadecimal digits.

##### Decimal Literals

Decimal literals represent floating-point numbers and are always encoded in double precision 64-bit IEEE 754 format. Decimal literals are identified by the presence of a single decimal point (`.`).

Decimals also support exponential notation (`0.255e3`). Case is irrelevant for the exponent marker (`e`).

Currently, decimal literals must begin with a digit (`0.0` rather than `.0`).

##### Numeric Literal Separators

All numeric literals (integers and decimals) can use underscores (`_`) as separators to improve readability. Underscores have no effect on the actual value and cannot start or end a numeric literal.

**Examples:**

```warble
const largeInteger = 1_000_000;
const binaryLiteral = 0b1010_1101;
const hexLiteral = 0xDE_AD_BE_EF;
const decimalValue = 123_456.789_012;
const scientificDecimal = 1.23e4;
```

#### 4.1.6 String

String literals in Warble represent textual data and are essentially a specialized subtype of array literals. However, they have their own dedicated syntax and internal representation for efficiency and convenience.

##### Syntax and Representation

String literals use double quotes (`"`) to denote the start and end of the string:

```warble
const greeting = "Hello, Warble!";
```

Internally, the compiler stores all string literals (including identifiers) within a large, per-module buffer of UTF-32 characters. Each character in this buffer is stored as a 32-bit Unicode code point.

Rather than using the `children` field, string symbols repurpose two fields from their symbol representation to reference their actual textual data:

* **`value`**: Interpreted as the starting index into the module's character buffer.
* **`size`**: Represents the length of the string (the number of UTF-32 characters). This effectively marks the end of the string relative to the starting index in the buffer.

These two fields (`value` and `size`) together fully define the range of characters representing the string literal.

##### Escape Sequences

String literals support the same set of escape sequences as character literals:

* Basic single character escape: `"\X"` (e.g., `"\n"`)
* Hexadecimal escape: `"\xHH"` (exactly 2 hex digits)
* Unicode code point escape: `"\uHHHH"` (exactly 4 hex digits)
* Unicode code point extended escape: `"\u{HHHHHH}"` (1 to 6 hex digits)

Example:

```warble
const message = "Line 1\nLine 2\tTabbed";
const unicodeExample = "I \u2764 Warble!";
```

##### Interaction with Arrays

Because strings are effectively specialized arrays of characters, they can be directly converted into arrays of characters through the spread syntax (`...`). This conversion is allowed explicitly due to the close relationship between strings and arrays:

```warble
const charArray = [..."Warble"];
// charArray is now an array of UTF-32 characters: ['W', 'a', 'r', 'b', 'l', 'e']
```

Note that this behavior is a special allowance. Generally, spreading one type of structured literal into another incompatible type (like spreading an object into an array) is not permitted. However, due to their internal relationship, strings naturally and conveniently support this conversion to arrays.

##### Summary of String Literal Features:

* Defined using double quotes (`"`).
* Stored internally as UTF-32 characters in a per-module character buffer.
* Repurpose symbol fields (`value` as index, `size` as length) for efficient storage.
* Support standard escape sequences consistent with character literals.
* Convertible to arrays of characters using spread syntax (`[...]`).


#### 4.1.7 Array

Array literals represent sequential collections of elements, where each element shares a common data type. The type of an array is always inferred from its first element. Once the type is defined by this initial element, every subsequent element must either match this type exactly or be implicitly convertible to it.

For instance, this is valid:

```warble
const nums = [0u8, 1, 2, 3];
```

Here, the first element explicitly defines the array as an array of unsigned 8-bit integers (`u8`). The following elements, although lacking explicit type suffixes, are implicitly converted to match the first element's type.

In contrast, the following is invalid because the second element (`null`) is of type `void`, which cannot be converted to a boolean:

```warble
// Invalid example:
const flags = [true, null]; // Error: Cannot place 'null' (void) into boolean array
```

##### Runtime Data vs. Compile-time Symbol Structure

Every symbol in Warble includes a `children` field, which is a collection of symbol references describing the compile-time structure of that symbol. For explicitly enumerated arrays, each child directly corresponds to an individual element. However, it's crucial to distinguish between this compile-time symbol representation and the actual runtime data structure.

Consider an array explicitly enumerating its elements:

```warble
const explicitArray = [1, 2, 3];
```

This array symbol will have three children, each representing the integer values `1`, `2`, and `3`.

##### Special Syntax: Repeated Array Initialization

Warble provides a convenient syntax (borrowed from Rust) to initialize arrays with repeated elements, useful for large arrays:

```warble
const repeatedArray = [0; 100]; // Creates an array of 100 zeros
```

This syntax indicates a single value (`0`) repeated to fill an array of length specified by a compile-time constant (`100`). The runtime memory footprint matches an array of 100 elements, but its compile-time symbol will only have a single child representing the repeated element, rather than explicitly listing each element. Such arrays are marked internally with the `REPEAT` flag. The actual length can be inferred at compile-time by dividing the total size of the array by the size of the single child element.

##### Spreading Arrays into Other Arrays

Warble also supports array expansion or "spreading" via the `...` syntax:

```warble
const firstArray = [1, 2];
const combinedArray = [0, ...firstArray, 3];
```

In this example, the array `combinedArray` contains `[0, 1, 2, 3]` at runtime. However, its compile-time `children` structure differs for efficiency reasons. Instead of cloning each element from `firstArray`, the symbol representing `combinedArray` will have three children:

* The integer literal `0`
* A single symbol referencing the array `firstArray` marked with the `SPREAD` flag
* The integer literal `3`

This avoids excessive duplication in the compiler's internal representation, especially important for large arrays.

##### Empty Arrays

Empty arrays are valid and simply contain no children in their compile-time representation:

```warble
const emptyArray = [];
```

Such arrays initially have no defined element type but can still be validly used in contexts where the element type is inferred later from usage.

##### Summary of Array Literal Rules:

* **Type Definition**: Determined by the first element. All subsequent elements must match or convert implicitly.
* **Runtime vs. Compile-time Structure**: The `children` field describes compile-time structure but is not a direct one-to-one mapping to runtime data, especially for repeated arrays and spread syntax.
* **Special Flags**:
  * `REPEAT`: Indicates repeated-initialization syntax `[value; count]`.
  * `SPREAD`: Indicates array spreading (`...[otherArray]`).
* **Size Field**: The `size` field of the array's symbol always matches its runtime memory footprint.
* **Empty Arrays**: Legal with zero `children` and initially undefined type.

#### 4.1.8 Enum

Enum literals in Warble represent sequences of symbols. Like string literals, enums are specialized subtypes of arrays designed explicitly to hold references to existing symbols. They provide a convenient, efficient way to represent collections of symbolic identifiers.

##### Syntax and Representation

Enum literals use angle brackets (`< >`) as their defining syntax:

```warble
const colors = <Red, Green, Blue>;
```

Each entry inside an enum literal must already exist as a defined symbol. If you attempt to reference an undeclared identifier within an enum literal, you will encounter a compilation error:

```warble
// Error example:
const invalidEnum = <Apple, Banana>; // Error if Apple or Banana aren't previously defined
```

This behavior emphasizes a critical difference between Warble enums and traditional enums found in languages like C++ or Java. Warble enums reference pre-existing identifiers rather than creating new symbolic names. If your goal is to define a new set of identifiers (similar to a traditional `enum class`), you should instead use object literals (covered in a later section).

##### Internal Storage and Implementation

Each symbol reference within an enum literal is internally stored as a 32-bit unsigned index into the module's symbol table. Thus, enums consist entirely of these symbol indices rather than direct runtime values.

To efficiently handle symbol references, the compiler maintains specialized per-module buffers, similar to how string literals are managed. Enum symbols reuse the symbol fields in the same manner as strings:

* **`value`**: Index of the first symbol reference in the enum buffer.
* **`size`**: Length of the enum, defined as the distance from the starting index.

##### Interaction with Arrays

Similar to strings, enum literals can be spread directly into arrays. This is explicitly permitted due to their inherent structural similarity. Enums are essentially considered a specialized form of array, and thus they naturally support this type of conversion:

```warble
const enumValues = <One, Two, Three>;
const arrayOfSymbols = [...enumValues];
// arrayOfSymbols now contains [$One, $Two, $Three]
```

##### Distinction from Arrays of Symbols

Despite this close relationship, there is a significant difference between enums and runtime arrays of symbols:

* **Enums**: Represent statically allocated sequences referencing existing symbols. They use specialized internal buffers optimized by the compiler.
* **Arrays of Symbols**: Represent runtime-local arrays explicitly created and initialized at runtime.

Example illustrating this difference clearly:

```warble
const enumExample = <a, b, c>;     // Enum literal (static, symbolic references)
const arrayExample = [$a, $b, $c]; // Array of symbols (runtime-local memory)
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
const exampleTuple = (1, true, "Warble");
```

This tuple contains three elements, each of different types: an integer, a boolean, and a string literal.

##### No Grouping Operators, Only Tuples

A critical point to understand about Warble is that it does not have traditional grouping operators using parentheses. The parentheses always create a tuple literal, never a mere expression grouping.

However, familiar grouping behavior can still be achieved through tuples due to a simple operator forwarding mechanism. When an operator is applied to a tuple, the operation is automatically forwarded to the first element of the tuple, and the tuple returns the result of this forwarded operation:

```warble
const result = (a + b) * c;
```

This code is intuitively identical in behavior to other languages, but internally, Warble interprets it slightly differently:

* `(a + b)` creates a tuple with a single element, the result of the expression `a + b`.
* `* c` applies the multiplication operator to the tuple. The tuple forwards this operation to its first (and only) element, effectively multiplying `(a + b)` by `c`.

Thus, the absence of grouping operators does not practically limit expressions but is crucial for understanding tuple behavior clearly in declaration contexts.

For instance, if you explicitly write:

```warble
const wrappedResult = (a + b);
```

Here, the resulting `wrappedResult` is a tuple containing one element, the result of `a + b`. If the tuple behavior is undesired, simply remove the parentheses:

```warble
const result = a + b; // Not wrapped in a tuple
```

Or explicitly destructure the tuple afterward, if necessary.

##### Spreading into Tuples

Tuples are the most permissive container type in Warble, capable of accepting elements of varying types and sources. Because of this, you can freely spread arrays, enums, and strings into tuples:

```warble
const arrayExample = [1, 2, 3];
const stringExample = "Warble";
const enumExample = <Red, Green>;

const combinedTuple = (...arrayExample, ..."Warble", ...enumExample, true, 3.14);
```

This works naturally due to tuples' inherent flexibility. They do not impose strict type requirements, making them a convenient "catch-all" container.

However, this flexibility is deliberately one-directional. While you can spread arrays, enums, and strings into tuples, you cannot spread tuples back into stricter container types such as arrays or enums—even if the tuple technically meets the type requirements:

```warble
const tupleOfInts = (1, 2, 3);

// Invalid:
const arrayOfInts = [...tupleOfInts]; // Not permitted, even though semantically valid.
```

This restriction ensures clarity and prevents confusion by keeping automatic conversions intuitive: from stricter containers to the more flexible tuple. To perform the reverse, you must explicitly convert or destructure the tuple manually.

##### Summary of Tuple Features:

* Defined using parentheses `( )`.
* Supports mixed element types with no uniformity constraints.
* No traditional grouping operators; parentheses always create tuples.
* Operators applied to tuples automatically forward to the tuple's first element.
* Allow spreading arrays, enums, and strings into tuples.
* Do not allow spreading tuples into stricter containers like arrays or enums.

#### 4.1.10 Template

Template literals in Warble are specialized structured containers designed for convenient text interpolation. They act as a hybrid between string literals and tuple literals, seamlessly blending literal textual content with embedded interpolated values.

##### Syntax and Interpolation

Template literals begin and end with the backtick character (`` ` ``), similar to JavaScript's template strings:

```warble
const greeting = `Hello, {name}! You have {unreadCount} unread messages.`;
```

Interpolated expressions are marked by curly braces (`{}`). Unlike JavaScript, Warble does not require a dollar sign (`$`) before these curly braces:

* To insert literal curly braces, escape them with a backslash (`\{` and `\}`):

```warble
const example = `Literal braces: \{ and \}`;
```

Within the curly braces, you can interpolate values of any type. The sections outside the braces become standard string literals, while the sections inside the braces retain their original types without automatic stringification.

##### Internal Representation and Type Distinction

Internally, a template literal is structured similarly to a tuple literal—an indexed sequence of values. The string portions between interpolations become regular string literals, while each interpolated value is preserved exactly as-is, maintaining its original type.

Each interpolated symbol inside a template literal is automatically flagged (the exact name of this flag is to be determined) to clearly indicate its status as an interpolated value. This distinction is critical because even interpolated string literals are logically different from the surrounding string literals within the template itself:

```warble
const count = 42;
const template = `You have {count} messages.`;
// Internally: tuple ["You have ", 42 (flagged), " messages."]
```

##### Stringification Responsibility

Importantly, template literals do **not** automatically stringify their interpolated contents. They simply store the original values directly. The responsibility for converting these values into a meaningful textual representation lies solely with whatever function or context consumes the template literal.

For example, if you pass a template literal to a printing function, that function must explicitly handle stringifying non-string values:

```warble
print(`The answer is {42}`); 
// The print function itself decides how to convert 42 into "42"
```

##### Interaction with Tuples

Because template literals structurally resemble tuples, you can freely spread a template literal into a tuple literal:

```warble
const tpl = `Value: {10}`;
const tplAsTuple = (...tpl);
// tplAsTuple becomes ("Value: ", 10)
```

This highlights their close semantic relationship. While currently considered a distinct built-in type, template literals could eventually become specialized syntactic sugar for tuples, though for now, they remain a distinct and specialized structured container.

##### Summary of Template Features:

* Defined using backticks (`` ` ``).
* Interpolation uses curly braces (`{}`), without a leading `$`.
* Literal curly braces are escaped with backslash (`\{` and `\}`).
* Internally stored as tuples mixing strings and interpolated values.
* Interpolated values are flagged internally (specific flag name TBD).
* No automatic stringification; that responsibility is left to the consumer.
* Freely convertible to tuples using spread syntax (`...`).

#### 4.1.11 Object

Object literals are the most versatile and complex type of literal in Warble. They represent structured, sequential containers of named declarations. While similar in concept to tuples (a sequential container of arbitrary types), objects differ fundamentally by using named lookups instead of numeric indexing.

##### Basic Syntax and Declarations

Object literals use curly braces `{ }` to enclose their declarations, with each declaration separated by a comma:

```warble
const point = { x = 10, y = 20 };
```

Declarations inside object literals follow nearly identical rules as scope-level declarations:

* They can optionally use `const` or `let` to specify mutability.
* If omitted, declarations default to immutable (`const`).
* Modifier keywords such as `private`, `protected`, and `public` can also be used, influencing property visibility rules during lookup.

For example:

```warble
const user = {
  name = "Sean",
  let age = 32,          // mutable
  private email = "...", // visibility modifier
};
```

##### Internal Representation and Lookup

Each declaration inside an object literal becomes a child symbol of the object, stored within its `children` list. Children are listed strictly in declaration order.

Lookups for object properties (`obj.a`) search backward through the `children` list, beginning at the end, to find a symbol matching the requested name. This backward lookup allows for shadowing of properties, enabling properties defined later to override earlier ones with the same name:

```warble
const obj = {
  a = 1,
  a = 2, // shadows the first `a`
};

obj.a; // resolves to the second declaration (value: 2)
```

This mechanism also supports function overloads, allowing multiple functions with identical names but differing signatures to coexist:

```warble
const mathOps = {
  add = (a, b){ return a + b; },
  add = (a, b, c){ return a + b + c; }, // overloads previous `add`
};
```

During lookup, Warble initially hits the most recent definition (the last listed). If the immediate match is not compatible (such as due to differing function parameters), the compiler continues searching backward for a suitable overload.

##### Special Property Names and Lookup Syntax

In addition to standard identifiers, properties can be named using string literals or even enum literals. These allow names that wouldn't otherwise be valid identifiers:

```warble
const obj = {
  "some-key" = 42,
  <Red, Green> = "ColorPair",
};

obj."some-key";    // accesses the property named "some-key"
obj.<Red, Green>;  // accesses the property named by enum <Red, Green>
```

These alternate naming syntaxes are particularly useful in advanced design patterns and object-oriented modeling (discussed in later sections).

##### Spreading Objects

Warble emphasizes composition over inheritance, making object spreading an essential feature. You can spread one object literal's properties into another using the spread syntax (`...`):

```warble
const position = { x = 0, y = 0 };
const velocity = { vx = 1, vy = 1 };

const movingObject = {
  ...position,
  ...velocity,
  name = "Player",
};
```

Internally, this spreading does not deeply clone each property. Instead, it adds a single symbol of type `object`, flagged with `SPREAD`, to the target's `children`. During property lookups, the compiler transparently recognizes these spread symbols, recursively searching their contained properties as needed.

Spreading can include visibility modifiers as well:

```warble
const obj = {
  private ...secretProperties,
  public ...publicProperties,
};
```

Note: Only object literals can be spread into object literals; other containers (arrays, enums, tuples) cannot, due to their lack of named properties.

##### Auto-Incrementing Enum-like Properties

Warble provides a convenient shorthand to replicate traditional enum class behavior (common in languages like C++). By prefixing property names with the hash character (`#`) and omitting explicit assignments, you instruct the compiler to assign sequential numeric values automatically, beginning at zero:

```warble
const Colors = { #Red, #Green, #Blue };
```

This shorthand expands to:

```warble
const Colors = { Red = 0, Green = 1, Blue = 2 };
```

Auto-incrementing properties may be mixed freely with explicitly defined properties, including methods and other values:

```warble
const Example = {
  #Start,          // implicitly 0
  #Middle,         // implicitly 1
  description = "Example enum-like structure",
  #End,            // implicitly 2
  method = (){ /*...*/ },
};
```

##### Summary of Object Literal Features:

* Defined using curly braces `{ }`.
* Consist of named declarations, optionally mutable (`let`) or immutable (`const`, default).
* Support visibility modifiers (`public`, `private`, `protected`).
* Allow shadowing and overloads via backward lookup.
* Allow property names defined as strings or enums.
* Enable composition via object spreading (`...`).
* Provide special syntax for auto-incrementing numeric declarations (`#`).

#### 4.1.12 Range

Range literals provide a concise syntax for defining numeric intervals to be iterated over. They serve primarily as simple iterators for loops or similar iteration contexts.

##### Syntax

Range literals use two simple forms:

* **Exclusive ranges**: `start..end`
* **Inclusive ranges**: `start...end`

Both `start` and `end` must resolve to numeric values at compile time, but they do not have to be literal constants. Identifiers and even more complex expressions are allowed, as long as they ultimately produce numeric values compatible with iteration.

##### Examples

A basic example of an exclusive range used in iteration:

```warble
// Iterates from 0 up to (but not including) 10
for (i in 0..10) {
  print(i);
}
```

An inclusive range example:

```warble
// Iterates from 1 through 5, including 5
for (value in 1...5) {
  print(value);
}
```

These two notations (`..` and `...`) provide clear and intuitive syntax for iteration boundaries, distinguishing whether the endpoint is included or excluded.

##### Summary of Range Literal Features:

* Defined using `start..end` (exclusive) or `start...end` (inclusive).
* Accept numeric literals, identifiers, or more complex numeric expressions.
* Primarily used for concise numeric iteration.

#### 4.1.13 Function

Function literals in Warble represent first-class functions defined directly as values, inspired primarily by C++ lambda syntax. Unlike traditional function definitions found in many languages, Warble treats all functions as literals, allowing flexibility in capturing context, declaring parameters, and defining behavior.

##### Syntax Overview

Function literals have three main parts, closely mirroring C++ lambdas:

* **Capture group**: Defined with square brackets (`[ ]`). *(Optional)*
* **Parameter list**: Defined with parentheses (`( )`). *(Optional, but at least one of captures or parameters is required)*
* **Function body**: Defined with curly braces (`{ }`). *(Required, except in shorthand arrow syntax)*

Examples:

```warble
// Full syntax with captures and parameters
const add = [x](y) { return x + y; };

// Captures omitted (implicit capture)
const square = (x) { return x * x; };

// Parameters omitted (captures required)
const printX = [x] { print(x); };
```

If both captures and parameters are present, captures must come first. If neither are specified, the function literal would be indistinguishable from an object literal, thus one of them must always be present.

##### Capture Groups

The capture group (`[ ]`) allows functions to access variables from their defining scope. Captures are optional. If omitted entirely, the function implicitly captures referenced external variables. To prevent implicit capturing, provide an explicit empty capture group (`[]`).

The syntax inside capture groups closely resembles object literal declarations:

* Comma-separated declarations.
* Default to immutable (`const`) if `let` isn't specified explicitly.
* Short-hand declarations allowed: writing `a` alone is equivalent to writing `const a = a;`.
* Captures can be renamed using explicit assignment: `[x = a]` captures `a` from the outer scope and names it `x` inside the function.

Examples:

```warble
const x = 10, y = 20;

// Implicit captures (automatically captures x)
const implicit = () { return x; };

// Explicit captures (no implicit capturing)
const explicit = [x, y] { return x + y; };

// Renamed captures
const renamed = [total = x] { return total; };
```

##### Explicit Copy or Reference Capturing

By default, the compiler intelligently decides whether captures should occur by copy or by reference. This decision is guided by the same logic used in function calls.

To explicitly force one behavior or the other, you can prefix captures with special unary prefix operators:

* `&a`: explicitly capture by reference.
* `@a`: explicitly capture by copy.

Examples:

```warble
const byRef = [&x] { /* x captured by reference */ };
const byCopy = [@x] { /* x captured by copy */ };
```

Symbols generated from capturing are flagged as `CAPTURE`.

##### Parameters

Parameters (`( )`) are also declared as a comma-separated list, following similar rules as captures:

* Parameters default to immutable (`const`), explicit use of `let` can override this.
* Short-hand notation allowed; explicit `const` or `let` can be omitted.
* Parameters uniquely allow incomplete declarations (no initial value), because their values are provided upon invocation.

Examples:

```warble
const add = (a, b) { return a + b; };
const increment = (let x) { x++; return x; };
```

All parameters are inherently templates (traditional template concept, not to be confused with template literals). Their types are determined by the function invocation arguments. Before specialization, parameter symbols are assigned the type `undefined`, marking them incomplete.

Symbols generated from parameters are flagged as `PARAMETER`.

##### Function Body and Return Values

The function body (`{ }`) is essentially a standard scope, allowing normal scope-level declarations, control-flow constructs, and expressions. Symbols declared locally within a function body are flagged as `LOCAL`.

Functions do not explicitly declare return types; instead, the return type is inferred from their usage:

* Multiple `return` statements are allowed, but they must all return exactly the same type.
* The return type is represented by a singular symbol flagged as `RETURN`.
* If a function lacks a return statement or returns explicitly without a value (`return;`), its inferred return type is `void` (equivalent to `return null;`).

Example:

```warble
// Inferred return type: int
const getValue = () {
  if (condition) return 10;
  return 20;
};

// Inferred return type: void
const logSomething = (msg) {
  print(msg);
  return; // equivalent to `return null;`
};
```

Function symbol structure in the compiler’s internal representation (`children` list):

1. Return symbol (flagged as `RETURN`)
2. Parameter symbols (flagged as `PARAMETER`)
3. Captures (flagged as `CAPTURE`)
4. Locals (flagged as `LOCAL`)

##### Shorthand (Arrow) Syntax

Warble also supports a concise shorthand for single-expression functions, inspired by JavaScript’s arrow functions:

* The syntax uses an arrow (`=>`) to denote the function.
* Omits the curly-braced function body.
* Immediately returns the single provided expression.

Examples:

```warble
const double = (x) => x * 2;

// Single parameter shorthand (no parentheses needed)
const square = x => x * x;

// With implicit captures
const multiplier = [factor] x => x * factor;
```

Unlike JavaScript, Warble explicitly disallows arrow functions with a curly-braced body. If a body is needed, a normal function literal must be used instead:

```warble
// Invalid: curly-braced bodies are disallowed in arrow functions
// const invalid = (x) => { return x + 1; };

// Correct form for explicit bodies:
const valid = (x) { return x + 1; };
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

#### 4.2.1 Variant

A **variant** in Warble is a type-safe tagged union, meaning it can represent exactly one of several possible types at runtime. Variants enable expressive, type-safe handling of values whose type can vary, without the overhead of traditional polymorphism or dynamic dispatch.

##### Creation and Syntax

Unlike many languages, Warble does not directly specify variants through type annotations. Instead, variants are implicitly created through expressions that combine multiple possible values.

For instance, the logical operators `||` (logical OR) and `&&` (logical AND) always return variants in Warble. This is because the result of these logical operators depends on runtime conditions:

```warble
let data = 42 || "";
```

Here, the compiler infers that `data` has the variant type `<compiler.integer, compiler.string>` since it could store either type depending on runtime conditions. The initial value placed in the variant (`42` in this example) follows the standard logical rules of the operator.

However, if you want to explicitly create a variant without performing logical evaluations, Warble provides the special `!!` operator:

```warble
let data = 0 !! "Hello" !! true;
```

In this case, `data` is explicitly assigned the variant `<compiler.integer, compiler.string, compiler.boolean>`. The value placed in the variant is always the first operand (`0`), regardless of its truthiness. This gives the programmer precise control over both the contained value and the variant's possible types.

##### Variant Returns from Functions

Warble functions do not explicitly declare variant return types. Instead, the compiler infers variant returns automatically when you use the special `return case` syntax within multiple code paths:

```warble
const parse(input: compiler.string) {
  if (input.is_number()) {
    return case input.to_int();
  } else {
    return case error("Not a number");
  }
}
```

Here, the compiler determines that `parse` returns the variant `<compiler.integer, compiler.error>`. The keyword `case` informs the compiler that the returned type is intended to be part of a variant, rather than forcing each return path to match exactly the same type.

##### Pattern Matching

Variants are commonly narrowed into specific types using Warble's pattern matching syntax:

```warble
let result = get_result();

match (result) {
  is (compiler.integer) as num {  
    print(num * 2); 
  }
  default { 
    print("Result was not an integer"); 
  }
}
```

Here, `match` inspects the runtime tag of the variant. If `result` currently holds a `compiler.integer`, the matched value is extracted into `num`, and the corresponding block executes. If it contains a different type, the `default` case executes.

##### Memory and Performance

Variants store a type-tag alongside their contained value. The total memory size of a variant is the size of its largest possible contained type plus a small, optimized tag. Details on how tags are organized and optimized are covered in section 4.3.3 Type Tag Layout.

Further details, including the complete semantics of variant creation, flattening rules, narrowing operators, and unpacking mechanisms, are explained thoroughly in section 4.3 Variant Mechanics.

#### 4.2.2 Module

In Warble, a **module** represents a single imported source file. For every source file included in a project, exactly one corresponding module object is created—no matter how many times that file is imported. The module acts as the root scope and primary container for all information defined within that source file.

##### Module Structure and Data Layout

Modules store extensive metadata required by the compiler, debugger, and runtime. Each module includes the following structures:

* **Symbol Table**
  The symbol table is stored in a columnar (struct-of-arrays) format rather than an array-of-structs. Each symbol's properties are stored as separate columns of data. Symbols are accessed exclusively via 32-bit indexes rather than pointers. Even constructs like symbol references and lookups internally resolve to these 32-bit symbol indexes. This design ensures memory compactness, cache efficiency, and simplified symbol management.

* **Character Data Buffer**
  The module stores a large buffer of UTF-32 characters. This character buffer aggregates all textual identifier names and string literals found within the source code of that module. Identifiers and string literals reference substrings within this buffer, optimizing memory reuse and lookup efficiency.

* **Enum Data Buffer**
  Similar to the character data buffer, modules store an enum data buffer. This buffer contains sequences of 32-bit symbol indexes, representing enum literal definitions. Enum literals are managed and referenced similarly to text literals, providing uniformity and efficiency.

* **Source Mapping Information**
  To support debugging, error reporting, and source reconstruction, the module maintains precise source mapping data. This consists of two tightly-coupled buffers:

  * **Tokens Buffer**: An array of 8-bit values representing each lexical token in the source code. Each numerical token uniquely identifies a syntactic element from the Warble source file.
  * **Widths Buffer**: An accompanying 8-bit array storing each token's width (in characters). The tokens and widths buffers are always kept exactly synchronized, ensuring the original source code can be faithfully reconstructed and providing exact character ranges for error reporting or source highlighting.

  Each symbol in the symbol table includes source mapping references that index into these buffers, enabling precise location information retrieval.

* **Lines Table**
  A module also maintains an array of 32-bit indexes called the "lines table," containing positions for each newline character (`\n`) in the original source text. This table enables efficient binary search operations to determine line and column information, essential for readable diagnostics and source code display.

##### Static Allocation and Memory

All module data described above are statically allocated and stored directly within data pages embedded in the compiled executable. This static embedding eliminates dynamic allocation overhead at runtime, ensuring rapid initialization and predictable memory usage.

Additionally, the module object may contain user-defined top-level declarations. To accommodate these declarations, the compiler reserves a dedicated static memory block for each module. Each top-level declaration is assigned a specific slot within this memory block, analogous to how a function call uses a stack frame for its local variables.

#### 4.2.3 Symbol

Symbols are fundamental building blocks of Warble's type and runtime systems. Every value in Warble, whether it's a literal, an object, a function, or any other entity, is represented internally by a **symbol**. Symbols encapsulate extensive metadata, including type information, memory layout details, visibility, and other compile-time properties. Users cannot directly instantiate or modify symbols, but they interact with them through reflection, type-checking operators, and object-oriented constructs.

##### Symbol Structure and Properties

Conceptually, each symbol presents to the user as a structured object containing these key properties:

* **`type`**: A simple 8 bit numeric ID that describes the type associated with the symbol.

* **`registers`**: A bitset marking which CPU register indexes this symbol’s runtime value potentially resides in at any point during its lifetime.

* **`flags`**: A bitset marking various modifiers and metadata. Common flags include:

  * `CONST` – indicates a declaration was bound with `const`.
  * `PUBLIC`, `PROTECTED`, `PRIVATE` – visibility modifiers.
  * `SPREAD` – marks a symbol as being created from a spread operation `...`.
  * `REPEAT` – marks an array symbol as being created by the repetition syntax (`[value; length]`).
  * ...Etc.

* **`value`**: A generic typeless field, interpreted according to the symbol’s type. It may store an immediate literal, a pointer or index, or other arbitrary data as required by the symbol’s semantics.

* **`size`**: A 32 bit number representing memory footprint of the symbol, measured in bytes. Some symbol types, like the string and enum, repurpose this to refer to the number of elements they represent instead of their byte size. The byte size can still be determined by multiplying the `size` by the element size.

* **`displacement`**: A 32 bit offset from its parent symbol in bytes. Symbols representing fields or subcomponents use this displacement for efficient address calculation.

* **`name`**: A reference to a symbol that identifies this symbol. For named declarations like variables or functions, this reference a string literal symbol, holding the identifier's name. An anonymous symbol will reference the globally unique `null` symbol indicating its lack of a name.

* **`parent`**: A reference to the parent symbol (such as a containing object, function, or module). This relationship enables efficient hierarchical address resolution. The root symbol of a hierarchy is always the module, which references the global `null` symbol as its parent.

* **`children`**: A reference to an enum list of child symbols. Aggregates such as objects or arrays represent their fields variants as child symbols. Many symbol types, such as primitives like booleans or numbers, do not have children. They reference an empty enum symbol `<>` to indicate this.

Additionally, symbols contain source mapping data, facilitating debugging and diagnostics:

* **`token_start`**, **`token_end`**, **`character_start`**: 32 bit indexes linking symbols back to their corresponding locations in the source code.

##### Symbol Creation

Symbols are exclusively produced by expressions in Warble. Each expression, no matter how simple or complex, generates exactly one symbol. Literals, arithmetic operations, function calls, and other constructs all yield symbols upon evaluation:

```warble
const a = 42; // creates a symbol representing the literal '42'
const b = a + 1; // creates symbols for 'a', '1', and the result of '+'
```

Symbols created by expressions are temporary and anonymous by default. To preserve a symbol beyond its immediate expression, it must be named explicitly through a declaration:

```warble
const namedValue = 100; // assigns the symbol for '100' a name and `CONST` flag
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
const sym = $namedValue;
print(sym.name); // Prints "namedValue"
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
* Spread (`...`) syntax provides inheritance by marking child symbols with the `SPREAD` flag.
* Visibility modifiers (`PUBLIC`, `PROTECTED`, `PRIVATE`) enforce encapsulation at compile-time.
* Overriding methods are implemented by symbol shadowing; the `super` keyword adjusts the lookup anchor to invoke overridden symbols.

##### Performance and Memory Considerations

Warble’s columnar internal symbol layout provides excellent memory locality and efficiency. Although users perceive symbols as individual structured entities, internally they are managed as columnar data (arrays for each property). Users interact transparently without needing to manage indexes directly, enjoying a simple abstraction that preserves high performance and scalability.

This columnar design allows Warble symbols to scale linearly with the number of program operations, ensuring predictable memory usage even in large codebases.

Symbols thus form the backbone of Warble’s powerful and expressive type system, providing efficient representation, strong type guarantees, flexible reflection capabilities, and performance-oriented internal design.

#### 4.2.4 Block
> TODO

### 4.3 Variant Mechanics

This section provides a detailed explanation of how variants are created, manipulated, and represented in memory. While variants are conceptually straightforward as tagged unions, Warble provides several powerful and ergonomic operators for working with them, ensuring both expressive power and performance efficiency.

#### 4.3.1 Creation (`||`, `&&`, `!!`, `return case`)

Variants in Warble are created implicitly through certain expressions rather than explicit type annotations. The main ways variants arise are:

##### Logical Operators (`||`, `&&`)

* `a || b` evaluates the truthiness of `a`. If `a` is truthy, the variant holds `a`; otherwise, it holds `b`.
* `a && b` evaluates the truthiness of `a`. If `a` is falsy, the variant holds `a`; otherwise, it holds `b`.

Example:

```warble
let result = 0 || "hello"; // variant holds "hello"
let another = 42 && false; // variant holds false
```

These operators always yield a variant containing exactly the operand chosen based on runtime conditions.

##### Explicit Variant Creation (`!!`)

The `!!` operator explicitly creates a variant without evaluating the truthiness of its operands. The first operand always becomes the initial value, while subsequent operands define additional possible types.

Example:

```warble
let explicitVariant = 0 !! "Hello" !! true;
// explicitVariant holds 0, with possible types: integer, string, boolean
```

This operator provides precise control over variant creation, especially useful for cases when the initial value may be falsy.

##### Variant Returns (`return case`)

Inside functions, the `return case` syntax explicitly indicates variant returns. If different code paths return distinct types using `return case`, Warble merges these into a single variant type:

```warble
const checkValue(x: compiler.integer, compiler.boolean) {
  if (x from compiler.integer) {
    return case x;
  } else {
    return case "bool";
  }
}

// Function returns a variant of <compiler.integer, compiler.string>
```

The compiler automatically infers the return variant type from these `return case` statements.

#### 4.3.2 Flattening

Warble automatically flattens nested variants, simplifying variant handling and preventing overly complex type trees.

For example:

```warble
const nested = 1 || false || "Hello";
// Instead of <compiler.integer, <compiler.boolean, compiler.string>>, Warble flattens it to <compiler.integer, compiler.boolean, compiler.string>
```

Flattening occurs at compile-time, merging all potential variant types into a single, flat variant type. This simplifies runtime logic and improves performance.

#### 4.3.3 Type-Tag Layout

Every variant stores a small type tag alongside its data to identify which of its possible types is currently active. The tag size is minimized by packing it into the alignment padding or unused bits of the variant's memory representation whenever possible.

The total size of a variant is determined as:

* The size of the largest possible type.
* Plus the minimal number of bits required to distinguish all variant types.

The compiler ensures tags are consistently ordered and deterministic, enabling efficient ABI and runtime optimizations. Tag lookup, type checking, and narrowing operations are highly performant as a result.

#### 4.3.4 Narrowing (`?.`)

The narrowing operator (`?.`) safely refines a variant to a specific type by verifying method or property presence. It removes incompatible types at compile-time, short-circuiting if runtime checks fail.

Example:

```warble
const variant = getAnimal(); // returns variant of <Cat, Dog, Fish>

variant?.bark()?.toUpperCase();
```

* First, Warble checks which types support `.bark()`. (Only `Dog` does.)
* If at runtime the variant isn't a `Dog`, the expression short-circuits.
* If it is a `Dog`, narrowing succeeds, and `.bark()` is safely invoked.
* Further methods, like `.toUpperCase()`, apply only to types remaining after narrowing.

This operator supports chaining, progressively narrowing the variant through multiple operations.

#### 4.3.5 Unpacking (`??`)

The unpacking operator (`??`) extracts a variant's contained value if its runtime type matches that of the fallback provided. If types don't match, the fallback value is used instead:

Example:

```warble
const data = 1 || "hello"; // variant <compiler.integer, compiler.string>

const str = data ?? "default";
// If data holds a string then it's unpacked. If not, `str` is "default"
```

This operator simplifies conditional extraction, eliminating boilerplate associated with explicit type checks or pattern matching. It provides a clean way to safely access a variant's underlying data based on runtime type.

### 4.4 Type Inference & Compatibility

### 4.5 Conversions & Coercions

## 5 Expressions & Operators

### 5.1 Evaluation Order & Short-Circuiting

### 5.2 Built-in Operators

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
  const localValue = 42; // Scoped to this block
  print(localValue);     // Accessible here
}

print(localValue); // Error: `localValue` is out of scope
```

Unlike object literals (`{}`), `do {}` blocks do not produce values or data structures. They exist purely to create local scopes and organize code logically.

**Nested blocks** clearly manage inner scopes and shadow outer-scope variables:

```warble
const value = 1;

do {
  const value = 2;  // shadows outer `value`
  
  do {
    const value = 3; // shadows the previous block's `value`
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
const value = if (condition) {
  return 42;
} else {
  return 0;
};
```

The compiler interprets this as:

```warble
const value = [condition]() {
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
const result = if (x > 0) {
  return "Positive";
} else if (x < 0) {
  return "Negative";
} else {
  return "Zero";
};
```

However, some statements—like loops—do not inherently guarantee that a `return` is executed on every path. For these cases, you use the special `default` keyword at the end of the statement function to provide a fallback value:

```warble
const found = for (v in values) {
  if (v == target) {
    return true;
  }
} default false;
```

Here, the compiler effectively interprets this as:

```warble
const found = [values, target]() {
  for (v in values) {
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
const value = if (a) { return b; } else { return c; };
```

The function interpretation is purely conceptual—an abstraction for understanding control flow. The compiled program never creates a real callable function object. You can freely use statement functions without concern about performance.

Statement functions are syntactic sugar—convenience shorthand for constructs you could write explicitly as an immediately-invoked function. However, because Warble handles this optimization seamlessly, there's no reason to write such functions manually. Statement functions are particularly useful given Warble’s omission of a traditional ternary operator. They provide a clear, expressive alternative:

```warble
const minValue = if (a < b) { return a; } else { return b; };
```

This construct neatly captures conditional logic within expression contexts, promoting clarity, consistency, and conciseness throughout Warble programs.

### 7.3 Conditional Statements

Conditional statements in Warble allow branching execution paths based on runtime conditions. Warble’s primary conditional construct is the familiar `if` statement, optionally followed by an `else` branch. While similar in style to languages like C++ or JavaScript, Warble introduces a few distinctive features and considerations, particularly involving the optional `as` keyword for binding results.

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

**Capturing Conditional Results with `as`**

Warble provides a special feature allowing you to capture the result of the condition expression into an identifier for convenient reuse inside the body. This is achieved with the `as` keyword, placed immediately after the condition:

```warble
if (a || b) as c {
  print(c); // `c` is the variant resulting from `a || b`
}
```

In this example:

* The expression `a || b` produces a variant that the condition evaluates for truthiness.
* If truthy, the variant is bound to the identifier `c` and made available within the block.

**Syntax and Stylistic Considerations**

Currently, the syntax for using `as` with an inline body, though technically unambiguous, presents stylistic concerns:

```warble
if (condition) as result return result; // Technically valid but stylistically unclear
```

Although the compiler easily interprets this—`as` clearly indicates the subsequent identifier (`result`) and then the body (`return result;`)—the syntax's readability and clarity are subjects of ongoing design evaluation.

Alternate placements for the `as` keyword, such as within the condition parentheses (`if (a || b as c)`), have been considered but are currently viewed less favorably, as this might suggest the capture is part of the expression rather than a separate binding step.

**Future Directions**

This aspect of Warble’s conditional syntax is still subject to refinement. Future versions of Warble might:

* Forbid inline `if` bodies entirely to enforce clarity.
* Adjust the placement or usage of the `as` keyword for improved readability or style.

For now, users should be aware that while the current syntax works and is clearly defined, it may evolve as the language matures.

#### 7.3.2 Pattern Matching (`match`, `is`, `has`)

Warble provides a powerful alternative to the traditional `switch` statement called **pattern matching**. Pattern matching is performed with the `match` statement, allowing sophisticated checks and bindings that greatly surpass the capabilities of classic `switch`.

A basic `match` statement looks like this:

```warble
match (a || b) {
  is (string) as s {
    print(s);
  }
  is (integer) as i {
    print(i);
  }
  default {
    print("Unknown type");
  }
}
```

**Basic Structure**

The syntax of the `match` statement is straightforward:

* The condition being matched is enclosed in parentheses immediately following the `match` keyword.
* The body of a `match` statement is required, and there is no inline version. It must be enclosed in curly braces `{}`.
* The body of a `match` statement is not a standard scope. It strictly allows specific matching statements: `is`, `has`, and `default`.

**Pattern Matching Keywords**

Within a `match` statement body, the following keywords are supported:

* **`is`**: Performs a type or value match, depending on the matched object’s implementation.
* **`has`**: Checks for the presence of certain properties or behaviors (primarily used in OOP patterns, details covered later).
* **`default`**: Serves as a fallback when no other pattern matches. You may only have one `default` case per `match` statement.

Each of these may currently be written with either inline or block syntax, although inline usage is under stylistic reconsideration.

**The `as` Keyword**

Both `is` and `has` can use the optional `as` keyword to bind the result of the match to an identifier. This provides convenient access to the matched value:

```warble
match (value) {
  is (string) as s {
    print("Got a string:", s);
  }
  has (length) as l {
    print("Has length:", l.length);
  }
  default {
    print("Unknown");
  }
}
```

The `default` case cannot use `as`, since it matches by default without performing any checks or bindings.

**Chaining and Fallthrough**

Pattern matching does **not** require a `break` statement. Patterns do not implicitly fall through into subsequent cases. Instead, multiple checks can lead into the same case body by chaining `is` or `has` statements without a body:

```warble
match (condition) {
  is (0)
  is (1)
  is (2) {
    // Handles cases 0, 1, and 2
  }
}
```

Rules for chaining:

* Every chain must eventually terminate with a body.
* `as` may be used exactly once per chain, and if used, it must appear directly before the body:

```warble
match (value) {
  is ("yes")
  is ("y") as affirmative {
    print(affirmative, "means yes");
  }
}
```

**Differences Between `is` and `has`**

The behavior of `is` and `has` depends on the implementation provided by the matched object's type:

* **`is`** typically performs equality or type tag checks. For instance:

  * For primitive types (integers, strings), `is` checks equality.
  * For variants, `is` checks the type tag rather than the contained value itself.
* **`has`** primarily checks structural properties or methods, used extensively with object literals to verify the presence of certain fields or functions.

The exact semantics of `has` and its common use cases are explored later in the section covering object-oriented patterns.

**Exhaustiveness and the `default` Case**

Currently, Warble requires `match` statements to be **exhaustive**, meaning every possible condition should be explicitly handled. If a `match` is not exhaustive, a `default` case is required:

```warble
match (variant) {
  is (int) { print("Integer"); }
  default  { print("Something else"); }
}
```

This rule promotes safety and helps prevent errors by explicitly handling every possible case. However, this exhaustiveness requirement may be adjusted in the future based on practical feedback.

**Shorthand Global Binding with `as`**

Warble provides a shorthand syntax that places `as` immediately after the `match` condition to bind a single identifier usable across all subsequent `is` and `has` cases:

```warble
match (value) as v {
  is (x) { print("Matched x:", v); }
  is (y) { print("Matched y:", v); }
  default { print("Default case"); } // Cannot use `v` here
}
```

This shorthand is syntactic sugar for explicitly adding `as` in each `is` or `has` case:

```warble
match (value) {
  is (x) as v { print("Matched x:", v); }
  is (y) as v { print("Matched y:", v); }
  default { print("Default case"); }
}
```

However, note carefully:

* The globally-bound identifier (`v` in the above example) **cannot** be used in the `default` case, since no match occurs there and no value is bound. Attempting to use it results in a compile-time error.
* Due to this subtlety, the use of global `as` binding remains under evaluation and may be adjusted or removed in future versions of Warble to enhance clarity and prevent confusion.

### 7.4 Loop Statements

#### 7.4.1 While (`while`)

#### 7.4.2 Do While (`do ... while`)

#### 7.4.3 For (`for`)

### 7.5 Jump Statements (`break`, `continue`, `return`)

#### 7.5.1 Break & Continue (`break`, `continue`)

#### 7.5.2 Return (`return`)

## 8 Objects & Composition

### 8.1 Object Literals & Property Declarations

### 8.2 Spread (`...`) & Shallow Copy

### 8.3 `this` Binding

### 8.4 Custom Property Resolution (operator overrides)

### 8.5 Function-Based “Constructors”

## 9 Lifetime & Memory Model

### 9.1 Ownership & Moves

### 9.2 Branching Rules

### 9.3 Loop Interaction

### 9.4 Destructor Ordering

### 9.5 Allocation Strategies (power-of-2, etc.)

## 10 Modules & Namespaces

### 10.1 Source Files as `module` Objects

### 10.2 Imports & Exports

### 10.3 Dependency Graph & Build Units

## 11 Compile-Time & Metaprogramming

### 11.1 Decorators (`#`)

### 11.2 Conditional Compilation

### 11.3 Symbol Introspection APIs

## 12 Error Handling & Diagnostics

### 12.1 Compile-Time Errors

### 12.2 Runtime Traps

### 12.3 Assertion Facilities

## 13 Concurrency & Parallelism

### 13.1 Tasks & Schedulers

### 13.2 Atomic Operations

### 13.3 Data-Parallel Helpers

## 14 Standard Library (overview)

## 15 Tooling & Ecosystem

### 15.1 Compiler Flags

### 15.2 Package Manager

### 15.3 IDE Integration

## 16 Best Practices & Idioms

## 17 Glossary

## 18 Appendices

### 18.1 Grammar (EBNF)

### 18.2 Reserved Words

### 18.3 Change Log