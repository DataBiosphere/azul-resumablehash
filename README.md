# resumablesha256
A Python sha256 hasher whose state can be saved and loaded, using B-Con's sha256 C implementation for speed.

## Motivation
When computing an sha256 of data as it is streamed, for example to a web server, there may be times when the next chunk of data will be received in a different request. When you don't want to or cannot rehash the data that has already been received, you can save out the current state of the hasher after digesting the last chunk, then load the state before handling the request with the newest chunk.

## Usage
For reference, here is how to create an sha256 hash using Python's standard libraries:
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

To make the hasher resumable, replace `hashlib` with `resumablesha256` and use pickle to save and load its state:
```python
>>> import pickle
>>> import resumablesha256
>>> hasher = resumablesha256.sha256()
>>> hasher.update(b"first chunk of data")
>>> pickled_hasher = pickle.dumps(hasher)
>>> new_hasher = pickle.loads(pickled_hasher)
>>> new_hasher.update(b"a later chunk")
>>> new_hasher.digest()
b'\xffs\xd95>\xa0\xf6Y\xcd\\\r\xb9\x0e"\x9c|\x03<\x84\xd8\x04e\x8f-\xd4\x0eo<\xc9\t f'
>>> new_hasher.hexdigest()
'ff73d9353ea0f659cd5c0db90e229c7c033c84d804658f2dd40e6f3cc9092066'
```

## Installation
```pip install resumablesha256```

## Speed
Because the sha256 implementation is written in C, it is orders of magnitude
faster than an equivalent Python implementation. However, note that it is
2.4x slower than hashlib.sha256 when testing against large data in large
chunks.
