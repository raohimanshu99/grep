# grep

# File Content Search System

A high-performance command-line file search tool that indexes text files and enables fast multi-keyword searches with ranked results. Built in C using hash tables and inverted index data structures.

## 🚀 Features

- **Fast Content Search**: O(1) average lookup time using hash tables
- **Multi-Keyword Search**: Search for multiple keywords simultaneously
- **Ranked Results**: Files ranked by number of keyword matches
- **Line Number Tracking**: Shows exact lines where matches occur
- **Syntax Highlighting**: Matched keywords highlighted in yellow
- **Modular Architecture**: Clean, maintainable code structure
- **Result Count Summary**: Shows total matches and files found
- **System-Wide Command**: Works from anywhere on your machine
- **Multiple File Types**: Supports .txt, .c, .h, .cpp, .java, .py, .html, .css, .js, .md, .log

## 📋 Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [How It Works](#how-it-works)
- [Architecture](#architecture)
- [Performance](#performance)
- [Examples](#examples)
- [Limitations](#limitations)
- [Future Enhancements](#future-enhancements)
- [Contributing](#contributing)
- [License](#license)

## 🔧 Installation

### Prerequisites

- GCC compiler
- Make utility
- Unix-like operating system (Linux, macOS, WSL)

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/raohimanshu99/grep.git
cd grep
```

2. Compile the project:
```bash
make
```

3. Install as a system-wide command (one time only):
```bash
sudo cp filesearch /usr/local/bin/filesearch
```
After this, `filesearch` works from **anywhere** on your machine — no need to copy the binary into every folder.

### Manual Compilation

If you don't have Make installed:
```bash
gcc -o filesearch main.c hashtable.c filemanager.c wordparser.c search.c
```

## 💻 Usage

1. Run from anywhere by passing the folder path:
```bash
filesearch ~/Desktop/my-project
```

2. The program will automatically index all text files in the given folder.

3. Enter search queries when prompted:

4. Type `quit` to exit.

⚠️ Avoid running on very large directories like your home folder as it may consume too much memory. Always point it to a specific project folder.

## 🔍 How It Works

### 1. Indexing Phase
- Reads all text files from the current directory
- Extracts words (2+ characters) from file contents
- Builds an inverted index using a hash table
- Maps each unique word to files and line numbers

### 2. Hash Table Structure
- **Size**: 10,007 buckets (prime number for better distribution)
- **Hash Function**: djb2 algorithm
  ```
  hash = 5381
  for each character c:
      hash = ((hash << 5) + hash) + c
  return hash % 10007
  ```
- **Collision Resolution**: Separate chaining with linked lists

### 3. Search Algorithm
- Parses user query into keywords
- Looks up each keyword in hash table (O(1) average)
- Counts matches per file
- Ranks files by match count (descending)
- Displays results with line numbers and highlighting

## 🏗️ Architecture

### Project Structure
```
file-search-system/
├── main.c              # Entry point and user interface
├── hashtable.c/.h      # Hash table implementation
├── filemanager.c/.h    # File reading and indexing
├── wordparser.c/.h     # Text processing utilities
├── search.c/.h         # Search and ranking logic
├── Makefile            # Build configuration
└── README.md           # Documentation
```

### Module Responsibilities

| Module | Purpose |
|--------|---------|
| **hashtable** | Core data structure, hash function, CRUD operations |
| **filemanager** | File I/O, content reading, indexing coordination |
| **wordparser** | Word extraction, text normalization, highlighting |
| **search** | Query parsing, result ranking, output formatting |
| **main** | Program flow, user interaction, cleanup |

### Data Structures

```
HashTable
└── HashEntry (word → files)
    └── FileNode (file index → lines)
        └── LineNode (line numbers)
```

## ⚡ Performance

### Time Complexity
- **Indexing**: O(n × m) where n = number of files, m = average file size
- **Search**: O(k) where k = number of keywords
- **Ranking**: O(f) where f = matching files

### Space Complexity
- O(w × f) where w = unique words, f = files containing them

### Benchmarks
- Indexes 100 files (~10MB total) in < 2 seconds
- Search query response in < 0.1 seconds
- Memory usage: ~50MB for typical codebases

## 📝 Examples

### Example 1: Single Keyword
```
Enter search query: malloc

Search results for: "malloc"
=====================================

[1/1 matches] memory.c
  Line 15: ptr = malloc(sizeof(int) * 10);
  Line 23: data = malloc(size);
  Line 45: buffer = malloc(BUFFER_SIZE);
```

### Example 2: Multiple Keywords
```
Enter search query: hash table collision

Search results for: "hash table collision"
=====================================

[3/3 matches] hashtable.c
  Line 12: // Hash table with collision resolution
  Line 67: unsigned int hash(const char* str) {
  Line 89: // Handle collision using chaining

[2/3 matches] main.c
  Line 5: #include "hashtable.h"
  Line 34: HashTable* ht = createHashTable();

[1/3 matches] README.md
  Line 102: Hash tables provide O(1) lookup time
```

### Example 3: No Results
```
Enter search query: nonexistent

Search results for: "nonexistent"
=====================================
No files found matching the query.
```

## ⚠️ Limitations

- **Fixed Hash Table Size**: Cannot dynamically resize (10,007 buckets)
- **Single Directory**: Only indexes files in current directory
- **No Persistence**: Must re-index on every run
- **No Fuzzy Search**: Exact word matching only
- **No Phrase Search**: Cannot search for "exact phrases"
- **Memory Intensive**: Large files consume significant RAM
- **No Stemming**: "running" won't match "run"
- **ASCII Only**: Limited Unicode support

## 🚧 Future Enhancements

### Planned Features
1. **Huffman Encoding**: Compress indexed data (30-50% memory reduction)
2. **Persistent Storage**: Save index to disk for faster startup
3. **Dynamic Resizing**: Auto-resize hash table when load factor > 0.75
4. **TF-IDF Ranking**: Better relevance scoring
5. **Boolean Operators**: Support AND, OR, NOT queries
6. **Regex Support**: Pattern matching in searches
7. **Parallel Indexing**: Multi-threading for large directories
8. **Web Interface**: Browser-based GUI
9. **Fuzzy Matching**: Handle typos and similar words
10. **Recursive Directory Search**: Index subdirectories

### Potential Algorithms
- **Stemming**: Porter Stemmer algorithm
- **Spell Correction**: Levenshtein distance
- **Compression**: Huffman coding, LZ77
- **Ranking**: BM25 algorithm

## 🤝 Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Commit changes: `git commit -am 'Add new feature'`
4. Push to branch: `git push origin feature-name`
5. Submit a pull request

### Code Style
- Follow K&R C coding style
- Use meaningful variable names
- Comment complex algorithms
- Keep functions under 50 lines
- Run `make` before committing

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👥 Authors

- Himanshu Yadav - Initial work - (https://github.com/raohimanshu99)

## 🙏 Acknowledgments

- djb2 hash function by Daniel J. Bernstein
- Inverted index concept inspired by search engine architectures
- ANSI color codes for terminal highlighting

## 📧 Contact

For questions or suggestions, please open an issue or contact (himanshu8736289@gmail.com)

---

⭐ If you find this project useful, please consider giving it a star on GitHub!
