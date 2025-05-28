# coding=utf-8
# MD5 Implementation without using hashlib
import struct

# Constants for MD5
S = [7, 12, 17, 22, 5, 9, 14, 20, 4, 11, 16, 23, 6, 10, 15, 21]
K = [
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0x2d6f2b94, 0x4a7484aa, 0x5cb0a9dc, 0x76e4d22f, 0x2a4b3b26, 0x15c7d61a, 0x07256e10, 0x1b78678f
]

# Initial MD5 hash values
A0 = 0x67452301
B0 = 0xEFCDAB89
C0 = 0x98BADCFE
D0 = 0x10325476


def F(x, y, z):
	return (x & y) | (~x & z)


def G(x, y, z):
	return (x & z) | (y & ~z)


def H(x, y, z):
	return x ^ y ^ z


def I(x, y, z):
	return y ^ (x | ~z)


def left_rotate(x, n):
	return ((x << n) | (x >> (32 - n))) & 0xFFFFFFFF


def md5_transform(A, B, C, D, block):
	A = A & 0xFFFFFFFF
	B = B & 0xFFFFFFFF
	C = C & 0xFFFFFFFF
	D = D & 0xFFFFFFFF

	# Break the block into 16 32-bit words
	X = [struct.unpack('<I', block[i:i + 4])[0] for i in range(0, 64, 4)]

	# Round 1
	a, b, c, d = A, B, C, D
	for i in range(16):
		if i < 16:
			f = F(b, c, d)
			g = i
		elif i < 32:
			f = G(b, c, d)
			g = (5 * i + 1) % 16
		elif i < 48:
			f = H(b, c, d)
			g = (3 * i + 5) % 16
		else:
			f = I(b, c, d)
			g = (7 * i) % 16

		temp = d
		d = c
		c = b
		b = b + left_rotate(a + f + K[i] + X[g], S[i % 4])
		a = temp

	# Add this chunk's hash to the result
	A = (A + a) & 0xFFFFFFFF
	B = (B + b) & 0xFFFFFFFF
	C = (C + c) & 0xFFFFFFFF
	D = (D + d) & 0xFFFFFFFF

	return A, B, C, D


def md5_padding(message):
	length = len(message) * 8
	message += b'\x80'
	while len(message) % 64 != 56:
		message += b'\x00'

	# Append the length of the original message in bits
	message += struct.pack('<Q', length)

	return message


def md5(message):
	# Step 1: Padding the message
	message = md5_padding(message)

	# Step 2: Initialize MD5 buffer
	A, B, C, D = A0, B0, C0, D0

	# Step 3: Process each 512-bit chunk
	for i in range(0, len(message), 64):
		block = message[i:i + 64]
		A, B, C, D = md5_transform(A, B, C, D, block)

	# Step 4: Return the final hash as a hexadecimal string
	return ''.join([format(x, '08x') for x in [A, B, C, D]])


if __name__ == "__main__":
	print("MD5 Hash Calculator")

	input_string = input("Enter the string to hash: ").strip()
	result = md5(input_string.encode('utf-8'))
	print(f"MD5 Hash of string: {result}")
