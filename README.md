# Warble Programming Language Overview

Warble is a cutting-edge, high-performance programming language designed to bring unparalleled efficiency, safety, and ease of use to developers. Its core philosophy revolves around strong typing, uniformity, and an emphasis on concurrency by default. This document outlines the defining features and motivations behind Warble's development.

## Key Features

### Strong Typing with Minimal Overhead

Warble is strongly typed, ensuring code safety and predictability. However, the compiler significantly reduces the tedium often associated with strong typing by automatically deducing types and generating templates. This feature allows developers to write robust and expressive code without being bogged down by repetitive type declarations.

### Immutability by Default

In Warble, immutability is the default behavior. This promotes safer code by minimizing unintended side effects, especially in concurrent programming. Developers can explicitly opt into mutability when necessary, maintaining flexibility while prioritizing safety.

### Memory Management: Stack Over Everything

Warble prioritizes stack allocation for most data, providing maximum performance and predictable memory usage. When heap allocation is necessary, Warble employs a **lock-free, power-of-two heap allocation strategy** that ensures high performance and low contention. This system divides memory into fixed-size blocks based on powers of two, enabling rapid allocations and deallocations without locking mechanisms. Combined with the built-in smart pointer system, this design eliminates memory leaks and dangling references while preserving runtime efficiency. By avoiding garbage collection, Warble delivers predictable and efficient memory management without sacrificing safety.

### Concurrency by Default

Warble takes a groundbreaking approach to concurrency. Every program is fully multithreaded by default, utilizing the CPU's full capabilities without requiring manual thread management. This is achieved through:

- **Compile-Time Constant DAG:** Modules define a Directed Acyclic Graph (DAG) at compile time, enabling efficient task scheduling.
- **Async/Await System:** Flexible task scheduling using promises allows seamless integration of asynchronous operations.
- **High-Performance Timers:** Functions can be scheduled for execution after specified intervals with negligible overhead.

Warble's decentralized and lock-free concurrency model ensures minimal contention between threads, achieving exceptional performance and scalability. This approach solves the persistent problem of underutilized CPU resources in modern applications.

### Lightweight and High-Performance Compilation

Warble’s compiler is exceptionally lean, with a size under 4 MB, and is designed for speed. It supports modular compilation, allowing files to be compiled in parallel as soon as they arrive. Programs with tens of thousands of lines can be compiled in less than a second in fast execution mode, enabling rapid iteration and debugging. Anyone that has developed in C++ will appreciate Warble's fast compilation times.

Moreover, the compiler is designed to be embedded, allowing dynamic compilation. This enables Warble to function either as a fully optimized pre-compiled language like C/C++/Rust or as a JIT-compiled scripting language more like JavaScript/Java/C#, depending on the use case.

### Uniformity and Internal Consistency

Warble's syntax is meticulously designed to avoid arbitrary rules and inconsistencies. The language's syntax and behavior follow a cohesive logic, making it intuitive for developers. For instance:

- Implicit calls unify function calls, indexing, and templating operators into a single consistent system.
- Symbols serve as runtime objects, allowing seamless introspection and debugging.
- Operators are fully customizable, enabling developers to tailor the language to their needs.

This uniformity makes Warble easier to learn and reason about, reducing the cognitive load on developers.

### Security and Safety

Warble excels in both security and safety:

- **Security:** Warble's sandboxed environment ensures that untrusted code cannot access unauthorized resources. Modules can only interact through explicitly allowed imports/exports, and packages have controlled access to system-level functionality.
- **Safety:** The smart pointer system prevents memory leaks, and the ownership model eliminates dangling references at compile time. Additionally, comprehensive debugging tools trace issues like memory consumption and CPU usage during development.

### Advanced Features

#### Variants for Flexible Type Safety

Variants (type-safe unions) are a core feature in Warble. They provide polymorphism with better performance and safety compared to traditional object-oriented designs. Variants are easy to create, unpack, and use, enabling developers to handle complex type scenarios elegantly.

#### Pattern Matching and Metaprogramming

Warble offers powerful pattern matching, enabling expressive function overloading and fine-grained control over code behavior. Its metaprogramming capabilities, facilitated by runtime symbols, allow for dynamic inspection and flexible runtime type information (RTTI).

## Conclusion

Warble is a reimagining of what a programming language can be. By combining strong typing, concurrency by default, security, safety, and a focus on performance, it delivers a uniquely powerful tool for developers. Its lightweight compiler and flexible design make it suitable for a wide range of applications, from high-performance systems to scalable, concurrent architectures. Whether you're building advanced software or exploring cutting-edge programming paradigms, Warble offers an unmatched blend of power and simplicity.

