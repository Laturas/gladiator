# Motivation
We need to go from a series of reverse polish notation IR tokens to valid assembly output with as close to optimal register usage as possible.

# Constraints
Pushing items to the stack is slow and stupid - Avoid at all costs
Mov calls are slightly less slow but we still want to avoid them where possible as they're usually unnecessary.

Any outputted assembly NEEDS to give an equivalent output to hte inputted stream of reverse polish notation symbols.

# Known things we can take advantage of
Gladiator only has binary and unary operations. All actions in the language can be thought of as a series of such operations.

~0 + 1 + 2 is equivalent to ((~0) + 1) + 1. We're only ever acting on one or two objects at any given time in this.
function_call(1, 2, 3) Can be thought of as (push(1) push(2) push(3) function_call) which is technically a stream of unary operations.

# Constructing an algorithm
While parsing the stream:
- Upon seeing an item, add it to a compiler stack
- Upon seeing an operation, one of the following cases will happen:
    - It's a unary or binary operation on something in a register, in which case write out the instruction to the assembly. In the case it's a binary operation, drop the second item.
    - It's a unary operation on something not in a register. Move the item into the next available register and apply the operation.

# Edge cases ruin everything
What if we run into something like this:
return ~0 + ~1 + !2 + ~0 + !0;
With a corresponding RVPN:

0
Complement
1
Complement
2
Not
0
Complement
0
Not
Add
Add
Add
Add

We would quickly run out of registers before reaching the first binary operator to free up a register! We've only got a couple to work with.
What if we could defer those unary operators until we actually need what they're applied to?

So that's what we'll do! We'll just create two stacks, 