import code.encoder;
import code.frame;
// import code.state;
import code.address;
import utility.os;
import utility.print;
import utility.to_string;
import node.declaration;
import node.scope.context;
import <stdexcept>;
import <vector>;
import <bit>;

namespace code {
  Encoder::Encoder() : endian{std::endian::native}, instructions{endian}, data{endian} {
    // stack.emplace_back(global);
    // Open(global);
  }

  Encoder::Encoder(std::endian endian) : endian{endian}, instructions{endian}, data{endian} {
    // stack.emplace_back(global);
    // Open(global);
  }

  size_t Encoder::RIP() const { return instructions.Size(); }
  size_t Encoder::RSP() const { return data.Size(); }

  // size_t Encoder::Size() const { return global.Index(); }
  // const std::vector<std::byte>& Encoder::Bytes() const { return global.Bytes(); }
  // const std::vector<code::Address>& Encoder::Addresses() const { return addresses; }
  // const std::vector<node::Declaration*>& Encoder::ActiveDeclarations() const { return active_declarations; }

  // void Encoder::PushLiveDeclaration(node::Declaration& declaration) {
  //   active_declarations.push_back(&declaration);
  // }

  // void Encoder::PopLiveDeclaration(node::Declaration& declaration) {
  //   active_declarations.pop_back();
  // }

  // std::vector<std::byte> Encoder::Compile() {
  //   std::vector<code::Frame*> sorted_frames{};

  //   uint64_t position = 0;
  //   for (auto* frame : frames) {
  //     frame->Position(position);
  //     position += frame->Size();
  //   }

  //   Patch(); // Allow the derived encoder to patch the addresses

  //   std::vector<std::byte> bytes{};
  //   for (auto* frame : frames) {
  //     bytes.append_range(frame->Bytes());
  //   }

  //   return bytes;
  // }

  // void Encoder::Save(const code::Address& address) {
  //   addresses.push_back(address);
  // }

  void Encoder::Print() {
    utility::Print(instructions.Bytes());

    // for (auto byte : code) {
    //   // utility::Write(byte);
    //   std::cout << utility::ToString(byte) << " ";
    //   // std::cout << std::hex << std::setw(2) << std::setfill('0') << std::to_integer<int>(byte) << " ";
    // }

    // std::cout << std::endl;

    // std::cout << std::dec << std::endl; // Reset back to decimal format
  }

  void Encoder::Execute() {
    // std::vector<code::Frame*> sorted_frames{frames.begin(), frames.end()};

    // // TODO: Decide how sorting should actually work. What defines the order? What goes first?
    // // For now, assuming no specific order is needed, but you can sort `sorted_frames` if necessary.
    // std::sort(sorted_frames.begin(), sorted_frames.end(), [](auto* a, auto* b){
    //   return *a > *b;
    // });

    // // Set the absolute position for each frame
    // uint64_t position = 0;
    // for (auto* frame : sorted_frames) {
    //   frame->Position(position);
    //   frame->Align();
    //   position += frame->Size();
    // }

    // // Allow the derived encoder to patch the addresses now that the absolute positions are known
    // Patch();

    // // Allocate a block of executable memory large enough to hold all the frames
    // auto bytes = utility::OS::Allocate(position);
    // for (auto* frame : sorted_frames) {
    //   // Clone each frame's bytes into the executable buffer
    //   bytes.append_range(frame->Bytes());
    // }

    // // Execute the machine code
    // utility::OS::Execute(bytes);
  }

  // code::Frame& Encoder::Frame(node::Scope& scope) {
  //   auto iter = frames.find(&scope);

  //   if (iter != frames.end()) {
  //     return iter->second;
  //   } else {
  //     // Get the parent frame, if it exists
  //     code::Frame* parent = scope.Parent() ? &this->Frame(*scope.Parent()) : nullptr;

  //     auto pair = frames.emplace(&scope, code::Frame{scope});
  //     return pair.first->second;
  //   }
  // }

  code::Frame& Encoder::Top() { return *stack.back(); }

  void Encoder::Open(code::Frame& frame) {
    stack.push_back(&frame);
  }

  void Encoder::Close(code::Frame& frame) {
    if (stack.back() != &frame) {
      throw std::runtime_error("Encoder frame stack is out of alignment");
    }

    stack.pop_back();
  }
};