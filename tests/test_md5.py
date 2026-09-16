import unittest
import pickle
import hashlib
import random
from resumablehash import md5


class TestMD5(unittest.TestCase):
    def test_basic_hash(self):
        h = md5()
        h.update(b"hello world")
        digest = h.digest()
        self.assertEqual(len(digest), 16)
        h2 = hashlib.md5()
        h2.update(b"hello world")
        assert h2.digest() == h.digest()

    def test_string_initialization(self):
        h1 = md5(b"another test")
        h2 = md5()
        h2.update(b"another test")
        self.assertEqual(h1.digest(), h2.digest())
        self.assertEqual(h1.hexdigest(), h2.hexdigest())

    def test_hexdigest(self):
        h = md5()
        h.update(b"hello world")
        hex_digest = h.hexdigest()
        self.assertEqual(len(hex_digest), 32)

    def test_copy(self):
        h1 = md5()
        h1.update(b"hello")
        h2 = h1.copy()
        self.assertEqual(h1.hexdigest(), h2.hexdigest())
        assert h1.__getstate__() == h2.__getstate__()

        # Updating them differently should lead to different digests.
        h1.update(b" world")
        h2.update(b" there")
        self.assertNotEqual(h1.hexdigest(), h2.hexdigest())

    def test_pickling(self):
        h = md5()
        h.update(b"hello world")
        state = pickle.dumps(h)
        h2 = pickle.loads(state)
        self.assertEqual(h.hexdigest(), h2.hexdigest())

    def test_properties(self):
        h = md5()
        self.assertEqual(h.digest_size, 16)
        self.assertEqual(h.block_size, 64)
        self.assertEqual(h.name, "md5")

    def test_string_construction(self):
        with self.assertRaises(TypeError):
            md5("this is not binary")

    def test_binary(self):
        h1 = md5()
        h2 = hashlib.md5()
        random.seed(234230)
        for i in range(200):
            chunk = (random.randint(0, 255).to_bytes(length=1, byteorder="big")
                * random.randint(1, 30))
            h1.update(chunk)
            h2.update(chunk)
            self.assertEqual(h1.digest(), h2.digest())
            self.assertEqual(h1.hexdigest(), h2.hexdigest())
            h1 = pickle.loads(pickle.dumps(h1))


    def test_buffer_protocol(self):
        payload = b"the quick brown fox" * 1000
        expected = hashlib.md5(payload).hexdigest()
        for buffer in [payload,
                       bytearray(payload),
                       memoryview(bytearray(payload)),
                       memoryview(bytearray(payload)).toreadonly()]:
            with self.subTest(type=type(buffer).__name__):
                h = md5()
                h.update(buffer)
                self.assertEqual(expected, h.hexdigest())

    def test_buffer_slice(self):
        payload = bytearray(b"the quick brown fox" * 1000)
        view = memoryview(payload)
        h = md5()
        for offset in range(0, len(view), 1024):
            h.update(view[offset:offset + 1024])
        self.assertEqual(hashlib.md5(bytes(payload)).hexdigest(), h.hexdigest())

    def test_str_is_rejected(self):
        h = md5()
        with self.assertRaises(TypeError):
            h.update("strings must be encoded before hashing")


if __name__ == '__main__':
    unittest.main()
