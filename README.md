Note that this fork is intended for internal use by UCSC Azul team. The license
is the same as for the upstream. Upstream copied the C implementation of SHA-256
from https://github.com/B-Con/crypto-algorithms, wheras we replaced that with a
Git submodule. We then added support for MD5 by blatantly copying and then
modifying the SHA-256 parts. We renamed the repository, and we don't currently
publish on PyPI, nor do we intend to, to avoid confusion with upstream. Again,
this is for our internal use only. Below is the original upstream README.

# resumablehash
A Python sha256 hasher whose state can be saved and loaded by pickling. It is
otherwise a  drop-in replacement for hashlib.sha256(). It uses B-Con's sha256 C
implementation for speed.

## Motivation
When working with large files or streaming data, you might not always have a
single, continuous session to compute a hash. For instance, in a web server
handling file uploads, data can arrive in multiple HTTP requests as the file
grows on the client side. With resumablehash, you can:

- Process incoming data in manageable chunks.
- Save the current state of the hash computation.
- Resume the hash calculation later, even across different sessions or processes.

## Installation
```bash
pip install resumablehash
```

## Usage
For reference, here is how to create an sha256 hash using Python's standard
library:
```python
>>> import hashlib
>>> hasher = hashlib.sha256()
>>> hasher.update(b"first chunk of data")
>>> hasher.update(b"a later chunk")
>>> hasher.digest()
b'\xffs\xd95>\xa0\xf6Y\xcd\\\r\xb9\x0e"\x9c|\x03<\x84\xd8\x04e\x8f-\xd4\x0eo<\xc9\t f'
>>> hasher.hexdigest()
'ff73d9353ea0f659cd5c0db90e229c7c033c84d804658f2dd40e6f3cc9092066'
```

To make the hasher resumable, replace `hashlib` with `resumablehash` and use pickle to save and load its state:
```python
>>> import pickle
>>> import resumablehash
>>> hasher = resumablehash.sha256()
>>> hasher.update(b"first chunk of data")
>>> pickled_hasher = pickle.dumps(hasher)
>>> new_hasher = pickle.loads(pickled_hasher)
>>> new_hasher.update(b"a later chunk")
>>> new_hasher.digest()
b'\xffs\xd95>\xa0\xf6Y\xcd\\\r\xb9\x0e"\x9c|\x03<\x84\xd8\x04e\x8f-\xd4\x0eo<\xc9\t f'
>>> new_hasher.hexdigest()
'ff73d9353ea0f659cd5c0db90e229c7c033c84d804658f2dd40e6f3cc9092066'
```

## Performance
Because the sha256 implementation is written in C, it is significantly faster
than an equivalent pure Python implementation. However, when processing large
data chunks, it is approximately 2.5x slower than `hashlib.sha256`.

## Acknowledgements
Thank you to Brad Conte for his public domain implementation of sha256
at https://github.com/B-Con/crypto-algorithms/tree/master.
