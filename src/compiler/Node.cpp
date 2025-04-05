import node;
import lexical.token;

Node::Node() noexcept : span{} {}
Node::Node(const lexical::source::Span& s) noexcept : span{s} {}
Node::~Node() = default;

std::string_view Node::View() const { return span.View(); }
const lexical::source::Span& Node::Span() const { return span; }
lexical::source::Span::TokenIterator Node::Begin() const { return span.Begin(); }
lexical::source::Span::TokenIterator Node::End() const { return span.End(); }

void Node::Span(const lexical::source::Span& s) { span = s; }
void Node::Begin(lexical::source::Span::TokenIterator iter) { span.Begin(iter); }
void Node::End(lexical::source::Span::TokenIterator iter) { span.End(iter); }