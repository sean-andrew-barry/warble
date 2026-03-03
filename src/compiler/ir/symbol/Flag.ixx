export module compiler.ir.symbol.Flag;

import <cstdint>;

namespace compiler::ir::symbol {
  export enum class Flag : uint8_t {
    None,
    Size8,
    Size16,
    Size32,
    Size64,
    Size128,
    Size256,
    Size512,
    Extended,
    Spread,
    Repeat, // Marks an array as being created via the repeat syntax, such as `[0; 10]`.
    Import,
    Export, // Modifier: symbol is exported from its module
    Private, // Modifier: symbol is privately accessible
    Protected, // Modifier: symbol is protected
    Public, // Modifier: symbol is public
    Static, // Modifier: symbol is static
    Mut, // Modifier: symbol is mutable
    Const, // Modifier: symbol is constant
    Let, // Modifier: symbol is neutral
    Symbolic, // Symbol only has a symbol address, no value address
    Async, // Modifier: symbol is asynchronous
    Volatile, // Modifier: symbol is volatile
    Generator, // Marks a function as a generator, so it returns an iterable.
    Varied, // Marks a function as being able to return different types, so it returns a `Variant`.
    Returned, // Symbol is returned from a function
    Captured, // Symbol is captured by a closure
    Parameter, // Symbol is a function parameter
    ParameterPack, // Symbol is a parameter pack. Can only appear as the last parameter of a function.
    Impure, // Indicates that this can cause side effects or contains something that can.
    Used, // Symbol is used (prevents "unused variable" warnings)
    Inline, // Symbol is inlined at its use sites
    Immediate, // Symbol's value carries the actual immediate value, not a slice
    Internal, // Has been statically proven to never escape its stack frame, enabling many optimizations.
    ThreadSafe,
    Accumulator, // An accumulator, such as the index in a loop. Useful for register allocation.
    Comtime, // Fully defined at compile-time.
    Runtime, // Unknown at compile-time.
    NonZero, // Specifies that a value will never hold `0` at runtime. This is an optimization hint, allowing things like optionals to avoid having a type tag.
    NonMax, // Specifies that a value will never hold its maximum value, all `1`s, at runtime. This is an optimization hint, allowing things like optionals to avoid having a type tag.
    Halts, // Specifies a function is proven to halt. In other words, it will never loop infinitely or recurse infinitely.
    Topic, // Indicates that this symbol is a topic, and so accessible via `this` or `that`
  };
}