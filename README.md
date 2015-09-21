# LainDB
LainDB is a very simple key-value store engine. LainDB aims to be fast and flexible at the same time,
so it uses modern c++ features, including nullptr and template. Therefore, LainDB needs c++11
support. However, Laindb avoids any platform-specific API and can run on any platform.

As a key-value store, you are able to store any kind of data (you may need to provide a serializer),
inside a key (a c-style string with a length limitation). This data can later be retrieved only if we
know the exact key used to store it. LainDB provides a minimal interface and it is easy to use but
also hard to write unsafe codes.

LainDB’s index is implemented by a top-down B+tree. Cache are handled using LRU(Least Recently
Used) algorithm.

Documentation is under /doc.
