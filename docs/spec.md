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

### 2.1 Character Set & Encoding

### 2.2 Tokens

#### 2.2.1 Identifiers

#### 2.2.2 Keywords

#### 2.2.3 Literals (overview—full details in §4)

### 2.3 Comments & Whitespace

### 2.4 Line Terminators & Semicolons

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

### 3.3 Semicolons vs. Empty Literals

### 3.4 **The Symbol Model**

#### 3.4.1 Symbol Anatomy (`name`, `type`, `flags`, hierarchy)

#### 3.4.2 Creation Rules

#### 3.4.3 Symbol Access (`$`, `$.` shorthand)

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

#### 4.2.2 Module

#### 4.2.3 Block

#### 4.2.4 Symbol

### 4.3 Variant Mechanics

#### 4.3.1 Creation (`||`, `&&`, `return case`, manual)

#### 4.3.2 Flattening & Tag Layout

#### 4.3.3 Narrowing & Unpacking (`?.`, `??`)

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

## 7 Control Flow

### 7.1 `do`, `if`, `else`, `match`

### 7.2 Loops (`for`, `while`, `do … while`)

### 7.3 Early Exit (`break`, `continue`, `return`)

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