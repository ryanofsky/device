// missing from devil:
//  bit masks 
//    bit masks accomplish two things:
//      runtime constraint checking
//        express that drivers require certain register bits to be 0 or 1
//        and allow generated code to check for this when reading and writing
//        this would be better implemented here in the IO struct
//
//      compile time safety and completeness check
//        mask marks every bit as used or unused
//        checks that every used bit maps to a variable (completeness)
//          this would be impossible to implement anyway under the current
//          framework, because you can't deduce variable types from a 
//          register type. It is redundant because if you forget
//          to declare a variable, the code that tries to use the variable
//          won't compile. 
//        checks that no unused bit is mapped to a variable (safety)
//          for this purpose masks provide a useful redundancy
//         
//  parameterized registers - using the example in the Devil manual
//
//    register reg(i : int(2)) = read base @ 0, pre {index = i} : bit[8];
//    register x_low = reg(0);
//
//    template<int i>
//    struct reg : Register<8, IndexIO< PortIO<0>, index, i > > {};
//
//    typedef reg<0> x_low;
//      instead of
//        typedef MyRegister1<32, PortIO<0>
//
//  Map Type Equation / Algebraic types:
//    algebraic types are essentially enumerations, instead of
//    just being defined by a bit width, they are described as 
//    lists of possible values. The simplest uses of algebraic t
//    types are just like C++ enum's. One feature they have
//    that enum's lack is that possible values may be described
//    wildcards
//
//  variable features missing:
//    volatile option
//      causes every device.get<var>()
//      to be implicity preceded by a device.read<var>()
//
//    trigger option
//      causes every device.set<var>()
//      to be implicity followed by a device.write<var>()
//
// structure serialized as
//   the default Structure reads and writes from registers
//   in the order first encountered in the variable list
//   This should almost always be sufficient. When it is not
//   simply define a custom structure classed with its
//   own read() and write() methods. The 
